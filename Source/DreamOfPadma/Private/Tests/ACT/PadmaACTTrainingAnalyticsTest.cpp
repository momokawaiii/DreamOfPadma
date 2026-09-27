#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Gameplay/ACT/Runtime/PadmaACTTrainingAnalytics.h"
#include "Gameplay/ACT/Runtime/PadmaACTMeleeLab.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaACTTrainingAnalyticsTest, "DreamOfPadma.ACT.ChenTrainingAnalytics",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPadmaACTTrainingAnalyticsTest::RunTest(const FString& Parameters)
{
	UWorld::InitializationValues Values;
	Values.AllowAudioPlayback(false).CreatePhysicsScene(false).ShouldSimulatePhysics(false)
		.EnableTraceCollision(false).CreateNavigation(false).CreateAISystem(false);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
	if (!TestNotNull(TEXT("Training analytics test world"), World)) return false;
	GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
	ON_SCOPE_EXIT { World->DestroyWorld(false); GEngine->DestroyWorldContext(World); };

	AActor* Host = World->SpawnActor<AActor>();
	UPadmaCombatComponent* Battle = NewObject<UPadmaCombatComponent>(Host);
	Host->AddInstanceComponent(Battle);
	Battle->RegisterComponent();
	UPadmaACTTrainingAnalyticsComponent* Analytics = NewObject<UPadmaACTTrainingAnalyticsComponent>(Host);
	Host->AddInstanceComponent(Analytics);
	Analytics->RegisterComponent();
	Analytics->BindBattle(Battle);
	Battle->PayCost = [](float, float, FName, FString&) { return true; };

	FPadmaCombatSetup Setup;
	Setup.Mode = EPadmaCombatMode::ACT;
	Setup.ArenaMinimum = FVector2D(-1000.f, -1000.f);
	Setup.ArenaMaximum = FVector2D(1000.f, 1000.f);

	FPadmaCombatUnitSpec Player;
	Player.Id = TEXT("chen-research");
	Player.DisplayName = FText::FromString(TEXT("Chen Qianyu"));
	Player.bPlayer = true;
	Player.Health = Player.MaxHealth = 1000.f;
	Player.Attack = 20.f;
	Player.AttackRange = 500.f;
	Player.Speed = 160.f;
	Player.Location = FVector(0.f, 0.f, 90.f);
	Setup.Units.Add(Player);

	const TArray<FName> TargetIds = { TEXT("blade-dummy"), TEXT("blade-dummy-2"), TEXT("blade-dummy-3") };
	const TArray<float> Defenses = { 30.f, 3.f, 10.f };
	const TArray<float> Health = { 1500.f, 30000.f, 30000.f };
	for (int32 Index = 0; Index < TargetIds.Num(); ++Index)
	{
		FPadmaCombatUnitSpec Target = Player;
		Target.Id = TargetIds[Index];
		Target.DisplayName = FText::FromName(TargetIds[Index]);
		Target.bPlayer = false;
		Target.Attack = 0.f;
		Target.Health = Target.MaxHealth = Health[Index];
		Target.Defense = Defenses[Index];
		Target.MagicDefense = Index == 2 ? 80.f : 10.f;
		Target.bCanPursueInACT = false;
		Target.bExecutionImmune = true;
		Target.Location = FVector(220.f + Index * 100.f, 0.f, 90.f);
		Setup.Units.Add(Target);
	}

	FString Failure;
	if (!TestTrue(TEXT("Training fixture starts"), Battle->StartBattle(Setup, Failure)))
	{
		AddError(Failure);
		return false;
	}
	Analytics->Reset();

	TestTrue(TEXT("First target attack settles"), Battle->Attack({ TargetIds[0] }, Failure));
	// This isolated recorder test has no Play world tick loop. Clear the
	// authoritative cooldown between fixtures so the second sample exercises
	// the same receipt/GE path without depending on wall-clock scheduling.
	if (APadmaCombatUnit* Source = Battle->GetPlayerUnit()) Source->AttackCooldown = 0.f;
	TestTrue(TEXT("Second target attack settles"), Battle->Attack({ TargetIds[1] }, Failure));
	TestEqual(TEXT("Damage receipt count is per target hit"), Analytics->GetHitCount(NAME_None), 2);
	TestTrue(TEXT("High-defense target still records minimum damage"), Analytics->GetTotalDamage(TargetIds[0]) >= 1.f);
	TestTrue(TEXT("Low-defense target records mitigated damage"), Analytics->GetTotalDamage(TargetIds[1]) > Analytics->GetTotalDamage(TargetIds[0]));
	TestTrue(TEXT("DPS recorder exposes a non-zero window"), Analytics->GetDPS(TargetIds[1]) > 0.f);

	bool bFoundHealthGE = false;
	for (const auto& Record : Analytics->GetGameplayEffectRecords())
		if (Record.GameplayEffectId == TEXT("GE.Padma.Damage.Health") && Record.TargetId == TargetIds[1]) bFoundHealthGE = true;
	TestTrue(TEXT("Native health GE is recorded with target identity"), bFoundHealthGE);
	const float InitialDPS = Analytics->GetDPS(TargetIds[1]);
	for (int32 Frame = 0; Frame < 60; ++Frame) static_cast<UActorComponent*>(Battle)->TickComponent(.05f, LEVELTICK_All, nullptr);
	TestTrue(TEXT("DPS decays without a new receipt"), Analytics->GetDPS(TargetIds[1]) < InitialDPS);
	for (int32 Frame = 0; Frame < 160; ++Frame) static_cast<UActorComponent*>(Battle)->TickComponent(.05f, LEVELTICK_All, nullptr);
	TestEqual(TEXT("Ten-second window expires while idle"), Analytics->GetDPS(TargetIds[1]), 0.f);
	TestEqual(TEXT("Idle does not erase session hits"), Analytics->GetHitCount(NAME_None), 2);
	TestEqual(TEXT("Zero-length DPS window is safe"), Analytics->GetDPS(TargetIds[1], 0.f), 0.f);

	Battle->ExitBattle();

	// Exercise the actual F8 command owner, not just Analytics->Reset in isolation.
	APadmaACTMeleeLab* Lab = World->SpawnActor<APadmaACTMeleeLab>();
	Lab->Analytics->BindBattle(Lab->Battle);
	Lab->ResetBattle();
	Lab->Battle->GetPlayerUnit()->Spec.AttackRange = 500.f;
	TestTrue(TEXT("Lab damage uses real receipt and GE path"), Lab->Battle->Attack({ TEXT("blade-dummy") }, Failure));
	TestTrue(TEXT("Lab recorded its hit"), Lab->Analytics->GetHitCount(NAME_None) > 0);
	Lab->Tick(.05f);
	TestTrue(TEXT("Fallback camera tick cannot clear recordings"), Lab->Analytics->GetHitCount(NAME_None) > 0);
	Lab->ResetBattle();
	TestEqual(TEXT("F8 reset clears all damage"), Lab->Analytics->GetHitCount(NAME_None), 0);
	TestEqual(TEXT("F8 reset clears all GE events"), Lab->Analytics->GetGameplayEffectRecords().Num(), 0);
	TestEqual(TEXT("F8 reset clears totals"), Lab->Analytics->GetTotalDamage(NAME_None), 0.f);
	TestEqual(TEXT("F8 reset clears DPS"), Lab->Analytics->GetDPS(NAME_None), 0.f);
	Lab->Battle->ExitBattle();
	return true;
}
