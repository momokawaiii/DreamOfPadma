#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataAsset.h"
#include "PadmaACTMelee.generated.h"

class UAnimSequenceBase;
class UAnimSequence;
class UAnimMontage;
class UNiagaraSystem;
class UNiagaraComponent;
class UStaticMesh;
class UStaticMeshComponent;
class APadmaCombatUnit;
class UMaterialInterface;
class UMeshComponent;
class USkeletalMesh;
class USkeletalMeshComponent;

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaACTMeleeWeapon
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UStaticMesh> Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName Socket;

    /** Optional non-combat mount. None preserves the legacy always-drawn presentation. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName StowedSocket;

    /** Mesh-to-mount conversion, including any source prefab offset. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTransform StowedTransform = FTransform::Identity;

    /** Optional project-owned materials supporting WeaponDissolve (0 visible, 1 gone). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSoftObjectPtr<UMaterialInterface>> PresentationMaterials;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector BladeBase = FVector(0, 0, 0);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector BladeTip = FVector(0, 0, 100);
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaACTContactShape
{
    GENERATED_BODY()
    /** Full box dimensions in centimeters; Radius > 0 selects a sphere. Local axes: forward, right, up. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector Size = FVector(200);
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector Center = FVector::ZeroVector;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FRotator Rotation = FRotator::ZeroRotator;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0")) float Radius = 0;
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaACTMeleeWindow
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 WeaponIndex = 0;

    // Demo tuning; this is not an assertion of the source game's damage formula.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DamageMultiplier = .5f;

    // Target locator for overlaps/volume contacts or an invalid surface impact.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName HitSocket;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bFaceTargetPlanar = false;

    // Optional contact VFX for this strike; null uses the profile fallback.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UNiagaraSystem> HitEffect;
    // Source actions may explicitly disable the common impact while retaining damage.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bSuppressFallbackHitEffect = false;
    /** Optional world overlap window for skills/plunge/execution; zero keeps blade sweeps. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0")) float AreaRadius = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector AreaOffset = FVector::ZeroVector;
    /** Union of source volumes, relative to capsule foot; a target is hit once across the union. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FPadmaACTContactShape> Shapes;
    /** Source damage uses its captured main target rather than a spatial finder. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bPrimaryTargetOnly = false;
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaACTMeleeFX
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UNiagaraSystem> System;

    // -1 = authored character-root effect; >=0 = actual equipped weapon.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 WeaponIndex = -1;

    // False snapshots the mount transform at spawn (source Stationary mode).
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bFollowMount = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTransform RelativeTransform;

    /** Optional skeletal locator; empty retains the existing root snapshot. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName MountSocket;
    /** A pose snapshot instead of Niagara, for source renderer-afterimage effects. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSoftObjectPtr<UMaterialInterface> AfterimageMaterial;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.01")) float AfterimageLifetime = .42f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSoftObjectPtr<USkeletalMesh> AnimatedMesh;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSoftObjectPtr<UAnimSequence> MeshAnimation;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float MeshFadeStart = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float MeshFadeDuration = .5f;
};

UCLASS(BlueprintType)
class DREAMOFPADMA_API UPadmaACTMeleeDefinition : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Presentation") bool bWeaponLifecycle = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Presentation") TSoftObjectPtr<UAnimMontage> DrawWeaponMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Presentation") TSoftObjectPtr<UAnimMontage> SheatheWeaponMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Presentation", meta=(ClampMin="0")) float DrawnHoldSeconds = 3.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Presentation", meta=(ClampMin="0")) float StowedHoldSeconds = 3.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Presentation", meta=(ClampMin="0.01")) float WeaponDissolveSeconds = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Presentation", meta=(ClampMin="0.01")) float MovingWeaponDissolveSeconds = .3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UAnimMontage> AttackMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName AttackSection = TEXT("Attack01");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UAnimSequence> IdleAnimation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TraceRadius = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator MeshRelativeRotation = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FPadmaACTMeleeWeapon> Weapons;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UNiagaraSystem> HitEffect;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FPadmaACTMeleeFinished, bool);

/** Montage notifies own timing. This component owns equipment, contacts and action FX instances. */
UCLASS(ClassGroup=(Padma), meta=(BlueprintSpawnableComponent))
class DREAMOFPADMA_API UPadmaACTMeleeComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UPadmaACTMeleeComponent();
    bool Configure(UPadmaACTMeleeDefinition* Definition);
    void ResetConfiguration();
    bool IsConfigured() const { return Profile != nullptr; }
    UFUNCTION(BlueprintPure) bool IsAttacking() const { return bAttacking; }
    bool BeginAttack();
    bool BeginAction(UAnimMontage* Montage, FName Section, float Scale);
    UFUNCTION(BlueprintCallable) bool RequestCombo();
    void FinishAttack(bool bCancelled);
    FPadmaACTMeleeFinished OnAttackFinished;
    UFUNCTION(BlueprintPure) int32 GetConfirmedContacts() const { return ConfirmedContacts; }
    UFUNCTION(BlueprintPure) int32 GetSpawnedHitEffects() const { return SpawnedHitEffects; }
    UFUNCTION(BlueprintPure) int32 GetSpawnedActionEffects() const { return SpawnedActionEffects; }
    UFUNCTION(BlueprintPure) int32 GetAttackCycle() const { return AttackCycle; }
    UFUNCTION(BlueprintPure) int32 GetActiveWindowCount() const { return Windows.Num(); }
    UFUNCTION(BlueprintPure) int32 GetLiveActionEffectCount() const;
    UFUNCTION(BlueprintPure) bool HasLiveAfterimages() const;
    UFUNCTION(BlueprintPure) bool IsComboWindowOpen() const;
    UFUNCTION(BlueprintPure) float GetAttackTime() const;
    UFUNCTION(BlueprintPure) float GetAttackPlayRate() const { return PlayRate; }
    UFUNCTION(BlueprintCallable) bool SetAttackPlayRate(float Rate);
    UFUNCTION(BlueprintPure) UStaticMeshComponent* GetWeapon(int32 Index) const;
    UAnimMontage* GetAttackMontage() const { return AttackMontage; }
    FName GetAttackSection() const { return ActiveSection; }
    bool AcceptsNotify(const UAnimSequenceBase* Animation) const;
    int32 GetMontageInstanceID() const { return MontageInstanceID; }
    uint64 GetActionGeneration() const { return ActionGeneration; }
    void QueueActionEffect(const FPadmaACTMeleeFX& Effect, bool bStop);
    void BeginHitWindow(const UObject* Key, const FPadmaACTMeleeWindow& Window, float Start, float End);
    void EndHitWindow(const UObject* Key);
    void SetComboWindow(bool bOpen, float Start = 0, float End = 0);
    void CaptureMontageInstance();
    void QueueMontageCompletion() { if (bAttacking) bCompletionPending = true; }
    /** Combat entry is an edge, not a per-frame request; ability completion can still retire weapons while locked. */
    void EnterWeaponCombat();
    void SetShowcasePresentation(bool bEnabled, bool bDissolveOnExit = false);
    void ReleaseShowcaseRestWeapons();
    void LeaveWeaponCombat();
    /** Selects the armed base idle beneath action/presentation Montages; never blocks locomotion. */
    UFUNCTION(BlueprintPure) bool WantsWeaponCombatIdle() const;
    void WeaponPresentationNotify(UAnimSequenceBase* Animation, int32 InstanceID, int32 WeaponIndex, bool bDrawn, bool bVisible, bool bReleaseCombatIdle = false);
protected:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* Tick) override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;
private:
    UPROPERTY(Transient) TObjectPtr<UPadmaACTMeleeDefinition> Profile;
    UPROPERTY(Transient) TObjectPtr<UAnimMontage> AttackMontage;
    UPROPERTY(Transient) TArray<TObjectPtr<UStaticMeshComponent>> Weapons;
    UPROPERTY(Transient) TArray<TObjectPtr<UNiagaraComponent>> Effects;
    UPROPERTY(Transient) TArray<TObjectPtr<UNiagaraComponent>> RetiringEffects;
    TArray<float> RetiringEffectAges;
    UPROPERTY(Transient) TArray<TObjectPtr<UMeshComponent>> Afterimages;
    TArray<float> AfterimageAges;
    TArray<float> AfterimageLifetimes;
    UPROPERTY(Transient) TArray<TObjectPtr<USkeletalMeshComponent>> AnimatedEffects;
    TArray<float> AnimatedEffectAges;
    TArray<FVector2f> AnimatedEffectFades;
    struct FWindowState
    {
        FPadmaACTMeleeWindow Config;
        TSet<TWeakObjectPtr<AActor>> Contacts;
        float Start = 0, End = 0;
        bool bClosing = false;
    };
    struct FPendingEffect { FPadmaACTMeleeFX Config; bool bStop; };
    TMap<const UObject*, TSharedPtr<FWindowState>> Windows;
    TArray<FPendingEffect> PendingEffects;
    TArray<FTransform> PreviousWeapons;
    // Captured once from placed components, never from their animated runtime mounts.
    TMap<TWeakObjectPtr<UStaticMeshComponent>, FTransform> SceneWeaponOffsets;
    TMap<TWeakObjectPtr<UStaticMeshComponent>, FName> SceneWeaponStowedSockets;
    // Observed Montage position for clipping sweeps between evaluated poses; never advanced here.
    float PreviousMontagePosition = 0;
    float PlayRate = 1;
    float DamageScale = 1;
    uint64 ActionGeneration = 0;
    FName ActiveSection;
    float ComboStart = 0, ComboEnd = 0;
    int32 MontageInstanceID = INDEX_NONE;
    bool bCompletionPending = false;
    bool bAttacking = false, bComboQueued = false, bComboOpen = false;
    int32 AttackCycle = 0, ConfirmedContacts = 0, SpawnedHitEffects = 0, SpawnedActionEffects = 0;
    void TraceWindow(const TSharedPtr<FWindowState>& State, float Position);
    void PlayActionEffect(const FPendingEffect& Effect);
    void ResetCycle();
    void ClearActionEffects(bool bAllowParticleTail = false);
    void SetWeaponsDrawn(bool bDrawn);
    bool bWeaponsDrawn = true;
    bool bShowcasePresentation = false;
    bool bShowcaseRestWeapons = false;
    enum class EWeaponPhase : uint8 { Stowed, Drawing, Drawn, Holding, Sheathing, Dissolving, Appearing, Hidden };
    EWeaponPhase WeaponPhase = EWeaponPhase::Stowed;
    float WeaponPhaseAge = 0;
    float WeaponDissolveDuration = 1.f;
    bool bActionDrewWeapons = false;
    bool bWeaponIdleReleased = false;
    UPROPERTY(Transient) TObjectPtr<UAnimMontage> WeaponMontage;
    int32 WeaponMontageInstanceID = INDEX_NONE;
    void TickWeaponPresentation(float Delta);
    void StopWeaponMontage();
    void PlayWeaponMontage(bool bDraw);
    void CompleteWeaponMontage(bool bDraw);
    void SetWeaponMount(int32 Index, bool bDrawn);
    void SetWeaponDissolve(float Alpha);
    void PrepareWeaponAction(bool bDraw);
    void FinishWeaponAction();
};
