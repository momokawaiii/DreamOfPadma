#include "Gameplay/ACT/Runtime/PadmaACTMelee.h"
#include "Gameplay/ACT/Runtime/PadmaACTActions.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

bool UPadmaACTMeleeComponent::WantsWeaponCombatIdle() const
{
    return Profile && Profile->bWeaponLifecycle && (WeaponPhase==EWeaponPhase::Drawing
        || WeaponPhase==EWeaponPhase::Drawn || WeaponPhase==EWeaponPhase::Holding || (WeaponPhase==EWeaponPhase::Sheathing && !bWeaponIdleReleased));
}

void UPadmaACTMeleeComponent::SetWeaponMount(int32 Index, bool Drawn)
{
    auto* Unit=Cast<APadmaCombatUnit>(GetOwner());
    if (!Profile || !Unit || !Weapons.IsValidIndex(Index)) return;
    const auto& Config=Profile->Weapons[Index];
    auto* Weapon=Weapons[Index].Get();
    if (!IsValid(Weapon) || Config.StowedSocket.IsNone()) return;
    const FName* AuthoredSocket=SceneWeaponStowedSockets.Find(Weapon);
    Weapon->AttachToComponent(Unit->GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale,Drawn ? Config.Socket : AuthoredSocket ? *AuthoredSocket : Config.StowedSocket);
    FTransform Mount=Drawn ? FTransform::Identity : Config.StowedTransform;
    if (const auto* Offset=SceneWeaponOffsets.Find(Weapon)) Mount=*Offset * Mount;
    Weapon->SetRelativeTransform(Mount);
    Weapon->UpdateComponentToWorld();
    if (PreviousWeapons.IsValidIndex(Index)) PreviousWeapons[Index]=Weapon->GetComponentTransform();
}

void UPadmaACTMeleeComponent::SetWeaponDissolve(float Alpha)
{
    for (auto Weapon:Weapons) if (IsValid(Weapon))
    {
        // The scabbard remains present while the displayed blade dissolves.
        if (bShowcaseRestWeapons && Alpha>0.f && Weapons.IsValidIndex(2) && Weapon==Weapons[2]) continue;
        // A sheath AN may already have hidden a particular blade. Fading must not resurrect it.
        if (Alpha<=0.f || Alpha>=1.f) Weapon->SetVisibility(Alpha<1.f);
        for (int32 Slot=0; Slot<Weapon->GetNumMaterials(); ++Slot)
            if (auto* MID=Cast<UMaterialInstanceDynamic>(Weapon->GetMaterial(Slot)))
                MID->SetScalarParameterValue(TEXT("WeaponDissolve"),FMath::Clamp(Alpha,0.f,1.f));
    }
}

void UPadmaACTMeleeComponent::StopWeaponMontage()
{
    // Invalidate before stopping: old queued ANs cannot affect a replacement action.
    auto* Old=WeaponMontage.Get();
    const int32 OldID=WeaponMontageInstanceID;
    WeaponMontage=nullptr; WeaponMontageInstanceID=INDEX_NONE;
    auto* Unit=Cast<APadmaCombatUnit>(GetOwner());
    auto* Anim=Unit ? Unit->GetMesh()->GetAnimInstance() : nullptr;
    auto* Instance=Anim ? Anim->GetMontageInstanceForID(OldID) : nullptr;
    if (Old && Instance && Instance->Montage==Old && Instance->IsActive()) Anim->Montage_Stop(.1f,Old);
}

void UPadmaACTMeleeComponent::CompleteWeaponMontage(bool Draw)
{
    StopWeaponMontage();
    // Events can be skipped by interruption; settle all mounts to the chosen terminal state.
    for (int32 I=0; I<Weapons.Num(); ++I) SetWeaponMount(I,Draw);
    bWeaponsDrawn=Draw; SetWeaponDissolve(0);
    WeaponPhase=Draw ? EWeaponPhase::Drawn : EWeaponPhase::Stowed;
    WeaponPhaseAge=0;
}

void UPadmaACTMeleeComponent::PlayWeaponMontage(bool Draw)
{
    StopWeaponMontage(); SetWeaponDissolve(0); WeaponPhaseAge=0;
    WeaponPhase=Draw ? EWeaponPhase::Drawing : EWeaponPhase::Sheathing;
    bWeaponIdleReleased=false;
    auto* Unit=Cast<APadmaCombatUnit>(GetOwner());
    auto* Anim=Unit ? Unit->GetMesh()->GetAnimInstance() : nullptr;
    auto* Montage=(Draw ? Profile->DrawWeaponMontage : Profile->SheatheWeaponMontage).LoadSynchronous();
    // Movement remains authoritative; transition poses must never freeze locomotion.
    if (!Anim || !Montage || !Unit->GetVelocity().IsNearlyZero(5.f) || Unit->GetCharacterMovement()->IsFalling())
    { CompleteWeaponMontage(Draw); return; }
    WeaponMontage=Montage;
    if (Anim->Montage_Play(Montage,1.f,EMontagePlayReturnType::MontageLength,0.f,false)<=0)
    { CompleteWeaponMontage(Draw); return; }
    if (auto* Instance=Anim->GetActiveInstanceForMontage(Montage)) WeaponMontageInstanceID=Instance->GetInstanceID();
    else CompleteWeaponMontage(Draw);
}

void UPadmaACTMeleeComponent::EnterWeaponCombat()
{
    ReleaseShowcaseRestWeapons();
    auto* Unit=Cast<APadmaCombatUnit>(GetOwner());
    if (!Profile || !Profile->bWeaponLifecycle || !Unit || !Unit->IsAlive()) return;
    if (bAttacking)
    {
        // Do not steal a jump/dodge/skill montage. Its completion starts the hold.
        bActionDrewWeapons=true; SetWeaponDissolve(0);
        for (int32 I=0; I<Weapons.Num(); ++I) SetWeaponMount(I,true);
        bWeaponsDrawn=true; WeaponPhase=EWeaponPhase::Drawn; WeaponPhaseAge=0; return;
    }
    if (WeaponPhase==EWeaponPhase::Drawn || WeaponPhase==EWeaponPhase::Holding)
    { WeaponPhase=EWeaponPhase::Drawn; WeaponPhaseAge=0; return; }
    PlayWeaponMontage(true);
}

void UPadmaACTMeleeComponent::LeaveWeaponCombat()
{
    if (!Profile || !Profile->bWeaponLifecycle || bAttacking) return;
    if (WeaponPhase==EWeaponPhase::Drawing) CompleteWeaponMontage(true);
    if (WeaponPhase==EWeaponPhase::Drawn) { WeaponPhase=EWeaponPhase::Holding; WeaponPhaseAge=0; }
}

void UPadmaACTMeleeComponent::PrepareWeaponAction(bool Draw)
{
    ReleaseShowcaseRestWeapons();
    if (!Profile->bWeaponLifecycle) { SetWeaponsDrawn(Draw); return; }
    const bool WasDrawing=WeaponPhase==EWeaponPhase::Drawing;
    const bool WasSheathing=WeaponPhase==EWeaponPhase::Sheathing;
    StopWeaponMontage();
    if (WasSheathing) CompleteWeaponMontage(false);
    if (WasDrawing) CompleteWeaponMontage(true);
    bActionDrewWeapons=Draw || WeaponPhase==EWeaponPhase::Drawn || WeaponPhase==EWeaponPhase::Holding;
    if (bActionDrewWeapons)
    {
        SetWeaponDissolve(0);
        for (int32 I=0; I<Weapons.Num(); ++I) SetWeaponMount(I,true);
        bWeaponsDrawn=true; WeaponPhase=EWeaponPhase::Drawn; WeaponPhaseAge=0;
    }
    // Non-weapon traversal preserves hidden/stowed weapons instead of flashing them into the hands.
}

void UPadmaACTMeleeComponent::FinishWeaponAction()
{
    if (!Profile) return;
    if (!Profile->bWeaponLifecycle) { SetWeaponsDrawn(false); return; }
    if (bActionDrewWeapons) { WeaponPhase=EWeaponPhase::Holding; WeaponPhaseAge=0; }
    bActionDrewWeapons=false;
}

void UPadmaACTMeleeComponent::WeaponPresentationNotify(UAnimSequenceBase* Animation,int32 InstanceID,int32 Index,bool Drawn,bool Visible,bool ReleaseCombatIdle)
{
    if (bAttacking || !WeaponMontage || Animation!=WeaponMontage || InstanceID!=WeaponMontageInstanceID) return;
    if (ReleaseCombatIdle && WeaponPhase==EWeaponPhase::Sheathing) bWeaponIdleReleased=true;
    auto Apply=[&](int32 I)
    {
        if (!Weapons.IsValidIndex(I)) return;
        SetWeaponMount(I,Drawn); Weapons[I]->SetVisibility(Visible);
    };
    if (Index<0) for (int32 I=0; I<Weapons.Num(); ++I) Apply(I);
    else Apply(Index);
}

void UPadmaACTMeleeComponent::TickWeaponPresentation(float Delta)
{
    if (bShowcasePresentation) return;
    if (!Profile || !Profile->bWeaponLifecycle) return;
    auto* Unit=Cast<APadmaCombatUnit>(GetOwner());
    if (!Unit) return;
    if (!Unit->IsAlive() || !Unit->GetBattle() || !Unit->GetBattle()->IsBattleActive() || Unit->GetBattle()->GetMode()!=EPadmaCombatMode::ACT)
    { ReleaseShowcaseRestWeapons(); StopWeaponMontage(); SetWeaponDissolve(1); WeaponPhase=EWeaponPhase::Hidden; WeaponPhaseAge=0; return; }
    if (bAttacking) return;
    if (WeaponPhase==EWeaponPhase::Appearing)
    {
        WeaponPhaseAge+=FMath::Max(0.f,Delta);
        const float Alpha=FMath::Clamp(WeaponPhaseAge/FMath::Max(.01f,WeaponDissolveDuration),0.f,1.f);
        // Reverse the same dissolve mask on the sheathed blade only. The main
        // sword stays hidden and the scabbard is never faded during this handoff.
        if (Weapons.IsValidIndex(1) && IsValid(Weapons[1]))
            for (int32 Slot=0; Slot<Weapons[1]->GetNumMaterials(); ++Slot)
                if (auto* MID=Cast<UMaterialInstanceDynamic>(Weapons[1]->GetMaterial(Slot)))
                    MID->SetScalarParameterValue(TEXT("WeaponDissolve"),1.f-Alpha);
        if (Alpha>=1.f) { WeaponPhase=EWeaponPhase::Stowed;WeaponPhaseAge=0; }
        return;
    }
    // Keep the sheathed blade and scabbard throughout the overview breathing loop.
    if (bShowcaseRestWeapons && WeaponPhase==EWeaponPhase::Stowed) return;
    if (Unit->GetActions()->HasLocomotionInput() && Unit->GetCharacterMovement()->IsMovingOnGround()
        && !Unit->GetCharacterMovement()->HasAnimRootMotion() && Unit->GetVelocity().Size2D()>5.f
        && WeaponPhase!=EWeaponPhase::Hidden)
    {
        // Walking/running retires at the CURRENT mount. Do not teleport a held blade onto the back.
        if (WeaponPhase==EWeaponPhase::Dissolving)
        {
            const float Alpha=FMath::Clamp(WeaponPhaseAge/FMath::Max(.01f,WeaponDissolveDuration),0.f,1.f);
            WeaponDissolveDuration=FMath::Max(.01f,Profile->MovingWeaponDissolveSeconds);
            WeaponPhaseAge=Alpha*WeaponDissolveDuration;
        }
        else
        {
            StopWeaponMontage(); bWeaponIdleReleased=true;
            WeaponDissolveDuration=Profile->MovingWeaponDissolveSeconds;
            WeaponPhase=EWeaponPhase::Dissolving; WeaponPhaseAge=0;
        }
    }
    if (WeaponPhase==EWeaponPhase::Drawing || WeaponPhase==EWeaponPhase::Sheathing)
    {
        auto* Anim=Unit->GetMesh()->GetAnimInstance();
        auto* Instance=Anim ? Anim->GetMontageInstanceForID(WeaponMontageInstanceID) : nullptr;
        if (!Instance || !Instance->IsActive() || !Unit->GetVelocity().IsNearlyZero(5.f) || Unit->GetCharacterMovement()->IsFalling())
            CompleteWeaponMontage(WeaponPhase==EWeaponPhase::Drawing);
        return;
    }
    WeaponPhaseAge+=FMath::Max(0.f,Delta);
    if (WeaponPhase==EWeaponPhase::Holding && WeaponPhaseAge>=FMath::Max(0.f,Profile->DrawnHoldSeconds)) PlayWeaponMontage(false);
    else if (WeaponPhase==EWeaponPhase::Stowed && WeaponPhaseAge>=FMath::Max(0.f,Profile->StowedHoldSeconds))
    { WeaponPhase=EWeaponPhase::Dissolving; WeaponPhaseAge=0; WeaponDissolveDuration=Profile->WeaponDissolveSeconds; }
    else if (WeaponPhase==EWeaponPhase::Dissolving)
    {
        const float Alpha=WeaponPhaseAge/FMath::Max(.01f,WeaponDissolveDuration);
        // Zero-delta ticks must not resurrect individually hidden equipment.
        if (Alpha>0.f) SetWeaponDissolve(Alpha);
        if (Alpha>=1)
        {
            // Reset mounts only after full invisibility, ready for the next draw transition.
            for (int32 I=0; I<Weapons.Num(); ++I) SetWeaponMount(I,false);
            bWeaponsDrawn=false; WeaponPhase=EWeaponPhase::Hidden; WeaponPhaseAge=0;
            if (bShowcaseRestWeapons)
            {
                // The held main blade is fully gone before its sheathed counterpart
                // becomes visible. Reuse the source inner/outer equipment mounts.
                // Its mask is still fully dissolved from the preceding phase.
                // Enable rendering now, then grow it back over half a second.
                if (Weapons.IsValidIndex(1) && IsValid(Weapons[1])) Weapons[1]->SetVisibility(true);
                WeaponDissolveDuration=.5f;
                WeaponPhase=EWeaponPhase::Appearing;
            }
        }
    }
}

void UPadmaACTMeleeComponent::ReleaseShowcaseRestWeapons()
{
    bShowcaseRestWeapons=false;
    if (WeaponPhase==EWeaponPhase::Appearing)
    {
        // An interrupted reveal retires from its current opacity, not a flash.
        WeaponPhaseAge=FMath::Max(0.f,WeaponDissolveDuration-WeaponPhaseAge);
        WeaponPhase=EWeaponPhase::Dissolving;
    }
}

void UPadmaACTMeleeComponent::SetShowcasePresentation(bool bEnabled, bool bDissolveOnExit)
{
    // A repeated graceful exit keeps the current fade; forced cleanup must still
    // retire rest equipment even after showcase ownership was handed off.
    if (bShowcasePresentation == bEnabled && (bDissolveOnExit || !bShowcaseRestWeapons)) return;
    bShowcasePresentation = bEnabled;
    bShowcaseRestWeapons=false;
    StopWeaponMontage();
    if (!bEnabled && bDissolveOnExit && Profile && Profile->bWeaponLifecycle)
    {
        bShowcaseRestWeapons=true;
        // Hand over to the normal material fade at the current sockets. Preserve
        // per-component visibility (the showcase offhand is already hidden).
        bWeaponIdleReleased=true;
        bActionDrewWeapons=false;
        WeaponDissolveDuration=FMath::Max(.01f,Profile->WeaponDissolveSeconds);
        WeaponPhase=EWeaponPhase::Dissolving;
        WeaponPhaseAge=0;
        return;
    }
    // Hide before moving mounts: never render a hand/back teleport on exit.
    SetWeaponDissolve(1.f);
    for (int32 I=0; I<Weapons.Num(); ++I) SetWeaponMount(I,bEnabled && I==0);
    bWeaponsDrawn=bEnabled;
    bWeaponIdleReleased=!bEnabled;
    SetWeaponDissolve(bEnabled ? 0.f : 1.f);
    // The weapon-page animation presents the main sword, not the combat pair.
    // Keep non-blade equipment (e.g. the scabbard) at its authored mount.
    if (bEnabled && Weapons.IsValidIndex(1) && IsValid(Weapons[1])) Weapons[1]->SetVisibility(false);
    WeaponPhase=bEnabled ? EWeaponPhase::Drawn : EWeaponPhase::Hidden;
    WeaponPhaseAge=0;
}
