#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PadmaACTAnimInstance.generated.h"

/** Minimal native idle -> DefaultSlot graph used by the optional melee presentation. */
UCLASS(Transient, BlueprintType)
class DREAMOFPADMA_API UPadmaACTAnimInstance : public UAnimInstance
{
    GENERATED_BODY()
public:
    UPROPERTY(Transient) TObjectPtr<UAnimSequence> IdleAnimation;
protected:
    virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;
    virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* Proxy) override;
};
