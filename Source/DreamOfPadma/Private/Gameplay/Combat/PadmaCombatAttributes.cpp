#include "Gameplay/Combat/PadmaCombatAttributes.h"
#include "GameplayEffectExtension.h"

UPadmaCombatAttributeEffect::UPadmaCombatAttributeEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
}

void UPadmaCombatAttributes::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetHealthAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	else if (Attribute == GetMaxHealthAttribute() || Attribute == GetShieldAttribute() || Attribute == GetBlockAttribute()) NewValue = FMath::Max(0.f, NewValue);
}

void UPadmaCombatAttributes::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	if (Data.EvaluatedData.Attribute == GetHealthAttribute()) SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	else if (Data.EvaluatedData.Attribute == GetShieldAttribute()) SetShield(FMath::Max(0.f, GetShield()));
	else if (Data.EvaluatedData.Attribute == GetBlockAttribute()) SetBlock(FMath::Max(0.f, GetBlock()));
}
