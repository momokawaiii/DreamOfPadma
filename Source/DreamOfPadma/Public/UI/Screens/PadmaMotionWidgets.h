#pragma once
#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SLeafWidget.h"
#include "Styling/SlateBrush.h"

class UMaterialInstanceDynamic;

/** Presentation-only entrance timing. Input activation never waits for this animation. */
class SPadmaMotionElement : public SCompoundWidget
{
public:
 SLATE_BEGIN_ARGS(SPadmaMotionElement) : _Delay(0), _Duration(.28f), _Distance(18) {}
  SLATE_ARGUMENT(float, Delay)
  SLATE_ARGUMENT(float, Duration)
  SLATE_ARGUMENT(float, Distance)
  SLATE_DEFAULT_SLOT(FArguments, Content)
 SLATE_END_ARGS()
 void Construct(const FArguments& Args);
 virtual void Tick(const FGeometry& Geometry,double Time,float Delta) override;
private:
 float Elapsed=0,Delay=0,Duration=.28f,Distance=18;
};

/** Two shares of a single balance. GPU shader draws the fluid; CPU only eases state changes. */
class SPadmaWarBalance : public SLeafWidget
{
public:
 SLATE_BEGIN_ARGS(SPadmaWarBalance) {}
  SLATE_ARGUMENT(UMaterialInstanceDynamic*, Material)
 SLATE_END_ARGS()
 void Construct(const FArguments& Args);
 void SetBalance(float Share);
 virtual void Tick(const FGeometry& Geometry,double Time,float Delta) override;
 virtual FVector2D ComputeDesiredSize(float) const override { return FVector2D(490,32); }
 virtual int32 OnPaint(const FPaintArgs&,const FGeometry&,const FSlateRect&,FSlateWindowElementList&,int32,const FWidgetStyle&,bool) const override;
private:
 TWeakObjectPtr<UMaterialInstanceDynamic> Material;
 FSlateBrush Brush;
 float Target=.5f, Display=.5f;
};
