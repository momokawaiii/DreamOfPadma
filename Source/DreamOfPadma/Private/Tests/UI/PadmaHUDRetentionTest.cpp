#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "UI/Screens/PadmaGameScreen.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Framework/Application/SlateApplication.h"
#include "Layout/Children.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"

#if WITH_DEV_AUTOMATION_TESTS
namespace
{
bool ContainsVisibleText(const TSharedRef<SWidget>& Widget,const FString& Text)
{
 if(Widget->GetTypeAsString()==TEXT("STextBlock")
    &&StaticCastSharedRef<STextBlock>(Widget)->GetText().ToString().Contains(Text))return true;
 FChildren* Children=Widget->GetChildren();
 for(int32 I=0;I<Children->Num();++I)if(ContainsVisibleText(Children->GetChildAt(I),Text))return true;
 return false;
}
void CollectHandTiles(const TSharedRef<SWidget>& Widget,TArray<TSharedPtr<SBorder>>& Out)
{
 if(Widget->GetTypeAsString()==TEXT("SPadmaIntentTile"))Out.Add(StaticCastSharedRef<SBorder>(Widget));
 FChildren* Children=Widget->GetChildren();
 for(int32 I=0;I<Children->Num();++I)CollectHandTiles(Children->GetChildAt(I),Out);
}
TSharedPtr<SWidget> LayoutChildWithText(const TSharedRef<SOverlay>& Layout,const FString& Text)
{
 FChildren* Children=Layout->GetChildren();
 for(int32 I=0;I<Children->Num();++I)
  if(ContainsVisibleText(Children->GetChildAt(I),Text))return Children->GetChildAt(I);
 return nullptr;
}
bool FindTextPath(const TSharedRef<SWidget>& Widget,const FString& Text,TArray<TSharedRef<SWidget>>& Path)
{
 Path.Add(Widget);
 if(Widget->GetTypeAsString()==TEXT("STextBlock")&&StaticCastSharedRef<STextBlock>(Widget)->GetText().ToString()==Text)return true;
 FChildren* Children=Widget->GetChildren();
 for(int32 I=0;I<Children->Num();++I)if(FindTextPath(Children->GetChildAt(I),Text,Path))return true;
 Path.Pop();return false;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaHUDRetentionTest,"DreamOfPadma.UI.HUDRetentionAcrossHoverAndModals",
 EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)

bool FPadmaHUDRetentionTest::RunTest(const FString& Parameters)
{
 if(!TestTrue(TEXT("Slate application is initialized"),FSlateApplication::IsInitialized()))return false;
 if(!TestFalse(TEXT("Fixture starts without an external mouse capture"),FSlateApplication::Get().HasAnyMouseCaptor()))return false;
 UWorld::InitializationValues Values;
 Values.AllowAudioPlayback(false).CreatePhysicsScene(false).ShouldSimulatePhysics(false)
  .EnableTraceCollision(false).CreateNavigation(false).CreateAISystem(false);
 auto* World=UWorld::CreateWorld(EWorldType::Game,false,NAME_None,nullptr,true,ERHIFeatureLevel::Num,&Values);
 if(!TestNotNull(TEXT("HUD fixture world"),World))return false;
 GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
 ON_SCOPE_EXIT {World->DestroyWorld(false);GEngine->DestroyWorldContext(World);};
 auto* Screen=CreateWidget<UPadmaGameScreen>(World,UPadmaGameScreen::StaticClass());
 if(!TestNotNull(TEXT("Native screen"),Screen))return false;
 ON_SCOPE_EXIT {Screen->ReleaseSlateResources(true);};
 const auto SlateRoot=Screen->TakeWidget();

 FPadmaGameView Base;
 Base.bWorldMap=true;Base.bPaintedTutorial=true;Base.PageKey=TEXT("map");
 Base.DetailNode=TEXT("home");Base.RightTitle=TEXT("Retention Home");Base.RightText=TEXT("Initial node details");
 Base.TerrainText=TEXT("平原");Base.OwnershipText=TEXT("我方领地");Base.NodeStateText=TEXT("我方控制");
 Base.CalendarTitle=TEXT("Retained Calendar");Base.DayText=TEXT("Day 1");Base.PhaseIndex=1;
 Base.PhaseHint=TEXT("Noon actions");Base.PhaseAction.Title=TEXT("Advance");Base.PhaseAction.Action.Command=TEXT("advance");
 Base.WarProgress=.5f;Base.WarText=TEXT("50 : 50");Base.DeckCount=17;Base.DiscardCount=2;
 Base.Resources.Add({TEXT("Flow"),TEXT("60"),TEXT("Resource fixture"),FLinearColor::White});
 FPadmaMinimapNode Home;Home.Id=TEXT("home");Home.Position=FVector2D(10.5,48.);Home.Title=TEXT("Retention Home");Home.Owner=TEXT("player");Home.bSelected=true;
 FPadmaMinimapNode Target;Target.Id=TEXT("target");Target.Position=FVector2D(48.2,51.5);Target.Title=TEXT("Retention Target");Target.Owner=TEXT("neutral");Target.HexQ=1;
 Base.MinimapNodes={Home,Target};Base.MinimapEdges.Add({0,1,false});
 for(int32 I=0;I<12;++I)
 {
  FPadmaUIEntry Card;Card.Title=FString::Printf(TEXT("Retained card %02d"),I);Card.Action={TEXT("hand"),FName(*FString::Printf(TEXT("card-%02d"),I))};
  Card.Badge=TEXT("A");Card.bCard=true;Card.bDraggable=true;Card.bSelected=I==0;Base.Hand.Add(Card);
 }
 Base.HandCount=Base.Hand.Num();
 Screen->Present(Base);
 SlateRoot->SlatePrepass(1.f);
 if(!TestTrue(TEXT("Tutorial panel hosts exist"),Screen->TutorialHeader.IsValid()&&Screen->TutorialHand.IsValid()
    &&Screen->TutorialDetails.IsValid()&&Screen->TutorialPhase.IsValid()&&Screen->HandScroll.IsValid()))return false;
 const auto Header=Screen->TutorialHeader->GetChildren()->GetChildAt(0);
 const auto Hand=Screen->TutorialHand->GetChildren()->GetChildAt(0);
 const auto Phase=Screen->TutorialPhase->GetChildren()->GetChildAt(0);
 const auto Canvas=Screen->Layout->GetChildren()->GetChildAt(0);
 const auto Scroll=Screen->HandScroll;
 TArray<TSharedPtr<SBorder>> OriginalTiles;CollectHandTiles(Hand,OriginalTiles);
 if(!TestEqual(TEXT("Actual hand contains all twelve card widgets"),OriginalTiles.Num(),Base.Hand.Num()))return false;
 const FLinearColor SelectedColor=OriginalTiles[0]->GetBorderBackgroundColor().GetSpecifiedColor();
 Screen->HandScroll->SetScrollOffset(137.f);

 auto CheckRetained=[&](const FString& Context)
 {
  TestTrue(Context+TEXT(": header content retains identity"),Screen->TutorialHeader->GetChildren()->GetChildAt(0)==Header);
  TestTrue(Context+TEXT(": hand content retains identity"),Screen->TutorialHand->GetChildren()->GetChildAt(0)==Hand);
  TestTrue(Context+TEXT(": phase controls retain identity"),Screen->TutorialPhase->GetChildren()->GetChildAt(0)==Phase);
  TestTrue(Context+TEXT(": canvas and passive panels retain identity"),Screen->Layout->GetChildren()->GetChildAt(0)==Canvas);
  TestTrue(Context+TEXT(": hand scroll widget is retained"),Screen->HandScroll==Scroll);
  TestEqual(Context+TEXT(": hand scroll offset is retained"),Screen->HandScroll->GetScrollOffset(),137.f);
  TArray<TSharedPtr<SBorder>> CurrentTiles;CollectHandTiles(Screen->TutorialHand->GetChildren()->GetChildAt(0),CurrentTiles);
  TestTrue(Context+TEXT(": original card widgets survive"),CurrentTiles==OriginalTiles);
  TestTrue(Context+TEXT(": original selected card survives"),Screen->Current.Hand.Num()==12&&Screen->Current.Hand[0].bSelected);
 };

 FPadmaGameView Hover;
 Hover.bDetailOnly=true;Hover.bWorldMap=true;Hover.bPaintedTutorial=true;Hover.HexGrid=Base.HexGrid;
 Hover.DetailNode=TEXT("target");Hover.RightTitle=TEXT("Hovered Target Details");Hover.RightText=TEXT("Hover-only projection");
 Hover.TerrainText=TEXT("林地");Hover.OwnershipText=TEXT("中立地区");Hover.NodeStateText=TEXT("宜居 · 可占领");
 Hover.MinimapNodes=Base.MinimapNodes;Hover.MinimapNodes[0].bSelected=false;Hover.MinimapNodes[1].bSelected=true;Hover.MinimapNodes[1].bHovered=true;
 Hover.MinimapEdges=Base.MinimapEdges;
 Screen->Present(Hover);
 CheckRetained(TEXT("Hover"));
 TestTrue(TEXT("Actual sidebar text follows hover"),ContainsVisibleText(Screen->TutorialDetails->GetChildren()->GetChildAt(0),Hover.RightTitle));
 TestEqual(TEXT("Hover keeps actual header data"),Screen->Current.CalendarTitle,Base.CalendarTitle);
 TestEqual(TEXT("Hover keeps hand count rather than partial-view defaults"),Screen->Current.HandCount,12);
 TestEqual(TEXT("Hover updates the map target"),Screen->Current.DetailNode,Hover.DetailNode);
 if(HasAnyErrors())return false;

 FPadmaGameView Modal=Screen->Current;Modal.bDetailOnly=false;
 Modal.ModalTitle=TEXT("确认部署");Modal.ModalText=TEXT("Retained page beneath confirmation");Modal.OverlayPath={FName(TEXT("placement-fixture"))};
 FPadmaUIEntry Confirm;Confirm.Title=TEXT("Confirm");Confirm.Action.Command=TEXT("placement-confirm");Modal.ModalActions.Add(Confirm);
 Screen->Present(Modal);
 CheckRetained(TEXT("Opening confirmation"));
 TestTrue(TEXT("Confirmation owns an actual overlay"),Screen->HasModal()&&Screen->GetOverlayDepth()==1);
 TestFalse(TEXT("Modal disables the underlying gameplay surface"),Screen->Layout->IsEnabled());
 FPadmaGameView BlockedHover=Hover;BlockedHover.RightTitle=TEXT("Blocked hover must not appear");Screen->Present(BlockedHover);
 TestEqual(TEXT("Partial hover cannot change a modal-owned page"),Screen->Current.RightTitle,Modal.RightTitle);
 FPadmaGameView Resume=Modal;Resume.ModalTitle.Reset();Resume.ModalText.Reset();Resume.ModalActions.Reset();Resume.OverlayPath.Reset();
 Screen->Present(Resume);
 CheckRetained(TEXT("Closing confirmation"));
 TestFalse(TEXT("Page is restored without an overlay"),Screen->HasModal());
 TestTrue(TEXT("Closing modal restores the gameplay surface"),Screen->Layout->IsEnabled());
 if(HasAnyErrors())return false;

 // Seed the state reached when a full view waits for pointer release. This avoids
 // capturing the automation process's real OS pointer merely to test queue retention.
 FPadmaGameView Pending=Screen->Current;Pending.bDetailOnly=false;
 Pending.Hand[0].bSelected=false;Pending.Hand[1].bSelected=true;Pending.DeckCount=8;Pending.Resources[0].Value=TEXT("99");
 Screen->DeferredView=Pending;Screen->bDeferredView=true;
 FPadmaGameView LaterHover=Hover;LaterHover.DetailNode=TEXT("home");LaterHover.RightTitle=TEXT("Later Home Details");
 Screen->Present(LaterHover);
 TestTrue(TEXT("Partial hover leaves the full view queued"),Screen->bDeferredView&&!Screen->DeferredView.bDetailOnly);
 TestTrue(TEXT("Queued selection cannot be replaced by stale hand state"),Screen->DeferredView.Hand.Num()==12
  &&!Screen->DeferredView.Hand[0].bSelected&&Screen->DeferredView.Hand[1].bSelected);
 if(!TestEqual(TEXT("Queued resource payload is retained"),Screen->DeferredView.Resources.Num(),1))return false;
 TestEqual(TEXT("Queued resource change survives hover"),Screen->DeferredView.Resources[0].Value,FString(TEXT("99")));
 TestEqual(TEXT("Queued deck count survives hover"),Screen->DeferredView.DeckCount,8);
 Screen->NativeTick(FGeometry::MakeRoot(FVector2D(1672,941),FSlateLayoutTransform()),0.f);
 TestFalse(TEXT("Full view commits once pointer capture is absent"),Screen->bDeferredView);
 TestTrue(TEXT("Committed hand uses the newer selection"),Screen->Current.Hand.Num()==12
  &&!Screen->Current.Hand[0].bSelected&&Screen->Current.Hand[1].bSelected);
 if(!TestEqual(TEXT("Committed resource payload is retained"),Screen->Current.Resources.Num(),1))return false;
 TestEqual(TEXT("Committed header uses newer resource value"),Screen->Current.Resources[0].Value,FString(TEXT("99")));
 TestTrue(TEXT("Actual header displays the newer resource value"),ContainsVisibleText(Screen->TutorialHeader->GetChildren()->GetChildAt(0),TEXT("99")));
 TestEqual(TEXT("Hand rebuild for real selection change retains scroll position"),Screen->HandScroll->GetScrollOffset(),137.f);
 SlateRoot->SlatePrepass(1.f);
 TArray<TSharedPtr<SBorder>> CommittedTiles;CollectHandTiles(Screen->TutorialHand->GetChildren()->GetChildAt(0),CommittedTiles);
 if(TestEqual(TEXT("Committed hand has twelve actual widgets"),CommittedTiles.Num(),12))
 {
  TestTrue(TEXT("Selected style moves to the new card widget"),CommittedTiles[1]->GetBorderBackgroundColor().GetSpecifiedColor().Equals(SelectedColor));
  TestFalse(TEXT("Old card no longer paints the selected style"),CommittedTiles[0]->GetBorderBackgroundColor().GetSpecifiedColor().Equals(SelectedColor));
 }
 if(HasAnyErrors())return false;

 // Legacy maps use the same regional-update contract, with their own right host.
 FPadmaGameView Legacy=Base;Legacy.bPaintedTutorial=false;Legacy.RightTitle=TEXT("Legacy original details");
 Screen->Present(Legacy);SlateRoot->SlatePrepass(1.f);
 if(!TestTrue(TEXT("Legacy right host exists"),Screen->GenericDetails.IsValid()))return false;
 const auto LegacyDetails=Screen->GenericDetails;
 const auto LegacyHeader=LayoutChildWithText(Screen->Layout.ToSharedRef(),Legacy.CalendarTitle);
 const auto LegacyHand=LayoutChildWithText(Screen->Layout.ToSharedRef(),Legacy.Hand[0].Title);
 if(!TestTrue(TEXT("Legacy header and hand exist in actual layout"),LegacyHeader.IsValid()&&LegacyHand.IsValid()))return false;
 TArray<TSharedPtr<SBorder>> LegacyTiles;CollectHandTiles(LegacyHand.ToSharedRef(),LegacyTiles);
 if(!TestEqual(TEXT("Legacy hand contains twelve actual cards"),LegacyTiles.Num(),12))return false;
 FPadmaGameView LegacyHover=Hover;LegacyHover.bPaintedTutorial=false;LegacyHover.RightTitle=TEXT("Legacy hovered target");
 LegacyHover.RightText=TEXT("Legacy regional text must update");Screen->Present(LegacyHover);
 TestTrue(TEXT("Legacy partial changes actual right text"),ContainsVisibleText(Screen->GenericDetails->GetChildren()->GetChildAt(0),LegacyHover.RightTitle)
  &&ContainsVisibleText(Screen->GenericDetails->GetChildren()->GetChildAt(0),LegacyHover.RightText));
 TestTrue(TEXT("Legacy partial preserves its right host"),Screen->GenericDetails==LegacyDetails);
 TestTrue(TEXT("Legacy partial preserves actual header identity"),LayoutChildWithText(Screen->Layout.ToSharedRef(),Legacy.CalendarTitle)==LegacyHeader);
 TestTrue(TEXT("Legacy partial preserves actual hand identity"),LayoutChildWithText(Screen->Layout.ToSharedRef(),Legacy.Hand[0].Title)==LegacyHand);
 TArray<TSharedPtr<SBorder>> LegacyAfterTiles;CollectHandTiles(LegacyHand.ToSharedRef(),LegacyAfterTiles);
 TestTrue(TEXT("Legacy partial keeps original card widgets"),LegacyAfterTiles==LegacyTiles);
 TestTrue(TEXT("Legacy partial keeps hand selection and counts"),Screen->Current.Hand.Num()==12&&Screen->Current.Hand[0].bSelected&&Screen->Current.HandCount==12);
 TestFalse(TEXT("Legacy hover does not switch to painted tutorial"),Screen->Current.bPaintedTutorial);
 if(HasAnyErrors())return false;

 // Every attachment candidate, including cancellation, must remain reachable in
 // one bounded scroll region when a node contains more A cards than fit onscreen.
 FPadmaGameView ManyChoices=Screen->Current;ManyChoices.bDetailOnly=false;
 ManyChoices.ModalTitle=TEXT("选择附着对象");ManyChoices.ModalText=TEXT("Nine A candidates and a cancellation action");
 ManyChoices.OverlayPath={FName(TEXT("many-attachment-targets"))};ManyChoices.ModalActions.Reset();
 for(int32 I=0;I<9;++I)
 {
  FPadmaUIEntry Candidate;Candidate.Title=FString::Printf(TEXT("Attachment candidate %d"),I);
  Candidate.Description=TEXT("Choose this colocated A instance");Candidate.Action={TEXT("placement-attach"),FName(*FString::Printf(TEXT("anchor-%d"),I))};
  ManyChoices.ModalActions.Add(Candidate);
 }
 FPadmaUIEntry Cancel;Cancel.Title=TEXT("Close attachment list");Cancel.Action.Command=TEXT("close");ManyChoices.ModalActions.Add(Cancel);
 Screen->Present(ManyChoices);SlateRoot->SlatePrepass(1.f);
 TestTrue(TEXT("Long attachment list is an active modal"),Screen->HasModal()&&Screen->GetOverlayDepth()==1);
 TSharedPtr<SWidget> ChoiceScroll;TSharedPtr<SWidget> ChoiceHeightBound;
 for(const auto& Action:ManyChoices.ModalActions)
 {
  TArray<TSharedRef<SWidget>> Path;
  if(!TestTrue(Action.Title+TEXT(": actual action text is present"),FindTextPath(Screen->RootLayout.ToSharedRef(),Action.Title,Path)))continue;
  int32 ScrollIndex=INDEX_NONE;bool HasButton=false;
  for(int32 I=0;I<Path.Num();++I)
  {
   if(Path[I]->GetTypeAsString()==TEXT("SScrollBox"))ScrollIndex=I;
   if(Path[I]->GetTypeAsString()==TEXT("SButton"))HasButton=true;
  }
  TestTrue(Action.Title+TEXT(": action is a button"),HasButton);
  if(!TestTrue(Action.Title+TEXT(": has a scroll ancestor"),ScrollIndex!=INDEX_NONE))continue;
  if(!ChoiceScroll)ChoiceScroll=Path[ScrollIndex];
  TestTrue(Action.Title+TEXT(": uses the same scroll region"),ChoiceScroll==Path[ScrollIndex]);
  if(!ChoiceHeightBound)for(int32 I=ScrollIndex-1;I>=0;--I)
   if(Path[I]->GetTypeAsString()==TEXT("SBox")){ChoiceHeightBound=Path[I];break;}
 }
 if(TestTrue(TEXT("Attachment scroll region has a height bound"),ChoiceHeightBound.IsValid()))
 {
  const float Height=ChoiceHeightBound->GetDesiredSize().Y;
  TestTrue(TEXT("Attachment list desired height is positive and at most 285"),Height>0.f&&Height<=285.01f);
 }
 return !HasAnyErrors();
}
#endif
