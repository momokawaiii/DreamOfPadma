#pragma once
#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "PadmaACTHologramComponent.generated.h"

/** A perspective-correct, depth-tested panel with native mouse hit testing. */
UCLASS()
class DREAMOFPADMA_API UPadmaACTHologramComponent : public UWidgetComponent
{
    GENERATED_BODY()
public:
    UPadmaACTHologramComponent();
};
