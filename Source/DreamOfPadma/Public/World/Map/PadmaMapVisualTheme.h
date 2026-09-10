#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PadmaMapVisualTheme.generated.h"

class UMaterialInterface;
class UStaticMesh;
class UPadmaModelDefinition;

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaTerrainVisual
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName TerrainId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FLinearColor Color = FLinearColor(0.22f, 0.35f, 0.29f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", ClampMax="180")) double Height = 18;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftObjectPtr<UMaterialInterface> GroundMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftObjectPtr<UStaticMesh> GroundMesh;
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaMapNodeVisualBinding
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName NodeId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftObjectPtr<UPadmaModelDefinition> GroundModel;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftObjectPtr<UPadmaModelDefinition> BuildingModel;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftObjectPtr<UPadmaModelDefinition> NPCModel;
};

/** Cosmetic map authoring only. Never feeds generated graph topology or gameplay randomness. */
UCLASS(BlueprintType)
class DREAMOFPADMA_API UPadmaMapVisualTheme : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Terrain") TSoftObjectPtr<UMaterialInterface> VertexColorMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Terrain") TArray<FPadmaTerrainVisual> TerrainVisuals;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Terrain", meta=(ClampMin="60", ClampMax="200")) double HexRadius = 112;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Terrain", meta=(ClampMin="100", ClampMax="1000")) double FieldMargin = 520;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Terrain", meta=(ClampMin="16", ClampMax="100")) double SurfaceGridSize = 32;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Terrain", meta=(ClampMin="0", ClampMax="400")) double HillRelief = 80;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Terrain", meta=(ClampMin="0", ClampMax="700")) double MountainRelief = 310;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Terrain") bool bShowRiver = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Terrain") TSoftObjectPtr<UMaterialInterface> WaterMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Nodes") TArray<FPadmaMapNodeVisualBinding> NodeBindings;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Feedback") FLinearColor SelectedColor = FLinearColor(1.f, .72f, .2f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Feedback") FLinearColor HoverColor = FLinearColor(.2f, .9f, 1.f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PCG") TArray<TSoftObjectPtr<UStaticMesh>> DecorationMeshes;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PCG") TSoftObjectPtr<UMaterialInterface> DecorationMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PCG", meta=(ClampMin="0", ClampMax="2400")) int32 DecorationCount = 700;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PCG", meta=(ClampMin="0", ClampMax="60")) double DecorationMaxSlopeDegrees = 32;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PCG", meta=(ClampMin="0.05", ClampMax="3")) double DecorationScaleMin = .3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PCG", meta=(ClampMin="0.05", ClampMax="3")) double DecorationScaleMax = .75;

	FPadmaTerrainVisual ResolveTerrain(FName Terrain) const;
	const FPadmaMapNodeVisualBinding* FindBinding(FName NodeId) const;
};
