#if WITH_EDITOR
#include "HAL/IConsoleManager.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Gameplay/ACT/Runtime/PadmaACTAnimNotifies.h"

static FAutoConsoleCommand PadmaChenWeaponMontages(
    TEXT("Padma.ChenActions.ConfigureWeaponMontages"), TEXT("Configure the two owned weapon presentation montages; caller saves."),
    FConsoleCommandDelegate::CreateLambda([]()
    {
        for (const auto* Name : {TEXT("DrawWeapon"),TEXT("SheatheWeapon")})
        {
            auto* M=LoadObject<UAnimMontage>(nullptr,*FString::Printf(TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Montages/AM_Chen_%s"),Name));
            if (!M) continue;
            M->Modify(); M->CompositeSections.Reset(); M->AddAnimCompositeSection(TEXT("Presentation"),0);
            M->BlendIn.SetBlendTime(.1f); M->BlendOut.SetBlendTime(.12f);
            M->MarkPackageDirty();
        }
    }));

// Source Ultimate animation overrides are on the same 30 Hz action clock as FX
// and damage. Keep notify times intact; do not move FX to the unretimed clip.
static FAutoConsoleCommand PadmaChenRetimeUltimate(
    TEXT("Padma.ChenActions.RetimeUltimate"), TEXT("Restore R source animation segments; caller saves."),
    FConsoleCommandDelegate::CreateLambda([]()
    {
        auto* M=LoadObject<UAnimMontage>(nullptr,TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Montages/AM_Chen_LieFengShuang"));
        if (!M || M->SlotAnimTracks.Num()!=1 || M->SlotAnimTracks[0].AnimTrack.AnimSegments.IsEmpty()) return;
        auto* A=Cast<UAnimSequence>(M->SlotAnimTracks[0].AnimTrack.AnimSegments[0].GetAnimReference());
        if (!A || !FMath::IsNearlyEqual(A->GetPlayLength(),10.f,.001f)) return;
        M->Modify();
        auto& Segments=M->SlotAnimTracks[0].AnimTrack.AnimSegments;
        Segments.Reset();
        auto Add=[&](float Timeline,float Start,float End,float Rate)
        {
            FAnimSegment S; S.SetAnimReference(A); S.StartPos=Timeline;
            S.AnimStartTime=Start; S.AnimEndTime=End; S.AnimPlayRate=Rate; S.LoopingCount=1;
            Segments.Add(S);
        };
        // The existing clip continues through the source's hidden interval until
        // the first explicit override at frame 77. Visibility itself is separate.
        Add(0,0,77.f/30,1);
        Add(77.f/30,122.f/30,133.f/30,.5f);
        Add(99.f/30,133.f/30,139.f/30,1.5f);
        Add(103.f/30,139.f/30,A->GetPlayLength(),1);
        M->SetCompositeLength(264.f/30);
        for (auto& N:M->Notifies)
            if (N.NotifyStateClass && N.GetEndTriggerTime()>M->GetPlayLength())
                N.SetDuration(FMath::Max(.001f,M->GetPlayLength()-.001f-N.GetTriggerTime()));
        M->RefreshCacheData(); M->MarkPackageDirty();
        UE_LOG(LogTemp,Display,TEXT("Chen R source segments restored: final slash clip 278 at action 206 (60 Hz)"));
    }));

static FAutoConsoleCommand PadmaChenResetRules(
    TEXT("Padma.ChenActions.ResetRules"), TEXT("Replace owned action gameplay notifies, preserving FX and sections; caller saves."),
    FConsoleCommandDelegate::CreateLambda([]()
    {
        const TCHAR* Names[]={TEXT("Attack01"),TEXT("Attack02"),TEXT("Attack03"),TEXT("Attack04"),TEXT("Attack05"),TEXT("GuiQiongYu"),TEXT("JianTianHe"),TEXT("LieFengShuang"),TEXT("Execution"),TEXT("Dodge"),TEXT("PerfectDodge"),TEXT("Plunge"),TEXT("Jump")};
        for (const auto* Name:Names)
        {
            auto* M=LoadObject<UAnimMontage>(nullptr,*FString::Printf(TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Montages/AM_Chen_%s"),Name));
            if (!M) continue;
            M->Modify();
            M->Notifies.RemoveAll([](const FAnimNotifyEvent& N){return Cast<UAnimNotifyState_PadmaACTHitWindow>(N.NotifyStateClass)
                || Cast<UAnimNotifyState_PadmaACTComboWindow>(N.NotifyStateClass) || Cast<UAnimNotifyState_PadmaACTTransitionWindow>(N.NotifyStateClass);});
            M->RefreshCacheData(); M->MarkPackageDirty();
        }
    }));

// CompositeSections is not Python-exposed in UE 5.8. Only this research asset is writable here.
static FAutoConsoleCommand PadmaChenConfigureMontage(
    TEXT("Padma.ChenFX.ConfigureMontage"), TEXT("Configure the authored Attack01 section and short blends in memory; Python saves the asset."),
    FConsoleCommandDelegate::CreateLambda([]()
    {
        auto* Montage=LoadObject<UAnimMontage>(nullptr,TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Montages/AM_Chen_Attack01.AM_Chen_Attack01"));
        if (!Montage) return;
        Montage->Modify();
        Montage->CompositeSections.Reset();
        Montage->AddAnimCompositeSection(TEXT("Attack01"),0.f);
        Montage->BlendIn.SetBlendTime(.03f);
        Montage->BlendOut.SetBlendTime(.08f);
        Montage->MarkPackageDirty();
    }));
static FAutoConsoleCommand PadmaChenConfigureActions(
    TEXT("Padma.ChenActions.ConfigureMontages"), TEXT("Configure new character action montages in memory; caller saves."),
    FConsoleCommandDelegate::CreateLambda([]()
    {
        const TCHAR* Names[]={TEXT("Attack02"),TEXT("Attack03"),TEXT("Attack04"),TEXT("Attack05"),TEXT("GuiQiongYu"),TEXT("JianTianHe"),TEXT("LieFengShuang"),TEXT("Execution"),TEXT("Dodge"),TEXT("PerfectDodge"),TEXT("Plunge"),TEXT("Jump")};
        for (const auto* Name:Names)
        {
            const FString Path=FString::Printf(TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Montages/AM_Chen_%s"),Name);
            auto* M=LoadObject<UAnimMontage>(nullptr,*Path); if (!M) continue;
            M->Modify(); M->CompositeSections.Reset();
            M->BlendIn.SetBlendTime(.08f); M->BlendOut.SetBlendTime(.12f);
            if (FString(Name)==TEXT("Plunge") || FString(Name)==TEXT("Jump"))
            {
                M->SlotAnimTracks[0].AnimTrack.AnimSegments.Reset();
                float Time=0;
                const bool Jump=FString(Name)==TEXT("Jump");
                const TCHAR* Clips[]={Jump ? TEXT("JumpStartL") : TEXT("PlungeStart"),Jump ? TEXT("FallL") : TEXT("PlungeLoop"),Jump ? TEXT("JumpLandL") : TEXT("PlungeEnd")};
                const FName Sections[]={TEXT("Start"),TEXT("Loop"),TEXT("Land")};
                for (int I=0;I<3;++I)
                {
                    auto* A=LoadObject<UAnimSequence>(nullptr,*FString::Printf(TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Sequences/Imported/AS_Chen_%s_CM"),Clips[I]));
                    if (!A) continue;
                    FAnimSegment Segment; Segment.SetAnimReference(A); Segment.AnimStartTime=0; Segment.AnimEndTime=A->GetPlayLength(); Segment.AnimPlayRate=1; Segment.LoopingCount=1; Segment.StartPos=Time;
                    M->SlotAnimTracks[0].AnimTrack.AnimSegments.Add(Segment);
                    M->AddAnimCompositeSection(Sections[I],Time); Time+=A->GetPlayLength();
                }
                M->SetCompositeLength(Time);
                M->CompositeSections[0].NextSectionName=TEXT("Loop"); M->CompositeSections[1].NextSectionName=TEXT("Loop"); M->CompositeSections[2].NextSectionName=NAME_None;
            }
            else M->AddAnimCompositeSection(TEXT("Action"),0);
            M->MarkPackageDirty();
        }
    }));
#endif
