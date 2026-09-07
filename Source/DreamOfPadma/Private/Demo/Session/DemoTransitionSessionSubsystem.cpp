#include "Demo/Session/DemoTransitionSessionSubsystem.h"

#include "Demo/World/DemoSandboxWorld.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "UObject/UObjectGlobals.h"

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

void UDemoTransitionSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(
		this,
		&UDemoTransitionSessionSubsystem::HandlePostLoadMap);
	WorldInitializedActorsHandle = FWorldDelegates::OnWorldInitializedActors.AddUObject(
		this,
		&UDemoTransitionSessionSubsystem::HandleWorldInitializedActors);

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		BindToSandboxWorld(GameInstance->GetWorld());
	}
}

void UDemoTransitionSessionSubsystem::Deinitialize()
{
	UnbindFromSandboxWorld();

	if (PostLoadMapHandle.IsValid())
	{
		FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapHandle);
		PostLoadMapHandle.Reset();
	}

	if (WorldInitializedActorsHandle.IsValid())
	{
		FWorldDelegates::OnWorldInitializedActors.Remove(WorldInitializedActorsHandle);
		WorldInitializedActorsHandle.Reset();
	}

	Super::Deinitialize();
}

void UDemoTransitionSessionSubsystem::HandlePostLoadMap(UWorld* LoadedWorld)
{
	BindToSandboxWorld(LoadedWorld);
}

void UDemoTransitionSessionSubsystem::HandleWorldInitializedActors(const FActorsInitializedParams& Params)
{
	if (Params.World == nullptr || Params.World->GetGameInstance() != GetGameInstance())
	{
		return;
	}

	BindToSandboxWorld(Params.World);
	if (!BoundSandboxWorld.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[TASK-007] No Demo Sandbox transition callback target was found after actors initialized."));
	}
}

void UDemoTransitionSessionSubsystem::BindToSandboxWorld(UWorld* World)
{
	if (World == nullptr || World->GetGameInstance() != GetGameInstance())
	{
		return;
	}

	UnbindFromSandboxWorld();

	for (TActorIterator<ADemoSandboxWorld> It(World); It; ++It)
	{
		ADemoSandboxWorld* SandboxWorld = *It;
		if (SandboxWorld == nullptr)
		{
			continue;
		}

		SandboxWorld->SetTransitionPublisher(
			FDemoTransitionContextPublisher::CreateUObject(
				this,
				&UDemoTransitionSessionSubsystem::PublishFromSandboxWorld));
		SandboxWorld->SetTransitionConsumer(
			FDemoTransitionContextConsumer::CreateUObject(
				this,
				&UDemoTransitionSessionSubsystem::ConsumeFromSandboxWorld));
		BoundSandboxWorld = SandboxWorld;
		UE_LOG(LogTemp, Log, TEXT("[TASK-007] Game session bound to Demo Sandbox transition callbacks."));
		return;
	}

	UE_LOG(LogTemp, Verbose, TEXT("[TASK-007] Demo Sandbox actor is not initialized yet; waiting for the actors-initialized callback."));
}

void UDemoTransitionSessionSubsystem::UnbindFromSandboxWorld()
{
	if (ADemoSandboxWorld* SandboxWorld = BoundSandboxWorld.Get())
	{
		SandboxWorld->SetTransitionPublisher(FDemoTransitionContextPublisher());
		SandboxWorld->SetTransitionConsumer(FDemoTransitionContextConsumer());
	}

	BoundSandboxWorld.Reset();
}

bool UDemoTransitionSessionSubsystem::PublishFromSandboxWorld(
	const FDemoTransitionContext& InContext,
	FText& OutFailure)
{
	const bool bPublished = PublishTransition(InContext, OutFailure);
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[TASK-007] Demo Sandbox transition publish %s."),
		bPublished ? TEXT("succeeded") : TEXT("failed"));
	return bPublished;
}

bool UDemoTransitionSessionSubsystem::ConsumeFromSandboxWorld(
	FDemoTransitionContext& OutContext,
	FText& OutFailure)
{
	const bool bConsumed = ConsumeTransition(OutContext, OutFailure);
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[TASK-007] Demo Sandbox transition consume %s."),
		bConsumed ? TEXT("succeeded") : TEXT("failed"));
	return bConsumed;
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
