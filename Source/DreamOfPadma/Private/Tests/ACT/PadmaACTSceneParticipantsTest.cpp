#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Gameplay/ACT/Runtime/PadmaACTMeleeLab.h"
#include "Gameplay/ACT/Runtime/PadmaACTActions.h"
#include "Gameplay/ACT/Runtime/PadmaACTTrainingDummyPresentation.h"
#include "Gameplay/ACT/Authoring/PadmaACTAuthoring.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimSequence.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/WorldSettings.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaACTSceneParticipantsTest,"DreamOfPadma.ACT.SceneParticipants",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPadmaACTSceneParticipantsTest::RunTest(const FString&)
{
    auto* Definition=LoadObject<UPadmaACTCharacterDefinition>(nullptr,TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/AbilitySystem/DA_ACTCharacter_CHEN"));
    auto* DummyMesh=LoadObject<USkeletalMesh>(nullptr,TEXT("/Game/Sandbox/ACT/Training/WoodenDummy/Art/Meshes/SK_WoodenDummy"));
    auto* Hit=LoadObject<UAnimSequence>(nullptr,TEXT("/Game/Sandbox/ACT/Training/WoodenDummy/Animation/Sequences/AS_WoodenDummy_Hit"));
    if (!TestNotNull(TEXT("Character definition"),Definition) || !TestNotNull(TEXT("Dummy mesh"),DummyMesh)) return false;
    UWorld::InitializationValues Values;
    Values.AllowAudioPlayback(false).CreatePhysicsScene(true).ShouldSimulatePhysics(false).CreateNavigation(false).CreateAISystem(false);
    auto* World=UWorld::CreateWorld(EWorldType::Game,false,NAME_None,nullptr,true,ERHIFeatureLevel::Num,&Values);
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT {World->EndPlay(EEndPlayReason::Quit);World->DestroyWorld(false);GEngine->DestroyWorldContext(World);};
    World->InitializeActorsForPlay(FURL());World->BeginPlay();World->GetWorldSettings()->NotifyBeginPlay();
    auto* Player=World->SpawnActor<APadmaCombatUnit>();
    auto* Target=World->SpawnActor<APadmaCombatUnit>();
    Player->SceneSpec.Id=TEXT("scene-player");Player->SceneSpec.Health=Player->SceneSpec.MaxHealth=1000;
    Player->SceneSpec.Presentation.ACTDefinition=Definition;Player->ApplyScenePresentation();
    Target->SceneSpec.Id=TEXT("scene-target");Target->SceneSpec.bPlayer=false;
    Target->SceneSpec.Health=Target->SceneSpec.MaxHealth=1000;Target->SceneSpec.bCanPursueInACT=false;
    Target->SceneSpec.Presentation.Model=DummyMesh;Target->ApplyScenePresentation();
    const FTransform Start(FRotator(0,42,0),FVector(35,24,90));Player->SetActorTransform(Start);
    Target->SetActorLocation(FVector(260,24,90));
    const FTransform Visual(FRotator(0,-73,0),FVector(3,4,-86),FVector(1.04));Player->GetMesh()->SetRelativeTransform(Visual);
    Player->GetCapsuleComponent()->SetCapsuleSize(24,91);
    auto* OriginalMaterial=Player->GetMesh()->GetMaterial(0);
    auto SceneWeapons=[&]()
    {
        TArray<UStaticMeshComponent*> All;Player->GetComponents(All);
        All.RemoveAll([](UStaticMeshComponent* C){return !C->ComponentTags.ContainsByPredicate([](FName T){return T.ToString().StartsWith(TEXT("Padma.SceneWeapon."));});});
        return All;
    };
    auto Weapons=SceneWeapons();const int32 ComponentCount=Weapons.Num();
    if(!TestTrue(TEXT("Scene weapons authored"),ComponentCount>0)) return false;
    auto* Weapon=Weapons[0];
    Weapon->AddLocalOffset(FVector(1,2,3));Weapon->SetRelativeScale3D(FVector(1.06));
    const FTransform WeaponTransform=Weapon->GetRelativeTransform();const FName WeaponSocket=Weapon->GetAttachSocketName();
    auto* Lab=World->SpawnActorDeferred<APadmaACTMeleeLab>(APadmaACTMeleeLab::StaticClass(),FTransform::Identity,nullptr,nullptr,ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
    Lab->ScenePlayer=Player;Lab->SceneTargets={Target};Lab->bRequireSceneParticipants=true;
    Lab->CharacterDefinition=Definition;Lab->TrainingDummyHitAnimation=Hit;Lab->FinishSpawning(FTransform::Identity);
    if(!TestTrue(TEXT("Placed battle starts"),Lab->Battle->IsBattleActive())) return false;
    auto* ASC=Player->GetAbilitySystemComponent();const int32 Grants=ASC->GetActivatableAbilities().Num();
    for(int32 Iteration=0;Iteration<3;++Iteration)
    {
        TestEqual(TEXT("Exact placed player is used"),Lab->Battle->GetPlayerUnit(),Player);
        TestEqual(TEXT("Exact placed target is used"),Lab->Battle->GetUnit(TEXT("scene-target")),Target);
        TestTrue(TEXT("Authored actor transform retained/restored"),Player->GetActorTransform().Equals(Start));
        TestTrue(TEXT("Authored mesh transform retained"),Player->GetMesh()->GetRelativeTransform().Equals(Visual));
        TestEqual(TEXT("Authored body material retained"),Player->GetMesh()->GetMaterial(0),OriginalMaterial);
        TestEqual(TEXT("Authored capsule retained"),Player->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(),91.f);
        TestEqual(TEXT("Ability grants do not accumulate"),ASC->GetActivatableAbilities().Num(),Grants);
        Weapons=SceneWeapons();TestEqual(TEXT("Weapon components do not accumulate"),Weapons.Num(),ComponentCount);
        TestTrue(TEXT("Weapon authored correction retained on reset"),Weapon->GetRelativeTransform().Equals(WeaponTransform));
        TestEqual(TEXT("Weapon stowed socket restored"),Weapon->GetAttachSocketName(),WeaponSocket);
        TestTrue(TEXT("Weapon visible again after reset"),Weapon->IsVisible());
        if(auto* MID=Cast<UMaterialInstanceDynamic>(Weapon->GetMaterial(0)))
            TestEqual(TEXT("Weapon dissolve reset"),MID->K2_GetScalarParameterValue(TEXT("WeaponDissolve")),0.f);
        TArray<UPadmaACTTrainingDummyPresentation*> Reactions;Target->GetComponents(Reactions);
        TestEqual(TEXT("One reusable dummy reaction component"),Reactions.Num(),1);
        TestTrue(TEXT("Action activates after each reset"),Player->GetActions()->RequestInput(TEXT("SkillE")));
        Weapon->SetVisibility(false);
        if(auto* MID=Cast<UMaterialInstanceDynamic>(Weapon->GetMaterial(0))) MID->SetScalarParameterValue(TEXT("WeaponDissolve"),1.f);
        Player->SetActorLocation(FVector(400,400,150));
        Lab->ResetBattle();
    }
    Lab->Battle->ExitBattle();
    TestTrue(TEXT("Borrowed actors survive exit"),IsValid(Player) && IsValid(Target));
    TestNull(TEXT("Combat pointer cleared"),Player->GetBattle());
    TestEqual(TEXT("All grants cleared"),ASC->GetActivatableAbilities().Num(),0);
    TestFalse(TEXT("Action configuration cleared"),Player->GetActions()->IsConfigured());
    FPadmaCombatSetup Invalid;Invalid.Mode=EPadmaCombatMode::ACT;Invalid.Units={Player->SceneSpec,Target->SceneSpec};
    FString Failure;
    TestFalse(TEXT("Duplicate placed actor rejected before registration"),Lab->Battle->StartBattle(Invalid,Failure,{Player,Player}));
    TestNull(TEXT("Rejected registration leaves actor unowned"),Player->GetBattle());
    Lab->ResetBattle();TestTrue(TEXT("Valid reset still works after rejection"),Lab->Battle->IsBattleActive());
    return true;
}

#if WITH_EDITOR
#include "Editor.h"
#include "EngineUtils.h"
#include "Tests/AutomationEditorCommon.h"
#include "Tests/AutomationCommon.h"
#include "GameFramework/PlayerController.h"

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FCheckPadmaPlacedPIE,FAutomationTestBase*,Test);
bool FCheckPadmaPlacedPIE::Update()
{
    UWorld* World=GEditor->PlayWorld;
    if(!Test->TestNotNull(TEXT("PIE world exists"),World)) return true;
    APadmaACTMeleeLab* Lab=nullptr;
    for(TActorIterator<APadmaACTMeleeLab> It(World);It;++It) {Lab=*It;break;}
    if(!Test->TestNotNull(TEXT("Saved map lab exists"),Lab)) return true;
    Test->TestTrue(TEXT("Saved map forbids runtime replacements"),Lab->bRequireSceneParticipants);
    auto* Player=Lab->ScenePlayer.Get();
    if(!Test->TestNotNull(TEXT("Placed player survives BeginPlay"),Player)) return true;
    Test->TestEqual(TEXT("Saved map target count"),Lab->SceneTargets.Num(),3);
    auto* PC=World->GetFirstPlayerController();
    if(PC) Test->TestEqual(TEXT("Player camera is active"),PC->GetViewTarget(),static_cast<AActor*>(Player));
    const auto Targets=Lab->SceneTargets;
    for(int32 Pass=0;Pass<3;++Pass)
    {
        Test->TestTrue(TEXT("Actual map battle active"),Lab->Battle->IsBattleActive());
        Test->TestEqual(TEXT("PIE uses saved player"),Lab->Battle->GetPlayerUnit(),Player);
        Test->TestTrue(TEXT("PIE action activates"),Player->GetActions()->RequestInput(TEXT("SkillE")));
        Lab->ResetBattle();
        Test->TestEqual(TEXT("PIE reset reuses saved player"),Lab->Battle->GetPlayerUnit(),Player);
        for(APadmaCombatUnit* Target:Targets)
            Test->TestTrue(TEXT("PIE reset reuses each saved target"),IsValid(Target) && Lab->Battle->GetUnit(Target->SceneSpec.Id)==Target);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaACTSceneMapPIETest,"DreamOfPadma.ACT.SceneMapPIE",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPadmaACTSceneMapPIETest::RunTest(const FString&)
{
    FAutomationEditorCommonUtils::LoadMap(TEXT("/Game/Sandbox/ACT/Training/Maps/L_ChenACT"));
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.0f));
    ADD_LATENT_AUTOMATION_COMMAND(FCheckPadmaPlacedPIE(this));
    ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand());
    return true;
}
#endif
