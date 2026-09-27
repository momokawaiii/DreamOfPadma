#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PadmaNPRProfile.generated.h"
class UMaterialInterface;
class UCurveFloat;
UENUM(BlueprintType)
enum class EPadmaNPRSurface : uint8 {
  Face,
  Skin,
  Hair,
  Eye,
  HairShadow,
  Cloth,
  HairLine,
  Brow,
  EyeShadow
};
USTRUCT(BlueprintType)
struct PADMANPRRUNTIME_API FPadmaNPRFaceSettings {
  GENERATED_BODY()
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Face")
  float ThresholdOffset = 0.f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Face",
            meta = (ClampMin = "0.001", ClampMax = "1"))
  float Feather = .04f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Face",
            meta = (ClampMin = "0", ClampMax = "1"))
  float OverheadBlend = .3f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Face",
            meta = (ClampMin = "0", ClampMax = "0.1"))
  float BlurUV = .002f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Face",
            meta = (ClampMin = "0", ClampMax = "1"))
  float SDFWeight = 1.f;
};
USTRUCT(BlueprintType)
struct PADMANPRRUNTIME_API FPadmaNPRSkinSettings {
  GENERATED_BODY()
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skin")
  FLinearColor ScatterColor = FLinearColor(1.f, .28f, .16f, 1.f);
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skin",
            meta = (ClampMin = "0", ClampMax = "1"))
  float Warmth = .12f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skin",
            meta = (ClampMin = "0", ClampMax = "1"))
  float Wrap = .35f;
};
USTRUCT(BlueprintType)
struct PADMANPRRUNTIME_API FPadmaNPRHairSettings {
  GENERATED_BODY()
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair")
  FLinearColor HighlightColor = FLinearColor(.65f, .7f, .8f, 1.f);
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair",
            meta = (ClampMin = "1", ClampMax = "1024"))
  float WidePower = 40.f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair",
            meta = (ClampMin = "1", ClampMax = "1024"))
  float NarrowPower = 200.f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair",
            meta = (ClampMin = "0", ClampMax = "8"))
  float WideStrength = .3f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair",
            meta = (ClampMin = "0", ClampMax = "8"))
  float NarrowStrength = .4f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair")
  float WideShift = .1f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair")
  float NarrowShift = -.15f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair",
            meta = (ClampMin = "0", ClampMax = "1"))
  float StrandNoise = .1f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair",
            meta = (ClampMin = "0", ClampMax = "1"))
  float Backlit = .1f;
  // Explicit NPR_RegionMask.R identifies bangs; default black cannot dissolve
  // the whole hairstyle.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bangs",
            meta = (ClampMin = "0", ClampMax = "1"))
  float BangOpacity = 1.f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bangs",
            meta = (ClampMin = "0", ClampMax = "1"))
  float FrontViewOnly = 1.f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bangs",
            meta = (ClampMin = "-10", ClampMax = "30", Units = "cm"))
  float BangMaxHeight = 4.f;
};
USTRUCT(BlueprintType)
struct PADMANPRRUNTIME_API FPadmaNPREyeSettings {
  GENERATED_BODY()
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Eye",
            meta = (ClampMin = "0", ClampMax = "1"))
  float MatcapWeight = .15f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Eye",
            meta = (ClampMin = "0", ClampMax = "1"))
  float Fresnel = .15f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Eye",
            meta = (ClampMin = "1", ClampMax = "1024"))
  float SpecularPower = 128.f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Eye",
            meta = (ClampMin = "0", ClampMax = "1"))
  float SpecularStrength = .4f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Eye",
            meta = (ClampMin = "0", ClampMax = "0.05"))
  float IrisDepth = 0.f;
};
USTRUCT(BlueprintType)
struct PADMANPRRUNTIME_API FPadmaNPRSlot {
  GENERATED_BODY()
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Binding")
  FName SlotName;
  // Source MI owns textures. Generated editor instances and runtime MIDs never
  // mutate it.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Binding")
  TObjectPtr<UMaterialInterface> Material;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Binding")
  EPadmaNPRSurface Type = EPadmaNPRSurface::Face;
  // Reference variants preserve source parameter semantics instead of mapping
  // them onto the intentionally smaller generic NPR controls below.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
  bool bReferenceResponse = false;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference",
            meta = (EditCondition = "bReferenceResponse"))
  TMap<FName, float> ReferenceScalars;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference",
            meta = (EditCondition = "bReferenceResponse"))
  TMap<FName, FLinearColor> ReferenceVectors;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity",
            meta = (ClampMin = "0", ClampMax = "65535"))
  int32 MaterialId = 0;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity",
            meta = (ClampMin = "0", ClampMax = "65535"))
  int32 MaterialRegionId = 0;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common")
  FLinearColor Tint = FLinearColor::White;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common")
  FLinearColor ShadowColor = FLinearColor(.58f, .45f, .48f, 1.f);
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common")
  FLinearColor FillColor = FLinearColor(1.f, .92f, .86f, 1.f);
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common",
            meta = (ClampMin = "0", ClampMax = "1"))
  float Threshold = .45f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common",
            meta = (ClampMin = "0.001", ClampMax = "1"))
  float Feather = .1f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common",
            meta = (ClampMin = "0", ClampMax = "1"))
  float FillIntensity = .12f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common",
            meta = (ClampMin = "0", ClampMax = "1"))
  float AOWeight = .5f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common",
            meta = (ClampMin = "0", ClampMax = "1"))
  float RimStrength = .05f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common",
            meta = (ClampMin = "0", ClampMax = "1"))
  float ExposureCompensation = 1.f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug",
            meta = (ClampMin = "0", ClampMax = "4"))
  int32 Debug = 0;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Face")
  FPadmaNPRFaceSettings Face;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skin")
  FPadmaNPRSkinSettings Skin;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair")
  FPadmaNPRHairSettings Hair;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Eye")
  FPadmaNPREyeSettings Eye;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HairShadow",
            meta = (ClampMin = "0", ClampMax = "1"))
  float HelperOpacity = .3f;
  void GetParameters(int32 ProfileId, TMap<FName, FLinearColor> &Out) const;
};
UCLASS(BlueprintType)
class PADMANPRRUNTIME_API UPadmaNPRProfile : public UDataAsset {
  GENERATED_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity",
            meta = (ClampMin = "0", ClampMax = "65535"))
  int32 ProfileId = 1;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Binding")
  FName HeadSocket = TEXT("NoseMd02Joint");
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Binding")
  FRotator HeadAxisCorrection = FRotator::ZeroRotator;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
  FVector FallbackLightDirection = FVector(.3f, .5f, 1.f);
  // Optional authored angle-to-SDF response, evaluated on CPU using UE's curve
  // interpolation (including weighted tangents). CPD 7=value, 11=valid.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
  TObjectPtr<UCurveFloat> ReferenceSDFAngleCurve;
  // Project the key light onto the calibrated head plane. Off preserves the
  // original reference's 3D-angle/world-Z convention.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
  bool bReferencePlanarSDF = false;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Surfaces")
  TArray<FPadmaNPRSlot> Slots;
  bool Validate(FString &Error) const;
};
