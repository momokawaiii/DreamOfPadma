#pragma once

#include "CoreMinimal.h"

#include "Demo/Session/DemoTransitionTypes.h"
#include "DemoWorldFixture.generated.h"

/**
 * Authored presentation fixture for one selectable Demo Sandbox tile. The
 * display location is rebuildable presentation data; the three IDs are the
 * only values that cross the transition boundary.
 */
USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FDemoWorldTileDefinition
{
	GENERATED_BODY()

	FDemoWorldTileDefinition();

	bool TryBuildTransitionContext(FDemoTransitionContext& OutContext, FText& OutFailure) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo Fixture")
	FDemoNodeId DemoNodeId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo Fixture")
	FDemoScenarioId TargetScenarioId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo Fixture")
	FDemoSpawnPointId SpawnPointId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo Presentation")
	FVector DisplayLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo Presentation")
	FLinearColor BaseColor = FLinearColor(0.12f, 0.35f, 0.8f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo Presentation")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo Fixture")
	bool bSelectable = true;
};

/**
 * Small command-facing selection model. It owns only presentation selection
 * state and fixture lookup; it never owns persistent world mutation.
 */
class DREAMOFPADMA_API FDemoWorldSelectionModel
{
public:
	void ReplaceFixture(const TArray<FDemoWorldTileDefinition>& InTiles);

	bool Select(const FDemoSelectNodeRequest& Request, FText& OutFailure);

	void Deselect();

	bool TryGetSelectedNodeId(FDemoNodeId& OutNodeId) const;

	bool IsSelected(const FDemoNodeId& NodeId) const;

	bool BuildTransitionContext(
		const FDemoConfirmNodeSelectionRequest& Request,
		FDemoTransitionContext& OutContext,
		FText& OutFailure) const;

	const FDemoWorldTileDefinition* FindTile(const FDemoNodeId& NodeId) const;

private:
	TMap<FDemoNodeId, FDemoWorldTileDefinition> FixtureByNode;
	FDemoNodeId SelectedNodeId;
	bool bHasSelection = false;
};
