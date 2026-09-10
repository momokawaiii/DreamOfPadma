#include "Game/Framework/PadmaGameMode.h"
#include "Game/Framework/PadmaPlayerController.h"

APadmaGameMode::APadmaGameMode()
{
	DefaultPawnClass=nullptr;PlayerControllerClass=APadmaPlayerController::StaticClass();
}
