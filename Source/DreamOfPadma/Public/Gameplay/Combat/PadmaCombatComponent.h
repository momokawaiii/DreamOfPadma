#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimerManager.h"
#include "Gameplay/Combat/PadmaCombatTypes.h"
#include "PadmaCombatComponent.generated.h"

class APadmaCombatUnit;

DECLARE_MULTICAST_DELEGATE_OneParam(FPadmaCombatReceiptEvent, const FPadmaCombatReceipt&);
DECLARE_MULTICAST_DELEGATE(FPadmaCombatChangedEvent);

/** Session-owned battle executor. Durable transactions remain in Core through callbacks. */
UCLASS(ClassGroup=(Padma), meta=(BlueprintSpawnableComponent))
class DREAMOFPADMA_API UPadmaCombatComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UPadmaCombatComponent();
	bool StartBattle(const FPadmaCombatSetup& InSetup, FString& Failure);
	bool Attack(const TArray<FName>& Targets, FString& Failure);
	bool Guard(FString& Failure);
	bool UseSkill(FName SkillId, FName TargetId, FString& Failure);
	bool SetSkillLibraryOpen(bool bOpen);
	/** Encounter only: retain the remaining enemy delay while details are open. Committed actions continue. */
	void SetInspectionOpen(bool bOpen);
	bool IsInspectionOpen() const { return bInspectionOpen; }
	bool SetMoveInput(FVector2D Input);
	bool MoveTo(FVector Destination);
	bool SetAttackHeld(bool bHeld);
	void ExitBattle();
	bool IsBattleActive() const { return bActive; }
	bool IsSkillLibraryOpen() const { return bSkillLibraryOpen; }
	bool IsCardUsed() const { return bCardUsed; }
	EPadmaCombatMode GetMode() const { return Setup.Mode; }
	FName GetActionOwner() const { return ActionOwner; }
	float GetActionTime() const { return ActionTime; }
	int32 GetActionCount() const { return ActionCount; }
	float GetElapsed() const { return Elapsed; }
	const TArray<TObjectPtr<APadmaCombatUnit>>& GetUnits() const { return Units; }
	APadmaCombatUnit* GetUnit(FName Id) const;
	APadmaCombatUnit* GetPlayerUnit() const;
	TArray<FPadmaCombatUnitSnapshot> GetSnapshots() const;
	TArray<FName> GetPredictedQueue(int32 Count = 8) const;
	/** Required. Must validate resources/availability and atomically pay/discard, or leave Core unchanged. */
	TFunction<bool(float, float, FName, FString&)> PayCost;
	TFunction<void(float, float)> ModifyFaith;
	TFunction<void(bool, const TArray<FPadmaCombatUnitSnapshot>&)> OnFinished;
	FPadmaCombatReceiptEvent OnReceipt;
	FPadmaCombatChangedEvent OnChanged;
	/** Native GAS entry, never a UI settlement path. */
	bool CanActivateMode(const APadmaCombatUnit* Source, EPadmaCombatMode RequiredMode) const;
	void ResolvePendingAction(APadmaCombatUnit* Source);
protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
private:
	enum class EAction : uint8 { None, Attack, Guard, Skill };
	UPROPERTY(Transient) FPadmaCombatSetup Setup;
	UPROPERTY(Transient) TArray<TObjectPtr<APadmaCombatUnit>> Units;
	UPROPERTY(Transient) TObjectPtr<APadmaCombatUnit> CardSource;
	EAction PendingAction = EAction::None;
	TWeakObjectPtr<APadmaCombatUnit> PendingSource;
	TArray<FName> PendingTargets;
	FName PendingSkill;
	float PendingFlow = 0;
	float PendingCalc = 0;
	FString PendingFailure;
	bool bPendingResolved = false;
	bool bActive = false;
	bool bSkillLibraryOpen = false;
	bool bInspectionOpen = false;
	bool bEnemyActionDeferred = false;
	bool bCardUsed = false;
	bool bAttackHeld = false;
	bool bMoveTo = false;
	bool bFinishing = false;
	FVector MoveDestination = FVector::ZeroVector;
	FVector2D MoveInput = FVector2D::ZeroVector;
	FName ActionOwner;
	float ActionTime = 0;
	int32 ActionCount = 0;
	float Elapsed = 0;
	float OriginalTimeDilation = 1;
	FTimerHandle EnemyTurnTimer;
	bool ActivateAction(APadmaCombatUnit* Source, EAction Action, const TArray<FName>& Targets, FName Skill, float Flow, float Calc, FString& Failure);
	void NextActor();
	void EndAction(APadmaCombatUnit* Unit);
	void EnemyAction();
	void TickACT(float DeltaTime);
	void ApplyEncounterSkill(const FPadmaEncounterSkillEffectRow& Skill, APadmaCombatUnit* Target);
	void ApplyACTSkill(const FPadmaACTSkillEffectRow& Skill, APadmaCombatUnit* Target);
	void ApplySkillEffect(const FPadmaSkillEffectRowBase& Skill, APadmaCombatUnit* Target, bool bEncounter);
	float Damage(APadmaCombatUnit* Source, APadmaCombatUnit* Target, float Amount, bool bTrue, bool bBasic, int32 Wave = 0);
	void Grant(APadmaCombatUnit* Source, APadmaCombatUnit* Target, FName Kind, float Amount, int32 Wave = 0);
	void Emit(APadmaCombatUnit* Source, APadmaCombatUnit* Target, FName Effect, const FPadmaCombatUnitSnapshot& Before, float Absorbed = 0, bool bBlocked = false, bool bTrue = false, int32 Wave = 0);
	void CleanupDead();
	bool CheckEnd();
	void Finish(bool bWon);
	void Cleanup();
};
