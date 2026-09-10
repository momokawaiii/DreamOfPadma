#include "Misc/AutomationTest.h"
#include "Core/Run/PadmaRunRules.h"
#include "Game/Run/PadmaRunSubsystem.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/Class.h"
#include <limits>

#if WITH_DEV_AUTOMATION_TESTS

// Data worker provides the checked, source-exported value catalog. No maps or cooked assets.
extern bool LoadPadmaDemoTestContent(FPadmaContentSnapshot& Out, FString& Error);

namespace
{
bool Ready(FAutomationTestBase& Test, FPadmaRunRules& Rules)
{
    FPadmaContentSnapshot Content;
    FString Error;
    if (!LoadPadmaDemoTestContent(Content, Error)) { Test.AddError(TEXT("Source fixture: ") + Error); return false; }
    Rules.Initialize(Content);
    const auto Result = Rules.Start();
    if (!Result.bOk) { Test.AddError(Result.Text); return false; }
    return true;
}
bool SameState(const FPadmaRunState& A, const FPadmaRunState& B)
{
    return FPadmaRunState::StaticStruct()->CompareScriptStruct(&A, &B, 0);
}
bool Do(FAutomationTestBase& Test, const FPadmaRunCommandResult& Result)
{
    if (!Result.bOk) Test.AddError(Result.Text);
    return Result.bOk;
}
bool AtPlain(FAutomationTestBase& Test, FPadmaRunRules& Rules)
{
    return Ready(Test, Rules) && Do(Test, Rules.Advance()) && Do(Test, Rules.Deploy(TEXT("card-3"), TEXT("home")))
        && Do(Test, Rules.Move({TEXT("card-3")}, TEXT("fire"))) && Do(Test, Rules.Move({TEXT("card-3")}, TEXT("plain")));
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaRunAtomicTest, "DreamOfPadma.MVP.TASK046.Run.InvalidCommandsAreAtomic",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPadmaRunAtomicTest::RunTest(const FString& Parameters)
{
    FPadmaRunRules Rules;
    if (!Ready(*this, Rules)) return false;
    const auto Before = Rules.Run();
    const uint64 Revision = Rules.Revision();
    TestFalse(TEXT("Deployment at dawn rejected"), Rules.Deploy(TEXT("card-1"), TEXT("home")).bOk);
    TestTrue(TEXT("Rejected deployment preserves every run property"), SameState(Before, Rules.Run()));
    TestFalse(TEXT("Unknown instance rejected"), Rules.Move({TEXT("unknown")}, TEXT("fire")).bOk);
    TestTrue(TEXT("Unknown instance leaves no resource/log mutation"), SameState(Before, Rules.Run()));
    TestFalse(TEXT("Fractional daily draw rejected"), Rules.Start({{TEXT("dailyDraw"), 0.5}}).bOk);
    TestTrue(TEXT("Invalid new-run settings cannot replace active run"), SameState(Before, Rules.Run()));
    TestTrue(TEXT("Notice revision is outside rollback"), Rules.Revision() > Revision);
    TestTrue(TEXT("New run passes the same save validation"), Rules.ValidateState(Rules.Run()).bOk);
    TestEqual(TEXT("Exact HTML opening hand size"), Rules.Run().Hand.Num(), 7);
    TestEqual(TEXT("Exact HTML deck size"), Rules.Run().Deck.Num(), 14);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaRunNoonTest, "DreamOfPadma.MVP.TASK046.Run.CalendarNoonOnce",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPadmaRunNoonTest::RunTest(const FString& Parameters)
{
    FPadmaRunRules Rules;
    if (!Ready(*this, Rules) || !Do(*this, Rules.Advance()) || !Do(*this, Rules.Deploy(TEXT("card-1"), TEXT("home"))) || !Do(*this, Rules.Deploy(TEXT("card-2"), TEXT("home"), TEXT("card-1")))) return false;
    TestEqual(TEXT("Deployment after noon entry does not accumulate retroactively"), Rules.Card(TEXT("card-2"))->Field, FVector2D(1, 3));
    Do(*this, Rules.Advance());
    TestEqual(TEXT("Dusk does not accumulate"), Rules.Card(TEXT("card-2"))->Field, FVector2D(1, 3));
    Do(*this, Rules.Advance());
    TestEqual(TEXT("Dawn advances absolute day"), Rules.Run().Calendar.AbsoluteDay, 2);
    TestEqual(TEXT("Dawn capped flow recovery"), Rules.Run().Resources.Flow, 80.0);
    TestEqual(TEXT("Dawn draws two ABC without recycle"), Rules.Run().Hand.Num(), 7);
    Do(*this, Rules.Advance());
    TestEqual(TEXT("Noon adds A tendency once"), Rules.Card(TEXT("card-2"))->Field, FVector2D(3, 4));
    TestEqual(TEXT("Accumulator persisted day"), Rules.Card(TEXT("card-2"))->LastAccumulatedDay, 2);
    const auto Noon = Rules.Run();
    TestFalse(TEXT("Re-deployment rejected"), Rules.Deploy(TEXT("card-2"), TEXT("home"), TEXT("card-1")).bOk);
    TestTrue(TEXT("Failed command cannot repeat noon accumulation"), SameState(Noon, Rules.Run()));
    for (int32 I = 0; I < 18; ++I) Do(*this, Rules.Advance());
    TestEqual(TEXT("Seven days form an era"), Rules.Run().Calendar.Day, 1);
    TestEqual(TEXT("Era increments at day eight"), Rules.Run().Calendar.Era, 2);
    TestEqual(TEXT("One accumulation per following noon"), Rules.Card(TEXT("card-2"))->Field, FVector2D(15, 10));
    TestTrue(TEXT("Calendar and accumulation remain saveable"), Rules.ValidateState(Rules.Run()).bOk);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaRunSynthesisTest, "DreamOfPadma.MVP.TASK046.Run.SynthesisMatchesHTMLGolden",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPadmaRunSynthesisTest::RunTest(const FString& Parameters)
{
    FPadmaRunRules Rules;
    if (!Ready(*this, Rules) || !Do(*this, Rules.Advance()) || !Do(*this, Rules.Deploy(TEXT("card-1"), TEXT("home"))) || !Do(*this, Rules.Deploy(TEXT("card-2"), TEXT("home"), TEXT("card-1")))) return false;
    const auto BeforePreview = Rules.Run();
    FPadmaSynthesisRecord Preview;
    if (!Do(*this, Rules.PreviewSynthesis(TEXT("card-1"), Preview))) return false;
    TestTrue(TEXT("Preview cannot consume RNG or resources"), SameState(BeforePreview, Rules.Run()));
    if (!Do(*this, Rules.Synthesize(TEXT("card-1")))) return false;
    const auto& Record = Rules.Run().SynthesisHistory.Last();
    // Independently obtained from Prototypes/HTML/engine.js at seed 731, day-one dawn+cherry.
    TestEqual(TEXT("First HTML LCG roll"), Record.SelectionRoll, 0.5193687970750034);
    TestEqual(TEXT("Second HTML LCG roll"), Record.SuccessRoll, 0.5830192428547889);
    TestEqual(TEXT("Exactly two random draws"), Rules.Run().RNG, 2504048581u);
    TestEqual(TEXT("Ordered softmax selection"), Record.Selected, FName(TEXT("vitruvian")));
    TestTrue(TEXT("HTML success probability accepted"), Record.bSuccess);
    TestEqual(TEXT("A discarded"), Rules.Card(TEXT("card-1"))->Zone, FName(TEXT("discard")));
    TestEqual(TEXT("B discarded and detached"), Rules.Card(TEXT("card-2"))->AttachedTo, NAME_None);
    TestEqual(TEXT("B zone is discard"), Rules.Card(TEXT("card-2"))->Zone, FName(TEXT("discard")));
    TestEqual(TEXT("C is created directly on world node"), Rules.Card(Record.CreatedId)->Node, FName(TEXT("home")));
    TestEqual(TEXT("C is not placed into the hand"), Rules.Card(Record.CreatedId)->Zone, FName(TEXT("world")));
    TestEqual(TEXT("No extra C deployment fee"), Rules.Run().Resources.Flow, 51.0);
    TestEqual(TEXT("Synthesis calculation cost"), Rules.Run().Resources.Calc, 37.0);
    TestEqual(TEXT("Cherry successful entropy reduction"), Rules.Run().Resources.Entropy, 7.0);
    TestEqual(TEXT("Vitruvian on-play self trigger"), Rules.Card(Record.CreatedId)->Attack, 1.0);
    TestTrue(TEXT("Synthesis record and references save safely"), Rules.ValidateState(Rules.Run()).bOk);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaRunGroupTest, "DreamOfPadma.MVP.TASK046.Run.AttachedGroupRejection",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPadmaRunGroupTest::RunTest(const FString& Parameters)
{
    FPadmaRunRules Rules;
    if (!Ready(*this, Rules) || !Do(*this, Rules.Advance()) || !Do(*this, Rules.Deploy(TEXT("card-4"), TEXT("home"))) || !Do(*this, Rules.Deploy(TEXT("card-5"), TEXT("home"), TEXT("card-4")))) return false;
    if (!Do(*this, Rules.Move({TEXT("card-5")}, TEXT("fire"))) || !Do(*this, Rules.Move({TEXT("card-4")}, TEXT("plain")))) return false;
    TestEqual(TEXT("Selecting B carries A"), Rules.Card(TEXT("card-4"))->Node, FName(TEXT("plain")));
    TestEqual(TEXT("Selecting A carries B"), Rules.Card(TEXT("card-5"))->Node, FName(TEXT("plain")));
    FPadmaCardMovePreview Preview;
    Do(*this, Rules.PreviewMove({TEXT("card-4")}, TEXT("gate"), Preview));
    TestEqual(TEXT("Preview expands complete attached group"), Preview.Ids.Num(), 2);
    TestTrue(TEXT("Conditional B identifies a forbidden mountain"), Preview.Checks.ContainsByPredicate([](const auto& C) { return C.Id == TEXT("card-5") && !C.bAllowed; }));
    const auto Before = Rules.Run();
    TestFalse(TEXT("Attached B blocks entire ACT movement"), Rules.Move({TEXT("card-4")}, TEXT("gate"), TEXT("act")).bOk);
    TestTrue(TEXT("No partial payment, movement, capture, or RNG mutation"), SameState(Before, Rules.Run()));
    TestFalse(TEXT("Rejected group creates no pending battle"), Rules.IsBattle());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaRunRollbackTest, "DreamOfPadma.MVP.TASK046.Run.ExactBattleRollback",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPadmaRunRollbackTest::RunTest(const FString& Parameters)
{
    FPadmaRunRules Rules;
    if (!AtPlain(*this, Rules) || !Do(*this, Rules.Advance())) return false;
    const auto Before = Rules.Run();
    const uint64 Revision = Rules.Revision();
    if (!Do(*this, Rules.Move({TEXT("card-3")}, TEXT("gate"), TEXT("encounter")))) return false;
    TestEqual(TEXT("Enemy battle entry allowed at dusk"), Rules.Run().Calendar.Phase, 2);
    TestEqual(TEXT("Movement cost is provisionally paid"), Rules.Run().Resources.Calc, Before.Resources.Calc - 4);
    Do(*this, Rules.TryPay(6));
    for (FName Skill : Rules.Run().SkillSlots) Do(*this, Rules.ConsumeBasicSkill(Skill));
    Do(*this, Rules.ChangeFaith(-2, -1));
    TestEqual(TEXT("Five basic identities exhausted and shuffled"), Rules.Run().SkillCycles, 1);
    TestTrue(TEXT("Basic skill shuffle consumes RNG"), Rules.Run().RNG != Before.RNG);
    const auto During = Rules.Run();
    TestFalse(TEXT("Battle freezes calendar"), Rules.Advance().bOk);
    TestTrue(TEXT("Frozen command preserves provisional state"), SameState(During, Rules.Run()));
    TestFalse(TEXT("Saving battle is unsafe"), Rules.CheckSaveBoundary().bOk);
    TestFalse(TEXT("Loading during battle is rejected"), Rules.Restore(Before).bOk);
    if (!Do(*this, Rules.FinishBattle(false))) return false;
    TestTrue(TEXT("Defeat exactly restores all reflected run properties including RNG and log"), SameState(Before, Rules.Run()));
    TestFalse(TEXT("Pending battle cleared after rollback"), Rules.IsBattle());
    TestTrue(TEXT("UI revision never rolls back"), Rules.Revision() > Revision);
    TestTrue(TEXT("Rollback restores safe save boundary"), Rules.CheckSaveBoundary().bOk);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaRunCommitTest, "DreamOfPadma.MVP.TASK046.Run.BattleCommitAndVictory",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPadmaRunCommitTest::RunTest(const FString& Parameters)
{
    FPadmaRunRules Rules;
    if (!AtPlain(*this, Rules) || !Do(*this, Rules.Move({TEXT("card-3")}, TEXT("gate"), TEXT("encounter")))) return false;
    const auto During = Rules.Run();
    TestFalse(TEXT("Unknown battle unit cannot be committed"), Rules.FinishBattle(true, {{TEXT("enemy-0"), 10}}, -1, 1, 1).bOk);
    TestTrue(TEXT("Invalid battle result is atomic"), SameState(During, Rules.Run()));
    if (!Do(*this, Rules.FinishBattle(true, {{TEXT("card-3"), 40}}, -1, 3, 2))) return false;
    TestEqual(TEXT("Surviving C HP committed"), Rules.Card(TEXT("card-3"))->HP, 40.0);
    TestEqual(TEXT("C movement committed"), Rules.Card(TEXT("card-3"))->Node, FName(TEXT("gate")));
    TestEqual(TEXT("Gate captured once"), Rules.Run().War, 60.0);
    TestTrue(TEXT("Gate save round trip is valid"), Rules.ValidateState(Rules.Run()).bOk);
    if (!Do(*this, Rules.Move({TEXT("card-3")}, TEXT("boss"), TEXT("act"))) || !Do(*this, Rules.FinishBattle(true, {}, 30, 4, 3.5))) return false;
    TestEqual(TEXT("ACT separate character HP committed"), Rules.Run().Preparation.ACTHP, 30.0);
    TestEqual(TEXT("ACT does not overwrite ABC unit HP"), Rules.Card(TEXT("card-3"))->HP, 40.0);
    TestEqual(TEXT("Ruler core strike commits victory"), Rules.Run().RulerCoreHP, 0.0);
    TestEqual(TEXT("Core victory takes precedence over balance"), Rules.Run().Outcome, FName(TEXT("core")));
    TestFalse(TEXT("Ended run rejects world mutation"), Rules.Advance().bOk);
    TestTrue(TEXT("Victory remains safe to save"), Rules.CheckSaveBoundary().bOk);
    TestTrue(TEXT("Victory state validates"), Rules.ValidateState(Rules.Run()).bOk);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaRunDialogueTest, "DreamOfPadma.MVP.TASK046.Run.DialogueCompleteOnceAndPreparation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPadmaRunDialogueTest::RunTest(const FString& Parameters)
{
    FPadmaRunRules Rules;
    if (!Ready(*this, Rules)) return false;
    auto Prep = Rules.Run().Preparation;
    Prep.ACTCharacter = TEXT("scout");
    Prep.ACTWeapon = TEXT("spear");
    Prep.HomeCharacter = TEXT("keeper-b");
    Prep.HomeOutfit = TEXT("formal");
    Prep.FPSCharacter = TEXT("fps-observer");
    Prep.FPSWeapon = TEXT("carbine");
    Prep.ACTHP = 999;
    const auto BeforePrep = Rules.Run();
    if (!Do(*this, Rules.ConfigurePreparation(Prep))) return false;
    TestEqual(TEXT("Preparation does not create ABC cards"), Rules.Run().Cards.Num(), BeforePrep.Cards.Num());
    TestEqual(TEXT("Display and character selection do not heal"), Rules.Run().Preparation.ACTHP, BeforePrep.Preparation.ACTHP);
    TestEqual(TEXT("Preparation does not spend or grant flow"), Rules.Run().Resources.Flow, BeforePrep.Resources.Flow);
    const auto GoodPrep = Rules.Run();
    Prep.HomeWeapon = TEXT("spear");
    Prep.HomePose = TEXT("salute");
    TestFalse(TEXT("Pose incompatible with display weapon rejected"), Rules.ConfigurePreparation(Prep).bOk);
    TestTrue(TEXT("Invalid display is atomic"), SameState(GoodPrep, Rules.Run()));
    if (!Do(*this, Rules.Advance()) || !Do(*this, Rules.Deploy(TEXT("card-3"), TEXT("home"))) || !Do(*this, Rules.Move({TEXT("card-3")}, TEXT("story")))) return false;
    const auto Before = Rules.Run();
    if (!Do(*this, Rules.Operation(TEXT("story")))) return false;
    TestEqual(TEXT("Starting dialogue grants no reward"), Rules.Run().Resources.Calc, Before.Resources.Calc);
    TestFalse(TEXT("Dialogue is unsafe to save"), Rules.CheckSaveBoundary().bOk);
    TestFalse(TEXT("Dialogue blocks unrelated commands"), Rules.Advance().bOk);
    Do(*this, Rules.DialogueCancel());
    TestTrue(TEXT("Cancel leaves exact run values unchanged"), SameState(Before, Rules.Run()));
    Do(*this, Rules.Operation(TEXT("story")));
    Do(*this, Rules.DialogueNext());
    Do(*this, Rules.DialogueNext(TEXT("ask")));
    Do(*this, Rules.DialogueNext());
    Do(*this, Rules.DialogueNext(TEXT("accept")));
    TestEqual(TEXT("Story grants authored calculation aid only on completion"), Rules.Run().Resources.Calc, 46.0);
    TestTrue(TEXT("Story completion unlocks stable edge flag"), Rules.Run().Unlocks.Contains(TEXT("story-road")));
    TestTrue(TEXT("Story event marked used"), Rules.Run().Nodes.FindChecked(TEXT("story")).bUsed);
    const auto Completed = Rules.Run();
    TestFalse(TEXT("Completed event cannot pay twice"), Rules.Operation(TEXT("story")).bOk);
    TestTrue(TEXT("Repeat complete attempt preserves state"), SameState(Completed, Rules.Run()));
    TestTrue(TEXT("Completed dialogue validates safe state"), Rules.ValidateState(Rules.Run()).bOk);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaRunSaveTest, "DreamOfPadma.MVP.TASK046.Run.VersionedSaveRejectsInvalidIds",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPadmaRunSaveTest::RunTest(const FString& Parameters)
{
    FPadmaRunRules Rules;
    if (!Ready(*this, Rules)) return false;
    const auto Before = Rules.Run();
    auto Reject = [&](FPadmaRunState Candidate, const TCHAR* Label)
    {
        TestFalse(Label, Rules.Restore(Candidate).bOk);
        TestTrue(TEXT("Rejected save never replaces current state"), SameState(Before, Rules.Run()));
    };
    auto Candidate = Before;
    Candidate.Cards[0].DefId = TEXT("missing-definition");
    Reject(Candidate, TEXT("Unknown card definition rejected"));
    Candidate = Before;
    Candidate.Hand[0] = TEXT("missing-instance");
    Reject(Candidate, TEXT("Unknown zone instance rejected"));
    Candidate = Before;
    Candidate.Nodes.Add(TEXT("unknown-node"), {});
    Reject(Candidate, TEXT("Unknown map node rejected"));
    Candidate = Before;
    Candidate.SkillsAvailable[0] = TEXT("unknown-basic-skill");
    Reject(Candidate, TEXT("Unknown basic skill rejected"));
    Candidate = Before;
    Candidate.Preparation.ACTCharacter = TEXT("keeper-a");
    Reject(Candidate, TEXT("Home identity is not an ACT identity"));
    Candidate = Before;
    Candidate.NextId = 1;
    Reject(Candidate, TEXT("Reused next card ID rejected"));
    Candidate = Before;
    Candidate.NextId = MAX_int32;
    Reject(Candidate, TEXT("Next card ID cannot overflow"));
    Candidate = Before;
    Candidate.Sequence = MAX_int32;
    Reject(Candidate, TEXT("Log sequence cannot overflow"));
    Candidate = Before;
    Candidate.Cards[2].HP = 0;
    Reject(Candidate, TEXT("Dead hand card cannot be deployed into an unsaveable world"));
    Candidate = Before;
    Candidate.Version = 99;
    Reject(Candidate, TEXT("Unknown run version rejected"));
    Candidate = Before;
    Candidate.Resources.Flow = std::numeric_limits<double>::quiet_NaN();
    Reject(Candidate, TEXT("Nonfinite resource rejected"));

    UPadmaRunSaveGame* Save = NewObject<UPadmaRunSaveGame>();
    Save->Run = Before;
    TArray<uint8> Bytes;
    TestTrue(TEXT("Native value envelope serializes without disk or map"), UGameplayStatics::SaveGameToMemory(Save, Bytes));
    const auto* Reloaded = Cast<UPadmaRunSaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes));
    TestNotNull(TEXT("Native envelope type survives roundtrip"), Reloaded);
    if (Reloaded)
    {
        TestTrue(TEXT("All reflected run fields survive native save serialization"), SameState(Before, Reloaded->Run));
        TestTrue(TEXT("Deserialized values pass strict reference validation"), Rules.Restore(Reloaded->Run).bOk);
    }
    UGameInstance* Game = NewObject<UGameInstance>();
    UPadmaRunSubsystem* Session = NewObject<UPadmaRunSubsystem>(Game);
    Session->Initialize(Rules.Content());
    Do(*this, Session->StartNew());
    Save->FormatVersion = 99;
    TestFalse(TEXT("Envelope version checked before replace"), Session->LoadValue(Save).bOk);
    TestTrue(TEXT("Incompatible envelope preserves session"), SameState(Before, Session->Rules().Run()));
    return true;
}

#endif
