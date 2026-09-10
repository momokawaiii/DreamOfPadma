#include "Core/Content/PadmaContentTypes.h"
#include "Core/WorldMap/PadmaWorldMapDefinition.h"

namespace
{
bool Fail(FString& Error, const FString& Message)
{
	Error = Message;
	return false;
}

template<typename T>
bool Ordered(const TMap<FName, T>& Rows, const TArray<FName>& Order, const TCHAR* Domain, FString& Error)
{
	TSet<FName> Seen;
	for (FName Id : Order)
	{
		if (!Rows.Contains(Id) || Seen.Contains(Id))
		{
			return Fail(Error, FString(Domain) + TEXT(" order contains an unknown or duplicate ID: ") + Id.ToString());
		}
		Seen.Add(Id);
	}
	return Seen.Num() == Rows.Num() || Fail(Error, FString(Domain) + TEXT(" order must contain every row exactly once."));
}

template<typename T>
bool Skills(const TMap<FName, T>& Rows, bool bEncounter, FString& Error)
{
	const TSet<FName> Effects = {TEXT("cut"), TEXT("wind"), TEXT("mantra"), TEXT("light"), TEXT("rain")};
	for (const auto& Pair : Rows)
	{
		const auto& Row = Pair.Value;
		if (!Effects.Contains(Row.EffectId) || Row.Id.IsNone() || Row.DisplayName.IsEmpty()
			|| !FMath::IsFinite(Row.FlowCost) || Row.FlowCost < 0
			|| !FMath::IsFinite(Row.CalculationCost) || Row.CalculationCost < 0
			|| !FMath::IsFinite(Row.Magnitude) || Row.Magnitude < 0 || Row.RepeatCount < 1
			|| !(Row.Target == TEXT("enemy") || Row.Target == TEXT("ally") || Row.Target == TEXT("all"))
			|| (!bEncounter && Row.bFaithIfUnacted)
			|| (Row.bFaithIfUnacted && Row.EffectId != TEXT("light")))
		{
			return Fail(Error, TEXT("Invalid or unsupported configured mode skill: ") + Row.Id.ToString());
		}
	}
	return true;
}
}

double FPadmaContentSnapshot::P(FName Id) const
{
	return Profiles.FindChecked(Id).Value;
}

bool FPadmaContentSnapshot::Validate(FString& Error) const
{
	Error.Reset();
	if (!MapLayout.MapId.IsNone())
	{
		if (!FPadmaWorldMapGenerator::Validate(MapLayout, Error)) { return false; }
		FPadmaMapLayout Published = MapLayout;
		Published.Nodes.Reset();
		for (const auto& Pair : Nodes)
		{
			if (Pair.Key != Pair.Value.Id) { return Fail(Error, TEXT("Map node lookup key differs from its stable ID.")); }
			Published.Nodes.Add(Pair.Value);
		}
		Published.Edges = Edges;
		if (FPadmaWorldMapGenerator::Signature(Published) != FPadmaWorldMapGenerator::Signature(MapLayout))
		{
			return Fail(Error, TEXT("Published map rows differ from the frozen configured layout."));
		}
	}
	if (Cards.IsEmpty() || Nodes.IsEmpty() || EncounterSkills.IsEmpty() || ACTSkills.IsEmpty()
		|| ACTCharacters.IsEmpty() || ACTWeapons.IsEmpty() || HomeCharacters.IsEmpty()
		|| HomeOutfits.IsEmpty() || HomeWeapons.IsEmpty() || HomePoses.IsEmpty()
		|| FPSCharacters.IsEmpty() || FPSWeapons.IsEmpty() || Dialogues.IsEmpty())
	{
		return Fail(Error, TEXT("Playable catalog is an incomplete draft."));
	}
	if (!Ordered(Cards, CardOrder, TEXT("Cards"), Error)
		|| !Ordered(Nodes, NodeOrder, TEXT("Nodes"), Error)
		|| !Ordered(EncounterSkills, SkillOrder, TEXT("Encounter skills"), Error)
		|| !Ordered(ACTSkills, SkillOrder, TEXT("ACT skills"), Error)) { return false; }
	if (Eras.Num() != 7 || Phases.Num() != 3)
	{
		return Fail(Error, TEXT("This HTML slice requires seven eras and three phases."));
	}
	const TArray<FName> RequiredProfiles = {
		TEXT("seed"), TEXT("flowStart"), TEXT("flowCap"), TEXT("calcStart"), TEXT("calcCap"),
		TEXT("faith"), TEXT("entropy"), TEXT("dailyFraction"), TEXT("dailyDraw"),
		TEXT("synthesisFlow"), TEXT("synthesisCalc"), TEXT("temperature"),
		TEXT("moveA"), TEXT("moveB"), TEXT("movePerson"), TEXT("moveBuilding"), TEXT("deployC"),
		TEXT("coreHP"), TEXT("coreStrike"), TEXT("warGoal"), TEXT("captureWar"), TEXT("gateWar"),
		TEXT("fireIncome"), TEXT("forgeFlow"), TEXT("storyCalc"), TEXT("enemyHP"), TEXT("enemyAtk"),
		TEXT("enemyDef"), TEXT("bossHP"), TEXT("playerSpeed"), TEXT("enemySpeed"),
		TEXT("actHP"), TEXT("actAtk"), TEXT("actDef"), TEXT("actSpeed"), TEXT("actRange"),
		TEXT("actCooldown"), TEXT("enemyInterval")
	};
	for (FName Id : RequiredProfiles)
	{
		if (!Profiles.Contains(Id)) { return Fail(Error, TEXT("Missing required profile parameter: ") + Id.ToString()); }
	}
	for (const auto& Pair : Profiles)
	{
		const auto& R = Pair.Value;
		if (!FMath::IsFinite(R.Value) || !FMath::IsFinite(R.Minimum) || !FMath::IsFinite(R.Maximum)
			|| R.Minimum > R.Maximum || R.Value < R.Minimum || R.Value > R.Maximum)
		{
			return Fail(Error, TEXT("Profile value is nonfinite or outside its authored bounds: ") + Pair.Key.ToString());
		}
	}
	if (P(TEXT("flowStart")) > P(TEXT("flowCap")) || P(TEXT("calcStart")) > P(TEXT("calcCap"))
		|| P(TEXT("temperature")) <= 0 || P(TEXT("actCooldown")) <= 0 || P(TEXT("enemyInterval")) <= 0)
	{
		return Fail(Error, TEXT("Invalid resource cap or nonpositive runtime denominator."));
	}
	const double Seed = P(TEXT("seed"));
	const double DailyDraw = P(TEXT("dailyDraw"));
	if (Seed < 1 || Seed > 2147483647 || Seed != double(int64(Seed))
		|| DailyDraw < 0 || DailyDraw > 10 || DailyDraw != double(int64(DailyDraw)))
	{
		return Fail(Error, TEXT("Seed and dailyDraw must be integers in the accepted HTML bounds."));
	}
	TSet<FName> Terrains;
	for (const auto& Pair : Nodes)
	{
		const auto& R = Pair.Value;
		if (R.Id.IsNone() || R.DisplayName.IsEmpty() || R.Type.IsNone() || R.Terrain.IsNone()
			|| !FMath::IsFinite(R.Position.X) || !FMath::IsFinite(R.Position.Y) || R.EnemyCount < 0
			|| !(R.InitialOwner == TEXT("player") || R.InitialOwner == TEXT("neutral") || R.InitialOwner == TEXT("ruler")))
		{
			return Fail(Error, TEXT("Invalid static map node: ") + Pair.Key.ToString());
		}
		Terrains.Add(R.Terrain);
	}
	const FName HomeNode = MapLayout.MapId.IsNone() ? FName(TEXT("home")) : MapLayout.HomeNode;
	if (!Nodes.Contains(HomeNode)) { return Fail(Error, TEXT("Playable map requires its declared home node.")); }
	for (const auto& Pair : Cards)
	{
		const auto& R = Pair.Value;
		FText MobilityError;
		if (R.Id.IsNone() || R.DisplayName.IsEmpty() || !R.Mobility.Validate(MobilityError)
			|| (R.Mobility.Policy == EPadmaCardMobility::Conditional && R.Mobility.ConditionId != TEXT("Demo.ExplicitMobility"))
			|| !FMath::IsFinite(R.MinFaith) || R.MinFaith < 0 || R.MinFaith > 100
			|| (!R.ForbiddenTerrain.IsNone() && !Terrains.Contains(R.ForbiddenTerrain))
			|| !(R.Family == TEXT("A") || R.Family == TEXT("B") || R.Family == TEXT("C"))
			|| R.Cost < 0 || R.Stability < 0 || R.DailyLimit < 0 || R.AttackCost < 0 || R.MaxTargets < 1
			|| !FMath::IsFinite(R.Tendency.X) || !FMath::IsFinite(R.Tendency.Y)
			|| !FMath::IsFinite(R.Field.X) || !FMath::IsFinite(R.Field.Y)
			|| !FMath::IsFinite(R.Target.X) || !FMath::IsFinite(R.Target.Y)
			|| (R.Family == TEXT("C") && (R.Health <= 0 || !(R.Kind == TEXT("person") || R.Kind == TEXT("building")))))
		{
			return Fail(Error, TEXT("Invalid card/mobility: ") + Pair.Key.ToString() + TEXT(" ") + MobilityError.ToString());
		}
		for (const auto& Tag : R.Tags)
		{
			if (Tag.Key.IsNone() || Tag.Value < 0) { return Fail(Error, TEXT("Invalid card tag: ") + Pair.Key.ToString()); }
		}
	}
	if (!Skills(EncounterSkills, true, Error) || !Skills(ACTSkills, false, Error)) { return false; }
	TSet<FName> EdgeIds;
	TSet<FString> EdgePairs;
	for (const auto& R : Edges)
	{
		const FString A = R.From.ToString(), B = R.To.ToString();
		const FString Pair = MapLayout.bHexWilderness ? A + TEXT("->") + B : A < B ? A + TEXT("|") + B : B + TEXT("|") + A;
		if (R.Id.IsNone() || EdgeIds.Contains(R.Id) || EdgePairs.Contains(Pair)
			|| !Nodes.Contains(R.From) || !Nodes.Contains(R.To) || R.From == R.To)
		{
			return Fail(Error, TEXT("Invalid/duplicate edge or missing endpoint: ") + R.Id.ToString());
		}
		EdgeIds.Add(R.Id);
		EdgePairs.Add(Pair);
	}
	if (Edges.IsEmpty()) { return Fail(Error, TEXT("Playable map has no edges.")); }
	for (const auto& Pair : ACTCharacters)
	{
		const auto& R = Pair.Value;
		if (R.DisplayName.IsEmpty() || !FMath::IsFinite(R.TerrainSpeedMultiplier) || R.TerrainSpeedMultiplier <= 0
			|| (!R.SpeedBonusTerrain.IsNone() && !Terrains.Contains(R.SpeedBonusTerrain))
			|| (!R.TraitDisabledTerrain.IsNone() && !Terrains.Contains(R.TraitDisabledTerrain)))
		{
			return Fail(Error, TEXT("Invalid ACT character/terrain trait: ") + Pair.Key.ToString());
		}
	}
	for (const auto& Pair : ACTWeapons)
	{
		const auto& R = Pair.Value;
		if (R.DisplayName.IsEmpty() || !FMath::IsFinite(R.DamageMultiplier) || R.DamageMultiplier <= 0
			|| !FMath::IsFinite(R.RangeMultiplier) || R.RangeMultiplier <= 0
			|| !FMath::IsFinite(R.IntervalMultiplier) || R.IntervalMultiplier <= 0)
		{
			return Fail(Error, TEXT("Invalid ACT weapon multipliers: ") + Pair.Key.ToString());
		}
	}
	for (const auto& Pair : HomeWeapons)
	{
		if (Pair.Value.CompatiblePoseIds.IsEmpty()) { return Fail(Error, TEXT("Home weapon requires explicit compatible poses.")); }
		TSet<FName> Poses;
		for (FName Pose : Pair.Value.CompatiblePoseIds)
		{
			if (!HomePoses.Contains(Pose) || Poses.Contains(Pose)) { return Fail(Error, TEXT("Unknown/duplicate home weapon pose.")); }
			Poses.Add(Pose);
		}
	}
	TMap<FName, int32> Starts;
	for (const auto& Pair : Dialogues)
	{
		const auto& R = Pair.Value;
		auto HasLine = [this, &R](FName Line)
		{
			const auto* Found = Dialogues.Find(FName(*(R.DialogueId.ToString() + TEXT(".") + Line.ToString())));
			return Found && Found->DialogueId == R.DialogueId && Found->LineId == Line;
		};
		if (R.DialogueId.IsNone() || R.LineId.IsNone() || R.Text.IsEmpty() || R.Speaker.IsEmpty()
			|| R.Id != FName(*(R.DialogueId.ToString() + TEXT(".") + R.LineId.ToString()))
			|| (!R.Next.IsNone() && !HasLine(R.Next)))
		{
			return Fail(Error, TEXT("Invalid dialogue line or next reference: ") + Pair.Key.ToString());
		}
		Starts.FindOrAdd(R.DialogueId) += R.bStart ? 1 : 0;
		TSet<FName> ChoiceIds;
		for (const auto& Choice : R.Choices)
		{
			const int32 Actions = !Choice.Next.IsNone() + int32(Choice.bComplete) + int32(Choice.bCancel);
			if (Choice.Id.IsNone() || ChoiceIds.Contains(Choice.Id) || Choice.Label.IsEmpty() || Actions != 1
				|| (!Choice.Next.IsNone() && !HasLine(Choice.Next)))
			{
				return Fail(Error, TEXT("Invalid dialogue choice or next reference: ") + Pair.Key.ToString());
			}
			ChoiceIds.Add(Choice.Id);
		}
	}
	for (const auto& Pair : Starts)
	{
		if (Pair.Value != 1) { return Fail(Error, TEXT("Dialogue requires exactly one start: ") + Pair.Key.ToString()); }
	}
	if (!MapLayout.MapId.IsNone())
	{
		for (const auto& Node : MapLayout.Nodes)
		{
			if (!Node.DialogueId.IsNone() && !Starts.Contains(Node.DialogueId))
			{
				return Fail(Error, TEXT("Map node references an unknown dialogue: ") + Node.Id.ToString());
			}
		}
		for (const auto& Checkpoint : MapLayout.Checkpoints)
		{
			if ((!Checkpoint.MatchedDialogueId.IsNone() && !Starts.Contains(Checkpoint.MatchedDialogueId))
				|| (!Checkpoint.MissedDialogueId.IsNone() && !Starts.Contains(Checkpoint.MissedDialogueId)))
			{
				return Fail(Error, TEXT("Map checkpoint references an unknown branch dialogue: ") + Checkpoint.Id.ToString());
			}
		}
	}
	return true;
}
