#include "Gameplay/ACT/Runtime/PadmaACTAbility.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Gameplay/ACT/Runtime/PadmaACTMelee.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"

UPadmaACTAbility::UPadmaACTAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

bool UPadmaACTAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	const auto* Unit = ActorInfo ? Cast<APadmaCombatUnit>(ActorInfo->AvatarActor.Get()) : nullptr;
	return Unit && Unit->GetBattle() && Unit->GetBattle()->CanActivateMode(Unit, EPadmaCombatMode::ACT)
		&& Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UPadmaACTAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		if (auto* Unit = Cast<APadmaCombatUnit>(ActorInfo->AvatarActor.Get()))
			if (Unit->GetBattle())
			{
				Unit->GetBattle()->ResolvePendingAction(Unit);
				if (Unit->GetMelee()->IsAttacking())
				{
					auto* Melee = Unit->GetMelee();
                    Melee->OnAttackFinished.AddUObject(this, &UPadmaACTAbility::OnMeleeFinished);
                    auto* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
                        this, TEXT("ACTMelee"), Melee->GetAttackMontage(), Melee->GetAttackPlayRate(), Melee->GetAttackSection(), true, 1.f, 0.f, true);
                    Task->OnCompleted.AddDynamic(this, &UPadmaACTAbility::OnMontageCompleted);
                    Task->OnInterrupted.AddDynamic(this, &UPadmaACTAbility::OnMontageCancelled);
                    Task->OnCancelled.AddDynamic(this, &UPadmaACTAbility::OnMontageCancelled);
                    Task->ReadyForActivation();
                    if (IsActive() && Melee->IsAttacking()) Melee->CaptureMontageInstance();
					return;
				}
			}
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UPadmaACTAbility::OnMeleeFinished(bool bCancelled)
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bCancelled);
}

void UPadmaACTAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (bEndingMelee || !IsEndAbilityValid(Handle, ActorInfo)) return;
    TGuardValue<bool> Guard(bEndingMelee, true);
    if (auto* Unit = ActorInfo ? Cast<APadmaCombatUnit>(ActorInfo->AvatarActor.Get()) : nullptr)
    {
        Unit->GetMelee()->OnAttackFinished.RemoveAll(this);
        Unit->GetMelee()->FinishAttack(bWasCancelled);
    }
    Super::EndAbility(Handle,ActorInfo,ActivationInfo,bReplicateEndAbility,bWasCancelled);
}

void UPadmaACTAbility::OnMontageCompleted()
{
    if (auto* Unit = CurrentActorInfo ? Cast<APadmaCombatUnit>(CurrentActorInfo->AvatarActor.Get()) : nullptr)
        Unit->GetMelee()->QueueMontageCompletion();
    else OnMeleeFinished(false);
}
void UPadmaACTAbility::OnMontageCancelled() { OnMeleeFinished(true); }
