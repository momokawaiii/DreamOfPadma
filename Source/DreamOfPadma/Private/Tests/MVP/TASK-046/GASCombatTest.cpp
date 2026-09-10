#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Gameplay/Combat/PadmaCombatAttributes.h"
#include "Gameplay/Encounter/PadmaEncounterAbility.h"
#include "Gameplay/ACT/Runtime/PadmaACTAbility.h"
#include "AbilitySystemComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"

namespace
{
	FPadmaCombatSetup CombatFixture(EPadmaCombatMode Mode)
	{
		FPadmaCombatSetup Setup;
		Setup.Mode = Mode;
		FPadmaCombatUnitSpec Player;
		Player.Id = TEXT("player-1");
		Player.DefinitionId = TEXT("vitruvian");
		Player.DisplayName = FText::FromString(TEXT("Player"));
		Player.Health = Player.MaxHealth = 20;
		Player.Attack = 4;
		Player.Defense = 1;
		Player.Speed = 12;
		Player.AttackCost = 1;
		Player.Location = FVector(160, 245, 36);
		Setup.Units.Add(Player);
		FPadmaCombatUnitSpec Enemy = Player;
		Enemy.Id = TEXT("enemy-1");
		Enemy.DefinitionId = TEXT("guard");
		Enemy.DisplayName = FText::FromString(TEXT("Enemy"));
		Enemy.bPlayer = false;
		Enemy.Health = Enemy.MaxHealth = 100;
		Enemy.Speed = 8;
		Enemy.Location.X = 200;
		Setup.Units.Add(Enemy);
		const TArray<FName> Ids = {TEXT("cut"), TEXT("wind"), TEXT("mantra"), TEXT("light"), TEXT("rain")};
		for (FName Id : Ids)
		{
			FPadmaEncounterSkillEffectRow Encounter;
			Encounter.Id = Encounter.EffectId = Id;
			Encounter.Target = Id == TEXT("cut") ? TEXT("enemy") : Id == TEXT("rain") ? TEXT("all") : TEXT("ally");
			Encounter.FlowCost = 1;
			Encounter.Magnitude = Id == TEXT("cut") ? 3 : Id == TEXT("light") ? 5 : Id == TEXT("mantra") ? 1 : 2;
			Encounter.bTrueDamage = Id == TEXT("cut");
			Encounter.RepeatCount = Id == TEXT("rain") ? 3 : 1;
			Encounter.bFaithIfUnacted = Id == TEXT("light");
			Setup.EncounterSkills.Add(Id, Encounter);
			FPadmaACTSkillEffectRow ACT;
			static_cast<FPadmaSkillEffectRowBase&>(ACT) = static_cast<const FPadmaSkillEffectRowBase&>(Encounter);
			ACT.bFaithIfUnacted = false;
			if (Id == TEXT("cut")) ACT.Magnitude = 7;
			Setup.ACTSkills.Add(Id, ACT);
		}
		return Setup;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaGASCombatTest, "DreamOfPadma.MVP.TASK046.GASModeAndLifecycle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPadmaGASCombatTest::RunTest(const FString& Parameters)
{
	UWorld::InitializationValues Values;
	Values.AllowAudioPlayback(false).CreatePhysicsScene(false).ShouldSimulatePhysics(false)
		.EnableTraceCollision(false).CreateNavigation(false).CreateAISystem(false);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
	if (!TestNotNull(TEXT("GAS test world"), World)) return false;
	GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
	ON_SCOPE_EXIT { World->DestroyWorld(false); GEngine->DestroyWorldContext(World); };
	auto* Host = World->SpawnActor<AActor>();
	auto* Battle = NewObject<UPadmaCombatComponent>(Host);
	Host->AddInstanceComponent(Battle);
	Battle->RegisterComponent();
	int32 Payments = 0, Finishes = 0;
	bool bDenyPayment = false;
	Battle->PayCost = [&](float Flow, float Calc, FName SkillId, FString& Failure) {
		if (bDenyPayment) { Failure = TEXT("Fixture insufficient resources"); return false; }
		++Payments; return true;
	};
	Battle->OnFinished = [&](bool bWon, const TArray<FPadmaCombatUnitSnapshot>& Units) { ++Finishes; };
	TArray<FPadmaCombatReceipt> Receipts;
	Battle->OnReceipt.AddLambda([&](const FPadmaCombatReceipt& Receipt) { Receipts.Add(Receipt); });
	FString Failure;
	auto Setup = CombatFixture(EPadmaCombatMode::Encounter);
	if (!TestTrue(TEXT("Encounter native world entry"), Battle->StartBattle(Setup, Failure))) { AddError(Failure); return false; }
	auto* Player = Battle->GetUnit(TEXT("player-1"));
	auto* Enemy = Battle->GetUnit(TEXT("enemy-1"));
	TestEqual(TEXT("GAS initial health effect"), Player->Health(), 20.f);
	TestNotNull(TEXT("Encounter ability granted"), Player->GetAbilitySystemComponent()->FindAbilitySpecFromClass(UPadmaEncounterAbility::StaticClass()));
	TestNull(TEXT("ACT ability is not granted in Encounter"), Player->GetAbilitySystemComponent()->FindAbilitySpecFromClass(UPadmaACTAbility::StaticClass()));
	TestEqual(TEXT("Deterministic initial timeline"), Battle->GetActionOwner(), FName(TEXT("player-1")));
	TestFalse(TEXT("Own-side attack rejected"), Battle->Attack({TEXT("player-1")}, Failure));
	TestFalse(TEXT("Duplicate attack target rejected"), Battle->Attack({TEXT("enemy-1"), TEXT("enemy-1")}, Failure));
	TestFalse(TEXT("Encounter requires explicit card target"), Battle->UseSkill(TEXT("cut"), NAME_None, Failure));
	TestEqual(TEXT("Illegal commands never pay"), Payments, 0);
	bDenyPayment = true;
	TestFalse(TEXT("Core denial rejects GAS command"), Battle->UseSkill(TEXT("cut"), TEXT("enemy-1"), Failure));
	TestEqual(TEXT("Denied card does not change attributes"), Enemy->Health(), 100.f);
	TestFalse(TEXT("Denied card does not consume window"), Battle->IsCardUsed());
	bDenyPayment = false;
	TestTrue(TEXT("Encounter card executes through GAS"), Battle->UseSkill(TEXT("cut"), TEXT("enemy-1"), Failure));
	TestEqual(TEXT("Encounter table defines three true damage"), Enemy->Health(), 97.f);
	TestEqual(TEXT("Action owner is unchanged after optional card"), Battle->GetActionOwner(), FName(TEXT("player-1")));
	TestFalse(TEXT("Only one card in the action"), Battle->UseSkill(TEXT("wind"), TEXT("player-1"), Failure));
	if (TestEqual(TEXT("One receipt for cut"), Receipts.Num(), 1))
	{
		TestEqual(TEXT("Card source does not use C unit origin"), Receipts[0].SourceKind, EPadmaCombatSource::Card);
		TestEqual(TEXT("Card stable source ID"), Receipts[0].SourceId, FName(TEXT("cut")));
		TestEqual(TEXT("Action owner tracked separately"), Receipts[0].ActionOwnerId, FName(TEXT("player-1")));
	}
	TestTrue(TEXT("Guard after optional card"), Battle->Guard(Failure));
	TestEqual(TEXT("Shield is an ASC attribute changed by GE"), Player->GetAttributes()->GetShield(), 2.f);
	TestEqual(TEXT("Guard ends logical action"), Battle->GetActionOwner(), FName(TEXT("enemy-1")));
	Battle->ExitBattle();
	Battle->ExitBattle();
	TestEqual(TEXT("Exit is commit/rollback callback once"), Finishes, 1);
	TestEqual(TEXT("Unit grants cleaned"), Player->GetAbilitySystemComponent()->GetActivatableAbilities().Num(), 0);
	TestTrue(TEXT("Battle actor is destroyed"), Player->IsActorBeingDestroyed());
	TestTrue(TEXT("World session releases all battle projections"), Battle->GetUnits().IsEmpty());

	Setup = CombatFixture(EPadmaCombatMode::ACT);
	if (!TestTrue(TEXT("ACT native world entry"), Battle->StartBattle(Setup, Failure))) { AddError(Failure); return false; }
	Player = Battle->GetUnit(TEXT("player-1"));
	Enemy = Battle->GetUnit(TEXT("enemy-1"));
	TestNotNull(TEXT("ACT distinct ability granted"), Player->GetAbilitySystemComponent()->FindAbilitySpecFromClass(UPadmaACTAbility::StaticClass()));
	TestNull(TEXT("Encounter ability not granted in ACT"), Player->GetAbilitySystemComponent()->FindAbilitySpecFromClass(UPadmaEncounterAbility::StaticClass()));
	TestFalse(TEXT("ACT has no Encounter guard"), Battle->Guard(Failure));
	TestFalse(TEXT("ACT card requires library input policy"), Battle->UseSkill(TEXT("cut"), TEXT("enemy-1"), Failure));
	TestTrue(TEXT("ACT library opens"), Battle->SetSkillLibraryOpen(true));
	TestEqual(TEXT("Native world time dilation"), World->GetWorldSettings()->TimeDilation, .1f);
	TestFalse(TEXT("ACT rejects new movement in Tab"), Battle->SetMoveInput(FVector2D(1, 0)));
	TestFalse(TEXT("ACT rejects new attack in Tab"), Battle->SetAttackHeld(true));
	TestFalse(TEXT("Direct ACT attack also respects Tab"), Battle->Attack({TEXT("enemy-1")}, Failure));
	TestTrue(TEXT("ACT own typed card binding executes"), Battle->UseSkill(TEXT("cut"), NAME_None, Failure));
	TestEqual(TEXT("ACT table differs from Encounter"), Enemy->Health(), 93.f);
	TestTrue(TEXT("ACT supports another card without an Encounter action gate"), Battle->UseSkill(TEXT("mantra"), TEXT("player-1"), Failure));
	TestEqual(TEXT("Block GE mutates native attribute"), Player->GetAttributes()->GetBlock(), 1.f);
	TestTrue(TEXT("ACT rain friendly fire"), Battle->UseSkill(TEXT("rain"), NAME_None, Failure));
	TestEqual(TEXT("First rain wave consumes block"), Player->GetAttributes()->GetBlock(), 0.f);
	TestEqual(TEXT("Later rain waves still settle"), Player->Health(), 18.f);
	TestTrue(TEXT("Rain has environment origin"), Receipts.ContainsByPredicate([](const auto& R) { return R.SourceKind == EPadmaCombatSource::Environment && R.Wave == 2; }));
	Battle->ExitBattle();
	TestEqual(TEXT("Exit restores exact original time dilation"), World->GetWorldSettings()->TimeDilation, 1.f);
	TestFalse(TEXT("Exit resets library gate"), Battle->IsSkillLibraryOpen());
	TestFalse(TEXT("Post-exit input unavailable"), Battle->SetAttackHeld(true));

	// Friendly fire can kill this action's owner while another player remains eligible.
	Setup = CombatFixture(EPadmaCombatMode::Encounter);
	Setup.Units[0].Health = 1;
	auto Ally = Setup.Units[0];
	Ally.Id = TEXT("player-2");
	Ally.Health = 20;
	Ally.Speed = 11;
	Setup.Units.Add(Ally);
	if (!TestTrue(TEXT("Owner-death fixture entry"), Battle->StartBattle(Setup, Failure))) return false;
	Player = Battle->GetUnit(TEXT("player-1"));
	TestTrue(TEXT("Environmental card can defeat its action owner"), Battle->UseSkill(TEXT("rain"), NAME_None, Failure));
	TestTrue(TEXT("Other ally keeps battle active"), Battle->IsBattleActive());
	TestEqual(TEXT("Dead owner's ASC grants cleaned"), Player->GetAbilitySystemComponent()->GetActivatableAbilities().Num(), 0);
	TestEqual(TEXT("Timeline advances to remaining ally"), Battle->GetActionOwner(), FName(TEXT("player-2")));
	TestFalse(TEXT("New owner receives a fresh optional card window"), Battle->IsCardUsed());
	Battle->ExitBattle();
	return true;
}
