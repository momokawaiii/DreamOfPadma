#pragma once

#include "CoreMinimal.h"

#include "DemoTransitionTypes.generated.h"

/** Stable identity for the one playtest node owned by TASK-007. */
USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FDemoNodeId
{
	GENERATED_BODY()

	FDemoNodeId() = default;
	explicit FDemoNodeId(const FName InValue)
		: Value(InValue)
	{
	}

	bool IsValid() const
	{
		return !Value.IsNone();
	}

	FString ToDebugString() const
	{
		return Value.ToString();
	}

	bool operator==(const FDemoNodeId& Other) const
	{
		return Value == Other.Value;
	}

	bool operator!=(const FDemoNodeId& Other) const
	{
		return !(*this == Other);
	}

	friend uint32 GetTypeHash(const FDemoNodeId& Id)
	{
		return GetTypeHash(Id.Value);
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo Identity")
	FName Value = NAME_None;
};

/** Stable identity for the scenario that consumes a selected demo node. */
USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FDemoScenarioId
{
	GENERATED_BODY()

	FDemoScenarioId() = default;
	explicit FDemoScenarioId(const FName InValue)
		: Value(InValue)
	{
	}

	bool IsValid() const
	{
		return !Value.IsNone();
	}

	FString ToDebugString() const
	{
		return Value.ToString();
	}

	bool operator==(const FDemoScenarioId& Other) const
	{
		return Value == Other.Value;
	}

	friend uint32 GetTypeHash(const FDemoScenarioId& Id)
	{
		return GetTypeHash(Id.Value);
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo Identity")
	FName Value = NAME_None;
};

/** Stable identity for the spawn point consumed by the target scenario. */
USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FDemoSpawnPointId
{
	GENERATED_BODY()

	FDemoSpawnPointId() = default;
	explicit FDemoSpawnPointId(const FName InValue)
		: Value(InValue)
	{
	}

	bool IsValid() const
	{
		return !Value.IsNone();
	}

	FString ToDebugString() const
	{
		return Value.ToString();
	}

	bool operator==(const FDemoSpawnPointId& Other) const
	{
		return Value == Other.Value;
	}

	friend uint32 GetTypeHash(const FDemoSpawnPointId& Id)
	{
		return GetTypeHash(Id.Value);
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo Identity")
	FName Value = NAME_None;
};

/** Typed input emitted by the map presentation when a tile is selected. */
USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FDemoSelectNodeRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo Request")
	FDemoNodeId DemoNodeId;
};

/** Typed input emitted when the selected tile is confirmed for entry. */
USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FDemoConfirmNodeSelectionRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo Request")
	FDemoNodeId DemoNodeId;
};

/**
 * Cross-level context. Only stable IDs are transition identity; map coordinates,
 * Actors, and level presentation details are deliberately absent.
 */
USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FDemoTransitionContext
{
	GENERATED_BODY()

	bool TryValidate(FText& OutFailure) const
	{
		if (!DemoNodeId.IsValid())
		{
			OutFailure = FText::FromString(TEXT("Demo transition context is missing the demo node ID."));
			return false;
		}

		if (!TargetScenarioId.IsValid())
		{
			OutFailure = FText::FromString(TEXT("Demo transition context is missing the target scenario ID."));
			return false;
		}

		if (!SpawnPointId.IsValid())
		{
			OutFailure = FText::FromString(TEXT("Demo transition context is missing the spawn-point ID."));
			return false;
		}

		OutFailure = FText::GetEmpty();
		return true;
	}

	UPROPERTY(BlueprintReadOnly, Category = "Demo Transition")
	FDemoNodeId DemoNodeId;

	UPROPERTY(BlueprintReadOnly, Category = "Demo Transition")
	FDemoScenarioId TargetScenarioId;

	UPROPERTY(BlueprintReadOnly, Category = "Demo Transition")
	FDemoSpawnPointId SpawnPointId;
};
