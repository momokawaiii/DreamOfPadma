#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "PadmaCombatAttributes.generated.h"

#define PADMA_ATTRIBUTE_ACCESSORS(Name) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UPadmaCombatAttributes, Name) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(Name) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(Name) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(Name)

UCLASS()
class DREAMOFPADMA_API UPadmaCombatAttributes : public UAttributeSet
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly) FGameplayAttributeData Health;
	PADMA_ATTRIBUTE_ACCESSORS(Health)
	UPROPERTY(BlueprintReadOnly) FGameplayAttributeData MaxHealth;
	PADMA_ATTRIBUTE_ACCESSORS(MaxHealth)
	UPROPERTY(BlueprintReadOnly) FGameplayAttributeData Shield;
	PADMA_ATTRIBUTE_ACCESSORS(Shield)
	UPROPERTY(BlueprintReadOnly) FGameplayAttributeData Block;
	PADMA_ATTRIBUTE_ACCESSORS(Block)
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};

/** Instant native GE; each spec contains explicitly authored attribute modifiers. */
UCLASS()
class DREAMOFPADMA_API UPadmaCombatAttributeEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UPadmaCombatAttributeEffect();
};

#undef PADMA_ATTRIBUTE_ACCESSORS
