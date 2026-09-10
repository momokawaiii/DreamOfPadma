#include "World/Combat/PadmaCombatFeedback.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMesh.h"
#include "Misc/App.h"

APadmaCombatFeedback::APadmaCombatFeedback()
{
	PrimaryActorTick.bCanEverTick=true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	Particle=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ReceiptParticle"));
	Particle->SetupAttachment(GetRootComponent());Particle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Particle->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Sphere.Sphere")));
	Particle->SetRelativeScale3D(FVector(.14));
	Number=CreateDefaultSubobject<UTextRenderComponent>(TEXT("ReceiptNumber"));
	Number->SetupAttachment(GetRootComponent());Number->SetHorizontalAlignment(EHTA_Center);Number->SetWorldSize(22);
}
void APadmaCombatFeedback::Play(const FVector& From,const FVector& To,const FString& Text,const FLinearColor& Color,float Delay,const FVector& CameraPosition)
{
	Start=From;End=To;Wait=Delay;Age=0;
	Particle->SetWorldLocation(Start);
	Number->SetText(FText::FromString(Text));Number->SetTextRenderColor(Color.ToFColor(true));
	Number->SetWorldLocation(End+FVector(0,0,35));Number->SetWorldRotation((CameraPosition-End).Rotation());
	Particle->SetVisibility(false);Number->SetVisibility(false);
}
void APadmaCombatFeedback::Tick(float Delta)
{
	Super::Tick(Delta);
	Age+=FMath::Min(FApp::GetDeltaTime(),.1);
	if(Age<Wait)return;
	const float Local=Age-Wait;
	Particle->SetVisibility(Local<.28f);
	Particle->SetWorldLocation(FMath::Lerp(Start,End,FMath::Clamp(Local/.28f,0.f,1.f)));
	if(Local>=.28f){Number->SetVisibility(true);Number->SetWorldLocation(End+FVector(0,0,35+(Local-.28f)*65));}
	if(Local>1.15f)Destroy();
}
