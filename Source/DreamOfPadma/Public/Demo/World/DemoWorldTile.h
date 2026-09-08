#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Demo/World/DemoWorldFixture.h"
#include "DemoWorldTile.generated.h"

class UMaterialInstanceDynamic;
class UStaticMeshComponent;
class UTextRenderComponent;

/** Presentation-only tile actor. Picking it immediately converts to a typed node ID. */
UCLASS()
class DREAMOFPADMA_API ADemoWorldTile : public AActor
{
	GENERATED_BODY()

public:
	ADemoWorldTile();

	void Initialize(const FDemoWorldTileDefinition& InDefinition);

	void SetSelected(bool bInSelected);

	const FDemoWorldTileDefinition& GetDefinition() const
	{
		return Definition;
	}

	const FDemoNodeId& GetDemoNodeId() const
	{
		return Definition.DemoNodeId;
	}

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> TileMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTextRenderComponent> Label;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> TileMaterial;

	FDemoWorldTileDefinition Definition;
	bool bSelected = false;

	void ApplyVisualState();
};
