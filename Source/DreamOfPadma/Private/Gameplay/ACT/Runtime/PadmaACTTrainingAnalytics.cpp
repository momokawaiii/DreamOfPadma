#include "Gameplay/ACT/Runtime/PadmaACTTrainingAnalytics.h"

#include "Gameplay/Combat/PadmaCombatComponent.h"

UPadmaACTTrainingAnalyticsComponent::UPadmaACTTrainingAnalyticsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPadmaACTTrainingAnalyticsComponent::BindBattle(UPadmaCombatComponent* InBattle)
{
	if (Battle == InBattle) return;
	UnbindBattle();
	Battle = InBattle;
	if (!Battle) return;
	ReceiptHandle = Battle->OnReceipt.AddUObject(this, &UPadmaACTTrainingAnalyticsComponent::HandleReceipt);
	GameplayEffectHandle = Battle->OnGameplayEffect.AddUObject(this, &UPadmaACTTrainingAnalyticsComponent::HandleGameplayEffect);
}

void UPadmaACTTrainingAnalyticsComponent::UnbindBattle()
{
	if (Battle)
	{
		if (ReceiptHandle.IsValid()) Battle->OnReceipt.Remove(ReceiptHandle);
		if (GameplayEffectHandle.IsValid()) Battle->OnGameplayEffect.Remove(GameplayEffectHandle);
	}
	ReceiptHandle.Reset();
	GameplayEffectHandle.Reset();
}

void UPadmaACTTrainingAnalyticsComponent::Reset()
{
	DamageSamples.Reset();
	GameplayEffects.Reset();
	++Revision;
	OnChanged.Broadcast();
}

void UPadmaACTTrainingAnalyticsComponent::HandleReceipt(const FPadmaCombatReceipt& Receipt)
{
	if (Receipt.Effect != TEXT("damage") || Receipt.TargetId.IsNone()) return;
	FPadmaACTTrainingDamageSample Sample;
	Sample.TimeSeconds = Battle ? Battle->GetElapsed() : 0;
	Sample.SourceId = Receipt.SourceId;
	Sample.TargetId = Receipt.TargetId;
	Sample.ActionId = Receipt.ActionId;
	Sample.DamageKind = Receipt.DamageKind;
	Sample.Amount = Receipt.Amount;
	Sample.Absorbed = Receipt.Absorbed;
	Sample.HealthBefore = Receipt.Before.Health;
	Sample.HealthAfter = Receipt.After.Health;
	Sample.bBlocked = Receipt.bBlocked;
	Sample.bTrueDamage = Receipt.bTrueDamage;
	Sample.Wave = Receipt.Wave;
	DamageSamples.Add(Sample);
	if (DamageSamples.Num() > MaxRecordedEvents) DamageSamples.RemoveAt(0, DamageSamples.Num() - MaxRecordedEvents);
	++Revision;
	OnChanged.Broadcast();
}

void UPadmaACTTrainingAnalyticsComponent::HandleGameplayEffect(const FPadmaGameplayEffectRecord& Record)
{
	if (Record.GameplayEffectId == TEXT("GE.Padma.Attribute.Init")) return;
	GameplayEffects.Add(Record);
	if (GameplayEffects.Num() > MaxRecordedEvents) GameplayEffects.RemoveAt(0, GameplayEffects.Num() - MaxRecordedEvents);
	++Revision;
	OnChanged.Broadcast();
}

float UPadmaACTTrainingAnalyticsComponent::GetTotalDamage(FName TargetId) const
{
	float Total = 0;
	for (const auto& Sample : DamageSamples) if (TargetId.IsNone() || Sample.TargetId == TargetId) Total += Sample.Amount;
	return Total;
}

float UPadmaACTTrainingAnalyticsComponent::GetDPS(FName TargetId, float WindowSeconds) const
{
	if (WindowSeconds <= 0) return 0;
	if (!Battle) return 0;
	const double Now = Battle->GetElapsed();
	double First = MAX_dbl;
	for (const auto& Sample : DamageSamples)
		if ((TargetId.IsNone() || Sample.TargetId == TargetId) && Sample.Amount > 0)
		{
			First = FMath::Min(First, Sample.TimeSeconds);
		}
	if (First == MAX_dbl) return 0;
	const double Cutoff = Now - WindowSeconds;
	float Total = 0;
	for (const auto& Sample : DamageSamples)
		if ((TargetId.IsNone() || Sample.TargetId == TargetId) && Sample.Amount > 0 && Sample.TimeSeconds > Cutoff && Sample.TimeSeconds <= Now) Total += Sample.Amount;
	// Warm up from the first hit (minimum one second), then use a rolling
	// combat-time window. Idle time must expire damage even without new events.
	const double Duration = FMath::Clamp(Now - First, FMath::Min(1.0, static_cast<double>(WindowSeconds)), static_cast<double>(WindowSeconds));
	return Total / static_cast<float>(Duration);
}

int32 UPadmaACTTrainingAnalyticsComponent::GetHitCount(FName TargetId) const
{
	int32 Count = 0;
	for (const auto& Sample : DamageSamples) if (TargetId.IsNone() || Sample.TargetId == TargetId) ++Count;
	return Count;
}

float UPadmaACTTrainingAnalyticsComponent::GetLastDamage(FName TargetId) const
{
	for (int32 Index = DamageSamples.Num() - 1; Index >= 0; --Index)
		if (TargetId.IsNone() || DamageSamples[Index].TargetId == TargetId) return DamageSamples[Index].Amount;
	return 0;
}

TArray<FPadmaACTTrainingDamageSample> UPadmaACTTrainingAnalyticsComponent::GetRecentDamage(FName TargetId, int32 MaxCount) const
{
	TArray<FPadmaACTTrainingDamageSample> Result;
	for (int32 Index = DamageSamples.Num() - 1; Index >= 0 && Result.Num() < FMath::Max(0, MaxCount); --Index)
		if (TargetId.IsNone() || DamageSamples[Index].TargetId == TargetId) Result.Add(DamageSamples[Index]);
	return Result;
}

TArray<FPadmaGameplayEffectRecord> UPadmaACTTrainingAnalyticsComponent::GetRecentGameplayEffects(int32 MaxCount) const
{
	TArray<FPadmaGameplayEffectRecord> Result;
	for (int32 Index = GameplayEffects.Num() - 1; Index >= 0 && Result.Num() < FMath::Max(0, MaxCount); --Index) Result.Add(GameplayEffects[Index]);
	return Result;
}

void UPadmaACTTrainingAnalyticsComponent::EndPlay(const EEndPlayReason::Type Reason)
{
	UnbindBattle();
	Super::EndPlay(Reason);
}
