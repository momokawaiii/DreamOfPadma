#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PadmaCombatFeedback.generated.h"
class UStaticMeshComponent;
class UTextRenderComponent;

/** Transient cosmetic replay of one receipt. It never calls a gameplay service. */
UCLASS()
class DREAMOFPADMA_API APadmaCombatFeedback : public AActor
{
	GENERATED_BODY()
public:
	APadmaCombatFeedback();
	void Play(const FVector& From,const FVector& To,const FString& Text,const FLinearColor& Color,float Delay,const FVector& CameraPosition);
	virtual void Tick(float DeltaSeconds) override;
private:
	UPROPERTY() TObjectPtr<UStaticMeshComponent> Particle;
	UPROPERTY() TObjectPtr<UTextRenderComponent> Number;
	FVector Start,End;
	float Age=0,Wait=0;
};
