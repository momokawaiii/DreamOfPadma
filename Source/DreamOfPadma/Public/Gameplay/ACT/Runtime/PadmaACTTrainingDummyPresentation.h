#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PadmaACTTrainingDummyPresentation.generated.h"

class UPadmaCombatComponent;
class APadmaCombatUnit;
class UAnimSequence;
struct FPadmaCombatReceipt;

/** Training-only visual reaction. Receipts remain the authority for damage. */
UCLASS()
class DREAMOFPADMA_API UPadmaACTTrainingDummyPresentation : public UActorComponent
{
    GENERATED_BODY()
public:
    UPadmaACTTrainingDummyPresentation();
    bool Configure(UPadmaCombatComponent* Battle, UAnimSequence* HitClip);
    bool IsReacting() const { return bReacting; }
    virtual void TickComponent(float Delta, ELevelTick Type, FActorComponentTickFunction* Function) override;
protected:
    virtual void OnUnregister() override;
private:
    void HandleReceipt(const FPadmaCombatReceipt& Receipt);
    void Unbind();
    TWeakObjectPtr<UPadmaCombatComponent> BoundBattle;
    TWeakObjectPtr<APadmaCombatUnit> Unit;
    UPROPERTY(Transient) TObjectPtr<UAnimSequence> HitAnimation;
    FDelegateHandle ReceiptHandle;
    bool bReacting = false;
};
