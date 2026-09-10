#include "Game/Presentation/PadmaNodePreview.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "UnrealClient.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"

APadmaNodePreview::APadmaNodePreview()
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	PreviewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PreviewCamera"));
	PreviewCamera->SetupAttachment(RootComponent);
	// Presentation framing only, unrelated to rule ranges or costs.
	PreviewCamera->SetRelativeLocation(FVector(850.0f, -1200.0f, 1050.0f));
	PreviewCamera->FieldOfView = 48.0f;
}

void APadmaNodePreview::ClearPresenters()
{
	for (APadmaModelPresentationActor* Presenter : Presenters)
	{
		if (IsValid(Presenter)) { Presenter->Destroy(); }
	}
	Presenters.Reset();
}

bool APadmaNodePreview::RebuildPreview()
{
	ClearPresenters();
	Diagnostics.Reset();
	const UPadmaNodeSceneDefinition* Scene = SceneDefinition.LoadSynchronous();
	const UPadmaPresentationCatalog* Sources = Catalog.LoadSynchronous();
	if (!GetWorld() || PreviewNodeId.IsNone() || !Scene || Scene->SceneDefinitionId.IsNone() || !Sources)
	{
		Diagnostics.Add(FText::FromString(TEXT("Preview needs World, NodeId, scene definition and catalog.")));
		return false;
	}
	TSet<FName> Slots;
	for (const FPadmaSceneSlot& Slot : Scene->Slots)
	{
		if (Slot.SlotId.IsNone() || Slots.Contains(Slot.SlotId) || Slot.Transform.ContainsNaN())
		{
			Diagnostics.Add(FText::FromString(TEXT("Scene has an empty/duplicate slot or invalid transform.")));
		}
		Slots.Add(Slot.SlotId);
	}
	TSet<FName> BoundSlots;
	TSet<FName> Instances;
	for (const FPadmaPreviewBinding& Binding : PreviewBindings)
	{
		if (!Slots.Contains(Binding.SlotId) || BoundSlots.Contains(Binding.SlotId)
			|| Binding.EntityInstanceId.IsNone() || Instances.Contains(Binding.EntityInstanceId))
		{
			Diagnostics.Add(FText::FromString(TEXT("Binding has an unknown/duplicate slot or empty/duplicate instance ID.")));
		}
		BoundSlots.Add(Binding.SlotId);
		Instances.Add(Binding.EntityInstanceId);
	}
	if (Scene->Slots.IsEmpty() || PreviewBindings.IsEmpty())
	{
		Diagnostics.Add(FText::FromString(TEXT("Preview layout/bindings are empty drafts.")));
	}
	if (!Diagnostics.IsEmpty()) { return false; }
	bool bAllVisualsReady = true;
	for (const FPadmaPreviewBinding& Binding : PreviewBindings)
	{
		FPadmaResolvedPresentation View = Sources->Resolve(Binding.Definition, Binding.SelectedSkillRow);
		Diagnostics.Append(View.VisualErrors);
		Diagnostics.Append(View.SkillErrors);
		bAllVisualsReady &= View.bVisualReady;
		const FPadmaSceneSlot* Slot = Scene->Slots.FindByPredicate(
			[&Binding](const FPadmaSceneSlot& Value) { return Value.SlotId == Binding.SlotId; });
		FActorSpawnParameters Parameters;
		Parameters.Owner = this;
		Parameters.ObjectFlags |= RF_Transient | RF_DuplicateTransient;
		APadmaModelPresentationActor* Presenter = GetWorld()->SpawnActor<APadmaModelPresentationActor>(
			APadmaModelPresentationActor::StaticClass(), Slot->Transform * GetActorTransform(), Parameters);
		if (!Presenter)
		{
			Diagnostics.Add(FText::FromString(TEXT("Cannot create presentation Actor.")));
			ClearPresenters();
			return false;
		}
		Presenter->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		Presenter->ApplyPresentation(View);
		Presenters.Add(Presenter);
		UE_LOG(LogTemp, Display, TEXT("[TASK-040] Node=%s Slot=%s Entity=%s Definition=%s Visual=%d Skills=%d"),
			*PreviewNodeId.ToString(), *Binding.SlotId.ToString(), *Binding.EntityInstanceId.ToString(),
			*View.DefinitionId.ToString(), View.bVisualReady, View.Skills.Num());
	}
	for (const FText& Message : Diagnostics)
	{
		UE_LOG(LogTemp, Display, TEXT("[TASK-040] %s"), *Message.ToString());
	}
	return bAllVisualsReady;
}

bool APadmaNodePreview::SetPreviewSelection(FName SlotId, FPadmaPresentationKey Definition)
{
	FPadmaPreviewBinding* Binding = PreviewBindings.FindByPredicate(
		[SlotId](const FPadmaPreviewBinding& Value) { return Value.SlotId == SlotId; });
	if (!Binding) { return false; }
	Binding->Definition = Definition;
	Binding->SelectedSkillRow = NAME_None;
	return RebuildPreview();
}

void APadmaNodePreview::BeginPlay()
{
	Super::BeginPlay();
	const bool bReady = RebuildPreview();
	PreviewCamera->SetWorldRotation((GetActorLocation() - PreviewCamera->GetComponentLocation()).Rotation());
	if (APlayerController* Controller = GetWorld()->GetFirstPlayerController())
	{
		Controller->SetViewTarget(this);
		Controller->bShowMouseCursor = true;
	}
#if !UE_BUILD_SHIPPING
	FString CapturePath;
	if (FParse::Value(FCommandLine::Get(), TEXT("PadmaModelCapture="), CapturePath) && !CapturePath.IsEmpty())
	{
		const uint64 FirstFrame = GFrameCounter;
		CaptureTicker = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateWeakLambda(this,
			[this, FirstFrame, CapturePath, bReady](float)
			{
				if (GFrameCounter < FirstFrame + 8) { return true; }
				FScreenshotRequest::RequestScreenshot(CapturePath, false, false);
				GetWorld()->GetTimerManager().SetTimer(CaptureExitTimer,
					FTimerDelegate::CreateWeakLambda(this, [bReady]()
					{
						FPlatformMisc::RequestExitWithStatus(false, bReady ? 0 : 1);
					}), 2.0f, false);
				return false;
			}));
	}
#endif
}

void APadmaNodePreview::StopCapture()
{
	FTSTicker::GetCoreTicker().RemoveTicker(CaptureTicker);
	if (GetWorld()) { GetWorld()->GetTimerManager().ClearTimer(CaptureExitTimer); }
}

void APadmaNodePreview::EndPlay(const EEndPlayReason::Type Reason)
{
	StopCapture();
	ClearPresenters();
	Super::EndPlay(Reason);
}

void APadmaNodePreview::Destroyed()
{
	StopCapture();
	ClearPresenters();
	Super::Destroyed();
}

APadmaModelPreviewGameMode::APadmaModelPreviewGameMode()
{
	DefaultPawnClass = nullptr;
}
