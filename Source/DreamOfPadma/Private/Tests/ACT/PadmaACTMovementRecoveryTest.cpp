#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Gameplay/ACT/Runtime/PadmaACTActions.h"
#include "Gameplay/ACT/Runtime/PadmaACTMelee.h"
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaChenMovementRecoveryTest,"DreamOfPadma.ACT.ChenMovementRecovery",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaChenMovementRecoveryTest::RunTest(const FString& Parameters)
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
    FString Failure;
    if (!TestTrue(TEXT("Battle starts"),Battle->StartBattle(Setup,Failure))) { AddError(Failure);return false; }
    auto* U=Battle->GetPlayerUnit();auto* A=U->GetActions();auto* M=U->GetMelee();auto* ASC=U->GetAbilitySystemComponent();
    auto Run=[&](float Seconds){for(float Left=Seconds;Left>UE_SMALL_NUMBER;Left-=1.f/60){++GFrameCounter;World->Tick(LEVELTICK_All,FMath::Min(Left,1.f/60));}};
    auto Reset=[&](){A->Move(FVector::ZeroVector);ASC->CancelAllAbilities();U->StopJumping();U->GetCharacterMovement()->StopMovementImmediately();U->SetActorLocation(FVector(0,0,90));U->GetCharacterMovement()->SetMovementMode(MOVE_Walking);Run(.25f);};
    Run(1.5f);
    struct FCase { FName Input; float Recovery; };
    const FCase Cases[]={{TEXT("Primary"),19.f/30},{TEXT("SkillE"),32.f/30},{TEXT("SkillQ"),40.f/30},{TEXT("SkillR"),130.f/30},{TEXT("Execution"),50.f/30},{TEXT("Dodge"),6.f/30}};
    for (const auto& C:Cases)
    {
        Reset();TestTrue(TEXT("Action starts for held movement"),A->RequestInput(C.Input));
        const FName Id=A->GetActiveActionId();const float Length=M->GetAttackMontage()->GetPlayLength();
        A->Move(FVector(0,1,0));Run(C.Recovery-.08f);
        TestEqual(*FString::Printf(TEXT("%s movement preserves startup"),*C.Input.ToString()),A->GetActiveActionId(),Id);
        Run(.15f);
        TestTrue(*FString::Printf(TEXT("%s held movement cancels in recovery before clip end %.3f"),*C.Input.ToString(),Length),A->GetActiveActionId().IsNone() && C.Recovery+.07f<Length);
        const FVector Position=U->GetActorLocation();
        for(int I=0;I<20;++I){A->Move(FVector(0,1,0));Run(1.f/60);}
        TestTrue(TEXT("Movement reaches CMC after cancellation"),U->GetActorLocation().Y>Position.Y+15);
        TestFalse(TEXT("Cancelled hit windows cannot remain active"),M->IsAttacking());
        Reset();A->RequestInput(C.Input);A->Move(FVector(0,1,0));Run(.05f);A->Move(FVector::ZeroVector);Run(C.Recovery+.03f);
        TestEqual(TEXT("Released axis does not cancel later"),A->GetActiveActionId(),Id);
    }
    // Skill jumps buffer briefly before recovery and execute without waiting for full animation completion.
    for (const auto& C:Cases)
    {
        if (C.Input==TEXT("Primary") || C.Input==TEXT("Dodge")) continue;
        Reset();A->RequestInput(C.Input);Run(C.Recovery-.04f);
        TestTrue(TEXT("Jump buffers at recovery boundary"),A->RequestInput(TEXT("Jump")));Run(.10f);
        TestEqual(TEXT("Jump consumes recovery buffer"),A->GetActiveActionId(),FName(TEXT("Chen.Jump")));
        TestTrue(TEXT("Jump moves capsule into air"),U->GetCharacterMovement()->IsFalling());
    }
    Reset();A->RequestInput(TEXT("SkillE"));Run(.1f);A->RequestInput(TEXT("Jump"));Run(1.15f);
    TestEqual(TEXT("Expired early jump cannot fire late"),A->GetActiveActionId(),FName(TEXT("Chen.GuiQiongYu")));
    for (const FName Input:{FName(TEXT("Primary")),FName(TEXT("Dodge"))})
    {
        Reset();A->RequestInput(Input);Run(.05f);A->RequestInput(TEXT("Jump"));Run(.05f);
        TestEqual(TEXT("Attack/dodge use their source jump edge before generic recovery"),A->GetActiveActionId(),FName(TEXT("Chen.Jump")));
    }
    Reset();A->RequestInput(TEXT("Primary"));Run(.40f);A->Move(FVector(1,0,0));A->RequestInput(TEXT("Primary"));Run(.15f);
    TestEqual(TEXT("Held movement does not steal buffered combo"),A->GetActiveActionId(),FName(TEXT("Chen.Attack02")));
    Reset();A->RequestInput(TEXT("Dodge"));Run(.04f);TestTrue(TEXT("Perfect dodge fixture evades"),A->TryEvade());Run(.02f);
    A->RequestInput(TEXT("Jump"));Run(.13f);
    TestEqual(TEXT("Perfect dodge protects startup and early jump expires"),A->GetActiveActionId(),FName(TEXT("Chen.PerfectDodge")));
    A->RequestInput(TEXT("Jump"));Run(.10f);
    TestEqual(TEXT("Perfect dodge jump uses its recovery boundary"),A->GetActiveActionId(),FName(TEXT("Chen.Jump")));
    Reset();A->RequestInput(TEXT("ToggleRun"));
    for(int I=0;I<30;++I){A->Move(FVector(1,0,0));Run(1.f/60);}
    const float Speed=U->GetVelocity().Size2D();A->RequestInput(TEXT("Jump"));
    TestTrue(TEXT("Jump preserves running momentum"),U->GetVelocity().Size2D()>=Speed-1.f);
    Run(.15f);const FVector AirStart=U->GetActorLocation();
    for(int I=0;I<20;++I){A->Move(FVector(0,1,0));Run(1.f/60);}
    TestTrue(TEXT("WASD steers while jump ability is active"),U->GetActorLocation().Y>AirStart.Y+5.f && U->GetCharacterMovement()->IsFalling());
    for(int I=0;I<180 && U->GetCharacterMovement()->IsFalling();++I){A->Move(FVector(0,1,0));Run(1.f/60);}
    Run(.06f);TestTrue(TEXT("Held movement exits landing pose without full clip wait"),A->GetActiveActionId().IsNone());
    TestEqual(TEXT("Landing restores selected run speed"),U->GetCharacterMovement()->MaxWalkSpeed,D->RunSpeed);
    ASC->CancelAllAbilities();return true;
}
