#pragma once

#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Gameplay/ACT/Authoring/PadmaACTAuthoring.h"
#include "Gameplay/ACT/Runtime/PadmaACTAnimNotifies.h"
#include "Misc/AutomationTest.h"
#include "NiagaraSystem.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/Package.h"

namespace PadmaACTTest
{
inline constexpr TCHAR CharacterPath[] = TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/AbilitySystem/DA_ACTCharacter_CHEN");
inline constexpr TCHAR EquipmentPath[] = TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/AbilitySystem/DA_Chen_Equipment");
inline constexpr TCHAR MontagePath[] = TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Montages/AM_Chen_Attack01");
inline constexpr TCHAR ImportedAttackPath[] = TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Sequences/Imported/AS_Chen_Attack01_CM");
// All five former V2 systems are distinct from the similarly named action FX.
inline constexpr TCHAR SystemsRoot[] = TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Art/Niagara/Systems/BladeContact/");

// Soft asset references do not keep transient objects alive. Hold every fixture root
// for the entire test, including battle teardown/restart and synchronous world ticks.
struct FLegacyMeleeFixture
{
    TStrongObjectPtr<UPadmaACTCharacterDefinition> Character;
    TStrongObjectPtr<UPadmaACTMeleeDefinition> Profile;
    TStrongObjectPtr<UDataTable> Slots;
    TStrongObjectPtr<UAnimMontage> Montage;
    TArray<TStrongObjectPtr<UNiagaraSystem>> Systems;

    bool Initialize(FAutomationTestBase& Test)
    {
        auto* Live = LoadObject<UPadmaACTCharacterDefinition>(nullptr, CharacterPath);
        auto* Equipment = LoadObject<UPadmaACTMeleeDefinition>(nullptr, EquipmentPath);
        auto* Sequence = LoadObject<UAnimSequence>(nullptr, ImportedAttackPath);
        if (!Test.TestNotNull(TEXT("Live fixture character"), Live)
            || !Test.TestNotNull(TEXT("Live fixture equipment"), Equipment)
            || !Test.TestNotNull(TEXT("Shared imported attack animation"), Sequence)) return false;
        if (!Test.TestTrue(TEXT("Live character owns canonical equipment"), Live->MeleeProfile.LoadSynchronous() == Equipment)
            || !Test.TestEqual(TEXT("Shared three-weapon layout"), Equipment->Weapons.Num(), 3)
            || !Test.TestTrue(TEXT("Legacy source clip remains 220 frames at 60 Hz"), FMath::IsNearlyEqual(Sequence->GetPlayLength(), 220.f / 60.f, .0001f))) return false;

        const TCHAR* Names[] = {TEXT("NS_chen_attack_01_start"), TEXT("NS_chen_attack_01_start2"),
            TEXT("NS_chen_attack_01_star_right"), TEXT("NS_fxbat_chen_common_hit_01"), TEXT("NS_fxbat_chen_common_hit_02")};
        for (const TCHAR* Name : Names)
        {
            auto* System = LoadObject<UNiagaraSystem>(nullptr, *(FString(SystemsRoot) + Name));
            if (!Test.TestNotNull(Name, System)
                || !Test.TestEqual(TEXT("Exact legacy FX package, no same-name action substitution"), System->GetOutermost()->GetName(), FString(SystemsRoot) + Name)) return false;
            Systems.Emplace(System);
        }

        Character.Reset(DuplicateObject<UPadmaACTCharacterDefinition>(Live, GetTransientPackage()));
        Profile.Reset(DuplicateObject<UPadmaACTMeleeDefinition>(Equipment, GetTransientPackage()));
        Slots.Reset(NewObject<UDataTable>(GetTransientPackage(), NAME_None, RF_Transient));
        Slots->RowStruct = FPadmaACTSkillRow::StaticStruct();
        Montage.Reset(UAnimMontage::CreateSlotAnimationAsDynamicMontage(Sequence, TEXT("DefaultSlot"), .03f, .08f));
        if (!Test.TestNotNull(TEXT("Transient legacy Montage"), Montage.Get())) return false;
        for (UObject* Object : {static_cast<UObject*>(Character.Get()), static_cast<UObject*>(Profile.Get()), static_cast<UObject*>(Montage.Get())})
        {
            Object->ClearFlags(RF_Public | RF_Standalone);
            Object->SetFlags(RF_Transient);
        }
        Character->bUseCharacterActions = false;
        Character->CharacterClass.Reset();
        Character->AnimationClass.Reset();
        Character->CameraProfile.Reset();
        Character->MeleeProfile = Profile.Get();
        Character->SkillTable = Slots.Get();
        Profile->AttackMontage = Montage.Get();
        Profile->AttackSection = TEXT("Attack01");
        Profile->bWeaponLifecycle = false;
        Profile->DrawWeaponMontage.Reset();
        Profile->SheatheWeaponMontage.Reset();
        Profile->TraceRadius = 5.f;
        Profile->HitEffect = Systems[3].Get();
        for (auto& Weapon : Profile->Weapons)
        {
            Weapon.StowedSocket = NAME_None;
            Weapon.StowedTransform = FTransform::Identity;
        }
        if (!Test.TestNotNull(TEXT("Shared legacy base animation"), Profile->IdleAnimation.LoadSynchronous())) return false;

        // Snapshot contract: Artifacts/ChenQianyu/Attack01/Montage0919/{snapshot.json,author_montage.py}.
        // Shared pose/meshes/FX, but deliberately independent legacy blade/combo rules.
        // Do not copy the live Montage's volume hits, transitions or root-motion variants.
        Montage->CompositeSections.Reset();
        // AddAnimCompositeSection is editor-only in UE 5.8; the first section links the same way without it.
        FCompositeSection LegacySection;
        LegacySection.SectionName = TEXT("Attack01");
        LegacySection.Link(Montage.Get(), 0.f);
        Montage->CompositeSections.Add(LegacySection);
        Montage->Notifies.Reset();
#if WITH_EDITOR
        // RefreshCacheData requires a valid editor track even for an unsaved Montage.
        Montage->AnimNotifyTracks.Reset();
        Montage->AnimNotifyTracks.Emplace(TEXT("Legacy regression"), FLinearColor::White);
#endif
        auto Event = [&](float Start, UAnimNotify* Notify, UAnimNotifyState* State, float Duration = 0.f)
        {
            auto& Entry = Montage->Notifies.AddDefaulted_GetRef();
            Entry.Link(Montage.Get(), Start);
#if WITH_EDITOR
            Entry.TriggerTimeOffset = GetTriggerTimeOffsetForType(Montage->CalculateOffsetForNotify(Start));
#endif
            Entry.Notify = Notify;
            Entry.NotifyStateClass = State;
            Entry.TrackIndex = 0;
#if WITH_EDITORONLY_DATA
            Entry.Guid = FGuid::NewGuid();
#endif
            if (State)
            {
                Entry.SetDuration(Duration);
                Entry.EndLink.Link(Montage.Get(), Start + Duration);
            }
        };
        for (int32 Index = 0; Index < 3; ++Index)
        {
            for (bool Stop : {false, true})
            {
                auto* Notify = NewObject<UAnimNotify_PadmaACTFX>(Montage.Get(), NAME_None, RF_Transient);
                Notify->Effect.System = Systems[Index].Get();
                Notify->Effect.WeaponIndex = -1;
                Notify->Effect.bFollowMount = false;
                Notify->Effect.RelativeTransform = FTransform::Identity;
                Notify->bStop = Stop;
                Event(Stop ? (Index == 2 ? 53.f : 50.f) / 30.f : (Index == 2 ? 10.f : 7.f) / 30.f, Notify, nullptr);
            }
        }
        for (int32 Weapon = 0; Weapon < 2; ++Weapon)
        {
            auto* Hit = NewObject<UAnimNotifyState_PadmaACTHitWindow>(Montage.Get(), NAME_None, RF_Transient);
            Hit->Window.WeaponIndex = Weapon;
            Hit->Window.DamageMultiplier = .2f;
            Hit->Window.HitSocket = TEXT("VB_Hit");
            Hit->Window.bFaceTargetPlanar = true;
            Hit->Window.HitEffect = Systems[3 + Weapon].Get();
            Event((Weapon == 0 ? 19.f : 11.f) / 60.f, nullptr, Hit, .1f);
        }
        Event(Montage->GetPlayLength() * .3f, nullptr,
            NewObject<UAnimNotifyState_PadmaACTComboWindow>(Montage.Get(), NAME_None, RF_Transient), Montage->GetPlayLength() * .6f);
        Montage->RefreshCacheData();
        return true;
    }
};
}
