#include "Gameplay/ACT/Runtime/PadmaACTMeleeLab.h"

#include "Gameplay/ACT/Runtime/PadmaACTMelee.h"
#include "Gameplay/ACT/Runtime/PadmaACTActions.h"
#include "Gameplay/ACT/Runtime/PadmaACTCamera.h"
#include "Gameplay/ACT/Runtime/PadmaACTTrainingAnalytics.h"
#include "Gameplay/ACT/Runtime/PadmaACTTrainingDummyPresentation.h"
#include "Game/Framework/PadmaPlayerController.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Gameplay/ACT/Authoring/PadmaACTAuthoring.h"
#include "UI/Screens/PadmaACTTrainingAnalyticsWidget.h"
#include "UI/Screens/PadmaACTHologramComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "World/Combat/PadmaCombatFeedback.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/Engine.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"   // [CHANGED] FindLookAtRotation

APadmaACTMeleeLab::APadmaACTMeleeLab()
{
    PrimaryActorTick.bCanEverTick = true;

    // [CHANGED] 相机直接作为 RootComponent，这样 SetViewTarget(this) 会使用相机 Transform
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    SetRootComponent(Camera);
    Hologram=CreateDefaultSubobject<UPadmaACTHologramComponent>(TEXT("TrainingHologram"));
    Hologram->SetupAttachment(Camera); Hologram->SetVisibility(false);
    ProjectionLight=CreateDefaultSubobject<UPointLightComponent>(TEXT("ProjectionSource"));
    ProjectionLight->SetupAttachment(Camera); ProjectionLight->SetIntensity(0);
    ProjectionLight->SetLightColor(FLinearColor(.08f,.8f,1.f));
    ProjectionLight->SetAttenuationRadius(60); ProjectionLight->SetCastShadows(false);
    ShowcaseEnter=TSoftObjectPtr<UAnimSequence>(FSoftObjectPath(TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Sequences/Imported/AS_Chen_ShowcaseEnter_CM.AS_Chen_ShowcaseEnter_CM")));
    ShowcaseLoop=TSoftObjectPtr<UAnimSequence>(FSoftObjectPath(TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Sequences/Imported/AS_Chen_ShowcaseLoop_CM.AS_Chen_ShowcaseLoop_CM")));
    ShowcaseExit=TSoftObjectPtr<UAnimSequence>(FSoftObjectPath(TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Sequences/Imported/AS_Chen_ShowcaseExit_CM.AS_Chen_ShowcaseExit_CM")));
    ShowcaseRestLoop=TSoftObjectPtr<UAnimSequence>(FSoftObjectPath(TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Sequences/Imported/AS_Chen_ShowcaseRestLoop_CM.AS_Chen_ShowcaseRestLoop_CM")));

    Camera->SetRelativeLocation(FVector(400.f, -540.f, 360.f));

    // [CHANGED] 看向 Actor 原点（零向量），而不是 FVector(50,0,95)
    Camera->SetRelativeRotation(
        UKismetMathLibrary::FindLookAtRotation(
            Camera->GetRelativeLocation(), FVector::ZeroVector));

    Camera->FieldOfView = 48.f;

    // Battle 组件保持为非根组件
    Battle = CreateDefaultSubobject<UPadmaCombatComponent>(TEXT("ACTBattle"));
    Analytics = CreateDefaultSubobject<UPadmaACTTrainingAnalyticsComponent>(TEXT("ACTTrainingAnalytics"));
    EnsureDefaultTrainingTargets();
}

void APadmaACTMeleeLab::EnsureDefaultTrainingTargets()
{
    if (!TrainingTargets.IsEmpty()) return;
    FPadmaACTTrainingTargetProfile Armor;
    Armor.Id = TEXT("blade-dummy");
    Armor.DisplayName = FText::FromString(TEXT("ARMOR TEST // HIGH DEF"));
    Armor.LocationOffset = FVector(220.f, 0.f, 0.f);
    Armor.MaxHealth = 1500.f;
    Armor.Defense = 30.f;
    Armor.MagicDefense = 10.f;
    Armor.Attribute = TEXT("D");
    Armor.Accent = FLinearColor(1.f, .42f, .06f, 1.f);
    TrainingTargets.Add(Armor);

    FPadmaACTTrainingTargetProfile Vitality;
    Vitality.Id = TEXT("blade-dummy-2");
    Vitality.DisplayName = FText::FromString(TEXT("VITALITY TEST // HIGH HP"));
    Vitality.LocationOffset = SecondDummyOffset;
    Vitality.MaxHealth = 30000.f;
    Vitality.Defense = 3.f;
    Vitality.MagicDefense = 12.f;
    Vitality.Attribute = TEXT("F");
    Vitality.Accent = FLinearColor(.12f, .72f, .84f, 1.f);
    TrainingTargets.Add(Vitality);

    FPadmaACTTrainingTargetProfile Arcane;
    Arcane.Id = TEXT("blade-dummy-3");
    Arcane.DisplayName = FText::FromString(TEXT("ARCANE TEST // HIGH M.RES"));
    Arcane.LocationOffset = FVector(360.f, -220.f, 0.f);
    Arcane.MaxHealth = 30000.f;
    Arcane.Defense = 10.f;
    Arcane.MagicDefense = 80.f;
    Arcane.Attribute = TEXT("R");
    Arcane.Accent = FLinearColor(1.f, .82f, .18f, 1.f);
    TrainingTargets.Add(Arcane);
}

void APadmaACTMeleeLab::BeginPlay()
{
    Super::BeginPlay();
    if (IsValid(ScenePlayer)) CharacterDefinition=ScenePlayer->SceneSpec.Presentation.ACTDefinition;

    if (auto* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (APawn* Pawn = PC->GetPawn())
        {
            PC->UnPossess();
            if (Pawn != ScenePlayer && !SceneTargets.Contains(Cast<APadmaCombatUnit>(Pawn))) Pawn->Destroy();
        }

        // [CHANGED] this 的 Root 现在是 Camera，SetViewTarget 会直接采用相机的 Transform
        // 若要平滑过渡可换成：PC->SetViewTargetWithBlend(this, 0.5f);
        PC->SetViewTarget(this);
        UE_LOG(LogTemp, Warning, TEXT("ViewTarget = %s, Root = %s"),
            *GetNameSafe(PC->GetViewTarget()),
            *GetNameSafe(GetRootComponent()));
        PC->bShowMouseCursor = false;
        PC->SetInputMode(FInputModeGameOnly());

        EnableInput(PC);
        InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &APadmaACTMeleeLab::AttackInput);
        InputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &APadmaACTMeleeLab::ReleaseShowcasePointer);
        InputComponent->BindKey(EKeys::MouseScrollUp, IE_Pressed, this, &APadmaACTMeleeLab::ScrollShowcaseUp);
        InputComponent->BindKey(EKeys::MouseScrollDown, IE_Pressed, this, &APadmaACTMeleeLab::ScrollShowcaseDown);
        ShowcaseInteraction=NewObject<UWidgetInteractionComponent>(this);
        AddInstanceComponent(ShowcaseInteraction);
        ShowcaseInteraction->InteractionSource=EWidgetInteractionSource::Mouse;
        ShowcaseInteraction->InteractionDistance=2000;
        ShowcaseInteraction->RegisterComponent();
        ShowcaseInteraction->Deactivate();
        const auto* D=CharacterDefinition.LoadSynchronous();
        InputComponent->BindKey(D && D->bUseCharacterActions ? EKeys::F8 : EKeys::R, IE_Pressed, this, &APadmaACTMeleeLab::ResetBattle);
        InputComponent->BindKey(EKeys::T, IE_Pressed, this, &APadmaACTMeleeLab::ToggleTarget);
        InputComponent->BindKey(EKeys::C, IE_Pressed, this, &APadmaACTMeleeLab::CancelAttack);
        InputComponent->BindKey(EKeys::B, IE_Pressed, this, &APadmaACTMeleeLab::ToggleAnalytics);

        Analytics->BindBattle(Battle);
        Battle->OnReceipt.AddUObject(this, &APadmaACTMeleeLab::HandleReceipt);
        AnalyticsWidget = CreateWidget<UPadmaACTTrainingAnalyticsWidget>(PC, UPadmaACTTrainingAnalyticsWidget::StaticClass());
        if (AnalyticsWidget)
        {
            AnalyticsWidget->Configure(Analytics, Battle, [this]() { SetAnalyticsOpen(false); });
            AnalyticsWidget->SetWorldPresentation(true);
            Hologram->SetWidget(AnalyticsWidget);
            AnalyticsWidget->SetOpen(false);
        }
    }

    ResetBattle();
}

void APadmaACTMeleeLab::ResetBattle()
{
    RestoreShowcase();
    SetAnalyticsOpen(false);
    DestroyFeedbackActors();
    RestoreActionCamera();
    Battle->ExitBattle();
    // Recorder lifetime follows the training session, never the camera tick.
    if (Analytics) Analytics->Reset();
    EnsureDefaultTrainingTargets();
    if (bRequireSceneParticipants || ScenePlayer || !SceneTargets.IsEmpty())
    {
        StartSceneBattle();
        return;
    }

    FPadmaCombatSetup Setup;
    Setup.Mode = EPadmaCombatMode::ACT;
    Setup.ArenaMinimum = FVector2D(-500.f, -500.f);
    Setup.ArenaMaximum = FVector2D(500.f, 500.f);

    FPadmaCombatUnitSpec Player;
    Player.Id = TEXT("chen-research");
    Player.DefinitionId = TEXT("chen-attack01-research");
    Player.DisplayName = FText::FromString(TEXT("Chen Qianyu"));
    Player.Health = Player.MaxHealth = 1000;
    Player.Attack = 20;
    Player.Speed = 160;
    Player.AttackRange = 180;

    // [CHANGED] 不再用 GetActorLocation()（现在等于相机位置），改用固定场地中心
    Player.Location = FVector(0.f, 0.f, ArenaCenterZ);

    Player.Presentation.ACTDefinition = CharacterDefinition;
    Setup.Units.Add(Player);

    const auto* D=CharacterDefinition.LoadSynchronous();
    if (D && D->bUseCharacterActions)
    {
        for (int32 Index = 0; Index < TrainingTargets.Num(); ++Index)
        {
            const auto& Profile = TrainingTargets[Index];
            auto Enemy = Player;
            Enemy.Id = Profile.Id;
            Enemy.DefinitionId = Profile.Id;
            Enemy.bPlayer = false;
            Enemy.Presentation = {};
            Enemy.Presentation.Model = TrainingDummyMesh;
            Enemy.DisplayName = Profile.DisplayName;
            Enemy.Attack = 5;
            Enemy.Health = Enemy.MaxHealth = FMath::Max(1.f, Profile.MaxHealth);
            Enemy.Defense = FMath::Max(0.f, Profile.Defense);
            Enemy.MagicDefense = FMath::Max(0.f, Profile.MagicDefense);
            Enemy.Attribute = Profile.Attribute;
            Enemy.bExecutionImmune = Profile.bExecutionImmune;
            Enemy.bCanPursueInACT = bDummyFollowsPlayer;
            FVector Offset = Profile.LocationOffset;
            if (Index == 0 && !bTargetNear) Offset.X = 400.f;
            Enemy.Location = Player.Location + Offset;
            Setup.Units.Add(Enemy);
        }
    }
    else
    {
        auto Enemy = Player;
        Enemy.Id = TEXT("blade-dummy");
        Enemy.DefinitionId = TEXT("research-dummy");
        Enemy.bPlayer = false;
        Enemy.Presentation = {};
        Enemy.Presentation.Model = TrainingDummyMesh;
        Enemy.DisplayName = FText::FromString(TEXT("Contact dummy"));
        Enemy.Attack = 0;
        Enemy.Health = Enemy.MaxHealth = FMath::Max(1.f, DummyMaxHealth);
        Enemy.bCanPursueInACT = bDummyFollowsPlayer;
        Enemy.Location = Player.Location + FVector(bTargetNear ? 220.f : 400.f, 0.f, 0.f);
        Setup.Units.Add(Enemy);
    }

    Battle->PayCost = [](float, float, FName, FString&) { return true; };

    FString Failure;
    if (!Battle->StartBattle(Setup, Failure))
    {
        UE_LOG(LogTemp, Error, TEXT("Melee lab: %s"), *Failure);
        return;
    }

    // Stationary dummy and manual movement share this battle's command/settlement path.
    Battle->SetComponentTickEnabled(D && D->bUseCharacterActions);

    for (APadmaCombatUnit* Unit : Battle->GetUnits())
    {
        Unit->GetCapsuleComponent()->SetCapsuleSize(22.f, 88.f);
    }

    for (APadmaCombatUnit* Dummy : Battle->GetUnits())
    {
        if (Dummy->Spec.bPlayer) continue;
        if (Dummy->GetMesh()->GetSkeletalMeshAsset())
        {
            Dummy->GetMesh()->SetRelativeTransform(TrainingDummyMeshTransform);
            auto* Presentation = NewObject<UPadmaACTTrainingDummyPresentation>(Dummy);
            Dummy->AddInstanceComponent(Presentation);
            Presentation->RegisterComponent();
            if (!Presentation->Configure(Battle, TrainingDummyHitAnimation.LoadSynchronous()))
                UE_LOG(LogTemp, Warning, TEXT("Training dummy hit animation is missing or incompatible: %s"), *Dummy->Spec.Id.ToString());
            continue;
        }
        if (auto* Visual = Dummy->FindComponentByClass<UStaticMeshComponent>())
        {
            Visual->SetRelativeLocation(FVector::ZeroVector);
            Visual->SetRelativeScale3D(FVector(0.44f, 0.44f, 1.76f));
        }
    }

    if (auto* PlayerUnit = Battle->GetPlayerUnit())
    {
        PlayerUnit->GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
        if (PlayerUnit->GetACTCamera()->IsConfigured())
        {
            AddTickPrerequisiteComponent(PlayerUnit->GetACTCamera());
            if(auto* PC=UGameplayStatics::GetPlayerController(this,0)) PC->SetViewTarget(PlayerUnit);
        }
    }
}

bool APadmaACTMeleeLab::StartSceneBattle()
{
    if (!IsValid(ScenePlayer) || SceneTargets.IsEmpty())
    { UE_LOG(LogTemp,Error,TEXT("ACT lab requires a placed player and placed targets.")); return false; }
    CharacterDefinition=ScenePlayer->SceneSpec.Presentation.ACTDefinition;
    TArray<APadmaCombatUnit*> Participants; Participants.Add(ScenePlayer);
    for (APadmaCombatUnit* Target:SceneTargets) Participants.Add(Target);
    TSet<APadmaCombatUnit*> Seen;
    FPadmaCombatSetup Setup; Setup.Mode=EPadmaCombatMode::ACT;
    Setup.ArenaMinimum=FVector2D(-500,-500); Setup.ArenaMaximum=FVector2D(500,500);
    for (int32 Index=0;Index<Participants.Num();++Index)
    {
        auto* Unit=Participants[Index];
        if (!IsValid(Unit) || Seen.Contains(Unit) || Unit->GetWorld()!=GetWorld())
        { UE_LOG(LogTemp,Error,TEXT("Invalid or duplicate ACT scene participant.")); return false; }
        Seen.Add(Unit);
        if (!SceneInitialTransforms.Contains(Unit)) SceneInitialTransforms.Add(Unit,Unit->GetActorTransform());
        auto Spec=Unit->SceneSpec; Spec.bPlayer=Index==0; Spec.Location=SceneInitialTransforms.FindChecked(Unit).GetLocation();
        Setup.Units.Add(Spec);
    }
    for (auto* Unit:Participants) Unit->SetActorTransform(SceneInitialTransforms.FindChecked(Unit),false,nullptr,ETeleportType::TeleportPhysics);
    Battle->PayCost=[](float,float,FName,FString&){return true;};
    FString Failure;
    if (!Battle->StartBattle(Setup,Failure,Participants))
    { UE_LOG(LogTemp,Error,TEXT("ACT placed battle: %s"),*Failure); return false; }
    Battle->SetComponentTickEnabled(true);
    for (APadmaCombatUnit* Dummy:SceneTargets)
    {
        auto* P=Dummy->FindComponentByClass<UPadmaACTTrainingDummyPresentation>();
        if (!P) {P=NewObject<UPadmaACTTrainingDummyPresentation>(Dummy);Dummy->AddInstanceComponent(P);P->RegisterComponent();}
        P->Configure(Battle,TrainingDummyHitAnimation.LoadSynchronous());
    }
    if (ScenePlayer->GetACTCamera()->IsConfigured())
    {
        AddTickPrerequisiteComponent(ScenePlayer->GetACTCamera());
        if(auto* PC=UGameplayStatics::GetPlayerController(this,0)) PC->SetViewTarget(ScenePlayer);
    }
    return true;
}

bool APadmaACTMeleeLab::Fire()
{
    if (bShowcaseActive || bAnalyticsOpen) return false;
    if (auto* Player = Battle->GetPlayerUnit())
    {
        if (auto* Melee = Player->GetMelee())
        {
            if (Player->GetActions()->IsConfigured()) return Player->GetActions()->RequestInput(TEXT("Primary"));
            // 当前已经处于 Attack01 中：
            // 不要再次发起 GAS Attack，而是尝试缓存连击。
            if (Melee->IsAttacking())
            {
                return Melee->RequestCombo();
            }
        }
    }

    // 当前没有攻击，才真正发起一次新的 GAS Attack。
    FString Failure;

    const bool Result = Battle->Attack({}, Failure);

    if (!Result)
    {
        UE_LOG(
            LogTemp,
            Display,
            TEXT("Melee lab attack: %s"),
            *Failure);
    }

    return Result;
}

void APadmaACTMeleeLab::ToggleTarget()
{
    if (bShowcaseActive || bAnalyticsOpen) return;
    bTargetNear = !bTargetNear;

    if (auto* Enemy = Battle->GetUnit(TEXT("blade-dummy")))
    {
        if (auto* Player = Battle->GetPlayerUnit())
        {
            Enemy->SetActorLocation(
                Player->GetActorLocation()
                + Player->GetActorForwardVector() * (bTargetNear ? 220.f : 400.f));
        }
    }
}

void APadmaACTMeleeLab::CancelAttack()
{
    if (bShowcaseActive || bAnalyticsOpen) return;
    if (auto* Player = Battle->GetPlayerUnit())
    {
        if (Player->GetActions()->IsGameplayInputLocked()) return;
        Player->GetAbilitySystemComponent()->CancelAllAbilities();
        Player->GetMelee()->FinishAttack(true);
    }
    RestoreActionCamera();
}

void APadmaACTMeleeLab::ToggleAnalytics()
{
    const double Now = FPlatformTime::Seconds();
    if (LastAnalyticsToggleTime >= 0.0 && Now - LastAnalyticsToggleTime < 0.15) return;
    LastAnalyticsToggleTime = Now;
    SetAnalyticsOpen(!bAnalyticsOpen);
}

void APadmaACTMeleeLab::SetAnalyticsOpen(bool bOpen)
{
    if (!AnalyticsWidget) return;
    if (bAnalyticsOpen == bOpen)
    {
        AnalyticsWidget->SetOpen(bOpen);
        return;
    }
    if (bOpen && !BeginShowcase()) return;
    bAnalyticsOpen = bOpen;
    if (!bOpen) CloseShowcase();
    AnalyticsWidget->SetOpen(bAnalyticsOpen);
    if (bAnalyticsOpen) Battle->SetMoveInput(FVector2D::ZeroVector);
    if (auto* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (auto* PadmaPC = Cast<APadmaPlayerController>(PC))
            PadmaPC->SetACTTrainingOverlayOpen(bAnalyticsOpen || bShowcaseActive);
        PC->bShowMouseCursor = bAnalyticsOpen;
        if (bAnalyticsOpen)
        {
            FInputModeGameAndUI Input;
            Input.SetHideCursorDuringCapture(false);
            Input.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PC->SetInputMode(Input);
        }
        else
        {
            PC->SetInputMode(FInputModeGameOnly());
        }
    }
}

void APadmaACTMeleeLab::HandleReceipt(const FPadmaCombatReceipt& Receipt)
{
    if (Receipt.Effect != TEXT("damage") || !Battle || !GetWorld()) return;
    auto* Target = Battle->GetUnit(Receipt.TargetId);
    if (!Target) return;
    FVector Start = Target->GetActorLocation() + FVector(0.f, 0.f, 85.f);
    if (Receipt.SourceKind == EPadmaCombatSource::Unit)
        if (auto* Source = Battle->GetUnit(Receipt.SourceId)) Start = Source->GetActorLocation() + FVector(0.f, 0.f, 55.f);
    const FVector End = Target->GetActorLocation() + FVector(0.f, 0.f, 55.f);
    const FLinearColor Color = Receipt.bBlocked ? FLinearColor(.45f, .7f, .75f, 1.f) : Receipt.bTrueDamage ? FLinearColor(1.f, .85f, .12f, 1.f) : FLinearColor(1.f, .42f, .12f, 1.f);
    const FString Prefix = Receipt.bBlocked ? TEXT("BLOCK ") : Receipt.bTrueDamage ? TEXT("TRUE ") : TEXT("-");
    if (auto* Feedback = GetWorld()->SpawnActor<APadmaCombatFeedback>())
    {
        Feedback->SetOwner(this);
        FeedbackActors.Add(Feedback);
        const FVector CameraPosition = Camera ? Camera->GetComponentLocation() : End + FVector(0.f, -300.f, 200.f);
        Feedback->Play(Start, End, FString::Printf(TEXT("%s%.1f"), *Prefix, Receipt.Amount), Color, Receipt.Wave * .08f, CameraPosition);
    }
}

void APadmaACTMeleeLab::DestroyFeedbackActors()
{
    for (const TObjectPtr<APadmaCombatFeedback>& Feedback : FeedbackActors) if (IsValid(Feedback.Get())) Feedback->Destroy();
    FeedbackActors.Reset();
}

void APadmaACTMeleeLab::RestoreActionCamera()
{
    if (!bCameraRestored && Camera)
    {
        Camera->SetWorldTransform(CameraReturnTransform);
        Camera->SetFieldOfView(CameraReturnFOV);
    }
    CameraAction=nullptr; CameraElapsed=0; bCameraRestored=true;
}

void APadmaACTMeleeLab::EndPlay(EEndPlayReason::Type Reason)
{
    if (Battle)
    {
        Battle->OnReceipt.RemoveAll(this);
    }
    if (Analytics) Analytics->BindBattle(nullptr);
    DestroyFeedbackActors();
    RestoreShowcase();
    SetAnalyticsOpen(false);
    if (AnalyticsWidget) AnalyticsWidget->RemoveFromParent();
    AnalyticsWidget = nullptr;
    RestoreActionCamera();
    Super::EndPlay(Reason);
}

void APadmaACTMeleeLab::TickActionCamera(float Delta)
{
    auto* Player=Battle && Battle->IsBattleActive() ? Battle->GetPlayerUnit() : nullptr;
    if (Player && Player->GetACTCamera()->IsConfigured())
    {
        // Retain the lab's preview/capture component; the character is the actual ViewTarget and sole camera owner.
        Camera->SetWorldTransform(Player->GetACTCamera()->GetComponentTransform());
        Camera->SetFieldOfView(Player->GetACTCamera()->FieldOfView);
        return;
    }
    auto* Active=Player && Player->IsAlive() ? Player->GetActions()->GetActiveDefinition() : nullptr;
    if (!Active || Active->CameraSamples.Num()<2 || Active->CameraDuration<=0)
    { RestoreActionCamera(); return; }
    if (CameraAction!=Active || CameraActionGeneration!=Player->GetMelee()->GetActionGeneration())
    {
        RestoreActionCamera();
        CameraReturnTransform=Camera->GetComponentTransform(); CameraReturnFOV=Camera->FieldOfView;
        CameraAction=Active; CameraElapsed=0; bCameraRestored=false;
        CameraActionGeneration=Player->GetMelee()->GetActionGeneration();
    }
    // Original camera action is unaffected by the character Montage's time scale.
    CameraElapsed+=Delta;
    if (CameraElapsed>=Active->CameraDuration)
    {
        if (!bCameraRestored)
        { Camera->SetWorldTransform(CameraReturnTransform); Camera->SetFieldOfView(CameraReturnFOV); bCameraRestored=true; }
        return;
    }
    const auto& Samples=Active->CameraSamples;
    int32 Index=0;
    while (Index+1<Samples.Num()-1 && Samples[Index+1].Time<CameraElapsed) ++Index;
    const auto& A=Samples[Index]; const auto& B=Samples[Index+1];
    const float T=FMath::Clamp((CameraElapsed-A.Time)/FMath::Max(.0001f,B.Time-A.Time),0.f,1.f);
    const FTransform Local(FQuat::Slerp(A.Rotation.Quaternion(),B.Rotation.Quaternion(),T),FMath::Lerp(A.Position,B.Position,T));
    FTransform World=Local*Player->GetMesh()->GetComponentTransform();
    const float Aspect=Camera->AspectRatio>0 ? Camera->AspectRatio : 16.f/9.f;
    float FOV=FMath::RadiansToDegrees(2*FMath::Atan(FMath::Tan(FMath::DegreesToRadians(FMath::Lerp(A.VerticalFOV,B.VerticalFOV,T)*.5f))*Aspect));
    const float Ease=FMath::SmoothStep(0.f,1.f,(CameraElapsed-(Active->CameraDuration-Active->CameraEaseOut))/FMath::Max(.001f,Active->CameraEaseOut));
    FTransform Blended; Blended.Blend(World,CameraReturnTransform,Ease);
    Camera->SetWorldTransform(Blended); Camera->SetFieldOfView(FMath::Lerp(FOV,CameraReturnFOV,Ease));
}

void APadmaACTMeleeLab::Tick(float Delta)
{
    TickShowcaseRestPose();
    Super::Tick(Delta);
    if (bShowcaseActive) { TickShowcase(Delta); return; }
    TickActionCamera(Delta);

    auto* Player = Battle->GetPlayerUnit();
    if (!Player)
    {
        return;
    }

    Player->AttackCooldown = FMath::Max(0.f, Player->AttackCooldown - Delta);
    if (Player->GetActions()->IsConfigured())
    {
        if (auto* PC=UGameplayStatics::GetPlayerController(this,0))
        {
            // Controller BeginPlay may install its map camera after this lab's BeginPlay.
            // Reacquire the character view when the isolated ACT arena becomes operational.
            if(Player->GetACTCamera()->IsConfigured() && PC->GetViewTarget()!=Player)
            {
                PC->SetViewTarget(Player);
                if (!bAnalyticsOpen)
                {
                    PC->bShowMouseCursor=false;
                    PC->SetInputMode(FInputModeGameOnly());
                }
            }
            if (bAnalyticsOpen)
            {
                Battle->SetMoveInput(FVector2D::ZeroVector);
                if (GEngine) GEngine->AddOnScreenDebugMessage(reinterpret_cast<uint64>(this), 0, FColor::Cyan,
                    TEXT("ACT ANALYTICS OPEN  |  B close  |  combat input paused"));
                return;
            }
            const FVector2D Input(PC->IsInputKeyDown(EKeys::W)-PC->IsInputKeyDown(EKeys::S),PC->IsInputKeyDown(EKeys::D)-PC->IsInputKeyDown(EKeys::A));
            auto* Cam=Player->GetACTCamera();
            Cam->HandleInput(PC,!Battle->IsSkillLibraryOpen());
            const FVector WorldInput=Cam->IsConfigured() ? Cam->CameraRelativeMovement(Input) : FVector(Input.X,Input.Y,0);
            Battle->SetMoveInput(FVector2D(WorldInput.X,WorldInput.Y));
            const TPair<FKey,FName> Keys[]={{EKeys::E,TEXT("SkillE")},{EKeys::Q,TEXT("SkillQ")},{EKeys::R,TEXT("SkillR")},
                {EKeys::SpaceBar,TEXT("Jump")},{EKeys::LeftShift,TEXT("Dodge")},{EKeys::F,TEXT("Execution")}};
            for (const auto& Key:Keys) if (PC->WasInputKeyJustPressed(Key.Key)) Player->GetActions()->RequestInput(Key.Value);
            if (PC->WasInputKeyJustPressed(EKeys::LeftControl) || PC->WasInputKeyJustPressed(EKeys::RightControl)) Player->GetActions()->RequestInput(TEXT("ToggleRun"));
        }
        if (GEngine) GEngine->AddOnScreenDebugMessage(reinterpret_cast<uint64>(this),0,FColor::White,
            FString::Printf(TEXT("Chen ACT | Mouse orbit  MMB lock/unlock  Wheel zoom / switch lock\nWASD move  Ctrl walk/run  Space jump  LMB combo/plunge  E/Q/R skills  Shift dodge  F execution\nC cancel  F8 reset  T target  B analytics | Action %s | Perfect dodge %d"),
            *Player->GetActions()->GetActiveActionId().ToString(),Player->GetActions()->GetPerfectDodgeCount()));
        return;
    }

    if (auto* PC = UGameplayStatics::GetPlayerController(this, 0);
        PC && !Player->GetMelee()->IsAttacking())
    {
        const FVector Direction(
            PC->IsInputKeyDown(EKeys::W) - PC->IsInputKeyDown(EKeys::S),
            PC->IsInputKeyDown(EKeys::D) - PC->IsInputKeyDown(EKeys::A),
            0.f);

        if (!Direction.IsNearlyZero())
        {
            Player->AddActorWorldOffset(Direction.GetSafeNormal() * 160.f * Delta);
            Player->SetActorRotation(Direction.Rotation());
        }
    }

    if (auto* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (PC->WasInputKeyJustPressed(EKeys::One)) Player->GetMelee()->SetAttackPlayRate(.5f);
        if (PC->WasInputKeyJustPressed(EKeys::Two)) Player->GetMelee()->SetAttackPlayRate(1.f);
        if (PC->WasInputKeyJustPressed(EKeys::Three)) Player->GetMelee()->SetAttackPlayRate(2.f);
    }
    if (GEngine)
    {
        auto* Enemy = Battle->GetUnit(TEXT("blade-dummy"));
        GEngine->AddOnScreenDebugMessage(
            reinterpret_cast<uint64>(this),
            0,
            FColor::White,
            FString::Printf(
                TEXT("Attack 01 | LMB attack  WASD move  T near/far  C cancel  R reset  1/2/3 speed\n")
                TEXT("Montage %.3f  contacts %d  hit FX %d  target HP %.1f"),
                Player->GetMelee()->GetAttackTime(),
                Player->GetMelee()->GetConfirmedContacts(),
                Player->GetMelee()->GetSpawnedHitEffects(),
                Enemy ? Enemy->Health() : 0.f));
    }
}
