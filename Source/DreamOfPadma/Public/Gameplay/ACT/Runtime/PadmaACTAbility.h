#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PadmaACTAbility.generated.h"

/** ACT input/cooldown policy has no Encounter turn fallback. */
UCLASS()
class DREAMOFPADMA_API UPadmaACTAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UPadmaACTAbility();
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
