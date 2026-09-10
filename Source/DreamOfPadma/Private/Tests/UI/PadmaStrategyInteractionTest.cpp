#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Game/Framework/PadmaPlayerController.h"
#include "Game/Run/PadmaRunSubsystem.h"
#include "Game/Content/PadmaContentCatalog.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "UI/Input/PadmaPressGesture.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"

#if WITH_DEV_AUTOMATION_TESTS
extern bool LoadPadmaDemoTestContent(FPadmaContentSnapshot& Out, FString& Error);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaStrategyInteractionTest, "DreamOfPadma.UI.StrategyInteraction",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPadmaStrategyInteractionTest::RunTest(const FString& Parameters)
{
 FPadmaContentSnapshot Content;FString Error;
 if(!LoadPadmaDemoTestContent(Content,Error)){AddError(Error);return false;}
 UWorld::InitializationValues Values;
 Values.AllowAudioPlayback(false).CreatePhysicsScene(false).ShouldSimulatePhysics(false)
  .EnableTraceCollision(false).CreateNavigation(false).CreateAISystem(false);
 auto* World=UWorld::CreateWorld(EWorldType::Game,false,NAME_None,nullptr,true,ERHIFeatureLevel::Num,&Values);
 if(!TestNotNull(TEXT("UI test world"),World))return false;
 GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
 ON_SCOPE_EXIT { World->DestroyWorld(false);GEngine->DestroyWorldContext(World); };
 auto* Controller=World->SpawnActor<APadmaPlayerController>();
 if(!TestNotNull(TEXT("Controller"),Controller))return false;
 Controller->Catalog=LoadObject<UPadmaContentCatalog>(nullptr,TEXT("/Game/Padma/MVP/Playable/Definitions/DA_PlayableCatalog.DA_PlayableCatalog"));
 if(!TestNotNull(TEXT("Presentation catalog"),Controller->Catalog.Get()))return false;
 Controller->Session=NewObject<UPadmaRunSubsystem>(NewObject<UGameInstance>());
 Controller->Session->Initialize(Content);
 if(!TestTrue(TEXT("Start"),Controller->Session->StartNew().bOk))return false;
 auto& Rules=Controller->Rules();
 const uint64 BeforeHover=Rules.Revision();
 Controller->bMouseHeld=true;Controller->bMapDrag=true;
 Controller->UpdateMapHover(TEXT("gate"));
 TestTrue(TEXT("Held pointer cannot replace preview"),Controller->PreviewNode.IsNone());
 Controller->MapCameraDistance=3100;Controller->TickMapCamera(.1f);Controller->FitMapCamera();
 TestFalse(TEXT("Card hold cancels camera drag"),Controller->bMapDrag);
 TestEqual(TEXT("Card hold blocks camera fitting"),Controller->MapCameraDistance,3100.f);
 Controller->bMouseHeld=false;
 Controller->bNeedsRefresh=false;Controller->bNeedsMapDetails=false;
 Controller->UpdateMapHover(TEXT("fire"));
 TestFalse(TEXT("Hover never requests sidebar details"),Controller->bNeedsMapDetails);
 TestFalse(TEXT("Hover does not request full HUD"),Controller->bNeedsRefresh);
 TestEqual(TEXT("Hover preserves the clicked detail target"),Controller->MapDetailNode(),FName(TEXT("home")));
 TestEqual(TEXT("Hover leaves clicked home selection"),Controller->SelectedNode,FName(TEXT("home")));
 Controller->ReceiveIntent({TEXT("select-node"),TEXT("fire")});
 TestTrue(TEXT("Single click requests regional details"),Controller->bNeedsMapDetails);
 TestFalse(TEXT("Node-only click does not rebuild the whole HUD"),Controller->bNeedsRefresh);
 Controller->bNeedsMapDetails=false;Controller->UpdateMapHover(TEXT("gate"));
 TestFalse(TEXT("Crossing another tile cannot refresh the pinned sidebar"),Controller->bNeedsMapDetails);
 FPadmaGameView HoverView;Controller->BuildWorldView(HoverView);
 TestEqual(TEXT("Right panel projects the clicked node after crossing another tile"),HoverView.RightTitle,Rules.Content().Nodes.FindChecked(TEXT("fire")).DisplayName.ToString());
 FPadmaGameView PartialView;Controller->BuildWorldView(PartialView,true);
 TestTrue(TEXT("Regional projection is explicitly marked"),PartialView.bDetailOnly);
 TestTrue(TEXT("Regional hover never projects hand, filters or header"),PartialView.Hand.IsEmpty()&&PartialView.HandFilters.IsEmpty()&&PartialView.Resources.IsEmpty()&&PartialView.ObjectiveText.IsEmpty());
 Controller->UpdateMapHover(NAME_None);
 TestEqual(TEXT("Entering right UI retains last hover preview"),Controller->MapDetailNode(),FName(TEXT("fire")));
 TestEqual(TEXT("Inspection does not mutate gameplay"),Rules.Revision(),BeforeHover);
 Controller->ReceiveIntent({TEXT("menu")});Controller->UpdateMapHover(TEXT("gate"));
 TestEqual(TEXT("Modal blocks hover underneath"),Controller->MapDetailNode(),FName(TEXT("fire")));
 Controller->EscapePressed();Controller->SelectedNode=TEXT("home");Controller->PreviewNode=NAME_None;Controller->HoverNode=NAME_None;
 auto Find=[&](FName Definition)->FName{for(const auto& C:Rules.Run().Cards)if(C.DefId==Definition&&C.Zone==TEXT("hand"))return C.Id;return NAME_None;};
 const FName CCard=Find(TEXT("dancer")),ACard=Find(TEXT("dawn")),BCard=Find(TEXT("cherry")),OtherA=Find(TEXT("earth"));
 if(!TestTrue(TEXT("Noon"),Rules.Advance().bOk)
  ||!TestTrue(TEXT("Deploy C"),Rules.Deploy(CCard,TEXT("home")).bOk)
  ||!TestTrue(TEXT("Deploy first A"),Rules.Deploy(ACard,TEXT("home")).bOk)
  ||!TestTrue(TEXT("Deploy second A"),Rules.Deploy(OtherA,TEXT("home")).bOk))return false;
 Controller->ReceiveIntent({TEXT("select-card"),CCard});
 Controller->ReceiveIntent({TEXT("select-card"),ACard});
 Controller->ReceiveIntent({TEXT("select-card"),OtherA});
 Controller->ReceiveIntent({TEXT("hand"),BCard});
 FPadmaGameView View;Controller->BuildWorldView(View);
 const auto* Deploy=View.Right.FindByPredicate([](const auto& E){return E.Action.Command==TEXT("deploy");});
 if(!TestNotNull(TEXT("Attachment action"),Deploy))return false;
 TestTrue(TEXT("Mixed C/A group offers attachment"),Deploy->bEnabled);
 Controller->ReceiveIntent(Deploy->Action);
 TestTrue(TEXT("Attachment action opens choice without mutation"),Rules.Card(BCard)->AttachedTo.IsNone());
 Controller->ReceiveIntent({TEXT("placement-attach"),ACard});
 TestEqual(TEXT("Explicit candidate attaches to chosen A instance"),Rules.Card(BCard)->AttachedTo,ACard);
 Controller->ReceiveIntent({TEXT("select-node"),TEXT("fire")});
 TestEqual(TEXT("Destination inspection retains source group"),Controller->SelectedCards.Num(),3);
 TestTrue(TEXT("Remote source A cannot be an attachment target here"),Controller->SelectedACard(TEXT("fire")).IsNone());
 Controller->ReceiveIntent({TEXT("hand-filter"),TEXT("B")});
 View=FPadmaGameView();Controller->BuildWorldView(View);
 for(const auto& Entry:View.Hand)TestEqual(TEXT("B filter only shows B cards"),Entry.Badge,FString(TEXT("B")));
 Controller->ReceiveIntent({TEXT("hand"),Find(TEXT("reed"))});
 Controller->ReceiveIntent({TEXT("hand"),Find(TEXT("reed"))});
 TestTrue(TEXT("Second click clears hand selection"),Controller->SelectedHand.IsNone());
 Controller->SelectedNode=TEXT("home");Controller->NavigateTo(TEXT("home"));
 Controller->Draft.HomeOutfit=TEXT("unsaved-preview");
 Controller->ReceiveIntent({TEXT("scene"),TEXT("home")});
 TestEqual(TEXT("Local scene retains preparation draft"),Controller->Draft.HomeOutfit,FName(TEXT("unsaved-preview")));
 Controller->ReceiveIntent({TEXT("home-tab"),TEXT("protagonist")});
 TestEqual(TEXT("Returning to protagonist retains draft"),Controller->Draft.HomeOutfit,FName(TEXT("unsaved-preview")));
 Controller->ReceiveIntent({TEXT("load")});
 TestTrue(TEXT("Load opens a blocking confirmation"),Controller->HasBlockingOverlay());
 Controller->EscapePressed();
 TestEqual(TEXT("Esc closes confirmation without leaving preparation"),Controller->Page,FName(TEXT("home")));
 TestFalse(TEXT("Confirmation closed"),Controller->HasBlockingOverlay());
 Controller->DetailUnit=TEXT("detail-fixture");Controller->ArmedSkill=TEXT("cut");
 Controller->EscapePressed();
 TestTrue(TEXT("Esc closes unit details first"),Controller->DetailUnit.IsNone());
 TestEqual(TEXT("Closing details does not cancel underlying targeting"),Controller->ArmedSkill,FName(TEXT("cut")));
 Controller->EscapePressed();
 TestTrue(TEXT("Next Esc cancels targeting"),Controller->ArmedSkill.IsNone());
 Controller->NavigateTo(TEXT("map"));
 TestEqual(TEXT("Leaving preparation discards draft"),Controller->Draft.HomeOutfit,Rules.Run().Preparation.HomeOutfit);

 Controller->ReceiveIntent({TEXT("menu")});Controller->ReceiveIntent({TEXT("codex")});
 Controller->ReceiveIntent({TEXT("inspect-definition"),TEXT("dancer")});
 TestEqual(TEXT("Menu, codex and detail keep two parent contexts"),Controller->OverlayHistory.Num(),2);
 TestEqual(TEXT("Catalog details use definition domain"),Controller->DetailDefinition,FName(TEXT("dancer")));
 const auto Phase=Rules.Run().Calendar.Phase;
 Controller->ReceiveIntent({TEXT("advance")});Controller->ReceiveIntent({TEXT("hand"),BCard});
 TestEqual(TEXT("Underlying advance blocked while details own input"),Rules.Run().Calendar.Phase,Phase);
 TestTrue(TEXT("Underlying hand selection blocked"),Controller->SelectedHand.IsNone());
 Controller->EscapePressed();TestEqual(TEXT("Back restores codex"),Controller->ModalTitle,FString(TEXT("卡牌图鉴")));
 Controller->EscapePressed();TestEqual(TEXT("Back restores menu"),Controller->ModalTitle,FString(TEXT("菜单")));
 Controller->EscapePressed();TestFalse(TEXT("Third Back restores gameplay input"),Controller->HasBlockingOverlay());
 Controller->ReceiveIntent({TEXT("inspect-card"),CCard});
 View=FPadmaGameView();Controller->BuildInspectionView(View);
 TestEqual(TEXT("Instance detail opens without a battle unit"),Controller->DetailCard,CCard);
 TestEqual(TEXT("Dancer uses stable atlas identity"),View.ModalArtIndex,7);
 TestTrue(TEXT("Level is explicitly unconfigured"),View.ModalSubtitle.Contains(TEXT("等级未配置")));
 TestTrue(TEXT("Shared skills are their own section"),View.DetailSections.ContainsByPredicate([](const auto& S){return S.Title.Contains(TEXT("玩家共享技能库"));}));
 TestTrue(TEXT("Instance health is projected"),View.DetailSections.ContainsByPredicate([&](const auto& S){return S.Title==TEXT("当前属性")&&S.Text.Contains(FString::Printf(TEXT("生命  %.0f"),Rules.Card(CCard)->HP));}));
 Controller->EscapePressed();
 Controller->ReceiveIntent({TEXT("codex")});Controller->ReceiveIntent({TEXT("inspect-definition"),TEXT("door")});
 TestTrue(TEXT("Undiscovered C definition is not disclosed"),Controller->DetailDefinition.IsNone());
 Controller->EscapePressed();
 FPadmaPressGesture Gesture;Gesture.Begin(FVector2D(10,10),1.);
 TestFalse(TEXT("Short press cannot inspect"),Gesture.Inspect(1.49,true));
 TestTrue(TEXT("500 ms opens inspect exactly once"),Gesture.Inspect(1.5,true));
 TestFalse(TEXT("Holding cannot repeatedly push overlays"),Gesture.Inspect(2.,true));
 TestTrue(TEXT("Release after inspection never clicks or drops"),Gesture.Release(true)==FPadmaPressGesture::ERelease::None);
 Gesture.Begin(FVector2D(10,10),3.);Gesture.Move(FVector2D(30,10));Gesture.Move(FVector2D(10,10));
 TestFalse(TEXT("Moving away then back still cancels inspection"),Gesture.Inspect(4.,true));
 TestTrue(TEXT("Moved non-draggable entry cannot click"),Gesture.Release(false)==FPadmaPressGesture::ERelease::None);
 Gesture.Begin(FVector2D::ZeroVector,5.);Gesture.Move(FVector2D(20,0));
 TestTrue(TEXT("Actual drag retains its drop action"),Gesture.Release(true)==FPadmaPressGesture::ERelease::Drop);
 if(TestTrue(TEXT("Prepare encounter route"),Rules.Move({CCard},TEXT("fire")).bOk)
  &&TestTrue(TEXT("Continue encounter route"),Rules.Move({CCard},TEXT("plain")).bOk)
  &&TestTrue(TEXT("Create pending battle"),Rules.Move({CCard},TEXT("gate"),TEXT("encounter")).bOk))
 {
  Controller->Combat=NewObject<UPadmaCombatComponent>(Controller);
  Controller->bBattleMap=true;Controller->DetailUnit=TEXT("finishing-unit");
  FPadmaOverlayContext Parent;Parent.Title=TEXT("菜单");Controller->OverlayHistory.Add(Parent);
  Controller->OnBattleFinished(false,{});
  TestFalse(TEXT("Settlement replaces stale inspection"),Controller->OverlayContext().HasDetails());
  TestTrue(TEXT("Settlement clears parents"),Controller->OverlayHistory.IsEmpty());
  TestTrue(TEXT("Result return action remains routable"),Controller->IsOverlayIntentAllowed({TEXT("return-world")}));
 }
 return true;
}
#endif
