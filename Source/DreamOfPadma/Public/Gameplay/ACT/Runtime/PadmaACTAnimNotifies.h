#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Gameplay/ACT/Runtime/PadmaACTMelee.h"
#include "Gameplay/ACT/Runtime/PadmaACTActions.h"
#include "PadmaACTAnimNotifies.generated.h"

/** Presentation-only mount/visibility event; never opens a hit or GAS window. */
UCLASS(meta=(DisplayName="Padma ACT Weapon Presentation"))
class DREAMOFPADMA_API UAnimNotify_PadmaACTWeaponPresentation : public UAnimNotify
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 WeaponIndex = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bDrawn = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bVisible = true;
    /** Prepare the neutral base pose while the sheath montage still has full weight. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bReleaseCombatIdle = false;
    virtual void Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference) override;
};

/** Queued notifies permit several source bursts at exactly the same animation time. */
UCLASS(meta=(DisplayName="Padma ACT Action FX"))
class DREAMOFPADMA_API UAnimNotify_PadmaACTFX : public UAnimNotify
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect") FPadmaACTMeleeFX Effect;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect") bool bStop = false;
    virtual void Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference) override;
};

UCLASS(meta=(DisplayName="Padma ACT Blade Contact Window"))
class DREAMOFPADMA_API UAnimNotifyState_PadmaACTHitWindow : public UAnimNotifyState
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Contact") FPadmaACTMeleeWindow Window;
    virtual void NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float Duration, const FAnimNotifyEventReference& Reference) override;
    virtual void NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference) override;
};

UCLASS(meta=(DisplayName="Padma ACT Combo Input Window"))
class DREAMOFPADMA_API UAnimNotifyState_PadmaACTComboWindow : public UAnimNotifyState
{
    GENERATED_BODY()
public:
    virtual void NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float Duration, const FAnimNotifyEventReference& Reference) override;
    virtual void NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference) override;
};

/** Separate source ComboCache, AllowNextSkill and exclusive-time recovery windows. */
UCLASS(meta=(DisplayName="Padma ACT Action Transition Window"))
class DREAMOFPADMA_API UAnimNotifyState_PadmaACTTransitionWindow : public UAnimNotifyState
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite) EPadmaACTTransitionWindow Kind = EPadmaACTTransitionWindow::Allow;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FName> Actions;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0")) float CacheSeconds = .3f;
    /** Source controller gate survives transitions into skills until its initial interval expires. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bCarryAcrossActions = false;
    virtual void NotifyBegin(USkeletalMeshComponent* Mesh,UAnimSequenceBase* Animation,float Duration,const FAnimNotifyEventReference& Reference) override;
    virtual void NotifyEnd(USkeletalMeshComponent* Mesh,UAnimSequenceBase* Animation,const FAnimNotifyEventReference& Reference) override;
};

UCLASS(meta=(DisplayName="Padma ACT Plunge Descend"))
class DREAMOFPADMA_API UAnimNotify_PadmaACTPlungeDescend : public UAnimNotify
{
    GENERATED_BODY()
public:
    virtual void Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference) override;
};

UCLASS(meta=(DisplayName="Padma ACT Dodge Window"))
class DREAMOFPADMA_API UAnimNotifyState_PadmaACTDodgeWindow : public UAnimNotifyState
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bPerfect = true;
    virtual void NotifyBegin(USkeletalMeshComponent* Mesh,UAnimSequenceBase* Animation,float Duration,const FAnimNotifyEventReference& Reference) override;
    virtual void NotifyEnd(USkeletalMeshComponent* Mesh,UAnimSequenceBase* Animation,const FAnimNotifyEventReference& Reference) override;
};
