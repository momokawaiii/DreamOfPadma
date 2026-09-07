#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Demo/World/DemoWorldFixture.h"
#include "DemoSandboxWorld.generated.h"

class UCameraComponent;
class UStaticMeshComponent;
class UTextRenderComponent;
class UDemoTransitionSessionSubsystem;
class ADemoWorldTile;

/**
 * Project-owned fixed Demo Sandbox presentation. It translates mouse/keyboard
 * input into typed requests and delegates cross-level state to the session
 * subsystem.
 */
UCLASS()
class DREAMOFPADMA_API ADemoSandboxWorld : public AActor
{
	GENERATED_BODY()

public:
	ADemoSandboxWorld();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** The one playtest fixture is editable for fixture validation, not save identity. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo Fixture")
	TArray<FDemoWorldTileDefinition> FixtureTiles;

	bool SelectTile(const FDemoSelectNodeRequest& Request, FText& OutFailure);
	void DeselectTile();
	bool ConfirmSelectedTile(
		const FDemoConfirmNodeSelectionRequest& Request,
		FDemoTransitionContext& OutContext,
		FText& OutFailure);

	/** Narrow fixture failure test hooks; these do not mutate session state. */
	void InvalidateSelectedFixtureForTesting();
	void RestoreFixtureForTesting();
	void ConsumePendingTransitionForTesting();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Ground;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTextRenderComponent> TitleText;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTextRenderComponent> StatusText;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> DemoCamera;

	UPROPERTY(Transient)
	TMap<FDemoNodeId, TObjectPtr<ADemoWorldTile>> TileActors;

	TArray<FDemoWorldTileDefinition> OriginalFixtureTiles;
	FDemoWorldSelectionModel SelectionModel;

	void SpawnTileActors();
	void ConfigurePlayerViewAndInput();
	void RefreshTilePresentation();
	void SetStatus(const FText& Status, const FColor& Color = FColor::White);

	const FDemoWorldTileDefinition* FindAuthoredFixture(const FDemoNodeId& NodeId) const;
	ADemoWorldTile* FindTileActor(const FDemoNodeId& NodeId) const;
	UDemoTransitionSessionSubsystem* GetSessionSubsystem() const;
	APlayerController* GetPlayerController() const;

	void HandlePrimaryClick();
	void HandleCancelSelection();
	void HandleConfirmSelection();
	void HandleInvalidateFixture();
	void HandleRestoreFixture();
	void HandleConsumeTransition();
};
