#include "Gameplay/ACT/Authoring/PadmaACTAuthoring.h"

#include "Animation/AnimBlueprintGeneratedClass.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/Skeleton.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"

namespace
{
struct FAuthoringValidator
{
	FPadmaACTAuthoringReport Report;
	TMap<FName, const UObject*> Definitions;
	const FPadmaACTAuthoringReferences& References;

	explicit FAuthoringValidator(const FPadmaACTAuthoringReferences& InReferences)
		: References(InReferences) {}

	void Error(const FString& Path, const FString& Message)
	{
		Report.Errors.Add(FText::FromString(Path + TEXT(": ") + Message));
	}

	void Identity(const UObject* Object, FName Id, int32 Version, const FText& Name)
	{
		const FString Path = Object->GetPathName();
		if (Id.IsNone()) { Error(Path, TEXT("DefinitionId is required.")); }
		else if (const UObject* const* Existing = Definitions.Find(Id))
		{
			if (*Existing != Object) { Error(Path, TEXT("Duplicate DefinitionId: ") + Id.ToString()); }
		}
		else { Definitions.Add(Id, Object); }
		if (Version < 1) { Error(Path, TEXT("ContentVersion must be a positive schema version.")); }
		if (Name.IsEmpty()) { Error(Path, TEXT("DisplayName is required.")); }
	}

	void Character(const UPadmaACTCharacterDefinition* Character)
	{
		const FString Path = Character->GetPathName();
		Identity(Character, Character->DefinitionId, Character->ContentVersion, Character->DisplayName);
		USkeletalMesh* Mesh = Character->Model.LoadSynchronous();
		if (!Mesh || !Mesh->GetSkeleton()) { Error(Path, TEXT("Model must resolve to a skeletal mesh with a skeleton.")); }
		if (!Character->AnimationClass.IsNull())
		{
			UClass* AnimClass = Character->AnimationClass.LoadSynchronous();
			if (!AnimClass || !AnimClass->IsChildOf(UAnimInstance::StaticClass()))
			{
				Error(Path, TEXT("AnimationClass must resolve to an AnimInstance class."));
			}
			else if (const UAnimBlueprintGeneratedClass* AnimBlueprint = Cast<UAnimBlueprintGeneratedClass>(AnimClass))
			{
				// Strict identity until an explicit retargeting adapter is supplied.
				if (!Mesh || Mesh->GetSkeleton() != AnimBlueprint->GetTargetSkeleton())
				{
					Error(Path, TEXT("AnimationClass and Model must use the same skeleton; retarget explicitly."));
				}
			}
		}

		TSet<FName> Traits;
		for (FName Trait : Character->TraitIds)
		{
			if (Trait.IsNone() || Traits.Contains(Trait)) { Error(Path, TEXT("TraitIds must be nonempty and unique.")); }
			Traits.Add(Trait);
		}
		TSet<FName> RestrictedTraits;
		for (const FPadmaACTTraitTerrainRestriction& Restriction : Character->TerrainRestrictions)
		{
			if (!Traits.Contains(Restriction.TraitId) || Restriction.TraitId.IsNone())
			{
				Error(Path, TEXT("Restriction references an undeclared TraitId."));
			}
			if (RestrictedTraits.Contains(Restriction.TraitId))
			{
				Error(Path, TEXT("Use one terrain selector per trait."));
			}
			RestrictedTraits.Add(Restriction.TraitId);
			if (Restriction.DisallowedTerrainIds.IsEmpty()) { Error(Path, TEXT("Trait restriction requires explicit terrain IDs.")); }
			TSet<FName> Terrains;
			for (FName Terrain : Restriction.DisallowedTerrainIds)
			{
				if (Terrain.IsNone() || !References.TerrainIds.Contains(Terrain) || Terrains.Contains(Terrain))
				{
					Error(Path, TEXT("Unknown, empty or duplicate terrain selector: ") + Terrain.ToString());
				}
				Terrains.Add(Terrain);
			}
		}

		UDataTable* Table = Character->SkillTable.LoadSynchronous();
		if (!Table || Table->GetRowStruct() != FPadmaACTSkillRow::StaticStruct())
		{
			Error(Path, TEXT("SkillTable must resolve to a DataTable with PadmaACTSkillRow rows."));
			return;
		}
		if (Table->GetRowMap().IsEmpty()) { Error(Path, TEXT("SkillTable is an empty draft; author ACT bindings first.")); }
		TSet<FName> SkillIds;
		TSet<FName> Bindings;
		for (const auto& Pair : Table->GetRowMap())
		{
			const FPadmaACTSkillRow& Row = *reinterpret_cast<const FPadmaACTSkillRow*>(Pair.Value);
			const FString RowPath = Path + TEXT(" / ") + Pair.Key.ToString();
			if (Row.SkillId.IsNone() || SkillIds.Contains(Row.SkillId)) { Error(RowPath, TEXT("SkillId must be nonempty and unique in the character table.")); }
			SkillIds.Add(Row.SkillId);
			if (Row.ActivationBindingId.IsNone() || Bindings.Contains(Row.ActivationBindingId)
				|| !References.ActivationBindingIds.Contains(Row.ActivationBindingId))
			{
				Error(RowPath, TEXT("ActivationBindingId must be declared and unique in the character table."));
			}
			Bindings.Add(Row.ActivationBindingId);
			const UPadmaACTSkillDefinition* Skill = Row.Definition.LoadSynchronous();
			if (!Skill) { Error(RowPath, TEXT("ACT skill Definition is missing or cannot load.")); continue; }
			Identity(Skill, Skill->DefinitionId, Skill->ContentVersion, Skill->DisplayName);
			if (Skill->DefinitionId != Row.SkillId) { Error(RowPath, TEXT("SkillId does not match the referenced ACT definition.")); }
			if (Skill->AbilityImplementationId.IsNone() || !References.AbilityImplementationIds.Contains(Skill->AbilityImplementationId))
			{
				Error(RowPath, TEXT("AbilityImplementationId is absent from the supplied ACT reference catalog."));
			}
			if (!Skill->Montage.IsNull())
			{
				const UAnimMontage* Montage = Skill->Montage.LoadSynchronous();
				if (!Montage || !Mesh || Mesh->GetSkeleton() != Montage->GetSkeleton())
				{
					Error(RowPath, TEXT("Montage must load and share the character skeleton; retarget explicitly."));
				}
			}
		}
	}

	void Weapon(const UPadmaACTWeaponDefinition* Weapon)
	{
		const FString Path = Weapon->GetPathName();
		Identity(Weapon, Weapon->DefinitionId, Weapon->ContentVersion, Weapon->DisplayName);
		const bool bStatic = !Weapon->StaticModel.IsNull();
		const bool bSkeletal = !Weapon->SkeletalModel.IsNull();
		if (bStatic == bSkeletal) { Error(Path, TEXT("Assign exactly one of StaticModel or SkeletalModel.")); }
		if (bStatic && !Weapon->StaticModel.LoadSynchronous()) { Error(Path, TEXT("StaticModel cannot load.")); }
		if (bSkeletal)
		{
			const USkeletalMesh* Mesh = Weapon->SkeletalModel.LoadSynchronous();
			if (!Mesh || !Mesh->GetSkeleton()) { Error(Path, TEXT("SkeletalModel must load with a skeleton.")); }
		}
		// A socket is optional (e.g. unattached preview). Compatibility belongs to
		// the selected character/weapon pair, not a rule inferred by this catalog.
	}
};
}

FPadmaACTAuthoringReport UPadmaACTAuthoringCatalog::ValidateCatalog() const
{
	FAuthoringValidator Validator(References);
	if (Characters.IsEmpty()) { Validator.Error(GetPathName(), TEXT("Catalog has no ACT characters; it is an incomplete draft.")); }
	TSet<const UObject*> Seen;
	for (const auto& Reference : Characters)
	{
		const UPadmaACTCharacterDefinition* Character = Reference.LoadSynchronous();
		if (!Character) { Validator.Error(GetPathName(), TEXT("Character soft reference is missing or cannot load.")); continue; }
		if (Seen.Contains(Character)) { Validator.Error(GetPathName(), TEXT("Duplicate character reference.")); }
		Seen.Add(Character);
		Validator.Character(Character);
	}
	for (const auto& Reference : Weapons)
	{
		const UPadmaACTWeaponDefinition* Weapon = Reference.LoadSynchronous();
		if (!Weapon) { Validator.Error(GetPathName(), TEXT("Weapon soft reference is missing or cannot load.")); continue; }
		if (Seen.Contains(Weapon)) { Validator.Error(GetPathName(), TEXT("Duplicate weapon reference.")); }
		Seen.Add(Weapon);
		Validator.Weapon(Weapon);
	}
	Validator.Report.bValid = Validator.Report.Errors.IsEmpty();
	return Validator.Report;
}

void UPadmaACTAuthoringCatalog::ValidateAuthoring() const
{
	const FPadmaACTAuthoringReport Report = ValidateCatalog();
	for (const FText& Error : Report.Errors)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ACT Authoring] %s"), *Error.ToString());
	}
	UE_LOG(LogTemp, Display, TEXT("[ACT Authoring] %s: %s. Authoring validation does not certify GAS execution or a playable roster."),
		*GetPathName(), Report.bValid ? TEXT("references valid") : TEXT("incomplete/invalid"));
}
