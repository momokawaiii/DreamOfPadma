#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PadmaNPRStudioLibrary.generated.h"
class UPadmaNPRProfile;
class UPadmaNPRComponent;
class USkeletalMeshComponent;
class ADirectionalLight;
class UTexture2D;
UCLASS()
class PADMANPREDITOR_API UPadmaNPRStudioLibrary
    : public UBlueprintFunctionLibrary {
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable, Category = "Padma NPR|Editor")
  static UTexture2D *BakeInvertedRedMask(UTexture2D *Source,
                                         const FString &PackagePath,
                                         FString &Error);
  UFUNCTION(BlueprintCallable, Category = "Padma NPR|Editor")
  static UPadmaNPRComponent *ApplyToActor(UPadmaNPRProfile *Profile,
                                          AActor *Actor,
                                          USkeletalMeshComponent *Mesh,
                                          ADirectionalLight *Light,
                                          FString &Error);
  UFUNCTION(BlueprintCallable, Category = "Padma NPR|Editor")
  static bool RestoreActor(AActor *Actor, FString &Error);
};
