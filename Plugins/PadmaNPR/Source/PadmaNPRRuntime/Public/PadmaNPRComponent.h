#pragma once
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "PadmaNPRComponent.generated.h"
class UPadmaNPRProfile;
class USkeletalMeshComponent;
class ADirectionalLight;
class UMaterialInterface;
class UMaterialInstanceDynamic;
// Reserves Custom Primitive Data floats 0..19 on the bound mesh. Never uses a
// global MPC.
UCLASS(ClassGroup = (Rendering), meta = (BlueprintSpawnableComponent))
class PADMANPRRUNTIME_API UPadmaNPRComponent : public UActorComponent {
  GENERATED_BODY()
public:
  UPadmaNPRComponent();
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Padma NPR")
  TObjectPtr<UPadmaNPRProfile> Profile;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Padma NPR")
  TObjectPtr<USkeletalMeshComponent> TargetMesh;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Padma NPR")
  TObjectPtr<ADirectionalLight> KeyLight;
  UFUNCTION(BlueprintCallable, Category = "Padma NPR")
  bool ApplyProfile(FString &Error);
  UFUNCTION(BlueprintCallable, Category = "Padma NPR") void RestoreMaterials();
  UFUNCTION(BlueprintCallable, Category = "Padma NPR")
  bool ValidateBinding(FString &Error) const;
  virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                             FActorComponentTickFunction *ThisTick) override;
  virtual void OnRegister() override;
  virtual void OnUnregister() override;
  UPROPERTY() TMap<FName, TObjectPtr<UMaterialInterface>> EditorOriginals;
  UPROPERTY() TMap<FName, TObjectPtr<UMaterialInterface>> EditorApplied;
  void UpdateFrameData();

private:
  UPROPERTY(Transient) TArray<TObjectPtr<UMaterialInterface>> Originals;
  UPROPERTY(Transient) TArray<TObjectPtr<UMaterialInstanceDynamic>> Active;
  TArray<float> PreviousPrimitiveData;
  TWeakObjectPtr<USkeletalMeshComponent> BoundMesh;
  bool bBindingActive = false;
  void BeginBinding();
};
