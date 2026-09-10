#include "Misc/AutomationTest.h"
#include "Core/WorldMap/PadmaWorldMapDefinition.h"
#include "Game/WorldMap/PadmaWorldMapAsset.h"
#include "Algo/Reverse.h"
#include <limits>

#if WITH_DEV_AUTOMATION_TESTS
namespace
{
FPadmaWorldNodeDefinitionRow Node(FName Id, FName Type, FVector2D Position)
{
	FPadmaWorldNodeDefinitionRow Result;
	Result.Id = Id; Result.Type = Type; Result.Position = Position;
	Result.DisplayName = FText::FromName(Id); Result.Terrain = TEXT("plain");
	Result.InitialOwner = Type == TEXT("home") ? FName(TEXT("player")) : FName(TEXT("neutral"));
	return Result;
}
FPadmaWorldEdgeDefinitionRow Edge(FName Id, FName From, FName To, FName Flag = NAME_None)
{
	FPadmaWorldEdgeDefinitionRow Result;
	Result.Id = Id; Result.From = From; Result.To = To; Result.UnlockFlag = Flag;
	return Result;
}
FPadmaMapLayout Template()
{
	FPadmaMapLayout Result;
	Result.MapId = TEXT("test-world"); Result.HomeNode = TEXT("camp"); Result.BossNode = TEXT("summit");
	Result.Nodes = {
		Node(TEXT("camp"), TEXT("home"), FVector2D(0, 0)),
		Node(TEXT("npc"), TEXT("story"), FVector2D(20, 10)),
		Node(TEXT("story"), TEXT("story"), FVector2D(45, 20)),
		Node(TEXT("checkpoint"), TEXT("gate"), FVector2D(70, 10)),
		Node(TEXT("summit"), TEXT("boss"), FVector2D(100, 0))
	};
	Result.Nodes[1].AnchorKind = EPadmaMapAnchorKind::FixedNPC;
	Result.Nodes[1].NPCId = TEXT("npc-vendor"); Result.Nodes[1].DialogueId = TEXT("greeting");
	Result.Nodes[2].AnchorKind = EPadmaMapAnchorKind::MainStory;
	Result.Nodes[2].NPCId = TEXT("npc-pilgrim"); Result.Nodes[2].DialogueId = TEXT("meeting");
	Result.Nodes[2].CompletionFlag = TEXT("pilgrim-complete");
	Result.Edges = {
		Edge(TEXT("e1"), TEXT("camp"), TEXT("npc")),
		Edge(TEXT("e2"), TEXT("npc"), TEXT("story")),
		Edge(TEXT("e3"), TEXT("story"), TEXT("checkpoint")),
		Edge(TEXT("e4"), TEXT("npc"), TEXT("checkpoint")),
		Edge(TEXT("e5"), TEXT("checkpoint"), TEXT("summit"), TEXT("pilgrim-missed"))
	};
	FPadmaStoryCheckpointRow Checkpoint;
	Checkpoint.Id = TEXT("pilgrim-branch"); Checkpoint.NodeId = TEXT("checkpoint"); Checkpoint.NPCId = TEXT("npc-pilgrim");
	Checkpoint.MatchedFlag = TEXT("pilgrim-met"); Checkpoint.MissedFlag = TEXT("pilgrim-missed");
	Result.Checkpoints.Add(Checkpoint);
	return Result;
}

TSet<FName> Reach(const FPadmaMapLayout& Layout, FName Start, bool bAllowLocked, FName Excluded = NAME_None)
{
	TSet<FName> Seen = {Start};
	TArray<FName> Queue = {Start};
	for (int32 Index = 0; Index < Queue.Num(); ++Index)
	{
		for (const auto& Link : Layout.Edges)
		{
			if (Link.From == Queue[Index] && Link.To != Excluded && !Seen.Contains(Link.To)
				&& (bAllowLocked || Link.UnlockFlag.IsNone())) { Seen.Add(Link.To); Queue.Add(Link.To); }
		}
	}
	return Seen;
}

FString Topology(const FPadmaMapLayout& Layout)
{
	TArray<FString> Edges;
	for (const auto& Link : Layout.Edges) { Edges.Add(Link.From.ToString() + TEXT("->") + Link.To.ToString()); }
	Edges.Sort();
	return FString::Join(Edges, TEXT(";"));
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaMapDeterminismTest, "DreamOfPadma.WorldMap.Generation.DeterministicTopologyAndFixedAnchors",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPadmaMapDeterminismTest::RunTest(const FString& Parameters)
{
	const auto Authored = Template();
	FPadmaMapGenerationSettings Settings;
	Settings.TerrainChoices = {TEXT("plain"), TEXT("forest")};
	FString Error;
	TSet<FString> Topologies;
	for (int32 Seed = 0; Seed < 64; ++Seed)
	{
		FPadmaMapLayout First, Second;
		if (!FPadmaWorldMapGenerator::Generate(Authored, Settings, Seed, First, Error)
			|| !FPadmaWorldMapGenerator::Generate(Authored, Settings, Seed, Second, Error)) { AddError(Error); return false; }
		TestEqual(TEXT("Same seed yields identical gameplay values"), FPadmaWorldMapGenerator::Signature(First), FPadmaWorldMapGenerator::Signature(Second));
		TestTrue(TEXT("Same seed yields identical reflected layout"), FPadmaMapLayout::StaticStruct()->CompareScriptStruct(&First, &Second, 0));
		TestTrue(TEXT("Generated graph and anchors validate"), FPadmaWorldMapGenerator::ValidateGenerated(Authored, First, Error));
		TestFalse(TEXT("Generated topology cannot bypass the authored road lock"), Reach(First, First.HomeNode, false).Contains(First.BossNode));
		TestTrue(TEXT("Boss remains reachable when authored locks permit it"), Reach(First, First.HomeNode, true).Contains(First.BossNode));
		TestTrue(TEXT("Optional main-story anchor remains bypassable"), Reach(First, First.HomeNode, true, TEXT("story")).Contains(First.BossNode));
		for (const auto& Original : Authored.Nodes)
		{
			const auto* Generated = First.Nodes.FindByPredicate([&](const auto& N) { return N.Id == Original.Id; });
			if (!TestNotNull(TEXT("Every authored node retained"), Generated)) { return false; }
			TestEqual(TEXT("Authored coordinates fixed exactly"), Generated->Position, Original.Position);
			TestEqual(TEXT("Authored NPC identity fixed"), Generated->NPCId, Original.NPCId);
			TestEqual(TEXT("Authored dialogue identity fixed"), Generated->DialogueId, Original.DialogueId);
		}
		Topologies.Add(Topology(First));
	}
	TestTrue(TEXT("Different seeds change edges/node counts, not only decoration or seed metadata"), Topologies.Num() > 1);
	FPadmaMapLayout Reordered = Authored, First, Second;
	Algo::Reverse(Reordered.Nodes); Algo::Reverse(Reordered.Edges);
	FPadmaWorldMapGenerator::Generate(Authored, Settings, 773, First, Error);
	FPadmaWorldMapGenerator::Generate(Reordered, Settings, 773, Second, Error);
	TestEqual(TEXT("Author array order cannot change generation"), FPadmaWorldMapGenerator::Signature(First), FPadmaWorldMapGenerator::Signature(Second));
	Algo::Reverse(Second.Nodes); Algo::Reverse(Second.Edges);
	TestEqual(TEXT("Signature ignores container order"), FPadmaWorldMapGenerator::Signature(First), FPadmaWorldMapGenerator::Signature(Second));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaMapValidationTest, "DreamOfPadma.WorldMap.Generation.InvalidEditsAreRejected",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPadmaMapValidationTest::RunTest(const FString& Parameters)
{
	const auto Authored = Template();
	FString Error;
	TestTrue(TEXT("Authored optional anchor bypass is valid"), FPadmaWorldMapGenerator::Validate(Authored, Error));
	auto LegacyTypes = Authored;
	LegacyTypes.Nodes[0].Type = TEXT("core"); LegacyTypes.Nodes[4].Type = TEXT("rulerCore");
	TestTrue(TEXT("Declared terminals accept real legacy core and rulerCore operation types"), FPadmaWorldMapGenerator::Validate(LegacyTypes, Error));
	auto Invalid = Authored;
	Invalid.Nodes[0].InitialOwner = TEXT("neutral");
	TestFalse(TEXT("Neutral declared home rejected"), FPadmaWorldMapGenerator::Validate(Invalid, Error));
	Invalid = Authored; Invalid.Nodes[0].InitialOwner = TEXT("ruler");
	TestFalse(TEXT("Enemy-owned declared home rejected"), FPadmaWorldMapGenerator::Validate(Invalid, Error));
	Invalid = Authored;
	Invalid.Nodes[1].Position.X = std::numeric_limits<double>::quiet_NaN();
	TestFalse(TEXT("Nonfinite coordinates rejected"), FPadmaWorldMapGenerator::Validate(Invalid, Error));
	Invalid = Authored; Invalid.Nodes.Add(Authored.Nodes[1]);
	TestFalse(TEXT("Duplicate node identity rejected"), FPadmaWorldMapGenerator::Validate(Invalid, Error));
	Invalid = Authored; Invalid.Edges[0].To = TEXT("missing");
	TestFalse(TEXT("Missing edge endpoint rejected"), FPadmaWorldMapGenerator::Validate(Invalid, Error));
	Invalid = Authored; Invalid.Edges[0].To = Invalid.Edges[0].From;
	TestFalse(TEXT("Self edge rejected"), FPadmaWorldMapGenerator::Validate(Invalid, Error));
	Invalid = Authored; Invalid.Edges.Add(Edge(TEXT("duplicate-pair"), TEXT("camp"), TEXT("npc")));
	TestFalse(TEXT("Duplicate directed pair rejected"), FPadmaWorldMapGenerator::Validate(Invalid, Error));
	Invalid = Authored; Invalid.Edges.Add(Edge(TEXT("cycle"), TEXT("checkpoint"), TEXT("camp")));
	TestFalse(TEXT("Directed cycle rejected"), FPadmaWorldMapGenerator::Validate(Invalid, Error));
	Invalid = Authored; Invalid.Edges.RemoveAt(0);
	TestFalse(TEXT("Unreachable anchor rejected"), FPadmaWorldMapGenerator::Validate(Invalid, Error));
	Invalid = Authored; Invalid.Checkpoints[0].NPCId = TEXT("unknown-npc");
	TestFalse(TEXT("Unknown checkpoint NPC rejected"), FPadmaWorldMapGenerator::Validate(Invalid, Error));
	Invalid = Authored; Invalid.Checkpoints[0].MissedFlag = Invalid.Checkpoints[0].MatchedFlag;
	TestFalse(TEXT("Indistinguishable checkpoint outcomes rejected"), FPadmaWorldMapGenerator::Validate(Invalid, Error));
	Invalid = Authored; Invalid.Checkpoints.Add(Authored.Checkpoints[0]);
	TestFalse(TEXT("Duplicate checkpoint rejected"), FPadmaWorldMapGenerator::Validate(Invalid, Error));
	Invalid = Authored; Invalid.Nodes[1].Position += FVector2D(1, 0);
	TestFalse(TEXT("Manual generated anchor movement rejected"), FPadmaWorldMapGenerator::ValidateGenerated(Authored, Invalid, Error));
	Invalid = Authored; Invalid.Nodes[2].NPCId = TEXT("replacement"); Invalid.Checkpoints[0].NPCId = TEXT("replacement");
	TestFalse(TEXT("Manual generated anchor identity replacement rejected"), FPadmaWorldMapGenerator::ValidateGenerated(Authored, Invalid, Error));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaMapUnlockClosureTest, "DreamOfPadma.WorldMap.Generation.UnlockProducerClosure",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPadmaMapUnlockClosureTest::RunTest(const FString& Parameters)
{
	const auto Authored = Template();
	FString Error;
	auto Map = Authored;
	Map.Edges.Last().UnlockFlag = TEXT("unknown-road");
	TestFalse(TEXT("Unknown road producer rejected"), FPadmaWorldMapGenerator::Validate(Map, Error));
	Map = Authored; Map.Edges[0].UnlockFlag = TEXT("story-road");
	TestFalse(TEXT("Legacy story producer behind its own lock rejected"), FPadmaWorldMapGenerator::Validate(Map, Error));
	Map = Authored; Map.Edges[1].UnlockFlag = TEXT("pilgrim-complete");
	TestFalse(TEXT("Explicit completion producer behind its own lock rejected"), FPadmaWorldMapGenerator::Validate(Map, Error));
	Map = Authored;
	Map.Nodes[1].CompletionFlag = TEXT("first-ready"); Map.Nodes[2].CompletionFlag = TEXT("second-ready");
	Map.Edges[0].UnlockFlag = TEXT("second-ready"); Map.Edges[1].UnlockFlag = TEXT("first-ready");
	TestFalse(TEXT("Cyclic unlock dependencies rejected even when directed graph itself is acyclic"), FPadmaWorldMapGenerator::Validate(Map, Error));
	Map = Authored;
	Map.Edges[1].UnlockFlag = TEXT("pilgrim-met");
	TestFalse(TEXT("Matched checkpoint cannot create its own inaccessible NPC prerequisite"), FPadmaWorldMapGenerator::Validate(Map, Error));
	Map = Authored;
	Map.Edges.Last().UnlockFlag = TEXT("pilgrim-met");
	TestTrue(TEXT("Reachable NPC enables an authored matched outcome"), FPadmaWorldMapGenerator::Validate(Map, Error));
	Map.Edges.Add(Edge(TEXT("alternate-outcome"), TEXT("npc"), TEXT("summit"), TEXT("pilgrim-missed")));
	TestTrue(TEXT("Mutually exclusive matched and missed routes remain valid authoring alternatives"), FPadmaWorldMapGenerator::Validate(Map, Error));
	Map = Authored;
	Map.Edges[1].UnlockFlag = TEXT("story-road");
	TestTrue(TEXT("Reachable legacy story fallback unlocks a later road"), FPadmaWorldMapGenerator::Validate(Map, Error));
	Map = Authored;
	Map.Nodes[1].CompletionFlag = TEXT("vendor-ready"); Map.Edges[1].UnlockFlag = TEXT("vendor-ready");
	TestTrue(TEXT("Reachable explicit completion unlocks a later road"), FPadmaWorldMapGenerator::Validate(Map, Error));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaMapAtomicTest, "DreamOfPadma.WorldMap.Generation.AtomicGenerationAndEditorRestore",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPadmaMapAtomicTest::RunTest(const FString& Parameters)
{
	const auto Authored = Template();
	FPadmaMapGenerationSettings Settings;
	FPadmaMapLayout Output;
	FString Error;
	if (!FPadmaWorldMapGenerator::Generate(Authored, Settings, 123, Output, Error)) { AddError(Error); return false; }
	const FString Before = FPadmaWorldMapGenerator::Signature(Output);
	Settings.BranchCountMax = 65;
	TestFalse(TEXT("Unbounded generation rejected"), FPadmaWorldMapGenerator::Generate(Authored, Settings, 456, Output, Error));
	TestEqual(TEXT("Failed generation preserves prior output"), FPadmaWorldMapGenerator::Signature(Output), Before);
	FPadmaContentSnapshot Content;
	TestTrue(TEXT("Valid layout publishes atomic map snapshot"), FPadmaWorldMapGenerator::ApplyToContent(Output, Content, Error));
	TestEqual(TEXT("Custom declared home preserved"), Content.MapLayout.HomeNode, FName(TEXT("camp")));
	TestEqual(TEXT("Every node published once"), Content.NodeOrder.Num(), Output.Nodes.Num());
	auto Invalid = Output; Invalid.HomeNode = TEXT("missing");
	TestFalse(TEXT("Invalid publication rejected"), FPadmaWorldMapGenerator::ApplyToContent(Invalid, Content, Error));
	TestEqual(TEXT("Failed publication preserves prior snapshot"), FPadmaWorldMapGenerator::Signature(Content.MapLayout), Before);
	UPadmaWorldMapAsset* Asset = NewObject<UPadmaWorldMapAsset>();
	Asset->Template = Authored;
	Asset->Generate();
	TestTrue(TEXT("Editor generation selects valid result"), Asset->bUseGenerated && Asset->bLastValidationSucceeded);
	const FString GeneratedBefore = FPadmaWorldMapGenerator::Signature(Asset->GeneratedLayout);
	Asset->GenerationSettings.BranchLengthMin = 0;
	Asset->Generate();
	TestFalse(TEXT("Editor reports failed generation"), Asset->bLastValidationSucceeded);
	TestTrue(TEXT("Failed editor generation retains active output"), Asset->bUseGenerated);
	TestEqual(TEXT("Failed editor generation retains old graph"), FPadmaWorldMapGenerator::Signature(Asset->GeneratedLayout), GeneratedBefore);
	Asset->RestoreTemplate();
	TestFalse(TEXT("Restore selects authored template"), Asset->bUseGenerated);
	TestEqual(TEXT("Restore resets generated values to authored template"), FPadmaWorldMapGenerator::Signature(Asset->GeneratedLayout), FPadmaWorldMapGenerator::Signature(Authored));
	return true;
}
#endif
