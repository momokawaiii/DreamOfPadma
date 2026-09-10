#include "Core/WorldMap/PadmaWorldMapDefinition.h"
#include "Core/WorldMap/PadmaTutorialMap.h"
#include "Math/RandomStream.h"
#include "Misc/SecureHash.h"

namespace
{
bool Fail(FString& Error, const FString& Message)
{
	Error = Message;
	return false;
}

bool NameLess(FName A, FName B) { return A.LexicalLess(B); }

void Canonicalize(FPadmaMapLayout& Layout)
{
	Layout.Nodes.Sort([](const auto& A, const auto& B) { return NameLess(A.Id, B.Id); });
	Layout.Edges.Sort([](const auto& A, const auto& B) { return NameLess(A.Id, B.Id); });
	Layout.Checkpoints.Sort([](const auto& A, const auto& B) { return NameLess(A.Id, B.Id); });
}

TSet<FName> Reachable(const FPadmaMapLayout& Layout, FName Start, bool bUnlockedOnly)
{
	TMap<FName, TArray<FName>> Next;
	for (const auto& Edge : Layout.Edges)
	{
		if (!bUnlockedOnly || Edge.UnlockFlag.IsNone()) { Next.FindOrAdd(Edge.From).Add(Edge.To); }
	}
	TSet<FName> Seen = {Start};
	TArray<FName> Queue = {Start};
	for (int32 Index = 0; Index < Queue.Num(); ++Index)
	{
		if (const auto* Destinations = Next.Find(Queue[Index]))
		{
			for (FName Destination : *Destinations)
			{
				if (!Seen.Contains(Destination)) { Seen.Add(Destination); Queue.Add(Destination); }
			}
		}
	}
	return Seen;
}

const FPadmaWorldNodeDefinitionRow* FindNode(const FPadmaMapLayout& Layout, FName Id)
{
	return Layout.Nodes.FindByPredicate([Id](const auto& Node) { return Node.Id == Id; });
}

bool SameFixedNode(const FPadmaWorldNodeDefinitionRow& A, const FPadmaWorldNodeDefinitionRow& B)
{
	return A.Id == B.Id && A.AnchorKind == B.AnchorKind && A.NPCId == B.NPCId
		&& A.Position == B.Position && A.Type == B.Type && A.DialogueId == B.DialogueId
		&& A.CompletionFlag == B.CompletionFlag;
}

void Token(FString& Out, const FString& Value)
{
	Out += FString::FromInt(Value.Len()) + TEXT(":") + Value;
}
void Name(FString& Out, FName Value) { Token(Out, Value.ToString().ToLower()); }
}

bool FPadmaWorldMapGenerator::Validate(const FPadmaMapLayout& Layout, FString& Error)
{
	Error.Reset();
	if (Layout.MapId.IsNone() || Layout.Version < 1 || Layout.GeneratorVersion < 1
		|| Layout.Nodes.Num() < 2 || Layout.Nodes.Num() > MaxNodes
		|| Layout.Edges.IsEmpty() || Layout.Edges.Num() > MaxEdges)
	{
		return Fail(Error, TEXT("Map requires an ID, positive versions, 2..1024 nodes and 1..8192 directed edges."));
	}
	TSet<FName> NodeIds, NPCIds;
	TSet<FIntPoint> HexCoordinates;
	if (Layout.bHexWilderness && Layout.MapId != TEXT("tutorial-painted-v1"))
		return Fail(Error, TEXT("Hex wilderness currently requires the tutorial-painted-v1 map contract."));
	if (Layout.bHexWilderness)
	{
		FPadmaHexGridConfig ApprovedGrid;
		if (!FPadmaTutorialMapGenerator::GridForVersion(Layout.GeneratorVersion, ApprovedGrid) || Layout.HexGrid != ApprovedGrid)
			return Fail(Error, TEXT("Tutorial grid values do not match a supported generator version."));
		int32 ExpectedCells = 0;
		for (int32 R = ApprovedGrid.MinR; R <= ApprovedGrid.MaxR; ++R)
			for (int32 Q = ApprovedGrid.MinQ; Q <= ApprovedGrid.MaxQ; ++Q)
				if (FPadmaTutorialMapGenerator::ContainsCell(ApprovedGrid, Q, R)) ++ExpectedCells;
		if (Layout.Nodes.Num() != ExpectedCells)
			return Fail(Error, TEXT("Tutorial layout must retain every cell of its saved grid."));
	}
	TMap<FName, int32> Indegree;
	for (const auto& Node : Layout.Nodes)
	{
		if (Node.Id.IsNone() || NodeIds.Contains(Node.Id) || Node.DisplayName.IsEmpty()
			|| Node.Type.IsNone() || Node.Terrain.IsNone() || Node.Stage < 0 || Node.EnemyCount < 0
			|| !FMath::IsFinite(Node.Position.X) || !FMath::IsFinite(Node.Position.Y)
			|| !(Node.InitialOwner == TEXT("player") || Node.InitialOwner == TEXT("neutral") || Node.InitialOwner == TEXT("ruler"))
			|| uint8(Node.AnchorKind) > uint8(EPadmaMapAnchorKind::MainStory))
		{
			return Fail(Error, TEXT("Invalid or duplicate map node: ") + Node.Id.ToString());
		}
		if (Node.AnchorKind != EPadmaMapAnchorKind::None)
		{
			if (Node.NPCId.IsNone() || Node.DialogueId.IsNone())
			{
				return Fail(Error, TEXT("Anchored NPC requires explicit NPC and dialogue identities: ") + Node.Id.ToString());
			}
			NPCIds.Add(Node.NPCId);
		}
		if (Layout.bHexWilderness)
		{
			const FIntPoint Cell(Node.HexQ, Node.HexR);
			if (HexCoordinates.Contains(Cell) || !FPadmaTutorialMapGenerator::ContainsCell(Layout.HexGrid, Node.HexQ, Node.HexR)
				|| (Node.bWilderness && !Node.Position.Equals(FPadmaTutorialMapGenerator::CellCenter(Layout.HexGrid, Node.HexQ, Node.HexR), 1.e-8))
				|| !FMath::IsFinite(Node.Elevation) || Node.Elevation < 0 || Node.Elevation > 1
				|| Node.Position.X < 0 || Node.Position.X > 100 || Node.Position.Y < 0 || Node.Position.Y > 100
				|| (Node.bHabitable && !Node.bTraversable)
				|| (!Node.bTraversable && (Node.InitialOwner != TEXT("neutral") || !Node.bWilderness || Node.EnemyCount != 0))
				|| (Node.bWilderness && (Node.AnchorKind != EPadmaMapAnchorKind::None || !Node.NPCId.IsNone() || !Node.DialogueId.IsNone()
					|| !Node.CompletionFlag.IsNone() || !Node.Id.ToString().StartsWith(TEXT("wild.")))))
				return Fail(Error, TEXT("Invalid tutorial hex metadata: ") + Node.Id.ToString());
			HexCoordinates.Add(Cell);
		}
		NodeIds.Add(Node.Id);
		Indegree.Add(Node.Id, 0);
	}
	if (Layout.HomeNode == Layout.BossNode || !NodeIds.Contains(Layout.HomeNode) || !NodeIds.Contains(Layout.BossNode))
	{
		return Fail(Error, TEXT("Map must declare distinct existing home and boss nodes."));
	}
	if (FindNode(Layout, Layout.HomeNode)->InitialOwner != TEXT("player"))
	{
		return Fail(Error, TEXT("The declared home node must initially belong to the player."));
	}
	TSet<FName> EdgeIds;
	TMap<FName, TSet<FName>> Destinations;
	for (const auto& Edge : Layout.Edges)
	{
		if (Edge.Id.IsNone() || EdgeIds.Contains(Edge.Id) || !NodeIds.Contains(Edge.From)
			|| !NodeIds.Contains(Edge.To) || Edge.From == Edge.To || Destinations.FindOrAdd(Edge.From).Contains(Edge.To))
		{
			return Fail(Error, TEXT("Invalid or duplicate directed edge: ") + Edge.Id.ToString());
		}
		EdgeIds.Add(Edge.Id);
		if (Layout.bHexWilderness && (!FindNode(Layout, Edge.From)->bTraversable || !FindNode(Layout, Edge.To)->bTraversable))
			return Fail(Error, TEXT("Tutorial road references blocked terrain: ") + Edge.Id.ToString());
		Destinations.FindOrAdd(Edge.From).Add(Edge.To);
		++Indegree.FindChecked(Edge.To);
	}
	TArray<FName> Queue;
	for (const auto& Node : Layout.Nodes) { if (Indegree.FindChecked(Node.Id) == 0) { Queue.Add(Node.Id); } }
	for (int32 Index = 0; Index < Queue.Num(); ++Index)
	{
		if (const auto* Next = Destinations.Find(Queue[Index]))
		{
			for (FName Destination : *Next) { if (--Indegree.FindChecked(Destination) == 0) { Queue.Add(Destination); } }
		}
	}
	if (!Layout.bHexWilderness && Queue.Num() != Layout.Nodes.Num()) { return Fail(Error, TEXT("Directed world map contains a cycle.")); }
	const TSet<FName> Reach = Reachable(Layout, Layout.HomeNode, false);
	if (Layout.Nodes.ContainsByPredicate([&](const auto& Node) { return (!Layout.bHexWilderness || Node.bTraversable) && !Reach.Contains(Node.Id); }))
	{
		return Fail(Error, TEXT("Every map node, including each fixed anchor and boss, must be reachable from home."));
	}
	TSet<FName> CheckpointIds, CheckpointNodes;
	for (const auto& Checkpoint : Layout.Checkpoints)
	{
		if (Checkpoint.Id.IsNone() || CheckpointIds.Contains(Checkpoint.Id) || CheckpointNodes.Contains(Checkpoint.NodeId)
			|| !NodeIds.Contains(Checkpoint.NodeId) || !NPCIds.Contains(Checkpoint.NPCId)
			|| Checkpoint.MatchedFlag.IsNone() || Checkpoint.MissedFlag.IsNone() || Checkpoint.MatchedFlag == Checkpoint.MissedFlag)
		{
			return Fail(Error, TEXT("Checkpoint requires unique ID/node, an anchored NPC and distinct nonempty result flags: ") + Checkpoint.Id.ToString());
		}
		CheckpointIds.Add(Checkpoint.Id);
		CheckpointNodes.Add(Checkpoint.NodeId);
	}
	TSet<FName> Producers;
	for (const auto& Node : Layout.Nodes)
	{
		if (!Node.CompletionFlag.IsNone()) { Producers.Add(Node.CompletionFlag); }
		else if (Node.Type == TEXT("story")) { Producers.Add(TEXT("story-road")); }
	}
	for (const auto& Checkpoint : Layout.Checkpoints)
	{
		Producers.Add(Checkpoint.MatchedFlag);
		Producers.Add(Checkpoint.MissedFlag);
	}
	for (const auto& Edge : Layout.Edges)
	{
		if (!Edge.UnlockFlag.IsNone() && !Producers.Contains(Edge.UnlockFlag))
		{
			return Fail(Error, TEXT("Road references an unlock flag with no authored producer: ") + Edge.Id.ToString() + TEXT(" / ") + Edge.UnlockFlag.ToString());
		}
	}
	// Authoring potential, not one playthrough: union the alternatives so an optional
	// story or mutually exclusive checkpoint branch is never made mandatory here.
	// The runtime still resolves each checkpoint once and emits exactly one result.
	TSet<FName> PossibleNodes = {Layout.HomeNode};
	TSet<FName> PossibleNPCs, PossibleFlags;
	bool bChanged = true;
	while (bChanged)
	{
		const int32 Before = PossibleNodes.Num() + PossibleNPCs.Num() + PossibleFlags.Num();
		for (const auto& Node : Layout.Nodes)
		{
			if (!PossibleNodes.Contains(Node.Id)) { continue; }
			if (!Node.NPCId.IsNone()) { PossibleNPCs.Add(Node.NPCId); }
			if (!Node.CompletionFlag.IsNone()) { PossibleFlags.Add(Node.CompletionFlag); }
			else if (Node.Type == TEXT("story")) { PossibleFlags.Add(TEXT("story-road")); }
		}
		for (const auto& Checkpoint : Layout.Checkpoints)
		{
			if (!PossibleNodes.Contains(Checkpoint.NodeId)) { continue; }
			PossibleFlags.Add(Checkpoint.MissedFlag);
			if (PossibleNPCs.Contains(Checkpoint.NPCId)) { PossibleFlags.Add(Checkpoint.MatchedFlag); }
		}
		for (const auto& Edge : Layout.Edges)
		{
			if (PossibleNodes.Contains(Edge.From) && (Edge.UnlockFlag.IsNone() || PossibleFlags.Contains(Edge.UnlockFlag)))
			{
				PossibleNodes.Add(Edge.To);
			}
		}
		bChanged = Before != PossibleNodes.Num() + PossibleNPCs.Num() + PossibleFlags.Num();
	}
	for (const auto& Edge : Layout.Edges)
	{
		if (!Edge.UnlockFlag.IsNone() && !PossibleFlags.Contains(Edge.UnlockFlag))
		{
			return Fail(Error, TEXT("Road unlock producer is blocked by its own or cyclic prerequisites: ") + Edge.Id.ToString() + TEXT(" / ") + Edge.UnlockFlag.ToString());
		}
	}
	if (!PossibleNodes.Contains(Layout.BossNode))
	{
		return Fail(Error, TEXT("No authored unlock progression can reach the declared boss node."));
	}
	if (Layout.bHexWilderness && Layout.Nodes.ContainsByPredicate([&](const auto& Node) { return Node.bHabitable && !PossibleNodes.Contains(Node.Id); }))
		return Fail(Error, TEXT("Tutorial contains habitable cells without a possible unlock progression."));
	return true;
}

bool FPadmaWorldMapGenerator::ValidateGenerated(const FPadmaMapLayout& Template, const FPadmaMapLayout& Generated, FString& Error)
{
	if (!Validate(Template, Error) || !Validate(Generated, Error)) { return false; }
	if (Template.MapId != Generated.MapId || Template.Version != Generated.Version
		|| Template.HomeNode != Generated.HomeNode || Template.BossNode != Generated.BossNode || Template.bHexWilderness != Generated.bHexWilderness)
	{
		return Fail(Error, TEXT("Generated layout changed the authored map or terminal identities."));
	}
	if (Template.bHexWilderness && (Template.GeneratorVersion != Generated.GeneratorVersion || Template.HexGrid != Generated.HexGrid))
		return Fail(Error, TEXT("Generated tutorial changed the trusted grid version or projection."));
	for (const auto& Node : Template.Nodes)
	{
		if (Node.AnchorKind == EPadmaMapAnchorKind::None && Node.Id != Template.HomeNode && Node.Id != Template.BossNode
			&& !(Template.bHexWilderness && !Node.bWilderness)) { continue; }
		const auto* GeneratedNode = FindNode(Generated, Node.Id);
		if (!GeneratedNode || !SameFixedNode(Node, *GeneratedNode)
			|| (Template.bHexWilderness && (Node.HexQ != GeneratedNode->HexQ || Node.HexR != GeneratedNode->HexR
				|| !GeneratedNode->bHabitable || !GeneratedNode->bTraversable || GeneratedNode->bWilderness)))
		{
			return Fail(Error, TEXT("Generated layout omitted or changed an authored anchor/terminal: ") + Node.Id.ToString());
		}
	}
	for (const auto& Node : Generated.Nodes)
	{
		if (Node.AnchorKind != EPadmaMapAnchorKind::None)
		{
			const auto* Authored = FindNode(Template, Node.Id);
			if (!Authored || !SameFixedNode(*Authored, Node)) { return Fail(Error, TEXT("New anchors must first be authored in Template: ") + Node.Id.ToString()); }
		}
	}
	return true;
}

bool FPadmaWorldMapGenerator::Generate(const FPadmaMapLayout& Template, const FPadmaMapGenerationSettings& Settings,
	int32 Seed, FPadmaMapLayout& Out, FString& Error)
{
	if (!Validate(Template, Error)) { return false; }
	if (Template.bHexWilderness) return Fail(Error, TEXT("Tutorial hex layouts use FPadmaTutorialMapGenerator rather than legacy branch generation."));
	if (Settings.BranchCountMin < 0 || Settings.BranchCountMax < Settings.BranchCountMin || Settings.BranchCountMax > 64
		|| Settings.BranchLengthMin < 1 || Settings.BranchLengthMax < Settings.BranchLengthMin || Settings.BranchLengthMax > 8
		|| !FMath::IsFinite(Settings.BranchSpread) || Settings.BranchSpread < 0 || Settings.BranchSpread > 1000000
		|| Settings.TerrainChoices.Contains(NAME_None))
	{
		return Fail(Error, TEXT("Invalid generation settings; use 0..64 branches, 1..8 nodes per branch and finite spread within 0..1000000."));
	}
	FPadmaMapLayout Candidate = Template;
	Canonicalize(Candidate);
	const FPadmaMapLayout Basis = Candidate;
	Candidate.Seed = Seed;
	Candidate.GeneratorVersion = CurrentGeneratorVersion;
	FRandomStream Random(Seed);
	const int32 BranchCount = Random.RandRange(Settings.BranchCountMin, Settings.BranchCountMax);
	TSet<FName> UsedNodes, UsedEdges;
	for (const auto& Node : Candidate.Nodes) { UsedNodes.Add(Node.Id); }
	for (const auto& Edge : Candidate.Edges) { UsedEdges.Add(Edge.Id); }
	for (int32 Branch = 0; Branch < BranchCount; ++Branch)
	{
		FName Source = Basis.Nodes[Random.RandRange(0, Basis.Nodes.Num() - 1)].Id;
		TArray<FName> Targets = Reachable(Basis, Source, true).Array();
		Targets.Remove(Source);
		Targets.Sort(NameLess);
		FName Target, RequiredFlag;
		if (!Targets.IsEmpty()) { Target = Targets[Random.RandRange(0, Targets.Num() - 1)]; }
		else
		{
			const auto& Edge = Basis.Edges[Random.RandRange(0, Basis.Edges.Num() - 1)];
			Source = Edge.From; Target = Edge.To; RequiredFlag = Edge.UnlockFlag;
		}
		const auto& From = *FindNode(Basis, Source);
		const auto& To = *FindNode(Basis, Target);
		const int32 Length = Random.RandRange(Settings.BranchLengthMin, Settings.BranchLengthMax);
		if (Candidate.Nodes.Num() + Length > MaxNodes || Candidate.Edges.Num() + Length + 1 > MaxEdges)
		{
			return Fail(Error, TEXT("Generated layout exceeds bounded map size."));
		}
		FVector2D Normal(-(To.Position - From.Position).Y, (To.Position - From.Position).X);
		Normal = Normal.GetSafeNormal();
		if (Normal.IsNearlyZero()) { Normal = FVector2D(0, 1); }
		const double Spread = Settings.BranchSpread * Random.FRandRange(0.5f, 1.5f) * (Random.RandRange(0, 1) ? 1 : -1);
		FName Previous = Source;
		for (int32 Step = 0; Step <= Length; ++Step)
		{
			FName Next = Target;
			if (Step < Length)
			{
				FPadmaWorldNodeDefinitionRow Node;
				Node.Id = FName(*FString::Printf(TEXT("generated-b%02d-n%02d"), Branch, Step));
				if (UsedNodes.Contains(Node.Id)) { return Fail(Error, TEXT("Authored node collides with reserved generated ID: ") + Node.Id.ToString()); }
				UsedNodes.Add(Node.Id);
				Node.DisplayName = FText::FromString(FString::Printf(TEXT("Path %d-%d"), Branch + 1, Step + 1));
				Node.Type = TEXT("plain"); Node.InitialOwner = TEXT("neutral"); Node.EnemyCount = 1;
				Node.Stage = From.Stage;
				Node.Terrain = Settings.TerrainChoices.IsEmpty() ? From.Terrain : Settings.TerrainChoices[Random.RandRange(0, Settings.TerrainChoices.Num() - 1)];
				const double Alpha = double(Step + 1) / double(Length + 1);
				Node.Position = FMath::Lerp(From.Position, To.Position, Alpha) + Normal * (Spread * FMath::Sin(PI * Alpha));
				Node.Provenance = TEXT("TASK-048 deterministic map branch");
				Next = Node.Id;
				Candidate.Nodes.Add(MoveTemp(Node));
			}
			FPadmaWorldEdgeDefinitionRow Edge;
			Edge.Id = FName(*FString::Printf(TEXT("generated-b%02d-e%02d"), Branch, Step));
			if (UsedEdges.Contains(Edge.Id)) { return Fail(Error, TEXT("Authored edge collides with reserved generated ID: ") + Edge.Id.ToString()); }
			UsedEdges.Add(Edge.Id);
			Edge.From = Previous; Edge.To = Next;
			Edge.UnlockFlag = RequiredFlag;
			Edge.Provenance = TEXT("TASK-048 deterministic map branch");
			Candidate.Edges.Add(MoveTemp(Edge));
			Previous = Next;
		}
	}
	Canonicalize(Candidate);
	if (!ValidateGenerated(Template, Candidate, Error)) { return false; }
	Out = MoveTemp(Candidate);
	return true;
}

bool FPadmaWorldMapGenerator::ApplyToContent(const FPadmaMapLayout& Layout, FPadmaContentSnapshot& Content, FString& Error)
{
	if (!Validate(Layout, Error)) { return false; }
	FPadmaMapLayout Ordered = Layout;
	Canonicalize(Ordered);
	Content.MapLayout = Ordered;
	Content.Nodes.Reset(); Content.NodeOrder.Reset();
	for (const auto& Node : Ordered.Nodes) { Content.Nodes.Add(Node.Id, Node); Content.NodeOrder.Add(Node.Id); }
	Content.Edges = Ordered.Edges;
	return true;
}

FString FPadmaWorldMapGenerator::Signature(const FPadmaMapLayout& Layout)
{
	FPadmaMapLayout Ordered = Layout;
	Canonicalize(Ordered);
	FString Values;
	Name(Values, Ordered.MapId); Token(Values, FString::FromInt(Ordered.Version));
	Token(Values, FString::FromInt(Ordered.GeneratorVersion)); Token(Values, FString::FromInt(Ordered.Seed));
	Name(Values, Ordered.HomeNode); Name(Values, Ordered.BossNode);
	if (Ordered.bHexWilderness) Token(Values, TEXT("tutorial-hex-v1"));
	// V1 keeps its exact pre-config signature: old saves deserialize the legacy grid defaults.
	if (Ordered.bHexWilderness && Ordered.GeneratorVersion >= 2)
	{
		const auto& Grid = Ordered.HexGrid;
		Token(Values, TEXT("projected-hex-grid-v2"));
		Token(Values, FString::Printf(TEXT("%.17g,%.17g,%.17g,%.17g,%d,%d,%d,%d,%.17g,%.17g"),
			Grid.Radius, Grid.ProjectionY, Grid.Origin.X, Grid.Origin.Y, Grid.MinQ, Grid.MaxQ, Grid.MinR, Grid.MaxR, Grid.MinU, Grid.MaxU));
	}
	Token(Values, FString::FromInt(Ordered.Nodes.Num()));
	for (const auto& Node : Ordered.Nodes)
	{
		Name(Values, Node.Id); Name(Values, Node.Type); Name(Values, Node.Terrain);
		Token(Values, FString::Printf(TEXT("%.17g,%.17g"), Node.Position.X, Node.Position.Y));
		Name(Values, Node.InitialOwner); Token(Values, FString::FromInt(Node.EnemyCount));
		Token(Values, FString::FromInt(Node.Stage)); Token(Values, FString::FromInt(int32(Node.AnchorKind)));
		Name(Values, Node.NPCId); Name(Values, Node.DialogueId); Name(Values, Node.CompletionFlag);
		if (Ordered.bHexWilderness)
		{
			Token(Values, FString::Printf(TEXT("%d,%d,%d,%d,%d,%.17g"), Node.bHabitable, Node.bTraversable,
				Node.bWilderness, Node.HexQ, Node.HexR, Node.Elevation));
		}
	}
	Token(Values, FString::FromInt(Ordered.Edges.Num()));
	for (const auto& Edge : Ordered.Edges)
	{
		Name(Values, Edge.Id); Name(Values, Edge.From); Name(Values, Edge.To); Name(Values, Edge.UnlockFlag);
	}
	Token(Values, FString::FromInt(Ordered.Checkpoints.Num()));
	for (const auto& Checkpoint : Ordered.Checkpoints)
	{
		Name(Values, Checkpoint.Id); Name(Values, Checkpoint.NodeId); Name(Values, Checkpoint.NPCId);
		Token(Values, Checkpoint.bRequireCompletion ? TEXT("1") : TEXT("0"));
		Name(Values, Checkpoint.MatchedFlag); Name(Values, Checkpoint.MissedFlag);
		Name(Values, Checkpoint.MatchedDialogueId); Name(Values, Checkpoint.MissedDialogueId);
	}
	const FTCHARToUTF8 Utf8(*Values);
	FMD5 Hash;
	Hash.Update(reinterpret_cast<const uint8*>(Utf8.Get()), Utf8.Length());
	uint8 Digest[16];
	Hash.Final(Digest);
	return BytesToHex(Digest, UE_ARRAY_COUNT(Digest));
}
