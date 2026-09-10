#include "World/Map/PadmaMapVisualTheme.h"

FPadmaTerrainVisual UPadmaMapVisualTheme::ResolveTerrain(FName Terrain) const
{
	if (const auto* Found = TerrainVisuals.FindByPredicate([Terrain](const auto& Entry) { return Entry.TerrainId == Terrain; })) { return *Found; }
	FPadmaTerrainVisual Result;
	Result.TerrainId = Terrain;
	if (Terrain == TEXT("forest")) { Result.Color = FLinearColor(.13f, .3f, .22f); Result.Height = 24; }
	else if (Terrain == TEXT("mountain") || Terrain == TEXT("hill")) { Result.Color = FLinearColor(.39f, .43f, .44f); Result.Height = 55; }
	else if (Terrain == TEXT("water") || Terrain == TEXT("river")) { Result.Color = FLinearColor(.1f, .36f, .43f); Result.Height = 8; }
	else if (Terrain == TEXT("desert") || Terrain == TEXT("fire")) { Result.Color = FLinearColor(.48f, .39f, .23f); Result.Height = 18; }
	else if (Terrain == TEXT("snow")) { Result.Color = FLinearColor(.66f, .75f, .75f); Result.Height = 32; }
	return Result;
}

const FPadmaMapNodeVisualBinding* UPadmaMapVisualTheme::FindBinding(FName NodeId) const
{
	return NodeBindings.FindByPredicate([NodeId](const auto& Binding) { return Binding.NodeId == NodeId; });
}
