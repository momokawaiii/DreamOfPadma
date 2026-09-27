#include "Gameplay/ACT/Runtime/PadmaACTMeleeLab.h"
#include "Gameplay/ACT/Runtime/PadmaACTActions.h"
#include "Gameplay/ACT/Runtime/PadmaACTCamera.h"
#include "Gameplay/ACT/Runtime/PadmaACTMelee.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Game/Framework/PadmaPlayerController.h"
#include "UI/Screens/PadmaACTHologramComponent.h"
#include "UI/Screens/PadmaACTTrainingAnalyticsWidget.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void APadmaACTMeleeLab::PlayShowcaseClip(UAnimSequence* Clip, bool bLoop, float Rate)
{
    auto* Unit=ShowcaseUnit.Get();
    auto* Anim=Unit ? Unit->GetMesh()->GetAnimInstance() : nullptr;
    if (!Anim || !Clip) return;
    if (ShowcaseMontage) Anim->Montage_Stop(.25f,ShowcaseMontage);
    ShowcaseMontage=UAnimMontage::CreateSlotAnimationAsDynamicMontage(Clip,TEXT("DefaultSlot"),.25f,.3f,Rate,bLoop ? 100000 : 1);
    // Entry holds its terminal pose until the loop takes ownership. Auto blend-out
    // here exposed the locomotion pose before our transition, producing a snap.
    if (ShowcaseMontage)
    {
        ShowcaseMontage->bEnableAutoBlendOut=false;
        Anim->Montage_Play(ShowcaseMontage,Rate);
    }
}

bool APadmaACTMeleeLab::BeginShowcase()
{
    auto* Unit=Battle->GetPlayerUnit();
    LoadedShowcaseEnter=ShowcaseEnter.LoadSynchronous();
    LoadedShowcaseLoop=ShowcaseLoop.LoadSynchronous();
    LoadedShowcaseExit=ShowcaseExit.LoadSynchronous();
    LoadedShowcaseRestLoop=ShowcaseRestLoop.LoadSynchronous();
    if (!Unit || !Unit->IsAlive() || !LoadedShowcaseEnter || !LoadedShowcaseLoop || !LoadedShowcaseExit || !LoadedShowcaseRestLoop) return false;
    ReleaseShowcaseRestPose();
    if (!bShowcaseActive)
    {
        ShowcaseUnit=Unit;
        Unit->GetAbilitySystemComponent()->CancelAllAbilities();
        Unit->GetMelee()->FinishAttack(true);
        Battle->SetMoveInput(FVector2D::ZeroVector);
        Unit->GetActions()->Move(FVector::ZeroVector);
        Unit->GetCharacterMovement()->StopMovementImmediately();
        bBattleWasTicking=Battle->IsComponentTickEnabled();
        bActionsWereTicking=Unit->GetActions()->IsComponentTickEnabled();
        bMovementWasTicking=Unit->GetCharacterMovement()->IsComponentTickEnabled();
        bCameraWasTicking=Unit->GetACTCamera()->IsComponentTickEnabled();
        Battle->SetComponentTickEnabled(false);
        Unit->GetActions()->SetComponentTickEnabled(false);
        Unit->GetCharacterMovement()->SetComponentTickEnabled(false);
        Unit->GetACTCamera()->SetComponentTickEnabled(false);
        ShowcaseReturnCamera=Unit->GetACTCamera()->GetComponentTransform();
        ShowcaseReturnFOV=Unit->GetACTCamera()->FieldOfView;
        ShowcaseReturnMesh=Unit->GetMesh()->GetRelativeRotation().Quaternion();
        // Keep the gameplay capsule/heading intact. Only the presentation mesh turns.
        const float Yaw=ShowcaseReturnCamera.Rotator().Yaw;
        const FQuat WorldFacing=FRotator(0,Yaw+180.f-90.f-50.f,0).Quaternion();
        ShowcaseTargetMesh=Unit->GetActorQuat().Inverse()*WorldFacing;
        const FVector Forward=FRotator(0,Yaw,0).Vector();
        const FVector Right=FRotationMatrix(FRotator(0,Yaw,0)).GetUnitAxis(EAxis::Y);
        const FVector Feet=Unit->GetActorLocation()-FVector(0,0,Unit->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
        const FVector Aim=Feet+FVector(0,0,100)+Right*72.f;
        ShowcaseTargetCamera=FTransform(FRotator(-3.f,Yaw,0),Aim-Forward*365.f+FVector(0,0,18));
        ProjectionLight->AttachToComponent(Unit->GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale,TEXT("wep_L"));
        bShowcaseActive=true;
        ShowcaseProgress=0;
    }
    ShowcaseAge=0;
    // Reopening during exit must also cancel the in-progress weapon fade.
    Unit->GetMelee()->SetShowcasePresentation(true);
    ShowcaseExitAge=0;
    bShowcaseLoop=false;
    PlayShowcaseClip(LoadedShowcaseEnter,false,1.6f);
    if (ShowcaseInteraction) ShowcaseInteraction->Activate();
    Hologram->SetVisibility(true);
    Hologram->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    if (auto* PC=UGameplayStatics::GetPlayerController(this,0)) PC->SetViewTarget(this);
    return true;
}

void APadmaACTMeleeLab::CloseShowcase()
{
    if (!bShowcaseActive) return;
    ShowcaseExitAge=0;
    ReleaseShowcasePointer();
    if (auto* Unit=ShowcaseUnit.Get()) Unit->GetMelee()->SetShowcasePresentation(false,true);
    if (ShowcaseInteraction) ShowcaseInteraction->Deactivate();
    PlayShowcaseClip(LoadedShowcaseExit,false,2.5f);
    Hologram->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APadmaACTMeleeLab::TickShowcase(float Delta)
{
    if (!bShowcaseActive) return;
    auto* Unit=ShowcaseUnit.Get();
    if (!Unit || !Unit->IsAlive()) {RestoreShowcase();return;}
    ShowcaseAge+=Delta;
    if (!bAnalyticsOpen) ShowcaseExitAge+=Delta;
    ShowcaseProgress=FMath::Clamp(ShowcaseProgress+Delta*(bAnalyticsOpen ? 1.f/.55f : -1.f/1.1f),0.f,1.f);
    const float Alpha=FMath::SmoothStep(0.f,1.f,ShowcaseProgress);
    FTransform View;View.Blend(ShowcaseReturnCamera,ShowcaseTargetCamera,Alpha);
    Camera->SetWorldTransform(View);
    Camera->SetFieldOfView(FMath::Lerp(ShowcaseReturnFOV,52.f,Alpha));
    Camera->PostProcessBlendWeight=Alpha;
    Camera->PostProcessSettings.bOverride_DepthOfFieldFstop=true;
    Camera->PostProcessSettings.DepthOfFieldFstop=2.4f;
    Camera->PostProcessSettings.bOverride_DepthOfFieldFocalDistance=true;
    Camera->PostProcessSettings.DepthOfFieldFocalDistance=365.f;
    Unit->GetMesh()->SetRelativeRotation(FQuat::Slerp(ShowcaseReturnMesh,ShowcaseTargetMesh,Alpha));
    // Local +X is the widget front normal; the panel faces the camera with a yaw tilt.
    const FTransform& Anchor=ShowcaseTargetCamera;
    const FVector PanelPosition=Anchor.TransformPosition(FVector(290,66,5-(1.f-Alpha)*35));
    // Fold inward like the right page of an open book: the edge beside the
    // character recedes, while the outer edge comes toward the viewer.
    const FQuat PanelRotation=Anchor.GetRotation()*FRotator(0,196.f,0.f).Quaternion();
    Hologram->SetWorldLocationAndRotation(PanelPosition,PanelRotation);
    const float Pop=1.f+.018f*FMath::Sin(FMath::Clamp(ShowcaseAge/.18f,0.f,1.f)*PI);
    Hologram->SetWorldScale3D(FVector(.135f*Pop));
    ProjectionLight->SetIntensity(5.f*Alpha);
    if (bAnalyticsOpen && !bShowcaseLoop && ShowcaseEnter.IsValid() && ShowcaseAge>=ShowcaseEnter->GetPlayLength()/1.6f-.18f)
    {
        PlayShowcaseClip(LoadedShowcaseLoop,true); bShowcaseLoop=true;
    }
    const float ExitDuration=LoadedShowcaseExit ? LoadedShowcaseExit->GetPlayLength()/2.5f : 1.1f;
    if (!bAnalyticsOpen && ShowcaseExitAge>=FMath::Max(1.1f,ExitDuration)) RestoreShowcase(true);
}

void APadmaACTMeleeLab::RestoreShowcase(bool bHoldExitPose)
{
    if (!bHoldExitPose) ReleaseShowcaseRestPose();
    if (!bShowcaseActive) return;
    ReleaseShowcasePointer();
    if (ShowcaseInteraction) ShowcaseInteraction->Deactivate();
    bShowcaseActive=false; bAnalyticsOpen=false;
    if (AnalyticsWidget) AnalyticsWidget->SetOpen(false);
    ProjectionLight->AttachToComponent(Camera,FAttachmentTransformRules::KeepWorldTransform);
    Hologram->SetVisibility(false);
    Hologram->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ProjectionLight->SetIntensity(0);
    Camera->PostProcessBlendWeight=0;
    Battle->SetComponentTickEnabled(bBattleWasTicking);
    if (auto* Unit=ShowcaseUnit.Get())
    {
        if (bHoldExitPose && Unit->IsAlive())
        {
            // Continue the original overview breathing cycle, never freeze the
            // exit's terminal frame or blend back to the combat idle.
            PlayShowcaseClip(LoadedShowcaseRestLoop,true);
            ShowcaseRestUnit=Unit;
            ShowcaseRestMontage=ShowcaseMontage;
        }
        else if (auto* Anim=Unit->GetMesh()->GetAnimInstance()) if (ShowcaseMontage) Anim->Montage_Stop(.25f,ShowcaseMontage);
        Unit->GetMesh()->SetRelativeRotation(ShowcaseReturnMesh);
        Unit->GetMelee()->SetShowcasePresentation(false,bHoldExitPose);
        Unit->GetActions()->SetComponentTickEnabled(bActionsWereTicking);
        Unit->GetCharacterMovement()->SetComponentTickEnabled(bMovementWasTicking);
        Unit->GetACTCamera()->SetComponentTickEnabled(bCameraWasTicking);
        if (auto* PC=UGameplayStatics::GetPlayerController(this,0))
        {
            PC->SetViewTarget(Unit);
        }
    }
    if (auto* PC=UGameplayStatics::GetPlayerController(this,0))
    {
        PC->SetInputMode(FInputModeGameOnly());PC->bShowMouseCursor=false;
        if (auto* PadmaPC=Cast<APadmaPlayerController>(PC)) PadmaPC->SetACTTrainingOverlayOpen(false);
    }
    ShowcaseMontage=nullptr;ShowcaseUnit.Reset();
}

void APadmaACTMeleeLab::ReleaseShowcaseRestPose()
{
    if (auto* Unit=ShowcaseRestUnit.Get())
    {
        Unit->GetMelee()->ReleaseShowcaseRestWeapons();
        if (auto* Anim=Unit->GetMesh()->GetAnimInstance())
            if (ShowcaseRestMontage) Anim->Montage_Stop(.2f,ShowcaseRestMontage);
    }
    ShowcaseRestMontage=nullptr;
    ShowcaseRestUnit.Reset();
}

void APadmaACTMeleeLab::TickShowcaseRestPose()
{
    if (!ShowcaseRestMontage) return;
    auto* Unit=ShowcaseRestUnit.Get();
    if (!Unit || !Unit->IsAlive() || !Battle->IsBattleActive()
        || Unit->GetActions()->HasLocomotionInput() || Unit->GetActions()->GetActiveDefinition()
        || Unit->GetCharacterMovement()->IsFalling() || !Unit->GetVelocity().IsNearlyZero(1.f))
        ReleaseShowcaseRestPose();
}

#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaACTShowcaseTest,"DreamOfPadma.ACT.ChenTrainingShowcase",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaACTShowcaseTest::RunTest(const FString&)
{
    UWorld::InitializationValues Values;
    Values.AllowAudioPlayback(false).CreatePhysicsScene(true).ShouldSimulatePhysics(false).CreateNavigation(false).CreateAISystem(false);
    UWorld* World=UWorld::CreateWorld(EWorldType::Game,false,NAME_None,nullptr,true,ERHIFeatureLevel::Num,&Values);
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT { World->DestroyWorld(false);GEngine->DestroyWorldContext(World); };
    auto* Lab=World->SpawnActor<APadmaACTMeleeLab>();
    Lab->CharacterDefinition=LoadObject<UPadmaACTCharacterDefinition>(nullptr,TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/AbilitySystem/DA_ACTCharacter_CHEN.DA_ACTCharacter_CHEN"));
    Lab->ResetBattle();
    auto* Unit=Lab->Battle->GetPlayerUnit();
    if (!TestNotNull(TEXT("Real Chen battle starts"),Unit)) return false;
    const auto Rotation=Unit->GetMesh()->GetRelativeRotation();
    const bool BattleTick=Lab->Battle->IsComponentTickEnabled();
    TestTrue(TEXT("Original three showcase clips resolve"),Lab->BeginShowcase());
    TestNotNull(TEXT("Actual Chen anim instance plays showcase montage"),Lab->ShowcaseMontage.Get());
    if (Lab->ShowcaseMontage) TestFalse(TEXT("Entry cannot blend to idle before loop"),Lab->ShowcaseMontage->bEnableAutoBlendOut);
    if (auto* Anim=Unit->GetMesh()->GetAnimInstance())
        if (auto* Instance=Anim->GetActiveInstanceForMontage(Lab->ShowcaseMontage))
            TestFalse(TEXT("Playing instance disables automatic blend-out"),Instance->bEnableAutoBlendOut);
    auto* MainWeapon=Unit->GetMelee()->GetWeapon(0);
    auto* Offhand=Unit->GetMelee()->GetWeapon(1);
    if (TestNotNull(TEXT("Real main sword"),MainWeapon)) TestTrue(TEXT("Showcase main sword visible"),MainWeapon->IsVisible());
    if (TestNotNull(TEXT("Real offhand sword"),Offhand)) TestFalse(TEXT("Showcase hides offhand sword"),Offhand->IsVisible());
    Lab->bAnalyticsOpen=true;
    TestFalse(TEXT("Battle clock pauses"),Lab->Battle->IsComponentTickEnabled());
    TestFalse(TEXT("Lab direct LMB cannot attack through menu"),Lab->Fire());
    Lab->TickShowcase(.55f);
    Lab->bAnalyticsOpen=false;Lab->CloseShowcase();Lab->TickShowcase(.4f);
    const FTransform PartialCamera=Lab->Camera->GetComponentTransform();
    Lab->BeginShowcase();Lab->bAnalyticsOpen=true;Lab->TickShowcase(0);
    TestTrue(TEXT("Reopen preserves camera continuity"),PartialCamera.Equals(Lab->Camera->GetComponentTransform(),.001f));
    TestFalse(TEXT("Reopen does not skip entry clip"),Lab->bShowcaseLoop);
    Lab->RestoreShowcase();
    if (MainWeapon) TestFalse(TEXT("Exit leaves no main sword flash"),MainWeapon->IsVisible());
    if (Offhand) TestFalse(TEXT("Exit leaves no offhand sword flash"),Offhand->IsVisible());
    TestEqual(TEXT("Battle ticking restored"),Lab->Battle->IsComponentTickEnabled(),BattleTick);
    TestTrue(TEXT("Mesh rotation restored"),Unit->GetMesh()->GetRelativeRotation().Equals(Rotation,.001f));
    TestFalse(TEXT("Restore clears open state"),Lab->IsAnalyticsOpen());
    Lab->BeginShowcase();Lab->bAnalyticsOpen=true;
    Lab->TickShowcase(.55f);
    Lab->bAnalyticsOpen=false;Lab->CloseShowcase();
    auto* ExitMontage=Lab->ShowcaseMontage.Get();
    const FName FadeSocket=MainWeapon->GetAttachSocketName();
    TestTrue(TEXT("Breathing transition keeps sword visible for dissolve"),MainWeapon->IsVisible());
    static_cast<UActorComponent*>(Unit->GetMelee())->TickComponent(.1f,LEVELTICK_All,nullptr);
    TestTrue(TEXT("Dissolve keeps current hand socket"),MainWeapon->GetAttachSocketName()==FadeSocket);
    TestTrue(TEXT("Mid dissolve keeps sword rendered"),MainWeapon->IsVisible());
    if (auto* Material=Cast<UMaterialInstanceDynamic>(MainWeapon->GetMaterial(0)))
    {
        const float Fade=Material->K2_GetScalarParameterValue(TEXT("WeaponDissolve"));
        TestTrue(TEXT("Weapon starts fading on close before exit animation completes"),Fade>0.f && Fade<1.f);
    }
    else AddError(TEXT("Showcase sword needs a dissolve material instance"));
    TestFalse(TEXT("Dissolve never resurrects offhand"),Offhand->IsVisible());
    static_cast<UActorComponent*>(Unit->GetMelee())->TickComponent(2.f,LEVELTICK_All,nullptr);
    TestFalse(TEXT("Completed dissolve hides sword"),MainWeapon->IsVisible());
    Lab->TickShowcase(1.2f);
    TestTrue(TEXT("Sheathed sword appears after held sword is gone"),Offhand->IsVisible());
    auto* SheathedMaterial=Cast<UMaterialInstanceDynamic>(Offhand->GetMaterial(0));
    if (TestNotNull(TEXT("Sheathed sword has dissolve material"),SheathedMaterial))
    {
        TestEqual(TEXT("Sheathed sword begins fully dissolved"),SheathedMaterial->K2_GetScalarParameterValue(TEXT("WeaponDissolve")),1.f);
        static_cast<UActorComponent*>(Unit->GetMelee())->TickComponent(.25f,LEVELTICK_All,nullptr);
        const float Reveal=SheathedMaterial->K2_GetScalarParameterValue(TEXT("WeaponDissolve"));
        TestTrue(TEXT("Sheathed blade grows through reverse dissolve"),Reveal>0.f && Reveal<1.f);
        TestFalse(TEXT("Reverse dissolve never reveals the main sword"),MainWeapon->IsVisible());
    }
    TestEqual(TEXT("Sheathed sword uses source inner mount"),Offhand->GetAttachSocketName(),FName(TEXT("inner")));
    auto* Scabbard=Unit->GetMelee()->GetWeapon(2);
    if (TestNotNull(TEXT("Scabbard exists"),Scabbard)) TestTrue(TEXT("Scabbard stays visible"),Scabbard->IsVisible());
    static_cast<UActorComponent*>(Unit->GetMelee())->TickComponent(10.f,LEVELTICK_All,nullptr);
    TestTrue(TEXT("Breathing loop retains sheathed sword indefinitely"),Offhand->IsVisible());
    if (SheathedMaterial) TestEqual(TEXT("Reverse dissolve completes fully visible"),SheathedMaterial->K2_GetScalarParameterValue(TEXT("WeaponDissolve")),0.f);
    TestFalse(TEXT("Breathing loop never duplicates the main sword"),MainWeapon->IsVisible());
    TestFalse(TEXT("Normal close releases camera and input"),Lab->bShowcaseActive);
    auto* RestMontage=Lab->ShowcaseRestMontage.Get();
    TestNotNull(TEXT("Normal close plays breathing loop"),RestMontage);
    TestTrue(TEXT("Breathing replaces frozen exit montage"),RestMontage && RestMontage!=ExitMontage);
    if (RestMontage)
    {
        const auto& Segments=RestMontage->SlotAnimTracks[0].AnimTrack.AnimSegments;
        TestTrue(TEXT("Rest uses original overview loop repeatedly"),Segments.Num()==1 && Segments[0].GetAnimReference()==Lab->LoadedShowcaseRestLoop && Segments[0].LoopingCount>1);
        auto* Anim=Unit->GetMesh()->GetAnimInstance();
        if (TestNotNull(TEXT("Breathing anim instance"),Anim))
        {
            const float Before=Anim->Montage_GetPosition(RestMontage);
            Unit->GetMesh()->TickAnimation(.5f,false);
            TestTrue(TEXT("Breathing playback advances while stationary"),Anim->Montage_GetPosition(RestMontage)>Before+.1f);
        }
    }
    Lab->TickShowcaseRestPose();
    TestTrue(TEXT("Standing still retains breathing loop"),Lab->ShowcaseRestMontage==RestMontage);
    Unit->GetActions()->Move(FVector::ForwardVector);
    Lab->TickShowcaseRestPose();
    TestNull(TEXT("Movement releases held pose"),Lab->ShowcaseRestMontage.Get());
    Unit->GetActions()->Move(FVector::ZeroVector);
    Lab->BeginShowcase();Lab->bAnalyticsOpen=true;Lab->ResetBattle();
    TestNull(TEXT("Reset clears held exit pose"),Lab->ShowcaseRestMontage.Get());
    TestFalse(TEXT("F8 releases showcase"),Lab->bShowcaseActive);
    TestFalse(TEXT("F8 clears menu state"),Lab->IsAnalyticsOpen());
    Lab->Battle->ExitBattle();
    return true;
}
#endif

void APadmaACTMeleeLab::AttackInput()
{
    if (bAnalyticsOpen && ShowcaseInteraction)
    {
        ShowcaseInteraction->TickComponent(0,LEVELTICK_All,nullptr);
        ShowcaseInteraction->PressPointerKey(EKeys::LeftMouseButton);
        return;
    }
    Fire();
}
void APadmaACTMeleeLab::ReleaseShowcasePointer()
{
    if (ShowcaseInteraction) ShowcaseInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
}
void APadmaACTMeleeLab::ScrollShowcaseUp()
{
    if (bAnalyticsOpen && ShowcaseInteraction) ShowcaseInteraction->ScrollWheel(1.f);
}
void APadmaACTMeleeLab::ScrollShowcaseDown()
{
    if (bAnalyticsOpen && ShowcaseInteraction) ShowcaseInteraction->ScrollWheel(-1.f);
}
