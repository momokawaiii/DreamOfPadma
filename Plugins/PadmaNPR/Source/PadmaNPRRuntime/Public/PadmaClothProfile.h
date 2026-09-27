#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PadmaClothProfile.generated.h"
class UMaterialInterface;
class UMaterialInstanceDynamic;
UENUM(BlueprintType)
enum class EPadmaClothDiffuseMode : uint8
{
    Threshold,
    ToonRemap,
    ZMDAdjust
};

USTRUCT(BlueprintType)
struct PADMANPRRUNTIME_API FPadmaClothBase
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
    FLinearColor Tint = FLinearColor::White;
};

USTRUCT(BlueprintType)
struct PADMANPRRUNTIME_API FPadmaClothNormal
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Normal")
    float MapWeight = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Normal")
    float CameraWeight = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Normal")
    float LightWeight = 0.f;
};

USTRUCT(BlueprintType)
struct PADMANPRRUNTIME_API FPadmaClothDiffuse
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Diffuse")
    float Threshold = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Diffuse")
    float Feather = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Diffuse")
    float Strength = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Diffuse")
    EPadmaClothDiffuseMode Mode = EPadmaClothDiffuseMode::ToonRemap;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Diffuse")
    FLinearColor ShadowColor = FLinearColor(0.5f, 0.5f, 0.5f, 0.f);
};

USTRUCT(BlueprintType)
struct PADMANPRRUNTIME_API FPadmaClothAO
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AO")
    float Smooth = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AO")
    float Offset = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AO")
    float Strength = 0.f;
};

USTRUCT(BlueprintType)
struct PADMANPRRUNTIME_API FPadmaClothRimShadow
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RimShadow")
    float Threshold = 0.7f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RimShadow")
    float Feather = 0.1f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RimShadow")
    float Strength = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RimShadow")
    FLinearColor Color = FLinearColor(0.5f, 0.5f, 0.5f, 0.f);
};

USTRUCT(BlueprintType)
struct PADMANPRRUNTIME_API FPadmaClothRimHighlight
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RimHighlight")
    float Threshold = 0.8f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RimHighlight")
    float Feather = 0.1f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RimHighlight")
    float Strength = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RimHighlight")
    FLinearColor Color = FLinearColor::White;
};

USTRUCT(BlueprintType)
struct PADMANPRRUNTIME_API FPadmaClothSpecular
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Specular")
    float BaseRoughness = 0.5f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Specular")
    float MetalRoughness = 0.3f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Specular")
    float RemapSmooth = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Specular")
    float RemapOffset = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Specular")
    float Strength = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Specular")
    FLinearColor BaseColor = FLinearColor::White;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Specular")
    FLinearColor MetalColor = FLinearColor::White;
};

USTRUCT(BlueprintType)
struct PADMANPRRUNTIME_API FPadmaClothMatcap
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Matcap")
    float Strength01 = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Matcap")
    float Strength02 = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Matcap")
    FLinearColor Tint01 = FLinearColor::White;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Matcap")
    FLinearColor Tint02 = FLinearColor::White;
};

USTRUCT(BlueprintType)
struct PADMANPRRUNTIME_API FPadmaClothSurface
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Surface")
    float MetallicScale = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Surface")
    float Specular = 0.5f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Surface")
    float EmissiveWeight = 0.f;
};

// Optional reference response. Legacy materials retain their original 16-parameter contract.
USTRUCT(BlueprintType)
struct PADMANPRRUNTIME_API FPadmaClothReference
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
    bool Enabled = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
    float BaseColorPower = 1.3f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
    float LamNormalDetail = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
    float RimShadowNormalDetail = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
    float RimHighlightNormalDetail = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
    float MatcapNormalDetail = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
    float GGXNormalDetail = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
    float GGXCameraWeight = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
    float GGXMetalStrength = 0.570667f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
    float GGXLamStrength = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
    float MetalMaskSmooth = 4.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
    float MetalMaskOffset = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
    float NativeMetallic = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
    float NativeRoughness = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
    float RoughnessMapEndpoint = 0.8f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
    float RoughnessScale = 0.3f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
    float ExposureCompensation = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
    float InverseTonemapStrength = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
    FLinearColor AOColor = FLinearColor(0.331597f,0.331597f,0.331597f,1.f);
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
    FLinearColor MatcapUV = FLinearColor(1.f,0.194203f,0.735f,0.5f);
};

USTRUCT(BlueprintType)
struct PADMANPRRUNTIME_API FPadmaClothSettings
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth") FPadmaClothBase Base;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth") FPadmaClothNormal Normal;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth") FPadmaClothDiffuse Diffuse;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth") FPadmaClothAO AO;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth") FPadmaClothRimShadow RimShadow;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth") FPadmaClothRimHighlight RimHighlight;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth") FPadmaClothSpecular Specular;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth") FPadmaClothMatcap Matcap;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth") FPadmaClothSurface Surface;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference") FPadmaClothReference Reference;
    void ToMaterialParameters(TMap<FName, FLinearColor> &Out) const;
};

USTRUCT(BlueprintType)
struct PADMANPRRUNTIME_API FPadmaClothSlotOverride
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Binding") FName SlotName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base") bool bOverrideBase = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base", meta = (EditCondition = "bOverrideBase"))
    FPadmaClothBase Base;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Normal") bool bOverrideNormal = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Normal", meta = (EditCondition = "bOverrideNormal"))
    FPadmaClothNormal Normal;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Diffuse") bool bOverrideDiffuse = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Diffuse", meta = (EditCondition = "bOverrideDiffuse"))
    FPadmaClothDiffuse Diffuse;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AO") bool bOverrideAO = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AO", meta = (EditCondition = "bOverrideAO"))
    FPadmaClothAO AO;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RimShadow") bool bOverrideRimShadow = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RimShadow", meta = (EditCondition = "bOverrideRimShadow"))
    FPadmaClothRimShadow RimShadow;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RimHighlight") bool bOverrideRimHighlight = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RimHighlight",
              meta = (EditCondition = "bOverrideRimHighlight"))
    FPadmaClothRimHighlight RimHighlight;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Specular") bool bOverrideSpecular = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Specular", meta = (EditCondition = "bOverrideSpecular"))
    FPadmaClothSpecular Specular;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Matcap") bool bOverrideMatcap = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Matcap", meta = (EditCondition = "bOverrideMatcap"))
    FPadmaClothMatcap Matcap;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Surface") bool bOverrideSurface = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Surface", meta = (EditCondition = "bOverrideSurface"))
    FPadmaClothSurface Surface;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference") bool bOverrideReference = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference", meta = (EditCondition = "bOverrideReference"))
    FPadmaClothReference Reference;
};

UCLASS(BlueprintType)
class PADMANPRRUNTIME_API UPadmaClothProfile : public UDataAsset
{
    GENERATED_BODY()
  public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cloth") FPadmaClothSettings Defaults;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cloth") TArray<FPadmaClothSlotOverride> SlotOverrides;
    bool Resolve(FName Slot, FPadmaClothSettings &Out, FString &Error) const;
};

UCLASS()
class PADMANPRRUNTIME_API UPadmaClothLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
  public:
    // Explicit authoring snapshot: callers can bake the resolved values into a project MI.
    UFUNCTION(BlueprintCallable, Category = "Padma NPR|Cloth")
    static TMap<FName, FLinearColor> GetClothParameters(const UPadmaClothProfile* Profile, FName SlotName, FString& Error);
    // Explicit factory only. Caller owns assigning/restoring this MID; no mesh or world is modified here.
    UFUNCTION(BlueprintCallable, Category = "Padma NPR|Cloth", meta = (DefaultToSelf = "Owner"))
    static UMaterialInstanceDynamic *CreateClothMID(UObject *Owner, UMaterialInterface *TextureBinding,
                                                    const UPadmaClothProfile *Profile, FName SlotName, FString &Error);
};
