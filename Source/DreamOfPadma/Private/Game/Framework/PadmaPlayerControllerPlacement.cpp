#include "Game/Framework/PadmaPlayerController.h"
#include "Game/Run/PadmaRunSubsystem.h"

void APadmaPlayerController::ClearPlacement()
{
 PendingPlacementCard=PendingPlacementNode=PendingPlacementFamily=NAME_None;
}

bool APadmaPlayerController::IsPlacementOverlay()const
{
 return !PendingPlacementCard.IsNone()&&!PendingPlacementNode.IsNone()
  &&(ModalTitle==TEXT("确认部署")||ModalTitle==TEXT("选择附着对象"));
}

FPadmaRunCommandResult APadmaPlayerController::PreviewPlacement(FName Card,FName Node,FName Anchor)const
{
 // Deploy has no separate preview API. Its value-only copy runs the exact Core
 // validation/atomic command without publishing state, costs, RNG or notices.
 FPadmaRunRules Preview=Rules();
 return Preview.Deploy(Card,Node,Anchor);
}

void APadmaPlayerController::SelectMapTarget(FName Node)
{
 if(!Session||!Rules().HasRun()||Page!=TEXT("map")||bBattleMap||HasBlockingOverlay()
    ||!Rules().Content().Nodes.Contains(Node))return;
 const bool Changed=MapDetailNode()!=Node;
 SelectedNode=PreviewNode=Node;
 if(const auto* Card=Rules().Card(SelectedHand))
  if(const auto* Def=Rules().Content().Cards.Find(Card->DefId))
   if(Card->Zone==TEXT("hand")&&(Def->Family==TEXT("A")||Def->Family==TEXT("B")))BeginPlacement(Card->Id,Node);
 if(Changed){if(MapView)RefreshGraph();bNeedsMapDetails=true;}
}

void APadmaPlayerController::BeginPlacement(FName CardId,FName Node)
{
 if(!Session||!Rules().HasRun()||Page!=TEXT("map")||bBattleMap||HasBlockingOverlay())return;
 const auto* Card=Rules().Card(CardId);
 const auto* Def=Card?Rules().Content().Cards.Find(Card->DefId):nullptr;
 if(!Card||Card->Zone!=TEXT("hand")||!Def||!Rules().Content().Nodes.Contains(Node))
 {Notice=TEXT("手牌或目标已失效，请重新选择。");bNeedsRefresh=true;return;}
 if(Def->Family==TEXT("C"))
 {
  const auto Result=Rules().Deploy(CardId,Node);ApplyResult(Result);
  if(Result.bOk)SelectedHand=NAME_None;
  return;
 }
 if(Def->Family!=TEXT("A")&&Def->Family!=TEXT("B"))return;
 PendingPlacementCard=CardId;PendingPlacementNode=Node;PendingPlacementFamily=Def->Family;
 ModalTitle=Def->Family==TEXT("A")?TEXT("确认部署"):TEXT("选择附着对象");
 ModalText.Reset();bNeedsRefresh=true;
}

void APadmaPlayerController::BuildPlacementView(FPadmaGameView& V)const
{
 const auto* Card=Rules().Card(PendingPlacementCard);
 const auto* Def=Card?Rules().Content().Cards.Find(Card->DefId):nullptr;
 const auto* Node=Rules().Content().Nodes.Find(PendingPlacementNode);
 if(!Def||!Node||Card->Zone!=TEXT("hand")||Def->Family!=PendingPlacementFamily)
 {V.ModalText=TEXT("所选手牌或目标已经变化。请取消后重新选择，尚未扣费。");return;}
 V.ModalText=FString::Printf(TEXT("%s → %s\n费用：心流 %d\n取消不会扣费；确认时重新校验当前阶段、手牌和地块。"),
  *Def->DisplayName.ToString(),*Node->DisplayName.ToString(),Def->Cost);
 if(PendingPlacementFamily==TEXT("A"))
 {
  const auto Check=PreviewPlacement(PendingPlacementCard,PendingPlacementNode);
  V.ModalActions.Add(Button(TEXT("确认部署并支付心流"),TEXT("placement-confirm"),NAME_None,NAME_None,
   Check.bOk?TEXT("部署到上述固定目标"):Check.Text,Check.bOk));
  return;
 }
 int32 Count=0,Eligible=0;
 for(const auto& Target:Rules().Run().Cards)
 {
  const auto* TargetDef=Rules().Content().Cards.Find(Target.DefId);
  if(Target.Zone!=TEXT("world")||Target.Node!=PendingPlacementNode||!TargetDef||TargetDef->Family!=TEXT("A"))continue;
  ++Count;
  const auto Check=PreviewPlacement(PendingPlacementCard,PendingPlacementNode,Target.Id);
  if(Check.bOk)++Eligible;
  V.ModalActions.Add(Button(TargetDef->DisplayName.ToString()+TEXT(" · ")+Target.Id.ToString(),
   TEXT("placement-attach"),Target.Id,NAME_None,Check.bOk?TEXT("附着到此 A 卡并支付费用"):Check.Text,Check.bOk));
 }
 if(Count==0)V.ModalText+=TEXT("\n\n此地没有 A 卡。请先部署 A 卡，或取消后选择其他地块。");
 else if(Eligible==0)V.ModalText+=TEXT("\n\n此地暂无可用附着对象；请查看各项禁用原因，或取消后重选。");
 else V.ModalText+=TEXT("\n\n请选择一个 A 卡实例作为附着对象：");
}

void APadmaPlayerController::ConfirmPlacement(FName Anchor)
{
 if(!IsPlacementOverlay())return;
 if((PendingPlacementFamily==TEXT("A")&&!Anchor.IsNone())
    ||(PendingPlacementFamily==TEXT("B")&&Anchor.IsNone()))return;
 const auto* Card=Rules().Card(PendingPlacementCard);
 const auto* Def=Card?Rules().Content().Cards.Find(Card->DefId):nullptr;
 if(!Def||Def->Family!=PendingPlacementFamily)
 {Notice=TEXT("待部署卡牌已变化，请取消后重新选择。");bNeedsRefresh=true;return;}
 const FName CardId=PendingPlacementCard,Node=PendingPlacementNode;
 const auto Result=Rules().Deploy(CardId,Node,Anchor);
 if(!Result.bOk)
 {Notice=Result.Text;ModalText=Result.Text;bNeedsRefresh=true;return;}
 ApplyResult(Result);
 if(SelectedHand==CardId)SelectedHand=NAME_None;
 SelectedNode=PreviewNode=Node;
 RestoreOverlay({});OverlayHistory.Reset();UpdateInspectionGate();RefreshGraph();
}
