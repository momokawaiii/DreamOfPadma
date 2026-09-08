#include "Demo/World/DemoWorldFixture.h"

FDemoWorldTileDefinition::FDemoWorldTileDefinition()
	: DemoNodeId(FName(TEXT("Node.Demo.Sandbox")))
	, TargetScenarioId(FName(TEXT("Scenario.Demo.Local")))
	, SpawnPointId(FName(TEXT("Spawn.Demo.Entry")))
	, DisplayLocation(FVector::ZeroVector)
	, BaseColor(FLinearColor(0.12f, 0.35f, 0.8f, 1.0f))
	, DisplayName(FText::FromString(TEXT("Demo Tile")))
	, bSelectable(true)
{
}

bool FDemoWorldTileDefinition::TryBuildTransitionContext(
	FDemoTransitionContext& OutContext,
	FText& OutFailure) const
{
	if (!bSelectable)
	{
		OutFailure = FText::FromString(TEXT("The selected demo fixture is not selectable."));
		return false;
	}

	FDemoTransitionContext Candidate;
	Candidate.DemoNodeId = DemoNodeId;
	Candidate.TargetScenarioId = TargetScenarioId;
	Candidate.SpawnPointId = SpawnPointId;
	if (!Candidate.TryValidate(OutFailure))
	{
		return false;
	}

	OutContext = Candidate;
	return true;
}

void FDemoWorldSelectionModel::ReplaceFixture(const TArray<FDemoWorldTileDefinition>& InTiles)
{
	FixtureByNode.Reset();
	for (const FDemoWorldTileDefinition& Tile : InTiles)
	{
		if (Tile.DemoNodeId.IsValid())
		{
			FixtureByNode.Add(Tile.DemoNodeId, Tile);
		}
	}

	if (!bHasSelection || !FixtureByNode.Contains(SelectedNodeId))
	{
		Deselect();
	}
}

bool FDemoWorldSelectionModel::Select(const FDemoSelectNodeRequest& Request, FText& OutFailure)
{
	if (!Request.DemoNodeId.IsValid())
	{
		OutFailure = FText::FromString(TEXT("Selection request is missing a demo node ID."));
		return false;
	}

	const FDemoWorldTileDefinition* Tile = FixtureByNode.Find(Request.DemoNodeId);
	if (Tile == nullptr)
	{
		OutFailure = FText::FromString(FString::Printf(
			TEXT("No demo fixture exists for node '%s'."),
			*Request.DemoNodeId.ToDebugString()));
		return false;
	}

	if (!Tile->bSelectable)
	{
		OutFailure = FText::FromString(FString::Printf(
			TEXT("Demo node '%s' is not selectable."),
			*Request.DemoNodeId.ToDebugString()));
		return false;
	}

	SelectedNodeId = Request.DemoNodeId;
	bHasSelection = true;
	OutFailure = FText::GetEmpty();
	return true;
}

void FDemoWorldSelectionModel::Deselect()
{
	SelectedNodeId = FDemoNodeId();
	bHasSelection = false;
}

bool FDemoWorldSelectionModel::TryGetSelectedNodeId(FDemoNodeId& OutNodeId) const
{
	if (!bHasSelection)
	{
		return false;
	}

	OutNodeId = SelectedNodeId;
	return true;
}

bool FDemoWorldSelectionModel::IsSelected(const FDemoNodeId& NodeId) const
{
	return bHasSelection && SelectedNodeId == NodeId;
}

bool FDemoWorldSelectionModel::BuildTransitionContext(
	const FDemoConfirmNodeSelectionRequest& Request,
	FDemoTransitionContext& OutContext,
	FText& OutFailure) const
{
	if (!bHasSelection)
	{
		OutFailure = FText::FromString(TEXT("Select a demo tile before confirming entry."));
		return false;
	}

	if (!Request.DemoNodeId.IsValid() || Request.DemoNodeId != SelectedNodeId)
	{
		OutFailure = FText::FromString(TEXT("The confirm request does not match the selected demo tile."));
		return false;
	}

	const FDemoWorldTileDefinition* Tile = FixtureByNode.Find(SelectedNodeId);
	if (Tile == nullptr)
	{
		OutFailure = FText::FromString(TEXT("The selected demo fixture is missing."));
		return false;
	}

	FDemoTransitionContext Candidate;
	if (!Tile->TryBuildTransitionContext(Candidate, OutFailure))
	{
		return false;
	}

	OutContext = Candidate;
	return true;
}

const FDemoWorldTileDefinition* FDemoWorldSelectionModel::FindTile(const FDemoNodeId& NodeId) const
{
	return FixtureByNode.Find(NodeId);
}
