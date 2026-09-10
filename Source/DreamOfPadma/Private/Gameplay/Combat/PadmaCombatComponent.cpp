#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Gameplay/Combat/PadmaCombatAttributes.h"
#include "Gameplay/ACT/Authoring/PadmaACTAuthoring.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Animation/AnimMontage.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "TimerManager.h"

namespace
{
	bool Fail(FString& Failure, const TCHAR* Message) { Failure = Message; return false; }
	bool FiniteNonNegative(float Value) { return FMath::IsFinite(Value) && Value >= 0; }
	bool ValidSkill(const FPadmaSkillEffectRowBase& Skill)
	{
		return (Skill.EffectId == TEXT("cut") || Skill.EffectId == TEXT("wind") || Skill.EffectId == TEXT("mantra") || Skill.EffectId == TEXT("light") || Skill.EffectId == TEXT("rain"))
			&& (Skill.Target == TEXT("ally") || Skill.Target == TEXT("enemy") || Skill.Target == TEXT("all"))
			&& FMath::IsFinite(Skill.Magnitude) && Skill.Magnitude >= 0 && Skill.RepeatCount > 0 && Skill.RepeatCount <= 32
			&& FMath::IsFinite(Skill.FlowCost) && Skill.FlowCost >= 0 && FMath::IsFinite(Skill.CalculationCost) && Skill.CalculationCost >= 0;
	}
}

UPadmaCombatComponent::UPadmaCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UPadmaCombatComponent::StartBattle(const FPadmaCombatSetup& InSetup, FString& Failure)
{
	if (bActive || bFinishing) return Fail(Failure, TEXT("Battle already active."));
	if (!GetWorld() || !PayCost) return Fail(Failure, TEXT("Battle requires a world and an atomic Core payment binding."));
	if (InSetup.Mode != EPadmaCombatMode::Encounter && InSetup.Mode != EPadmaCombatMode::ACT) return Fail(Failure, TEXT("Unsupported battle mode."));
	int32 Players = 0, Enemies = 0;
	TSet<FName> Ids;
	for (const auto& Unit : InSetup.Units)
	{
		if (Unit.Id.IsNone() || Ids.Contains(Unit.Id) || !FiniteNonNegative(Unit.Health) || !FiniteNonNegative(Unit.MaxHealth)
			|| Unit.Health > Unit.MaxHealth || Unit.MaxHealth <= 0 || !FiniteNonNegative(Unit.Attack) || !FiniteNonNegative(Unit.Defense)
			|| !FiniteNonNegative(Unit.AttackCost) || !FMath::IsFinite(Unit.Speed) || Unit.Speed <= 0 || Unit.MaxTargets < 1 || Unit.Location.ContainsNaN()
			|| !FMath::IsFinite(Unit.AttackInterval) || Unit.AttackInterval <= 0 || !FiniteNonNegative(Unit.AttackRange) || !FiniteNonNegative(Unit.InitialAttackDelay))
			return Fail(Failure, TEXT("Invalid or duplicate battle unit definition."));
		Ids.Add(Unit.Id);
		if (Unit.Health > 0) { if (Unit.bPlayer) ++Players; else ++Enemies; }
		if (InSetup.Mode == EPadmaCombatMode::ACT && Unit.bPlayer && !Unit.Presentation.ACTDefinition.IsNull())
		{
			auto* Definition = Unit.Presentation.ACTDefinition.LoadSynchronous();
			auto* Table = Definition ? Definition->SkillTable.LoadSynchronous() : nullptr;
			if (!Definition || !Table || Table->GetRowStruct() != FPadmaACTSkillRow::StaticStruct())
				return Fail(Failure, TEXT("ACT presentation binding has no valid ACT skill table."));
			TSet<FName> BoundSkills;
			TSet<FName> BoundSlots;
			for (const auto& Pair : Table->GetRowMap())
			{
				const auto* Row = reinterpret_cast<const FPadmaACTSkillRow*>(Pair.Value);
				const auto* Effect = InSetup.ACTSkills.Find(Row->SkillId);
				auto* SkillDefinition = Row->Definition.LoadSynchronous();
				if (!Effect || !SkillDefinition || SkillDefinition->DefinitionId != Row->SkillId || SkillDefinition->AbilityImplementationId != FName(*(TEXT("Demo.ACT.") + Effect->EffectId.ToString()))
					|| Row->ActivationBindingId.IsNone() || BoundSkills.Contains(Row->SkillId) || BoundSlots.Contains(Row->ActivationBindingId))
					return Fail(Failure, TEXT("ACT ability binding is missing or not registered in the native Demo executor."));
				BoundSkills.Add(Row->SkillId);
				BoundSlots.Add(Row->ActivationBindingId);
			}
			for (const auto& Pair : InSetup.ACTSkills) if (!BoundSkills.Contains(Pair.Key)) return Fail(Failure, TEXT("ACT character table omits a configured basic skill binding."));
		}
	}
	if (!Players || !Enemies || (InSetup.Mode == EPadmaCombatMode::ACT && Players != 1)) return Fail(Failure, TEXT("Battle requires both sides; ACT requires exactly one player."));
	if (!FMath::IsFinite(InSetup.EnemyTurnDelay) || InSetup.EnemyTurnDelay <= 0 || !FMath::IsFinite(InSetup.EnemyAttackInterval) || InSetup.EnemyAttackInterval <= 0
		|| InSetup.ArenaOrigin.ContainsNaN() || InSetup.ArenaMinimum.ContainsNaN() || InSetup.ArenaMaximum.ContainsNaN()
		|| InSetup.ArenaMinimum.X >= InSetup.ArenaMaximum.X || InSetup.ArenaMinimum.Y >= InSetup.ArenaMaximum.Y)
		return Fail(Failure, TEXT("Invalid battle clock or arena bounds."));
	if (InSetup.Mode == EPadmaCombatMode::Encounter)
	{
		for (const auto& Pair : InSetup.EncounterSkills) if (Pair.Key != Pair.Value.Id || !ValidSkill(Pair.Value)) return Fail(Failure, TEXT("Invalid Encounter effect binding."));
	}
	else for (const auto& Pair : InSetup.ACTSkills) if (Pair.Key != Pair.Value.Id || !ValidSkill(Pair.Value)) return Fail(Failure, TEXT("Invalid ACT effect binding."));
	Setup = InSetup;
	ActionOwner = NAME_None;
	ActionTime = 0;
	ActionCount = 0;
	Elapsed = 0;
	bCardUsed = false;
	bAttackHeld = false;
	bMoveTo = false;
	bSkillLibraryOpen = false;
	bInspectionOpen = false;
	bEnemyActionDeferred = false;
	MoveInput = FVector2D::ZeroVector;
	OriginalTimeDilation = GetWorld()->GetWorldSettings()->TimeDilation;
	FActorSpawnParameters Params;
	Params.ObjectFlags |= RF_Transient;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for (const auto& Spec : Setup.Units)
	{
		auto* Unit = GetWorld()->SpawnActor<APadmaCombatUnit>(APadmaCombatUnit::StaticClass(), Spec.Location, FRotator::ZeroRotator, Params);
		if (!Unit) { Cleanup(); return Fail(Failure, TEXT("Failed to spawn battle unit.")); }
		Units.Add(Unit);
		Unit->InitializeCombat(this, Spec, Setup.Mode);
	}
	CardSource = GetWorld()->SpawnActor<APadmaCombatUnit>(APadmaCombatUnit::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, Params);
	if (!CardSource) { Cleanup(); return Fail(Failure, TEXT("Failed to create transient card ASC.")); }
	FPadmaCombatUnitSpec SourceSpec;
	SourceSpec.DisplayName = FText::FromString(TEXT("Card / environment"));
	SourceSpec.Family = TEXT("Card");
	CardSource->InitializeCombat(this, SourceSpec, Setup.Mode, true);
	bActive = true;
	CleanupDead();
	if (Setup.Mode == EPadmaCombatMode::Encounter) NextActor();
	else ActionOwner = GetPlayerUnit()->Spec.Id;
	Failure.Reset();
	OnChanged.Broadcast();
	return true;
}

APadmaCombatUnit* UPadmaCombatComponent::GetUnit(FName Id) const
{
	for (APadmaCombatUnit* Unit : Units) if (IsValid(Unit) && Unit->Spec.Id == Id) return Unit;
	return nullptr;
}

APadmaCombatUnit* UPadmaCombatComponent::GetPlayerUnit() const
{
	for (APadmaCombatUnit* Unit : Units) if (IsValid(Unit) && Unit->Spec.bPlayer && Unit->IsAlive()) return Unit;
	return nullptr;
}

TArray<FPadmaCombatUnitSnapshot> UPadmaCombatComponent::GetSnapshots() const
{
	TArray<FPadmaCombatUnitSnapshot> Result;
	for (APadmaCombatUnit* Unit : Units) if (IsValid(Unit)) Result.Add(Unit->Snapshot());
	return Result;
}

TArray<FName> UPadmaCombatComponent::GetPredictedQueue(int32 Count) const
{
	TArray<FName> Result;
	if (!bActive || Setup.Mode != EPadmaCombatMode::Encounter) return Result;
	TArray<APadmaCombatUnit*> Alive;
	TMap<FName, float> Ready;
	for (APadmaCombatUnit* Unit : Units) if (Unit->IsAlive()) { Alive.Add(Unit); Ready.Add(Unit->Spec.Id, Unit->ReadyTime); }
	for (int32 Index = 0; Index < FMath::Clamp(Count, 0, 32) && !Alive.IsEmpty(); ++Index)
	{
		Alive.Sort([&Ready](const APadmaCombatUnit& A, const APadmaCombatUnit& B) {
			return Ready[A.Spec.Id] == Ready[B.Spec.Id] ? A.Spec.Id.LexicalLess(B.Spec.Id) : Ready[A.Spec.Id] < Ready[B.Spec.Id]; });
		auto* Unit = Alive[0];
		Result.Add(Unit->Spec.Id);
		Ready[Unit->Spec.Id] += 100.f / Unit->Spec.Speed;
	}
	return Result;
}

bool UPadmaCombatComponent::Attack(const TArray<FName>& Targets, FString& Failure)
{
	if (!bActive) return Fail(Failure, TEXT("No active battle."));
	auto* Source = Setup.Mode == EPadmaCombatMode::Encounter ? GetUnit(ActionOwner) : GetPlayerUnit();
	if (!Source || !Source->IsAlive() || !Source->Spec.bPlayer) return Fail(Failure, TEXT("Wait for a living player action owner."));
	if (Setup.Mode == EPadmaCombatMode::ACT && (bSkillLibraryOpen || Source->AttackCooldown > 0)) return Fail(Failure, TEXT("ACT attack input is unavailable."));
	const int32 Limit = Setup.Mode == EPadmaCombatMode::Encounter ? Source->Spec.MaxTargets : 1;
	if (Targets.IsEmpty() || Targets.Num() > Limit) return Fail(Failure, TEXT("Choose the permitted number of distinct enemies."));
	TSet<FName> Unique;
	for (FName Id : Targets)
	{
		auto* Target = GetUnit(Id);
		if (!Target || !Target->IsAlive() || Target->Spec.bPlayer || Unique.Contains(Id)) return Fail(Failure, TEXT("Illegal or duplicate attack target."));
		if (Setup.Mode == EPadmaCombatMode::ACT && FVector::Dist2D(Source->GetActorLocation(), Target->GetActorLocation()) > Source->Spec.AttackRange)
			return Fail(Failure, TEXT("Target outside ACT attack range."));
		Unique.Add(Id);
	}
	return ActivateAction(Source, EAction::Attack, Targets, NAME_None, Setup.Mode == EPadmaCombatMode::Encounter ? Source->Spec.AttackCost : 0, 0, Failure);
}

bool UPadmaCombatComponent::Guard(FString& Failure)
{
	auto* Source = GetUnit(ActionOwner);
	if (!bActive || Setup.Mode != EPadmaCombatMode::Encounter || !Source || !Source->Spec.bPlayer || !Source->IsAlive()) return Fail(Failure, TEXT("Guard requires an active Encounter player."));
	return ActivateAction(Source, EAction::Guard, {}, NAME_None, 0, 0, Failure);
}

bool UPadmaCombatComponent::UseSkill(FName SkillId, FName TargetId, FString& Failure)
{
	if (!bActive) return Fail(Failure, TEXT("No active battle."));
	auto* Owner = Setup.Mode == EPadmaCombatMode::Encounter ? GetUnit(ActionOwner) : GetPlayerUnit();
	if (!Owner || !Owner->IsAlive() || !Owner->Spec.bPlayer) return Fail(Failure, TEXT("A living player action owner is required."));
	const FPadmaSkillEffectRowBase* Skill = nullptr;
	if (Setup.Mode == EPadmaCombatMode::Encounter)
	{
		if (bCardUsed) return Fail(Failure, TEXT("One optional card has already been used this action."));
		Skill = Setup.EncounterSkills.Find(SkillId);
	}
	else
	{
		if (!bSkillLibraryOpen) return Fail(Failure, TEXT("Open the ACT skill library with Tab first."));
		Skill = Setup.ACTSkills.Find(SkillId);
	}
	if (!Skill) return Fail(Failure, TEXT("Missing skill binding for this battle mode."));
	auto* Target = GetUnit(TargetId);
	if (Setup.Mode == EPadmaCombatMode::ACT && TargetId.IsNone())
	{
		if (Skill->Target == TEXT("ally")) Target = Owner;
		else for (APadmaCombatUnit* Unit : Units) if (!Unit->Spec.bPlayer && Unit->IsAlive()) { Target = Unit; break; }
	}
	if (Skill->Target != TEXT("all") && (!Target || !Target->IsAlive() || Target->Spec.bCore || Target->Spec.bPlayer != (Skill->Target == TEXT("ally"))))
		return Fail(Failure, TEXT("Skill target side, identity or state is illegal."));
	TArray<FName> Targets;
	if (Target) Targets.Add(Target->Spec.Id);
	return ActivateAction(CardSource, EAction::Skill, Targets, SkillId, Skill->FlowCost, Skill->CalculationCost, Failure);
}

bool UPadmaCombatComponent::CanActivateMode(const APadmaCombatUnit* Source, EPadmaCombatMode RequiredMode) const
{
	if (!bActive || bFinishing || Setup.Mode != RequiredMode || PendingAction == EAction::None || PendingSource.Get() != Source || !Source || !Source->IsAlive()) return false;
	if (RequiredMode == EPadmaCombatMode::Encounter)
	{
		const auto* Owner = GetUnit(ActionOwner);
		return Owner && Owner->IsAlive() && (Source->IsCardSource() ? Owner->Spec.bPlayer && !bCardUsed && PendingAction == EAction::Skill : Source == Owner && PendingAction != EAction::Skill);
	}
	if (Source->IsCardSource()) return bSkillLibraryOpen && GetPlayerUnit() && PendingAction == EAction::Skill;
	return PendingAction == EAction::Attack && (!Source->Spec.bPlayer || (!bSkillLibraryOpen && Source->AttackCooldown <= 0));
}

bool UPadmaCombatComponent::ActivateAction(APadmaCombatUnit* Source, EAction Action, const TArray<FName>& Targets, FName Skill, float Flow, float Calc, FString& Failure)
{
	if (PendingAction != EAction::None || !Source) return Fail(Failure, TEXT("Another authoritative action is resolving."));
	PendingSource = Source;
	PendingAction = Action;
	PendingTargets = Targets;
	PendingSkill = Skill;
	PendingFlow = Flow;
	PendingCalc = Calc;
	PendingFailure.Reset();
	bPendingResolved = false;
	Source->GetAbilitySystemComponent()->TryActivateAbility(Source->GetModeAbility());
	const bool bResolved = bPendingResolved;
	Failure = PendingFailure;
	PendingAction = EAction::None;
	PendingSource.Reset();
	PendingTargets.Reset();
	PendingSkill = NAME_None;
	if (!bResolved)
	{
		if (Failure.IsEmpty()) Failure = TEXT("Native mode ability rejected activation.");
		return false;
	}
	// Cleanup/finish happens after EndAbility returned: never destroy a currently executing ASC.
	CleanupDead();
	if (!CheckEnd() && Setup.Mode == EPadmaCombatMode::Encounter)
	{
		if (Action != EAction::Skill || !GetUnit(ActionOwner) || !GetUnit(ActionOwner)->IsAlive()) NextActor();
	}
	OnChanged.Broadcast();
	return true;
}

void UPadmaCombatComponent::ResolvePendingAction(APadmaCombatUnit* Source)
{
	if (bPendingResolved || !CanActivateMode(Source, Setup.Mode)) return;
	// All target/window/binding validation and GAS CommitAbility precede the one Core mutation.
	if ((PendingFlow > 0 || PendingCalc > 0 || !PendingSkill.IsNone())
		&& (!PayCost || !PayCost(PendingFlow, PendingCalc, PendingSkill, PendingFailure))) return;
	bPendingResolved = true;
	if (PendingAction == EAction::Guard)
	{
		Grant(Source, Source, TEXT("shield"), 2);
		EndAction(Source);
	}
	else if (PendingAction == EAction::Attack)
	{
		Source->PlayAttackPresentation();
		for (FName Id : PendingTargets)
		{
			const float Dealt = Damage(Source, GetUnit(Id), Source->Spec.Attack, false, false);
			if (Setup.Mode == EPadmaCombatMode::Encounter && Source->Spec.DefinitionId == TEXT("door") && Dealt > 0 && ModifyFaith) ModifyFaith(-1, -1);
		}
		if (Setup.Mode == EPadmaCombatMode::Encounter) EndAction(Source);
		else Source->AttackCooldown = Source->Spec.bPlayer ? Source->Spec.AttackInterval : Setup.EnemyAttackInterval;
	}
	else if (PendingAction == EAction::Skill)
	{
		auto* Target = PendingTargets.IsEmpty() ? nullptr : GetUnit(PendingTargets[0]);
		if (Setup.Mode == EPadmaCombatMode::Encounter) ApplyEncounterSkill(Setup.EncounterSkills.FindChecked(PendingSkill), Target);
		else ApplyACTSkill(Setup.ACTSkills.FindChecked(PendingSkill), Target);
		bCardUsed = true;
	}
}

void UPadmaCombatComponent::NextActor()
{
	GetWorld()->GetTimerManager().ClearTimer(EnemyTurnTimer);
	bEnemyActionDeferred = false;
	const auto Queue = GetPredictedQueue(1);
	if (Queue.IsEmpty()) return;
	ActionOwner = Queue[0];
	auto* Unit = GetUnit(ActionOwner);
	ActionTime = Unit->ReadyTime;
	bCardUsed = false;
	Unit->bActed = false;
	++ActionCount;
	if (!Unit->Spec.bPlayer)
	{
		GetWorld()->GetTimerManager().SetTimer(EnemyTurnTimer, this, &UPadmaCombatComponent::EnemyAction, Setup.EnemyTurnDelay, false);
		if (bInspectionOpen) GetWorld()->GetTimerManager().PauseTimer(EnemyTurnTimer);
	}
}

void UPadmaCombatComponent::EndAction(APadmaCombatUnit* Unit)
{
	Unit->bActed = true;
	Unit->ReadyTime += 100.f / Unit->Spec.Speed;
}

void UPadmaCombatComponent::EnemyAction()
{
	if (!bActive || Setup.Mode != EPadmaCombatMode::Encounter) return;
	// A dispatched timer may already be entering its callback when inspection opens.
	if (bInspectionOpen) { bEnemyActionDeferred = true; return; }
	bEnemyActionDeferred = false;
	auto* Unit = GetUnit(ActionOwner);
	auto* Target = GetPlayerUnit();
	if (!Unit || Unit->Spec.bPlayer || !Unit->IsAlive() || !Target) { CheckEnd(); return; }
	FString Failure;
	ActivateAction(Unit, EAction::Attack, {Target->Spec.Id}, NAME_None, 0, 0, Failure);
}

void UPadmaCombatComponent::ApplyEncounterSkill(const FPadmaEncounterSkillEffectRow& Skill, APadmaCombatUnit* Target)
{
	ApplySkillEffect(Skill, Target, true);
}

void UPadmaCombatComponent::ApplyACTSkill(const FPadmaACTSkillEffectRow& Skill, APadmaCombatUnit* Target)
{
	// ACT definitions own their effect values and never inherit the Encounter unacted/faith rule.
	ApplySkillEffect(Skill, Target, false);
	if (auto* Player = GetPlayerUnit())
	{
		if (auto* Definition = Player->Spec.Presentation.ACTDefinition.LoadSynchronous())
			if (auto* Table = Definition->SkillTable.LoadSynchronous())
				for (const auto& Pair : Table->GetRowMap())
				{
					const auto* Row = reinterpret_cast<const FPadmaACTSkillRow*>(Pair.Value);
					if (Row->SkillId == Skill.Id)
						if (auto* SkillDefinition = Row->Definition.LoadSynchronous())
							if (auto* Montage = SkillDefinition->Montage.LoadSynchronous()) Player->PlayAnimMontage(Montage);
				}
	}
}

void UPadmaCombatComponent::ApplySkillEffect(const FPadmaSkillEffectRowBase& Skill, APadmaCombatUnit* Target, bool bEncounter)
{
	// Cards/environment own a distinct ASC in both modes. No C action owner is the caster.
	if (Skill.EffectId == TEXT("rain"))
	{
		for (int32 Wave = 0; Wave < Skill.RepeatCount; ++Wave)
			for (APadmaCombatUnit* Unit : Units) if (!Unit->Spec.bCore) Damage(CardSource, Unit, Skill.Magnitude, Skill.bTrueDamage, true, Wave);
	}
	else if (Skill.EffectId == TEXT("cut")) Damage(CardSource, Target, Skill.Magnitude, Skill.bTrueDamage, true);
	else if (Skill.EffectId == TEXT("wind")) Grant(CardSource, Target, TEXT("shield"), Skill.Magnitude);
	else if (Skill.EffectId == TEXT("mantra")) Grant(CardSource, Target, TEXT("block"), Skill.Magnitude);
	else if (Skill.EffectId == TEXT("light"))
	{
		Grant(CardSource, Target, TEXT("heal"), Skill.Magnitude);
		if (bEncounter && Skill.bFaithIfUnacted && Target && !Target->bActed && ModifyFaith) ModifyFaith(1, 0);
	}
}

float UPadmaCombatComponent::Damage(APadmaCombatUnit* Source, APadmaCombatUnit* Target, float Amount, bool bTrue, bool bBasic, int32 Wave)
{
	if (!Source || !Target || !Target->IsAlive()) return 0;
	float Value = Amount;
	if (!bTrue)
	{
		float Factor = 1;
		if (bBasic && Target->Spec.Family == TEXT("C")) Factor = .5f;
		else if (!Source->Spec.Attribute.IsNone() && !Target->Spec.Attribute.IsNone() && Setup.Era != 6)
		{
			const auto Beats = [](FName A, FName B) { return (A == TEXT("D") && B == TEXT("F")) || (A == TEXT("F") && B == TEXT("R")) || (A == TEXT("R") && B == TEXT("D")); };
			if (Beats(Source->Spec.Attribute, Target->Spec.Attribute)) Factor = 2;
			else if (Beats(Target->Spec.Attribute, Source->Spec.Attribute)) Factor = .5f;
		}
		Value = Amount * Factor - Target->Spec.Defense;
		if (Value <= 0)
		{
			Value = 1;
			if (!bBasic && !Source->IsCardSource() && Source->IsAlive())
			{
				const auto Before = Source->Snapshot();
				Source->ApplyAttribute(Source, UPadmaCombatAttributes::GetHealthAttribute(), -1);
				Emit(Source, Source, TEXT("recoil"), Before, 0, false, false, Wave);
			}
		}
	}
	const auto Before = Target->Snapshot();
	const bool bBlocked = Before.Block > 0;
	if (bBlocked) { Target->ApplyAttribute(Source, UPadmaCombatAttributes::GetBlockAttribute(), -1); Value = 0; }
	const float Absorbed = FMath::Min(Before.Shield, Value);
	if (Absorbed > 0) Target->ApplyAttribute(Source, UPadmaCombatAttributes::GetShieldAttribute(), -Absorbed);
	Value -= Absorbed;
	if (Value > 0) Target->ApplyAttribute(Source, UPadmaCombatAttributes::GetHealthAttribute(), -Value);
	Emit(Source, Target, TEXT("damage"), Before, Absorbed, bBlocked, bTrue, Wave);
	return Value;
}

void UPadmaCombatComponent::Grant(APadmaCombatUnit* Source, APadmaCombatUnit* Target, FName Kind, float Amount, int32 Wave)
{
	if (!Target || !Target->IsAlive()) return;
	const auto Before = Target->Snapshot();
	const FGameplayAttribute Attribute = Kind == TEXT("shield") ? UPadmaCombatAttributes::GetShieldAttribute() : Kind == TEXT("block") ? UPadmaCombatAttributes::GetBlockAttribute() : UPadmaCombatAttributes::GetHealthAttribute();
	Target->ApplyAttribute(Source, Attribute, Amount);
	Emit(Source, Target, Kind, Before, 0, false, false, Wave);
}

void UPadmaCombatComponent::Emit(APadmaCombatUnit* Source, APadmaCombatUnit* Target, FName Effect, const FPadmaCombatUnitSnapshot& Before, float Absorbed, bool bBlocked, bool bTrue, int32 Wave)
{
	FPadmaCombatReceipt Receipt;
	Receipt.Mode = Setup.Mode;
	Receipt.SourceKind = Source->IsCardSource() ? (PendingSkill == TEXT("rain") ? EPadmaCombatSource::Environment : EPadmaCombatSource::Card) : EPadmaCombatSource::Unit;
	Receipt.SourceId = Source->IsCardSource() ? PendingSkill : Source->Spec.Id;
	Receipt.TargetId = Target->Spec.Id;
	Receipt.ActionOwnerId = Setup.Mode == EPadmaCombatMode::ACT && !Source->IsCardSource() ? Source->Spec.Id : ActionOwner;
	Receipt.Effect = Effect;
	Receipt.Before = Before;
	Receipt.After = Target->Snapshot();
	Receipt.Amount = Effect == TEXT("shield") ? Receipt.After.Shield - Before.Shield : Effect == TEXT("block") ? Receipt.After.Block - Before.Block : FMath::Abs(Receipt.After.Health - Before.Health);
	Receipt.Absorbed = Absorbed;
	Receipt.bBlocked = bBlocked;
	Receipt.bTrueDamage = bTrue;
	Receipt.Wave = Wave;
	OnReceipt.Broadcast(Receipt);
}

void UPadmaCombatComponent::SetInspectionOpen(bool bOpen)
{
	if (!bActive || Setup.Mode != EPadmaCombatMode::Encounter || bInspectionOpen == bOpen) return;
	bInspectionOpen = bOpen;
	auto& Timers = GetWorld()->GetTimerManager();
	if (bOpen) Timers.PauseTimer(EnemyTurnTimer);
	else if (bEnemyActionDeferred)
	{
		// This callback's delay already expired; do not restart the full enemy delay.
		bEnemyActionDeferred = false;
		EnemyTurnTimer = Timers.SetTimerForNextTick(this, &UPadmaCombatComponent::EnemyAction);
	}
	else Timers.UnPauseTimer(EnemyTurnTimer);
}

bool UPadmaCombatComponent::SetSkillLibraryOpen(bool bOpen)
{
	if (!bActive || Setup.Mode != EPadmaCombatMode::ACT || PendingAction != EAction::None) return false;
	bSkillLibraryOpen = bOpen;
	bAttackHeld = false;
	GetWorld()->GetWorldSettings()->SetTimeDilation(bOpen ? OriginalTimeDilation * .1f : OriginalTimeDilation);
	OnChanged.Broadcast();
	return true;
}

bool UPadmaCombatComponent::SetMoveInput(FVector2D Input)
{
	if (!bActive || Setup.Mode != EPadmaCombatMode::ACT || bSkillLibraryOpen || Input.ContainsNaN()) return false;
	MoveInput = Input.GetClampedToMaxSize(1);
	if (!MoveInput.IsNearlyZero()) bMoveTo = false;
	return true;
}

bool UPadmaCombatComponent::MoveTo(FVector Destination)
{
	if (!bActive || Setup.Mode != EPadmaCombatMode::ACT || bSkillLibraryOpen || Destination.ContainsNaN()) return false;
	bMoveTo = true;
	MoveDestination = Destination;
	MoveInput = FVector2D::ZeroVector;
	return true;
}

bool UPadmaCombatComponent::SetAttackHeld(bool bHeld)
{
	if (!bActive || Setup.Mode != EPadmaCombatMode::ACT || bSkillLibraryOpen) return false;
	bAttackHeld = bHeld;
	return true;
}

void UPadmaCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bActive) return;
	if (Setup.Mode == EPadmaCombatMode::ACT)
	{
		// HTML clamps the unscaled frame before applying bullet time. World delta is already scaled.
		const float SimDelta = FMath::Clamp(DeltaTime, 0.f, bSkillLibraryOpen ? .005f : .05f);
		Elapsed += SimDelta;
		TickACT(SimDelta);
	}
}

void UPadmaCombatComponent::TickACT(float DeltaTime)
{
	auto* Player = GetPlayerUnit();
	if (!Player) { CheckEnd(); return; }
	if (!bSkillLibraryOpen)
	{
		FVector2D Direction = MoveInput;
		if (bMoveTo)
		{
			const FVector Difference = MoveDestination - Player->GetActorLocation();
			if (Difference.Size2D() <= Player->Spec.Speed * DeltaTime) { bMoveTo = false; Direction = FVector2D::ZeroVector; }
			else Direction = FVector2D(Difference.X, Difference.Y).GetSafeNormal();
		}
		Player->CombatVelocity = FVector(Direction.X, Direction.Y, 0) * Player->Spec.Speed;
	}
	// Tab rejects new movement while retaining existing velocity; world time already supplies the .1 factor.
	FVector Location = Player->GetActorLocation() + Player->CombatVelocity * DeltaTime;
	Location.X = FMath::Clamp(Location.X, Setup.ArenaOrigin.X + Setup.ArenaMinimum.X, Setup.ArenaOrigin.X + Setup.ArenaMaximum.X);
	Location.Y = FMath::Clamp(Location.Y, Setup.ArenaOrigin.Y + Setup.ArenaMinimum.Y, Setup.ArenaOrigin.Y + Setup.ArenaMaximum.Y);
	Player->SetActorLocation(Location, false);
	if (!Player->CombatVelocity.IsNearlyZero()) Player->SetActorRotation(Player->CombatVelocity.Rotation());
	Player->AttackCooldown = FMath::Max(0.f, Player->AttackCooldown - DeltaTime);
	if (!bSkillLibraryOpen && bAttackHeld && Player->AttackCooldown <= 0)
	{
		APadmaCombatUnit* Closest = nullptr;
		float Distance = Player->Spec.AttackRange;
		for (APadmaCombatUnit* Unit : Units) if (!Unit->Spec.bPlayer && Unit->IsAlive())
		{
			const float Candidate = FVector::Dist2D(Location, Unit->GetActorLocation());
			if (Candidate <= Distance) { Closest = Unit; Distance = Candidate; }
		}
		if (Closest) { FString Failure; Attack({Closest->Spec.Id}, Failure); }
		if (!bActive) return;
	}
	// Commands may end and destroy all units, therefore iterate a local pointer snapshot.
	const auto Enemies = Units;
	for (APadmaCombatUnit* Enemy : Enemies)
	{
		if (!bActive) return;
		if (Enemy->Spec.bPlayer || !Enemy->IsAlive()) continue;
		const FVector Difference = Player->GetActorLocation() - Enemy->GetActorLocation();
		const float Distance = Difference.Size2D();
		Enemy->AttackCooldown -= DeltaTime;
		if (Enemy->WindupRemaining > 0)
		{
			Enemy->WindupRemaining = FMath::Max(0.f, Enemy->WindupRemaining - DeltaTime);
			if (Enemy->WindupRemaining <= 0)
			{
				if (Distance < 68) { FString Failure; ActivateAction(Enemy, EAction::Attack, {Player->Spec.Id}, NAME_None, 0, 0, Failure); }
				if (!bActive) return;
				Enemy->AttackCooldown = Setup.EnemyAttackInterval;
			}
		}
		else if (Distance < 58 && Enemy->AttackCooldown <= 0)
		{
			Enemy->WindupRemaining = .55f;
			Enemy->PlayAttackPresentation();
			OnChanged.Broadcast();
		}
		else if (Distance > 48)
		{
			Enemy->CombatVelocity = Difference.GetSafeNormal2D() * 80;
			Enemy->SetActorLocation(Enemy->GetActorLocation() + Enemy->CombatVelocity * DeltaTime, false);
			Enemy->SetActorRotation(Difference.Rotation());
		}
	}
	CleanupDead();
	CheckEnd();
}

void UPadmaCombatComponent::CleanupDead()
{
	for (APadmaCombatUnit* Unit : Units) if (!Unit->IsAlive())
	{
		Unit->CleanupGAS();
		Unit->SetActorEnableCollision(false);
		// Keep a visible defeated unit for snapshot/details; no grants/timers remain on it.
	}
}

bool UPadmaCombatComponent::CheckEnd()
{
	if (!bActive) return true;
	bool bPlayer = false, bEnemy = false;
	for (APadmaCombatUnit* Unit : Units) if (Unit->IsAlive()) { if (Unit->Spec.bPlayer) bPlayer = true; else bEnemy = true; }
	if (!bPlayer) { Finish(false); return true; }
	if (!bEnemy) { Finish(true); return true; }
	return false;
}

void UPadmaCombatComponent::Finish(bool bWon)
{
	if (!bActive || bFinishing) return;
	bFinishing = true;
	const auto FinalUnits = GetSnapshots();
	bActive = false;
	Cleanup();
	if (OnFinished) OnFinished(bWon, FinalUnits);
	bFinishing = false;
	OnChanged.Broadcast();
}

void UPadmaCombatComponent::Cleanup()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(EnemyTurnTimer);
		if (bSkillLibraryOpen) GetWorld()->GetWorldSettings()->SetTimeDilation(OriginalTimeDilation);
	}
	bSkillLibraryOpen = false;
	bInspectionOpen = false;
	bEnemyActionDeferred = false;
	bAttackHeld = false;
	bMoveTo = false;
	MoveInput = FVector2D::ZeroVector;
	PendingAction = EAction::None;
	PendingSource.Reset();
	PendingTargets.Reset();
	for (APadmaCombatUnit* Unit : Units) if (IsValid(Unit)) { Unit->CleanupGAS(); Unit->Destroy(); }
	Units.Reset();
	if (IsValid(CardSource)) { CardSource->CleanupGAS(); CardSource->Destroy(); }
	CardSource = nullptr;
}

void UPadmaCombatComponent::ExitBattle() { if (PendingAction == EAction::None) Finish(false); }

void UPadmaCombatComponent::EndPlay(const EEndPlayReason::Type Reason)
{
	if (bActive) Finish(false);
	else Cleanup();
	Super::EndPlay(Reason);
}
