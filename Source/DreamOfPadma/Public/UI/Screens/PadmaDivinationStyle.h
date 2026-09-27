#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Curves/CurveFloat.h"
#include "PadmaDivinationStyle.generated.h"

class UMaterialInterface;
class UTexture2D;
class UFontFace;

/** A presentation fixture; these cards grant no rewards and never enter the run. */
USTRUCT(BlueprintType)
struct FPadmaDivinationCard
{
 GENERATED_BODY()
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Card") FText Title;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Card") FText Subtitle;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Card", meta=(ClampMin="0",ClampMax="8")) int32 ArtIndex=0;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Motion") FVector2D Destination=FVector2D::ZeroVector;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Motion", meta=(ClampMin="0")) float StartTime=.383f;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Motion", meta=(ClampMin="0.01")) float Duration=.834f;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Motion") FVector StartOffset=FVector(136,-395,559);
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Motion", meta=(ClampMin="0.01")) float StartScale=.072f;
 // XYZ degrees about screen-space axes, including authored full revolutions.
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Motion") FVector RotationDegrees=FVector(695,0,0);
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Motion") FVector FinalRotation=FVector::ZeroVector;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Motion") FVector2D CardSize=FVector2D(290,445);
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Motion") bool bReversed=false;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Motion") FRuntimeFloatCurve Travel;
};

/** Designer-editable timing and art for the isolated daily-divination presentation. */
UCLASS(BlueprintType)
class DREAMOFPADMA_API UPadmaDivinationStyle : public UDataAsset
{
 GENERATED_BODY()
public:
 UPadmaDivinationStyle();
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Timing", meta=(ClampMin="0.1")) float EnterDuration=2.f;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Timing", meta=(ClampMin="0.1")) float RevealDuration=2.267f;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Timing", meta=(ClampMin="0.05")) float CloseDuration=.417f;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Timing", meta=(ClampMin="0.1",ClampMax="3")) float PlaybackRate=1.f;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Art") FLinearColor Accent=FLinearColor(.015f,.32f,1.f);
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Art") TSoftObjectPtr<UFontFace> SerifFont;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reference") TMap<FName,TSoftObjectPtr<UTexture2D>> OriginalArt;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reference") FString OriginalLayoutJson;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reference") TMap<FName,TSoftObjectPtr<UFontFace>> OriginalFonts;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Art") FVector2D HeroPosition=FVector2D(290,1623.75);
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Art") float HeroScale=.925f;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Art") bool bRomanNumerals=false;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Timing") float StartupDuration=6.5f;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Art") FText Heading;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Art") FText ResultHeading;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Art") TSoftObjectPtr<UTexture2D> CardAtlas;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Art") TSoftObjectPtr<UMaterialInterface> WaterMaterial;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Art") TSoftObjectPtr<UMaterialInterface> CardMaterial;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reference") TMap<FName,TSoftObjectPtr<UMaterialInterface>> PageMaterials;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Art", meta=(ClampMin="0",ClampMax="1")) float WaterStrength=.75f;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Art", meta=(ClampMin="0",ClampMax="20")) float ParallaxDegrees=6.5f;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cards", meta=(TitleProperty="Title")) TArray<FPadmaDivinationCard> Cards;
};
