#include "Gameplay/ACT/Runtime/PadmaACTTrainingDummyPresentation.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "Animation/Skeleton.h"
#include "Components/SkeletalMeshComponent.h"

UPadmaACTTrainingDummyPresentation::UPadmaACTTrainingDummyPresentation()
{
    PrimaryComponentTick.bCanEverTick = true;
}

bool UPadmaACTTrainingDummyPresentation::Configure(UPadmaCombatComponent* Battle, UAnimSequence* HitClip)
{
    Unbind();
    auto* OwnerUnit = Cast<APadmaCombatUnit>(GetOwner());
    if (!Battle || !OwnerUnit || OwnerUnit->Spec.bPlayer || !HitClip || !HitClip->GetSkeleton()
        || !OwnerUnit->GetMesh()->GetSkeletalMeshAsset()
        || !HitClip->GetSkeleton()->IsCompatibleMesh(OwnerUnit->GetMesh()->GetSkeletalMeshAsset())) return false;
    Unit = OwnerUnit;
    BoundBattle = Battle;
    HitAnimation = HitClip;
    auto* Mesh = OwnerUnit->GetMesh();
    Mesh->SetCollisionProfileName(TEXT("NoCollision"));
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Mesh->SetSimulatePhysics(false);
    Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
    Mesh->PlayAnimation(HitClip, false);
    Mesh->GetSingleNodeInstance()->SetPosition(0.f, false);
    Mesh->GetSingleNodeInstance()->SetPlaying(false);
    AddTickPrerequisiteComponent(Mesh);
    ReceiptHandle = Battle->OnReceipt.AddUObject(this, &ThisClass::HandleReceipt);
    return true;
}

void UPadmaACTTrainingDummyPresentation::HandleReceipt(const FPadmaCombatReceipt& Receipt)
{
    if (!Unit.IsValid() || Receipt.Effect != TEXT("damage") || Receipt.TargetId != Unit->Spec.Id) return;
    // A shielded/blocked contact still reacts; buffs, evasions and empty swings do not.
    if (auto* Instance = Unit->GetMesh()->GetSingleNodeInstance())
    {
        Instance->SetPosition(0.f, false);
        Instance->SetPlaying(true);
        bReacting = true;
    }
}

void UPadmaACTTrainingDummyPresentation::TickComponent(float Delta, ELevelTick Type, FActorComponentTickFunction* Function)
{
    Super::TickComponent(Delta, Type, Function);
    if (bReacting && Unit.IsValid())
        if (auto* Instance = Unit->GetMesh()->GetSingleNodeInstance(); Instance && !Instance->IsPlaying())
        {
            Instance->SetPosition(0.f, false);
            bReacting = false;
        }
}

void UPadmaACTTrainingDummyPresentation::Unbind()
{
    if (BoundBattle.IsValid()) BoundBattle->OnReceipt.Remove(ReceiptHandle);
    if (Unit.IsValid()) RemoveTickPrerequisiteComponent(Unit->GetMesh());
    ReceiptHandle.Reset();
    BoundBattle.Reset();
    Unit.Reset();
    HitAnimation = nullptr;
    bReacting = false;
}

void UPadmaACTTrainingDummyPresentation::OnUnregister()
{
    Unbind();
    Super::OnUnregister();
}
