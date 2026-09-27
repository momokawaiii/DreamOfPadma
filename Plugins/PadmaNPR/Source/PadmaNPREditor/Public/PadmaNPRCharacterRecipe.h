#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PadmaNPRCharacterRecipe.generated.h"
class USkeletalMesh;
class UTexture2D;
class UAnimSequence;
class UPadmaNPRProfile;
class UMaterial;
UENUM()
enum class EPadmaManualSurface : uint8 {
  KeepOriginal,
  Cloth,
  Skin,
  Face,
  Hair,
  Eye,
  Brow
};
UENUM()
enum class EPadmaTextureChannel : uint8 { R, G, B, A };
USTRUCT()
struct FPadmaManualSlot {
  GENERATED_BODY()
  UPROPERTY(VisibleAnywhere, Category = "Slot") FName SlotName;
  UPROPERTY(EditAnywhere, Category = "Slot")
  EPadmaManualSurface Surface = EPadmaManualSurface::KeepOriginal;
  UPROPERTY(EditAnywhere, Category = "Textures")
  TObjectPtr<UTexture2D> BaseColor;
  // Standard UE tangent-space Normal compression. Custom reference RG encodings
  // are not accepted.
  UPROPERTY(EditAnywhere, Category = "Textures") TObjectPtr<UTexture2D> Normal;
  UPROPERTY(EditAnywhere, Category = "Textures",
            meta = (ClampMin = "0", ClampMax = "4"))
  float NormalStrength = 1;
  UPROPERTY(EditAnywhere, Category = "Textures") bool bFlipNormalGreen = false;
  UPROPERTY(EditAnywhere, Category = "Masks") TObjectPtr<UTexture2D> Roughness;
  UPROPERTY(EditAnywhere, Category = "Masks")
  EPadmaTextureChannel RoughnessChannel = EPadmaTextureChannel::R;
  UPROPERTY(EditAnywhere, Category = "Masks") bool bGlossiness = false;
  UPROPERTY(EditAnywhere, Category = "Masks",
            meta = (ClampMin = "0.04", ClampMax = "1"))
  float ConstantRoughness = .6f;
  UPROPERTY(EditAnywhere, Category = "Masks") TObjectPtr<UTexture2D> Metallic;
  UPROPERTY(EditAnywhere, Category = "Masks")
  EPadmaTextureChannel MetallicChannel = EPadmaTextureChannel::R;
  UPROPERTY(EditAnywhere, Category = "Masks",
            meta = (ClampMin = "0", ClampMax = "1"))
  float ConstantMetallic = 0;
  UPROPERTY(EditAnywhere, Category = "Masks") TObjectPtr<UTexture2D> AO;
  UPROPERTY(EditAnywhere, Category = "Masks")
  EPadmaTextureChannel AOChannel = EPadmaTextureChannel::R;
  UPROPERTY(EditAnywhere, Category = "Face SDF") TObjectPtr<UTexture2D> FaceSDF;
  UPROPERTY(EditAnywhere, Category = "Face SDF")
  EPadmaTextureChannel SDFChannel = EPadmaTextureChannel::B;
  UPROPERTY(EditAnywhere, Category = "Face SDF") bool bMirrorSDF = false;
  UPROPERTY(EditAnywhere, Category = "Matcap") TObjectPtr<UTexture2D> Matcap;
  UPROPERTY(EditAnywhere, Category = "Matcap",
            meta = (ClampMin = "0", ClampMax = "2"))
  float MatcapStrength = 0;
  UPROPERTY(EditAnywhere, Category = "Style")
  FLinearColor Tint = FLinearColor::White;
  UPROPERTY(EditAnywhere, Category = "Style")
  FLinearColor ShadowColor = FLinearColor(.5f, .45f, .5f);
  UPROPERTY(EditAnywhere, Category = "Style",
            meta = (ClampMin = "0", ClampMax = "1"))
  float Threshold = .45f;
  UPROPERTY(EditAnywhere, Category = "Style",
            meta = (ClampMin = "0.001", ClampMax = "0.5"))
  float Feather = .08f;
  UPROPERTY(EditAnywhere, Category = "Style",
            meta = (ClampMin = "0", ClampMax = "1"))
  float Rim = .05f;
  UPROPERTY(EditAnywhere, Category = "Style",
            meta = (ClampMin = "0", ClampMax = "2"))
  float Highlight = .25;
  UPROPERTY(EditAnywhere, Category = "UV",
            meta = (ClampMin = "0", ClampMax = "3"))
  int32 UVChannel = 0;
  UPROPERTY(EditAnywhere, Category = "UV") FVector2D UVScale = FVector2D(1, 1);
  UPROPERTY(EditAnywhere, Category = "UV")
  FVector2D UVOffset = FVector2D::ZeroVector;
};
// Editor-only recipe; generated runtime profiles never reference this object.
UCLASS(BlueprintType)
class PADMANPREDITOR_API UPadmaNPRCharacterRecipe : public UDataAsset {
  GENERATED_BODY()
public:
  UPROPERTY(EditAnywhere, Category = "Character")
  TObjectPtr<USkeletalMesh> Mesh;
  UPROPERTY(EditAnywhere, Category = "Output")
  FString OutputFolder = TEXT("/Game/Padma/NPR/GeneratedCharacter");
  UPROPERTY(EditAnywhere, Category = "Character") FName HeadBone;
  UPROPERTY(EditAnywhere, Category = "Character")
  FRotator HeadAxes = FRotator::ZeroRotator;
  UPROPERTY(EditAnywhere, Category = "Preview")
  TObjectPtr<UAnimSequence> Animation;
  UPROPERTY(EditAnywhere, Category = "Preview") bool bPlayAnimation = true;
  UPROPERTY(EditAnywhere, Category = "Preview",
            meta = (ClampMin = "-180", ClampMax = "180"))
  float LightYaw = -45;
  UPROPERTY(EditAnywhere, Category = "Preview",
            meta = (ClampMin = "-89", ClampMax = "89"))
  float LightPitch = -35;
  UPROPERTY(EditAnywhere, Category = "Surfaces",
            meta = (TitleProperty = "SlotName"))
  TArray<FPadmaManualSlot> Slots;
  virtual bool IsEditorOnly() const override { return true; }
  UFUNCTION(BlueprintCallable, Category = "NPR") void ReadSlots();
  bool Validate(FString &Error) const;
  UFUNCTION(BlueprintCallable, Category = "NPR")
  UPadmaNPRProfile *Build(bool bSave, FString &Error);
};
