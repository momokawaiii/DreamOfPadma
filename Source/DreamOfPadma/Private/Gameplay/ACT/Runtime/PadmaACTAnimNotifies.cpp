#include "Gameplay/ACT/Runtime/PadmaACTAnimNotifies.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Gameplay/ACT/Runtime/PadmaACTActions.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/ActiveMontageInstanceScope.h"

void UAnimNotify_PadmaACTWeaponPresentation::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference)
{
    auto* Unit = Mesh ? Cast<APadmaCombatUnit>(Mesh->GetOwner()) : nullptr;
    const auto* Context = Reference.GetContextData<UE::Anim::FAnimNotifyMontageInstanceContext>();
    if (Unit && Context) Unit->GetMelee()->WeaponPresentationNotify(Animation,Context->MontageInstanceID,WeaponIndex,bDrawn,bVisible,bReleaseCombatIdle);
}

namespace
{
UPadmaACTMeleeComponent* ActiveMelee(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference)
{
    auto* Unit = Mesh ? Cast<APadmaCombatUnit>(Mesh->GetOwner()) : nullptr;
    auto* Melee = Unit ? Unit->GetMelee() : nullptr;
    const auto* Context=Reference.GetContextData<UE::Anim::FAnimNotifyMontageInstanceContext>();
    return Melee && Melee->AcceptsNotify(Animation) && (!Context || Context->MontageInstanceID==Melee->GetMontageInstanceID()) ? Melee : nullptr;
}
}
void UAnimNotify_PadmaACTFX::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference)
{
    if (auto* Melee = ActiveMelee(Mesh, Animation, Reference)) Melee->QueueActionEffect(Effect, bStop);
}
void UAnimNotify_PadmaACTPlungeDescend::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference)
{
    if (ActiveMelee(Mesh,Animation,Reference)) CastChecked<APadmaCombatUnit>(Mesh->GetOwner())->GetActions()->StartPlungeDescent();
}
void UAnimNotifyState_PadmaACTHitWindow::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float Duration, const FAnimNotifyEventReference& Reference)
{
    const auto* Event = Reference.GetNotify();
    if (auto* Melee = ActiveMelee(Mesh, Animation, Reference); Melee && Event)
        Melee->BeginHitWindow(this, Window, Event->GetTime(), Event->GetTime() + Event->GetDuration());
}
void UAnimNotifyState_PadmaACTHitWindow::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference)
{
    if (auto* Melee = ActiveMelee(Mesh, Animation, Reference)) Melee->EndHitWindow(this);
}
void UAnimNotifyState_PadmaACTComboWindow::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float Duration, const FAnimNotifyEventReference& Reference)
{
    const auto* Event = Reference.GetNotify();
    if (auto* Melee = ActiveMelee(Mesh, Animation, Reference); Melee && Event) Melee->SetComboWindow(true, Event->GetTime(), Event->GetTime()+Event->GetDuration());
}
void UAnimNotifyState_PadmaACTComboWindow::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference)
{
    if (auto* Melee = ActiveMelee(Mesh, Animation, Reference)) Melee->SetComboWindow(false);
}
void UAnimNotifyState_PadmaACTDodgeWindow::NotifyBegin(USkeletalMeshComponent* Mesh,UAnimSequenceBase* Animation,float Duration,const FAnimNotifyEventReference& Reference)
{
    const auto* Event=Reference.GetNotify();
    if (ActiveMelee(Mesh,Animation,Reference) && Event)
        CastChecked<APadmaCombatUnit>(Mesh->GetOwner())->GetActions()->SetDodgeWindow(true,bPerfect,Event->GetTime(),Event->GetTime()+Event->GetDuration());
}
void UAnimNotifyState_PadmaACTDodgeWindow::NotifyEnd(USkeletalMeshComponent* Mesh,UAnimSequenceBase* Animation,const FAnimNotifyEventReference& Reference)
{
    if (ActiveMelee(Mesh,Animation,Reference)) CastChecked<APadmaCombatUnit>(Mesh->GetOwner())->GetActions()->SetDodgeWindow(false,false);
}

void UAnimNotifyState_PadmaACTTransitionWindow::NotifyBegin(USkeletalMeshComponent* Mesh,UAnimSequenceBase* Animation,float Duration,const FAnimNotifyEventReference& Reference)
{
    const auto* Event=Reference.GetNotify();
    if (ActiveMelee(Mesh,Animation,Reference) && Event)
        CastChecked<APadmaCombatUnit>(Mesh->GetOwner())->GetActions()->SetTransitionWindow(this,true,Kind,Actions,Event->GetTime(),Event->GetTime()+Event->GetDuration(),CacheSeconds);
}
void UAnimNotifyState_PadmaACTTransitionWindow::NotifyEnd(USkeletalMeshComponent* Mesh,UAnimSequenceBase* Animation,const FAnimNotifyEventReference& Reference)
{
    if (ActiveMelee(Mesh,Animation,Reference)) CastChecked<APadmaCombatUnit>(Mesh->GetOwner())->GetActions()->SetTransitionWindow(this,false,Kind,Actions,0,0,CacheSeconds);
}
