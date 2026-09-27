#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Gameplay/Combat/PadmaCombatTypes.h"
#include "PadmaACTTrainingAnalytics.generated.h"

class UPadmaCombatComponent;

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaACTTrainingTargetProfile
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training Target")
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training Target")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training Target")
	FVector LocationOffset = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training Target", meta = (ClampMin = "1"))
	float MaxHealth = 10000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training Target", meta = (ClampMin = "0"))
	float Defense = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training Target", meta = (ClampMin = "0"))
	float MagicDefense = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training Target")
	FName Attribute;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training Target")
	FLinearColor Accent = FLinearColor(0.1f, 0.65f, 0.75f, 1.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training Target")
	bool bExecutionImmune = true;
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaACTTrainingDamageSample
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) double TimeSeconds = 0;
	UPROPERTY(BlueprintReadOnly) FName SourceId;
	UPROPERTY(BlueprintReadOnly) FName TargetId;
	UPROPERTY(BlueprintReadOnly) FName ActionId;
	UPROPERTY(BlueprintReadOnly) EPadmaACTDamageKind DamageKind = EPadmaACTDamageKind::Physical;
	UPROPERTY(BlueprintReadOnly) float Amount = 0;
	UPROPERTY(BlueprintReadOnly) float Absorbed = 0;
	UPROPERTY(BlueprintReadOnly) float HealthBefore = 0;
	UPROPERTY(BlueprintReadOnly) float HealthAfter = 0;
	UPROPERTY(BlueprintReadOnly) bool bBlocked = false;
	UPROPERTY(BlueprintReadOnly) bool bTrueDamage = false;
	UPROPERTY(BlueprintReadOnly) int32 Wave = 0;
};

DECLARE_MULTICAST_DELEGATE(FPadmaACTTrainingAnalyticsChangedEvent);

/** Read-only ACT training recorder. It observes combat events and never settles gameplay. */
UCLASS(ClassGroup = (Padma), meta = (BlueprintSpawnableComponent))
class DREAMOFPADMA_API UPadmaACTTrainingAnalyticsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPadmaACTTrainingAnalyticsComponent();
	void BindBattle(UPadmaCombatComponent* InBattle);
	void Reset();
	float GetTotalDamage(FName TargetId) const;
	float GetDPS(FName TargetId, float WindowSeconds = 10.f) const;
	int32 GetHitCount(FName TargetId) const;
	float GetLastDamage(FName TargetId) const;
	TArray<FPadmaACTTrainingDamageSample> GetRecentDamage(FName TargetId = NAME_None, int32 MaxCount = 8) const;
	TArray<FPadmaGameplayEffectRecord> GetRecentGameplayEffects(int32 MaxCount = 12) const;
	int32 GetRevision() const { return Revision; }
	const TArray<FPadmaACTTrainingDamageSample>& GetDamageSamples() const { return DamageSamples; }
	const TArray<FPadmaGameplayEffectRecord>& GetGameplayEffectRecords() const { return GameplayEffects; }

	FPadmaACTTrainingAnalyticsChangedEvent OnChanged;

protected:
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;

private:
	void HandleReceipt(const FPadmaCombatReceipt& Receipt);
	void HandleGameplayEffect(const FPadmaGameplayEffectRecord& Record);
	void UnbindBattle();

	UPROPERTY(Transient)
	TObjectPtr<UPadmaCombatComponent> Battle;
	TArray<FPadmaACTTrainingDamageSample> DamageSamples;
	TArray<FPadmaGameplayEffectRecord> GameplayEffects;
	FDelegateHandle ReceiptHandle;
	FDelegateHandle GameplayEffectHandle;
	int32 Revision = 0;
	UPROPERTY(EditAnywhere, Category = "Training", meta = (ClampMin = "64", ClampMax = "8192"))
	int32 MaxRecordedEvents = 2048;
};
