#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffect.h"
#include "Gameplay/ACT/Authoring/PadmaACTAuthoring.h"
#include "PadmaACTActions.generated.h"

class APadmaCombatUnit;
class UAbilityTask_PlayMontageAndWait;

UENUM(BlueprintType)
enum class EPadmaACTTransitionWindow : uint8 { Cache, Allow, Recovery, Block, BlockInput };

/** Per-character action grants, targeting and movement. All timing windows come from Montage notifies. */
UCLASS(ClassGroup=(Padma), meta=(BlueprintSpawnableComponent))
class DREAMOFPADMA_API UPadmaACTActionsComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UPadmaACTActionsComponent();
    static bool ValidateDefinition(UPadmaACTCharacterDefinition* Definition, FString& Failure);
    bool Configure(UPadmaACTCharacterDefinition* Definition);
    void ResetConfiguration();
    bool IsConfigured() const { return Character != nullptr; }
    UFUNCTION(BlueprintCallable) bool RequestInput(FName Binding);
    UFUNCTION(BlueprintCallable) void Move(FVector WorldDirection);
    UFUNCTION(BlueprintPure) bool WantsToRun() const { return bWantsToRun; }
    bool HasLocomotionInput() const { return !MoveDirection.IsNearlyZero(); }
    UFUNCTION(BlueprintPure) bool IsGameplayInputLocked() const;
    void StartPlungeDescent();
    UFUNCTION(BlueprintPure) FName GetActiveActionId() const { return Active ? Active->DefinitionId : NAME_None; }
    UFUNCTION(BlueprintPure) int32 GetPerfectDodgeCount() const { return PerfectDodgeCount; }
    UFUNCTION(BlueprintPure) bool IsDodgeWindowOpen() const { return bDodgeWindow; }
    UPadmaACTSkillDefinition* GetActiveDefinition() const { return Active; }
    bool CanStart(const UPadmaACTSkillDefinition* Definition) const;
    bool BeginAction(UPadmaACTSkillDefinition* Definition);
    void EndAction(const UPadmaACTSkillDefinition* Definition);
    void Landed();
    void SetDodgeWindow(bool bOpen, bool bPerfect, float Start=0, float End=0);
    bool TryEvade();
    bool AllowsContact(APadmaCombatUnit* Target) const;
    APadmaCombatUnit* GetPrimaryTarget() const { return PrimaryTarget.Get(); }
    void SetTransitionWindow(const void* Key, bool Open, EPadmaACTTransitionWindow Kind, const TArray<FName>& Actions, float Start, float End, float CacheSeconds);
    bool IsCoolingDown(FName Id) const;
    void SetCooldown(FName Id, FActiveGameplayEffectHandle Handle);
protected:
    virtual void TickComponent(float Delta, ELevelTick Type, FActorComponentTickFunction* Tick) override;
private:
    UPROPERTY(Transient) TObjectPtr<UPadmaACTCharacterDefinition> Character;
    UPROPERTY(Transient) TMap<FName,TObjectPtr<UPadmaACTSkillDefinition>> Definitions;
    TMap<FName,FName> Bindings;
    TMap<FName,FGameplayAbilitySpecHandle> Grants;
    TMap<FName,FActiveGameplayEffectHandle> Cooldowns;
    UPROPERTY(Transient) TObjectPtr<UPadmaACTSkillDefinition> Active;
    struct FTransition { EPadmaACTTransitionWindow Kind; TArray<FName> Actions; float Start,End,CacheSeconds; };
    TMap<const void*,FTransition> Transitions;
    struct FCarriedBlock { TArray<FName> Actions; double Expires; };
    TArray<FCarriedBlock> CarriedBlocks;
    FName BufferedAction;
    TWeakObjectPtr<APadmaCombatUnit> PrimaryTarget;
    double BufferExpires = 0;
    float BufferWindowEnd = MAX_flt;
    FVector MoveDirection = FVector::ZeroVector, DodgeDirection = FVector::ZeroVector;
    bool bDodgeWindow = false, bPerfectWindow = false, bChangingAction = false;
    float DodgeStart = 0, DodgeEnd = 0;
    FName PendingPerfect;
    int32 PerfectDodgeCount = 0;
    bool bWantsToRun = false;
    bool bMoveRequiresRelease = false;
    bool bPlungeMovementOverride = false;
    bool bPlungeDescending = false;
    float SavedGravityScale = 1.f, SavedAirControl = 0.f;
    void RestorePlungeMovement();
    float LocomotionSpeed() const;
    bool CanTransition(const UPadmaACTSkillDefinition* Definition) const;
    bool CanResumeLocomotion() const;
    void TryResumeLocomotion();
    bool IsInputBlocked(FName Id) const;
    bool QueueAction(FName Id);
    bool ActivateActionId(FName Id, bool bInternal = false);
    void UpdateAnimation();
};

UCLASS()
class DREAMOFPADMA_API UPadmaACTActionCooldown : public UGameplayEffect
{
    GENERATED_BODY()
public: UPadmaACTActionCooldown();
};

/** One granted spec per immutable skill definition; official GAS Montage task owns playback/cancellation. */
UCLASS()
class DREAMOFPADMA_API UPadmaACTActionAbility : public UGameplayAbility
{
    GENERATED_BODY()
public:
    UPadmaACTActionAbility();
    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
        const FGameplayTagContainer* SourceTags=nullptr, const FGameplayTagContainer* TargetTags=nullptr, FGameplayTagContainer* Relevant=nullptr) const override;
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
        const FGameplayAbilityActivationInfo Activation, const FGameplayEventData* Event) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
        const FGameplayAbilityActivationInfo Activation, bool Replicate, bool Cancelled) override;
private:
    UPROPERTY(Transient) TObjectPtr<UPadmaACTSkillDefinition> Definition;
    bool bEnding = false;
    UFUNCTION() void Completed();
    UFUNCTION() void Interrupted();
    void MeleeFinished(bool bCancelled);
};
