#include "Demo/World/DemoSandboxWorld.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Demo/Session/DemoTransitionSessionSubsystem.h"
#include "Demo/World/DemoWorldTile.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ADemoSandboxWorld::ADemoSandboxWorld()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	Ground = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ground"));
	Ground->SetupAttachment(SceneRoot);
	Ground->SetRelativeLocation(FVector(0.0f, 0.0f, -30.0f));
	Ground->SetRelativeScale3D(FVector(14.0f, 14.0f, 1.0f));
	Ground->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Ground->SetCollisionResponseToAllChannels(ECR_Block);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(
		TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (PlaneMesh.Succeeded())
	{
		Ground->SetStaticMesh(PlaneMesh.Object);
	}

	TitleText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TitleText"));
	TitleText->SetupAttachment(SceneRoot);
	TitleText->SetRelativeLocation(FVector(0.0f, 480.0f, 80.0f));
	TitleText->SetHorizontalAlignment(EHTA_Center);
	TitleText->SetVerticalAlignment(EVRTA_TextCenter);
	TitleText->SetWorldSize(42.0f);
	TitleText->SetTextRenderColor(FColor(180, 220, 255));
	TitleText->SetText(FText::FromString(TEXT("DREAM OF PADMA // DEMO SANDBOX")));

	StatusText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("StatusText"));
	StatusText->SetupAttachment(SceneRoot);
	StatusText->SetRelativeLocation(FVector(0.0f, -480.0f, 80.0f));
	StatusText->SetHorizontalAlignment(EHTA_Center);
	StatusText->SetVerticalAlignment(EVRTA_TextCenter);
	StatusText->SetWorldSize(25.0f);
	StatusText->SetTextRenderColor(FColor::White);

	DemoCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("DemoCamera"));
	DemoCamera->SetupAttachment(SceneRoot);
	DemoCamera->SetRelativeLocation(FVector(0.0f, -1350.0f, 1150.0f));
	DemoCamera->SetRelativeRotation(FRotator(-40.0f, 0.0f, 0.0f));
	DemoCamera->FieldOfView = 48.0f;

	FixtureTiles.Add(FDemoWorldTileDefinition());
}

void ADemoSandboxWorld::BeginPlay()
{
	Super::BeginPlay();

	if (FixtureTiles.Num() == 0)
	{
		FixtureTiles.Add(FDemoWorldTileDefinition());
	}

	OriginalFixtureTiles = FixtureTiles;
	SelectionModel.ReplaceFixture(FixtureTiles);
	SpawnTileActors();
	ConfigurePlayerViewAndInput();
	RefreshTilePresentation();
	SetStatus(FText::FromString(
		TEXT("Click the Demo Tile to select. Enter confirms. Right Click/Esc deselects. F10/F11 test invalid/restore fixture. F12 consumes context.")));
}

void ADemoSandboxWorld::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (const TPair<FDemoNodeId, TObjectPtr<ADemoWorldTile>>& Pair : TileActors)
	{
		if (IsValid(Pair.Value))
		{
			Pair.Value->Destroy();
		}
	}
	TileActors.Reset();

	Super::EndPlay(EndPlayReason);
}

void ADemoSandboxWorld::SpawnTileActors()
{
	if (GetWorld() == nullptr)
	{
		return;
	}

	for (const FDemoWorldTileDefinition& Definition : FixtureTiles)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ADemoWorldTile* Tile = GetWorld()->SpawnActor<ADemoWorldTile>(
			ADemoWorldTile::StaticClass(),
			GetActorTransform(),
			SpawnParameters);
		if (Tile == nullptr)
		{
			continue;
		}

		Tile->Initialize(Definition);
		Tile->SetActorLocation(GetActorTransform().TransformPosition(Definition.DisplayLocation));
		TileActors.Add(Definition.DemoNodeId, Tile);
	}
}

void ADemoSandboxWorld::ConfigurePlayerViewAndInput()
{
	APlayerController* PlayerController = GetPlayerController();
	if (PlayerController == nullptr)
	{
		return;
	}

	PlayerController->bShowMouseCursor = true;
	PlayerController->bEnableClickEvents = true;
	PlayerController->bEnableMouseOverEvents = true;

	const FVector CameraLocation = GetActorLocation() + FVector(0.0f, -1350.0f, 1150.0f);
	DemoCamera->SetWorldLocation(CameraLocation);
	DemoCamera->SetWorldRotation((GetActorLocation() - CameraLocation).ToOrientationRotator());
	TitleText->SetWorldRotation((CameraLocation - TitleText->GetComponentLocation()).ToOrientationRotator());
	StatusText->SetWorldRotation((CameraLocation - StatusText->GetComponentLocation()).ToOrientationRotator());

	PlayerController->SetViewTarget(this);
	EnableInput(PlayerController);
	if (InputComponent == nullptr)
	{
		return;
	}

	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ADemoSandboxWorld::HandlePrimaryClick);
	InputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &ADemoSandboxWorld::HandleCancelSelection);
	InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &ADemoSandboxWorld::HandleCancelSelection);
	InputComponent->BindKey(EKeys::Enter, IE_Pressed, this, &ADemoSandboxWorld::HandleConfirmSelection);
	InputComponent->BindKey(EKeys::F10, IE_Pressed, this, &ADemoSandboxWorld::HandleInvalidateFixture);
	InputComponent->BindKey(EKeys::F11, IE_Pressed, this, &ADemoSandboxWorld::HandleRestoreFixture);
	InputComponent->BindKey(EKeys::F12, IE_Pressed, this, &ADemoSandboxWorld::HandleConsumeTransition);
}

bool ADemoSandboxWorld::SelectTile(const FDemoSelectNodeRequest& Request, FText& OutFailure)
{
	if (!SelectionModel.Select(Request, OutFailure))
	{
		SetStatus(OutFailure, FColor::Red);
		return false;
	}

	RefreshTilePresentation();
	SetStatus(FText::FromString(FString::Printf(
		TEXT("Selected %s. Press Enter to confirm or Esc to deselect."),
		*Request.DemoNodeId.ToDebugString())), FColor::Yellow);
	return true;
}

void ADemoSandboxWorld::DeselectTile()
{
	SelectionModel.Deselect();
	RefreshTilePresentation();
	SetStatus(FText::FromString(TEXT("Selection cleared. Click the Demo Tile to select it.")));
}

bool ADemoSandboxWorld::ConfirmSelectedTile(
	const FDemoConfirmNodeSelectionRequest& Request,
	FDemoTransitionContext& OutContext,
	FText& OutFailure)
{
	FDemoTransitionContext Candidate;
	if (!SelectionModel.BuildTransitionContext(Request, Candidate, OutFailure))
	{
		SetStatus(FText::FromString(FString::Printf(TEXT("Transition failed: %s (existing session context unchanged)."), *OutFailure.ToString())), FColor::Red);
		return false;
	}

	UDemoTransitionSessionSubsystem* SessionSubsystem = GetSessionSubsystem();
	if (SessionSubsystem == nullptr)
	{
		OutFailure = FText::FromString(TEXT("Demo transition session is unavailable."));
		SetStatus(FText::FromString(FString::Printf(TEXT("Transition failed: %s (existing session context unchanged)."), *OutFailure.ToString())), FColor::Red);
		return false;
	}

	if (!SessionSubsystem->PublishTransition(Candidate, OutFailure))
	{
		SetStatus(FText::FromString(FString::Printf(TEXT("Transition failed: %s (existing session context unchanged)."), *OutFailure.ToString())), FColor::Red);
		return false;
	}

	OutContext = Candidate;
	SetStatus(FText::FromString(FString::Printf(
		TEXT("Transition context ready | Node=%s | Scenario=%s | Spawn=%s | stored in GameInstance session."),
		*Candidate.DemoNodeId.ToDebugString(),
		*Candidate.TargetScenarioId.ToDebugString(),
		*Candidate.SpawnPointId.ToDebugString())), FColor::Green);
	return true;
}

void ADemoSandboxWorld::RefreshTilePresentation()
{
	for (const TPair<FDemoNodeId, TObjectPtr<ADemoWorldTile>>& Pair : TileActors)
	{
		ADemoWorldTile* Tile = Pair.Value;
		if (!IsValid(Tile))
		{
			continue;
		}

		if (const FDemoWorldTileDefinition* Definition = FindAuthoredFixture(Pair.Key))
		{
			Tile->Initialize(*Definition);
			Tile->SetActorLocation(GetActorTransform().TransformPosition(Definition->DisplayLocation));
		}
		Tile->SetSelected(SelectionModel.IsSelected(Pair.Key));
	}
}

void ADemoSandboxWorld::SetStatus(const FText& Status, const FColor& Color)
{
	if (StatusText != nullptr)
	{
		StatusText->SetText(Status);
		StatusText->SetTextRenderColor(Color);
	}

	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(71007, 4.0f, Color, Status.ToString());
	}

	UE_LOG(LogTemp, Log, TEXT("[TASK-007] %s"), *Status.ToString());
}

const FDemoWorldTileDefinition* ADemoSandboxWorld::FindAuthoredFixture(const FDemoNodeId& NodeId) const
{
	return FixtureTiles.FindByPredicate([&NodeId](const FDemoWorldTileDefinition& Definition)
	{
		return Definition.DemoNodeId == NodeId;
	});
}

ADemoWorldTile* ADemoSandboxWorld::FindTileActor(const FDemoNodeId& NodeId) const
{
	if (const TObjectPtr<ADemoWorldTile>* FoundTile = TileActors.Find(NodeId))
	{
		return FoundTile->Get();
	}
	return nullptr;
}

UDemoTransitionSessionSubsystem* ADemoSandboxWorld::GetSessionSubsystem() const
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		return GameInstance->GetSubsystem<UDemoTransitionSessionSubsystem>();
	}
	return nullptr;
}

APlayerController* ADemoSandboxWorld::GetPlayerController() const
{
	return UGameplayStatics::GetPlayerController(this, 0);
}

void ADemoSandboxWorld::HandlePrimaryClick()
{
	APlayerController* PlayerController = GetPlayerController();
	if (PlayerController == nullptr)
	{
		return;
	}

	FHitResult HitResult;
	if (!PlayerController->GetHitResultUnderCursor(ECC_Visibility, true, HitResult))
	{
		SetStatus(FText::FromString(TEXT("No Demo Tile was clicked.")), FColor::Yellow);
		return;
	}

	ADemoWorldTile* Tile = Cast<ADemoWorldTile>(HitResult.GetActor());
	if (Tile == nullptr)
	{
		SetStatus(FText::FromString(TEXT("Click the Demo Tile, not the sandbox ground.")), FColor::Yellow);
		return;
	}

	FDemoNodeId SelectedNodeId;
	if (SelectionModel.TryGetSelectedNodeId(SelectedNodeId) && SelectedNodeId == Tile->GetDemoNodeId())
	{
		DeselectTile();
		return;
	}

	FDemoSelectNodeRequest Request;
	Request.DemoNodeId = Tile->GetDemoNodeId();
	FText Failure;
	SelectTile(Request, Failure);
}

void ADemoSandboxWorld::HandleCancelSelection()
{
	DeselectTile();
}

void ADemoSandboxWorld::HandleInvalidateFixture()
{
	InvalidateSelectedFixtureForTesting();
}

void ADemoSandboxWorld::HandleRestoreFixture()
{
	RestoreFixtureForTesting();
}

void ADemoSandboxWorld::HandleConsumeTransition()
{
	ConsumePendingTransitionForTesting();
}

void ADemoSandboxWorld::HandleConfirmSelection()
{
	FDemoNodeId SelectedNodeId;
	if (!SelectionModel.TryGetSelectedNodeId(SelectedNodeId))
	{
		FDemoConfirmNodeSelectionRequest Request;
		FDemoTransitionContext UnusedContext;
		FText Failure;
		ConfirmSelectedTile(Request, UnusedContext, Failure);
		return;
	}

	FDemoConfirmNodeSelectionRequest Request;
	Request.DemoNodeId = SelectedNodeId;
	FDemoTransitionContext Context;
	FText Failure;
	ConfirmSelectedTile(Request, Context, Failure);
}

void ADemoSandboxWorld::InvalidateSelectedFixtureForTesting()
{
	FDemoNodeId SelectedNodeId;
	if (!SelectionModel.TryGetSelectedNodeId(SelectedNodeId))
	{
		SetStatus(FText::FromString(TEXT("Select the Demo Tile before running the invalid-fixture test.")), FColor::Yellow);
		return;
	}

	FDemoWorldTileDefinition* Fixture = FixtureTiles.FindByPredicate([&SelectedNodeId](FDemoWorldTileDefinition& Definition)
	{
		return Definition.DemoNodeId == SelectedNodeId;
	});
	if (Fixture == nullptr)
	{
		SetStatus(FText::FromString(TEXT("Invalid-fixture test could not find the selected fixture.")), FColor::Red);
		return;
	}

	Fixture->TargetScenarioId = FDemoScenarioId();
	SelectionModel.ReplaceFixture(FixtureTiles);
	RefreshTilePresentation();
	SetStatus(FText::FromString(TEXT("Fixture test active: target scenario ID cleared. Press Enter; failure must keep the previous session context.")), FColor::Yellow);
}

void ADemoSandboxWorld::RestoreFixtureForTesting()
{
	FixtureTiles = OriginalFixtureTiles;
	SelectionModel.ReplaceFixture(FixtureTiles);
	RefreshTilePresentation();
	SetStatus(FText::FromString(TEXT("Demo fixture restored. Existing session context was not changed by restore.")), FColor::Green);
}

void ADemoSandboxWorld::ConsumePendingTransitionForTesting()
{
	UDemoTransitionSessionSubsystem* SessionSubsystem = GetSessionSubsystem();
	if (SessionSubsystem == nullptr)
	{
		SetStatus(FText::FromString(TEXT("Demo transition session is unavailable.")), FColor::Red);
		return;
	}

	FDemoTransitionContext Context;
	FText Failure;
	if (!SessionSubsystem->ConsumeTransition(Context, Failure))
	{
		SetStatus(Failure, FColor::Yellow);
		return;
	}

	SetStatus(FText::FromString(FString::Printf(
		TEXT("Consumed transition context | Node=%s | Scenario=%s | Spawn=%s."),
		*Context.DemoNodeId.ToDebugString(),
		*Context.TargetScenarioId.ToDebugString(),
		*Context.SpawnPointId.ToDebugString())), FColor::Green);
}
