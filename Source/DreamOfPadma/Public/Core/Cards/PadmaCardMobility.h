#pragma once

#include "CoreMinimal.h"
#include "PadmaCardMobility.generated.h"

UENUM(BlueprintType)
enum class EPadmaCardMobility : uint8
{
	Movable,
	Immovable,
	Conditional
};

/** Static card metadata. Validation is not permission to move a deployed instance. */
USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaCardMobilityDefinition
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bConfigured = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (EditCondition = "bConfigured"))
	EPadmaCardMobility Policy = EPadmaCardMobility::Immovable;
	/** Selects a future registered rule evaluator, never an executable expression. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (EditCondition = "bConfigured"))
	FName ConditionId;

	bool Validate(FText& OutFailure) const;
};
