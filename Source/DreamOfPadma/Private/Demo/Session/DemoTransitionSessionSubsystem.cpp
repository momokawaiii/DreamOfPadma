#include "Demo/Session/DemoTransitionSessionSubsystem.h"

bool FDemoTransitionContextStore::PublishTransition(const FDemoTransitionContext& InContext, FText& OutFailure)
{
	FDemoTransitionContext ValidatedContext = InContext;
	if (!ValidatedContext.TryValidate(OutFailure))
	{
		return false;
	}

	// Commit only after every field is valid. An invalid replacement therefore
	// leaves an already-published context untouched.
	PendingTransition = ValidatedContext;
	bHasPendingTransition = true;
	OutFailure = FText::GetEmpty();
	return true;
}

bool FDemoTransitionContextStore::PeekTransition(FDemoTransitionContext& OutContext, FText& OutFailure) const
{
	if (!bHasPendingTransition)
	{
		OutFailure = FText::FromString(TEXT("No demo transition context is pending."));
		return false;
	}

	OutContext = PendingTransition;
	OutFailure = FText::GetEmpty();
	return true;
}

bool FDemoTransitionContextStore::ConsumeTransition(FDemoTransitionContext& OutContext, FText& OutFailure)
{
	if (!bHasPendingTransition)
	{
		OutFailure = FText::FromString(TEXT("No demo transition context is pending."));
		return false;
	}

	OutContext = PendingTransition;
	PendingTransition = FDemoTransitionContext();
	bHasPendingTransition = false;
	OutFailure = FText::GetEmpty();
	return true;
}

void FDemoTransitionContextStore::ClearTransition()
{
	PendingTransition = FDemoTransitionContext();
	bHasPendingTransition = false;
}

bool UDemoTransitionSessionSubsystem::PublishTransition(const FDemoTransitionContext& InContext, FText& OutFailure)
{
	return ContextStore.PublishTransition(InContext, OutFailure);
}

bool UDemoTransitionSessionSubsystem::PeekTransition(FDemoTransitionContext& OutContext, FText& OutFailure) const
{
	return ContextStore.PeekTransition(OutContext, OutFailure);
}

bool UDemoTransitionSessionSubsystem::ConsumeTransition(FDemoTransitionContext& OutContext, FText& OutFailure)
{
	return ContextStore.ConsumeTransition(OutContext, OutFailure);
}

void UDemoTransitionSessionSubsystem::ClearTransition()
{
	ContextStore.ClearTransition();
}
