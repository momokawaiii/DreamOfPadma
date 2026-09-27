#include "UI/Screens/PadmaACTHologramComponent.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

UPadmaACTHologramComponent::UPadmaACTHologramComponent()
{
    SetWidgetSpace(EWidgetSpace::World);
    SetDrawSize(FVector2D(1200, 800));
    SetPivot(FVector2D(.5f, .5f));
    SetBlendMode(EWidgetBlendMode::Transparent);
    SetTwoSided(true);
    SetBackgroundColor(FLinearColor::Transparent);
    SetWindowFocusable(false);
    bReceiveHardwareInput = false;
    SetTickWhenOffscreen(true);
    SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SetCollisionResponseToAllChannels(ECR_Ignore);
    SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    CastShadow = false;
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("/Game/Padma/UI/Training/M_ACTTrainingHologram"));
    if (Material.Succeeded()) SetMaterial(0,Material.Object);
}
