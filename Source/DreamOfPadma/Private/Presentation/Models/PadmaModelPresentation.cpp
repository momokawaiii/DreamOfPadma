#include "Presentation/Models/PadmaModelPresentation.h"
#include "Animation/AnimInstance.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

APadmaModelPresentationActor::APadmaModelPresentationActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	StaticVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticVisual"));
	StaticVisual->SetupAttachment(RootComponent);
	StaticVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkeletalVisual = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalVisual"));
	SkeletalVisual->SetupAttachment(RootComponent);
	SkeletalVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APadmaModelPresentationActor::ApplyPresentation(const FPadmaResolvedPresentation& InPresentation)
{
	// Clear both representations and old animation before applying a replacement.
	StaticVisual->SetStaticMesh(nullptr);
	SkeletalVisual->SetAnimInstanceClass(nullptr);
	SkeletalVisual->SetSkeletalMesh(nullptr);
	StaticVisual->SetVisibility(false);
	SkeletalVisual->SetVisibility(false);
	Presentation = InPresentation;
	if (!Presentation.bVisualReady) { return; }
	StaticVisual->SetRelativeTransform(Presentation.ModelTransform);
	SkeletalVisual->SetRelativeTransform(Presentation.ModelTransform);
	if (Presentation.StaticModel)
	{
		StaticVisual->SetStaticMesh(Presentation.StaticModel);
		StaticVisual->SetVisibility(true);
	}
	else if (Presentation.SkeletalModel)
	{
		SkeletalVisual->SetSkeletalMesh(Presentation.SkeletalModel);
		SkeletalVisual->SetAnimInstanceClass(Presentation.AnimationClass);
		SkeletalVisual->SetVisibility(true);
	}
}
