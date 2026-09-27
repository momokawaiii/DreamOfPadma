#pragma once
#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Engine/DataAsset.h"
#include "PadmaACTCamera.generated.h"

class APadmaCombatUnit;
class APlayerController;
class UPadmaACTSkillDefinition;
class UStaticMeshComponent;
class UMaterialInterface;

USTRUCT(BlueprintType)
struct FPadmaACTOrbitSample
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Vertical = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Height = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Distance = 500;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float VerticalFOV = 58;
};

/** Source orbit samples use centimeters. Input/selection parameters are separately editable UE adaptations. */
UCLASS(BlueprintType)
class DREAMOFPADMA_API UPadmaACTCameraDefinition : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FPadmaACTOrbitSample> Orbit;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float InitialVertical = .55f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float MinVertical = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxVertical = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float MinZoom = .7f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxZoom = .8f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float InitialZoom = .7f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShoulderHeight = 165;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float LookHeight = 140;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float FollowDamping = .22f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float VerticalDamping = .1f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float HorizontalTweenSpeed = 14;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float VerticalTweenSpeed = 16;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float ZoomDamping = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float CollisionRadius = 20;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float CollisionReleaseDamping = 2;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float AutoYawDelay = .5f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float AutoYawSpeed = 15;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float AutoYawAcceleration = 7.5f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float LockVertical = .55f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float LockDamping = .6f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float LockEnterTime = .2f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float LockLookWeight = .3f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float LockMinZoom = .7f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float LockMaxZoom = .85f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UE Input") float MouseYawSensitivity = .2f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UE Input") float MouseVerticalSensitivity = .002f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UE Input") float ZoomStep = .04f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UE Target Selection") float LockRange = 2000;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UE Target Selection") float UnlockRange = 2500;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UE Target Selection") float AcquireHalfAngle = 65;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UE Target Selection") float OcclusionGrace = .5f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UE Target Selection") TSoftObjectPtr<UMaterialInterface> LockMarkerMaterial;
};

/** Character-owned local ACT view. Both lab and production input routes use this component. */
UCLASS(ClassGroup=(Padma), meta=(BlueprintSpawnableComponent))
class DREAMOFPADMA_API UPadmaACTCameraComponent : public UCameraComponent
{
    GENERATED_BODY()
public:
    UPadmaACTCameraComponent();
    bool Configure(UPadmaACTCameraDefinition* Definition);
    bool IsConfigured() const { return Profile!=nullptr; }
    bool IsUsable() const;
    UFUNCTION(BlueprintCallable) void Look(FVector2D MouseDelta);
    UFUNCTION(BlueprintCallable) void Zoom(float Steps);
    UFUNCTION(BlueprintCallable) bool ToggleLock();
    UFUNCTION(BlueprintCallable) bool SwitchLock(float Direction);
    UFUNCTION(BlueprintPure) APadmaCombatUnit* GetLockedTarget() const;
    FVector CameraRelativeMovement(FVector2D ForwardRight) const;
    void HandleInput(APlayerController* Controller, bool bEnabled);
    void ClearLock();
    void ResetView();
    virtual void TickComponent(float Delta, ELevelTick Type, FActorComponentTickFunction* Tick) override;
private:
    UPROPERTY(Transient) TObjectPtr<UPadmaACTCameraDefinition> Profile;
    UPROPERTY(Transient) TObjectPtr<UStaticMeshComponent> LockMarker;
    TWeakObjectPtr<APadmaCombatUnit> LockedTarget;
    TWeakObjectPtr<UPadmaACTSkillDefinition> CameraAction;
    uint64 CameraGeneration = 0;
    float Yaw=0, Vertical=.55f, ZoomScale=.7f, SmoothedYaw=0, SmoothedVertical=.55f, SmoothedZoom=.7f;
    float SinceLook=0, YawSpeed=0, ObscuredTime=0, LockAge=0, CameraElapsed=0, CollisionDistance=0, ReturnRemaining=0;
    FVector FollowPosition=FVector::ZeroVector, FollowVelocity=FVector::ZeroVector;
    FTransform LastActionView;
    float LastActionFOV=58;
    bool bReady=false, bActionOverriding=false;
    APadmaCombatUnit* Unit() const;
    bool IsValidTarget(APadmaCombatUnit* Target, float Range, bool bRequireVisibility) const;
    bool VisibleTarget(APadmaCombatUnit* Target) const;
    APadmaCombatUnit* SelectTarget(float SwitchDirection=0) const;
    void SetLock(APadmaCombatUnit* Target);
    FPadmaACTOrbitSample SampleOrbit(float Value) const;
    void ApplyActionCamera(float Delta, FTransform& View, float& FOV);
};
