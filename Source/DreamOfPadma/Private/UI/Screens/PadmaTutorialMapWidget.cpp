#include "UI/Screens/PadmaTutorialMapWidget.h"
#include "Core/WorldMap/PadmaTutorialMap.h"
#include "Engine/Texture2D.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Fonts/FontMeasure.h"

namespace
{
 const FLinearColor Gold(.96f,.68f,.24f), Jade(.2f,.88f,.73f), Red(.92f,.28f,.22f), Navy(.006f,.025f,.043f,.88f);
 FLinearColor Faction(FName Owner){return Owner==TEXT("player")?Jade:Owner==TEXT("ruler")?Red:FLinearColor(.79f,.68f,.43f);}
}
void SPadmaTutorialMap::Construct(const FArguments& A)
{
 Background=A._Background;bMini=A._Mini;Select=A._Select;CanNavigate=A._CanNavigate;SetClipping(EWidgetClipping::ClipToBounds);SetCanTick(true);
 if(A._Atlas)for(int32 I=0;I<9;++I)
 {
  auto B=MakeShared<FSlateBrush>();B->SetResourceObject(A._Atlas);B->ImageSize=FVector2D(26,32);
  B->SetUVRegion(FBox2D(FVector2D((I%3+.05)/3.,(I/3+.02)/3.),FVector2D((I%3+.95)/3.,(I/3+.98)/3.)));Portraits.Add(B);
 }
}
FVector2D FPadmaTutorialMapTransform::Project(FVector2D UV,FVector2D Size)const
{return ((UV/100.-FVector2D(.5,.5))*Zoom+FVector2D(.5,.5)+Pan)*Size;}
FVector2D FPadmaTutorialMapTransform::Unproject(FVector2D P,FVector2D Size)const
{return ((P/Size-FVector2D(.5,.5)-Pan)/Zoom+FVector2D(.5,.5))*100.;}
void FPadmaTutorialMapTransform::ZoomBy(float M)
{
 if(!FMath::IsFinite(M)||M<=0)return;
 Zoom=FMath::Clamp(Zoom*M,1.f,1.8f);
 const double Limit=(Zoom-1)*.5;
 Pan.X=FMath::Clamp(Pan.X,-Limit,Limit);Pan.Y=FMath::Clamp(Pan.Y,-Limit,Limit);
}
void FPadmaTutorialMapTransform::PanBy(FVector2D D,FVector2D Size)
{
 if(!FMath::IsFinite(Size.X)||!FMath::IsFinite(Size.Y)||Size.X<1||Size.Y<1
  ||!FMath::IsFinite(D.X)||!FMath::IsFinite(D.Y)||D.IsNearlyZero())return;
 // The initial view fits the whole painting. Its first pan supplies enough overscan to move
 // while clamping the painted edge to the viewport; reset restores the exact original fit.
 Zoom=FMath::Max(Zoom,1.12f);Pan+=D/Size;ZoomBy(1);
}
void FPadmaTutorialMapTransform::Reset(){Zoom=1;Pan=FVector2D::ZeroVector;}
void SPadmaTutorialMap::Update(const FPadmaGameView& V){Nodes=V.MinimapNodes;Edges=V.MinimapEdges;HexGrid=V.HexGrid;DetailNode=V.DetailNode;MoveSource=V.MoveSource;Invalidate(EInvalidateWidgetReason::Paint);}
FVector2D SPadmaTutorialMap::Point(FVector2D UV,FVector2D Size)const{return Transform.Project(UV,Size);}
FVector2D SPadmaTutorialMap::Unproject(FVector2D P,FVector2D Size)const{return Transform.Unproject(P,Size);}
void SPadmaTutorialMap::ZoomBy(float M){Transform.ZoomBy(M);Invalidate(EInvalidateWidgetReason::Paint);}
void SPadmaTutorialMap::PanBy(FVector2D D){Transform.PanBy(D,GetCachedGeometry().GetLocalSize());Invalidate(EInvalidateWidgetReason::Paint);}
void SPadmaTutorialMap::ResetView(){Transform.Reset();Invalidate(EInvalidateWidgetReason::Paint);}
FReply SPadmaTutorialMap::OnMouseButtonDown(const FGeometry&,const FPointerEvent& E)
{
 if(bMini||!IsEnabled()||(CanNavigate&&!CanNavigate())||(E.GetEffectingButton()!=EKeys::LeftMouseButton&&E.GetEffectingButton()!=EKeys::MiddleMouseButton))return FReply::Unhandled();
 PointerButton=E.GetEffectingButton();PointerStart=PointerLast=E.GetScreenSpacePosition();NavigationKeys.Reset();
 return FReply::Handled().CaptureMouse(SharedThis(this));
}
FReply SPadmaTutorialMap::OnMouseButtonUp(const FGeometry&,const FPointerEvent& E)
{
 if(E.GetEffectingButton()!=PointerButton)return FReply::Unhandled();
 const bool Click=PointerButton==EKeys::LeftMouseButton&&FVector2D::Distance(PointerStart,E.GetScreenSpacePosition())<8;
 PointerButton=FKey();const FName Target=Click?Hit(E.GetScreenSpacePosition()):NAME_None;
 if(!Target.IsNone()&&Select)Select(Target);
 return FReply::Handled().ReleaseMouseCapture();
}
FReply SPadmaTutorialMap::OnMouseMove(const FGeometry& G,const FPointerEvent& E)
{
 if(CanNavigate&&!CanNavigate()){CancelInteraction();return FReply::Unhandled();}
 if(PointerButton==EKeys::MiddleMouseButton&&HasMouseCapture()){PanBy((E.GetScreenSpacePosition()-PointerLast)/FMath::Max(.01f,G.Scale));PointerLast=E.GetScreenSpacePosition();return FReply::Handled();}
 return FReply::Unhandled();
}
FReply SPadmaTutorialMap::OnMouseWheel(const FGeometry&,const FPointerEvent& E)
{if(bMini||!IsEnabled()||(CanNavigate&&!CanNavigate()))return FReply::Unhandled();ZoomBy(FMath::Pow(1.12f,E.GetWheelDelta()));return FReply::Handled();}
void SPadmaTutorialMap::OnMouseCaptureLost(const FCaptureLostEvent& E)
{PointerButton=FKey();SLeafWidget::OnMouseCaptureLost(E);}
void SPadmaTutorialMap::CancelInteraction()
{
 NavigationKeys.Reset();PointerButton=FKey();
 if(FSlateApplication::IsInitialized()&&HasMouseCapture())FSlateApplication::Get().ReleaseAllPointerCapture(0);
}
bool SPadmaTutorialMap::HandleNavigationKey(const FKey& Key,bool Down)
{
 if(Key==EKeys::Home){if(Down)ResetView();return true;}
 FVector2D Direction=FVector2D::ZeroVector;
 if(Key==EKeys::W||Key==EKeys::Up)Direction.Y=1;else if(Key==EKeys::S||Key==EKeys::Down)Direction.Y=-1;
 else if(Key==EKeys::A||Key==EKeys::Left)Direction.X=1;else if(Key==EKeys::D||Key==EKeys::Right)Direction.X=-1;else return false;
 if(Down){if(!NavigationKeys.Contains(Key))PanBy(Direction*12);NavigationKeys.Add(Key);}else NavigationKeys.Remove(Key);return true;
}
void SPadmaTutorialMap::Tick(const FGeometry& G,double Now,float Delta)
{
 SLeafWidget::Tick(G,Now,Delta);if(NavigationKeys.IsEmpty())return;
 if(!IsEnabled()||(CanNavigate&&!CanNavigate())||FSlateApplication::Get().HasAnyMouseCaptor()||FSlateApplication::Get().GetActiveTopLevelWindow()!=FSlateApplication::Get().FindWidgetWindow(SharedThis(this)))
 {NavigationKeys.Reset();return;}
 FVector2D Motion((NavigationKeys.Contains(EKeys::A)||NavigationKeys.Contains(EKeys::Left)?1.:0.)-(NavigationKeys.Contains(EKeys::D)||NavigationKeys.Contains(EKeys::Right)?1.:0.),
 (NavigationKeys.Contains(EKeys::W)||NavigationKeys.Contains(EKeys::Up)?1.:0.)-(NavigationKeys.Contains(EKeys::S)||NavigationKeys.Contains(EKeys::Down)?1.:0.));
 PanBy(Motion.GetClampedToMaxSize(1.)*Delta*600.);
}
FVector2D SPadmaTutorialMap::ComputeDesiredSize(float)const{return bMini?FVector2D(238,170):FVector2D(1296,635);}
FName SPadmaTutorialMap::Hit(const FVector2D& Absolute)const
{
 const auto& G=GetCachedGeometry();if(!G.IsUnderLocation(Absolute)||G.GetLocalSize().X<1||G.GetLocalSize().Y<1)return NAME_None;
 const FVector2D UV=Unproject(G.AbsoluteToLocal(Absolute),G.GetLocalSize());
 // Prioritize visible landmark footprints, then exact hex containment at the regular grid center.
 if(!bMini)for(const auto& N:Nodes)if(!N.bWilderness&&FVector2D::Distance((UV-N.Position)*FVector2D(1,1./HexGrid.ProjectionY),FVector2D::ZeroVector)<2.8)return N.Id;
 for(const auto& N:Nodes)
 {
  if(FPadmaTutorialMapGenerator::ContainsPoint(HexGrid,N.HexQ,N.HexR,UV))return N.Id;
 }
 return NAME_None;
}
int32 SPadmaTutorialMap::OnPaint(const FPaintArgs&,const FGeometry& G,const FSlateRect&,FSlateWindowElementList& Out,int32 L,const FWidgetStyle&,bool)const
{
 const auto Size=G.GetLocalSize();const auto* White=FCoreStyle::Get().GetBrush("WhiteBrush");
 if(Background)FSlateDrawElement::MakeBox(Out,L,G.ToPaintGeometry(Size*Transform.Zoom,FSlateLayoutTransform(Point(FVector2D::ZeroVector,Size))),Background.Get(),ESlateDrawEffect::None,bMini?FLinearColor(.55f,.65f,.68f):FLinearColor::White);
 auto Line=[&](const TArray<FVector2D>& P,FLinearColor C,float W,int32 Layer){FSlateDrawElement::MakeLines(Out,Layer,G.ToPaintGeometry(),P,ESlateDrawEffect::None,C,true,W);};
 auto Box=[&](FVector2D P,FVector2D S,FLinearColor C,int32 Layer){FSlateDrawElement::MakeBox(Out,Layer,G.ToPaintGeometry(S,FSlateLayoutTransform(P)),White,ESlateDrawEffect::None,C);};
 TMap<FIntPoint,const FPadmaMinimapNode*> Grid;for(const auto& N:Nodes)Grid.Add(FIntPoint(N.HexQ,N.HexR),&N);
 const FIntPoint Neighbors[]={FIntPoint(0,1),FIntPoint(-1,1),FIntPoint(-1,0),FIntPoint(0,-1),FIntPoint(1,-1),FIntPoint(1,0)};
 for(const auto& N:Nodes)
 {
  TArray<FVector2D> Corners;
  for(int32 I=0;I<6;++I)Corners.Add(Point(FPadmaTutorialMapGenerator::CellCorner(HexGrid,N.HexQ,N.HexR,I),Size));
  const bool Active=N.Id==DetailNode;
  for(int32 I=0;I<6;++I)
  {
   const auto* Other=Grid.Find(FIntPoint(N.HexQ,N.HexR)+Neighbors[I]);
   const bool Border=N.Owner!=TEXT("neutral")&&(!Other||(*Other)->Owner!=N.Owner);
   if(Border)Line({Corners[I],Corners[(I+1)%6]},Faction(N.Owner)*FLinearColor(1,1,1,.8f),bMini?1.2f:2.f,L+1);
  }
  if(Active)
  {
   const FVector2D FirstCorner=Corners[0];Corners.Add(FirstCorner);if(!bMini)Line(Corners,FLinearColor(1,.58f,.08f,.2f),9,L+2);
   Line(Corners,Gold,bMini?2:2.6f,L+3);
  }
 }
 // Only a currently legal direct movement preview is highlighted; background roads carry no rule authority.
 const auto* Source=Nodes.FindByPredicate([&](const auto& N){return N.Id==MoveSource;});
 const auto* Target=Nodes.FindByPredicate([&](const auto& N){return N.Id==DetailNode;});
 if(Source&&Target&&Source!=Target)
 {
  const int32 A=int32(Source-Nodes.GetData()),B=int32(Target-Nodes.GetData());
  if(Edges.ContainsByPredicate([&](const auto& E){return E.From==A&&E.To==B&&!E.bLocked;}))
  {TArray<FVector2D> P={Point(Source->Position,Size),Point(Target->Position,Size)};Line(P,FLinearColor(1,.55f,.06f,.18f),10,L+3);Line(P,Gold,2.2f,L+4);}
 }
 const auto Font=FCoreStyle::GetDefaultFontStyle("Regular",bMini?9:17);
 for(const auto& N:Nodes)if(!N.bWilderness)
 {
  const auto P=Point(N.Position,Size);const auto C=Faction(N.Owner);
  if(bMini){Box(P-FVector2D(2,2),FVector2D(4,4),C,L+4);continue;}
  const auto TextSize=FSlateApplication::Get().GetRenderer()->GetFontMeasureService()->Measure(N.Title,Font);
  const FVector2D Banner=P-FVector2D(TextSize.X*.5+12,65);
  Box(Banner-FVector2D(1,1),TextSize+FVector2D(26,12),C*.7f,L+5);Box(Banner,TextSize+FVector2D(24,10),Navy,L+6);
  FSlateDrawElement::MakeText(Out,L+7,G.ToPaintGeometry(TextSize,FSlateLayoutTransform(Banner+FVector2D(12,5))),N.Title,Font,ESlateDrawEffect::None,FLinearColor(.94f,.9f,.8f));
  const FVector2D Diamond=Banner+FVector2D(-14,TextSize.Y*.5+5);
  Line({Diamond+FVector2D(0,-15),Diamond+FVector2D(13,0),Diamond+FVector2D(0,15),Diamond+FVector2D(-13,0),Diamond+FVector2D(0,-15)},C,2,L+7);
  if(N.Garrison>0)
  {
   const auto Troop=P+FVector2D(-27,-15);Box(Troop-FVector2D(3,3),FVector2D(81,39),Navy,L+5);
   if(Portraits.IsValidIndex(N.ArtIndex))FSlateDrawElement::MakeBox(Out,L+6,G.ToPaintGeometry(FVector2D(27,33),FSlateLayoutTransform(Troop)),Portraits[N.ArtIndex].Get());
   FSlateDrawElement::MakeText(Out,L+7,G.ToPaintGeometry(FVector2D(45,25),FSlateLayoutTransform(Troop+FVector2D(33,5))),FString::Printf(TEXT("× %d"),N.Garrison),Font,ESlateDrawEffect::None,FLinearColor(.95f,.92f,.83f));
  }
 }
 return L+8;
}
