#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PadmaGameMode.generated.h"

/** Native entry point on the authored playable maps. */
UCLASS()
class DREAMOFPADMA_API APadmaGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	APadmaGameMode();
};
