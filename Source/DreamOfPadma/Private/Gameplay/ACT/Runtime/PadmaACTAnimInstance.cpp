#include "Gameplay/ACT/Runtime/PadmaACTAnimInstance.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimNode_SequencePlayer.h"
#include "Animation/AnimSequence.h"
#include "AnimNodes/AnimNode_Slot.h"

namespace
{
struct FPadmaACTAnimProxy final : FAnimInstanceProxy
{
    FAnimNode_SequencePlayer_Standalone Idle;
    FAnimNode_Slot Slot;
    explicit FPadmaACTAnimProxy(UAnimInstance* Instance) : FAnimInstanceProxy(Instance)
    {
        Slot.SlotName = TEXT("DefaultSlot");
        Slot.Source.SetLinkNode(&Idle);
        Slot.bAlwaysUpdateSourcePose = true;
        Idle.SetLoopAnimation(true);
    }
    virtual FAnimNode_Base* GetCustomRootNode() override { return &Slot; }
    virtual void GetCustomNodes(TArray<FAnimNode_Base*>& Nodes) override { Nodes.Add(&Idle); Nodes.Add(&Slot); }
    virtual void PreUpdate(UAnimInstance* Instance, float Delta) override
    {
        FAnimInstanceProxy::PreUpdate(Instance, Delta);
        Idle.SetSequence(CastChecked<UPadmaACTAnimInstance>(Instance)->IdleAnimation);
    }
};
}
FAnimInstanceProxy* UPadmaACTAnimInstance::CreateAnimInstanceProxy() { return new FPadmaACTAnimProxy(this); }
void UPadmaACTAnimInstance::DestroyAnimInstanceProxy(FAnimInstanceProxy* Proxy) { delete Proxy; }
