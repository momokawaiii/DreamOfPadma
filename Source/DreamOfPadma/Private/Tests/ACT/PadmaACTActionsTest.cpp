#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Gameplay/ACT/Runtime/PadmaACTActions.h"
#include "Gameplay/ACT/Runtime/PadmaACTMelee.h"
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaChenActionsTest,"DreamOfPadma.ACT.ChenCharacterActions",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaChenActionsTest::RunTest(const FString& Parameters)
{
    auto* D=LoadObject<UPadmaACTCharacterDefinition>(nullptr,TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/AbilitySystem/DA_ACTCharacter_CHEN"));
    FString Failure;
    if (!TestNotNull(TEXT("Saved character definition"),D) || !TestTrue(TEXT("Complete action catalog validates"),UPadmaACTActionsComponent::ValidateDefinition(D,Failure))) { AddError(Failure); return false; }
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
    auto Cancel=[&](){ASC->CancelAllAbilities();Run(.15);};
    Run(1.5f); // CMC defers initial falling for the first second while an editor world starts.
    TestEqual(TEXT("Authored BP spawned"),U->GetClass(),D->CharacterClass.LoadSynchronous());
    TestEqual(TEXT("User ABP remains active"),U->GetMesh()->GetAnimInstance()->GetClass(),D->AnimationClass.LoadSynchronous());
    TestTrue(TEXT("Grounded capsule"),!U->GetCharacterMovement()->IsFalling());
    const FVector Start=U->GetActorLocation();
    for(int I=0;I<30;++I){A->Move(FVector(1,0,0));Step(1.f/60);}
    TestTrue(TEXT("CharacterMovement moves with collision"),U->GetActorLocation().X>Start.X+30);
    AddInfo(FString::Printf(TEXT("Motor start %s end %s velocity %s capsule %.1f/%.1f mode %d floor %s"),*Start.ToString(),*U->GetActorLocation().ToString(),*U->GetVelocity().ToString(),U->GetCapsuleComponent()->GetUnscaledCapsuleRadius(),U->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(),int(U->GetCharacterMovement()->MovementMode),*GetNameSafe(U->GetCharacterMovement()->CurrentFloor.HitResult.GetActor())));
    auto* Speed=FindFProperty<FDoubleProperty>(U->GetMesh()->GetAnimInstance()->GetClass(),TEXT("GroundSpeed"));
    TestTrue(TEXT("ABP receives real speed"),Speed && Speed->GetPropertyValue_InContainer(U->GetMesh()->GetAnimInstance())>5);
    A->Move(FVector::ZeroVector);Run(.3);
    auto* Anim=U->GetMesh()->GetAnimInstance();
    const int32 LocomotionIndex=Anim->GetStateMachineIndex(TEXT("SM_Locomotion"));
    TestTrue(TEXT("Ctrl binding toggles run"),A->RequestInput(TEXT("ToggleRun")) && A->WantsToRun());
    for(int I=0;I<45;++I){A->Move(FVector(1,0,0));Step(1.f/60);}
    TestTrue(TEXT("Run uses authored speed"),FMath::IsNearlyEqual(U->GetVelocity().Size2D(),D->RunSpeed,1.f));
    TestEqual(TEXT("Actual ABP enters Run"),Anim->GetCurrentStateName(LocomotionIndex),FName(TEXT("Run")));
    A->RequestInput(TEXT("ToggleRun"));
    for(int I=0;I<30;++I){A->Move(FVector(1,0,0));Step(1.f/60);}
    TestTrue(TEXT("Moving Run transitions back to Walk"),Anim->GetCurrentStateName(LocomotionIndex)!=FName(TEXT("Run")) && FMath::IsNearlyEqual(U->GetVelocity().Size2D(),D->WalkSpeed,1.f));
    A->RequestInput(TEXT("ToggleRun"));
    for(int I=0;I<30;++I){A->Move(FVector(1,0,0));Step(1.f/60);}
    TestEqual(TEXT("Moving Walk transitions back to Run"),Anim->GetCurrentStateName(LocomotionIndex),FName(TEXT("Run")));
    A->Move(FVector::ZeroVector);Run(.5f);
    TestTrue(TEXT("Stopped run returns to idle"),Anim->GetCurrentStateName(LocomotionIndex)!=FName(TEXT("Run")));
    Enemy->SetActorLocation(FVector(3000,2000,90));Enemy2->SetActorLocation(FVector(3500,2000,90));
    U->SetActorLocation(FVector(0,0,90));U->SetActorRotation(FRotator::ZeroRotator);Run(.1f);
    const FVector RootStart=U->GetActorLocation();
    TestTrue(TEXT("Root-motion E starts"),A->RequestInput(TEXT("SkillE")));Run(M->GetAttackMontage()->GetPlayLength()+.5f);
    const FVector RootEnd=U->GetActorLocation();
    TestTrue(TEXT("E root motion moves capsule forward"),RootEnd.X>RootStart.X+200.f);
    TestTrue(TEXT("E ends naturally"),A->GetActiveActionId().IsNone());
    Run(.5f);
    TestTrue(TEXT("Skill completion keeps reached location"),FVector::Dist2D(U->GetActorLocation(),RootEnd)<2.f);
    TestEqual(TEXT("Action restores selected run speed"),U->GetCharacterMovement()->MaxWalkSpeed,D->RunSpeed);
    const FVector QStart=U->GetActorLocation();
    TestTrue(TEXT("Root-motion Q starts"),A->RequestInput(TEXT("SkillQ")));Run(M->GetAttackMontage()->GetPlayLength()+.5f);
    const FVector QEnd=U->GetActorLocation();Run(.5f);
    TestTrue(TEXT("Q preserves capsule displacement"),QEnd.X>QStart.X+450.f && FVector::Dist2D(QEnd,U->GetActorLocation())<2.f);
    A->RequestInput(TEXT("SkillE"));Run(.35f);Cancel();
    const FVector CancelEnd=U->GetActorLocation();Run(.5f);
    TestTrue(TEXT("Interrupted skill keeps current capsule position"),FVector::Dist2D(CancelEnd,U->GetActorLocation())<2.f);
    A->RequestInput(TEXT("Dodge"));Run(.1f);
    A->RequestInput(TEXT("ToggleRun"));
    TestTrue(TEXT("Toggle during dodge preserves dodge motor speed"),U->GetCharacterMovement()->MaxWalkSpeed>D->WalkSpeed);
    Cancel();TestEqual(TEXT("Dodge end applies newly selected walk speed"),U->GetCharacterMovement()->MaxWalkSpeed,D->WalkSpeed);
    A->RequestInput(TEXT("ToggleRun"));
    auto* Wall=World->SpawnActor<AStaticMeshActor>();
    Wall->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
    Wall->GetStaticMeshComponent()->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube")));
    Wall->SetActorScale3D(FVector(.2,10,4));
    Wall->SetActorLocation(U->GetActorLocation()+FVector(150,0,0));
    Wall->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
    const float WallX=Wall->GetActorLocation().X;
    A->RequestInput(TEXT("SkillE"));Run(M->GetAttackMontage()->GetPlayLength()+.5f);
    TestTrue(TEXT("Root motion respects capsule blocking wall"),U->GetActorLocation().X<WallX-20.f);
    Wall->Destroy();
    TestTrue(TEXT("Ctrl toggles back to walk"),A->RequestInput(TEXT("ToggleRun")) && !A->WantsToRun());
    TestEqual(TEXT("Walk speed restored"),U->GetCharacterMovement()->MaxWalkSpeed,D->WalkSpeed);
    U->SetActorLocation(FVector(0,0,90));Enemy->SetActorLocation(FVector(1000,0,90));Enemy2->SetActorLocation(FVector(1500,0,90));
    AddInfo(FString::Printf(TEXT("Before attack %s velocity %s mode %d"),*U->GetActorLocation().ToString(),*U->GetVelocity().ToString(),int(U->GetCharacterMovement()->MovementMode)));
    TestTrue(TEXT("First attack activates through GAS"),A->RequestInput(TEXT("Primary")));
    TestTrue(TEXT("Same-frame input enters source cache"),A->RequestInput(TEXT("Primary")));
    Run(.02f);
    TestTrue(TEXT("Early input enters source cache"),A->RequestInput(TEXT("Primary")));
    Run(.3f);
    TestEqual(TEXT("Expired .15 second input does not chain"),A->GetActiveActionId(),FName(TEXT("Chen.Attack01")));
    Run(.07f);TestTrue(TEXT("Input shortly before window is buffered"),A->RequestInput(TEXT("Primary")));Run(.13f);
    TestEqual(TEXT("14/30 source gate consumes cached combo"),A->GetActiveActionId(),FName(TEXT("Chen.Attack02")));
    for(int Index=3;Index<=5;++Index)
    {
        for(int I=0;I<90 && !M->IsComboWindowOpen();++I)Step(1.f/60);
        TestTrue(TEXT("ANS permits next attack"),A->RequestInput(TEXT("Primary")));
        TestEqual(TEXT("Correct combo definition"),A->GetActiveActionId(),FName(*FString::Printf(TEXT("Chen.Attack%02d"),Index)));
        Run(.02f);
    }
    for(int I=0;I<90 && !M->IsComboWindowOpen();++I)Step(1.f/60);
    TestTrue(TEXT("Source fifth attack loops to first"),A->RequestInput(TEXT("Primary")));
    TestEqual(TEXT("Loop resolves first"),A->GetActiveActionId(),FName(TEXT("Chen.Attack01")));Cancel();
    A->RequestInput(TEXT("Primary"));Run(.02f);A->RequestInput(TEXT("SkillE"));
    TestEqual(TEXT("E interrupts exclusive basic"),A->GetActiveActionId(),FName(TEXT("Chen.GuiQiongYu")));
    A->RequestInput(TEXT("SkillQ"));TestEqual(TEXT("Q interrupts E"),A->GetActiveActionId(),FName(TEXT("Chen.JianTianHe")));
    A->RequestInput(TEXT("Dodge"));TestEqual(TEXT("Dodge interrupts Q"),A->GetActiveActionId(),FName(TEXT("Chen.Dodge")));
    A->RequestInput(TEXT("SkillR"));TestEqual(TEXT("R interrupts dodge"),A->GetActiveActionId(),FName(TEXT("Chen.LieFengShuang")));
    Run(.02f);A->RequestInput(TEXT("Dodge"));Run(.3f);
    TestEqual(TEXT("Dodge cannot cancel R exclusive; expired request discarded"),A->GetActiveActionId(),FName(TEXT("Chen.LieFengShuang")));Cancel();
    A->RequestInput(TEXT("SkillQ"));Run(.02f);A->RequestInput(TEXT("SkillE"));Run(.4f);
    TestEqual(TEXT("Lower E request expires before Q whitelist"),A->GetActiveActionId(),FName(TEXT("Chen.JianTianHe")));
    Run(.27f);A->RequestInput(TEXT("SkillE"));Run(.13f);
    TestEqual(TEXT("Q allow-next window opens E before exclusive ends"),A->GetActiveActionId(),FName(TEXT("Chen.GuiQiongYu")));Cancel();
    for (FName Binding:{FName(TEXT("SkillE")),FName(TEXT("SkillQ")),FName(TEXT("SkillR"))})
    {
        TestTrue(TEXT("Skill input activates"),A->RequestInput(Binding)); Run(.2);
        TestTrue(TEXT("Skill has active GAS spec and Montage"),M->IsAttacking() && !A->GetActiveActionId().IsNone());
        U->GetMesh()->GetAnimInstance()->Montage_Stop(.02);Run(.1);
        TestTrue(TEXT("External interruption clears action"),A->GetActiveActionId().IsNone() && M->GetActiveWindowCount()==0);
    }
    auto* Jump=LoadObject<UPadmaACTSkillDefinition>(nullptr,TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/AbilitySystem/Abilities/DA_Chen_Jump"));
    const float OldCost=Jump->FlowCost;Jump->FlowCost=1;
    Battle->PayCost=[](float,float,FName,FString&){return false;};
    A->RequestInput(TEXT("Jump"));Run(.1);
    TestFalse(TEXT("Failed payment cannot jump"),U->GetCharacterMovement()->IsFalling());
    TestFalse(TEXT("Failed payment leaves no jump flag"),U->bPressedJump);
    Jump->FlowCost=OldCost;Battle->PayCost=[](float,float,FName,FString&){return true;};
    TestTrue(TEXT("Same-frame jump fixture starts"),A->RequestInput(TEXT("Jump")));ASC->CancelAllAbilities();Run(.1f);
    TestFalse(TEXT("Cancelled pre-physics jump clears flag"),U->bPressedJump);
    TestFalse(TEXT("Cancelled pre-physics jump stays grounded"),U->GetCharacterMovement()->IsFalling());
    TestTrue(TEXT("Normal jump starts"),A->RequestInput(TEXT("Jump")));Run(.7f);
    TestEqual(TEXT("Apex enters airborne loop"),U->GetMesh()->GetAnimInstance()->Montage_GetCurrentSection(M->GetAttackMontage()),FName(TEXT("Loop")));
    Run(3.f);
    TestTrue(TEXT("Normal landing finishes GAS and restores locomotion"),A->GetActiveActionId().IsNone() && !U->GetCharacterMovement()->IsFalling());
    TestTrue(TEXT("Space action accepted"),A->RequestInput(TEXT("Jump")));Run(.25);
    TestTrue(TEXT("Real jumping physics"),U->GetCharacterMovement()->IsFalling() && U->GetVelocity().Z>0);
    TestTrue(TEXT("Airborne primary activates plunge"),A->RequestInput(TEXT("Primary")));Run(.4);
    TestEqual(TEXT("Landed plunge section"),U->GetMesh()->GetAnimInstance()->Montage_GetCurrentSection(M->GetAttackMontage()),FName(TEXT("Land")));
    TestFalse(TEXT("Plunge returned to grounded physics"),U->GetCharacterMovement()->IsFalling());Cancel();
    Enemy->SetActorLocation(U->GetActorLocation()+FVector(2500,2000,0));Enemy2->SetActorLocation(U->GetActorLocation()+FVector(3000,2000,0));
    const float Healthy=Enemy->Health();
    auto* Execution=LoadObject<UPadmaACTSkillDefinition>(nullptr,TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/AbilitySystem/Abilities/DA_Chen_Execution"));
    TestEqual(TEXT("Execution uses regular test damage scale"),Execution->DamageScale,1.f);
    TestTrue(TEXT("F activates with no target in range"),A->RequestInput(TEXT("Execution")));Run(1.2f);
    TestEqual(TEXT("No target execution whiffs"),Enemy->Health(),Healthy);Cancel();
    Enemy->SetActorLocation(U->GetActorLocation()+FVector(120,0,0));
    TestTrue(TEXT("F activates against full health target"),A->RequestInput(TEXT("Execution")));Run(1.3f);
    TestTrue(TEXT("Execution direct target damage settles without HP gate"),Enemy->Health()<Healthy);Cancel();
    TestTrue(TEXT("Execution leaves target alive for remaining skills"),Enemy->IsAlive());
    Enemy->SetActorLocation(FVector(1000,0,90));
    // Source box is wider than blade path but excludes enemies outside its authored volume.
    U->SetActorRotation(FRotator::ZeroRotator);
    Enemy->SetActorLocation(U->GetActorLocation()+FVector(170,40,0));
    Enemy2->SetActorLocation(U->GetActorLocation()+FVector(350,0,0));
    const float InHP=Enemy->Health(),OutHP=Enemy2->Health();
    A->RequestInput(TEXT("Primary"));Run(.42f);
    TestTrue(TEXT("Original box hits off-blade target"),Enemy->Health()<InHP);
    TestEqual(TEXT("Original box excludes distant target"),Enemy2->Health(),OutHP);Cancel();
    Enemy->SetActorLocation(FVector(1000,0,90));Enemy2->SetActorLocation(FVector(1500,0,90));
    auto* Skill=LoadObject<UPadmaACTSkillDefinition>(nullptr,TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/AbilitySystem/Abilities/DA_Chen_GuiQiongYu"));
    const float OldCooldown=Skill->CooldownSeconds;Skill->CooldownSeconds=.8;
    TestTrue(TEXT("Cooldown action starts"),A->RequestInput(TEXT("SkillE")));Cancel();
    TestTrue(TEXT("Cooldown is a live GAS effect"),A->IsCoolingDown(Skill->DefinitionId));
    TestFalse(TEXT("Cooldown rejects restart"),A->RequestInput(TEXT("SkillE")));Run(.8);
    TestFalse(TEXT("GAS cooldown expires"),A->IsCoolingDown(Skill->DefinitionId));Skill->CooldownSeconds=OldCooldown;
    TestTrue(TEXT("Dodge gate fixture"),A->RequestInput(TEXT("Dodge")));
    TestFalse(TEXT("Same-frame dodge blocks attack input"),A->RequestInput(TEXT("Primary")));
    Run(.12f);TestTrue(TEXT("Dodge uses extended attack cache"),A->RequestInput(TEXT("Primary")));
    Run(.17f);TestEqual(TEXT("Dodge attack still blocked before .35"),A->GetActiveActionId(),FName(TEXT("Chen.Dodge")));
    Run(.1f);TestEqual(TEXT("Dodge cached attack starts after .35"),A->GetActiveActionId(),FName(TEXT("Chen.Attack01")));Cancel();
    A->RequestInput(TEXT("Dodge"));Run(.04f);TestTrue(TEXT("Early perfect fixture receives contact"),A->TryEvade());Run(.02f);
    Run(.12f);A->RequestInput(TEXT("Primary"));Run(.15f);
    TestEqual(TEXT("Early perfect replaces longer normal attack-entry timer"),A->GetActiveActionId(),FName(TEXT("Chen.Attack01")));Cancel();
    TestTrue(TEXT("Dodge starts"),A->RequestInput(TEXT("Dodge")));Run(.08);
    Enemy->SetActorLocation(U->GetActorLocation()+FVector(45,0,0));Enemy->WindupRemaining=.005;Enemy->AttackCooldown=10;
    const float HP=U->Health();Battle->SetComponentTickEnabled(true);Step(.01);Battle->SetComponentTickEnabled(false);Run(.04);
    TestEqual(TEXT("Perfect dodge prevented actual attack damage"),U->Health(),HP);
    TestEqual(TEXT("Confirmed perfect dodge switches variant"),A->GetActiveActionId(),FName(TEXT("Chen.PerfectDodge")));
    TestEqual(TEXT("Each contact counts perfect dodge once"),A->GetPerfectDodgeCount(),2);
    A->RequestInput(TEXT("Dodge"));Run(.3f);
    TestEqual(TEXT("Perfect blocks dash and short request expires"),A->GetActiveActionId(),FName(TEXT("Chen.PerfectDodge")));
    A->RequestInput(TEXT("SkillE"));A->RequestInput(TEXT("Dodge"));Run(.1f);
    TestEqual(TEXT("Perfect dash block survives switching into E"),A->GetActiveActionId(),FName(TEXT("Chen.GuiQiongYu")));
    A->RequestInput(TEXT("Dodge"));Run(.15f);
    TestEqual(TEXT("Perfect permits cached dash after .5"),A->GetActiveActionId(),FName(TEXT("Chen.Dodge")));Cancel();
    A->RequestInput(TEXT("SkillE"));Run(.86f);A->RequestInput(TEXT("SkillE"));Run(.4f);
    TestEqual(TEXT("Same Montage recast remains active"),A->GetActiveActionId(),FName(TEXT("Chen.GuiQiongYu")));
    Run(.46f);TestTrue(TEXT("New same-Montage whitelist survives outgoing notify ends"),A->RequestInput(TEXT("SkillE")));
    TestTrue(TEXT("Same-Montage restarts at beginning"),M->GetAttackTime()<.05f);Cancel();
    U->SetActorRotation(FRotator::ZeroRotator);
    Enemy->SetActorLocation(U->GetActorLocation()+FVector(400,0,0));Enemy2->SetActorLocation(U->GetActorLocation()+FVector(410,0,0));
    int Hits=0;
    const FDelegateHandle Receipt=Battle->OnReceipt.AddLambda([&](const FPadmaCombatReceipt& R){if(R.SourceId==U->Spec.Id && R.Effect==TEXT("damage")){++Hits;A->RequestInput(TEXT("Dodge"));}});
    TestTrue(TEXT("Reentrant fixture starts"),A->RequestInput(TEXT("SkillE")));Run(.7);
    Battle->OnReceipt.Remove(Receipt);
    TestEqual(TEXT("Old window cannot damage second target after new action"),Hits,1);Cancel();
    Enemy->SetActorLocation(FVector(1000,0,90));Enemy2->SetActorLocation(FVector(1500,0,90));
    const FVector MoveStart=U->GetActorLocation();Battle->SetComponentTickEnabled(true);
    Enemy->Spec.bCanPursueInACT=false;
    const FVector DummyStart=Enemy->GetActorLocation(),MobileEnemyStart=Enemy2->GetActorLocation();
    TestTrue(TEXT("Existing click movement accepted"),Battle->MoveTo(MoveStart+FVector(100,0,0)));Run(.8);Battle->SetComponentTickEnabled(false);
    TestTrue(TEXT("Click movement reaches configured motor"),U->GetActorLocation().X>MoveStart.X+50);
    TestTrue(TEXT("Stationary dummy never pursues moving player"),Enemy->GetActorLocation().Equals(DummyStart,.01f));
    TestTrue(TEXT("Ordinary enemies retain pursuit"),!Enemy2->GetActorLocation().Equals(MobileEnemyStart,.01f));
    A->RequestInput(TEXT("SkillR"));Battle->ExitBattle();
    TestFalse(TEXT("Exit ends battle"),Battle->IsBattleActive());
    return true;
}
