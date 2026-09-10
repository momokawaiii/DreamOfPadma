#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PadmaEncounterAbility.generated.h"

/** Encounter grants only within the active logical action window. */
UCLASS()
class DREAMOFPADMA_API UPadmaEncounterAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UPadmaEncounterAbility();
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
