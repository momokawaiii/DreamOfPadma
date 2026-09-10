#pragma once
#include "UI/Screens/PadmaGameScreen.h"
#include "Core/Content/PadmaContentTypes.h"
#include "Widgets/SLeafWidget.h"

/** Painting-space view transform, shared by Slate paint and pointer input. */
struct FPadmaTutorialMapTransform
{
 float Zoom=1;
 FVector2D Pan=FVector2D::ZeroVector;
 FVector2D Project(FVector2D UV,FVector2D Size)const;
 FVector2D Unproject(FVector2D Point,FVector2D Size)const;
 void ZoomBy(float Multiplier);
 void PanBy(FVector2D Delta,FVector2D Size);
 void Reset();
};

/** Fixed painted projection. Input and borders share the exact same view transform. */
class SPadmaTutorialMap : public SLeafWidget
{
public:
 SLATE_BEGIN_ARGS(SPadmaTutorialMap){}
  SLATE_ARGUMENT(TSharedPtr<FSlateBrush>, Background)
  SLATE_ARGUMENT(UTexture2D*, Atlas)
  SLATE_ARGUMENT(bool, Mini)
  SLATE_ARGUMENT(TFunction<void(FName)>, Select)
  SLATE_ARGUMENT(TFunction<bool()>, CanNavigate)
 SLATE_END_ARGS()
 void Construct(const FArguments& Args);
 void Update(const FPadmaGameView& View);
 FName Hit(const FVector2D& Absolute) const;
 void ZoomBy(float Multiplier);
 void PanBy(FVector2D Delta);
 void ResetView();
 bool HandleNavigationKey(const FKey& Key,bool Down);
 void CancelInteraction();
 virtual FReply OnMouseButtonDown(const FGeometry&,const FPointerEvent&) override;
 virtual FReply OnMouseButtonUp(const FGeometry&,const FPointerEvent&) override;
 virtual FReply OnMouseMove(const FGeometry&,const FPointerEvent&) override;
 virtual FReply OnMouseWheel(const FGeometry&,const FPointerEvent&) override;
 virtual void OnMouseCaptureLost(const FCaptureLostEvent&) override;
 virtual void Tick(const FGeometry&,double,float) override;
 virtual FVector2D ComputeDesiredSize(float) const override;
 virtual int32 OnPaint(const FPaintArgs&,const FGeometry&,const FSlateRect&,FSlateWindowElementList&,int32,const FWidgetStyle&,bool)const override;
private:
 TArray<FPadmaMinimapNode> Nodes;
 TArray<FPadmaMinimapEdge> Edges;
 TSharedPtr<FSlateBrush> Background;
 TArray<TSharedPtr<FSlateBrush>> Portraits;
 FName DetailNode, MoveSource;
 FPadmaHexGridConfig HexGrid;
 FPadmaTutorialMapTransform Transform;
 bool bMini=false;
 TFunction<void(FName)> Select;
 TFunction<bool()> CanNavigate;
 TSet<FKey> NavigationKeys;
 FKey PointerButton;
 FVector2D PointerStart,PointerLast;
 FVector2D Point(FVector2D UV,FVector2D Size)const;
 FVector2D Unproject(FVector2D Point,FVector2D Size)const;
};
