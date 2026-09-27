#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/ACT/Runtime/PadmaACTTrainingAnalytics.h"
#include "PadmaACTMeleeLab.generated.h"

class UPadmaCombatComponent;
class UPadmaACTCharacterDefinition;
class UCameraComponent;
class UPadmaACTSkillDefinition;
class UPadmaACTTrainingAnalyticsWidget;
class APadmaCombatFeedback;
class UPadmaACTHologramComponent;
class UPointLightComponent;
class UWidgetInteractionComponent;
class UAnimSequence;
class UAnimMontage;
class APadmaCombatUnit;
class USkeletalMesh;

/** Isolated research arena using the same native battle/GAS path as ACT nodes. */
UCLASS()
class DREAMOFPADMA_API APadmaACTMeleeLab : public AActor
{
    GENERATED_BODY()
#if WITH_DEV_AUTOMATION_TESTS
    friend class FPadmaACTShowcaseTest;
#endif

public:
    APadmaACTMeleeLab();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UPadmaACTCharacterDefinition> CharacterDefinition;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Scene Participants") TObjectPtr<APadmaCombatUnit> ScenePlayer;
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Scene Participants") TArray<TObjectPtr<APadmaCombatUnit>> SceneTargets;
    /** Enabled on the authored Chen map; missing references must not silently spawn replacements. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scene Participants") bool bRequireSceneParticipants = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Training", meta=(ClampMin="1"))
    float DummyMaxHealth = 10000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Training")
    bool bDummyFollowsPlayer = false;

    /** Additional stationary target in the character-action lab, relative to the player spawn. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Training")
    FVector SecondDummyOffset = FVector(320.f, 220.f, 0.f);

    /** Three deliberately different mitigation fixtures; these are training content, not encounter enemies. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Training")
    TArray<FPadmaACTTrainingTargetProfile> TrainingTargets;

    /** Optional training visual; the capsule and mitigation fixtures remain unchanged. */
    UPROPERTY(EditAnywhere, Category="Training|Presentation") TSoftObjectPtr<USkeletalMesh> TrainingDummyMesh;
    UPROPERTY(EditAnywhere, Category="Training|Presentation") TSoftObjectPtr<UAnimSequence> TrainingDummyHitAnimation;
    UPROPERTY(EditAnywhere, Category="Training|Presentation")
    FTransform TrainingDummyMeshTransform = FTransform(FRotator::ZeroRotator, FVector(0, 0, -90), FVector(.85f));

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Training")
    TObjectPtr<UPadmaACTTrainingAnalyticsComponent> Analytics;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UPadmaCombatComponent> Battle;

    UPROPERTY(VisibleAnywhere, Category="Showcase") TObjectPtr<UPadmaACTHologramComponent> Hologram;
    UPROPERTY(VisibleAnywhere, Category="Showcase") TObjectPtr<UPointLightComponent> ProjectionLight;
    UPROPERTY(EditAnywhere, Category="Showcase") TSoftObjectPtr<UAnimSequence> ShowcaseEnter;
    UPROPERTY(EditAnywhere, Category="Showcase") TSoftObjectPtr<UAnimSequence> ShowcaseLoop;
    UPROPERTY(EditAnywhere, Category="Showcase") TSoftObjectPtr<UAnimSequence> ShowcaseExit;
    UPROPERTY(EditAnywhere, Category="Showcase") TSoftObjectPtr<UAnimSequence> ShowcaseRestLoop;

    // [CHANGED] Camera 现在是 RootComponent，作为 ViewTarget 直接提供 Transform
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UCameraComponent> Camera;

    UFUNCTION(BlueprintCallable) void ResetBattle();
    UFUNCTION(BlueprintCallable) bool Fire();
    UFUNCTION(BlueprintCallable) void ToggleTarget();
    UFUNCTION(BlueprintCallable) void CancelAttack();
    UFUNCTION(BlueprintCallable) void ToggleAnalytics();
    UFUNCTION(BlueprintCallable) void SetAnalyticsOpen(bool bOpen);
    UFUNCTION(BlueprintPure) bool IsAnalyticsOpen() const { return bAnalyticsOpen; }

    virtual void Tick(float DeltaSeconds) override;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;

private:
    bool StartSceneBattle();
    TMap<TWeakObjectPtr<APadmaCombatUnit>,FTransform> SceneInitialTransforms;
    bool BeginShowcase();
    void CloseShowcase();
    void TickShowcase(float Delta);
    void RestoreShowcase(bool bHoldExitPose = false);
    void ReleaseShowcaseRestPose();
    void TickShowcaseRestPose();
    TWeakObjectPtr<APadmaCombatUnit> ShowcaseRestUnit;
    UPROPERTY(Transient) TObjectPtr<UAnimMontage> ShowcaseRestMontage;
    void PlayShowcaseClip(UAnimSequence* Clip, bool bLoop, float Rate = 1.f);
    TWeakObjectPtr<APadmaCombatUnit> ShowcaseUnit;
    UPROPERTY(Transient) TObjectPtr<UAnimMontage> ShowcaseMontage;
    UPROPERTY(Transient) TObjectPtr<UAnimSequence> LoadedShowcaseEnter;
    UPROPERTY(Transient) TObjectPtr<UAnimSequence> LoadedShowcaseLoop;
    UPROPERTY(Transient) TObjectPtr<UAnimSequence> LoadedShowcaseExit;
    UPROPERTY(Transient) TObjectPtr<UAnimSequence> LoadedShowcaseRestLoop;
    FTransform ShowcaseReturnCamera, ShowcaseTargetCamera;
    FQuat ShowcaseReturnMesh, ShowcaseTargetMesh;
    float ShowcaseReturnFOV = 48, ShowcaseAge = 0, ShowcaseExitAge = 0, ShowcaseProgress = 0;
    bool bShowcaseActive = false, bShowcaseLoop = false;
    bool bBattleWasTicking = false, bActionsWereTicking = false, bMovementWasTicking = false, bCameraWasTicking = false;
    void AttackInput();
    void ReleaseShowcasePointer();
    void ScrollShowcaseUp();
    void ScrollShowcaseDown();
    UPROPERTY(Transient) TObjectPtr<UWidgetInteractionComponent> ShowcaseInteraction;
    void HandleReceipt(const FPadmaCombatReceipt& Receipt);
    void EnsureDefaultTrainingTargets();
    void DestroyFeedbackActors();

    bool bTargetNear = true;
    void TickActionCamera(float DeltaSeconds);
    void RestoreActionCamera();
    UPROPERTY(Transient) TObjectPtr<UPadmaACTSkillDefinition> CameraAction;
    FTransform CameraReturnTransform;
    float CameraReturnFOV = 48;
    float CameraElapsed = 0;
    uint64 CameraActionGeneration = 0;
    bool bCameraRestored = true;
    bool bAnalyticsOpen = false;
    double LastAnalyticsToggleTime = -1.0;
    UPROPERTY(Transient) TObjectPtr<UPadmaACTTrainingAnalyticsWidget> AnalyticsWidget;
    UPROPERTY(Transient) TArray<TObjectPtr<APadmaCombatFeedback>> FeedbackActors;

    // [CHANGED] 固定的战斗场地中心，不再依赖 GetActorLocation()
    // （Camera 成为 Root 后，Actor 位置 = 相机位置，不再是场地中心）
    static constexpr float ArenaCenterZ = 90.f;
};
