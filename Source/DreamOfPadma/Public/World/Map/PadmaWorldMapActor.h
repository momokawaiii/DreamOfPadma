#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Content/PadmaContentTypes.h"
#include "World/PCG/PadmaLandscapeSurface.h"
#include "PadmaWorldMapActor.generated.h"

class UProceduralMeshComponent;
class UTextRenderComponent;
class UBoxComponent;
class UPCGComponent;
class UPCGGraph;
class UPadmaMapVisualTheme;
class APadmaModelPresentationActor;
class UPadmaModelDefinition;

struct FPadmaWorldNodeView
{
	FName Id;
	FText Name;
	FString Detail;
	FVector Position = FVector::ZeroVector;
	FLinearColor Color = FLinearColor::White;
	FName Terrain;
	FName Type;
	FName Owner;
	int32 FriendlyCount = 0;
	int32 EnemyCount = 0;
	EPadmaMapAnchorKind AnchorKind = EPadmaMapAnchorKind::None;
	FName NPCId;
	bool bAnchorCompleted = false;
};
struct FPadmaWorldEdgeView { FName From; FName To; bool bUnlocked = true; };

/** Clickable visual proxy; NodeId is a lookup key, never mutable node authority. */
UCLASS()
class DREAMOFPADMA_API APadmaWorldNodeActor : public AActor
{
	GENERATED_BODY()
public:
	APadmaWorldNodeActor();
	UPROPERTY(VisibleAnywhere) TObjectPtr<UProceduralMeshComponent> Tile;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UProceduralMeshComponent> Feedback;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UProceduralMeshComponent> Markers;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UTextRenderComponent> Caption;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UTextRenderComponent> Detail;
	FName NodeId;
	double SurfaceHeight = 20;
	void BuildStatic(const FPadmaWorldNodeView& View, UPadmaMapVisualTheme* Theme);
	void SetView(const FPadmaWorldNodeView& View, bool bSelected, bool bHovered, UPadmaMapVisualTheme* Theme);
	void SetVisible(bool bVisible);
protected:
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	virtual void Destroyed() override;
private:
	UPROPERTY() TArray<TObjectPtr<APadmaModelPresentationActor>> Models;
	FString MarkerKey;
	FString FeedbackKey;
};

/** Cached cosmetic graph, continuous terrain field and PCG decoration. No rule/travel authority. */
UCLASS()
class DREAMOFPADMA_API APadmaWorldMapActor : public AActor
{
	GENERATED_BODY()
public:
	APadmaWorldMapActor();
	void ShowGraph(const TArray<FPadmaWorldNodeView>& Nodes, const TArray<FPadmaWorldEdgeView>& Edges, FName Selected);
	void ClearGraph();
	void SetHoveredNode(FName NodeId);
	void SetMoveSource(FName NodeId);
	void SetVisible(bool bVisible);
	void ConfigureTheme(UPadmaMapVisualTheme* Theme, int32 Seed);
	FBox GetMapBounds() const { return MapBounds; }
	double SampleSurfaceHeight(const FVector2D& Position) const { return Landscape.Height(Position); }
	FLinearColor SampleSurfaceColor(const FVector2D& Position) const { return Landscape.Color(Position); }
	static FVector NodeLocation(const FVector2D& Position);
	UPROPERTY(VisibleAnywhere, Category="Map") TObjectPtr<UBoxComponent> BoundsVolume;
	UPROPERTY(VisibleAnywhere, Category="Map") TObjectPtr<UProceduralMeshComponent> TerrainField;
	UPROPERTY(VisibleAnywhere, Category="Map") TObjectPtr<UProceduralMeshComponent> WaterGeometry;
	UPROPERTY(VisibleAnywhere, Category="Map") TObjectPtr<UProceduralMeshComponent> RoadGeometry;
	UPROPERTY(VisibleAnywhere, Category="PCG") TObjectPtr<UPCGComponent> DecorationPCG;
	UPROPERTY(VisibleAnywhere, Transient, Category="PCG") int32 GeneratedDecorationInstances = 0;
	UPROPERTY(VisibleAnywhere, Transient, Category="Map") int32 LayoutBuildCount = 0;
	UPROPERTY(VisibleAnywhere, Transient, Category="Map") int32 TerrainTriangleCount = 0;
protected:
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	virtual void Destroyed() override;
private:
	UPROPERTY() TMap<FName,TObjectPtr<APadmaWorldNodeActor>> Tiles;
	UPROPERTY() TObjectPtr<UPadmaMapVisualTheme> VisualTheme;
	UPROPERTY() TObjectPtr<UPCGGraph> DecorationGraph;
	TArray<FPadmaWorldNodeView> LastNodes;
	TArray<FPadmaWorldEdgeView> LastEdges;
	FBox MapBounds = FBox(ForceInit);
	FPadmaLandscapeSurface Landscape;
	FString LayoutKey;
	FString RoadKey;
	FName SelectedNode;
	FName HoveredNode;
	FName MoveSource;
	int32 DecorationSeed = 12345;
	int32 RequestedDecorationPoints = 0;
	bool bMapVisible = true;
	void RebuildStatic();
	void RefreshDynamic();
	void RebuildRoads();
	void GenerateDecoration();
	UFUNCTION() void OnDecorationGenerated(UPCGComponent* Component);
};
