#include "Components/PoseableMeshComponent.h"
#include "Gameplay/ACT/Runtime/PadmaACTAnimNotifies.h"
#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Gameplay/ACT/Runtime/PadmaACTActions.h"
#include "Gameplay/ACT/Runtime/PadmaACTMelee.h"
#include "Gameplay/ACT/Runtime/PadmaACTMeleeLab.h"
#include "Camera/CameraComponent.h"
#include "Gameplay/ACT/Runtime/PadmaACTCamera.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "Gameplay/Combat/PadmaCombatAttributes.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/WorldSettings.h"
#include "UObject/UnrealType.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaChenActionFXTest,"DreamOfPadma.ACT.ChenActionFXLifecycle",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaChenActionFXTest::RunTest(const FString& Parameters)
{
    auto* D=LoadObject<UPadmaACTCharacterDefinition>(nullptr,TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/AbilitySystem/DA_ACTCharacter_CHEN"));
    FString Failure;
    if (!TestNotNull(TEXT("Saved character definition"),D) || !TestTrue(TEXT("Complete action catalog validates"),UPadmaACTActionsComponent::ValidateDefinition(D,Failure))) { AddError(Failure); return false; }
    auto* Ultimate=LoadObject<UAnimMontage>(nullptr,TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Montages/AM_Chen_LieFengShuang"));
    if (!TestNotNull(TEXT("Saved R montage"),Ultimate) || Ultimate->SlotAnimTracks.Num()!=1) return false;
    const auto& Track=Ultimate->SlotAnimTracks[0].AnimTrack;
    // Compare the saved playback mapping with original PlayAnimationAction data.
    // Clip frame 278 must coincide with action frame 103, not play 1.2 s late.
    const FVector2f SourceSamples[]={{77.f/30+.1f,122.f/30+.05f},
        {99.f/30+.05f,133.f/30+.075f},{103.f/30+.05f,139.f/30+.05f}};
    for (const auto& Sample:SourceSamples)
    {
        const auto* Segment=Track.GetSegmentAtTime(Sample.X);
        if (!TestNotNull(TEXT("Source R segment exists"),Segment)) return false;
        TestTrue(TEXT("R pose and FX share original action clock"),FMath::IsNearlyEqual(Segment->ConvertTrackPosToAnimPos(Sample.X),Sample.Y,.001f));
    }
    for (const auto& N:Ultimate->Notifies)
        TestTrue(TEXT("R notify stays inside retimed montage"),N.GetEndTriggerTime()<=Ultimate->GetPlayLength()+.001f);
    UWorld::InitializationValues Values; Values.AllowAudioPlayback(false).CreatePhysicsScene(true).ShouldSimulatePhysics(false).EnableTraceCollision(true).CreateNavigation(false).CreateAISystem(false);
    auto* World=UWorld::CreateWorld(EWorldType::Game,false,NAME_None,nullptr,true,ERHIFeatureLevel::Num,&Values);
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT { World->EndPlay(EEndPlayReason::Quit); World->DestroyWorld(false); GEngine->DestroyWorldContext(World); };
    auto* Floor=World->SpawnActor<AStaticMeshActor>();
    Floor->GetStaticMeshComponent()->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube")));
    Floor->SetActorScale3D(FVector(100,100,1)); Floor->SetActorLocation(FVector(0,0,-50));
    Floor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
    World->InitializeActorsForPlay(FURL());World->BeginPlay();World->GetWorldSettings()->NotifyBeginPlay();
    auto* Host=World->SpawnActor<AActor>();auto* Battle=NewObject<UPadmaCombatComponent>(Host);Host->AddInstanceComponent(Battle);Battle->RegisterComponent();Battle->SetComponentTickEnabled(false);
    Battle->PayCost=[](float,float,FName,FString&){return true;};
    FPadmaCombatSetup Setup;Setup.Mode=EPadmaCombatMode::ACT;
    FPadmaCombatUnitSpec P;P.Id=TEXT("chen");P.Health=P.MaxHealth=1000;P.Attack=20;P.Speed=160;P.Presentation.ACTDefinition=D;P.Location=FVector(0,0,95);Setup.Units.Add(P);
    auto E=P;E.Id=TEXT("enemy");E.bPlayer=false;E.Presentation={};E.Location=FVector(1000,0,90);Setup.Units.Add(E);
    E.Id=TEXT("enemy2");E.Location=FVector(1500,0,90);Setup.Units.Add(E);
    if (!TestTrue(TEXT("Character action battle starts"),Battle->StartBattle(Setup,Failure))) { AddError(Failure); return false; }
    auto* U=Battle->GetPlayerUnit();auto* Enemy=Battle->GetUnit(TEXT("enemy"));auto* Enemy2=Battle->GetUnit(TEXT("enemy2"));auto* A=U->GetActions();auto* M=U->GetMelee();auto* ASC=U->GetAbilitySystemComponent();
    auto Step=[&](float Dt){++GFrameCounter;World->Tick(LEVELTICK_All,Dt);};
    auto Run=[&](float Seconds){for(float Left=Seconds;Left>UE_SMALL_NUMBER;Left-=1.f/60)Step(FMath::Min(Left,1.f/60));};
    auto Cancel=[&](){ASC->CancelAllAbilities();M->FinishAttack(true);Run(.15f);};
    Run(1.5f);
    Enemy->SetActorLocation(FVector(20000,0,90));Enemy2->SetActorLocation(FVector(21000,0,90));
    struct FCase {const TCHAR* Binding; float Seconds; int32 Expected;};
    const FCase Cases[]={{TEXT("Combo2"),.8f,2},{TEXT("Combo3"),.8f,3},{TEXT("Combo4"),.8f,3},{TEXT("Combo5"),.9f,4},
        {TEXT("SkillE"),.8f,1},{TEXT("SkillQ"),1.1f,2},{TEXT("SkillR"),4.6f,12},{TEXT("Execution"),1.2f,2}};
    for (float Rate : {.5f,1.f,2.f})
    {
        for (const auto& C:Cases)
        {
            U->SetActorLocation(FVector(0,0,90));U->SetActorRotation(FRotator::ZeroRotator);Run(.1f);
            TestTrue(FString::Printf(TEXT("FX action starts %s"),C.Binding),A->RequestInput(C.Binding));M->SetAttackPlayRate(Rate);Run(C.Seconds/Rate);
            TestEqual(FString::Printf(TEXT("All source action FX spawned %s at %.1fx"),C.Binding,Rate),M->GetSpawnedActionEffects(),C.Expected);
            TestEqual(TEXT("Whiff never creates confirmed contact FX"),M->GetSpawnedHitEffects(),0);
            Cancel();TestEqual(TEXT("Cancel releases every source action instance"),M->GetLiveActionEffectCount(),0);
        }
    }
    M->SetAttackPlayRate(1);
    A->RequestInput(TEXT("SkillR"));Run(.4f);
    TArray<USkeletalMeshComponent*> MeshEffects;U->GetComponents(MeshEffects);
    MeshEffects.Remove(U->GetMesh());TestEqual(TEXT("R has one owned animated dragon"),MeshEffects.Num(),1);
    if (MeshEffects.Num()==1)
    {
        auto* Dragon=MeshEffects[0];const auto Pose=Dragon->GetComponentSpaceTransforms();
        Run(.4f);bool Changed=false;
        for (int32 I=0; I<Pose.Num(); ++I) Changed|=!Pose[I].Equals(Dragon->GetComponentSpaceTransforms()[I],.001f);
        TestTrue(TEXT("Dragon original skeletal clip advances"),Changed);
        Run(1.9f);
        if (auto* MID=Cast<UMaterialInstanceDynamic>(Dragon->GetMaterial(0)))
        {
            const float Opacity=MID->K2_GetScalarParameterValue(TEXT("SourceOpacity"));
            TestTrue(TEXT("Dragon fades between animation end and renderer stop"),Opacity>0.f && Opacity<1.f);
        }
        else AddError(TEXT("Dragon requires its own fade material instance"));
        Run(.4f);U->GetComponents(MeshEffects);MeshEffects.Remove(U->GetMesh());
        TestEqual(TEXT("Source R stop releases animated dragon"),MeshEffects.Num(),0);
    }
    Cancel();
    U->SetActorLocation(FVector(0,0,90));Run(.1f);
    TArray<UStaticMeshComponent*> StaticBefore;U->GetComponents(StaticBefore);
    int32 Equipped=0;while (M->GetWeapon(Equipped)) ++Equipped;
    TestTrue(TEXT("Dodge starts for pose snapshot"),A->RequestInput(TEXT("Dodge")));Run(.2f);
    TestEqual(TEXT("Dodge source afterimage burst"),M->GetSpawnedActionEffects(),1);
    TArray<UPoseableMeshComponent*> Ghosts;U->GetComponents(Ghosts);
    TestEqual(TEXT("One source pose snapshot"),Ghosts.Num(),1);
    TArray<UStaticMeshComponent*> SnapshotWeapons;U->GetComponents(SnapshotWeapons);
    TestEqual(TEXT("One snapshot per equipped weapon"),SnapshotWeapons.Num()-StaticBefore.Num(),Equipped);
    if (Ghosts.Num()==1)
    {
        auto* Ghost=Ghosts[0];const FTransform Frozen=Ghost->GetComponentTransform();
        TestFalse(TEXT("Frozen pose does not tick"),Ghost->IsComponentTickEnabled());
        TestTrue(TEXT("Snapshot skeleton matches"),Ghost->GetSkinnedAsset()==U->GetMesh()->GetSkinnedAsset());
        Run(.1f);TestTrue(TEXT("Afterimage stays at captured world transform"),Ghost->GetComponentTransform().Equals(Frozen));
    }
    Cancel();TestEqual(TEXT("Dodge cancellation removes snapshot"),M->GetLiveActionEffectCount(),0);
    A->RequestInput(TEXT("Dodge"));Run(.08f);TestTrue(TEXT("Incoming perfect event accepted"),A->TryEvade());Run(.23f);
    TestEqual(TEXT("Perfect variant active"),A->GetActiveActionId(),FName(TEXT("Chen.PerfectDodge")));
    TestEqual(TEXT("Perfect variant owns its source snapshot"),M->GetSpawnedActionEffects(),1);Cancel();
    A->RequestInput(TEXT("Jump"));Run(.12f);TestEqual(TEXT("Jump shared-source adaptation plays"),M->GetSpawnedActionEffects(),1);
    Run(.15f);TestTrue(TEXT("Airborne primary starts plunge"),A->RequestInput(TEXT("Primary")));Run(.03f);
    TestTrue(TEXT("Plunge air FX does not wait past UE early landing"),M->GetSpawnedActionEffects()>=1);
    Run(.8f);TestEqual(TEXT("Actual landing emits second plunge group"),M->GetSpawnedActionEffects(),2);Cancel();
    A->RequestInput(TEXT("Execution"));Run(.2f);
    M->FinishAttack(false);
    const int32 TailCount=M->GetLiveActionEffectCount();
    TestTrue(TEXT("Natural completion keeps live particles to finish"),TailCount>0);
    TestTrue(TEXT("Next action can start while particles retire"),A->RequestInput(TEXT("Combo2")));
    TestTrue(TEXT("Starting next action preserves the previous tail"),M->GetLiveActionEffectCount()>=TailCount);
    Cancel();TestEqual(TEXT("Cancel also removes retiring particles"),M->GetLiveActionEffectCount(),0);
    Enemy->SetActorLocation(U->GetActorLocation()+U->GetActorForwardVector()*180);
    TestTrue(TEXT("F starts with nearby confirmed target"),A->RequestInput(TEXT("Execution")));Run(1.35f);
    TestTrue(TEXT("F still settles contact damage"),M->GetConfirmedContacts()>0);
    TestEqual(TEXT("Source F disables the extra default impact layer"),M->GetSpawnedHitEffects(),0);Cancel();
    A->RequestInput(TEXT("Dodge"));Run(.2f);Battle->ExitBattle();
    TestEqual(TEXT("Battle exit clears live snapshots"),M->GetLiveActionEffectCount(),0);
    auto* Lab=World->SpawnActorDeferred<APadmaACTMeleeLab>(APadmaACTMeleeLab::StaticClass(),FTransform::Identity);
    Lab->CharacterDefinition=D;UGameplayStatics::FinishSpawningActor(Lab,FTransform::Identity);
    Lab->Battle->SetComponentTickEnabled(false);
    auto* CameraActions=Lab->Battle->GetPlayerUnit()->GetActions();
    Run(.3f);
    const FTransform Rest=Lab->Camera->GetComponentTransform();const float RestFOV=Lab->Camera->FieldOfView;
    TestTrue(TEXT("R camera action begins"),CameraActions->RequestInput(TEXT("SkillR")));Run(.3f);
    const FTransform FirstCamera=Lab->Camera->GetComponentTransform();
    TestFalse(TEXT("R source camera moves from gameplay view"),FirstCamera.Equals(Rest));
    Run(4.1f);
    TestTrue(TEXT("R can restart in its recovery window"),CameraActions->RequestInput(TEXT("SkillR")));Run(.3f);
    TestFalse(TEXT("Repeated R restarts source camera rather than staying at rest"),Lab->Camera->GetComponentTransform().Equals(Rest));
    Lab->CancelAttack();
    Run(.3f);
    TestTrue(TEXT("Cancel restores current character camera"),Lab->Camera->GetComponentTransform().Equals(Lab->Battle->GetPlayerUnit()->GetACTCamera()->GetComponentTransform(),.01f));
    TestTrue(TEXT("Cancel restores FOV"),FMath::IsNearlyEqual(Lab->Camera->FieldOfView,RestFOV,.01f));
    CameraActions->RequestInput(TEXT("SkillR"));Run(.3f);Lab->ResetBattle();
    Run(.3f);
    TestTrue(TEXT("Battle reset restores character follow camera"),Lab->Camera->GetComponentTransform().Equals(Lab->Battle->GetPlayerUnit()->GetACTCamera()->GetComponentTransform(),.01f));
    Lab->Destroy();
    return true;
}
