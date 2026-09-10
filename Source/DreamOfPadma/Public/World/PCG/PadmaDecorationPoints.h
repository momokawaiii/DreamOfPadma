#pragma once

#include "CoreMinimal.h"
#include "PCGSettings.h"
#include "Data/PCGPointData.h"
#include "PadmaDecorationPoints.generated.h"

/** Cosmetic points supplied by the map presenter; PCG performs the actual mesh spawning. */
UCLASS(BlueprintType, ClassGroup=(Procedural))
class DREAMOFPADMA_API UPadmaDecorationPointsSettings : public UPCGSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Decoration") TArray<FPCGPoint> Points;
	virtual bool UseSeed() const override { return true; }
#if WITH_EDITOR
	virtual FName GetDefaultNodeName() const override { return TEXT("PadmaDecorationPoints"); }
	virtual FText GetDefaultNodeTitle() const override { return NSLOCTEXT("PadmaMap", "DecorationPoints", "Padma Decoration Points"); }
	virtual EPCGSettingsType GetType() const override { return EPCGSettingsType::Spatial; }
#endif
protected:
	virtual TArray<FPCGPinProperties> InputPinProperties() const override { return {}; }
	virtual TArray<FPCGPinProperties> OutputPinProperties() const override { return DefaultPointOutputPinProperties(); }
	virtual FPCGElementPtr CreateElement() const override;
};
