#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Game/Presentation/PadmaNodePreview.h"
#include "Gameplay/ACT/Authoring/PadmaACTAuthoring.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "Animation/AnimInstance.h"
#include "Animation/Skeleton.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaModelAuthoringTest, "DreamOfPadma.MVP.TASK040.ModelAndSkillResolution",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPadmaModelAuthoringTest::RunTest(const FString& Parameters)
{
	FPadmaCardMobilityDefinition Mobility;
	FText Failure;
	TestFalse(TEXT("Unconfigured is not immovable approval"), Mobility.Validate(Failure));
	Mobility.bConfigured = true;
	TestTrue(TEXT("Explicit immovable metadata is valid"), Mobility.Validate(Failure));
	Mobility.Policy = EPadmaCardMobility::Conditional;
	TestFalse(TEXT("Condition missing"), Mobility.Validate(Failure));
	Mobility.ConditionId = TEXT("Condition.Authoring.Test");
	TestTrue(TEXT("Condition reference is data, not evaluator permission"), Mobility.Validate(Failure));
	Mobility.Policy = EPadmaCardMobility::Movable;
	TestFalse(TEXT("Unconditional card cannot carry stale condition"), Mobility.Validate(Failure));
	Mobility.ConditionId = NAME_None;
	TestTrue(TEXT("Explicit movable metadata valid"), Mobility.Validate(Failure));
	TestNotNull(TEXT("ACT character has card mobility"), UPadmaACTCharacterDefinition::StaticClass()->FindPropertyByName(TEXT("Mobility")));

	UPadmaPresentationCatalog* Catalog = NewObject<UPadmaPresentationCatalog>();
	UPadmaModelDefinition* Model = NewObject<UPadmaModelDefinition>();
	Model->VisualDefinitionId = TEXT("Test.SameId");
	Model->DisplayName = FText::FromString(TEXT("Model fixture"));
	FPadmaPresentationCatalogEntry Entry;
	Entry.Key.DefinitionId = Model->VisualDefinitionId;
	Entry.Model = Model;
	Catalog->Entries.Add(Entry);
	TestFalse(TEXT("Empty model draft rejected"), Catalog->Resolve(Entry.Key, NAME_None).bVisualReady);
	Model->StaticModel = NewObject<UStaticMesh>();
	TestTrue(TEXT("Static visual resolves"), Catalog->Resolve(Entry.Key, NAME_None).bVisualReady);
	USkeletalMesh* Skeletal = NewObject<USkeletalMesh>();
	Skeletal->SetSkeleton(NewObject<USkeleton>());
	Model->SkeletalModel = Skeletal;
	TestFalse(TEXT("Two visual types rejected"), Catalog->Resolve(Entry.Key, NAME_None).bVisualReady);
	Model->StaticModel = nullptr;
	UAnimBlueprintGeneratedClass* Animation = NewObject<UAnimBlueprintGeneratedClass>();
	Animation->SetSuperStruct(UAnimInstance::StaticClass());
	Animation->TargetSkeleton = Skeletal->GetSkeleton();
	Model->AnimationClass = Animation;
	TestTrue(TEXT("Matching AnimInstance skeleton accepted"), Catalog->Resolve(Entry.Key, NAME_None).bVisualReady);
	Animation->TargetSkeleton = NewObject<USkeleton>();
	TestFalse(TEXT("Wrong skeleton rejected"), Catalog->Resolve(Entry.Key, NAME_None).bVisualReady);
	Model->AnimationClass = nullptr;
	TestTrue(TEXT("Skeletal authoring without animation can be displayed"), Catalog->Resolve(Entry.Key, NAME_None).bVisualReady);
	Catalog->Entries.Add(Entry);
	TestFalse(TEXT("Duplicate qualified identity rejected"), Catalog->Resolve(Entry.Key, NAME_None).bVisualReady);
	Catalog->Entries.Pop();

	UPadmaACTCharacterDefinition* ACT = NewObject<UPadmaACTCharacterDefinition>();
	ACT->DefinitionId = Model->VisualDefinitionId;
	ACT->DisplayName = FText::FromString(TEXT("Independent ACT identity"));
	ACT->Model = Skeletal;
	FPadmaPresentationCatalogEntry ACTEntry;
	ACTEntry.Key.Domain = EPadmaPresentationSource::ACTCharacter;
	ACTEntry.Key.DefinitionId = ACT->DefinitionId;
	ACTEntry.ACTCharacter = ACT;
	Catalog->Entries.Add(ACTEntry);
	auto View = Catalog->Resolve(ACTEntry.Key, NAME_None);
	TestTrue(TEXT("Same textual ID in different domains allowed"), View.bVisualReady);
	TestFalse(TEXT("Model readiness is not skill readiness"), View.bSkillDetailsReady);
	UDataTable* Wrong = NewObject<UDataTable>();
	Wrong->RowStruct = FTableRowBase::StaticStruct();
	ACT->SkillTable = Wrong;
	TestFalse(TEXT("Wrong row schema rejected"), Catalog->Resolve(ACTEntry.Key, NAME_None).bSkillDetailsReady);
	UDataTable* Table = NewObject<UDataTable>();
	Table->RowStruct = FPadmaACTSkillRow::StaticStruct();
	ACT->SkillTable = Table;
	UPadmaACTSkillDefinition* Skill = NewObject<UPadmaACTSkillDefinition>();
	Skill->DefinitionId = TEXT("Skill.Test");
	Skill->DisplayName = FText::FromString(TEXT("Test skill"));
	FPadmaACTSkillRow Row;
	Row.SkillId = Skill->DefinitionId;
	Row.ActivationBindingId = TEXT("Input.Test");
	Row.Definition = Skill;
	Table->AddRow(TEXT("First"), Row);
	View = Catalog->Resolve(ACTEntry.Key, TEXT("First"));
	TestTrue(TEXT("Read-only skill identity does not require a GAS executor"), View.bSkillDetailsReady);
	if (!TestEqual(TEXT("Selected table row read"), View.Skills.Num(), 1)) { return false; }
	TestTrue(TEXT("Absent ACT description stays empty"), View.Skills[0].Description.IsEmpty());
	TestFalse(TEXT("Unknown row rejected"), Catalog->Resolve(ACTEntry.Key, TEXT("Absent")).bSkillDetailsReady);
	Row.SkillId = TEXT("Mismatch");
	Table->AddRow(TEXT("First"), Row);
	TestEqual(TEXT("Bad row cannot leak stale/partial skills"), Catalog->Resolve(ACTEntry.Key, NAME_None).Skills.Num(), 0);
	Model->VisualDefinitionId = TEXT("Wrong.Identity");
	TestFalse(TEXT("Source identity mismatch rejected"), Catalog->Resolve(Entry.Key, NAME_None).bVisualReady);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaModelPreviewTest, "DreamOfPadma.MVP.TASK040.NodeBindingReplacement",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPadmaModelPreviewTest::RunTest(const FString& Parameters)
{
	UWorld::InitializationValues Values;
	Values.AllowAudioPlayback(false).CreatePhysicsScene(false).ShouldSimulatePhysics(false)
		.EnableTraceCollision(false).CreateNavigation(false).CreateAISystem(false);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
	if (!TestNotNull(TEXT("Preview test world"), World)) { return false; }
	GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
	ON_SCOPE_EXIT { World->DestroyWorld(false); GEngine->DestroyWorldContext(World); };
	UPadmaModelDefinition* Model = NewObject<UPadmaModelDefinition>();
	Model->VisualDefinitionId = TEXT("Model.Test");
	Model->StaticModel = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	UPadmaPresentationCatalog* Catalog = NewObject<UPadmaPresentationCatalog>();
	FPadmaPresentationCatalogEntry Entry;
	Entry.Key.DefinitionId = Model->VisualDefinitionId;
	Entry.Model = Model;
	Catalog->Entries.Add(Entry);
	UPadmaNodeSceneDefinition* Scene = NewObject<UPadmaNodeSceneDefinition>();
	Scene->SceneDefinitionId = TEXT("Scene.Test");
	FPadmaSceneSlot Slot;
	Slot.SlotId = TEXT("Garrison");
	Scene->Slots.Add(Slot);
	APadmaNodePreview* Preview = World->SpawnActor<APadmaNodePreview>();
	Preview->PreviewNodeId = TEXT("Node.Test");
	Preview->SceneDefinition = Scene;
	Preview->Catalog = Catalog;
	FPadmaPreviewBinding Binding;
	Binding.SlotId = Slot.SlotId;
	Binding.EntityInstanceId = TEXT("Instance.One");
	Binding.Definition = Entry.Key;
	Preview->PreviewBindings.Add(Binding);
	TestTrue(TEXT("Preview creates visual from explicit definition"), Preview->RebuildPreview());
	if (!TestEqual(TEXT("One presenter"), Preview->Presenters.Num(), 1)) { return false; }
	APadmaModelPresentationActor* Old = Preview->Presenters[0];
	TestTrue(TEXT("Rebuild succeeds"), Preview->RebuildPreview());
	TestTrue(TEXT("Old projection destroyed"), Old->IsActorBeingDestroyed());
	if (!TestEqual(TEXT("No duplicate presenters"), Preview->Presenters.Num(), 1)) { return false; }
	Preview->PreviewBindings[0].SelectedSkillRow = TEXT("Stale");
	FPadmaPresentationKey Missing = Entry.Key;
	Missing.DefinitionId = TEXT("Model.Unknown");
	TestFalse(TEXT("Missing replacement is visible failure"), Preview->SetPreviewSelection(Slot.SlotId, Missing));
	TestTrue(TEXT("Changing definition clears row selection"), Preview->PreviewBindings[0].SelectedSkillRow.IsNone());
	if (!TestEqual(TEXT("Failure retains one empty projection"), Preview->Presenters.Num(), 1)) { return false; }
	TestNull(TEXT("Failure clears old model"), Preview->Presenters[0]->StaticVisual->GetStaticMesh());
	TestEqual(TEXT("Failure clears old skills"), Preview->Presenters[0]->Presentation.Skills.Num(), 0);
	TestTrue(TEXT("Rebind known definition"), Preview->SetPreviewSelection(Slot.SlotId, Entry.Key));
	Preview->PreviewBindings.Add(Binding);
	TestFalse(TEXT("Duplicate slot/instance rejected before constructing"), Preview->RebuildPreview());
	TestEqual(TEXT("Invalid layout leaves no partial actors"), Preview->Presenters.Num(), 0);
	Preview->PreviewBindings.Pop();
	Scene->Slots.Add(Slot);
	TestFalse(TEXT("Duplicate static slot rejected"), Preview->RebuildPreview());
	TestEqual(TEXT("Static scene/card definitions never changed by selection"), Catalog->Entries[0].Key.DefinitionId, Model->VisualDefinitionId);
	return true;
}
