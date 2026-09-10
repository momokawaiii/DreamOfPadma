#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Core/Run/PadmaRunRules.h"
#include "PadmaRunSaveGame.generated.h"

/** Versioned native envelope. Every nested run field is a stable ID or value. */
UCLASS()
class DREAMOFPADMA_API UPadmaRunSaveGame : public USaveGame
{
    GENERATED_BODY()
public:
    /** Reads current and TASK-046 envelopes without rewriting the stored file. */
    static UPadmaRunSaveGame* LoadFromMemory(const TArray<uint8>& Data);

    UPROPERTY(SaveGame) FString AppId = TEXT("padma-native-demo");
    UPROPERTY(SaveGame) int32 FormatVersion = 1;
    UPROPERTY(SaveGame) FPadmaRunState Run;
};
