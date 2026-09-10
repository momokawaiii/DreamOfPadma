#pragma once

#include "CoreMinimal.h"
#include "Game/Save/PadmaRunSaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/Run/PadmaRunRules.h"
#include "PadmaRunSubsystem.generated.h"

/** Session lifetime/persistence adapter. Domain settlement stays in the pure rules object. */
UCLASS()
class DREAMOFPADMA_API UPadmaRunSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    using UGameInstanceSubsystem::Initialize;
    // A controller created after travel may offer content again; an active run remains intact.
    void Initialize(const FPadmaContentSnapshot& Content, const FPadmaMapLayout* AuthoredReference=nullptr);
    FPadmaRunRules& Rules() { return RunRules; }
    const FPadmaRunRules& Rules() const { return RunRules; }
    FPadmaRunCommandResult StartNew(const TMap<FName, double>& Overrides = {});
    FPadmaRunCommandResult StartNewMap(const FPadmaContentSnapshot& Content, const FPadmaMapLayout* AuthoredReference=nullptr);
    FPadmaRunCommandResult SaveRun(const FString& Slot = TEXT("PadmaDemo046"));
    FPadmaRunCommandResult LoadRun(const FString& Slot = TEXT("PadmaDemo046"));
    // In-memory envelope validation also supports save round-trip automation without disk fixtures.
    FPadmaRunCommandResult LoadValue(const UPadmaRunSaveGame* Save);

private:
    FPadmaRunRules RunRules;
};
