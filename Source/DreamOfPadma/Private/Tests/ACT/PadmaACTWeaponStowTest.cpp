#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Gameplay/ACT/Runtime/PadmaACTActions.h"
#include "Gameplay/ACT/Runtime/PadmaACTMelee.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimMontage.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/WorldSettings.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "PadmaACTLegacyFixture.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaACTWeaponStowTest,"DreamOfPadma.ACT.ChenWeaponStow",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaACTWeaponStowTest::RunTest(const FString& Parameters)
{
    auto* D=LoadObject<UPadmaACTCharacterDefinition>(nullptr,PadmaACTTest::CharacterPath);
    if (!TestNotNull(TEXT("Saved character"),D)) return false;
    auto* Profile=D->MeleeProfile.LoadSynchronous();
    if (!TestNotNull(TEXT("Saved equipment profile"),Profile) || !TestEqual(TEXT("Three independent equipment components"),Profile->Weapons.Num(),3)) return false;
    UWorld::InitializationValues Values;Values.AllowAudioPlayback(false).CreatePhysicsScene(true).ShouldSimulatePhysics(false).EnableTraceCollision(true).CreateNavigation(false).CreateAISystem(false);
    auto* World=UWorld::CreateWorld(EWorldType::Game,false,NAME_None,nullptr,true,ERHIFeatureLevel::Num,&Values);
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT {World->EndPlay(EEndPlayReason::Quit);World->DestroyWorld(false);GEngine->DestroyWorldContext(World);};
    auto* Floor=World->SpawnActor<AStaticMeshActor>();
    Floor->GetStaticMeshComponent()->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube")));
    Floor->SetActorScale3D(FVector(100,100,1));Floor->SetActorLocation(FVector(0,0,-50));Floor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
    World->InitializeActorsForPlay(FURL());World->BeginPlay();World->GetWorldSettings()->NotifyBeginPlay();
    auto* Host=World->SpawnActor<AActor>();auto* Battle=NewObject<UPadmaCombatComponent>(Host);Host->AddInstanceComponent(Battle);Battle->RegisterComponent();Battle->SetComponentTickEnabled(false);
    Battle->PayCost=[](float,float,FName,FString&){return true;};
    FPadmaCombatSetup Setup;Setup.Mode=EPadmaCombatMode::ACT;
    FPadmaCombatUnitSpec P;P.Id=TEXT("chen");P.Health=P.MaxHealth=1000;P.Attack=20;P.Speed=160;P.Presentation.ACTDefinition=D;P.Location=FVector(0,0,95);Setup.Units.Add(P);
    auto E=P;E.Id=TEXT("enemy");E.bPlayer=false;E.Presentation={};E.Location=FVector(3000,3000,90);Setup.Units.Add(E);
    FString Failure;
    if (!TestTrue(TEXT("Battle starts"),Battle->StartBattle(Setup,Failure))) {AddError(Failure);return false;}
    auto* U=Battle->GetPlayerUnit();auto* A=U->GetActions();auto* M=U->GetMelee();auto* ASC=U->GetAbilitySystemComponent();
    FVector HeldMove=FVector::ZeroVector;
    const float StowWait=Profile->bWeaponLifecycle
        ? Profile->DrawnHoldSeconds+Profile->SheatheWeaponMontage.LoadSynchronous()->GetPlayLength()
            +Profile->StowedHoldSeconds+Profile->WeaponDissolveSeconds+.2f : .6f;
    auto Run=[&](float Seconds){for(float Left=Seconds;Left>UE_SMALL_NUMBER;Left-=1.f/60){if(!HeldMove.IsNearlyZero()) A->Move(HeldMove);++GFrameCounter;World->Tick(LEVELTICK_All,FMath::Min(Left,1.f/60));}};
    auto Check=[&](bool Drawn)
    {
        for(int32 I=0;I<3;++I)
        {
            auto* W=M->GetWeapon(I);if(!TestNotNull(TEXT("Equipment exists"),W)) continue;
            const auto& C=Profile->Weapons[I];const bool Stowed=!Drawn && !C.StowedSocket.IsNone();
            TestEqual(TEXT("Current mount matches action presentation"),W->GetAttachSocketName(),Stowed ? C.StowedSocket : C.Socket);
            TestTrue(TEXT("Equipment scale remains one"),W->GetComponentScale().Equals(FVector::OneVector,.001));
            const auto Expected=(Stowed ? C.StowedTransform : FTransform::Identity)*U->GetMesh()->GetSocketTransform(Stowed ? C.StowedSocket : C.Socket);
            TestTrue(TEXT("Evaluated equipment transform follows mount without duplicated prefab offset"),W->GetComponentTransform().Equals(Expected,.02));
        }
    };
    auto Reset=[&](){HeldMove=FVector::ZeroVector;A->Move(FVector::ZeroVector);ASC->CancelAllAbilities();U->StopJumping();U->GetCharacterMovement()->StopMovementImmediately();U->SetActorLocation(FVector(0,0,90));U->GetCharacterMovement()->SetMovementMode(MOVE_Walking);Run(StowWait);};
    Run(1.5f);Check(false);
    TestEqual(TEXT("Main rest anchor uses source back helper"),Profile->Weapons[0].StowedSocket,FName(TEXT("wep_m")));
    TestEqual(TEXT("Offhand returns to source sheath mount"),Profile->Weapons[1].StowedSocket,FName(TEXT("inner")));
    auto Start=U->GetActorLocation();HeldMove=FVector::ForwardVector;Run(.4f);Check(false);
    TestTrue(TEXT("Walking with stowed weapons advances"),U->GetActorLocation().X>Start.X+10);
    A->RequestInput(TEXT("ToggleRun"));Run(.4f);Check(false);Reset();
    for(const FName Binding:{FName(TEXT("Primary")),FName(TEXT("SkillE")),FName(TEXT("SkillQ")),FName(TEXT("SkillR")),FName(TEXT("Execution"))})
    {
        TestTrue(TEXT("Weapon action activates"),A->RequestInput(Binding));Run(.12f);Check(true);
        ASC->CancelAllAbilities();
        // Lifecycle deliberately holds the drawn pose before sheath/dissolve; cancellation is not instant stow.
        if(Profile->bWeaponLifecycle && Profile->DrawnHoldSeconds>.2f) {Run(.1f);Check(true);}
        Run(StowWait);Check(false);Reset();
    }
    TestTrue(TEXT("Natural-end attack activates"),A->RequestInput(TEXT("Primary")));
    Run(M->GetAttackMontage()->GetPlayLength()+StowWait);Check(false);Reset();
    TestTrue(TEXT("Jump activates without weapons in hands"),A->RequestInput(TEXT("Jump")));Run(.3f);Check(false);
    TestTrue(TEXT("Airborne primary draws for plunge"),A->RequestInput(TEXT("Primary")));Run(.05f);Check(true);Reset();
    TestTrue(TEXT("Dodge activates without weapons in hands"),A->RequestInput(TEXT("Dodge")));Run(.05f);Check(false);Reset();
    TestTrue(TEXT("Attack before battle exit"),A->RequestInput(TEXT("Primary")));Run(.1f);Check(true);
    Battle->ExitBattle();Run(.1f);
    // Exercise no-rest-mount configuration through the runtime, without a saved second character.
    PadmaACTTest::FLegacyMeleeFixture Legacy;
    if (!Legacy.Initialize(*this)) return false;
    TestFalse(TEXT("Legacy lifecycle disabled"),Legacy.Profile->bWeaponLifecycle);
    for(const auto& W:Legacy.Profile->Weapons) TestTrue(TEXT("Legacy mounts absent"),W.StowedSocket.IsNone());
    Setup.Units[0].Presentation.ACTDefinition=Legacy.Character.Get();
    if (!TestTrue(TEXT("Transient legacy battle starts"),Battle->StartBattle(Setup,Failure))) {AddError(Failure);return false;}
    U=Battle->GetPlayerUnit();A=U->GetActions();M=U->GetMelee();ASC=U->GetAbilitySystemComponent();
    HeldMove=FVector::ZeroVector;
    auto CheckLegacy=[&]()
    {
        for(int32 I=0;I<Legacy.Profile->Weapons.Num();++I)
        {
            auto* W=M->GetWeapon(I);if(!TestNotNull(TEXT("Legacy equipment exists"),W)) continue;
            TestEqual(TEXT("No rest mount keeps held socket"),W->GetAttachSocketName(),Legacy.Profile->Weapons[I].Socket);
            TestTrue(TEXT("Legacy equipment remains visible"),W->IsVisible());
        }
    };
    Run(.1f);CheckLegacy();
    TestTrue(TEXT("Legacy attack activates"),Battle->Attack({},Failure));Run(.35f);CheckLegacy();
    ASC->CancelAllAbilities();Run(StowWait);CheckLegacy();
    Battle->ExitBattle();
    TestTrue(TEXT("Live profile identity preserved"),D->MeleeProfile.LoadSynchronous()==Profile);
    TestEqual(TEXT("Live main rest anchor unchanged"),Profile->Weapons[0].StowedSocket,FName(TEXT("wep_m")));
    TestEqual(TEXT("Live offhand rest anchor unchanged"),Profile->Weapons[1].StowedSocket,FName(TEXT("inner")));
    return true;
}
