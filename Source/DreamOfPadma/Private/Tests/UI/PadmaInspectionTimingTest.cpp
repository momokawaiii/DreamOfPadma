#include "Misc/AutomationTest.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"

namespace
{
	FPadmaCombatSetup InspectionFixture(EPadmaCombatMode Mode = EPadmaCombatMode::Encounter)
	{
		FPadmaCombatSetup Setup;
		Setup.Mode = Mode;
		Setup.EnemyTurnDelay = 1.f;
		FPadmaCombatUnitSpec Player;
		Player.Id = TEXT("inspection-player");
		Player.DefinitionId = TEXT("vitruvian");
		Player.DisplayName = FText::FromString(TEXT("Inspection player"));
		Player.Health = Player.MaxHealth = 100;
		Player.Attack = 4;
		Player.Defense = 1;
		Player.AttackCost = 1;
		Player.Speed = 12;
		Player.Location = FVector(160, 245, 36);
		Setup.Units.Add(Player);
		FPadmaCombatUnitSpec Enemy = Player;
		Enemy.Id = TEXT("inspection-enemy");
		Enemy.DefinitionId = TEXT("guard");
		Enemy.DisplayName = FText::FromString(TEXT("Inspection enemy"));
		Enemy.bPlayer = false;
		Enemy.Speed = 20;
		Enemy.Location.X = 200;
		Setup.Units.Add(Enemy);
		return Setup;
	}

	// One timer tick per automation frame avoids changing GFrameCounter or waiting on wall time.
	class FPadmaInspectionTimingCommand final : public IAutomationLatentCommand
	{
	public:
		explicit FPadmaInspectionTimingCommand(FAutomationTestBase* InTest) : Test(InTest) {}
		virtual ~FPadmaInspectionTimingCommand() override
		{
			if (World)
			{
				World->DestroyWorld(false);
				GEngine->DestroyWorldContext(World);
			}
		}

		virtual bool Update() override
		{
			FString Failure;
			switch (Step++)
			{
			case 0:
				if (!CreateFixture()) return true;
				World->GetTimerManager().Tick(0.f);
				break;
			case 1:
				World->GetTimerManager().Tick(.25f);
				Battle->SetInspectionOpen(true);
				Battle->SetInspectionOpen(true);
				Test->TestTrue(TEXT("Repeated open keeps Encounter inspection active"), Battle->IsInspectionOpen());
				Test->TestEqual(TEXT("Inspection does not change world dilation"), World->GetWorldSettings()->TimeDilation, 1.f);
				break;
			case 2:
				World->GetTimerManager().Tick(2.f);
				Test->TestEqual(TEXT("Enemy does not act while inspecting"), EnemyReceipts, 0);
				Test->TestEqual(TEXT("Inspection preserves the pending action owner"), Battle->GetActionOwner(), FName(TEXT("inspection-enemy")));
				Battle->SetInspectionOpen(false);
				Battle->SetInspectionOpen(false);
				Test->TestFalse(TEXT("Repeated close is idempotent"), Battle->IsInspectionOpen());
				break;
			case 3:
				World->GetTimerManager().Tick(.5f);
				Test->TestEqual(TEXT("Closing inspection does not execute before remaining delay"), EnemyReceipts, 0);
				break;
			case 4:
				World->GetTimerManager().Tick(.26f);
				Test->TestEqual(TEXT("Enemy resumes remaining delay rather than restarting it"), EnemyReceipts, 1);
				Test->TestEqual(TEXT("Resumed action hands off to player"), Battle->GetActionOwner(), FName(TEXT("inspection-player")));
				// Open inspection inside the already committed action's payment callback.
				bInspectDuringPayment = true;
				Test->TestTrue(TEXT("An in-flight player action completes while inspection opens"), Battle->Attack({TEXT("inspection-enemy")}, Failure));
				Test->TestTrue(TEXT("Payment callback opened inspection"), Battle->IsInspectionOpen());
				if (auto* Enemy = Battle->GetUnit(TEXT("inspection-enemy")))
					Test->TestTrue(TEXT("Inspection does not cancel committed damage"), Enemy->Health() < 100.f);
				else Test->AddError(TEXT("Committed-action fixture lost its enemy"));
				break;
			case 5:
				World->GetTimerManager().Tick(2.f);
				Test->TestEqual(TEXT("A newly scheduled enemy turn also waits during inspection"), EnemyReceipts, 1);
				Battle->SetInspectionOpen(false);
				break;
			case 6:
				World->GetTimerManager().Tick(.5f);
				Test->TestEqual(TEXT("New paused turn retains its full unspent delay"), EnemyReceipts, 1);
				break;
			case 7:
				World->GetTimerManager().Tick(.51f);
				Test->TestEqual(TEXT("New paused turn resumes once"), EnemyReceipts, 2);
				Battle->SetInspectionOpen(true);
				Battle->ExitBattle();
				Battle->ExitBattle();
				Battle->SetInspectionOpen(false);
				Test->TestFalse(TEXT("Exit clears inspection state"), Battle->IsInspectionOpen());
				Test->TestFalse(TEXT("Exit ends the battle"), Battle->IsBattleActive());
				Test->TestEqual(TEXT("Exit notification is not duplicated"), Finishes, 1);
				break;
			case 8:
				World->GetTimerManager().Tick(2.f);
				Test->TestEqual(TEXT("Exiting clears the paused enemy callback"), EnemyReceipts, 2);
				Battle->SetInspectionOpen(true);
				Test->TestFalse(TEXT("Inactive battle cannot retain inspection state"), Battle->IsInspectionOpen());
				if (!Test->TestTrue(TEXT("Battle can restart after an inspected exit"), Battle->StartBattle(InspectionFixture(), Failure)))
				{
					Test->AddError(Failure);
					return true;
				}
				Test->TestFalse(TEXT("New battle starts without a stale inspection gate"), Battle->IsInspectionOpen());
				break;
			case 9:
				World->GetTimerManager().Tick(0.f);
				break;
			case 10:
				World->GetTimerManager().Tick(1.01f);
				Test->TestEqual(TEXT("Restarted battle enemy timer is active"), EnemyReceipts, 3);
				Battle->ExitBattle();
				World->GetWorldSettings()->SetTimeDilation(.8f);
				if (!Test->TestTrue(TEXT("ACT fixture starts"), Battle->StartBattle(InspectionFixture(EPadmaCombatMode::ACT), Failure)))
				{
					Test->AddError(Failure);
					return true;
				}
				Battle->SetInspectionOpen(true);
				Test->TestFalse(TEXT("Encounter inspection gate does not apply to ACT"), Battle->IsInspectionOpen());
				Test->TestEqual(TEXT("Inspection leaves normal ACT dilation unchanged"), World->GetWorldSettings()->TimeDilation, .8f);
				Test->TestTrue(TEXT("ACT skill library remains independently operable"), Battle->SetSkillLibraryOpen(true));
				Battle->SetInspectionOpen(true);
				Battle->SetInspectionOpen(false);
				Test->TestTrue(TEXT("Inspection leaves ACT Tab open"), Battle->IsSkillLibraryOpen());
				Test->TestTrue(TEXT("Inspection preserves ACT bullet time"), FMath::IsNearlyEqual(World->GetWorldSettings()->TimeDilation, .08f));
				Battle->ExitBattle();
				Test->TestEqual(TEXT("ACT exit still restores the original dilation"), World->GetWorldSettings()->TimeDilation, .8f);
				return true;
			default:
				return true;
			}
			return false;
		}

	private:
		bool CreateFixture()
		{
			UWorld::InitializationValues Values;
			Values.AllowAudioPlayback(false).CreatePhysicsScene(false).ShouldSimulatePhysics(false)
				.EnableTraceCollision(false).CreateNavigation(false).CreateAISystem(false);
			World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
			if (!Test->TestNotNull(TEXT("Inspection timing world"), World)) return false;
			GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
			auto* Host = World->SpawnActor<AActor>();
			if (!Test->TestNotNull(TEXT("Inspection timing host"), Host)) return false;
			Battle = NewObject<UPadmaCombatComponent>(Host);
			Host->AddInstanceComponent(Battle);
			Battle->RegisterComponent();
			Battle->PayCost = [this](float, float, FName, FString&)
			{
				if (bInspectDuringPayment) { bInspectDuringPayment = false; Battle->SetInspectionOpen(true); }
				return true;
			};
			Battle->OnFinished = [this](bool, const TArray<FPadmaCombatUnitSnapshot>&) { ++Finishes; };
			Battle->OnReceipt.AddLambda([this](const FPadmaCombatReceipt& Receipt)
			{
				if (Receipt.SourceId == TEXT("inspection-enemy") && Receipt.TargetId == TEXT("inspection-player")) ++EnemyReceipts;
			});
			FString Failure;
			if (!Test->TestTrue(TEXT("Encounter inspection fixture starts"), Battle->StartBattle(InspectionFixture(), Failure)))
			{
				Test->AddError(Failure);
				return false;
			}
			return true;
		}

		FAutomationTestBase* Test;
		UWorld* World = nullptr;
		UPadmaCombatComponent* Battle = nullptr;
		int32 Step = 0;
		int32 EnemyReceipts = 0;
		int32 Finishes = 0;
		bool bInspectDuringPayment = false;
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaInspectionTimingTest, "DreamOfPadma.UI.TASK050.InspectionTiming",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPadmaInspectionTimingTest::RunTest(const FString& Parameters)
{
	ADD_LATENT_AUTOMATION_COMMAND(FPadmaInspectionTimingCommand(this));
	return true;
}
