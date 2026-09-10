#include "Misc/AutomationTest.h"
#include "Core/WorldMap/PadmaTutorialMap.h"
#include "Core/WorldMap/PadmaWorldMapDefinition.h"
#include "Core/Run/PadmaRunRules.h"
#include "Algo/Reverse.h"
#include "Game/Save/PadmaRunSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "JsonObjectConverter.h"
#include "UI/Screens/PadmaTutorialMapWidget.h"

#if WITH_DEV_AUTOMATION_TESTS
extern bool LoadPadmaDemoTestContent(FPadmaContentSnapshot& Out, FString& Error);
namespace
{
FPadmaMapLayout SourceMap()
{
    FPadmaMapLayout Map; Map.MapId=TEXT("tutorial-source-test"); Map.HomeNode=TEXT("home"); Map.BossNode=TEXT("boss");
    const FName Ids[]={TEXT("home"),TEXT("fire"),TEXT("story"),TEXT("plain"),TEXT("forge"),TEXT("gate"),TEXT("boss")};
    const FName Types[]={TEXT("core"),TEXT("fire"),TEXT("story"),TEXT("normal"),TEXT("forge"),TEXT("gate"),TEXT("rulerCore")};
    const FName Terrains[]={TEXT("城镇"),TEXT("平原"),TEXT("林地"),TEXT("平原"),TEXT("丘陵"),TEXT("山地"),TEXT("城镇")};
    for (int32 I=0; I<7; ++I)
    {
        FPadmaWorldNodeDefinitionRow Node; Node.Id=Ids[I]; Node.Type=Types[I]; Node.Terrain=Terrains[I];
        Node.DisplayName=FText::FromName(Node.Id); Node.Position=FVector2D(I*100,300);
        Node.InitialOwner=I==0?FName(TEXT("player")):I>=5?FName(TEXT("ruler")):FName(TEXT("neutral")); Node.EnemyCount=I>=5?2:0;
        if (I==2) { Node.AnchorKind=EPadmaMapAnchorKind::MainStory; Node.NPCId=TEXT("watcher"); Node.DialogueId=TEXT("story"); Node.CompletionFlag=TEXT("story-road"); }
        if (I==4) { Node.AnchorKind=EPadmaMapAnchorKind::FixedNPC; Node.NPCId=TEXT("smith"); Node.DialogueId=TEXT("forge"); }
        Map.Nodes.Add(Node);
    }
    auto Add=[&](FName From,FName To,FName Flag=NAME_None)
    {
        FPadmaWorldEdgeDefinitionRow Edge; Edge.Id=FName(*(TEXT("authored.")+From.ToString()+TEXT(".")+To.ToString()));
        Edge.From=From; Edge.To=To; Edge.UnlockFlag=Flag; Map.Edges.Add(Edge);
    };
    Add(TEXT("home"),TEXT("fire")); Add(TEXT("home"),TEXT("story")); Add(TEXT("fire"),TEXT("plain")); Add(TEXT("story"),TEXT("plain"));
    Add(TEXT("plain"),TEXT("forge")); Add(TEXT("plain"),TEXT("gate")); Add(TEXT("forge"),TEXT("boss")); Add(TEXT("gate"),TEXT("boss"));
    Add(TEXT("story"),TEXT("forge"),TEXT("story-road"));
    FPadmaStoryCheckpointRow Check; Check.Id=TEXT("watcher-gate"); Check.NodeId=TEXT("gate"); Check.NPCId=TEXT("watcher");
    Check.MatchedFlag=TEXT("watcher-aid"); Check.MissedFlag=TEXT("watcher-absent"); Map.Checkpoints.Add(Check);
    return Map;
}
TSet<FName> Reach(const FPadmaMapLayout& Map,bool bOpenOnly)
{
    TSet<FName> Seen={Map.HomeNode}; TArray<FName> Queue={Map.HomeNode};
    for (int32 I=0; I<Queue.Num(); ++I)
        for (const auto& Edge:Map.Edges)
            if (Edge.From==Queue[I] && (!bOpenOnly||Edge.UnlockFlag.IsNone()) && !Seen.Contains(Edge.To)) { Seen.Add(Edge.To); Queue.Add(Edge.To); }
    return Seen;
}
bool Step(FAutomationTestBase& Test,const FPadmaRunCommandResult& Result)
{ if (!Result.bOk) Test.AddError(Result.Text); return Result.bOk; }
bool Ready(FAutomationTestBase& Test,FPadmaContentSnapshot& Content,FPadmaRunRules& Rules)
{
    FString Error; FPadmaMapLayout Map;
    if (!LoadPadmaDemoTestContent(Content,Error)||!FPadmaTutorialMapGenerator::Generate(SourceMap(),12345,Map,Error)
        ||!FPadmaWorldMapGenerator::ApplyToContent(Map,Content,Error)) { Test.AddError(Error); return false; }
    Rules.Initialize(Content); return Step(Test,Rules.Start());
}
int32 RulerWildCount(const FPadmaRunRules& Rules)
{
    int32 Count=0;
    for (const auto& Node:Rules.Content().Nodes)
        if (Node.Value.bWilderness && Rules.Run().Nodes.FindChecked(Node.Key).Owner==TEXT("ruler")) ++Count;
    return Count;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaTutorialGenerationTest,"DreamOfPadma.WorldMap.Tutorial.DeterministicHexesAndFixedLandmarks",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaTutorialGenerationTest::RunTest(const FString& Parameters)
{
    const auto Source=SourceMap(); FString Error; FPadmaMapLayout First,Repeat,Changed;
    if (!TestTrue(TEXT("Tutorial generates"),FPadmaTutorialMapGenerator::Generate(Source,12345,First,Error))) { AddError(Error); return false; }
    auto Reordered=Source; Algo::Reverse(Reordered.Nodes); Algo::Reverse(Reordered.Edges);
    TestTrue(TEXT("Reordered source generates"),FPadmaTutorialMapGenerator::Generate(Reordered,12345,Repeat,Error));
    TestEqual(TEXT("Seed and semantic input determine topology"),FPadmaWorldMapGenerator::Signature(First),FPadmaWorldMapGenerator::Signature(Repeat));
    TestTrue(TEXT("Another seed generates"),FPadmaTutorialMapGenerator::Generate(Source,9021,Changed,Error));
    TestEqual(TEXT("Fresh tutorial uses the reduced v2 grid"),First.Nodes.Num(),55);
    TestEqual(TEXT("Fresh tutorial freezes the nearby-guard generator version"),First.GeneratorVersion,3);
    TestTrue(TEXT("Tutorial accepts explicit reciprocal hex edges"),FPadmaWorldMapGenerator::Validate(First,Error));
    const auto All=Reach(First,false); int32 Blocked=0,Variations=0;
    for (const auto& Node:First.Nodes)
    {
        TestTrue(TEXT("All cell display positions remain within the painting"),Node.Position.X>=0&&Node.Position.X<=100&&Node.Position.Y>=0&&Node.Position.Y<=100);
        if (!Node.bTraversable)
        {
            ++Blocked; TestFalse(TEXT("Blocked terrain has no travel edges"),First.Edges.ContainsByPredicate([&](const auto& E) { return E.From==Node.Id||E.To==Node.Id; }));
        }
        if (Node.bHabitable) TestTrue(TEXT("Habitable cells remain connected through permitted progression"),All.Contains(Node.Id));
        if (Node.bWilderness)
        {
            TestTrue(TEXT("Stable wilderness ID prefix"),Node.Id.ToString().StartsWith(TEXT("wild.")));
            const auto* Other=Changed.Nodes.FindByPredicate([&](const auto& N) { return N.Id==Node.Id; });
            Variations+=(Other&&(Other->Terrain!=Node.Terrain||Other->Elevation!=Node.Elevation))?1:0;
        }
    }
    TestTrue(TEXT("Water/mountain barriers remain in the repaired layout"),Blocked>0);
    TestTrue(TEXT("Seed changes actual wilderness terrain values"),Variations>0);
    for (const auto& Original:Source.Nodes)
    {
        const auto* Node=First.Nodes.FindByPredicate([&](const auto& N) { return N.Id==Original.Id; });
        if (!TestNotNull(TEXT("Every existing critical identity survives"),Node)) return false;
        FVector2D Position; FPadmaTutorialMapGenerator::FixedPosition(Original.Id,Position);
        TestEqual(TEXT("Landmark matches approved painting coordinate exactly"),Node->Position,Position);
        TestEqual(TEXT("Existing node role is unchanged"),Node->Type,Original.Type);
        TestEqual(TEXT("Only the approved nearby outpost changes initial owner"),Node->InitialOwner,Original.Id==TEXT("fire")?FName(TEXT("ruler")):Original.InitialOwner);
        TestEqual(TEXT("NPC identity remains fixed"),Node->NPCId,Original.NPCId);
        TestEqual(TEXT("Dialogue identity remains fixed"),Node->DialogueId,Original.DialogueId);
    }
    for (const auto& Edge:First.Edges)
    {
        if (!Edge.Id.ToString().StartsWith(TEXT("hex."))) continue;
        const auto* A=First.Nodes.FindByPredicate([&](const auto& N) { return N.Id==Edge.From; });
        const auto* B=First.Nodes.FindByPredicate([&](const auto& N) { return N.Id==Edge.To; });
        const int32 Q=A->HexQ-B->HexQ,R=A->HexR-B->HexR;
        TestEqual(TEXT("Generated links connect exactly one axial neighbor"),(FMath::Abs(Q)+FMath::Abs(R)+FMath::Abs(Q+R))/2,1);
    }
    FPadmaMapGenerationSettings Branches; Branches.BranchCountMin=Branches.BranchCountMax=2; FPadmaMapLayout OldGenerated,Stripped;
    TestTrue(TEXT("Legacy branch fixture generates"),FPadmaWorldMapGenerator::Generate(Source,Branches,3,OldGenerated,Error));
    TestTrue(TEXT("Tutorial consumes legacy generated source"),FPadmaTutorialMapGenerator::Generate(OldGenerated,12345,Stripped,Error));
    TestEqual(TEXT("Old path branches are not duplicated into the tutorial"),FPadmaWorldMapGenerator::Signature(Stripped),FPadmaWorldMapGenerator::Signature(First));
    auto LegacyChanged=Source; LegacyChanged.Nodes[0].Elevation=.73; LegacyChanged.Nodes[0].HexQ=17;
    TestEqual(TEXT("Legacy signatures ignore opt-in-only fields"),FPadmaWorldMapGenerator::Signature(Source),FPadmaWorldMapGenerator::Signature(LegacyChanged));
    auto Corrupt=First; Corrupt.Nodes[0].Elevation+=.01;
    TestTrue(TEXT("Tutorial signatures include elevation"),FPadmaWorldMapGenerator::Signature(Corrupt)!=FPadmaWorldMapGenerator::Signature(First));
    return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaTutorialGateTest,"DreamOfPadma.WorldMap.Tutorial.LockedPartitionsCannotBeBypassed",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaTutorialGateTest::RunTest(const FString& Parameters)
{
    auto Source=SourceMap();
    for (auto& Edge:Source.Edges)
        if (Edge.From==TEXT("plain")&&(Edge.To==TEXT("forge")||Edge.To==TEXT("gate"))) Edge.UnlockFlag=TEXT("story-road");
    FString Error;
    for (int32 Version:{1,2})
    for (int32 Seed:{1,7,731,12345})
    {
        FPadmaMapLayout Map;
        if (!TestTrue(TEXT("Both grid versions preserve real gated partitions"),FPadmaTutorialMapGenerator::Generate(Source,Seed,Map,Error,Version))) { AddError(Error); return false; }
        const auto Open=Reach(Map,true),All=Reach(Map,false);
        TestFalse(TEXT("Wilderness cannot reach gated forge before story"),Open.Contains(TEXT("forge")));
        TestFalse(TEXT("Wilderness cannot reach gated boss before story"),Open.Contains(TEXT("boss")));
        TestTrue(TEXT("Unlock restores access to the boss"),All.Contains(TEXT("boss")));
        TestTrue(TEXT("Story flag producer stays reachable"),Open.Contains(TEXT("story")));
    }
    auto Alternatives=Source;
    Alternatives.Nodes.FindByPredicate([](const auto& N) { return N.Id==TEXT("fire"); })->CompletionFlag=TEXT("alternate-pass");
    Alternatives.Nodes.FindByPredicate([](const auto& N) { return N.Id==TEXT("fire"); })->DialogueId=TEXT("forge");
    Alternatives.Edges.FindByPredicate([](const auto& E) { return E.From==TEXT("story")&&E.To==TEXT("forge"); })->UnlockFlag=TEXT("alternate-pass");
    FPadmaMapLayout AlternativeMap;
    if (!TestTrue(TEXT("Parallel alternative locked routes generate"),FPadmaTutorialMapGenerator::Generate(Alternatives,12345,AlternativeMap,Error))) { AddError(Error); return false; }
    TestFalse(TEXT("Two alternative locks do not become an unlocked wilderness bypass"),Reach(AlternativeMap,true).Contains(TEXT("forge")));
    auto BadCoordinates=AlternativeMap; BadCoordinates.Nodes[0].HexQ=MAX_int32;
    TestFalse(TEXT("Invalid axial coordinates are rejected by layout validation"),FPadmaWorldMapGenerator::Validate(BadCoordinates,Error));
    auto Missing=Source; Missing.Nodes.RemoveAll([](const auto& N) { return N.Id==TEXT("story"); });
    FPadmaMapLayout Output=Source; const FString Before=FPadmaWorldMapGenerator::Signature(Output);
    TestFalse(TEXT("Invalid source rejected"),FPadmaTutorialMapGenerator::Generate(Missing,12345,Output,Error));
    TestEqual(TEXT("Failed generation is atomic"),Before,FPadmaWorldMapGenerator::Signature(Output));
    return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaTutorialOccupationTest,"DreamOfPadma.WorldMap.Tutorial.PlayerCostsDuskFrontierAndNoWarFarming",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaTutorialOccupationTest::RunTest(const FString& Parameters)
{
    FPadmaContentSnapshot Content; FPadmaRunRules Rules; if (!Ready(*this,Content,Rules)) return false;
    if (!Step(*this,Rules.Advance())||!Step(*this,Rules.Deploy(TEXT("card-1"),TEXT("home")))) return false;
    FName Wild,Blocked;
    for (const auto& Node:Content.Nodes) if (!Node.Value.bTraversable) { Blocked=Node.Key; break; }
    for (const auto& Edge:Content.Edges)
        if (Edge.From==TEXT("home")&&Rules.PreviewWildernessOccupation(Edge.From,Edge.To,TEXT("player")).bOk) { Wild=Edge.To; break; }
    if (!TestFalse(TEXT("A home frontier wilderness exists"),Wild.IsNone())) return false;
    const auto Before=Rules.Run();
    TestFalse(TEXT("Blocked terrain rejects movement"),Rules.Move({TEXT("card-1")},Blocked).bOk);
    TestTrue(TEXT("Blocked movement has no mutation"),FPadmaRunState::StaticStruct()->CompareScriptStruct(&Before,&Rules.Run(),0));
    FPadmaCardMobilityPolicy Immovable; Immovable.Mode=TEXT("immovable"); Step(*this,Rules.ConfigureMobility(TEXT("dawn"),Immovable));
    TestFalse(TEXT("Hex movement preserves per-card immobility"),Rules.Move({TEXT("card-1")},Wild).bOk);
    Step(*this,Rules.Restore(Before));
    auto NoBudget=Before; NoBudget.Resources.Calc=0; Step(*this,Rules.Restore(NoBudget));
    TestFalse(TEXT("Hex movement still requires calculation budget"),Rules.Move({TEXT("card-1")},Wild).bOk);
    TestTrue(TEXT("Failed payment is atomic"),FPadmaRunState::StaticStruct()->CompareScriptStruct(&NoBudget,&Rules.Run(),0));
    Step(*this,Rules.Restore(Before));
    if (!Step(*this,Rules.Move({TEXT("card-1")},Wild))) return false;
    TestEqual(TEXT("Player owns entered habitable wilderness"),Rules.Run().Nodes.FindChecked(Wild).Owner,FName(TEXT("player")));
    TestEqual(TEXT("Existing authored movement cost is charged"),Rules.Run().Resources.Calc,Before.Resources.Calc-Rules.P(TEXT("moveA")));
    TestEqual(TEXT("Wilderness capture grants no strategic war reward"),Rules.Run().War,Before.War);
    Step(*this,Rules.Move({TEXT("card-1")},TEXT("home"))); Step(*this,Rules.Move({TEXT("card-1")},Wild));
    TestEqual(TEXT("Revisits cannot farm war"),Rules.Run().War,Before.War);
    const uint32 BeforeAI=Rules.Run().RNG;
    Step(*this,Rules.Advance());
    TestEqual(TEXT("Dusk expands exactly one ruler wilderness cell"),RulerWildCount(Rules),1);
    TestEqual(TEXT("AI does not consume shared gameplay randomness"),Rules.Run().RNG,BeforeAI);
    TestEqual(TEXT("AI does not manufacture strategic war rewards"),Rules.Run().War,Before.War);
    TestEqual(TEXT("AI never replaces occupied player territory"),Rules.Run().Nodes.FindChecked(Wild).Owner,FName(TEXT("player")));
    FName EmptyHostile,Approach;
    for (const auto& Edge:Content.Edges)
    {
        if (Content.Nodes.FindChecked(Edge.To).bWilderness && Rules.Run().Nodes.FindChecked(Edge.To).Owner==TEXT("ruler")
            && Content.Nodes.FindChecked(Edge.From).bWilderness && Rules.Run().Nodes.FindChecked(Edge.From).Owner==TEXT("neutral")
            && Edge.UnlockFlag.IsNone()) { EmptyHostile=Edge.To; Approach=Edge.From; break; }
    }
    if (!TestFalse(TEXT("AI frontier has a testable empty hostile approach"),EmptyHostile.IsNone())) return false;
    auto Positioned=Rules.Run(); Positioned.Calendar.Phase=1;
    Positioned.Nodes.FindChecked(Approach).Owner=TEXT("player"); Positioned.Nodes.FindChecked(Approach).bVisited=true;
    Positioned.Cards.FindByPredicate([](const auto& C) { return C.Id==TEXT("card-1"); })->Node=Approach;
    FPadmaRunRules EmptyRules; EmptyRules.Initialize(Content);
    if (!Step(*this,EmptyRules.Restore(Positioned))) return false;
    const double EmptyMoveBudget=EmptyRules.Run().Resources.Calc;
    if (!Step(*this,EmptyRules.Move({TEXT("card-1")},EmptyHostile))) return false;
    TestFalse(TEXT("Empty hostile wilderness does not invent a guard battle"),EmptyRules.IsBattle());
    TestEqual(TEXT("Empty hostile wilderness becomes player-owned"),EmptyRules.Run().Nodes.FindChecked(EmptyHostile).Owner,FName(TEXT("player")));
    TestEqual(TEXT("Empty hostile capture pays the normal move cost"),EmptyRules.Run().Resources.Calc,EmptyMoveBudget-EmptyRules.P(TEXT("moveA")));
    TestEqual(TEXT("Recovering hostile wilderness cannot farm strategic war"),EmptyRules.Run().War,Before.War);
    Step(*this,Rules.Advance()); Step(*this,Rules.Advance());
    TestEqual(TEXT("Dawn and noon do not expand AI territory"),RulerWildCount(Rules),1);
    Step(*this,Rules.Advance()); TestEqual(TEXT("Next dusk performs one bounded expansion"),RulerWildCount(Rules),2);
    TestTrue(TEXT("Tutorial territory remains a valid save state"),Rules.ValidateState(Rules.Run()).bOk);
    auto* Save=NewObject<UPadmaRunSaveGame>(); Save->Run=Rules.Run(); TArray<uint8> Bytes;
    TestTrue(TEXT("Tutorial values serialize"),UGameplayStatics::SaveGameToMemory(Save,Bytes));
    auto* Loaded=UPadmaRunSaveGame::LoadFromMemory(Bytes); if (!TestNotNull(TEXT("Tutorial save loads"),Loaded)) return false;
    FPadmaRunRules Restored; Restored.Initialize(Content);
    if (!Step(*this,Restored.Restore(Loaded->Run))) return false;
    TestTrue(TEXT("Grid metadata and faction state round trip exactly"),FPadmaRunState::StaticStruct()->CompareScriptStruct(&Rules.Run(),&Restored.Run(),0));
    auto Invalid=Rules.Run(); Invalid.Nodes.FindChecked(Blocked).Owner=TEXT("player");
    TestFalse(TEXT("Save cannot occupy blocked terrain"),Restored.Restore(Invalid).bOk);
    return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaTutorialBattleFreezeTest,"DreamOfPadma.WorldMap.Tutorial.BattleFreezesFrontier",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaTutorialBattleFreezeTest::RunTest(const FString& Parameters)
{
    FPadmaContentSnapshot Content; FPadmaRunRules Rules; if (!Ready(*this,Content,Rules)) return false;
    if (!Step(*this,Rules.Advance())||!Step(*this,Rules.Deploy(TEXT("card-3"),TEXT("home")))
        ||!Step(*this,Rules.Move({TEXT("card-3")},TEXT("fire"),TEXT("encounter")))) return false;
    TestTrue(TEXT("Nearby guarded outpost immediately starts Encounter"),Rules.IsBattle());
    TestEqual(TEXT("The pending battle targets the adjacent outpost"),Rules.PendingBattle().Node,FName(TEXT("fire")));
    const auto Before=Rules.Run(); TestFalse(TEXT("Local battle blocks dusk advance"),Rules.Advance().bOk);
    TestTrue(TEXT("Battle freeze preserves every world value"),FPadmaRunState::StaticStruct()->CompareScriptStruct(&Before,&Rules.Run(),0));
    TestEqual(TEXT("No AI expansion during battle"),RulerWildCount(Rules),0);
    Step(*this,Rules.FinishBattle(false)); TestEqual(TEXT("Rollback preserves faction frontier"),RulerWildCount(Rules),0);
    return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaTutorialCrossMapRestoreTest,"DreamOfPadma.WorldMap.Tutorial.AuthoredAndTutorialRestoreOrder",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaTutorialCrossMapRestoreTest::RunTest(const FString& Parameters)
{
    FString Error; FPadmaContentSnapshot Base;
    if (!LoadPadmaDemoTestContent(Base,Error)) { AddError(Error); return false; }
    const FPadmaMapLayout TrustedSource=SourceMap();
    FPadmaMapGenerationSettings Settings; Settings.BranchCountMin=Settings.BranchCountMax=2;
    FPadmaMapLayout Generated,Tutorial;
    if (!FPadmaWorldMapGenerator::Generate(TrustedSource,Settings,619,Generated,Error)
        || !FPadmaTutorialMapGenerator::Generate(TrustedSource,12345,Tutorial,Error)) { AddError(Error); return false; }
    FPadmaContentSnapshot AuthoredContent=Base,TutorialContent=Base;
    if (!FPadmaWorldMapGenerator::ApplyToContent(Generated,AuthoredContent,Error)
        || !FPadmaWorldMapGenerator::ApplyToContent(Tutorial,TutorialContent,Error)) { AddError(Error); return false; }

    FPadmaRunRules NativeMaker; NativeMaker.Initialize(AuthoredContent,&TrustedSource);
    if (!Step(*this,NativeMaker.Start())||!Step(*this,NativeMaker.Advance())||!Step(*this,NativeMaker.Deploy(TEXT("card-3"),TEXT("home")))
        ||!Step(*this,NativeMaker.Move({TEXT("card-3")},TEXT("fire")))) return false;
    const FPadmaRunState NativeSave=NativeMaker.Run();
    FPadmaRunRules TutorialMaker; TutorialMaker.Initialize(TutorialContent,&TrustedSource);
    if (!Step(*this,TutorialMaker.Start())||!Step(*this,TutorialMaker.Advance())||!Step(*this,TutorialMaker.Advance())) return false;
    const FPadmaRunState TutorialSave=TutorialMaker.Run();
    TestEqual(TEXT("Tutorial save includes its separate ruler frontier"),RulerWildCount(TutorialMaker),1);

    // Fresh tutorial startup retains the approved pre-generation authored reference.
    FPadmaRunRules TutorialFirst; TutorialFirst.Initialize(TutorialContent,&TrustedSource);
    if (!Step(*this,TutorialFirst.Start())||!Step(*this,TutorialFirst.Restore(NativeSave))) return false;
    TestTrue(TEXT("Fresh tutorial can load the authored/generated save exactly"),FPadmaRunState::StaticStruct()->CompareScriptStruct(&NativeSave,&TutorialFirst.Run(),0));
    if (!Step(*this,TutorialFirst.Restore(TutorialSave))) return false;
    TestTrue(TEXT("Tutorial remains loadable after an authored save replaced active content"),FPadmaRunState::StaticStruct()->CompareScriptStruct(&TutorialSave,&TutorialFirst.Run(),0));

    // StartConfiguredMap may initialize directly with an approved generated native layout.
    // Its one-argument default must still derive tutorial compatibility from that configuration.
    FPadmaRunRules NativeFirst; NativeFirst.Initialize(AuthoredContent);
    if (!Step(*this,NativeFirst.Start())||!Step(*this,NativeFirst.Restore(TutorialSave))) return false;
    TestTrue(TEXT("Configured native map loads tutorial values exactly"),FPadmaRunState::StaticStruct()->CompareScriptStruct(&TutorialSave,&NativeFirst.Run(),0));
    if (!Step(*this,NativeFirst.Restore(NativeSave))) return false;
    TestTrue(TEXT("Native save remains loadable after tutorial content was active"),FPadmaRunState::StaticStruct()->CompareScriptStruct(&NativeSave,&NativeFirst.Run(),0));
    if (!Step(*this,NativeFirst.Restore(TutorialSave))) return false;

    auto BadTutorial=TutorialSave;
    BadTutorial.MapLayout.Nodes.FindByPredicate([](const auto& N) { return N.Id==TEXT("story"); })->Position.X+=.5;
    TestFalse(TEXT("Derived tutorial reference rejects a saved moved anchor"),NativeFirst.Restore(BadTutorial).bOk);
    TestTrue(TEXT("Rejected tutorial cannot replace the active run"),FPadmaRunState::StaticStruct()->CompareScriptStruct(&TutorialSave,&NativeFirst.Run(),0));
    auto BadNative=NativeSave;
    BadNative.MapLayout.Nodes.FindByPredicate([](const auto& N) { return N.Id==TEXT("story"); })->Position.X+=.5;
    TestFalse(TEXT("Retained authored reference rejects a saved moved anchor"),TutorialFirst.Restore(BadNative).bOk);
    TestTrue(TEXT("Rejected authored save cannot replace the active run"),FPadmaRunState::StaticStruct()->CompareScriptStruct(&TutorialSave,&TutorialFirst.Run(),0));
    auto Unknown=NativeSave; Unknown.MapLayout.MapId=TEXT("unconfigured-map");
    TestFalse(TEXT("Saved layout cannot establish its own map authority"),TutorialFirst.Restore(Unknown).bOk);
    FPadmaRunRules StandaloneTutorial; StandaloneTutorial.Initialize(TutorialContent);
    if (!Step(*this,StandaloneTutorial.Start())) return false;
    TestFalse(TEXT("Tutorial-only setup does not invent an authored reference"),StandaloneTutorial.Restore(NativeSave).bOk);
    return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaTutorialGridVersionsTest,"DreamOfPadma.WorldMap.Tutorial.VersionedGridAndSavedLayout",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaTutorialGridVersionsTest::RunTest(const FString& Parameters)
{
    const auto Source=SourceMap(); FString Error; FPadmaMapLayout Maps[2];
    for (int32 I=0; I<2; ++I)
        if (!TestTrue(TEXT("Supported grid version generates"),FPadmaTutorialMapGenerator::Generate(Source,12345,Maps[I],Error,I+1)))
        { AddError(Error); return false; }
    TestEqual(TEXT("Legacy v1 retains all 162 cells"),Maps[0].Nodes.Num(),162);
    TestEqual(TEXT("V2 has 55 cells including the seven landmarks"),Maps[1].Nodes.Num(),55);
    TestTrue(TEXT("Absent new grid property defaults to the complete v1 projection"),Maps[0].HexGrid==FPadmaHexGridConfig());
    const FName Ids[]={TEXT("boss"),TEXT("fire"),TEXT("forge"),TEXT("gate"),TEXT("home"),TEXT("plain"),TEXT("story")};
    const FIntPoint LegacyCells[]={{7,2},{-4,12},{1,12},{4,7},{-3,8},{0,9},{2,4}};
    for (int32 I=0; I<UE_ARRAY_COUNT(Ids); ++I)
    {
        const auto* Legacy=Maps[0].Nodes.FindByPredicate([&](const auto& N) { return N.Id==Ids[I]; });
        const auto* Current=Maps[1].Nodes.FindByPredicate([&](const auto& N) { return N.Id==Ids[I]; });
        if (!TestNotNull(TEXT("Legacy landmark remains present"),Legacy)||!TestNotNull(TEXT("V2 landmark remains present"),Current)) return false;
        TestEqual(TEXT("Legacy anchor axial coordinates are frozen"),FIntPoint(Legacy->HexQ,Legacy->HexR),LegacyCells[I]);
        TestEqual(TEXT("Density change retains exact painting coordinates"),Current->Position,Legacy->Position);
    }
    for (const auto& Map:Maps)
    {
        const auto& Grid=Map.HexGrid;
        TestEqual(TEXT("Origin is shared by geometry helpers"),FPadmaTutorialMapGenerator::CellCenter(Grid,0,0),Grid.Origin);
        TestTrue(TEXT("Adjacent pointy hexes share their upper corner"),
            FPadmaTutorialMapGenerator::CellCorner(Grid,0,0,0).Equals(FPadmaTutorialMapGenerator::CellCorner(Grid,1,0,2),1.e-8));
        TestTrue(TEXT("Adjacent pointy hexes share their lower corner"),
            FPadmaTutorialMapGenerator::CellCorner(Grid,0,0,5).Equals(FPadmaTutorialMapGenerator::CellCorner(Grid,1,0,3),1.e-8));
        for (const auto& Node:Map.Nodes)
        {
            TestTrue(TEXT("Every saved cell is inside its own grid config"),FPadmaTutorialMapGenerator::ContainsCell(Grid,Node.HexQ,Node.HexR));
            if (Node.bWilderness) TestEqual(TEXT("Wilderness center uses the shared saved projection"),
                Node.Position,FPadmaTutorialMapGenerator::CellCenter(Grid,Node.HexQ,Node.HexR));
        }
    }

    // Exercise reflected missing-property behavior separately from the new full binary round trip.
    const auto OldJson=FJsonObjectConverter::UStructToJsonObject(Maps[0]);
    if (!TestTrue(TEXT("Legacy layout exports through reflection"),OldJson.IsValid())) return false;
    TestTrue(TEXT("Export contains the new projected grid property"),OldJson->HasField(TEXT("hexGrid")));
    OldJson->RemoveField(TEXT("hexGrid"));
    FPadmaMapLayout MissingGrid;
    if (!TestTrue(TEXT("Old values without HexGrid deserialize"),FJsonObjectConverter::JsonObjectToUStruct(OldJson.ToSharedRef(),&MissingGrid))) return false;
    TestTrue(TEXT("Old missing-field layout validates with legacy grid defaults"),FPadmaWorldMapGenerator::Validate(MissingGrid,Error));
    TestEqual(TEXT("Old missing-field layout retains its signature"),FPadmaWorldMapGenerator::Signature(MissingGrid),FPadmaWorldMapGenerator::Signature(Maps[0]));
    Maps[0]=MoveTemp(MissingGrid);
    auto ChangedGrid=Maps[1]; ChangedGrid.HexGrid.Radius+=.1;
    TestTrue(TEXT("V2 signature includes the frozen projected grid values"),FPadmaWorldMapGenerator::Signature(ChangedGrid)!=FPadmaWorldMapGenerator::Signature(Maps[1]));
    TestFalse(TEXT("Saved config cannot invent a new trusted anchor grid"),FPadmaWorldMapGenerator::Validate(ChangedGrid,Error));
    auto LegacyGrid=Maps[0]; LegacyGrid.HexGrid.Radius+=.1;
    TestEqual(TEXT("V1 signature format predates the new property"),FPadmaWorldMapGenerator::Signature(LegacyGrid),FPadmaWorldMapGenerator::Signature(Maps[0]));
    TestFalse(TEXT("Unsigned legacy config is still constrained to its original preset"),FPadmaWorldMapGenerator::Validate(LegacyGrid,Error));
    FPadmaMapLayout Output=Maps[1];
    TestFalse(TEXT("Unknown generation version is rejected"),FPadmaTutorialMapGenerator::Generate(Source,12345,Output,Error,99));
    TestTrue(TEXT("Unsupported generation leaves output untouched"),FPadmaMapLayout::StaticStruct()->CompareScriptStruct(&Output,&Maps[1],0));

    FPadmaContentSnapshot Base,Contents[2]; FPadmaRunState Saves[2];
    if (!LoadPadmaDemoTestContent(Base,Error)) { AddError(Error); return false; }
    for (int32 I=0; I<2; ++I)
    {
        Contents[I]=Base;
        if (!FPadmaWorldMapGenerator::ApplyToContent(Maps[I],Contents[I],Error)) { AddError(Error); return false; }
        FPadmaRunRules Maker; Maker.Initialize(Contents[I],&Source);
        if (!Step(*this,Maker.Start())||!Step(*this,Maker.Advance())||!Step(*this,Maker.Deploy(TEXT("card-1"),TEXT("home")))) return false;
        FName Wild;
        for (const auto& Edge:Maps[I].Edges)
            if (Edge.From==TEXT("home")&&Maker.PreviewWildernessOccupation(Edge.From,Edge.To,TEXT("player")).bOk) { Wild=Edge.To; break; }
        if (!TestFalse(TEXT("Both versions retain a payable player frontier"),Wild.IsNone())) return false;
        const double Budget=Maker.Run().Resources.Calc,War=Maker.Run().War;
        if (!Step(*this,Maker.Move({TEXT("card-1")},Wild))||!Step(*this,Maker.Advance())) return false;
        TestEqual(TEXT("Both versions charge normal movement"),Maker.Run().Resources.Calc,Budget-Maker.P(TEXT("moveA")));
        TestEqual(TEXT("Both versions capture without farming war"),Maker.Run().War,War);
        TestEqual(TEXT("Both versions retain a bounded dusk frontier"),RulerWildCount(Maker),1);
        auto* Save=NewObject<UPadmaRunSaveGame>(); Save->Run=Maker.Run(); TArray<uint8> Bytes;
        if (!TestTrue(TEXT("Versioned grid saves to memory"),UGameplayStatics::SaveGameToMemory(Save,Bytes))) return false;
        const auto* Loaded=UPadmaRunSaveGame::LoadFromMemory(Bytes);
        if (!TestNotNull(TEXT("Versioned grid loads from memory"),Loaded)) return false;
        Saves[I]=Loaded->Run;
        TestTrue(TEXT("Memory save preserves config, terrain and occupation exactly"),FPadmaRunState::StaticStruct()->CompareScriptStruct(&Maker.Run(),&Saves[I],0));
    }
    for (int32 First=0; First<2; ++First)
    {
        // Explicit authored initialization and tutorial-only initialization both retain their trusted source.
        for (bool bExplicitSource:{false,true})
        {
            FPadmaRunRules Reader; Reader.Initialize(Contents[First],bExplicitSource?&Source:nullptr);
            if (!Step(*this,Reader.Start())) return false;
            for (int32 Index:{1-First,First,1-First})
            {
                if (!Step(*this,Reader.Restore(Saves[Index]))) return false;
                TestTrue(TEXT("Either load order restores frozen cells and territory without regeneration"),
                    FPadmaRunState::StaticStruct()->CompareScriptStruct(&Reader.Run(),&Saves[Index],0));
            }
            const auto Before=Reader.Run(); auto Forged=Before;
            Forged.MapLayout.HexGrid.Origin.X+=.25;
            TestFalse(TEXT("Load rejects forged grid parameters"),Reader.Restore(Forged).bOk);
            Forged=Before; Forged.MapLayout.GeneratorVersion=99;
            TestFalse(TEXT("Load rejects unknown grid versions"),Reader.Restore(Forged).bOk);
            Forged=Before; Forged.MapLayout.Nodes.FindByPredicate([](const auto& N) { return N.Id==TEXT("story"); })->Position.X+=.25;
            TestFalse(TEXT("Version-aware load still rejects moved story anchors"),Reader.Restore(Forged).bOk);
            TestTrue(TEXT("Rejected grid and anchor saves are atomic"),FPadmaRunState::StaticStruct()->CompareScriptStruct(&Reader.Run(),&Before,0));
        }
    }
    return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaTutorialNearbyGuardSaveTest,"DreamOfPadma.WorldMap.Tutorial.NearbyGuardAndLegacySaves",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaTutorialNearbyGuardSaveTest::RunTest(const FString& Parameters)
{
    FPadmaContentSnapshot Base;FString Error;
    if(!LoadPadmaDemoTestContent(Base,Error)){AddError(Error);return false;}
    const auto Source=SourceMap();FPadmaMapLayout Current;
    if(!TestTrue(TEXT("Current tutorial generates"),FPadmaTutorialMapGenerator::Generate(Source,12345,Current,Error)))return false;
    TestEqual(TEXT("Guard change retains 55 cells"),Current.Nodes.Num(),55);
    const auto* Guard=Current.Nodes.FindByPredicate([](const auto& N){return N.Id==TEXT("fire");});
    if(!TestNotNull(TEXT("Nearby outpost exists"),Guard))return false;
    TestEqual(TEXT("Nearby outpost initially belongs to ruler"),Guard->InitialOwner,FName(TEXT("ruler")));
    TestEqual(TEXT("Nearby outpost has one guard"),Guard->EnemyCount,1);
    TestTrue(TEXT("Home has an unlocked direct route to outpost"),Current.Edges.ContainsByPredicate([](const auto& E){return E.From==TEXT("home")&&E.To==TEXT("fire")&&E.UnlockFlag.IsNone();}));
    auto NewContent=Base;
    if(!FPadmaWorldMapGenerator::ApplyToContent(Current,NewContent,Error)){AddError(Error);return false;}
    FPadmaRunRules NewReader;NewReader.Initialize(NewContent,&Source);
    if(!Step(*this,NewReader.Start()))return false;
    const auto NewSave=NewReader.Run();
    for(int32 Version:{1,2})
    {
        FPadmaMapLayout Legacy;auto OldContent=Base;
        if(!FPadmaTutorialMapGenerator::Generate(Source,12345,Legacy,Error,Version)
           ||!FPadmaWorldMapGenerator::ApplyToContent(Legacy,OldContent,Error)){AddError(Error);return false;}
        FPadmaRunRules OldReader;OldReader.Initialize(OldContent,&Source);
        if(!Step(*this,OldReader.Start()))return false;
        const auto OldSave=OldReader.Run();
        if(!Step(*this,NewReader.Restore(OldSave)))return false;
        TestTrue(TEXT("Old topology and territory restore exactly without new guards"),FPadmaRunState::StaticStruct()->CompareScriptStruct(&OldSave,&NewReader.Run(),0));
        if(!Step(*this,NewReader.Restore(NewSave))||!Step(*this,OldReader.Restore(NewSave)))return false;
        TestTrue(TEXT("Trusted old-map session can restore new guard layout"),FPadmaRunState::StaticStruct()->CompareScriptStruct(&NewSave,&OldReader.Run(),0));
    }
    auto Forged=NewSave;
    Forged.MapLayout.Nodes.FindByPredicate([](const auto& N){return N.Id==TEXT("fire");})->Position.X+=.25;
    TestFalse(TEXT("Saved layout cannot move the nearby fixed outpost"),NewReader.Restore(Forged).bOk);
    TestTrue(TEXT("Rejected outpost relocation preserves current run"),FPadmaRunState::StaticStruct()->CompareScriptStruct(&NewSave,&NewReader.Run(),0));
    return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaTutorialProjectionTest,"DreamOfPadma.UI.Tutorial.PanZoomAndVersionedHitProjection",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaTutorialProjectionTest::RunTest(const FString& Parameters)
{
    const FVector2D Size(1296,635); FPadmaTutorialMapTransform View;
    TestEqual(TEXT("Initial painting top left fits the viewport"),View.Project(FVector2D::ZeroVector,Size),FVector2D::ZeroVector);
    TestEqual(TEXT("Initial painting bottom right fits the viewport"),View.Project(FVector2D(100,100),Size),Size);
    View.PanBy(FVector2D::ZeroVector,Size);
    TestEqual(TEXT("No input does not crop the initial painting"),View.Zoom,1.f);
    View.PanBy(FVector2D(32,-20),Size);
    TestTrue(TEXT("First default-zoom pan creates painted overscan"),View.Zoom>1.f);
    TestTrue(TEXT("First horizontal and vertical input moves the image"),View.Pan.X>0. && View.Pan.Y<0.);
    TestTrue(TEXT("Requested translation is applied in viewport pixels"),View.Project(FVector2D(50,50),Size).Equals(Size*.5+FVector2D(32,-20),1.e-8));
    for (int32 Version:{1,2})
    {
        FPadmaHexGridConfig Grid;
        if (!TestTrue(TEXT("Hit projection obtains a saved grid preset"),FPadmaTutorialMapGenerator::GridForVersion(Version,Grid))) return false;
        const FVector2D Center=FPadmaTutorialMapGenerator::CellCenter(Grid,1,3);
        const FVector2D Recovered=View.Unproject(View.Project(Center,Size),Size);
        TestTrue(TEXT("Paint and input invert the same panned transform"),Center.Equals(Recovered,1.e-8));
        TestTrue(TEXT("Painted cell center hits its own versioned polygon"),FPadmaTutorialMapGenerator::ContainsPoint(Grid,1,3,Recovered));
        TestFalse(TEXT("Neighbor center cannot hit the original polygon"),FPadmaTutorialMapGenerator::ContainsPoint(Grid,1,3,FPadmaTutorialMapGenerator::CellCenter(Grid,2,3)));
    }
    for (const FVector2D Direction:{FVector2D(100000,100000),FVector2D(-100000,-100000)})
    {
        View.PanBy(Direction,Size);
        const auto Min=View.Project(FVector2D::ZeroVector,Size),Max=View.Project(FVector2D(100,100),Size);
        TestTrue(TEXT("Clamped pan cannot reveal blank beyond either painted edge"),Min.X<=1.e-8&&Min.Y<=1.e-8&&Max.X>=Size.X-1.e-8&&Max.Y>=Size.Y-1.e-8);
    }
    View.ZoomBy(100); TestEqual(TEXT("Zoom retains its original upper bound"),View.Zoom,1.8f);
    const auto Before=View; View.PanBy(FVector2D(4,8),FVector2D::ZeroVector);
    TestEqual(TEXT("Unavailable geometry cannot change pan"),View.Pan,Before.Pan);
    View.ZoomBy(.0001f);
    TestEqual(TEXT("Zoom retains its original fit lower bound"),View.Zoom,1.f);
    TestEqual(TEXT("Returning to fit removes pan that would expose a blank edge"),View.Pan,FVector2D::ZeroVector);
    View.PanBy(FVector2D(32,20),Size); View.Reset();
    TestEqual(TEXT("Reset restores exact initial composition"),View.Project(FVector2D(100,100),Size),Size);
    return !HasAnyErrors();
}
#endif
