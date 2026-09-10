#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Game/Framework/PadmaPlayerController.h"
#include "Game/Run/PadmaRunSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

#if WITH_DEV_AUTOMATION_TESTS
extern bool LoadPadmaDemoTestContent(FPadmaContentSnapshot& Out,FString& Error);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaPlacementInteractionTest,"DreamOfPadma.UI.PlacementConfirmationAndAttachmentChoice",
 EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)

bool FPadmaPlacementInteractionTest::RunTest(const FString& Parameters)
{
 FPadmaContentSnapshot Content;FString Error;
 if(!LoadPadmaDemoTestContent(Content,Error)){AddError(Error);return false;}
 UWorld::InitializationValues Values;
 Values.AllowAudioPlayback(false).CreatePhysicsScene(false).ShouldSimulatePhysics(false)
  .EnableTraceCollision(false).CreateNavigation(false).CreateAISystem(false);
 auto* World=UWorld::CreateWorld(EWorldType::Game,false,NAME_None,nullptr,true,ERHIFeatureLevel::Num,&Values);
 if(!TestNotNull(TEXT("Placement test world"),World))return false;
 GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
 ON_SCOPE_EXIT {World->DestroyWorld(false);GEngine->DestroyWorldContext(World);};
 auto* Controller=World->SpawnActor<APadmaPlayerController>();
 if(!TestNotNull(TEXT("Controller"),Controller))return false;
 Controller->Session=NewObject<UPadmaRunSubsystem>(NewObject<UGameInstance>());
 Controller->Session->Initialize(Content);
 auto& Rules=Controller->Rules();
 if(!TestTrue(TEXT("Start"),Controller->Session->StartNew().bOk))return false;
 auto Find=[&](FName Definition)->FName{for(const auto& C:Rules.Run().Cards)if(C.DefId==Definition&&C.Zone==TEXT("hand"))return C.Id;return NAME_None;};
 const FName A=Find(TEXT("dawn")),OtherA=Find(TEXT("earth")),B=Find(TEXT("cherry")),OtherB=Find(TEXT("reed")),ThirdA=Find(TEXT("banquet")),ThirdB=Find(TEXT("cleopatra"));
 if(!TestTrue(TEXT("Placement fixture cards exist"),!A.IsNone()&&!OtherA.IsNone()&&!B.IsNone()&&!OtherB.IsNone()&&!ThirdA.IsNone()&&!ThirdB.IsNone()))return false;
 auto Unchanged=[&](const TCHAR* Label,const FPadmaRunState& Before)
 {TestTrue(Label,FPadmaRunState::StaticStruct()->CompareScriptStruct(&Before,&Rules.Run(),0));};
 auto Choice=[&](const FPadmaGameView& View,FName Id)->const FPadmaUIEntry*
 {return View.ModalActions.FindByPredicate([&](const auto& E){return E.Action.Command==TEXT("placement-attach")&&E.Action.Id==Id;});};

 auto Before=Rules.Run();const uint64 InitialRevision=Rules.Revision();
 Controller->ReceiveIntent({TEXT("hand"),A});Controller->ReceiveIntent({TEXT("select-node"),TEXT("home")});
 TestEqual(TEXT("A target click opens confirmation"),Controller->ModalTitle,FString(TEXT("确认部署")));
 FPadmaGameView View;Controller->BuildPlacementView(View);
 TestTrue(TEXT("Dawn confirm is disabled by deployment rules"),View.ModalActions.Num()==1&&!View.ModalActions[0].bEnabled);
 Unchanged(TEXT("Dawn preview does not deploy or pay"),Before);
 Controller->EscapePressed();
 TestTrue(TEXT("Cancel clears frozen placement"),Controller->PendingPlacementCard.IsNone()&&Controller->PendingPlacementNode.IsNone());
 TestEqual(TEXT("Preview and cancellation do not revise rules"),Rules.Revision(),InitialRevision);
 Controller->ReceiveIntent({TEXT("hand"),B});Controller->ReceiveIntent({TEXT("select-node"),TEXT("home")});
 View={};Controller->BuildPlacementView(View);
 TestTrue(TEXT("No A opens a clear no-target modal"),Controller->IsPlacementOverlay()&&View.ModalActions.IsEmpty()&&View.ModalText.Contains(TEXT("没有 A")));
 Unchanged(TEXT("No-target modal has no side effects"),Before);Controller->EscapePressed();

 if(!TestTrue(TEXT("Noon"),Rules.Advance().bOk))return false;
 Controller->ReceiveIntent({TEXT("hand"),A});Controller->ReceiveIntent({TEXT("select-node"),TEXT("home")});
 Before=Rules.Run();const double AFlow=Before.Resources.Flow;
 Controller->UpdateMapHover(TEXT("fire"));Controller->ReceiveIntent({TEXT("hand"),OtherA});Controller->ReceiveIntent({TEXT("select-node"),TEXT("fire")});
 TestEqual(TEXT("Modal cannot replace pending card"),Controller->PendingPlacementCard,A);
 TestEqual(TEXT("Modal cannot replace pending target"),Controller->PendingPlacementNode,FName(TEXT("home")));
 TestFalse(TEXT("Modal blocks keyboard pan"),Controller->IsMapKeyboardAvailable());
 Unchanged(TEXT("A confirmation waits without settlement"),Before);
 Controller->ReceiveIntent({TEXT("placement-confirm"),OtherA,TEXT("fire")});
 TestEqual(TEXT("A confirmation uses frozen target"),Rules.Card(A)->Node,FName(TEXT("home")));
 TestEqual(TEXT("A is deployed only after confirmation"),Rules.Card(A)->Zone,FName(TEXT("world")));
 TestEqual(TEXT("A pays authored deployment cost"),Rules.Run().Resources.Flow,AFlow-double(Content.Cards.FindChecked(TEXT("dawn")).Cost));
 TestFalse(TEXT("Successful confirmation closes modal"),Controller->HasBlockingOverlay());

 Controller->ReceiveIntent({TEXT("hand"),OtherA});Controller->ReceiveIntent({TEXT("deploy"),OtherA,TEXT("home")});
 TestEqual(TEXT("Sidebar cannot bypass A confirmation"),Rules.Card(OtherA)->Zone,FName(TEXT("hand")));
 Controller->ReceiveIntent({TEXT("placement-confirm")});
 if(!TestEqual(TEXT("Second A deploys"),Rules.Card(OtherA)->Zone,FName(TEXT("world"))))return false;
 Controller->SelectedCards={A};Controller->ReceiveIntent({TEXT("hand"),B});Controller->ReceiveIntent({TEXT("select-node"),TEXT("home")});
 Before=Rules.Run();View={};Controller->BuildPlacementView(View);
 const auto* First=Choice(View,A);const auto* Second=Choice(View,OtherA);
 if(!TestTrue(TEXT("B offers every eligible colocated A instance"),First&&First->bEnabled&&Second&&Second->bEnabled))return false;
 Unchanged(TEXT("B candidate preview has no side effects"),Before);
 Controller->ReceiveIntent({TEXT("placement-attach"),OtherA});
 TestEqual(TEXT("B attaches to explicit choice instead of selected group A"),Rules.Card(B)->AttachedTo,OtherA);
 TestEqual(TEXT("B pays authored cost"),Rules.Run().Resources.Flow,Before.Resources.Flow-double(Content.Cards.FindChecked(TEXT("cherry")).Cost));

 Controller->ReceiveIntent({TEXT("hand"),OtherB});Controller->ReceiveIntent({TEXT("select-node"),TEXT("home")});
 Before=Rules.Run();View={};Controller->BuildPlacementView(View);First=Choice(View,A);Second=Choice(View,OtherA);
 TestTrue(TEXT("Already attached A remains visible but disabled"),First&&First->bEnabled&&Second&&!Second->bEnabled);
 Controller->ReceiveIntent({TEXT("placement-attach"),OtherA});
 Unchanged(TEXT("Disabled attachment cannot bypass Core legality"),Before);
 Controller->EscapePressed();Unchanged(TEXT("B cancel preserves cards and resources"),Before);
 Controller->ReceiveIntent({TEXT("select-node"),TEXT("home")});
 const auto BeforeOccupied=Rules.Run();
 if(!TestTrue(TEXT("A candidate becomes occupied after preview"),Rules.Deploy(ThirdB,TEXT("home"),A).bOk))return false;
 Before=Rules.Run();Controller->ReceiveIntent({TEXT("placement-attach"),A});
 Unchanged(TEXT("Confirm revalidates a previously eligible A"),Before);
 if(!TestTrue(TEXT("Restore fixture before external attachment"),Rules.Restore(BeforeOccupied).bOk))return false;
 if(!TestTrue(TEXT("External state change deploys pending B"),Rules.Deploy(OtherB,TEXT("home"),A).bOk))return false;
 Before=Rules.Run();Controller->ReceiveIntent({TEXT("placement-attach"),A});
 Unchanged(TEXT("Stale hand instance cannot be deployed twice"),Before);Controller->EscapePressed();

 Controller->ReceiveIntent({TEXT("hand"),ThirdA});Controller->ReceiveIntent({TEXT("select-node"),TEXT("home")});
 if(!TestTrue(TEXT("External phase changes to dusk"),Rules.Advance().bOk))return false;
 Before=Rules.Run();Controller->ReceiveIntent({TEXT("placement-confirm")});
 Unchanged(TEXT("Confirm revalidates current phase"),Before);Controller->EscapePressed();
 Controller->bMouseHeld=true;TestFalse(TEXT("Held card blocks keyboard pan"),Controller->IsMapKeyboardAvailable());Controller->bMouseHeld=false;
 Controller->Page=TEXT("settings");TestFalse(TEXT("Non-map page blocks keyboard pan"),Controller->IsMapKeyboardAvailable());Controller->Page=TEXT("map");

 TestEqual(TEXT("Neutral war is 50:50"),APadmaPlayerController::WarPlayerShare(0,100),.5f);
 TestEqual(TEXT("Positive half-goal is 75 percent"),APadmaPlayerController::WarPlayerShare(50,100),.75f);
 TestEqual(TEXT("Negative half-goal is 25 percent"),APadmaPlayerController::WarPlayerShare(-50,100),.25f);
 TestEqual(TEXT("Player share caps at goal"),APadmaPlayerController::WarPlayerShare(200,100),1.f);
 TestEqual(TEXT("Enemy share caps below negative goal"),APadmaPlayerController::WarPlayerShare(-200,100),0.f);
 return !HasAnyErrors();
}
#endif
