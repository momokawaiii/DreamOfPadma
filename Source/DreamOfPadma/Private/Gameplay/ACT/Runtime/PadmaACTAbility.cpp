#include "Gameplay/ACT/Runtime/PadmaACTAbility.h"
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
			if (Unit->GetBattle()) Unit->GetBattle()->ResolvePendingAction(Unit);
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
