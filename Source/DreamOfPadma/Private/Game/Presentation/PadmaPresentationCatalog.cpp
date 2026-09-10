#include "Game/Presentation/PadmaPresentationCatalog.h"
#include "Gameplay/ACT/Authoring/PadmaACTAuthoring.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "Animation/AnimInstance.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"

namespace
{
void AddPreviewDiagnostic(TArray<FText>& Errors, const FString& Context, const FString& Message)
{
	Errors.Add(FText::FromString(Context + TEXT(": ") + Message));
}

void Visual(FPadmaResolvedPresentation& Out, const TSoftObjectPtr<UStaticMesh>& Static,
	const TSoftObjectPtr<USkeletalMesh>& Skeletal, const TSoftClassPtr<UAnimInstance>& Animation)
{
	const FString Context = Out.DefinitionId.ToString();
	if (Static.IsNull() == Skeletal.IsNull())
	{
		AddPreviewDiagnostic(Out.VisualErrors, Context, TEXT("Assign exactly one StaticModel or SkeletalModel."));
		return;
	}
	Out.StaticModel = Static.LoadSynchronous();
	Out.SkeletalModel = Skeletal.LoadSynchronous();
	if ((!Static.IsNull() && !Out.StaticModel) || (!Skeletal.IsNull() && !Out.SkeletalModel))
	{
		AddPreviewDiagnostic(Out.VisualErrors, Context, TEXT("Configured model cannot load."));
	}
	if (Out.SkeletalModel && !Out.SkeletalModel->GetSkeleton())
	{
		AddPreviewDiagnostic(Out.VisualErrors, Context, TEXT("Skeletal model has no skeleton."));
	}
	if (!Animation.IsNull())
	{
		UClass* Class = Animation.LoadSynchronous();
		if (!Out.SkeletalModel || !Class || !Class->IsChildOf(UAnimInstance::StaticClass()))
		{
			AddPreviewDiagnostic(Out.VisualErrors, Context, TEXT("AnimationClass requires a skeletal model and an AnimInstance class."));
		}
		else if (const UAnimBlueprintGeneratedClass* BP = Cast<UAnimBlueprintGeneratedClass>(Class);
			BP && BP->GetTargetSkeleton() != Out.SkeletalModel->GetSkeleton())
		{
			AddPreviewDiagnostic(Out.VisualErrors, Context, TEXT("Animation and model skeleton differ; retarget explicitly."));
		}
		else { Out.AnimationClass = Class; }
	}
	Out.bVisualReady = Out.VisualErrors.IsEmpty();
}

void ACTSkills(FPadmaResolvedPresentation& Out, const UPadmaACTCharacterDefinition& Character, FName SelectedRow)
{
	const FString Context = Character.GetPathName() + TEXT(".SkillTable");
	UDataTable* Table = Character.SkillTable.LoadSynchronous();
	if (!Table || Table->GetRowStruct() != FPadmaACTSkillRow::StaticStruct())
	{
		AddPreviewDiagnostic(Out.SkillErrors, Context, TEXT("Expected PadmaACTSkillRow table."));
		return;
	}
	TArray<FName> Names = Table->GetRowNames();
	Names.Sort(FNameLexicalLess());
	if (Names.IsEmpty() || (!SelectedRow.IsNone() && !Names.Contains(SelectedRow)))
	{
		AddPreviewDiagnostic(Out.SkillErrors, Context, TEXT("Empty table or selected row does not exist."));
		return;
	}
	TSet<FName> Skills;
	TSet<FName> Bindings;
	TArray<FPadmaPresentationSkillView> Candidate;
	for (FName Name : Names)
	{
		const FPadmaACTSkillRow* Row = Table->FindRow<FPadmaACTSkillRow>(Name, TEXT("ModelPreview"), false);
		const UPadmaACTSkillDefinition* Skill = Row ? Row->Definition.LoadSynchronous() : nullptr;
		if (!Skill || Row->SkillId.IsNone() || Row->SkillId != Skill->DefinitionId
			|| Row->ActivationBindingId.IsNone() || Skill->DisplayName.IsEmpty()
			|| Skills.Contains(Row->SkillId) || Bindings.Contains(Row->ActivationBindingId))
		{
			AddPreviewDiagnostic(Out.SkillErrors, Context + TEXT(".") + Name.ToString(), TEXT("Missing, mismatched or duplicate skill/binding."));
			continue;
		}
		Skills.Add(Row->SkillId);
		Bindings.Add(Row->ActivationBindingId);
		if (SelectedRow.IsNone() || SelectedRow == Name)
		{
			FPadmaPresentationSkillView& View = Candidate.AddDefaulted_GetRef();
			View.RowName = Name;
			View.SkillId = Skill->DefinitionId;
			View.DisplayName = Skill->DisplayName;
			// ACT's source has no description/numerical schema yet. Leave it empty.
		}
	}
	Out.bSkillDetailsReady = Out.SkillErrors.IsEmpty();
	if (Out.bSkillDetailsReady) { Out.Skills = MoveTemp(Candidate); }
}
}

FPadmaResolvedPresentation UPadmaPresentationCatalog::Resolve(FPadmaPresentationKey Key, FName SelectedRow) const
{
	FPadmaResolvedPresentation Out;
	Out.DefinitionId = Key.DefinitionId;
	const FPadmaPresentationCatalogEntry* Entry = nullptr;
	for (const auto& Item : Entries)
	{
		if (Item.Key == Key)
		{
			if (Entry)
			{
				AddPreviewDiagnostic(Out.VisualErrors, GetPathName(), TEXT("Duplicate domain/definition key."));
				return Out;
			}
			Entry = &Item;
		}
	}
	if (Key.DefinitionId.IsNone() || !Entry)
	{
		AddPreviewDiagnostic(Out.VisualErrors, GetPathName(), TEXT("Missing selected definition key; no first-entry fallback."));
		return Out;
	}
	const int32 Count = !Entry->Model.IsNull()
		+ !Entry->ACTCharacter.IsNull() + !Entry->ACTWeapon.IsNull();
	if (Count != 1)
	{
		AddPreviewDiagnostic(Out.VisualErrors, GetPathName(), TEXT("Each entry must have exactly one source reference."));
		return Out;
	}
	FName SourceId;
	switch (Key.Domain)
	{
	case EPadmaPresentationSource::Model:
		if (const UPadmaModelDefinition* Source = Entry->Model.LoadSynchronous())
		{
			SourceId = Source->VisualDefinitionId;
			Out.DisplayName = Source->DisplayName;
			Out.ModelTransform = Source->ModelTransform;
			if (Source->ContentVersion < 1 || Source->ModelTransform.ContainsNaN())
			{
				AddPreviewDiagnostic(Out.VisualErrors, Source->GetPathName(), TEXT("Invalid content version or model transform."));
			}
			Visual(Out, Source->StaticModel, Source->SkeletalModel, Source->AnimationClass);
		}
		break;
	case EPadmaPresentationSource::ACTCharacter:
		if (const UPadmaACTCharacterDefinition* Source = Entry->ACTCharacter.LoadSynchronous())
		{
			SourceId = Source->DefinitionId;
			Out.DisplayName = Source->DisplayName;
			Visual(Out, {}, Source->Model, Source->AnimationClass);
			ACTSkills(Out, *Source, SelectedRow);
		}
		break;
	case EPadmaPresentationSource::ACTWeapon:
		if (const UPadmaACTWeaponDefinition* Source = Entry->ACTWeapon.LoadSynchronous())
		{
			SourceId = Source->DefinitionId;
			Out.DisplayName = Source->DisplayName;
			Visual(Out, Source->StaticModel, Source->SkeletalModel, {});
		}
		break;
	default: break;
	}
	if (SourceId.IsNone() || SourceId != Key.DefinitionId)
	{
		Out = FPadmaResolvedPresentation();
		Out.DefinitionId = Key.DefinitionId;
		AddPreviewDiagnostic(Out.VisualErrors, GetPathName(), TEXT("Wrong source domain, unloaded source, or mismatched definition ID."));
	}
	return Out;
}
