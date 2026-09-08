#include "Demo/World/DemoWorldTile.h"

#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

ADemoWorldTile::ADemoWorldTile()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	TileMesh->SetupAttachment(SceneRoot);
	TileMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TileMesh->SetCollisionResponseToAllChannels(ECR_Block);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		TileMesh->SetStaticMesh(CubeMesh.Object);
	}

	Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Label"));
	Label->SetupAttachment(SceneRoot);
	Label->SetRelativeLocation(FVector(0.0f, 0.0f, 85.0f));
	Label->SetHorizontalAlignment(EHTA_Center);
	Label->SetVerticalAlignment(EVRTA_TextCenter);
	Label->SetWorldSize(30.0f);
	Label->SetTextRenderColor(FColor::White);

	if (TileMesh->GetMaterial(0) != nullptr)
	{
		TileMaterial = UMaterialInstanceDynamic::Create(TileMesh->GetMaterial(0), this);
		TileMesh->SetMaterial(0, TileMaterial);
	}
}

void ADemoWorldTile::Initialize(const FDemoWorldTileDefinition& InDefinition)
{
	Definition = InDefinition;
	SetActorLocation(InDefinition.DisplayLocation);
	Label->SetText(InDefinition.DisplayName);
	ApplyVisualState();
}

void ADemoWorldTile::SetSelected(const bool bInSelected)
{
	bSelected = bInSelected;
	ApplyVisualState();
}

void ADemoWorldTile::ApplyVisualState()
{
	if (TileMesh == nullptr || Label == nullptr)
	{
		return;
	}

	const FVector TileScale = bSelected
		? FVector(3.2f, 3.2f, 0.45f)
		: FVector(2.8f, 2.8f, 0.25f);
	TileMesh->SetRelativeScale3D(TileScale);
	TileMesh->SetRenderCustomDepth(bSelected);

	if (TileMaterial != nullptr)
	{
		const FLinearColor Color = bSelected
			? FLinearColor(1.0f, 0.72f, 0.08f, 1.0f)
			: Definition.BaseColor;
		TileMaterial->SetVectorParameterValue(TEXT("Color"), Color);
	}

	const FString LabelText = bSelected
		? FString::Printf(TEXT("[SELECTED] %s\n%s"), *Definition.DisplayName.ToString(), *Definition.DemoNodeId.ToDebugString())
		: FString::Printf(TEXT("%s\nClick to select"), *Definition.DisplayName.ToString());
	Label->SetText(FText::FromString(LabelText));
	Label->SetTextRenderColor(bSelected ? FColor::Yellow : FColor::White);
}
