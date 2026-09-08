#include "Misc/AutomationTest.h"

#include "Demo/Session/DemoTransitionSessionSubsystem.h"
#include "Demo/World/DemoWorldFixture.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FDemoTransitionContextTest,
	"DreamOfPadma.Demo.TASK007.TransitionContext",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FDemoTransitionContextTest::RunTest(const FString& Parameters)
{
	FDemoWorldTileDefinition Fixture;
	TArray<FDemoWorldTileDefinition> Fixtures;
	Fixtures.Add(Fixture);

	FDemoWorldSelectionModel SelectionModel;
	SelectionModel.ReplaceFixture(Fixtures);

	FDemoSelectNodeRequest SelectRequest;
	SelectRequest.DemoNodeId = Fixture.DemoNodeId;
	FText Failure;
	TestTrue(TEXT("The stable demo node can be selected"), SelectionModel.Select(SelectRequest, Failure));

	FDemoConfirmNodeSelectionRequest ConfirmRequest;
	ConfirmRequest.DemoNodeId = Fixture.DemoNodeId;
	FDemoTransitionContext ValidContext;
	TestTrue(
		TEXT("The selected fixture produces a typed transition context"),
		SelectionModel.BuildTransitionContext(ConfirmRequest, ValidContext, Failure));
	TestEqual(TEXT("The node identity is preserved"), ValidContext.DemoNodeId.Value, Fixture.DemoNodeId.Value);
	TestEqual(TEXT("The scenario identity is preserved"), ValidContext.TargetScenarioId.Value, Fixture.TargetScenarioId.Value);
	TestEqual(TEXT("The spawn-point identity is preserved"), ValidContext.SpawnPointId.Value, Fixture.SpawnPointId.Value);

	FDemoTransitionContextStore SessionStore;

	TestTrue(TEXT("The valid context is published"), SessionStore.PublishTransition(ValidContext, Failure));

	FDemoTransitionContext InvalidContext = ValidContext;
	InvalidContext.TargetScenarioId = FDemoScenarioId();
	TestFalse(
		TEXT("An invalid replacement is rejected"),
		SessionStore.PublishTransition(InvalidContext, Failure));

	FDemoTransitionContext PeekedContext;
	TestTrue(TEXT("The previously valid context remains pending"), SessionStore.PeekTransition(PeekedContext, Failure));
	TestEqual(TEXT("Invalid publish does not mutate the node identity"), PeekedContext.DemoNodeId.Value, ValidContext.DemoNodeId.Value);
	TestEqual(TEXT("Invalid publish does not mutate the scenario identity"), PeekedContext.TargetScenarioId.Value, ValidContext.TargetScenarioId.Value);
	TestEqual(TEXT("Invalid publish does not mutate the spawn-point identity"), PeekedContext.SpawnPointId.Value, ValidContext.SpawnPointId.Value);

	Fixtures[0].TargetScenarioId = FDemoScenarioId();
	SelectionModel.ReplaceFixture(Fixtures);
	FDemoTransitionContext UnchangedCandidate = ValidContext;
	TestFalse(
		TEXT("A missing fixture field fails before a context is produced"),
		SelectionModel.BuildTransitionContext(ConfirmRequest, UnchangedCandidate, Failure));
	TestEqual(
		TEXT("Fixture failure leaves the caller's candidate untouched"),
		UnchangedCandidate.TargetScenarioId.Value,
		ValidContext.TargetScenarioId.Value);

	return true;
}
