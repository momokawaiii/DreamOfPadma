#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Demo/Session/DemoTransitionTypes.h"
#include "DemoTransitionSessionSubsystem.generated.h"

/** Map-free atomic store used by the session boundary and its narrow tests. */
class DREAMOFPADMA_API FDemoTransitionContextStore
{
public:
	bool PublishTransition(const FDemoTransitionContext& InContext, FText& OutFailure);
	bool PeekTransition(FDemoTransitionContext& OutContext, FText& OutFailure) const;
	bool ConsumeTransition(FDemoTransitionContext& OutContext, FText& OutFailure);
	bool HasPendingTransition() const
	{
		return bHasPendingTransition;
	}
	void ClearTransition();

private:
	FDemoTransitionContext PendingTransition;
	bool bHasPendingTransition = false;
};

/**
 * Session-lifetime handoff for a typed demo transition. The GameInstance
 * subsystem survives a world change without becoming a general gameplay
 * manager.
 */
UCLASS()
class DREAMOFPADMA_API UDemoTransitionSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Demo Transition")
	bool PublishTransition(const FDemoTransitionContext& InContext, FText& OutFailure);

	UFUNCTION(BlueprintCallable, Category = "Demo Transition")
	bool PeekTransition(FDemoTransitionContext& OutContext, FText& OutFailure) const;

	UFUNCTION(BlueprintCallable, Category = "Demo Transition")
	bool ConsumeTransition(FDemoTransitionContext& OutContext, FText& OutFailure);

	UFUNCTION(BlueprintPure, Category = "Demo Transition")
	bool HasPendingTransition() const
	{
		return ContextStore.HasPendingTransition();
	}

	UFUNCTION(BlueprintCallable, Category = "Demo Transition")
	void ClearTransition();

private:
	FDemoTransitionContextStore ContextStore;
};
