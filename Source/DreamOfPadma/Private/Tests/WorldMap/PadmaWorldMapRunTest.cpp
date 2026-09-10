#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Core/Run/PadmaRunRules.h"
#include "Core/WorldMap/PadmaWorldMapDefinition.h"
#include "Game/Save/PadmaRunSaveGame.h"
#include "Kismet/GameplayStatics.h"

extern bool LoadPadmaDemoTestContent(FPadmaContentSnapshot& Out, FString& Error);
namespace
{
bool Step(FAutomationTestBase& Test, const FPadmaRunCommandResult& Result)
{
    if (!Result.bOk) Test.AddError(Result.Text);
    return Result.bOk;
}
bool Setup(FAutomationTestBase& Test, FPadmaContentSnapshot& Content, FPadmaRunRules& Rules)
{
    FString Error;
    if (!LoadPadmaDemoTestContent(Content, Error)) { Test.AddError(Error); return false; }
    Content.LegacyMapLayout.MapId = TEXT("legacy-html-v1");
    Content.LegacyMapLayout.HomeNode = TEXT("home"); Content.LegacyMapLayout.BossNode = TEXT("boss");
    for (FName Id : Content.NodeOrder) Content.LegacyMapLayout.Nodes.Add(Content.Nodes.FindChecked(Id));
    Content.LegacyMapLayout.Edges = Content.Edges;
    FPadmaMapLayout Map;
    Map.MapId = TEXT("anchor-test"); Map.Seed = 48; Map.HomeNode = TEXT("camp"); Map.BossNode = TEXT("boss");
    for (FName Id : Content.NodeOrder)
    {
        auto Node = Content.Nodes.FindChecked(Id);
        if (Id == TEXT("home")) Node.Id = TEXT("camp");
        if (Id == TEXT("story"))
        {
            Node.AnchorKind = EPadmaMapAnchorKind::MainStory;
            Node.NPCId = TEXT("watcher"); Node.DialogueId = TEXT("story"); Node.CompletionFlag = TEXT("story-road");
        }
        if (Id == TEXT("forge")) { Node.AnchorKind = EPadmaMapAnchorKind::FixedNPC; Node.NPCId = TEXT("smith"); Node.DialogueId = TEXT("forge"); }
        Map.Nodes.Add(Node);
    }
    Map.Edges = Content.Edges;
    for (auto& Edge : Map.Edges) if (Edge.From == TEXT("home")) Edge.From = TEXT("camp");
    FPadmaStoryCheckpointRow Check;
    Check.Id = TEXT("watcher-gate"); Check.NodeId = TEXT("gate"); Check.NPCId = TEXT("watcher");
    Check.MatchedFlag = TEXT("watcher-aid"); Check.MissedFlag = TEXT("watcher-absent");
    Check.MatchedDialogueId = TEXT("gate-met"); Check.MissedDialogueId = TEXT("gate-missed");
    Map.Checkpoints.Add(Check);
    for (FName Id : {FName(TEXT("gate-met")), FName(TEXT("gate-missed"))})
    {
        FPadmaDialogueLineRow Line;
        Line.Id = FName(*(Id.ToString() + TEXT(".start"))); Line.DialogueId = Id; Line.LineId = TEXT("start");
        Line.bStart = true; Line.Title = FText::FromString(TEXT("Checkpoint")); Line.Speaker = FText::FromString(TEXT("Narrator")); Line.Text = FText::FromName(Id);
        FPadmaDialogueChoice Choice; Choice.Id = TEXT("finish"); Choice.Label = FText::FromString(TEXT("Continue")); Choice.bComplete = true;
        Line.Choices.Add(Choice); Content.Dialogues.Add(Line.Id, Line);
    }
    if (!FPadmaWorldMapGenerator::ApplyToContent(Map, Content, Error)) { Test.AddError(Error); return false; }
    Rules.Initialize(Content);
    return Step(Test, Rules.Start()) && Step(Test, Rules.Advance()) && Step(Test, Rules.Deploy(TEXT("card-3"), TEXT("camp")));
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaAnchoredStoryRunTest, "DreamOfPadma.WorldMap.Run.CharacterCheckpointAndRollback",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPadmaAnchoredStoryRunTest::RunTest(const FString& Parameters)
{
    FPadmaContentSnapshot Content; FPadmaRunRules Rules;
    if (!Setup(*this, Content, Rules)) return false;
    if (!Step(*this, Rules.Move({TEXT("card-3")}, TEXT("story")))) return false;
    TestTrue(TEXT("Arrival starts main-character dialogue"), Rules.IsDialogue());
    TestTrue(TEXT("Encounter records seen identity"), Rules.Run().SeenNPCs.Contains(TEXT("watcher")));
    TestFalse(TEXT("Seen is not completed"), Rules.Run().CompletedNPCs.Contains(TEXT("watcher")));
    Step(*this, Rules.DialogueCancel());
    TestTrue(TEXT("Cancel preserves encounter but grants no completion"), Rules.Run().SeenNPCs.Contains(TEXT("watcher")) && Rules.Run().CompletedNPCs.IsEmpty());
    Step(*this, Rules.Operation(TEXT("story")));
    Step(*this, Rules.DialogueNext()); Step(*this, Rules.DialogueNext(TEXT("ask"))); Step(*this, Rules.DialogueNext()); Step(*this, Rules.DialogueNext(TEXT("accept")));
    TestTrue(TEXT("Full conversation records completion"), Rules.Run().CompletedNPCs.Contains(TEXT("watcher")));
    if (!Step(*this, Rules.Move({TEXT("card-3")}, TEXT("plain")))) return false;
    const auto BeforeBattle = Rules.Run();
    if (!Step(*this, Rules.Move({TEXT("card-3")}, TEXT("gate"), TEXT("encounter")))) return false;
    TestTrue(TEXT("Pending battle has not committed checkpoint"), Rules.Run().StoryCheckpoints.IsEmpty());
    Step(*this, Rules.FinishBattle(false));
    TestTrue(TEXT("Defeat restores map and complete story state"), FPadmaRunState::StaticStruct()->CompareScriptStruct(&BeforeBattle, &Rules.Run(), 0));
    Step(*this, Rules.Move({TEXT("card-3")}, TEXT("gate"), TEXT("encounter")));
    Step(*this, Rules.FinishBattle(true, {{TEXT("card-3"), 40}}, 30, 1, 1));
    TestTrue(TEXT("Committed checkpoint uses completed storyline"), Rules.Run().StoryCheckpoints.FindRef(TEXT("watcher-gate")));
    TestEqual(TEXT("Matching authored branch opens"), Rules.Dialogue().DialogueId, FName(TEXT("gate-met")));
    Step(*this, Rules.DialogueNext(TEXT("finish")));
    TestTrue(TEXT("Branch result is valid save state"), Rules.ValidateState(Rules.Run()).bOk);
    auto* Save = NewObject<UPadmaRunSaveGame>(); Save->Run = Rules.Run();
    TArray<uint8> Bytes; UGameplayStatics::SaveGameToMemory(Save, Bytes);
    auto* Loaded = UPadmaRunSaveGame::LoadFromMemory(Bytes);
    if (!TestNotNull(TEXT("Map and branch save loads"), Loaded)) return false;
    auto Different = Content;
    FPadmaMapGenerationSettings Settings; Settings.BranchCountMin = Settings.BranchCountMax = 2;
    FPadmaMapLayout Generated; FString Error;
    if (!TestTrue(TEXT("Other seed generates"), FPadmaWorldMapGenerator::Generate(Content.MapLayout, Settings, 732, Generated, Error))) return false;
    FPadmaWorldMapGenerator::ApplyToContent(Generated, Different, Error);
    FPadmaRunRules Restored; Restored.Initialize(Different);
    if (!Step(*this, Restored.Restore(Loaded->Run))) return false;
    TestTrue(TEXT("Load uses saved topology and branch, not new seed"), FPadmaRunState::StaticStruct()->CompareScriptStruct(&Rules.Run(), &Restored.Run(), 0));
    auto Corrupt = Loaded->Run; Corrupt.MapLayout.Nodes.FindByPredicate([](const auto& N) { return N.Id == TEXT("story"); })->Position.X += 1;
    TestFalse(TEXT("Moved anchor in save rejected"), Restored.Restore(Corrupt).bOk);
    TestTrue(TEXT("Rejected save leaves previous run intact"), FPadmaRunState::StaticStruct()->CompareScriptStruct(&Rules.Run(), &Restored.Run(), 0));
    return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaSkippedStoryRunTest, "DreamOfPadma.WorldMap.Run.BypassBranchesAtCheckpoint",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPadmaSkippedStoryRunTest::RunTest(const FString& Parameters)
{
    FPadmaContentSnapshot Content; FPadmaRunRules Rules;
    if (!Setup(*this, Content, Rules)) return false;
    Step(*this, Rules.Move({TEXT("card-3")}, TEXT("fire")));
    Step(*this, Rules.Move({TEXT("card-3")}, TEXT("plain")));
    TestTrue(TEXT("Detour does not immediately declare story missed"), Rules.Run().StoryCheckpoints.IsEmpty());
    Step(*this, Rules.Move({TEXT("card-3")}, TEXT("gate"), TEXT("encounter")));
    Step(*this, Rules.FinishBattle(true, {{TEXT("card-3"), 40}}, 30, 1, 1));
    TestTrue(TEXT("Checkpoint persists a missed branch"), Rules.Run().StoryCheckpoints.Contains(TEXT("watcher-gate")) && !Rules.Run().StoryCheckpoints.FindRef(TEXT("watcher-gate")));
    TestEqual(TEXT("Alternate conversation chosen"), Rules.Dialogue().DialogueId, FName(TEXT("gate-missed")));
    Step(*this, Rules.DialogueNext(TEXT("finish")));
    TestTrue(TEXT("Bypass is valid rather than a universal progression gate"), Rules.ValidateState(Rules.Run()).bOk);
    TestTrue(TEXT("Untouched fixed NPC remains on the map"), Rules.Content().Nodes.FindChecked(TEXT("forge")).NPCId == TEXT("smith"));
    return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaMapRestoreOrderTest, "DreamOfPadma.WorldMap.Run.LegacyAndCurrentRestoreOrder",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPadmaMapRestoreOrderTest::RunTest(const FString& Parameters)
{
    FPadmaContentSnapshot Content; FPadmaRunRules Rules;
    if (!Setup(*this, Content, Rules)) return false;
    const auto Current = Rules.Run();
    FPadmaContentSnapshot Legacy; FString Error; FPadmaRunRules Old;
    if (!LoadPadmaDemoTestContent(Legacy, Error)) { AddError(Error); return false; }
    Old.Initialize(Legacy); if (!Step(*this, Old.Start())) return false;
    if (!Step(*this, Rules.Restore(Old.Run()))) return false;
    TestEqual(TEXT("Pre-layout save maps to original graph"), Rules.Run().MapLayout.MapId, FName(TEXT("legacy-html-v1")));
    if (!Step(*this, Rules.Restore(Current))) return false;
    TestTrue(TEXT("Current save remains loadable after legacy import"), FPadmaRunState::StaticStruct()->CompareScriptStruct(&Current, &Rules.Run(), 0));
    return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaCheckpointPhaseTest, "DreamOfPadma.WorldMap.Run.CheckpointOutsideNoon",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPadmaCheckpointPhaseTest::RunTest(const FString& Parameters)
{
    for (int32 Phase : {0, 2})
    {
        FPadmaContentSnapshot Content; FPadmaRunRules Rules;
        if (!Setup(*this, Content, Rules)) return false;
        if (!Step(*this, Rules.Move({TEXT("card-3")}, TEXT("fire"))) || !Step(*this, Rules.Move({TEXT("card-3")}, TEXT("plain")))) return false;
        auto Snapshot = Rules.Run(); Snapshot.Calendar.Phase = Phase;
        if (!Step(*this, Rules.Restore(Snapshot))) return false;
        if (!Step(*this, Rules.Move({TEXT("card-3")}, TEXT("gate"), TEXT("encounter"))) || !Step(*this, Rules.FinishBattle(true, {{TEXT("card-3"), 40}}, 30, 1, 1))) return false;
        TestFalse(TEXT("Arrival does not open an unusable non-noon dialogue"), Rules.IsDialogue());
        TestTrue(TEXT("Checkpoint still commits at arrival"), Rules.Run().StoryCheckpoints.Contains(TEXT("watcher-gate")));
        for (int32 StepIndex=0; Rules.Run().Calendar.Phase!=1 && StepIndex<3; ++StepIndex) if (!Step(*this, Rules.Advance())) return false;
        if (!Step(*this, Rules.Operation(TEXT("gate"))) || !Step(*this, Rules.DialogueNext(TEXT("finish")))) return false;
        TestTrue(TEXT("Saved branch completes at noon"), Rules.ValidateState(Rules.Run()).bOk);
    }
    return !HasAnyErrors();
}
#endif
