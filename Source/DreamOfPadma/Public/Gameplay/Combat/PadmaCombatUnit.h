#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpecHandle.h"
#include "Gameplay/Combat/PadmaCombatTypes.h"
#include "PadmaCombatUnit.generated.h"

class UPadmaACTMeleeComponent;
class UPadmaACTActionsComponent;
class UPadmaACTCameraComponent;
class UAbilitySystemComponent;
class UPadmaCombatAttributes;
class UPadmaCombatComponent;
class UStaticMeshComponent;
struct FGameplayAttribute;

/** Native GAS combatant: scene-owned or spawned. Only authored SceneSpec persists; combat state is transient. */
UCLASS()
class DREAMOFPADMA_API APadmaCombatUnit : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	APadmaCombatUnit();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystem; }
	void InitializeCombat(UPadmaCombatComponent* Battle, const FPadmaCombatUnitSpec& InSpec, EPadmaCombatMode Mode, bool bSourceOnly = false, bool bPreservePresentation = false);
	/** Scene-owned configuration. Runtime never writes back to these authored values. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|Scene") FPadmaCombatUnitSpec SceneSpec;
	/** Explicit initial assembly from UE assets; subsequent manual edits are preserved in play. */
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Combat|Scene") void ApplyScenePresentation();
	virtual void OnConstruction(const FTransform& Transform) override;
	bool PreservesScenePresentation() const { return bScenePresentation; }
	void ApplyAttribute(APadmaCombatUnit* Source, const FGameplayAttribute& Attribute, float Magnitude, bool bOverride = false, FName GameplayEffectId = TEXT("GE.Padma.Attribute.Instant"));
	FPadmaCombatUnitSnapshot Snapshot() const;
	float Health() const;
	bool IsAlive() const { return Health() > 0; }
	bool IsCardSource() const { return bCardSource; }
	UPadmaCombatComponent* GetBattle() const { return Combat.Get(); }
	UPadmaCombatAttributes* GetAttributes() const { return Attributes; }
	FGameplayAbilitySpecHandle GetModeAbility() const { return ModeAbility; }
	void CleanupGAS();
	void PlayAttackPresentation();
	UPadmaACTMeleeComponent* GetMelee() const { return Melee; }
    UFUNCTION(BlueprintPure) UPadmaACTActionsComponent* GetActions() const { return Actions; }
    UFUNCTION(BlueprintPure) UPadmaACTCameraComponent* GetACTCamera() const { return ACTCamera; }
    virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;
    virtual void Landed(const FHitResult& Hit) override;
	UPROPERTY(Transient) FPadmaCombatUnitSpec Spec;
	float ReadyTime = 0;
	bool bActed = false;
	float AttackCooldown = 0;
	float WindupRemaining = 0;
	FVector CombatVelocity = FVector::ZeroVector;
protected:
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
private:
	UPROPERTY(VisibleAnywhere) TObjectPtr<UPadmaACTMeleeComponent> Melee;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UPadmaACTActionsComponent> Actions;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UPadmaACTCameraComponent> ACTCamera;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UAbilitySystemComponent> AbilitySystem;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UPadmaCombatAttributes> Attributes;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> Placeholder;
	UPROPERTY(Transient) TWeakObjectPtr<UPadmaCombatComponent> Combat;
	FGameplayAbilitySpecHandle ModeAbility;
	bool bCardSource = false;
	bool bGASCleaned = false;
	bool bScenePresentation = false;
};
