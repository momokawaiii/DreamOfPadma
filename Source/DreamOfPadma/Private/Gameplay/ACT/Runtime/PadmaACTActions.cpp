#include "Gameplay/ACT/Runtime/PadmaACTActions.h"
#include "Gameplay/ACT/Runtime/PadmaACTMelee.h"
#include "Gameplay/ACT/Runtime/PadmaACTCamera.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Gameplay/ACT/Runtime/PadmaACTAnimNotifies.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "UObject/UnrealType.h"

namespace { APadmaCombatUnit* OwnerUnit(const UActorComponent* C) { return Cast<APadmaCombatUnit>(C->GetOwner()); } }
UPadmaACTActionsComponent::UPadmaACTActionsComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;
}
bool UPadmaACTActionsComponent::ValidateDefinition(UPadmaACTCharacterDefinition* D, FString& Failure)
{
    auto Fail=[&](const TCHAR* Text){ Failure=Text; return false; };
    auto* Table=D ? D->SkillTable.LoadSynchronous() : nullptr;
    auto* Mesh=D ? D->Model.LoadSynchronous() : nullptr;
    if (!D || !D->bUseCharacterActions || !Mesh || !Table || Table->GetRowStruct()!=FPadmaACTSkillRow::StaticStruct()
        || Table->GetRowMap().IsEmpty() || !D->AnimationClass.LoadSynchronous() || !D->MeleeProfile.LoadSynchronous()
        || !FMath::IsFinite(D->RunSpeed) || D->RunSpeed<=0 || !FMath::IsFinite(D->WalkSpeed) || D->WalkSpeed<=0 || !FMath::IsFinite(D->JumpSpeed) || D->JumpSpeed<=0
        || !FMath::IsFinite(D->CapsuleRadius) || D->CapsuleRadius<=0 || !FMath::IsFinite(D->CapsuleHalfHeight) || D->CapsuleHalfHeight<D->CapsuleRadius)
        return Fail(TEXT("Character action definition requires mesh, ABP, equipment, movement and skill table."));
    TMap<FName,const UPadmaACTSkillDefinition*> Skills; TSet<FName> Inputs;
    for (const auto& Pair : Table->GetRowMap())
    {
        const auto& Row=*reinterpret_cast<const FPadmaACTSkillRow*>(Pair.Value);
        auto* S=Row.Definition.LoadSynchronous();
        if (!S || S->DefinitionId!=Row.SkillId || Row.SkillId.IsNone() || Row.ActivationBindingId.IsNone()
            || Skills.Contains(Row.SkillId) || Inputs.Contains(Row.ActivationBindingId)
            || S->AbilityImplementationId!=TEXT("Native.ACT.CharacterAction") || S->ActionKind==EPadmaACTActionKind::Legacy)
            return Fail(TEXT("Invalid or duplicate native character action binding."));
        for (float V : {S->DamageScale,S->CooldownSeconds,S->FlowCost,S->CalculationCost,S->DodgeSpeed,S->PlungeSpeed,S->MinPlungeHeight,S->InputCacheSeconds})
            if (!FMath::IsFinite(V) || V<0) return Fail(TEXT("Action magnitudes must be finite and nonnegative."));
        if (!FMath::IsFinite(S->PlayRate) || S->PlayRate<.1f || S->PlayRate>4.f
            || !FMath::IsFinite(S->TargetRange) || S->TargetRange<=0) return Fail(TEXT("Invalid action rate or targeting policy."));
        if (S->ActionKind!=EPadmaACTActionKind::Legacy)
        {
            auto* M=S->Montage.LoadSynchronous();
            if (!M || M->GetSkeleton()!=Mesh->GetSkeleton() || (!S->StartSection.IsNone() && M->GetSectionIndex(S->StartSection)==INDEX_NONE))
                return Fail(TEXT("Action montage/section/skeleton is invalid."));
            if ((S->ActionKind==EPadmaACTActionKind::Plunge || S->ActionKind==EPadmaACTActionKind::Jump) && (M->GetSectionIndex(TEXT("Loop"))==INDEX_NONE || M->GetSectionIndex(TEXT("Land"))==INDEX_NONE))
                return Fail(TEXT("Plunge montage requires Loop and Land sections."));
            if (S->ActionKind==EPadmaACTActionKind::Plunge && S->bPlungeHoldUntilNotify
                && !M->Notifies.ContainsByPredicate([](const FAnimNotifyEvent& E){return Cast<UAnimNotify_PadmaACTPlungeDescend>(E.Notify)!=nullptr;}))
                return Fail(TEXT("Suspended plunge requires a Descend notify before enabling its movement override."));
        }
        Skills.Add(Row.SkillId,S); Inputs.Add(Row.ActivationBindingId);
    }
    for (const auto& Pair:Skills)
    {
        const auto* S=Pair.Value;
        if (!S->NextComboId.IsNone() && (!Skills.Contains(S->NextComboId) || Skills[S->NextComboId]->ActionKind!=EPadmaACTActionKind::Attack))
            return Fail(TEXT("Combo edge must resolve to an attack in this character table."));
        if (!S->PerfectDodgeId.IsNone() && (!Skills.Contains(S->PerfectDodgeId) || Skills[S->PerfectDodgeId]->ActionKind!=EPadmaACTActionKind::PerfectDodge))
            return Fail(TEXT("Perfect dodge edge must resolve to a perfect dodge action."));
    }
    Failure.Reset(); return true;
}
bool UPadmaACTActionsComponent::Configure(UPadmaACTCharacterDefinition* D)
{
    FString Failure; auto* U=OwnerUnit(this);
    if (Character || !U || !ValidateDefinition(D,Failure)) return false;
    Character=D;
    for (const auto& Pair:D->SkillTable.LoadSynchronous()->GetRowMap())
    {
        const auto& Row=*reinterpret_cast<const FPadmaACTSkillRow*>(Pair.Value);
        auto* Skill=Row.Definition.LoadSynchronous(); Definitions.Add(Row.SkillId,Skill); Bindings.Add(Row.ActivationBindingId,Row.SkillId);
        Grants.Add(Row.SkillId,U->GetAbilitySystemComponent()->GiveAbility(FGameplayAbilitySpec(UPadmaACTActionAbility::StaticClass(),1,INDEX_NONE,Skill)));
    }
    auto* Motor=U->GetCharacterMovement();
    if (!U->PreservesScenePresentation()) U->GetCapsuleComponent()->SetCapsuleSize(D->CapsuleRadius,D->CapsuleHalfHeight);
    Motor->SetUpdatedComponent(U->GetCapsuleComponent());
    Motor->bRunPhysicsWithNoController=true; Motor->MaxWalkSpeed=D->WalkSpeed; Motor->JumpZVelocity=D->JumpSpeed;
    Motor->bOrientRotationToMovement=true; Motor->RotationRate=FRotator(0,720,0);
    Motor->SetComponentTickEnabled(true); Motor->Activate(); Motor->SetMovementMode(MOVE_Walking);
    U->SetActorTickEnabled(true);
    U->bUseControllerRotationYaw=false;
    U->GetMesh()->AddTickPrerequisiteComponent(this);
    AddTickPrerequisiteComponent(Motor);
    return true;
}
bool UPadmaACTActionsComponent::IsCoolingDown(FName Id) const
{
    const auto* Handle=Cooldowns.Find(Id); auto* U=OwnerUnit(this);
    return Handle && U && U->GetAbilitySystemComponent()->GetActiveGameplayEffect(*Handle)!=nullptr;
}
void UPadmaACTActionsComponent::SetCooldown(FName Id,FActiveGameplayEffectHandle Handle) { Cooldowns.Add(Id,Handle); }
bool UPadmaACTActionsComponent::CanStart(const UPadmaACTSkillDefinition* D) const
{
    auto* U=OwnerUnit(this); auto* B=U ? U->GetBattle() : nullptr;
    if (!Character || !D || !U || !U->IsAlive() || !B || !B->IsBattleActive() || B->GetMode()!=EPadmaCombatMode::ACT
        || B->IsSkillLibraryOpen() || IsCoolingDown(D->DefinitionId)) return false;
    const bool Air=U->GetCharacterMovement()->IsFalling();
    if (D->ActionKind==EPadmaACTActionKind::Plunge)
    {
        if (!Air) return false;
        if (D->MinPlungeHeight<=0) return true;
        const FVector Foot=U->GetActorLocation()-FVector(0,0,U->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
        FCollisionObjectQueryParams Objects; Objects.AddObjectTypesToQuery(ECC_WorldStatic); Objects.AddObjectTypesToQuery(ECC_WorldDynamic);
        FCollisionQueryParams Params(SCENE_QUERY_STAT(ACTPlungeClearance),false,U);
        FHitResult Ground;
        const bool Hit=GetWorld()->LineTraceSingleByObjectType(Ground,Foot,Foot-FVector(0,0,D->MinPlungeHeight),Objects,Params);
        return !Hit || (!Ground.bStartPenetrating && Ground.Distance>=D->MinPlungeHeight-UE_KINDA_SMALL_NUMBER);
    }
    if (Air) return false;
    return true;
}
bool UPadmaACTActionsComponent::ActivateActionId(FName Id,bool Internal)
{
    if (bChangingAction) return false;
    auto* D=Definitions.FindRef(Id).Get(); auto* U=OwnerUnit(this);
    if (!CanStart(D) || (!Internal && IsInputBlocked(Id))) return false;
    if (D->ActionKind==EPadmaACTActionKind::PerfectDodge && !Internal) return false;
    if (Active && !Internal && !CanTransition(D)) return false;
    TGuardValue<bool> Guard(bChangingAction,true);
    if (Active) U->GetAbilitySystemComponent()->CancelAbilityHandle(Grants.FindRef(Active->DefinitionId));
    return U->GetAbilitySystemComponent()->TryActivateAbility(Grants.FindRef(Id)) && Active==D;
}
bool UPadmaACTActionsComponent::RequestInput(FName Binding)
{
    auto* U=OwnerUnit(this);
    if (!U || !Character || IsGameplayInputLocked()) return false;
    if (Binding==TEXT("ToggleRun"))
    {
        bWantsToRun=!bWantsToRun;
        if (!Active || Active->ActionKind==EPadmaACTActionKind::Jump) U->GetCharacterMovement()->MaxWalkSpeed=LocomotionSpeed();
        UpdateAnimation();
        return true;
    }
    if (Binding==TEXT("Primary"))
    {
        if (U->GetCharacterMovement()->IsFalling()) Binding=TEXT("Plunge");
        else if (Active && !Active->NextComboId.IsNone())
        {
            const FName Next=Active->NextComboId;
            bool Mapped=false;
            const float Time=U->GetMelee()->GetAttackTime();
            for (const auto& Pair:Transitions)
                if (Pair.Value.Kind==EPadmaACTTransitionWindow::Cache && Time>=Pair.Value.Start && Time<Pair.Value.End && Pair.Value.Actions.Contains(Next)) Mapped=true;
            if (Mapped) return ActivateActionId(Next) || QueueAction(Next);
        }
    }
    const FName Id=Bindings.FindRef(Binding);
    return ActivateActionId(Id) || QueueAction(Id);
}
float UPadmaACTActionsComponent::LocomotionSpeed() const
{ return Character ? (bWantsToRun ? Character->RunSpeed : Character->WalkSpeed) : 0.f; }
void UPadmaACTActionsComponent::Move(FVector Direction)
{
    if (Direction.ContainsNaN()) return;
    if (IsGameplayInputLocked())
    {
        bMoveRequiresRelease=!Direction.IsNearlyZero();
        MoveDirection=FVector::ZeroVector;
        // BeginAction cleared prior user input. Do not consume the dodge motor's own pending input here.
        return;
    }
    if (bMoveRequiresRelease)
    {
        if (Direction.IsNearlyZero()) bMoveRequiresRelease=false;
        else return;
    }
    MoveDirection=FVector(Direction.X,Direction.Y,0).GetClampedToMaxSize(1);
    auto* U=OwnerUnit(this);
    if (Character && U && (!Active || Active->ActionKind==EPadmaACTActionKind::Jump) && U->IsAlive() && U->GetBattle() && U->GetBattle()->IsBattleActive() && !U->GetBattle()->IsSkillLibraryOpen())
        U->AddMovementInput(MoveDirection,1.f,true);
}
void UPadmaACTActionsComponent::ResetConfiguration()
{
    if (Active) EndAction(Active);
    RestorePlungeMovement();
    Character=nullptr; Definitions.Reset(); Bindings.Reset(); Grants.Reset(); Cooldowns.Reset();
    Transitions.Reset(); CarriedBlocks.Reset(); BufferedAction=PendingPerfect=NAME_None;
    PrimaryTarget.Reset(); MoveDirection=DodgeDirection=FVector::ZeroVector;
    BufferExpires=0; BufferWindowEnd=MAX_flt; PerfectDodgeCount=0;
    bDodgeWindow=bPerfectWindow=bChangingAction=bWantsToRun=bMoveRequiresRelease=false;
}
bool UPadmaACTActionsComponent::CanResumeLocomotion() const
{
    if (!Active) return false;
    if (IsGameplayInputLocked()) return false;
    if (Active->bWaitForAfterimagesBeforeLocomotion && OwnerUnit(this)->GetMelee()->HasLiveAfterimages()) return false;
    const float Time=OwnerUnit(this)->GetMelee()->GetAttackTime();
    const FName MoveId(TEXT("Locomotion.Move"));
    bool Allowed=false;
    for (const auto& Pair:Transitions)
    {
        const auto& W=Pair.Value;
        if (Time<W.Start || Time>=W.End) continue;
        if ((W.Kind==EPadmaACTTransitionWindow::Block || W.Kind==EPadmaACTTransitionWindow::BlockInput) && W.Actions.Contains(MoveId)) return false;
        Allowed |= W.Kind==EPadmaACTTransitionWindow::Recovery || (W.Kind==EPadmaACTTransitionWindow::Allow && W.Actions.Contains(MoveId));
    }
    return Allowed;
}
void UPadmaACTActionsComponent::TryResumeLocomotion()
{
    auto* U=OwnerUnit(this);
    if (!Character || !U || !Active || bChangingAction || MoveDirection.IsNearlyZero() || !U->IsAlive()
        || !U->GetBattle() || !U->GetBattle()->IsBattleActive() || U->GetBattle()->IsSkillLibraryOpen() || !CanResumeLocomotion()) return;
    // Resolve buffered discrete input before held axes. In particular, movement must not steal the next combo.
    if (!BufferedAction.IsNone() && GetWorld()->GetTimeSeconds()<BufferExpires && U->GetMelee()->GetAttackTime()<BufferWindowEnd) return;
    TGuardValue<bool> Guard(bChangingAction,true);
    U->GetAbilitySystemComponent()->CancelAbilityHandle(Grants.FindRef(Active->DefinitionId));
    if (!Active) U->AddMovementInput(MoveDirection,1.f,true);
}
bool UPadmaACTActionsComponent::BeginAction(UPadmaACTSkillDefinition* D)
{
    if (Active || !CanStart(D)) return false;
    auto* U=OwnerUnit(this); Active=D;
    PrimaryTarget.Reset();
    auto* Locked=U->GetACTCamera()->GetLockedTarget();
    if (Locked && FVector::Dist(U->GetActorLocation(),Locked->GetActorLocation())<=D->TargetRange) PrimaryTarget=Locked;
    if (D->bUsePrimaryTarget)
    {
        float Distance=D->TargetRange;
        for (APadmaCombatUnit* Target:U->GetBattle()->GetUnits())
        {
            if (Locked && PrimaryTarget==Locked) break;
            if (!IsValid(Target) || !Target->IsAlive() || Target->Spec.bPlayer==U->Spec.bPlayer) continue;
            const float Candidate=FVector::Dist(U->GetActorLocation(),Target->GetActorLocation());
            if (Candidate<Distance || (Candidate==Distance && (!PrimaryTarget.IsValid() || Target->Spec.Id.LexicalLess(PrimaryTarget->Spec.Id))))
            { PrimaryTarget=Target; Distance=Candidate; }
        }
        if (PrimaryTarget.IsValid())
        { auto Facing=(PrimaryTarget->GetActorLocation()-U->GetActorLocation()).Rotation(); Facing.Pitch=Facing.Roll=0; U->SetActorRotation(Facing); }
    }
    auto* FacingTarget=D->bUsePrimaryTarget && PrimaryTarget.IsValid() ? PrimaryTarget.Get() : Locked;
    if (FacingTarget && (D->ActionKind==EPadmaACTActionKind::Attack || D->ActionKind==EPadmaACTActionKind::Skill || D->ActionKind==EPadmaACTActionKind::Execution))
    { auto Facing=(FacingTarget->GetActorLocation()-U->GetActorLocation()).Rotation();Facing.Pitch=Facing.Roll=0;U->SetActorRotation(Facing); }
    DodgeDirection=MoveDirection.IsNearlyZero() ? -U->GetActorForwardVector() : MoveDirection.GetSafeNormal();
    if (D->bBlockAllInputUntilAfterimagesRetire)
    {
        bMoveRequiresRelease=!MoveDirection.IsNearlyZero();
        MoveDirection=FVector::ZeroVector; BufferedAction=NAME_None;
        U->ConsumeMovementInputVector(); U->StopJumping();
    }
    const bool Jump=D->ActionKind==EPadmaACTActionKind::Jump;
    U->GetCharacterMovement()->bOrientRotationToMovement=Jump;
    if (!Jump) U->GetCharacterMovement()->StopMovementImmediately();
    if (!U->GetMelee()->BeginAction(D->Montage.LoadSynchronous(),D->StartSection,D->DamageScale)) { Active=nullptr; return false; }
    // Prime time-zero policy from the Montage itself: multiple inputs may arrive before mesh evaluation.
    auto* Montage=D->Montage.Get();
    const int32 SectionIndex=Montage->GetSectionIndex(D->StartSection);
    const float StartTime=SectionIndex!=INDEX_NONE ? Montage->GetAnimCompositeSection(SectionIndex).GetTime() : 0.f;
    for (const auto& Event:Montage->Notifies)
        if (auto* Policy=Cast<UAnimNotifyState_PadmaACTTransitionWindow>(Event.NotifyStateClass);
            Policy && Event.GetTime()<=StartTime+UE_SMALL_NUMBER && Event.GetTime()+Event.GetDuration()>StartTime)
        {
            SetTransitionWindow(Policy,true,Policy->Kind,Policy->Actions,Event.GetTime(),Event.GetTime()+Event.GetDuration(),Policy->CacheSeconds);
            if (Policy->bCarryAcrossActions && Policy->Kind==EPadmaACTTransitionWindow::Block)
            {
                // Perfect dodge resets the same attack-entry timer, it does not stack with normal dodge.
                CarriedBlocks.RemoveAll([Policy](const FCarriedBlock& Block){return Block.Actions==Policy->Actions;});
                CarriedBlocks.Add({Policy->Actions,GetWorld()->GetTimeSeconds()+(Event.GetTime()+Event.GetDuration()-StartTime)/D->PlayRate});
            }
        }
    if (D->ActionKind==EPadmaACTActionKind::Jump) U->Jump();
    if (D->ActionKind==EPadmaACTActionKind::Plunge)
    {
        auto* Motor=U->GetCharacterMovement();
        if (D->bPlungeHoldUntilNotify)
        {
            SavedGravityScale=Motor->GravityScale; SavedAirControl=Motor->AirControl;
            bPlungeMovementOverride=true; bPlungeDescending=false;
            Motor->GravityScale=0; Motor->AirControl=0;
            Motor->PendingLaunchVelocity=FVector::ZeroVector;
            Motor->StopMovementImmediately(); U->ConsumeMovementInputVector(); U->StopJumping();
        }
        else U->LaunchCharacter(FVector(0,0,-D->PlungeSpeed),true,true);
    }
    return true;
}
void UPadmaACTActionsComponent::StartPlungeDescent()
{
    if (!Active || Active->ActionKind!=EPadmaACTActionKind::Plunge || !bPlungeMovementOverride || bPlungeDescending) return;
    auto* U=OwnerUnit(this);
    if (!U || !U->GetCharacterMovement()->IsFalling()) return;
    bPlungeDescending=true;
    U->GetCharacterMovement()->Velocity=FVector(0,0,-Active->PlungeSpeed);
    U->GetAbilitySystemComponent()->CurrentMontageJumpToSection(TEXT("Loop"));
}
void UPadmaACTActionsComponent::RestorePlungeMovement()
{
    if (!bPlungeMovementOverride) return;
    if (auto* U=OwnerUnit(this))
    {
        U->GetCharacterMovement()->GravityScale=SavedGravityScale;
        U->GetCharacterMovement()->AirControl=SavedAirControl;
        U->GetCharacterMovement()->PendingLaunchVelocity=FVector::ZeroVector;
    }
    bPlungeMovementOverride=bPlungeDescending=false;
}
void UPadmaACTActionsComponent::EndAction(const UPadmaACTSkillDefinition* D)
{
    if (Active!=D) return;
    auto* U=OwnerUnit(this);
    if (U && D && D->ActionKind==EPadmaACTActionKind::Jump) U->StopJumping();
    if (U && D && D->ActionKind==EPadmaACTActionKind::Plunge) U->GetCharacterMovement()->PendingLaunchVelocity=FVector::ZeroVector;
    RestorePlungeMovement();
    if (U && Character) { U->GetCharacterMovement()->MaxWalkSpeed=LocomotionSpeed(); U->GetCharacterMovement()->bOrientRotationToMovement=true; }
    Active=nullptr; PrimaryTarget.Reset(); Transitions.Reset(); BufferedAction=NAME_None; bDodgeWindow=bPerfectWindow=false; PendingPerfect=NAME_None;
}
void UPadmaACTActionsComponent::Landed()
{
    auto* U=OwnerUnit(this);
    RestorePlungeMovement();
    if (U) U->StopJumping();
    if (Active && (Active->ActionKind==EPadmaACTActionKind::Plunge || Active->ActionKind==EPadmaACTActionKind::Jump) && U)
        U->GetAbilitySystemComponent()->CurrentMontageJumpToSection(TEXT("Land"));
}
void UPadmaACTActionsComponent::SetDodgeWindow(bool Open,bool Perfect,float Start,float End)
{
    if (!Active || (Active->ActionKind!=EPadmaACTActionKind::Dodge && Active->ActionKind!=EPadmaACTActionKind::PerfectDodge)) return;
    bDodgeWindow=Open; bPerfectWindow=Open && Perfect; DodgeStart=Start; DodgeEnd=End;
}
bool UPadmaACTActionsComponent::TryEvade()
{
    auto* U=OwnerUnit(this);
    if (!Active || !bDodgeWindow || !U) return false;
    const float Time=U->GetMelee()->GetAttackTime();
    if (Time<DodgeStart || Time>=DodgeEnd) return false;
    if (bPerfectWindow && PendingPerfect.IsNone() && !Active->PerfectDodgeId.IsNone())
    { PendingPerfect=Active->PerfectDodgeId; ++PerfectDodgeCount; }
    return true;
}
bool UPadmaACTActionsComponent::AllowsContact(APadmaCombatUnit* Target) const
{
    return true; // Execution is currently an unconditional action; geometry still validates every hit.
}
void UPadmaACTActionsComponent::SetTransitionWindow(const void* Key,bool Open,EPadmaACTTransitionWindow Kind,const TArray<FName>& Actions,float Start,float End,float CacheSeconds)
{
    if (!Active) return;
    if (Open) Transitions.Add(Key,{Kind,Actions,Start,End,CacheSeconds});
    else Transitions.Remove(Key);
}
bool UPadmaACTActionsComponent::CanTransition(const UPadmaACTSkillDefinition* D) const
{
    if (!Active) return true;
    if (IsGameplayInputLocked()) return false;
    if (D->ActionKind==EPadmaACTActionKind::Jump && Active->bWaitForAfterimagesBeforeLocomotion && OwnerUnit(this)->GetMelee()->HasLiveAfterimages()) return false;
    const float Time=OwnerUnit(this)->GetMelee()->GetAttackTime();
    // Source controller carries dodge entry timers through Skill states; AttackToDash bypasses them.
    if (Active->ActionKind==EPadmaACTActionKind::Skill || Active->ActionKind==EPadmaACTActionKind::Dodge || Active->ActionKind==EPadmaACTActionKind::PerfectDodge)
        for (const auto& Block:CarriedBlocks)
            if (GetWorld()->GetTimeSeconds()<Block.Expires && Block.Actions.Contains(D->DefinitionId)) return false;
    for (const auto& Pair:Transitions)
        if (Pair.Value.Kind==EPadmaACTTransitionWindow::Block && Time>=Pair.Value.Start && Time<Pair.Value.End && Pair.Value.Actions.Contains(D->DefinitionId)) return false;
    if (D->bCanInterrupt || D->InterruptPriority>Active->InterruptPriority) return true;
    for (const auto& Pair:Transitions)
    {
        const auto& W=Pair.Value;
        if (Time<W.Start || Time>=W.End) continue;
        if (W.Kind==EPadmaACTTransitionWindow::Recovery || (W.Kind==EPadmaACTTransitionWindow::Allow && W.Actions.Contains(D->DefinitionId))) return true;
    }
    return false;
}
bool UPadmaACTActionsComponent::QueueAction(FName Id)
{
    auto* D=Definitions.FindRef(Id).Get();
    if (!Active || !CanStart(D) || bChangingAction || IsInputBlocked(Id)) return false;
    const float Time=OwnerUnit(this)->GetMelee()->GetAttackTime();
    for (const auto& Pair:Transitions)
    {
        const auto& W=Pair.Value;
        if (W.Kind!=EPadmaACTTransitionWindow::Cache || Time<W.Start || Time>=W.End || !W.Actions.Contains(Id)) continue;
        BufferedAction=Id;
        BufferWindowEnd=W.End;
        BufferExpires=GetWorld()->GetTimeSeconds()+(W.CacheSeconds>0 ? W.CacheSeconds : (W.End-Time)/Active->PlayRate);
        return true;
    }
    if (D->InputCacheSeconds>0)
    {
        BufferedAction=Id; BufferWindowEnd=MAX_flt; BufferExpires=GetWorld()->GetTimeSeconds()+D->InputCacheSeconds; return true;
    }
    return false;
}
bool UPadmaACTActionsComponent::IsInputBlocked(FName Id) const
{
    if (!Active) return false;
    if (IsGameplayInputLocked()) return true;
    const auto* Requested=Definitions.FindRef(Id).Get();
    if (Requested && Requested->ActionKind==EPadmaACTActionKind::Jump && Active->bWaitForAfterimagesBeforeLocomotion && OwnerUnit(this)->GetMelee()->HasLiveAfterimages()) return true;
    const float Time=OwnerUnit(this)->GetMelee()->GetAttackTime();
    for (const auto& Pair:Transitions)
        if (Pair.Value.Kind==EPadmaACTTransitionWindow::BlockInput && Time>=Pair.Value.Start && Time<Pair.Value.End && Pair.Value.Actions.Contains(Id)) return true;
    return false;
}
bool UPadmaACTActionsComponent::IsGameplayInputLocked() const
{
    if (!Active || !Active->bBlockAllInputUntilAfterimagesRetire) return false;
    auto* M=OwnerUnit(this)->GetMelee();
    if (M->HasLiveAfterimages()) return true;
    const float Time=M->GetAttackTime();
    for (const auto& Pair:Transitions)
    {
        const auto& W=Pair.Value;
        if (W.Kind==EPadmaACTTransitionWindow::BlockInput && W.Actions.Contains(TEXT("Input.All")) && Time>=W.Start && Time<W.End) return true;
    }
    return false;
}
void UPadmaACTActionsComponent::UpdateAnimation()
{
    auto* U=OwnerUnit(this); auto* Anim=U ? U->GetMesh()->GetAnimInstance() : nullptr;
    if (!Anim) return;
    if (auto* P=FindFProperty<FBoolProperty>(Anim->GetClass(),TEXT("bWantsToRun"))) P->SetPropertyValue_InContainer(Anim,bWantsToRun);
    if (auto* P=FindFProperty<FBoolProperty>(Anim->GetClass(),TEXT("bWeaponCombatIdle"))) P->SetPropertyValue_InContainer(Anim,U->GetMelee()->WantsWeaponCombatIdle());
    auto SetNumber=[&](FName Name,double Value)
    {
        if (auto* P=FindFProperty<FDoubleProperty>(Anim->GetClass(),Name)) P->SetPropertyValue_InContainer(Anim,Value);
        else if (auto* FloatProperty=FindFProperty<FFloatProperty>(Anim->GetClass(),Name)) FloatProperty->SetPropertyValue_InContainer(Anim,float(Value));
    };
    SetNumber(TEXT("GroundSpeed"),U->GetVelocity().Size2D()); SetNumber(TEXT("VerticalSpeed"),U->GetVelocity().Z);
    if (auto* P=FindFProperty<FBoolProperty>(Anim->GetClass(),TEXT("bIsInAir"))) P->SetPropertyValue_InContainer(Anim,U->GetCharacterMovement()->IsFalling());
}
void UPadmaACTActionsComponent::TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Tick)
{
    Super::TickComponent(Delta,Type,Tick);
    if (!Character) return;
    auto* U=OwnerUnit(this);
    CarriedBlocks.RemoveAll([this](const FCarriedBlock& Block){return GetWorld()->GetTimeSeconds()>=Block.Expires;});
    if (!PendingPerfect.IsNone()) { const FName Id=PendingPerfect; PendingPerfect=NAME_None; ActivateActionId(Id,true); }
    if (IsGameplayInputLocked()) BufferedAction=NAME_None;
    if (!BufferedAction.IsNone())
    {
        const FName Id=BufferedAction;
        if (GetWorld()->GetTimeSeconds()>=BufferExpires || !Active || U->GetMelee()->GetAttackTime()>=BufferWindowEnd) BufferedAction=NAME_None;
        else if (CanTransition(Definitions.FindRef(Id))) { BufferedAction=NAME_None; ActivateActionId(Id); }
    }
    // A held axis has no new press edge when the animation opens recovery.
    TryResumeLocomotion();
    if (Active && Active->ActionKind==EPadmaACTActionKind::Jump && U->GetCharacterMovement()->IsFalling())
    {
        U->StopJumping();
        if (U->GetVelocity().Z<=0 && U->GetMesh()->GetAnimInstance()->Montage_GetCurrentSection(Active->Montage.Get())==TEXT("Start"))
            U->GetAbilitySystemComponent()->CurrentMontageJumpToSection(TEXT("Loop"));
    }
    if (Active && bDodgeWindow && U && U->IsAlive())
    {
        U->GetCharacterMovement()->MaxWalkSpeed=Active->DodgeSpeed;
        U->AddMovementInput(DodgeDirection,1.f,true);
    }
    UpdateAnimation();
}
UPadmaACTActionCooldown::UPadmaACTActionCooldown() { DurationPolicy=EGameplayEffectDurationType::HasDuration; }
UPadmaACTActionAbility::UPadmaACTActionAbility()
{
    InstancingPolicy=EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy=EGameplayAbilityNetExecutionPolicy::ServerOnly;
}
bool UPadmaACTActionAbility::CanActivateAbility(FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* Info,
    const FGameplayTagContainer* SourceTags,const FGameplayTagContainer* TargetTags,FGameplayTagContainer* Relevant) const
{
    auto* U=Info ? Cast<APadmaCombatUnit>(Info->AvatarActor.Get()) : nullptr;
    auto* Spec=Info && Info->AbilitySystemComponent.IsValid() ? Info->AbilitySystemComponent->FindAbilitySpecFromHandle(Handle) : nullptr;
    auto* D=Spec ? Cast<UPadmaACTSkillDefinition>(Spec->SourceObject.Get()) : nullptr;
    return U && !U->GetActions()->GetActiveDefinition() && U->GetActions()->CanStart(D)
        && Super::CanActivateAbility(Handle,Info,SourceTags,TargetTags,Relevant);
}
void UPadmaACTActionAbility::ActivateAbility(FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* Info,
    FGameplayAbilityActivationInfo Activation,const FGameplayEventData* Event)
{
    auto* U=Cast<APadmaCombatUnit>(Info->AvatarActor.Get());
    Definition=Cast<UPadmaACTSkillDefinition>(GetCurrentSourceObject());
    if (!U || !Definition || !CommitAbility(Handle,Info,Activation))
    { EndAbility(Handle,Info,Activation,true,true); return; }
    FString Failure;
    if ((Definition->FlowCost>0 || Definition->CalculationCost>0) && (!U->GetBattle()->PayCost
        || !U->GetBattle()->PayCost(Definition->FlowCost,Definition->CalculationCost,NAME_None,Failure)))
    { EndAbility(Handle,Info,Activation,true,true); return; }
    if (!U->GetActions()->BeginAction(Definition)) { EndAbility(Handle,Info,Activation,true,true); return; }
    if (Definition->CooldownSeconds>0)
    {
        auto Spec=MakeOutgoingGameplayEffectSpec(UPadmaACTActionCooldown::StaticClass());
        Spec.Data->SetDuration(Definition->CooldownSeconds,true);
        U->GetActions()->SetCooldown(Definition->DefinitionId,ApplyGameplayEffectSpecToOwner(Handle,Info,Activation,Spec));
    }
    U->GetMelee()->SetAttackPlayRate(Definition->PlayRate);
    U->GetMelee()->OnAttackFinished.AddUObject(this,&UPadmaACTActionAbility::MeleeFinished);
    auto* Task=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,Definition->Montage.LoadSynchronous(),Definition->PlayRate,Definition->StartSection,true,1.f,0.f,true);
    Task->OnCompleted.AddDynamic(this,&UPadmaACTActionAbility::Completed);
    Task->OnCancelled.AddDynamic(this,&UPadmaACTActionAbility::Interrupted);
    Task->OnInterrupted.AddDynamic(this,&UPadmaACTActionAbility::Interrupted);
    Task->ReadyForActivation();
    if (IsActive()) U->GetMelee()->CaptureMontageInstance();
}
void UPadmaACTActionAbility::Completed()
{
    if (auto* U=Cast<APadmaCombatUnit>(GetAvatarActorFromActorInfo())) U->GetMelee()->QueueMontageCompletion();
    else Interrupted();
}
void UPadmaACTActionAbility::Interrupted() { EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,true); }
void UPadmaACTActionAbility::MeleeFinished(bool Cancelled) { EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,Cancelled); }
void UPadmaACTActionAbility::EndAbility(FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* Info,
    FGameplayAbilityActivationInfo Activation,bool Replicate,bool Cancelled)
{
    if (bEnding || !IsEndAbilityValid(Handle,Info)) return;
    TGuardValue<bool> Guard(bEnding,true);
    if (auto* U=Info ? Cast<APadmaCombatUnit>(Info->AvatarActor.Get()) : nullptr)
    {
        U->GetMelee()->OnAttackFinished.RemoveAll(this);
        if (U->GetActions()->GetActiveDefinition()==Definition)
        { U->GetMelee()->FinishAttack(Cancelled); U->GetActions()->EndAction(Definition); }
    }
    Super::EndAbility(Handle,Info,Activation,Replicate,Cancelled);
}
