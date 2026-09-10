#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpecHandle.h"
#include "Gameplay/Combat/PadmaCombatTypes.h"
#include "PadmaCombatUnit.generated.h"

class UAbilitySystemComponent;
class UPadmaCombatAttributes;
class UPadmaCombatComponent;
class UStaticMeshComponent;
struct FGameplayAttribute;

/** Transient native GAS combatant; never persisted. CardSource is a hidden separate ASC. */
UCLASS()
class DREAMOFPADMA_API APadmaCombatUnit : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	APadmaCombatUnit();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystem; }
	void InitializeCombat(UPadmaCombatComponent* Battle, const FPadmaCombatUnitSpec& InSpec, EPadmaCombatMode Mode, bool bSourceOnly = false);
	void ApplyAttribute(APadmaCombatUnit* Source, const FGameplayAttribute& Attribute, float Magnitude, bool bOverride = false);
	FPadmaCombatUnitSnapshot Snapshot() const;
	float Health() const;
	bool IsAlive() const { return Health() > 0; }
	bool IsCardSource() const { return bCardSource; }
	UPadmaCombatComponent* GetBattle() const { return Combat.Get(); }
	UPadmaCombatAttributes* GetAttributes() const { return Attributes; }
	FGameplayAbilitySpecHandle GetModeAbility() const { return ModeAbility; }
	void CleanupGAS();
	void PlayAttackPresentation();
	UPROPERTY(Transient) FPadmaCombatUnitSpec Spec;
	float ReadyTime = 0;
	bool bActed = false;
	float AttackCooldown = 0;
	float WindupRemaining = 0;
	FVector CombatVelocity = FVector::ZeroVector;
protected:
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
private:
	UPROPERTY(VisibleAnywhere) TObjectPtr<UAbilitySystemComponent> AbilitySystem;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UPadmaCombatAttributes> Attributes;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> Placeholder;
	UPROPERTY(Transient) TWeakObjectPtr<UPadmaCombatComponent> Combat;
	FGameplayAbilitySpecHandle ModeAbility;
	bool bCardSource = false;
	bool bGASCleaned = false;
};
