#include "UI/Screens/PadmaGameScreen.h"
#include "UI/Screens/PadmaActivatableLayer.h"
#include "UI/Input/PadmaPressGesture.h"
#include "UI/Screens/PadmaTutorialMapWidget.h"
#include "UI/Screens/PadmaMotionWidgets.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Blueprint/WidgetTree.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Layout/SWrapBox.h"
#include "UObject/StrongObjectPtr.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SBackgroundBlur.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SLeafWidget.h"
#include "Engine/Texture2D.h"
#include "Styling/CoreStyle.h"
#include "Brushes/SlateColorBrush.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "Framework/Application/SlateApplication.h"
#include "Rendering/DrawElements.h"

namespace
{
 const FLinearColor Ink(.9f,.88f,.8f), Muted(.56f,.62f,.63f), Gold(.82f,.61f,.3f), Jade(.12f,.65f,.52f);
 const FLinearColor Surface(.008f,.022f,.036f,.97f), Line(.4f,.29f,.14f,.85f), Paper(.88f,.8f,.63f);
 FSlateBrush PanelBrush, GoldButtonBrush, PaperBrush;
 constexpr float Sidebar=350, HUDTop=90, HUDHandHeight=258;
 TSharedRef<SWidget> Text(const FString& S,int32 Size=15,FLinearColor Color=Ink,bool Wrap=true)
 {
  return SNew(STextBlock).Text(FText::FromString(S)).Font(FCoreStyle::GetDefaultFontStyle("Regular",Size)).ColorAndOpacity(Color).AutoWrapText(Wrap);
 }
 TSharedRef<SWidget> Frame(TSharedRef<SWidget> Content,FMargin Padding=FMargin(14),FLinearColor Border=Line)
 {
  if(PanelBrush.GetResourceObject())return SNew(SBorder).Padding(1).BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush")).BorderBackgroundColor(Border)
   [SNew(SBorder).Padding(Padding+FMargin(2)).BorderImage(&PanelBrush)[Content]];
  return SNew(SBorder).Padding(1).BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush")).BorderBackgroundColor(Border)
   [SNew(SBorder).Padding(Padding).BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush")).BorderBackgroundColor(Surface)[Content]];
 }
 const FButtonStyle& ButtonStyle()
 {
  static FButtonStyle Style;
  FSlateBrush Normal=FSlateRoundedBoxBrush(Surface,3.f,FLinearColor(.34f,.28f,.17f),1.f);
  FSlateBrush Hover=FSlateRoundedBoxBrush(FLinearColor(.045f,.085f,.10f),3.f,Gold,1.f);
  FSlateBrush Pressed=FSlateRoundedBoxBrush(FLinearColor(.035f,.15f,.14f),3.f,Jade,1.f);
  Style=FButtonStyle(FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("Button")).SetNormal(Normal).SetHovered(Hover).SetPressed(Pressed);
  return Style;
 }
 const FButtonStyle& PrimaryButtonStyle()
 {
  static FButtonStyle Style;FSlateBrush Hover=GoldButtonBrush,Pressed=GoldButtonBrush;
  Hover.TintColor=FLinearColor(1.2f,1.12f,.95f);Pressed.TintColor=FLinearColor(.78f,.72f,.6f);
  Style=FButtonStyle(ButtonStyle()).SetNormal(GoldButtonBrush).SetHovered(Hover).SetPressed(Pressed);
  return Style;
 }
 TSharedRef<SWidget> SmallButton(UPadmaGameScreen* Screen,const FPadmaUIEntry& E,float FontSize=14)
 {
  TWeakObjectPtr<UPadmaGameScreen> Weak(Screen);
  return SNew(SButton).ButtonStyle(&ButtonStyle()).IsEnabled(E.bEnabled).ContentPadding(FMargin(5,2))
   .ButtonColorAndOpacity(E.bSelected?FLinearColor(1.55f,1.4f,.94f):FLinearColor::White)
   .ToolTipText(FText::FromString(E.Description)).OnClicked_Lambda([Weak,E]{if(Weak.IsValid()&&Weak->OnIntent)Weak->OnIntent(E.Action);return FReply::Handled();})
   [SNew(STextBlock).Text(FText::FromString(E.Title)).Font(FCoreStyle::GetDefaultFontStyle("Regular",int32(FontSize))).ColorAndOpacity(E.bSelected?Gold:Ink).Justification(ETextJustify::Center).MinDesiredWidth(24)];
 }
 TSharedRef<SWidget> RuleLine(){return SNew(SBox).HeightOverride(1)[SNew(SBorder).Padding(0).BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush")).BorderBackgroundColor(FLinearColor(.34f,.39f,.39f,.6f))];}

 bool SameAction(const FPadmaUIAction& A,const FPadmaUIAction& B){return A.Command==B.Command&&A.Id==B.Id&&A.Other==B.Other&&A.bDropped==B.bDropped&&A.ScreenPosition==B.ScreenPosition;}
 bool SameEntry(const FPadmaUIEntry& A,const FPadmaUIEntry& B)
 {
  return A.Title==B.Title&&A.Description==B.Description&&SameAction(A.Action,B.Action)&&SameAction(A.InspectAction,B.InspectAction)
   &&A.bEnabled==B.bEnabled&&A.bSelected==B.bSelected&&A.bDraggable==B.bDraggable&&A.bInspectable==B.bInspectable&&A.bPrimary==B.bPrimary&&A.bCard==B.bCard&&A.ArtIndex==B.ArtIndex&&A.Badge==B.Badge;
 }
 bool SameEntries(const TArray<FPadmaUIEntry>& A,const TArray<FPadmaUIEntry>& B)
 {if(A.Num()!=B.Num())return false;for(int32 I=0;I<A.Num();++I)if(!SameEntry(A[I],B[I]))return false;return true;}
 bool SameHeader(const FPadmaGameView& A,const FPadmaGameView& B)
 {
  if(A.CalendarTitle!=B.CalendarTitle||A.DayText!=B.DayText||A.PhaseIndex!=B.PhaseIndex||!SameEntries(A.Navigation,B.Navigation)||A.Resources.Num()!=B.Resources.Num())return false;
  for(int32 I=0;I<A.Resources.Num();++I){const auto& X=A.Resources[I];const auto& Y=B.Resources[I];if(X.Label!=Y.Label||X.Value!=Y.Value||X.Hint!=Y.Hint||X.Color!=Y.Color)return false;}return true;
 }

 class SPadmaIntentTile:public SBorder
 {
 public:
  SLATE_BEGIN_ARGS(SPadmaIntentTile) {}
   SLATE_ARGUMENT(FPadmaUIEntry,Entry)
   SLATE_ARGUMENT(TSharedPtr<FSlateBrush>,Art)
   SLATE_EVENT(FSimpleDelegate,Click)
   SLATE_EVENT(FSimpleDelegate,Inspect)
   SLATE_EVENT(FSimpleDelegate,Drop)
  SLATE_END_ARGS()
  void Construct(const FArguments& A)
  {
   Item=A._Entry;Art=A._Art;Click=A._Click;Inspect=A._Inspect;Drop=A._Drop;
   TSharedRef<SVerticalBox> Body=SNew(SVerticalBox);
   if(Item.bCard)
   {
    FLinearColor Family=Item.Badge==TEXT("B")?FLinearColor(.29f,.16f,.38f):Item.Badge==TEXT("C")?FLinearColor(.4f,.15f,.15f):FLinearColor(.05f,.29f,.26f);
    Body->AddSlot().AutoHeight()[SNew(SBox).HeightOverride(31)[SNew(SBorder).Padding(FMargin(5,3)).BorderImage(&PaperBrush)
     [SNew(SHorizontalBox)
      +SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)[SNew(SBorder).Padding(FMargin(4,0)).BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush")).BorderBackgroundColor(Family)[Text(Item.Badge,17,Paper,false)]]
      +SHorizontalBox::Slot().FillWidth(1).Padding(5,0).VAlign(VAlign_Center)[SNew(SScaleBox).Stretch(EStretch::ScaleToFit).StretchDirection(EStretchDirection::DownOnly)[Text(Item.Title,13,Surface,false)]]]]];
    if(Art)Body->AddSlot().FillHeight(1)[SNew(SBorder).Padding(FMargin(5,0)).BorderImage(&PaperBrush)[SNew(SImage).Image(Art.Get())]];
    else Body->AddSlot().FillHeight(1)[SNew(SBorder).Padding(10).HAlign(HAlign_Center).VAlign(VAlign_Center)
     .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush")).BorderBackgroundColor(Family)[Text(Item.Badge.IsEmpty()?TEXT("◇"):Item.Badge,40,Paper)]];
    Body->AddSlot().AutoHeight()[SNew(SBox).HeightOverride(28)[SNew(SBorder).Padding(FMargin(5,4)).BorderImage(&PaperBrush)[Text(Item.Description,10,Surface)]]];
   }
   else
   {
    Body->AddSlot().AutoHeight()[Text(Item.Title,15,Ink)];
    if(!Item.Description.IsEmpty())Body->AddSlot().AutoHeight().Padding(0,4,0,0)[Text(Item.Description,13,Item.bSelected?Ink:Muted)];
   }
   TSharedRef<SWidget> TileBody=Body;
   if(Art&&!Item.bCard)TileBody=SNew(SHorizontalBox)
    +SHorizontalBox::Slot().AutoWidth().Padding(0,0,10,0)[SNew(SBox).WidthOverride(48).HeightOverride(58)[SNew(SImage).Image(Art.Get())]]
    +SHorizontalBox::Slot().FillWidth(1).VAlign(VAlign_Center)[Body];
   SBorder::Construct(SBorder::FArguments().Padding(Item.bCard?2:9).BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
    .BorderBackgroundColor_Lambda([this]{return Item.bSelected?(Item.bCard?Gold:FLinearColor(.08f,.19f,.2f)):IsHovered()?FLinearColor(.09f,.17f,.18f):Item.bCard?Line:Surface;})[TileBody]);
   SetToolTipText(FText::FromString(Item.Title+TEXT("\n")+Item.Description));SetEnabled(Item.bEnabled);
  }
  virtual FReply OnMouseButtonDown(const FGeometry&,const FPointerEvent& E) override
  {
   if(E.GetEffectingButton()!=EKeys::LeftMouseButton||!Item.bEnabled)return FReply::Unhandled();
   Gesture.Begin(E.GetScreenSpacePosition(),FPlatformTime::Seconds());
   return FReply::Handled().CaptureMouse(SharedThis(this));
  }
  virtual FReply OnMouseMove(const FGeometry&,const FPointerEvent& E) override
  {
   Gesture.Move(E.GetScreenSpacePosition());return Gesture.bHeld?FReply::Handled():FReply::Unhandled();
  }
  virtual void Tick(const FGeometry& G,double Now,float Delta) override
  {
   SBorder::Tick(G,Now,Delta);
   if(Gesture.Inspect(FPlatformTime::Seconds(),Item.bInspectable))Inspect.ExecuteIfBound();
  }
  virtual FReply OnMouseButtonUp(const FGeometry&,const FPointerEvent& E) override
  {
   if(!Gesture.bHeld||E.GetEffectingButton()!=EKeys::LeftMouseButton)return FReply::Unhandled();
   const auto Result=Gesture.Release(Item.bDraggable);
   if(Result==FPadmaPressGesture::ERelease::Drop)Drop.ExecuteIfBound();
   else if(Result==FPadmaPressGesture::ERelease::Click)Click.ExecuteIfBound();
   return FReply::Handled().ReleaseMouseCapture();
  }
  virtual void OnMouseCaptureLost(const FCaptureLostEvent& E) override{Gesture.Cancel();SBorder::OnMouseCaptureLost(E);}
 private:
  FPadmaUIEntry Item;TSharedPtr<FSlateBrush> Art;FSimpleDelegate Click,Inspect,Drop;
  FPadmaPressGesture Gesture;
 };
 class SPadmaMinimap:public SLeafWidget
 {
 public:
  SLATE_BEGIN_ARGS(SPadmaMinimap){}
   SLATE_ARGUMENT(TArray<FPadmaMinimapNode>,Nodes)
   SLATE_ARGUMENT(TArray<FPadmaMinimapEdge>,Edges)
   SLATE_ARGUMENT(TFunction<void(FName)>,Select)
  SLATE_END_ARGS()
  void Construct(const FArguments& A){Nodes=A._Nodes;Edges=A._Edges;Select=A._Select;Bounds=FBox2D(ForceInit);for(const auto& N:Nodes)Bounds+=N.Position;}
  virtual FVector2D ComputeDesiredSize(float)const override{return FVector2D(210,150);}
  FVector2D Point(int32 I,FVector2D Size)const
  {
   const FVector2D Range=Bounds.GetSize();
   const double Scale=FMath::Min((Size.X-30)/FMath::Max(1.,Range.X),(Size.Y-28)/FMath::Max(1.,Range.Y));
   return (Size-Range*Scale)*.5+(Nodes[I].Position-Bounds.Min)*Scale;
  }
  virtual int32 OnPaint(const FPaintArgs&,const FGeometry& G,const FSlateRect&,FSlateWindowElementList& Out,int32 Layer,const FWidgetStyle&,bool)const override
  {
   for(const auto& E:Edges)if(Nodes.IsValidIndex(E.From)&&Nodes.IsValidIndex(E.To))
   {
    TArray<FVector2D> P={Point(E.From,G.GetLocalSize()),Point(E.To,G.GetLocalSize())};
    FSlateDrawElement::MakeLines(Out,Layer,G.ToPaintGeometry(),P,ESlateDrawEffect::None,E.bLocked?Muted:Jade,true,E.bLocked?1:1.5f);
   }
   for(int32 I=0;I<Nodes.Num();++I)
   {
    auto P=Point(I,G.GetLocalSize());const float R=Nodes[I].bSelected?7:4;
    FSlateDrawElement::MakeBox(Out,Layer+1,G.ToPaintGeometry(FVector2D(R*2,R*2),FSlateLayoutTransform(P-FVector2D(R,R))),FCoreStyle::Get().GetBrush("WhiteBrush"),ESlateDrawEffect::None,Nodes[I].bSelected?Gold:Nodes[I].Color);
   }
   return Layer+1;
  }
  virtual FReply OnMouseButtonDown(const FGeometry& G,const FPointerEvent& E)override
  {
   if(E.GetEffectingButton()!=EKeys::LeftMouseButton)return FReply::Unhandled();
   const auto P=G.AbsoluteToLocal(E.GetScreenSpacePosition());
   for(int32 I=0;I<Nodes.Num();++I)if(FVector2D::Distance(P,Point(I,G.GetLocalSize()))<13){if(Select)Select(Nodes[I].Id);break;}
   return FReply::Handled();
  }
 private:
  TArray<FPadmaMinimapNode> Nodes;TArray<FPadmaMinimapEdge> Edges;TFunction<void(FName)> Select;FBox2D Bounds;
 };
}
TSharedRef<SWidget> UPadmaGameScreen::RebuildWidget()
{
 Super::RebuildWidget();CardAtlas=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Padma/UI/Textures/T_CardIllustrations.T_CardIllustrations"));
 PanelTexture=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Padma/UI/Textures/T_HUDPanel.T_HUDPanel"));
 ButtonTexture=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Padma/UI/Textures/T_HUDButton.T_HUDButton"));
 TutorialTexture=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Padma/UI/Textures/T_TutorialMap.T_TutorialMap"));
 TutorialBrush=MakeShared<FSlateBrush>();TutorialBrush->ImageSize=FVector2D(1774,887);
 if(auto* Base=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Padma/UI/Materials/MI_TutorialMap.MI_TutorialMap")))
 {
  TutorialMaterial=UMaterialInstanceDynamic::Create(Base,this);TutorialBrush->SetResourceObject(TutorialMaterial);
#if !UE_BUILD_SHIPPING
  float ArtTime=0;
  if(FParse::Value(FCommandLine::Get(),TEXT("PadmaMapArtTime="),ArtTime))
  {TutorialMaterial->SetScalarParameterValue(TEXT("FreezeAnimation"),1);TutorialMaterial->SetScalarParameterValue(TEXT("PreviewTime"),ArtTime);}
#endif
 }
 else if(TutorialTexture)TutorialBrush->SetResourceObject(TutorialTexture);
 PanelBrush=FSlateColorBrush(Surface);GoldButtonBrush=FSlateColorBrush(Gold);PaperBrush=FSlateColorBrush(Paper);SkinMaterials.Reset();
 auto Skin=[&](const TCHAR* Name,FSlateBrush* Brush)->UMaterialInstanceDynamic*
 {
  auto* Base=LoadObject<UMaterialInterface>(nullptr,*FString::Printf(TEXT("/Game/Padma/UI/Materials/MI_%s.MI_%s"),Name,Name));if(!Base)return nullptr;
  auto* Material=UMaterialInstanceDynamic::Create(Base,this);SkinMaterials.Add(Material);
  if(Brush){Brush->SetResourceObject(Material);Brush->DrawAs=ESlateBrushDrawType::Image;Brush->TintColor=FLinearColor::White;}
#if !UE_BUILD_SHIPPING
  float ArtTime=0;if(FParse::Value(FCommandLine::Get(),TEXT("PadmaMapArtTime="),ArtTime)){Material->SetScalarParameterValue(TEXT("FreezeAnimation"),1);Material->SetScalarParameterValue(TEXT("PreviewTime"),ArtTime);}
#endif
  return Material;
 };
 Skin(TEXT("UIInkPanel"),&PanelBrush);Skin(TEXT("UIGoldAction"),&GoldButtonBrush);Skin(TEXT("UIPaper"),&PaperBrush);
 WarMaterial=Skin(TEXT("UIWarLiquid"),nullptr);RevealMaterial=Skin(TEXT("UIPortraitReveal"),nullptr);
 if(!WidgetTree)WidgetTree=NewObject<UWidgetTree>(this,TEXT("NativeWidgetTree"));
 PageStack=WidgetTree->ConstructWidget<UCommonActivatableWidgetStack>();OverlayStack=WidgetTree->ConstructWidget<UCommonActivatableWidgetStack>();
 PageStack->SetTransitionDuration(0);OverlayStack->SetTransitionDuration(0);
 SAssignNew(Layout,SOverlay);SAssignNew(RootLayout,SOverlay);
 RootLayout->AddSlot()[PageStack->TakeWidget()];RootLayout->AddSlot()[OverlayStack->TakeWidget()];
 Build();SyncLayers();return RootLayout.ToSharedRef();
}
void UPadmaGameScreen::ReleaseSlateResources(bool Children)
{
 // These stacks are constructed manually rather than through WidgetTree.RootWidget.
 // Release their pooled SObjectWidgets explicitly, before GC inspects the PIE session.
 if(PaintedMap)PaintedMap->CancelInteraction();
 if(RootLayout)RootLayout->ClearChildren();
 if(OverlayStack)static_cast<UWidget*>(OverlayStack.Get())->ReleaseSlateResources(true);
 if(PageStack)static_cast<UWidget*>(PageStack.Get())->ReleaseSlateResources(true);
 Super::ReleaseSlateResources(Children);Layout.Reset();RootLayout.Reset();Markers.Reset();WorldArea.Reset();PaintedMap.Reset();TutorialMini.Reset();TutorialHeader.Reset();TutorialDetails.Reset();TutorialHand.Reset();TutorialPhase.Reset();HandScroll.Reset();WarBalance.Reset();GenericDetails.Reset();PortraitBrush.Reset();PageLayer=nullptr;OverlayLayers.Reset();
 OverlayStack=nullptr;PageStack=nullptr;TutorialBrush.Reset();
 SkinMaterials.Reset();PortraitMaterials.Reset();WarMaterial=nullptr;RevealMaterial=nullptr;TutorialMaterial=nullptr;
 bDeferredView=false;
}
void UPadmaGameScreen::ShutdownPresentation()
{
 OnIntent=nullptr;
 if(PaintedMap)PaintedMap->CancelInteraction();
 DeactivateWidget();RemoveFromParent();
 ReleaseSlateResources(true);
}
TOptional<FUIInputConfig> UPadmaGameScreen::GetDesiredInputConfig() const
{
 return FUIInputConfig(ECommonInputMode::All,EMouseCaptureMode::NoCapture,EMouseLockMode::DoNotLock,false);
}
bool UPadmaGameScreen::IsWorldPointerAvailable() const
{
 if(Current.bPaintedTutorial&&Current.bWorldMap)
 {
  if(HasModal()||!PaintedMap)return false;
  const auto P=FSlateApplication::Get().GetCursorPos();const auto& G=PaintedMap->GetCachedGeometry();
  if(G.GetLocalSize().X<1||!G.IsUnderLocation(P))return false;
  const auto Local=G.AbsoluteToLocal(P);
  // Floating objective and balance displays sit over the painting, never pass clicks through.
  return !(Local.X<280&&Local.Y<142)&&!(Local.X>540&&Local.X<1060&&Local.Y<36);
 }
 if(HasModal()||!WorldArea.IsValid()||!Layout.IsValid())return false;
 const auto PointerPosition=FSlateApplication::Get().GetCursorPos();
 if(WorldArea->GetCachedGeometry().GetLocalSize().X<=0||!WorldArea->GetCachedGeometry().IsUnderLocation(PointerPosition))return false;
 // Exclude the actual arranged HUD panels, including floating objective/notice boxes.
 const auto* Children=Layout->GetChildren();
 for(int32 I=2;I<Children->Num();++I)
 {
  const auto Child=Children->GetChildAt(I);
  if(Child->GetVisibility().IsHitTestVisible()&&Child->GetCachedGeometry().IsUnderLocation(PointerPosition))return false;
 }
 return true;
}
FVector2D UPadmaGameScreen::GetWorldAreaSize()const{return WorldArea.IsValid()?WorldArea->GetCachedGeometry().GetLocalSize():FVector2D::ZeroVector;}
FVector2D UPadmaGameScreen::GetWorldAreaCenter()const
{
 if(!WorldArea.IsValid()||!Layout.IsValid())return FVector2D(.5,.5);
 const auto& G=Layout->GetCachedGeometry();const auto Size=G.GetLocalSize();
 if(Size.X<=0||Size.Y<=0)return FVector2D(.5,.5);
 return G.AbsoluteToLocal(WorldArea->GetCachedGeometry().GetAbsolutePositionAtCoordinates(FVector2D(.5,.5)))/Size;
}
void UPadmaGameScreen::PresentLabels(const TArray<FPadmaWorldLabelView>& Labels)
{
 if(!Markers.IsValid())return;Markers->ClearChildren();
 for(const auto& L:Labels)Markers->AddSlot().Offset(FMargin(L.Position.X,L.Position.Y,0,0)).Alignment(FVector2D(.5,1)).AutoSize(true)[Frame(Text(L.Text,15,L.Color,false),FMargin(9,4))];
}
void UPadmaGameScreen::Present(const FPadmaGameView& View)
{
 if(View.bDetailOnly)
 {
  if(!Current.bWorldMap||HasModal()||(Current.bPaintedTutorial?!TutorialDetails:!GenericDetails))return;
  // A preview never replaces a complete deferred transaction or any persistent HUD state.
  Current.DetailNode=View.DetailNode;Current.MoveSource=View.MoveSource;Current.RightTitle=View.RightTitle;Current.RightText=View.RightText;
  Current.LeftTitle=View.LeftTitle;Current.Left=View.Left;Current.Right=View.Right;Current.NodeArtIndex=View.NodeArtIndex;
  Current.TerrainText=View.TerrainText;Current.OwnershipText=View.OwnershipText;Current.NodeStateText=View.NodeStateText;Current.NodeUV=View.NodeUV;
  Current.EnemyName=View.EnemyName;Current.EnemyCount=View.EnemyCount;Current.EnemyHealth=View.EnemyHealth;
  Current.MovementRoute=View.MovementRoute;Current.MovingCards=View.MovingCards;Current.HexGrid=View.HexGrid;
  Current.MinimapNodes=View.MinimapNodes;Current.MinimapEdges=View.MinimapEdges;
  if(Current.bPaintedTutorial){TutorialDetails->SetContent(BuildTutorialDetails());if(PaintedMap)PaintedMap->Update(Current);}
  else GenericDetails->SetContent(BuildGenericDetails());return;
 }
 if(PaintedMap&&(View.PageKey!=Current.PageKey||View.OverlayPath!=Current.OverlayPath||View.ModalTitle!=Current.ModalTitle||!View.bWorldMap||!View.bPaintedTutorial))PaintedMap->CancelInteraction();
 if(FSlateApplication::Get().HasAnyMouseCaptor()&&View.PageKey==Current.PageKey&&View.OverlayPath==Current.OverlayPath&&View.ModalTitle==Current.ModalTitle)
 {DeferredView=View;bDeferredView=true;return;}
 bDeferredView=false;
 const FPadmaGameView Before=Current;
 const bool Retain=Layout&&TutorialHeader&&TutorialHand&&TutorialDetails&&Current.bPaintedTutorial&&Current.bWorldMap&&View.bPaintedTutorial&&View.bWorldMap&&Current.PageKey==View.PageKey;
 Current=View;CurrentPortrait=View.ModalPortrait;PortraitBrush=ArtBrush(View.ModalArtIndex,CurrentPortrait);
 if(Retain)
 {
  if(!SameHeader(Before,Current))TutorialHeader->SetContent(BuildTutorialHeader());
  if(!SameEntries(Before.Hand,Current.Hand)||!SameEntries(Before.HandFilters,Current.HandFilters)||!SameEntries(Before.HandActions,Current.HandActions)||Before.DeckCount!=Current.DeckCount||Before.DiscardCount!=Current.DiscardCount)TutorialHand->SetContent(BuildTutorialHand());
  if(Before.PhaseIndex!=Current.PhaseIndex||!SameEntry(Before.PhaseAction,Current.PhaseAction))TutorialPhase->SetContent(BuildTutorialPhase());
  TutorialDetails->SetContent(BuildTutorialDetails());PaintedMap->Update(Current);if(TutorialMini)TutorialMini->Update(Current);
  if(WarBalance)WarBalance->SetBalance(Current.WarProgress);
 }
 else Build();
 SyncLayers();
}
void UPadmaGameScreen::NativeTick(const FGeometry& Geometry,float Delta)
{
 Super::NativeTick(Geometry,Delta);
 if(bDeferredView&&!FSlateApplication::Get().HasAnyMouseCaptor())Present(DeferredView);
 if(RevealMaterial&&PortraitRevealTime<.6f){PortraitRevealTime+=Delta;const float T=FMath::Clamp((PortraitRevealTime-.12f)/.42f,0.f,1.f);RevealMaterial->SetScalarParameterValue(TEXT("RevealAmount"),1.f-FMath::Pow(1.f-T,3.f));}
}
void UPadmaGameScreen::Intent(const FPadmaUIAction& Action){if(OnIntent)OnIntent(Action);}
TSharedRef<SWidget> UPadmaGameScreen::Entry(const FPadmaUIEntry& Value)
{
 TWeakObjectPtr<UPadmaGameScreen> Weak(this);
 if(Value.bDraggable||Value.bInspectable||Value.bCard)
 {
  TSharedPtr<FSlateBrush> Art;
  if(CardAtlas&&Value.ArtIndex>=0&&Value.ArtIndex<9)
  {
   Art=MakeShared<FSlateBrush>();Art->SetResourceObject(CardAtlas);Art->ImageSize=FVector2D(160,130);
   const int32 Col=Value.ArtIndex%3,Row=Value.ArtIndex/3;
   Art->SetUVRegion(FBox2D(FVector2D((Col+.005)/3.,(Row+.005)/3.),FVector2D((Col+.995)/3.,(Row+.995)/3.)));
  }
  return SNew(SPadmaIntentTile).Entry(Value).Art(Art)
   .Click(FSimpleDelegate::CreateLambda([Weak,Value]{if(Weak.IsValid())Weak->Intent(Value.Action);}))
   .Inspect(FSimpleDelegate::CreateLambda([Weak,Value]{if(Weak.IsValid()){auto A=Value.InspectAction;if(A.Command.IsNone()){A=Value.Action;A.Command=TEXT("inspect-unit");}Weak->Intent(A);}}))
   .Drop(FSimpleDelegate::CreateLambda([Weak,Value]{if(Weak.IsValid()){auto A=Value.Action;A.bDropped=true;Weak->Intent(A);}}));
 }
 auto Content=SNew(SVerticalBox);
 Content->AddSlot().AutoHeight().HAlign(Value.bPrimary?HAlign_Center:HAlign_Left)[Text(Value.Title,Value.bPrimary?18:15,!Value.bEnabled?Ink:Value.bPrimary?FLinearColor(.02f,.025f,.025f):Value.bSelected?Gold:Ink,false)];
 if(!Value.Description.IsEmpty())Content->AddSlot().AutoHeight().Padding(0,4,0,0)[Text(Value.Description,13,Value.bPrimary&&Value.bEnabled?FLinearColor(.12f,.13f,.12f):Muted)];
 auto Style=MakeShared<FButtonStyle>(Value.bPrimary?PrimaryButtonStyle():ButtonStyle());
 TSharedPtr<TStrongObjectPtr<UMaterialInstanceDynamic>> HoverMaterial;
 if(Value.bPrimary)if(auto* Parent=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Padma/UI/Materials/MI_UIGoldAction.MI_UIGoldAction")))
 {
  // A Slate-owned strong reference must never retain the owning PIE widget through Outer.
  auto* Material=UMaterialInstanceDynamic::Create(Parent,GetTransientPackage());HoverMaterial=MakeShared<TStrongObjectPtr<UMaterialInstanceDynamic>>(Material);
#if !UE_BUILD_SHIPPING
  float ArtTime=0;if(FParse::Value(FCommandLine::Get(),TEXT("PadmaMapArtTime="),ArtTime)){Material->SetScalarParameterValue(TEXT("FreezeAnimation"),1);Material->SetScalarParameterValue(TEXT("PreviewTime"),ArtTime);}
#endif
  FSlateBrush Normal=GoldButtonBrush;Normal.SetResourceObject(Material);Style->SetNormal(Normal).SetHovered(Normal).SetPressed(Normal);
 }
 return SNew(SButton).ButtonStyle(&Style.Get()).IsFocusable(true).IsEnabled(Value.bEnabled)
  .ButtonColorAndOpacity(Value.bSelected?FLinearColor(1.3f,1.6f,1.4f):FLinearColor::White)
  .ToolTipText(FText::FromString(Value.Description)).ContentPadding(Value.bPrimary?FMargin(18,17):FMargin(13,10))
  .OnHovered_Lambda([HoverMaterial]{if(HoverMaterial)HoverMaterial->Get()->SetScalarParameterValue(TEXT("HoverAmount"),1);})
  .OnUnhovered_Lambda([HoverMaterial]{if(HoverMaterial)HoverMaterial->Get()->SetScalarParameterValue(TEXT("HoverAmount"),0);})
  .OnClicked_Lambda([Weak,Value,Style,HoverMaterial]{if(Weak.IsValid())Weak->Intent(Value.Action);return FReply::Handled();})[Content];
}
void UPadmaGameScreen::Build()
{
 if(!Layout.IsValid())return;Layout->ClearChildren();
 if(Current.bPaintedTutorial&&Current.bWorldMap){BuildTutorial();return;}
 TutorialHeader.Reset();TutorialDetails.Reset();TutorialHand.Reset();TutorialPhase.Reset();TutorialMini.Reset();WarBalance.Reset();HandScroll.Reset();
 const bool Map=Current.bWorldMap,ShowHand=Map||!Current.Hand.IsEmpty();const float LeftInset=Map?12:266;
 const float HandInset=ShowHand?HUDHandHeight:12;
 Layout->AddSlot().Padding(LeftInset,HUDTop+36,Sidebar+28,HandInset+12)[SAssignNew(WorldArea,SBox).Visibility(EVisibility::HitTestInvisible)];
 SAssignNew(Markers,SConstraintCanvas).Visibility(EVisibility::HitTestInvisible);Layout->AddSlot()[Markers.ToSharedRef()];
 if(Current.bBlurWorld)Layout->AddSlot()[SNew(SBackgroundBlur).BlurStrength(3).Visibility(EVisibility::HitTestInvisible)];
 auto Header=SNew(SHorizontalBox);
 Header->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(8,0,26,0)[SNew(SVerticalBox)
  +SVerticalBox::Slot().AutoHeight()[Text(TEXT("Dream of Padma"),24,Gold,false)]
  +SVerticalBox::Slot().AutoHeight().Padding(0,4,0,0)[Text(TEXT("莲 华 之 梦"),13,Muted,false)]];
 Header->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(0,0,24,0)[SNew(SVerticalBox)
  +SVerticalBox::Slot().AutoHeight()[Text(Current.CalendarTitle,18,Ink,false)]
  +SVerticalBox::Slot().AutoHeight().Padding(0,4,0,0)[Text(Current.DayText,14,Muted,false)]];
 auto Phases=SNew(SHorizontalBox);const TCHAR* PhaseNames[]={TEXT("黎明"),TEXT("晌午"),TEXT("黄昏")};
 for(int32 I=0;I<3;++I)Phases->AddSlot().AutoWidth().Padding(3,0)[SNew(SBorder).Padding(FMargin(12,9))
  .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush")).BorderBackgroundColor(I==Current.PhaseIndex?FLinearColor(.14f,.13f,.085f):Surface)[Text(PhaseNames[I],16,I==Current.PhaseIndex?Gold:Muted,false)]];
 Header->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(0,0,16,0)[Phases];
 for(const auto& R:Current.Resources)Header->AddSlot().FillWidth(1).VAlign(VAlign_Center).Padding(8,0)[SNew(SVerticalBox)
  +SVerticalBox::Slot().AutoHeight()[Text(R.Label,13,Muted,false)]
  +SVerticalBox::Slot().AutoHeight().Padding(0,3,0,0)[SNew(STextBlock).Text(FText::FromString(R.Value)).Font(FCoreStyle::GetDefaultFontStyle("Regular",21)).ColorAndOpacity(R.Color).ToolTipText(FText::FromString(R.Hint))]];
 for(const auto& E:Current.Navigation)Header->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(4,0)[Entry(E)];
 Layout->AddSlot().VAlign(VAlign_Top).Padding(0,0,0,0)[SNew(SBox).HeightOverride(76)[Frame(Header,FMargin(16,5))]];
 Layout->AddSlot().HAlign(HAlign_Center).VAlign(VAlign_Top).Padding(0,77,Sidebar,0)[SNew(SBox).WidthOverride(410)[Frame(SNew(SHorizontalBox)
  +SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0,0,12,0)[Text(Current.WarText,13,Gold,false)]
  +SHorizontalBox::Slot().FillWidth(1).VAlign(VAlign_Center)[SNew(SBox).HeightOverride(5)[SNew(SProgressBar).Percent(Current.WarProgress).FillColorAndOpacity(Jade)]],FMargin(12,6))]];
 auto RightColumn=SNew(SVerticalBox);
 RightColumn->AddSlot().FillHeight(1)[SAssignNew(GenericDetails,SBox)[BuildGenericDetails()]];
 if(Map)
 {
  auto PhaseBox=SNew(SVerticalBox);
  PhaseBox->AddSlot().AutoHeight().HAlign(HAlign_Center).Padding(0,0,0,10)[Text(Current.PhaseHint,14,Gold)];
  PhaseBox->AddSlot().AutoHeight()[Entry(Current.PhaseAction)];
  RightColumn->AddSlot().AutoHeight().Padding(0,10,0,0)[Frame(PhaseBox,FMargin(14))];
 }
 Layout->AddSlot().HAlign(HAlign_Right).Padding(0,HUDTop,12,12)[SNew(SBox).WidthOverride(Sidebar)[RightColumn]];
 if(Map)
 {
  Layout->AddSlot().HAlign(HAlign_Left).VAlign(VAlign_Top).Padding(12,HUDTop,0,0)[SNew(SBox).WidthOverride(245)[Frame(SNew(SVerticalBox)
   +SVerticalBox::Slot().AutoHeight()[Text(TEXT("章 节 目 标"),15,Gold)]
   +SVerticalBox::Slot().AutoHeight().Padding(0,6)[Text(Current.ObjectiveText,14)],FMargin(12))]];
  TWeakObjectPtr<UPadmaGameScreen> Weak(this);auto MapBox=SNew(SVerticalBox);
  MapBox->AddSlot().AutoHeight()[Text(TEXT("区域总览"),14,Gold)];
  MapBox->AddSlot().FillHeight(1)[SNew(SPadmaMinimap).Nodes(Current.MinimapNodes).Edges(Current.MinimapEdges).Select([Weak](FName Id){if(Weak.IsValid())Weak->Intent({TEXT("select-node"),Id});})];
  FPadmaUIEntry Fit;Fit.Title=TEXT("地图全览  ·  Home");Fit.Action.Command=TEXT("map-fit");MapBox->AddSlot().AutoHeight()[Entry(Fit)];
  Layout->AddSlot().HAlign(HAlign_Left).VAlign(VAlign_Bottom).Padding(12,0,0,12)[SNew(SBox).WidthOverride(244).HeightOverride(HUDHandHeight-12)[Frame(MapBox,FMargin(12))]];
 }
 else if(!Current.LeftTitle.IsEmpty()||Current.Left.Num())
 {
  auto Left=SNew(SVerticalBox);Left->AddSlot().AutoHeight().Padding(0,0,0,10)[Text(Current.LeftTitle,18,Gold)];
  for(const auto& E:Current.Left)Left->AddSlot().AutoHeight().Padding(0,4)[Entry(E)];
  Layout->AddSlot().HAlign(HAlign_Left).Padding(12,HUDTop,0,HandInset)[SNew(SBox).WidthOverride(240)[Frame(SNew(SScrollBox)+SScrollBox::Slot()[Left])]];
 }
 auto Hand=SNew(SVerticalBox);auto HandTop=SNew(SHorizontalBox);
 HandTop->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(0,0,16,0)[Text(Map?FString::Printf(TEXT("手牌  %d"),Current.HandCount):Current.HandTitle,17,Gold,false)];
 for(const auto& E:Current.HandFilters)HandTop->AddSlot().AutoWidth().Padding(2,0)[Entry(E)];
 HandTop->AddSlot().FillWidth(1).HAlign(HAlign_Right).VAlign(VAlign_Center).Padding(8,0)[Text(Map?FString::Printf(TEXT("牌库 %d   ·   弃牌 %d"),Current.DeckCount,Current.DiscardCount):Current.HandHint,13,Muted,false)];
 for(auto E:Current.HandActions){E.Description.Reset();HandTop->AddSlot().AutoWidth().Padding(3,0)[Entry(E)];}
 Hand->AddSlot().AutoHeight().Padding(0,0,0,8)[HandTop];auto Cards=SNew(SWrapBox).UseAllottedSize(true);
 for(const auto& E:Current.Hand)Cards->AddSlot().Padding(0,0,8,8)[SNew(SBox).WidthOverride(Map?154:205).HeightOverride(Map?178:165)[Entry(E)]];
 if(Current.Hand.IsEmpty())Cards->AddSlot().VAlign(VAlign_Center)[Text(Map?TEXT("此分类暂无手牌 · 切换「全部」查看其他卡牌"):TEXT("当前未打开技能卡库"),15,Muted)];
 Hand->AddSlot().FillHeight(1)[SNew(SScrollBox).Orientation(Orient_Vertical)+SScrollBox::Slot()[Cards]];
 if(ShowHand)Layout->AddSlot().VAlign(VAlign_Bottom).Padding(Map?268:12,0,Sidebar+24,12)[SNew(SBox).HeightOverride(HUDHandHeight-12)[Frame(Hand,FMargin(12,9))]];
 if(!Current.Notice.IsEmpty())Layout->AddSlot().HAlign(HAlign_Center).VAlign(VAlign_Bottom).Padding(270,0,Sidebar+30,HandInset+10)[SNew(SBox).MaxDesiredWidth(650)[Frame(Text(Current.Notice,14,Ink),FMargin(12,7))]];
}

FName UPadmaGameScreen::HitPaintedNode()const{return IsWorldPointerAvailable()&&PaintedMap?PaintedMap->Hit(FSlateApplication::Get().GetCursorPos()):NAME_None;}
void UPadmaGameScreen::ZoomPaintedMap(float M){if(PaintedMap)PaintedMap->ZoomBy(M);}
void UPadmaGameScreen::PanPaintedMap(FVector2D D)
{
 if(PaintedMap){const float Scale=PaintedMap->GetCachedGeometry().Scale;PaintedMap->PanBy(D/FMath::Max(.01f,Scale));}
}
void UPadmaGameScreen::ResetPaintedMap(){if(PaintedMap)PaintedMap->ResetView();}
void UPadmaGameScreen::BuildTutorial()
{
 Markers.Reset();WorldArea.Reset();GenericDetails.Reset();
 auto Canvas=SNew(SConstraintCanvas);
 auto Place=[&](float X,float Y,float W,float H,TSharedRef<SWidget> Widget)
 {Canvas->AddSlot().Offset(FMargin(X,Y,W,H)).Alignment(FVector2D::ZeroVector)[Widget];};
 Place(0,0,1672,941,SNew(SBorder).BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush")).BorderBackgroundColor(Surface));
 TWeakObjectPtr<UPadmaGameScreen> Weak(this);
 if(!PaintedMap)SAssignNew(PaintedMap,SPadmaTutorialMap).Background(TutorialBrush).Atlas(CardAtlas).Mini(false)
  .Select([Weak](FName Node){if(Weak.IsValid()&&Weak->IsWorldPointerAvailable())Weak->Intent({TEXT("select-node"),Node});})
  .CanNavigate([Weak]{return Weak.IsValid()&&!Weak->HasModal()&&Weak->Current.bWorldMap&&Weak->Current.bPaintedTutorial;});
 PaintedMap->Update(Current);Place(0,56,1296,636,PaintedMap.ToSharedRef());
 Place(0,0,1672,56,SAssignNew(TutorialHeader,SBox)[BuildTutorialHeader()]);
 SAssignNew(WarBalance,SPadmaWarBalance).Material(WarMaterial);WarBalance->SetBalance(Current.WarProgress);
 Place(540,56,520,35,Frame(WarBalance.ToSharedRef(),FMargin(8,0)));
 Place(14,80,255,98,Frame(SNew(SVerticalBox)
  +SVerticalBox::Slot().AutoHeight().Padding(0,0,0,6)[Text(TEXT("章 节 目 标"),16,Gold)]
  +SVerticalBox::Slot().AutoHeight()[Text(TEXT("摧毁统治者核心\n或使战争天平倾向我方"),14,Ink)],FMargin(13,7)));
 Place(1298,60,370,699,SAssignNew(TutorialDetails,SBox)[BuildTutorialDetails()]);
 Place(1298,768,370,139,SAssignNew(TutorialPhase,SBox)[BuildTutorialPhase()]);
 SAssignNew(TutorialMini,SPadmaTutorialMap).Background(TutorialBrush).Atlas(CardAtlas).Mini(true);TutorialMini->Update(Current);
 auto MiniBox=SNew(SVerticalBox);auto Tools=SNew(SHorizontalBox);
 FPadmaUIEntry ZoomIn,ZoomOut,Fit;ZoomIn.Title=TEXT("＋");ZoomIn.Action.Command=TEXT("map-zoom-in");ZoomOut.Title=TEXT("−");ZoomOut.Action.Command=TEXT("map-zoom-out");Fit.Title=TEXT("地图全览");Fit.Action.Command=TEXT("map-fit");
 for(const auto& E:{ZoomIn,ZoomOut,Fit})Tools->AddSlot().AutoWidth().Padding(2,0)[SmallButton(this,E,12)];
 MiniBox->AddSlot().AutoHeight()[Tools];MiniBox->AddSlot().FillHeight(1).Padding(0,7,0,0)[TutorialMini.ToSharedRef()];
 Place(8,695,261,231,Frame(MiniBox,FMargin(7,7)));
 Place(277,695,1014,231,SAssignNew(TutorialHand,SBox)[BuildTutorialHand()]);
 Layout->AddSlot()[SNew(SScaleBox).Stretch(EStretch::ScaleToFit)[SNew(SBox).WidthOverride(1672).HeightOverride(941)[Canvas]]];
}


TSharedRef<SWidget> UPadmaGameScreen::BuildTutorialHeader()
{
 auto Header=SNew(SHorizontalBox);
 Header->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(7,0,15,0)[Text(TEXT("✦"),31,Gold,false)];
 Header->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(0,0,22,0)[Text(TEXT("第零章 · 新手教程"),23,Gold,false)];
 Header->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(0,0,22,0)[Text(Current.CalendarTitle,20,Ink,false)];
 Header->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(0,0,28,0)[Text(Current.DayText,16,Ink,false)];
 auto Phases=SNew(SHorizontalBox);const TCHAR* Names[]={TEXT("◉  黎明"),TEXT("✦  晌午"),TEXT("◉  黄昏")};
 for(int32 I=0;I<3;++I)Phases->AddSlot().AutoWidth().Padding(9,0)[SNew(SVerticalBox)
  +SVerticalBox::Slot().AutoHeight().Padding(0,5)[Text(Names[I],16,I==Current.PhaseIndex?Gold:Muted*.7f,false)]
  +SVerticalBox::Slot().AutoHeight()[SNew(SBox).HeightOverride(3)[SNew(SBorder).Padding(0).BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush")).BorderBackgroundColor(I==Current.PhaseIndex?Gold:Surface)]]];
 Header->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(0,0,15,0)[Phases];
 const TCHAR* Icons[]={TEXT("◉"),TEXT("◇"),TEXT("◈"),TEXT("✦")};int32 RI=0;
 for(const auto& R:Current.Resources)
 {
  Header->AddSlot().FillWidth(1).VAlign(VAlign_Center).Padding(7,0)[SNew(SHorizontalBox)
   +SHorizontalBox::Slot().AutoWidth().Padding(0,0,5,0).VAlign(VAlign_Center)[Text(Icons[FMath::Min(RI,3)],24,R.Color,false)]
   +SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)[Text(R.Label+TEXT(" ")+R.Value,15,Ink,false)]];++RI;
 }
 for(auto E:Current.Navigation){if(E.Action.Command==TEXT("menu"))E.Title=TEXT("菜单");Header->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(4,0)[SmallButton(this,E)];}
return Frame(Header,FMargin(12,1));
}

TSharedRef<SWidget> UPadmaGameScreen::BuildTutorialDetails()
{
 // The side panel previews the last hovered cell; actions carry its stable ID.
 auto Right=SNew(SVerticalBox);
 TSharedPtr<FSlateBrush> Preview=MakeShared<FSlateBrush>();Preview->SetResourceObject(TutorialTexture);Preview->ImageSize=FVector2D(344,163);
 const FVector2D UV=Current.NodeUV;const FVector2D Top(FMath::Clamp(UV.X-.16,0.,.68),FMath::Clamp(UV.Y-.18,0.,.64));
 Preview->SetUVRegion(FBox2D(Top,Top+FVector2D(.32,.36)));
 auto Hero=SNew(SOverlay)
  +SOverlay::Slot()[SNew(SImage).Image_Lambda([Preview]{return Preview.Get();})]
  +SOverlay::Slot().VAlign(VAlign_Bottom)[SNew(SBorder).Padding(FMargin(11,6)).BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush")).BorderBackgroundColor(FLinearColor(.004f,.015f,.025f,.86f))
   [SNew(SVerticalBox)+SVerticalBox::Slot().AutoHeight()[Text(Current.RightTitle,24,Ink)]
    +SVerticalBox::Slot().AutoHeight().Padding(0,2,0,0)[Text(Current.OwnershipText+TEXT(" · ")+Current.TerrainText,13,Current.OwnershipText.Contains(TEXT("统治"))?FLinearColor(.9f,.36f,.28f):Jade)]]];
 Right->AddSlot().AutoHeight()[SNew(SBox).HeightOverride(167)[Hero]];
 Right->AddSlot().AutoHeight().Padding(6,9)[Text(TEXT("地形：    ")+Current.TerrainText,14)];
 Right->AddSlot().AutoHeight()[RuleLine()];
 Right->AddSlot().AutoHeight().Padding(6,8)[Text(TEXT("状态：    ")+Current.NodeStateText,14)];
 Right->AddSlot().AutoHeight()[RuleLine()];
 auto Body=SNew(SVerticalBox);
 Body->AddSlot().AutoHeight().Padding(0,7)[Text(Current.EnemyCount>0?TEXT("› 敌方驻军"):TEXT("› 此地驻军"),16,Gold)];
 if(Current.EnemyCount>0)
 {
  const auto Portrait=ArtBrush(6);
  Body->AddSlot().AutoHeight()[SNew(SHorizontalBox)
   +SHorizontalBox::Slot().AutoWidth().Padding(10,0,13,0)[SNew(SBox).WidthOverride(70).HeightOverride(82)[SNew(SImage).Image_Lambda([Portrait]{return Portrait.Get();})]]
   +SHorizontalBox::Slot().FillWidth(1)[SNew(SVerticalBox)
    +SVerticalBox::Slot().AutoHeight().Padding(0,4)[Text(Current.EnemyName,17)]
    +SVerticalBox::Slot().AutoHeight().Padding(0,3)[Text(FString::Printf(TEXT("× %d"),Current.EnemyCount),16)]
    +SVerticalBox::Slot().AutoHeight()[SNew(SHorizontalBox)
     +SHorizontalBox::Slot().FillWidth(1).VAlign(VAlign_Center)[SNew(SBox).HeightOverride(9)[SNew(SProgressBar).Percent(1.f).FillColorAndOpacity(FLinearColor(.78f,.19f,.15f))]]
     +SHorizontalBox::Slot().AutoWidth().Padding(8,0)[Text(FString::Printf(TEXT("%.0f / %.0f"),Current.EnemyHealth,Current.EnemyHealth),12)]]]];
 }
 else if(Current.Left.IsEmpty())Body->AddSlot().AutoHeight().Padding(6,5,0,10)[Text(TEXT("暂无驻军"),14,Muted)];
 else for(const auto& E:Current.Left)Body->AddSlot().AutoHeight().Padding(0,2)[Entry(E)];
 Body->AddSlot().AutoHeight().Padding(0,10)[RuleLine()];
 Body->AddSlot().AutoHeight().Padding(6,0,0,7)[Text(TEXT("› 待移动卡组"),16,Gold)];
 if(!Current.MovingCards.IsEmpty())
 {
  auto Moving=SNew(SHorizontalBox);int32 I=0;for(const auto& E:Current.MovingCards)
  {if(I++>=2)break;Moving->AddSlot().FillWidth(1).Padding(8,0)[SNew(SBox).HeightOverride(120)[Entry(E)]];}
  Body->AddSlot().AutoHeight()[Moving];
  Body->AddSlot().AutoHeight().Padding(6,10)[Text(Current.MovementRoute,13,Ink)];
 }
 else Body->AddSlot().AutoHeight().Padding(6,4,0,10)[Text(TEXT("选择已部署卡牌，再单击目标地块"),13,Muted)];
 Body->AddSlot().AutoHeight()[RuleLine()];
 for(auto E:Current.Right)
 {
  if(E.Title==TEXT("待操作卡组"))continue;
  if(E.Action.Command==TEXT("move"))E.Title=TEXT("移动到此处");
  Body->AddSlot().AutoHeight().Padding(0,4)[Entry(E)];
 }
 if(!Current.Right.ContainsByPredicate([](const auto& E){return E.Action.Command==TEXT("move")||E.Action.Command==TEXT("deploy");}))
 {FPadmaUIEntry E;E.Title=TEXT("移动到此处");E.bPrimary=true;E.bEnabled=false;Body->AddSlot().AutoHeight().Padding(0,4)[Entry(E)];}
 Body->AddSlot().AutoHeight().HAlign(HAlign_Center).Padding(0,7)[Text(TEXT("单击地块固定详情 · 移动需点击确认"),11,Muted)];
 if(!Current.Notice.IsEmpty())Body->AddSlot().AutoHeight().Padding(4,4)[Text(Current.Notice,12,Gold)];
 Right->AddSlot().FillHeight(1)[SNew(SScrollBox)+SScrollBox::Slot()[Body]];
return Frame(Right,FMargin(9,8));
}

TSharedRef<SWidget> UPadmaGameScreen::BuildTutorialPhase()
{
 const TCHAR* BriefPhase[]={TEXT("黎明 · 整理手牌"),TEXT("晌午 · 部署与移动"),TEXT("黄昏 · 本日行动结束")};
 auto Phase=SNew(SVerticalBox);Phase->AddSlot().AutoHeight().HAlign(HAlign_Center).Padding(0,0,0,11)[Text(BriefPhase[FMath::Clamp(Current.PhaseIndex,0,2)],14,Gold,false)];
 Phase->AddSlot().AutoHeight()[Entry(Current.PhaseAction)];return Frame(Phase,FMargin(18,12));
}

TSharedRef<SWidget> UPadmaGameScreen::BuildTutorialHand()
{
 auto Hand=SNew(SVerticalBox);auto Tabs=SNew(SHorizontalBox);
 Tabs->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(0,0,22,0)[Text(TEXT("手牌"),17,Gold,false)];
 for(const auto& E:Current.HandFilters)Tabs->AddSlot().AutoWidth().Padding(3,0)[SNew(SBox).WidthOverride(E.Title==TEXT("全部")?58:48).HeightOverride(26)[SmallButton(this,E,12)]];
 Hand->AddSlot().AutoHeight().Padding(0,0,0,7)[Tabs];auto Cards=SNew(SUniformGridPanel).SlotPadding(FMargin(0,0,7,10));
 int32 CardIndex=0;for(const auto& E:Current.Hand){Cards->AddSlot(CardIndex%5,CardIndex/5)[SNew(SBox).WidthOverride(148).HeightOverride(177)[Entry(E)]];++CardIndex;}
 const float Offset=HandScroll?HandScroll->GetScrollOffset():0.f;
 Hand->AddSlot().FillHeight(1)[SAssignNew(HandScroll,SScrollBox).Orientation(Orient_Vertical).ScrollBarThickness(FVector2D(3,3))+SScrollBox::Slot()[Cards]];HandScroll->SetScrollOffset(Offset);
 auto HandRow=SNew(SHorizontalBox);HandRow->AddSlot().FillWidth(1)[Hand];auto Actions=SNew(SVerticalBox);
 for(auto E:Current.HandActions){E.Title=E.Action.Command==TEXT("codex")?TEXT("卡牌图鉴"):TEXT("◇  合成");E.Description.Reset();Actions->AddSlot().AutoHeight().Padding(0,4)[Entry(E)];}
 Actions->AddSlot().AutoHeight().Padding(0,12)[RuleLine()];
 auto Piles=SNew(SHorizontalBox);
 for(int32 Pile=0;Pile<2;++Pile)Piles->AddSlot().FillWidth(1)[SNew(SVerticalBox)
  +SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)[Text(Pile==0?TEXT("牌库"):TEXT("弃牌堆"),12,Muted,false)]
  +SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0,4)[Text(FString::FromInt(Pile==0?Current.DeckCount:Current.DiscardCount),18,Ink,false)]];
 Actions->AddSlot().AutoHeight()[Piles];
 HandRow->AddSlot().AutoWidth().Padding(9,28,0,0)[SNew(SBox).WidthOverride(126)[Actions]];
return Frame(HandRow,FMargin(15,9));
}

TSharedPtr<FSlateBrush> UPadmaGameScreen::ArtBrush(int32 Index,UTexture2D* Portrait)
{
 if(!Portrait&&(!CardAtlas||Index<0||Index>=9))return nullptr;
 auto Brush=MakeShared<FSlateBrush>();Brush->SetResourceObject(Portrait?Portrait:CardAtlas.Get());Brush->ImageSize=FVector2D(240,300);
 if(!Portrait){const int32 Col=Index%3,Row=Index/3;Brush->SetUVRegion(FBox2D(FVector2D((Col+.01)/3.,(Row+.01)/3.),FVector2D((Col+.99)/3.,(Row+.99)/3.)));}
 return Brush;
}
int32 UPadmaGameScreen::GetOverlayDepth()const{return OverlayLayers.Num();}
FName UPadmaGameScreen::GetActiveOverlayKey()const{return OverlayLayers.IsEmpty()?NAME_None:OverlayLayers.Last()->GetLayerKey();}
void UPadmaGameScreen::SyncLayers()
{
 if(!PageStack||!OverlayStack||!Layout)return;
 TWeakObjectPtr<UPadmaGameScreen> Weak(this);
 auto Back=[Weak]{if(Weak.IsValid())Weak->Intent({TEXT("back")});};
 if(!PageLayer||PageLayer->GetLayerKey()!=Current.PageKey)
 {
  PageStack->ClearWidgets();
  PageLayer=PageStack->AddWidget<UPadmaActivatableLayer>(UPadmaActivatableLayer::StaticClass(),[&](UPadmaActivatableLayer& Layer){Layer.Configure(Current.PageKey,false,Back);Layer.SetSurface(Layout.ToSharedRef());});
 }
 else PageLayer->SetSurface(Layout.ToSharedRef());
 PageLayer->SetNavigationKeyHandler([Weak](const FKeyEvent& E,bool Down)
 {
  if(!Weak.IsValid()||Weak->HasModal()||!Weak->Current.bWorldMap||!Weak->Current.bPaintedTutorial||!Weak->PaintedMap)return false;
  // Consume navigation during a pointer gesture, clearing rather than starting it.
  return Weak->PaintedMap->HandleNavigationKey(E.GetKey(),Down&&!FSlateApplication::Get().HasAnyMouseCaptor());
 });
 TArray<FName> Desired=Current.OverlayPath;if(HasModal()&&Desired.IsEmpty())Desired.Add(FName(*Current.ModalTitle));
 int32 Shared=0;while(Shared<Desired.Num()&&Shared<OverlayLayers.Num()&&OverlayLayers[Shared]->GetLayerKey()==Desired[Shared])++Shared;
 const bool bPurePop=Shared==Desired.Num()&&Desired.Num()<OverlayLayers.Num();
 while(OverlayLayers.Num()>Shared){auto* Layer=OverlayLayers.Pop().Get();OverlayStack->RemoveWidget(*Layer);}
 for(int32 I=Shared;I<Desired.Num();++I)
 {
  auto* Layer=OverlayStack->AddWidget<UPadmaActivatableLayer>(UPadmaActivatableLayer::StaticClass(),[&](UPadmaActivatableLayer& NewLayer){NewLayer.Configure(Desired[I],true,Back);NewLayer.SetSurface(BuildOverlay());});
  OverlayLayers.Add(Layer);
 }
 // A pure pop restores the retained parent controls and their CommonUI focus target.
 if(!OverlayLayers.IsEmpty()&&!bPurePop&&Shared==Desired.Num())OverlayLayers.Last()->SetSurface(BuildOverlay());
 if(OverlayLayers.IsEmpty()){PortraitMaterials.Reset();RevealMaterial=nullptr;}
 OverlayStack->SetVisibility(OverlayLayers.IsEmpty()?ESlateVisibility::Collapsed:ESlateVisibility::SelfHitTestInvisible);
 Layout->SetEnabled(OverlayLayers.IsEmpty());
}
TSharedRef<SWidget> UPadmaGameScreen::BuildOverlay()
{
 const bool Details=!Current.DetailSections.IsEmpty();
 auto Body=SNew(SVerticalBox);
 Body->AddSlot().AutoHeight().Padding(0,0,0,8)[SNew(SPadmaMotionElement).Delay(.05f).Distance(-32)[Text(Current.ModalTitle,30,Gold)]];
 if(!Current.ModalSubtitle.IsEmpty())Body->AddSlot().AutoHeight().Padding(0,0,0,16)[SNew(SPadmaMotionElement).Delay(.09f).Distance(-18)[Text(Current.ModalSubtitle,14,Muted)]];
 auto Columns=SNew(SHorizontalBox);
 if(Details)
 {
  auto Portrait=SNew(SVerticalBox);auto Brush=PortraitBrush;
  if(Brush)
  {
   if(RevealMaterial)RevealMaterial->SetScalarParameterValue(TEXT("RevealAmount"),1);
   if(auto* Base=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Padma/UI/Materials/MI_UIPortraitReveal.MI_UIPortraitReveal")))
   {
    RevealMaterial=UMaterialInstanceDynamic::Create(Base,this);PortraitMaterials.Add(RevealMaterial);PortraitRevealTime=0;
    RevealMaterial->SetTextureParameterValue(TEXT("PortraitTexture"),CurrentPortrait?CurrentPortrait.Get():CardAtlas.Get());
    const int32 I=Current.ModalArtIndex,Col=FMath::Max(0,I)%3,Row=FMath::Max(0,I)/3;
    const bool Atlas=!CurrentPortrait&&I>=0&&I<9;
    RevealMaterial->SetScalarParameterValue(TEXT("UVMinX"),Atlas?(Col+.01f)/3.f:0);RevealMaterial->SetScalarParameterValue(TEXT("UVMinY"),Atlas?(Row+.01f)/3.f:0);
    RevealMaterial->SetScalarParameterValue(TEXT("UVMaxX"),Atlas?(Col+.99f)/3.f:1);RevealMaterial->SetScalarParameterValue(TEXT("UVMaxY"),Atlas?(Row+.99f)/3.f:1);
    RevealMaterial->SetScalarParameterValue(TEXT("RevealAmount"),0);
    Brush=MakeShared<FSlateBrush>();Brush->SetResourceObject(RevealMaterial);Brush->ImageSize=FVector2D(238,310);
   }
  }
  Portrait->AddSlot().AutoHeight()[SNew(SBox).WidthOverride(238).HeightOverride(310)[Brush.IsValid()?StaticCastSharedRef<SWidget>(SNew(SImage).Image_Lambda([Brush]{return Brush.Get();})):Text(TEXT("立绘尚未配置"),20,Muted)]];
  Portrait->AddSlot().AutoHeight().Padding(0,12)[Text(Current.PortraitCaption,13,Muted)];
  Columns->AddSlot().AutoWidth().Padding(0,0,24,0)[SNew(SPadmaMotionElement).Delay(.12f).Duration(.42f).Distance(-22)[Portrait]];
 }
 auto Information=SNew(SVerticalBox);
 if(!Current.ModalText.IsEmpty())Information->AddSlot().AutoHeight().Padding(0,0,0,14)[Text(Current.ModalText,16)];
 for(const auto& Section:Current.DetailSections)Information->AddSlot().AutoHeight().Padding(0,0,0,13)[SNew(SVerticalBox)
  +SVerticalBox::Slot().AutoHeight().Padding(0,0,0,6)[Text(TEXT("◇  ")+Section.Title,18,Gold)]
  +SVerticalBox::Slot().AutoHeight()[Text(Section.Text,15)]];
 Columns->AddSlot().FillWidth(1)[SNew(SPadmaMotionElement).Delay(.18f).Distance(22)[SNew(SBox).MaxDesiredHeight(440)[SNew(SScrollBox)+SScrollBox::Slot()[Information]]]];
 Body->AddSlot().AutoHeight()[Columns];
 if(!Current.ModalCards.IsEmpty())
 {
  auto Grid=SNew(SUniformGridPanel).SlotPadding(FMargin(5));int32 I=0;
  for(const auto& E:Current.ModalCards){Grid->AddSlot(I%4,I/4)[SNew(SBox).WidthOverride(150).HeightOverride(190)[Entry(E)]];++I;}
  Body->AddSlot().AutoHeight()[SNew(SBox).MaxDesiredHeight(430)[SNew(SScrollBox)+SScrollBox::Slot()[Grid]]];
 }
 if(Current.bMapSeedInput)
 {
  TWeakObjectPtr<UPadmaGameScreen> Weak(this);
  Body->AddSlot().AutoHeight().Padding(0,10)[SNew(SSpinBox<int32>).Value(Current.MapSeed).OnValueChanged_Lambda([Weak](int32 Value){if(Weak.IsValid())Weak->Intent({TEXT("map-seed"),FName(*FString::FromInt(Value))});})];
 }
 auto Actions=SNew(SHorizontalBox);
 if(Current.ModalActions.Num()>3)
 {
  auto Choices=SNew(SVerticalBox);int32 I=0;for(const auto& E:Current.ModalActions)Choices->AddSlot().AutoHeight().Padding(0,4)[SNew(SPadmaMotionElement).Delay(.25f+.035f*FMath::Min(I++,5)).Distance(12)[Entry(E)]];
  Body->AddSlot().AutoHeight().Padding(0,14,0,0)[SNew(SBox).MaxDesiredHeight(285)[SNew(SScrollBox)+SScrollBox::Slot()[Choices]]];
 }
 else
 {
  int32 I=0;for(const auto& E:Current.ModalActions)Actions->AddSlot().FillWidth(1).Padding(4,0)[SNew(SPadmaMotionElement).Delay(.25f+.04f*I++).Distance(12)[Entry(E)]];
  Body->AddSlot().AutoHeight().Padding(0,18,0,0)[Actions];
 }
 return SNew(SPadmaMotionElement).Duration(.16f).Distance(0)[SNew(SBorder).BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush")).BorderBackgroundColor(FLinearColor(.002f,.008f,.015f,.78f))
  [SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center).Padding(30)[SNew(SBox).WidthOverride(Details?930:Current.ModalCards.IsEmpty()?650:760).MaxDesiredHeight(740)[Frame(Body,FMargin(28),Gold)]]]];
}

TSharedRef<SWidget> UPadmaGameScreen::BuildGenericDetails()
{
 const bool Map=Current.bWorldMap;
 auto Right=SNew(SVerticalBox);
 if(Map)if(auto Art=ArtBrush(Current.NodeArtIndex))Right->AddSlot().AutoHeight().Padding(0,0,0,12)[SNew(SBox).HeightOverride(128)[SNew(SImage).Image_Lambda([Art]{return Art.Get();})]];
 Right->AddSlot().AutoHeight().Padding(0,0,0,8)[Text(Current.RightTitle,25,Gold)];Right->AddSlot().AutoHeight()[Text(Current.RightText,15)];
 if(Map)
 {
  Right->AddSlot().AutoHeight().Padding(0,16,0,6)[Text(Current.LeftTitle,15,Gold)];
  if(Current.Left.IsEmpty())Right->AddSlot().AutoHeight().Padding(0,0,0,8)[Text(TEXT("此地暂无我方卡牌"),14,Muted)];
  for(const auto& E:Current.Left)Right->AddSlot().AutoHeight().Padding(0,3)[Entry(E)];
 }
 for(const auto& E:Current.Right)Right->AddSlot().AutoHeight().Padding(0,4)[Entry(E)];
 return Frame(SNew(SScrollBox)+SScrollBox::Slot()[Right]);
}
