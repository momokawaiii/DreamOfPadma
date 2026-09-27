#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Gameplay/ACT/Runtime/PadmaACTActions.h"
#include "Gameplay/ACT/Runtime/PadmaACTMelee.h"
#include "Gameplay/ACT/Runtime/PadmaACTCamera.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/WorldSettings.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaChenCameraTest,"DreamOfPadma.ACT.ChenCharacterCamera",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaChenCameraTest::RunTest(const FString& Parameters)
{
    auto* D=LoadObject<UPadmaACTCharacterDefinition>(nullptr,TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/AbilitySystem/DA_ACTCharacter_CHEN"));
    if (!TestNotNull(TEXT("Saved character"),D)) return false;
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
    auto E=P;E.Id=TEXT("enemy");E.bPlayer=false;E.Presentation={};E.Location=FVector(4000,4000,90);Setup.Units.Add(E);
    E.Id=TEXT("enemy2");E.Location=FVector(3500,3500,90);Setup.Units.Add(E);
    FString Failure;
    if (!TestTrue(TEXT("Battle starts"),Battle->StartBattle(Setup,Failure))) { AddError(Failure);return false; }
    auto* U=Battle->GetPlayerUnit();auto* A=U->GetActions();auto* M=U->GetMelee();auto* ASC=U->GetAbilitySystemComponent();
    auto Run=[&](float Seconds){for(float Left=Seconds;Left>UE_SMALL_NUMBER;Left-=1.f/60){++GFrameCounter;World->Tick(LEVELTICK_All,FMath::Min(Left,1.f/60));}};
    auto Reset=[&](){A->Move(FVector::ZeroVector);ASC->CancelAllAbilities();U->StopJumping();U->GetCharacterMovement()->StopMovementImmediately();U->SetActorLocation(FVector(0,0,90));U->GetCharacterMovement()->SetMovementMode(MOVE_Walking);Run(.25f);};
    Run(1.5f);
    auto* C=U->GetACTCamera();auto* Enemy=Battle->GetUnit(TEXT("enemy"));auto* Other=Battle->GetUnit(TEXT("enemy2"));
    TestTrue(TEXT("Saved profile configures character-owned camera"),C->IsConfigured() && C->IsUsable());
    FMinimalViewInfo View;U->CalcCamera(0,View);
    TestTrue(TEXT("Character CalcCamera returns component view"),View.Location.Equals(C->GetComponentLocation(),.01));
    const FVector CameraStart=C->GetComponentLocation();
    U->SetActorLocation(U->GetActorLocation()+FVector(400,0,0));Run(1.2f);
    TestTrue(TEXT("Camera follows capsule displacement"),C->GetComponentLocation().X>CameraStart.X+390);
    TestTrue(TEXT("Source 58 vertical degrees converted to horizontal FOV"),FMath::IsNearlyEqual(C->FieldOfView,FMath::RadiansToDegrees(2*FMath::Atan(FMath::Tan(FMath::DegreesToRadians(29.f))*C->AspectRatio)),.01f));
    auto* CameraProfile=D->CameraProfile.LoadSynchronous();
    if(!TestNotNull(TEXT("Camera profile loads"),CameraProfile)) return false;
    const float QuarterTurnInput=90.f/CameraProfile->MouseYawSensitivity;
    C->Look(FVector2D(QuarterTurnInput,0));Run(.8f);
    TestTrue(TEXT("Forward input follows rotated camera yaw"),C->CameraRelativeMovement(FVector2D(1,0)).Y>.99);
    C->Look(FVector2D(-QuarterTurnInput,0));Run(.8f);
    const FVector ZoomOrigin=C->GetComponentLocation();
    C->Zoom(10000);Run(3.f);const FVector Near=C->GetComponentLocation();
    TestTrue(TEXT("Unlocked wheel visibly moves camera closer"),Near.X>ZoomOrigin.X+50);
    C->Zoom(10000);Run(3.f);
    TestTrue(TEXT("Repeated zoom-in stops at near limit"),C->GetComponentLocation().Equals(Near,1.f));
    C->Zoom(-10000);Run(3.f);const FVector Far=C->GetComponentLocation();
    TestTrue(TEXT("Unlocked wheel visibly moves camera farther"),Far.X<Near.X-150);
    C->Zoom(-10000);Run(3.f);
    TestTrue(TEXT("Repeated zoom-out stops at far limit"),C->GetComponentLocation().Equals(Far,1.f));
    C->ResetView();Run(3.f);
    Enemy->SetActorLocation(U->GetActorLocation()+FVector(220,0,0));Other->SetActorLocation(U->GetActorLocation()+FVector(300,250,0));
    TestTrue(TEXT("Middle-button command acquires visible center enemy"),C->ToggleLock());
    TestTrue(TEXT("Camera lock uses selected enemy"),C->GetLockedTarget()==Enemy);
    Run(.5f);Enemy->SetActorLocation(U->GetActorLocation()+FVector(220,150,0));Run(1.f);
    TestTrue(TEXT("Camera tracks target around character"),C->CameraRelativeMovement(FVector2D(1,0)).Y>.3f);
    Enemy->SetActorLocation(U->GetActorLocation()+FVector(220,0,0));Run(1.f);
    Other->SetActorLocation(U->GetActorLocation()+FVector(100,-100,0));
    TestTrue(TEXT("Execution can use lock"),A->RequestInput(TEXT("Execution")));
    TestTrue(TEXT("Explicit lock beats closer unrelated enemy"),A->GetPrimaryTarget()==Enemy);
    ASC->CancelAllAbilities();Run(.3f);
    Enemy->SetActorLocation(U->GetActorLocation()+FVector(1200,0,0));Other->SetActorLocation(U->GetActorLocation()+FVector(0,100,0));
    A->RequestInput(TEXT("Execution"));
    TestTrue(TEXT("Out-of-cast-range camera lock falls back to legal primary target"),A->GetPrimaryTarget()==Other);
    TestTrue(TEXT("Fallback facing agrees with actual damage target"),U->GetActorForwardVector().Y>.99f);
    ASC->CancelAllAbilities();Run(.3f);Enemy->SetActorLocation(U->GetActorLocation()+FVector(220,0,0));
    TestTrue(TEXT("Second middle-button command unlocks"),C->ToggleLock() && !C->GetLockedTarget());
    U->SetActorRotation(FRotator::ZeroRotator);C->ResetView();Run(.5f);
    Other->SetActorLocation(U->GetActorLocation()+FVector(300,250,0));C->ToggleLock();
    TestTrue(TEXT("Wheel command changes lock to screen-right enemy"),C->SwitchLock(1) && C->GetLockedTarget()==Other);
    Other->SetActorLocation(U->GetActorLocation()+FVector(4000,0,0));Run(.1f);
    TestNull(TEXT("Out-of-range target releases lock"),C->GetLockedTarget());
    U->SetActorRotation(FRotator::ZeroRotator);C->ResetView();Run(.5f);C->ToggleLock();
    Enemy->Destroy();Run(.1f);TestNull(TEXT("Destroyed target releases lock safely"),C->GetLockedTarget());
    auto* Wall=World->SpawnActor<AStaticMeshActor>();
    Wall->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
    Wall->GetStaticMeshComponent()->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube")));
    Wall->SetActorScale3D(FVector(.2,10,5));Wall->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
    U->SetActorRotation(FRotator::ZeroRotator);C->ResetView();Run(.5f);
    Wall->SetActorLocation(U->GetActorLocation()+FVector(-100,0,100));Run(.1f);
    TestTrue(TEXT("Camera collision retracts in front of wall"),C->GetComponentLocation().X>Wall->GetActorLocation().X+20);
    Wall->SetActorLocation(U->GetActorLocation()+FVector(100,0,0));Other->SetActorLocation(U->GetActorLocation()+FVector(300,0,0));Run(3.f);
    TestFalse(TEXT("Cannot acquire through occluder"),C->ToggleLock());
    Wall->SetActorLocation(U->GetActorLocation()+FVector(100,1800,0));Run(.1f);
    TestTrue(TEXT("Acquire after obstruction removed"),C->ToggleLock());
    Wall->SetActorLocation(U->GetActorLocation()+FVector(100,0,0));Run(.2f);
    TestNotNull(TEXT("Brief occlusion retains lock"),C->GetLockedTarget());Run(.5f);
    TestNull(TEXT("Sustained occlusion releases lock"),C->GetLockedTarget());Wall->Destroy();
    Reset();Other->SetActorLocation(FVector(4000,4000,90));U->SetActorRotation(FRotator::ZeroRotator);C->ResetView();Run(3.f);
    const FTransform BeforeR=C->GetComponentTransform();A->RequestInput(TEXT("SkillR"));Run(.3f);
    TestFalse(TEXT("Skill camera overlays follow camera"),C->GetComponentTransform().Equals(BeforeR));
    U->SetActorLocation(U->GetActorLocation()+FVector(500,0,0));ASC->CancelAllAbilities();Run(1.2f);
    TestTrue(TEXT("Cancel returns to moved character, not old camera snapshot"),C->GetComponentLocation().X>BeforeR.GetLocation().X+490);
    A->RequestInput(TEXT("SkillR"));Run(.3f);Battle->ExitBattle();Run(.1f);
    TestFalse(TEXT("Battle exit disables character camera input"),C->IsUsable());
    TestFalse(TEXT("Inactive camera cannot acquire"),C->ToggleLock());
    return true;
}
