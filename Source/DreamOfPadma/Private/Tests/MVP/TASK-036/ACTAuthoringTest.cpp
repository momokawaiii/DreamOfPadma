#include "Misc/AutomationTest.h"

#include "Gameplay/ACT/Authoring/PadmaACTAuthoring.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "Animation/AnimMontage.h"
#include "Animation/Skeleton.h"
#include "Engine/SkeletalMesh.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaACTAuthoringTest, "DreamOfPadma.MVP.TASK036.AuthoringValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPadmaACTAuthoringTest::RunTest(const FString& Parameters)
{
	UPadmaACTAuthoringCatalog* Catalog = NewObject<UPadmaACTAuthoringCatalog>();
	TestFalse(TEXT("Empty catalog is a draft"), Catalog->ValidateCatalog().bValid);
	UPadmaACTCharacterDefinition* Character = NewObject<UPadmaACTCharacterDefinition>();
	Catalog->Characters.Add(Character);
	TestFalse(TEXT("Missing character fields rejected"), Catalog->ValidateCatalog().bValid);
	Character->DefinitionId = TEXT("ACT.Character.Test");
	Character->DisplayName = FText::FromString(TEXT("Authoring fixture"));
	USkeletalMesh* Mesh = NewObject<USkeletalMesh>();
	USkeleton* Skeleton = NewObject<USkeleton>();
	Mesh->SetSkeleton(Skeleton);
	Character->Model = Mesh;
	UDataTable* Table = NewObject<UDataTable>();
	Table->RowStruct = FPadmaACTSkillRow::StaticStruct();
	Character->SkillTable = Table;
	TestFalse(TEXT("Empty ACT skill table rejected"), Catalog->ValidateCatalog().bValid);
	UPadmaACTSkillDefinition* Skill = NewObject<UPadmaACTSkillDefinition>();
	Skill->DefinitionId = TEXT("ACT.Skill.Test");
	Skill->DisplayName = FText::FromString(TEXT("Test binding"));
	Skill->AbilityImplementationId = TEXT("ACT.Executor.Test");
	FPadmaACTSkillRow Row;
	Row.SkillId = Skill->DefinitionId;
	Row.ActivationBindingId = TEXT("ACT.Input.Test");
	Row.Definition = Skill;
	Table->AddRow(TEXT("Test"), Row);
	TestFalse(TEXT("Undeclared executor/input rejected"), Catalog->ValidateCatalog().bValid);
	Catalog->References.AbilityImplementationIds.Add(Skill->AbilityImplementationId);
	Catalog->References.ActivationBindingIds.Add(Row.ActivationBindingId);
	TestTrue(TEXT("Complete static references validate without gameplay values"), Catalog->ValidateCatalog().bValid);

	UDataTable* WrongTable = NewObject<UDataTable>();
	WrongTable->RowStruct = FTableRowBase::StaticStruct();
	Character->SkillTable = WrongTable;
	TestFalse(TEXT("Wrong mode/row schema rejected before reading rows"), Catalog->ValidateCatalog().bValid);
	Character->SkillTable = Table;
	Table->AddRow(TEXT("Duplicate"), Row);
	TestFalse(TEXT("Duplicate skill and activation binding rejected"), Catalog->ValidateCatalog().bValid);
	Table->RemoveRow(TEXT("Duplicate"));
	Row.SkillId = TEXT("ACT.Skill.Mismatch");
	Table->AddRow(TEXT("Test"), Row);
	TestFalse(TEXT("Dangling skill identity rejected"), Catalog->ValidateCatalog().bValid);
	Row.SkillId = Skill->DefinitionId;
	Table->AddRow(TEXT("Test"), Row);

	UPadmaACTWeaponDefinition* Weapon = NewObject<UPadmaACTWeaponDefinition>();
	Weapon->DefinitionId = Character->DefinitionId;
	Weapon->DisplayName = FText::FromString(TEXT("Weapon fixture"));
	Weapon->SkeletalModel = Mesh;
	Catalog->Weapons.Add(Weapon);
	TestFalse(TEXT("Duplicate definition identity across asset kinds rejected"), Catalog->ValidateCatalog().bValid);
	Weapon->DefinitionId = TEXT("ACT.Weapon.Test");
	TestTrue(TEXT("Separate weapon identity validates"), Catalog->ValidateCatalog().bValid);

	Character->TraitIds.Add(TEXT("ACT.Trait.Test"));
	FPadmaACTTraitTerrainRestriction Restriction;
	Restriction.TraitId = TEXT("ACT.Trait.Test");
	Restriction.DisallowedTerrainIds.Add(TEXT("Terrain.Test"));
	Character->TerrainRestrictions.Add(Restriction);
	TestFalse(TEXT("Unknown external terrain rejected"), Catalog->ValidateCatalog().bValid);
	Catalog->References.TerrainIds.Add(TEXT("Terrain.Test"));
	TestTrue(TEXT("Restriction validates as data without banning character"), Catalog->ValidateCatalog().bValid);
	Character->TerrainRestrictions[0].TraitId = TEXT("ACT.Trait.Undeclared");
	TestFalse(TEXT("Restriction must target a declared trait"), Catalog->ValidateCatalog().bValid);
	Character->TerrainRestrictions[0].TraitId = TEXT("ACT.Trait.Test");

	UAnimMontage* Montage = NewObject<UAnimMontage>();
	Montage->SetSkeleton(NewObject<USkeleton>());
	Skill->Montage = Montage;
	TestFalse(TEXT("Montage skeleton mismatch rejected"), Catalog->ValidateCatalog().bValid);
	Montage->SetSkeleton(Skeleton);
	TestTrue(TEXT("Matching montage skeleton validates"), Catalog->ValidateCatalog().bValid);
	TestEqual(TEXT("Validation did not remove any trait"), Character->TraitIds.Num(), 1);
	TestEqual(TEXT("Validation did not remove character selection"), Catalog->Characters.Num(), 1);
	return true;
}
