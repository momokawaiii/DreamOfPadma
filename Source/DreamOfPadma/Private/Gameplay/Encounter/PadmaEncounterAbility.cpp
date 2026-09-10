#include "Gameplay/Encounter/PadmaEncounterAbility.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"

UPadmaEncounterAbility::UPadmaEncounterAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

bool UPadmaEncounterAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	const auto* Unit = ActorInfo ? Cast<APadmaCombatUnit>(ActorInfo->AvatarActor.Get()) : nullptr;
	return Unit && Unit->GetBattle() && Unit->GetBattle()->CanActivateMode(Unit, EPadmaCombatMode::Encounter)
		&& Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UPadmaEncounterAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		if (auto* Unit = Cast<APadmaCombatUnit>(ActorInfo->AvatarActor.Get()))
			if (Unit->GetBattle()) Unit->GetBattle()->ResolvePendingAction(Unit);
	}
	// Damage and logical advancement are synchronous. Missing montage notifies cannot strand a turn.
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
