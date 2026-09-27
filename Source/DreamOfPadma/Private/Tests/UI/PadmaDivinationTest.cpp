#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "UI/Screens/PadmaDivinationWidget.h"
#include "UI/Screens/PadmaGameScreen.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Layout/Children.h"
#include "Input/HittestGrid.h"
#include "InputCoreTypes.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Types/PaintArgs.h"
#include "Widgets/SWindow.h"
#include "Rendering/DrawElements.h"

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaDivinationPlaybackTest,"DreamOfPadma.UI.Divination.Playback",
 EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaDivinationPlaybackTest::RunTest(const FString&)
{
 const auto* S=GetDefault<UPadmaDivinationStyle>();FPadmaDivinationPlayback P;
 TestFalse(TEXT("Reveal is rejected during entrance"),P.Reveal());
 P.Advance(100,*S);
 TestTrue(TEXT("Long frame stops at explicit input gate"),P.Phase==EPadmaDivinationPhase::AwaitReveal);
 TestTrue(TEXT("First reveal accepted"),P.Reveal());
 TestFalse(TEXT("Repeated reveal cannot restart animation"),P.Reveal());
 P.Advance(.5f,*S);P.bPaused=true;P.Advance(10,*S);
 TestEqual(TEXT("Paused reveal is frozen"),P.Time,.5f);
 P.Seek(S->EnterDuration+S->RevealDuration,*S);
 TestTrue(TEXT("Scrub reaches stable result"),P.Phase==EPadmaDivinationPhase::Result);
 P.Replay();TestTrue(TEXT("Replay clears paused state"),!P.bPaused&&P.Time==0&&P.Phase==EPadmaDivinationPhase::Enter);
 P.bPaused=true;TestTrue(TEXT("Close works while paused"),P.Close());
 P.Replay();P.Seek(0,*S);
 TestTrue(TEXT("Replay and scrub cannot interrupt closing"),P.Phase==EPadmaDivinationPhase::Closing);
 TestTrue(TEXT("Closing finishes despite pause"),P.Advance(1,*S));
 TestFalse(TEXT("Closed completion emitted once"),P.Advance(1,*S));
 FPadmaDivinationPlayback A,B;A.Rate=B.Rate=.25f;
 for(int32 I=0;I<120;++I)A.Advance(1.f/60,*S);
 B.Advance(2,*S);TestTrue(TEXT("Playback is independent of timestep"),FMath::IsNearlyEqual(A.Time,B.Time,.00001f));
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaDivinationStagesTest,"DreamOfPadma.UI.Divination.ClickStages",
 EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaDivinationStagesTest::RunTest(const FString&)
{
 auto W=SNew(SPadmaDivinationWidget);
 const auto G=FGeometry::MakeRoot(FVector2D(1600,900),FSlateLayoutTransform());
 TestTrue(TEXT("Open lands on daily selection"),W->IsSelection());
 W->ContinuePresentation();
 TestTrue(TEXT("Existing daily answer requires confirmation"),W->IsJudge());
 W->ConfirmDivination(true);W->Tick(G,0,.6f);W->ContinuePresentation();
 TestFalse(TEXT("Confirmed click enters clock"),W->IsSelection());
 TestTrue(TEXT("Double click cannot skip the clock entrance"),W->GetPlayback().Phase==EPadmaDivinationPhase::Enter);
 W->Tick(G,0,2.1f);W->Tick(G,0,10);
 TestTrue(TEXT("Clock waits indefinitely for a second deliberate click"),W->GetPlayback().Phase==EPadmaDivinationPhase::AwaitReveal);
 W->ContinuePresentation();W->Tick(G,0,.4f);W->ContinuePresentation();
 TestEqual(TEXT("Repeat input does not restart a flip"),W->GetPlayback().Time,.4f);
 W->Tick(G,0,3);W->ContinuePresentation();
 TestTrue(TEXT("Result continues back to daily selection"),W->IsSelection());
 int32 Closed=0;
 auto Intro=SNew(SPadmaDivinationWidget).TitleIntro(true).OnClosed(FSimpleDelegate::CreateLambda([&]{++Closed;}));
 TestTrue(TEXT("Startup uses title mode"),Intro->IsTitleIntro());
 Intro->Tick(G,0,6.6f);Intro->Tick(G,0,1);
 TestEqual(TEXT("Startup automatically completes once"),Closed,1);
 auto Skip=SNew(SPadmaDivinationWidget).TitleIntro(true).OnClosed(FSimpleDelegate::CreateLambda([&]{++Closed;}));
 Skip->ContinuePresentation();Skip->ContinuePresentation();Skip->Tick(G,0,1);
 TestEqual(TEXT("Title click skips once"),Closed,2);
 return true;
}

namespace
{
TSharedPtr<SPadmaDivinationWidget> FindDivination(const TSharedRef<SWidget>& W)
{
 if(W->GetTypeAsString()==TEXT("SPadmaDivinationWidget"))return StaticCastSharedRef<SPadmaDivinationWidget>(W);
 auto* Children=W->GetChildren();for(int32 I=0;I<Children->Num();++I)if(auto Result=FindDivination(Children->GetChildAt(I)))return Result;
 return nullptr;
}
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaDivinationLayerTest,"DreamOfPadma.UI.Divination.LayerLifetime",
 EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaDivinationLayerTest::RunTest(const FString&)
{
 UWorld::InitializationValues Values;Values.AllowAudioPlayback(false).CreatePhysicsScene(false).ShouldSimulatePhysics(false).EnableTraceCollision(false).CreateNavigation(false).CreateAISystem(false);
 auto* World=UWorld::CreateWorld(EWorldType::Game,false,NAME_None,nullptr,true,ERHIFeatureLevel::Num,&Values);
 if(!TestNotNull(TEXT("Fixture world"),World))return false;
 GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
 ON_SCOPE_EXIT{World->DestroyWorld(false);GEngine->DestroyWorldContext(World);};
 auto* Screen=CreateWidget<UPadmaGameScreen>(World,UPadmaGameScreen::StaticClass());
 ON_SCOPE_EXIT{Screen->ShutdownPresentation();};
 const auto Root=Screen->TakeWidget();
 FPadmaGameView Menu;Menu.PageKey=TEXT("map");Menu.ModalTitle=TEXT("菜单");Menu.OverlayPath={TEXT("菜单")};Screen->Present(Menu);
 FPadmaGameView Preview=Menu;Preview.ModalTitle=TEXT("占卜演出预览");Preview.OverlayPath.Add(TEXT("占卜演出预览"));Screen->Present(Preview);
 auto Widget=FindDivination(Root);
 if(!TestTrue(TEXT("Preview uses actual native surface"),Widget.IsValid()))return false;
 TestEqual(TEXT("Preview is nested above menu"),Screen->GetOverlayDepth(),2);
 Screen->Present(Preview);TestTrue(TEXT("Refresh retains the same animation surface"),FindDivination(Root)==Widget);
 int32 Closed=0;
 Screen->OnIntent=[&](const FPadmaUIAction& Intent){if(Intent.Command==TEXT("close")){++Closed;Screen->Present(Menu);}};
 Widget->RequestClose();Widget->RequestClose();
 Widget->Tick(FGeometry::MakeRoot(FVector2D(1600,900),FSlateLayoutTransform()),0,1);
 TestEqual(TEXT("Close sent once"),Closed,1);
 TestEqual(TEXT("Parent menu restored"),Screen->GetActiveOverlayKey(),FName(TEXT("菜单")));
 TestEqual(TEXT("Only parent remains"),Screen->GetOverlayDepth(),1);
 TWeakPtr<SPadmaDivinationWidget> Weak=Widget;Widget.Reset();
 TestFalse(TEXT("Popped surface is released despite stack pooling"),Weak.IsValid());
 Screen->Present(Preview);TestTrue(TEXT("Reopen starts a fresh entrance"),FindDivination(Root)->GetPlayback().Phase==EPadmaDivinationPhase::Enter);
 Screen->ShutdownPresentation();TestEqual(TEXT("Shutdown clears layers"),Screen->GetOverlayDepth(),0);
 Screen->OnIntent=nullptr;
 return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaTarotCatalogInteractionTest,"DreamOfPadma.UI.Divination.CatalogAndPointerInteraction",
 EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaTarotCatalogInteractionTest::RunTest(const FString&)
{
 auto W=SNew(SPadmaDivinationWidget);const auto G=FGeometry::MakeRoot(FVector2D(1600,900),FSlateLayoutTransform());
 if(!TestTrue(TEXT("Original activity catalog loaded"),W->GetTarot().IsReady()))return false;
 TestEqual(TEXT("All activity cards are present"),W->GetTarot().Cards.Num(),8);
 TestEqual(TEXT("All spreads are present"),W->GetTarot().Spreads.Num(),6);
 TestEqual(TEXT("Daily reference uses Strength"),W->GetTarot().CurrentCards()[0].Card,5);
 TestEqual(TEXT("Daily reference uses Pope"),W->GetTarot().CurrentCards()[1].Card,4);
 W->HandlePointer(FVector2D(125,55));TestTrue(TEXT("Help is separate from Back"),W->IsHelp());
 W->RequestClose();TestFalse(TEXT("Escape only dismisses help"),W->IsHelp());
 W->HandlePointer(FVector2D(1440,50));TestTrue(TEXT("Tarot Case opens its own view"),W->IsGallery());
 // At scroll zero the Hermit crosses x=800; its visible left edge must remain interactive.
 W->HandlePointer(FVector2D(785,760));
 TestEqual(TEXT("Card left of the screen midpoint remains selectable"),W->GetGalleryCard(),2);
 for(int32 I=0;I<8;++I)
 {
  TestTrue(TEXT("Every owned card can be selected"),W->SelectGalleryCard(I));
  W->Tick(G,0,1);
  W->HandlePointer(SPadmaDivinationWidget::GalleryPosition(I,I));
  TestEqual(TEXT("Source Bezier card hit selects the card"),W->GetGalleryCard(),I);
  TestFalse(TEXT("Original upright meaning exists"),W->GetTarot().Cards[I].Upright.IsEmpty());
  TestFalse(TEXT("Original reverse meaning exists"),W->GetTarot().Cards[I].Reversed.IsEmpty());
 }
 TestFalse(TEXT("Gallery has a finite beginning"),W->SelectGalleryCard(-1));
 TestFalse(TEXT("Gallery has a finite end"),W->SelectGalleryCard(8));
 const auto Anchor=SPadmaDivinationWidget::GalleryPosition(3,3);
 TestTrue(TEXT("Source viewport origin locates the selected card"),FVector2D::Distance(Anchor,FVector2D(1225.800,443.793))<.1);
 W->HandlePointer(FVector2D(162,648));TestTrue(TEXT("Source rotate button selects reversed meaning"),W->IsGalleryReversed());
 W->Tick(G,0,.7f);W->HandlePointer(FVector2D(162,648));TestFalse(TEXT("Source rotate button restores upright"),W->IsGalleryReversed());
 W->RequestClose();TestFalse(TEXT("Back pops gallery"),W->IsGallery());
 const int32 Counts[]={3,4,4,4,5,2};
 for(int32 I=0;I<6;++I)
 {
  W->HandlePointer(I==5?FVector2D(1400,830):FVector2D(1400,235+I*91));
  TestEqual(TEXT("Each visible tab selects its original spread"),W->GetTarot().Selected,I);
  TestEqual(TEXT("Original slot count"),W->GetTarot().CurrentCards().Num(),Counts[I]);
  W->Tick(G,0,.7f);W->HandlePointer(W->GetTarot().CurrentSpread().Selection[0].Position);
  TestTrue(TEXT("Owned card opens its original meaning"),W->GetDetailSlot()!=INDEX_NONE);
  W->RequestClose();W->Tick(G,0,.4f);
  if(I<5)
  {
   W->ContinuePresentation();TestTrue(TEXT("Already revealed normal spread cannot reroll"),W->IsSelection());
   TestFalse(TEXT("Normal spread has no daily judge"),W->IsJudge());
  }
 }
 const int32 Before=W->GetTarot().CurrentCards()[0].Card;
 W->ContinuePresentation();TestTrue(TEXT("Daily repeats require source confirmation"),W->IsJudge());
 W->ConfirmDivination(false);W->Tick(G,0,.6f);TestTrue(TEXT("Cancel retains board"),W->IsSelection());
 TestEqual(TEXT("Cancel retains prior answer"),W->GetTarot().CurrentCards()[0].Card,Before);
 W->ContinuePresentation();W->ConfirmDivination(true);W->Tick(G,0,.6f);W->Tick(G,0,2.1f);W->ContinuePresentation();
 TestTrue(TEXT("Confirmed daily answer enters reveal"),W->GetPlayback().Phase==EPadmaDivinationPhase::Reveal);
 W->Tick(G,0,3);W->ContinuePresentation();TestTrue(TEXT("Result returns to board"),W->IsSelection());

 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaTarotUnlockTest,"DreamOfPadma.UI.Divination.NormalUnlockRules",
 EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaTarotUnlockTest::RunTest(const FString&)
{
 auto* Style=LoadObject<UPadmaDivinationStyle>(nullptr,TEXT("/Game/Padma/UI/Divination/DA_DivinationStyleArk.DA_DivinationStyleArk"));
 if(!TestNotNull(TEXT("Original style"),Style))return false;
 FPadmaTarotSession Session;FString Error;if(!TestTrue(TEXT("Original catalog loads"),Session.Load(Style->OriginalLayoutJson,Error)))return false;
 Session.SetNormalRevealed(false);
 TestFalse(TEXT("All owned does not unlock daily until normal answers are revealed"),Session.IsDailyUnlocked());
 TestFalse(TEXT("Locked daily cannot be selected"),Session.Select(5));
 for(int32 I=0;I<5;++I)
 {
  Session.Select(I);const auto Before=Session.CurrentCards();
  TestTrue(TEXT("Constructed normal spread reveals once"),Session.Draw());
  for(int32 J=0;J<Before.Num();++J)
  {
   TestEqual(TEXT("Normal reveal preserves built card"),Session.CurrentCards()[J].Card,Before[J].Card);
   TestEqual(TEXT("Normal reveal preserves orientation"),Session.CurrentCards()[J].bReversed,Before[J].bReversed);
  }
  TestFalse(TEXT("Normal answer cannot be rerolled"),Session.Draw());
 }
 TestTrue(TEXT("Five revealed normal spreads unlock daily"),Session.IsDailyUnlocked());
 Session.Select(5);TestFalse(TEXT("First daily answer has no prior cards to confirm"),Session.NeedsConfirmation());
 TestTrue(TEXT("First daily draw succeeds"),Session.Draw());
 TestTrue(TEXT("Subsequent daily answer requires confirmation"),Session.NeedsConfirmation());
 TestTrue(TEXT("Daily remains repeatable"),Session.Draw());
 return true;
}


// Exercise real Slate painting: callback-only tests cannot detect packed animation row overruns.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaTarotPagePaintTest,"DreamOfPadma.UI.Divination.PagePaint",
 EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaTarotPagePaintTest::RunTest(const FString&)
{
 const auto G=FGeometry::MakeRoot(FVector2D(1600,900),FSlateLayoutTransform());
 auto Window=SNew(SWindow).ClientSize(FVector2D(1600,900));FHittestGrid Grid;
 Grid.SetHittestArea(FVector2D::ZeroVector,FVector2D(1600,900));
 for(int32 Page=0;Page<4;++Page)
 {
  auto W=SNew(SPadmaDivinationWidget).TitleIntro(Page==3);Window->SetContent(W);
  if(Page==0)W->OpenGallery();
  else if(Page==1){W->Tick(G,0,1);W->HandlePointer(W->GetTarot().CurrentSpread().Selection[0].Position);}
  else if(Page==2)W->ContinuePresentation();
  for(float Delta:{0.f,.3f,2.f})
  {
   W->Tick(G,0,Delta);W->SlatePrepass();FSlateWindowElementList Elements(Window);
   FPaintArgs Args(&Window.Get(),Grid,FVector2D::ZeroVector,0,Delta);
   const int32 Layer=W->Paint(Args,G,FSlateRect(0,0,1600,900),Elements,0,FWidgetStyle(),true);
   TestTrue(TEXT("Original page paints animated and stable frames"),Layer>=100);
  }
  W->RequestClose();W->Tick(G,0,.15f);W->SlatePrepass();FSlateWindowElementList Elements(Window);
  FPaintArgs Args(&Window.Get(),Grid,FVector2D::ZeroVector,0,.15f);
  W->Paint(Args,G,FSlateRect(0,0,1600,900),Elements,0,FWidgetStyle(),true);
 }
 return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaTarotDragTest,"DreamOfPadma.UI.Divination.GalleryCardDrag",
 EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaTarotDragTest::RunTest(const FString&)
{
 for(float Scale:{1.f,.75f})
 {
  const auto G=FGeometry::MakeRoot(FVector2D(1600,900),FSlateLayoutTransform(Scale));
  auto Window=SNew(SWindow).ClientSize(FVector2D(1600,900));auto W=SNew(SPadmaDivinationWidget);
  Window->SetContent(W);W->OpenGallery();W->Tick(G,0,1);W->SlatePrepass();
  FHittestGrid Grid;Grid.SetHittestArea(FVector2D::ZeroVector,FVector2D(1600,900));
  FSlateWindowElementList Elements(Window);FPaintArgs Args(&Window.Get(),Grid,FVector2D::ZeroVector,0,0);
  W->Paint(Args,G,FSlateRect(0,0,1600,900),Elements,0,FWidgetStyle(),true);
  auto Pointer=[&](FVector2D P,FVector2D Previous,bool Held)
  {return FPointerEvent(0,G.LocalToAbsolute(P),G.LocalToAbsolute(Previous),Held?TSet<FKey>{EKeys::LeftMouseButton}:TSet<FKey>{},EKeys::LeftMouseButton,0,FModifierKeysState());};
  const FVector2D Start(360,405),Right(520,405),Left(200,405);
  W->OnMouseButtonDown(G,Pointer(Start,Start,true));
  TestTrue(TEXT("Press on the main card captures its rotation gesture"),W->bGalleryCardDrag);
  W->OnMouseMove(G,Pointer(Right,Start,true));W->Tick(G,0,.1f);
  TestTrue(TEXT("Horizontal hold and drag rotates the rendered card"),W->GalleryTilt.X>10);
  TestEqual(TEXT("Main card gesture does not scroll the catalog"),W->GalleryScroll,0.f);
  W->OnMouseMove(G,Pointer(Left,Right,true));W->Tick(G,0,.2f);
  TestTrue(TEXT("Dragging back through the press point reverses rotation"),W->GalleryTilt.X<-10);
  W->OnMouseMove(G,Pointer(Start,Left,true));
  TestTrue(TEXT("Returning to the press position is still a drag, not a click"),W->bPointerMoved);
  W->OnMouseButtonUp(G,Pointer(Start,Start,false));W->Tick(G,0,.5f);
  TestTrue(TEXT("Released card settles back to its authored pose"),W->GalleryTilt.Size()<.1);
  TestFalse(TEXT("Drag does not switch upright/reversed meaning"),W->IsGalleryReversed());
  const auto ListStart=SPadmaDivinationWidget::GalleryPosition(0,0);
  W->OnMouseButtonDown(G,Pointer(ListStart,ListStart,true));
  W->OnMouseMove(G,Pointer(ListStart-FVector2D(0,150),ListStart,true));
  TestTrue(TEXT("Right-side vertical dragging still scrolls the catalog"),W->GalleryScroll>.3);
  W->OnMouseButtonUp(G,Pointer(ListStart-FVector2D(0,150),ListStart,false));
  W->OnMouseButtonDown(G,Pointer(Start,Start,true));W->OnMouseMove(G,Pointer(Right,Start,true));W->Tick(G,0,.1f);
  W->OnMouseCaptureLost(FCaptureLostEvent(0,0));W->Tick(G,0,.5f);
  TestFalse(TEXT("Capture loss clears held gesture"),W->bPointerPressed);
  TestTrue(TEXT("Capture loss also settles the card"),W->GalleryTilt.Size()<.1);
 }
 return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaTarotBoardDragTest,"DreamOfPadma.UI.Divination.DailyBoardCardDrag",
 EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaTarotBoardDragTest::RunTest(const FString&)
{
 for(float Scale:{1.f,.75f})
 {
  const auto G=FGeometry::MakeRoot(FVector2D(1600,900),FSlateLayoutTransform(Scale));
  auto Window=SNew(SWindow).ClientSize(FVector2D(1600,900));auto W=SNew(SPadmaDivinationWidget);
  Window->SetContent(W);W->Tick(G,0,1);W->SlatePrepass();
  FHittestGrid Grid;Grid.SetHittestArea(FVector2D::ZeroVector,FVector2D(1600,900));
  auto Paint=[&](){FSlateWindowElementList Elements(Window);FPaintArgs Args(&Window.Get(),Grid,FVector2D::ZeroVector,0,0);W->Paint(Args,G,FSlateRect(0,0,1600,900),Elements,0,FWidgetStyle(),true);};
  Paint();
  auto Pointer=[&](FVector2D P,FVector2D Previous,bool Held)
  {return FPointerEvent(0,G.LocalToAbsolute(P),G.LocalToAbsolute(Previous),Held?TSet<FKey>{EKeys::LeftMouseButton}:TSet<FKey>{},EKeys::LeftMouseButton,0,FModifierKeysState());};
  TestEqual(TEXT("Fixture starts on daily reading"),W->GetTarot().Selected,5);
  for(int32 Slot=0;Slot<2;++Slot)
  {
   const FVector2D Start=W->GetTarot().CurrentSpread().Selection[Slot].Position,End=Start+FVector2D(Slot?-140:140,0);
   W->OnMouseButtonDown(G,Pointer(Start,Start,true));
   TestEqual(TEXT("Each daily card captures its own drag"),W->BoardDragSlot,Slot);
   W->OnMouseMove(G,Pointer(End,Start,true));W->Tick(G,0,.2f);Paint();
   const auto Tilt=W->BoardCardPoses.FindRef(Slot).Tilt;
   TestTrue(TEXT("Daily card follows horizontal movement"),Slot?Tilt.X<-15:Tilt.X>15);
   TestTrue(TEXT("Other card retains its own pose"),W->BoardCardPoses.FindRef(1-Slot).Tilt.Size()<.1);
   const auto& Cards=W->GetTarot().CurrentCards();const float Base=W->Parallax.X+W->Parallax.Y;
   TestTrue(TEXT("Dragged card material receives the matching foil tilt"),FMath::Abs(W->CardMaterials[Cards[Slot].Card]->K2_GetScalarParameterValue(TEXT("Tilt"))-Base)> .4f);
   TestTrue(TEXT("Other card material is not driven by this drag"),FMath::Abs(W->CardMaterials[Cards[1-Slot].Card]->K2_GetScalarParameterValue(TEXT("Tilt"))-Base)<.01f);
   if(Slot==0)W->OnMouseButtonUp(G,Pointer(End,End,false));else W->OnMouseCaptureLost(FCaptureLostEvent(0,0));
   W->Tick(G,0,.6f);Paint();
   TestTrue(TEXT("Daily card smoothly returns on release or capture loss"),W->BoardCardPoses.FindRef(Slot).Tilt.Size()<.1);
   TestEqual(TEXT("A drag does not open detail"),W->GetDetailSlot(),INDEX_NONE);
   W->OnMouseButtonDown(G,Pointer(Start,Start,true));W->OnMouseButtonUp(G,Pointer(Start,Start,false));
   TestEqual(TEXT("A simple click still opens this card's detail"),W->GetDetailSlot(),Slot);
   W->RequestClose();W->Tick(G,0,.4f);Paint();
  }
  W->SelectSpread(0);
  TestTrue(TEXT("Changing spread clears stale card poses"),W->BoardCardPoses.IsEmpty());
 }
 return true;
}

#endif
