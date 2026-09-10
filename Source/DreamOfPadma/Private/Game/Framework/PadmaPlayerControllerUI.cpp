#include "Game/Framework/PadmaPlayerController.h"
#include "Game/Content/PadmaContentCatalog.h"
#include "Game/Run/PadmaRunSubsystem.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Gameplay/ACT/Authoring/PadmaACTAuthoring.h"
#include "Engine/World.h"
#include "Engine/Texture2D.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "UI/Screens/PadmaCardArt.h"

FPadmaUIEntry APadmaPlayerController::Button(const FString& Title,FName Command,FName Id,FName Other,const FString& Description,bool Enabled,bool Selected) const
{
	FPadmaUIEntry E;E.Title=Title;E.Description=Description;E.Action={Command,Id,Other};E.bEnabled=Enabled;E.bSelected=Selected;
	E.bPrimary=Command==TEXT("move")||Command==TEXT("deploy")||Command==TEXT("advance")||Command==TEXT("apply-preparation")||Command==TEXT("skill-confirm")||Command==TEXT("synthesis-confirm")||Command==TEXT("placement-confirm");
	return E;
}
FString APadmaPlayerController::CardDescription(FName Id) const
{
	const auto* C=Rules().Card(Id);if(!C)return TEXT("卡牌不存在");
	const auto* D=Rules().Content().Cards.Find(C->DefId);if(!D)return TEXT("定义不存在");
	FString S=FString::Printf(TEXT("%s / %s · %s\n实例 %s · %s\n生命 %.0f · 攻击 %.0f · 防御 %d\n%s"),*D->Family.ToString(),*D->Attribute.ToString(),*D->DisplayName.ToString(),*C->Id.ToString(),*C->Node.ToString(),C->HP,C->Attack,D->Defense,*D->Coverage.ToString());
	if(D->Family==TEXT("A"))S+=FString::Printf(TEXT("\n倾向 [%.0f,%.0f]"),D->Tendency.X,D->Tendency.Y);
	if(D->Family==TEXT("B"))S+=FString::Printf(TEXT("\n当前场域 [%.0f,%.0f] · 附着 %s"),C->Field.X,C->Field.Y,*C->AttachedTo.ToString());
	if(const auto* Policy=Rules().Run().Mobility.Find(C->DefId))S+=FString::Printf(TEXT("\n移动 %s · 条件信仰≥%.0f · 禁入%s"),*Policy->Mode.ToString(),Policy->MinFaith,*Policy->ForbiddenTerrain);
	return S;
}
void APadmaPlayerController::RefreshView()
{
	if(!Screen||!Session||!Rules().HasRun())return;
	FPadmaGameView V;const auto& R=Rules().Run();const auto& D=Rules().Content();
	V.PageKey=bBattleMap?FName(TEXT("battle")):Page;
	V.Heading=TEXT("莲华之梦");
	V.CalendarTitle=FString::Printf(TEXT("第 %d 章 · 第 %d 时代"),R.Calendar.Chapter,R.Calendar.Era);
	V.DayText=FString::Printf(TEXT("第 %d 日 / 7"),R.Calendar.Day);
	if(R.MapLayout.bHexWilderness){V.CalendarTitle=R.Calendar.Era==1?TEXT("图腾时代"):FString::Printf(TEXT("第 %d 时代"),R.Calendar.Era);V.DayText=FString::Printf(TEXT("第 %d 日 / 第 7 日"),R.Calendar.Day);}
	V.PhaseIndex=R.Calendar.Phase;
	V.Resources={
		{TEXT("心流"),FString::Printf(TEXT("%.1f"),R.Resources.Flow),FString::Printf(TEXT("上限 %.0f"),Rules().P(TEXT("flowCap"))),FLinearColor(.25f,.7f,.9f)},
		{TEXT("计算力"),FString::Printf(TEXT("%.1f"),R.Resources.Calc),FString::Printf(TEXT("上限 %.0f"),Rules().P(TEXT("calcCap"))),FLinearColor(.35f,.8f,.8f)},
		{TEXT("全局熵"),FString::Printf(TEXT("%.0f"),R.Resources.Entropy),TEXT("本局熵值"),FLinearColor(.65f,.4f,.85f)},
		{TEXT("信仰"),FString::Printf(TEXT("%.0f"),R.Resources.Faith),TEXT("本局信仰"),FLinearColor(.85f,.63f,.3f)}};
	V.WarProgress=WarPlayerShare(R.War,Rules().P(TEXT("warGoal")));
	V.WarText=FString::Printf(TEXT("战争天平  我方 %.0f%% : 王庭 %.0f%%"),V.WarProgress*100.,(1.-V.WarProgress)*100.);
	V.Status=FString::Printf(TEXT("第 %d 章 · 第 %d 时代 · 第 %d 天 %s    心流 %.1f / %.0f    计算力 %.1f / %.0f    信仰 %.0f    熵 %.0f    天平 %.0f / %.0f"),R.Calendar.Chapter,R.Calendar.Era,R.Calendar.Day,D.Phases.IsValidIndex(R.Calendar.Phase)?*D.Phases[R.Calendar.Phase].ToString():TEXT(""),R.Resources.Flow,Rules().P(TEXT("flowCap")),R.Resources.Calc,Rules().P(TEXT("calcCap")),R.Resources.Faith,R.Resources.Entropy,R.War,Rules().P(TEXT("warGoal")));
	V.Notice=Notice.IsEmpty()?Rules().Notice():Notice;
	if(Page!=TEXT("map")&&!bBattleMap)V.Navigation.Add(Button(TEXT("返回沙盘"),TEXT("page"),TEXT("map")));
	V.Navigation.Add(Button(TEXT("全局备战"),TEXT("settings"),NAME_None,NAME_None,TEXT(""),!bBattleMap&&!Rules().IsDialogue()));
	V.Navigation.Add(Button(TEXT("菜单"),TEXT("menu"),NAME_None,NAME_None,TEXT(""),!Rules().IsDialogue()));
	if(bBattleMap)BuildCombatView(V);
	else if(Page==TEXT("home")||Page==TEXT("settings")||Page==TEXT("scene"))BuildHomeView(V);
	else BuildWorldView(V);
	if(Rules().IsDialogue())
	{
		for(const auto& Pair:D.Dialogues)
		{
			const auto& L=Pair.Value;
			if(L.DialogueId!=Rules().Dialogue().DialogueId||L.LineId!=Rules().Dialogue().Line)continue;
			V.ModalTitle=L.Title.ToString();V.ModalText=L.Speaker.ToString()+TEXT("\n\n")+L.Text.ToString();
			if(L.Choices.IsEmpty())V.ModalActions.Add(Button(TEXT("继续"),TEXT("dialogue-next")));
			for(const auto& C:L.Choices)V.ModalActions.Add(Button(C.Label.ToString(),TEXT("dialogue-next"),C.Id));
			V.ModalActions.Add(Button(TEXT("暂时离开"),TEXT("dialogue-cancel")));break;
		}
	}
	else if(!ModalTitle.IsEmpty())
	{
		V.ModalTitle=ModalTitle;V.ModalText=ModalText;
		if(IsPlacementOverlay())BuildPlacementView(V);
		else if(ModalTitle==TEXT("菜单"))
		{
			V.ModalActions.Add(Button(TEXT("保存进度"),TEXT("save"),NAME_None,NAME_None,TEXT(""),!Rules().IsBattle()));
			V.ModalActions.Add(Button(TEXT("读取存档"),TEXT("load"),NAME_None,NAME_None,TEXT(""),!Rules().IsBattle()));
			V.ModalActions.Add(Button(TEXT("开始新局"),TEXT("new"),NAME_None,NAME_None,TEXT(""),!Rules().IsBattle()));
			V.ModalActions.Add(Button(TEXT("本局行记"),TEXT("journal")));
			V.ModalActions.Add(Button(TEXT("卡牌图鉴"),TEXT("codex")));
			V.ModalActions.Add(Button(TEXT("试玩移动规则"),TEXT("mobility-menu"),NAME_None,NAME_None,TEXT("高级配置 · 修改立即生效"),!Rules().IsBattle()));
		}
		else if(ModalTitle==TEXT("卡牌图鉴"))
		{
			V.ModalText=TEXT("点击卡牌查看定义 · 已持有角色可长按查看实例详情");
			for(FName Id:D.CardOrder)if(const auto* Def=D.Cards.Find(Id))
			{
				const bool Known=Def->Family!=TEXT("C")||R.Cards.ContainsByPredicate([Id](const auto& C){return C.DefId==Id&&(C.Zone==TEXT("hand")||C.Zone==TEXT("world"));})||R.SynthesisHistory.ContainsByPredicate([Id](const auto& S){return S.Selected==Id&&S.bSuccess;});
				auto Entry=Button(Def->DisplayName.ToString(),TEXT("inspect-definition"),Id,NAME_None,Known?TEXT("查看定义"):TEXT("成功召唤后解锁详情"),Known);
				Entry.bCard=true;Entry.Badge=Def->Family.ToString();Entry.ArtIndex=Known?PadmaCardArtIndex(Id):INDEX_NONE;
				V.ModalCards.Add(Entry);
			}
		}
		else if(ModalTitle==TEXT("读取存档"))V.ModalActions.Add(Button(TEXT("确认读取"),TEXT("load-confirm")));
		else if(ModalTitle==TEXT("撤退确认"))V.ModalActions.Add(Button(TEXT("确认撤退并回滚"),TEXT("exit-confirm")));
		else if(ModalTitle==TEXT("试玩移动规则"))
			for(FName Id:D.CardOrder)if(const auto* Def=D.Cards.Find(Id))if(const auto* M=R.Mobility.Find(Id))
				V.ModalActions.Add(Button(Def->DisplayName.ToString(),TEXT("mobility"),Id,NAME_None,
					M->Mode==TEXT("movable")?TEXT("可移动"):M->Mode==TEXT("immovable")?TEXT("不可移动"):TEXT("条件移动")));
		if(bBattleFinished)V.ModalActions.Add(Button(TEXT("返回沙盘"),TEXT("return-world")));
		else if(ModalTitle==TEXT("合成预览"))V.ModalActions.Add(Button(TEXT("确认合成并支付费用"),TEXT("synthesis-confirm")));
		else if(ModalTitle==TEXT("创建地图新局"))
		{
			V.bMapSeedInput=true;V.MapSeed=MapSeedDraft;
			V.ModalActions.Add(Button(TEXT("按种子生成并开始新局"),TEXT("map-generate")));
			V.ModalActions.Insert(Button(TEXT("开始固定绘景新手教程"),TEXT("map-tutorial")),0);
			V.ModalActions.Add(Button(TEXT("使用资产中当前布局开始新局"),TEXT("map-authored")));
		}
		V.ModalActions.Add(Button(OverlayHistory.IsEmpty()?TEXT("关闭  ·  Esc"):TEXT("返回上一页  ·  Esc"),TEXT("close"),NAME_None,NAME_None,TEXT(""),!bBattleFinished));
	}
	BuildInspectionView(V);
	if(!R.Outcome.IsNone()&&!bBattleMap&&ModalTitle!=TEXT("创建地图新局"))
	{
		V.ModalTitle=TEXT("本局胜利");V.ModalText=R.Outcome==TEXT("core")?TEXT("王庭核心生命归零。"):TEXT("战局天平抵达玩家胜利边界。");
		V.ModalActions={Button(TEXT("保存本局"),TEXT("save")),Button(TEXT("开始新局"),TEXT("new"))};
	}
	if(!V.ModalTitle.IsEmpty())
	{
		for(const auto& Parent:OverlayHistory)V.OverlayPath.Add(Parent.Key());
		FName Key=OverlayContext().Key();if(Key.IsNone())Key=FName(*V.ModalTitle);
		V.OverlayPath.Add(Key);
	}
	UpdateInspectionGate();Screen->Present(V);
}
float APadmaPlayerController::WarPlayerShare(double War,double Goal)
{
 return FMath::Clamp(float(.5+.5*(War/FMath::Max(UE_DOUBLE_SMALL_NUMBER,Goal))),0.f,1.f);
}
void APadmaPlayerController::RefreshMapDetails()
{
 if(!Screen||!Session||!Rules().HasRun()||Page!=TEXT("map")||bBattleMap||HasBlockingOverlay())return;
 FPadmaGameView V;BuildWorldView(V,true);Screen->Present(V);
}
void APadmaPlayerController::BuildWorldView(FPadmaGameView& V,bool bDetailsOnly)
{
 const auto& D=Rules().Content();const auto& R=Rules().Run();
 V.bDetailOnly=bDetailsOnly;
 V.HexGrid=R.MapLayout.HexGrid;
 V.bWorldMap=true; V.bPaintedTutorial=R.MapLayout.bHexWilderness; const FName DetailNode=MapDetailNode(); V.DetailNode=DetailNode;
 if(!bDetailsOnly)V.ObjectiveText=FString::Printf(TEXT("摧毁王庭核心，或让战争天平抵达胜利边界。\n\n归处 %.0f   ·   王庭 %.0f"),R.PlayerCoreHP,R.RulerCoreHP);
 V.LeftTitle=TEXT("此地驻军 · 点击选择");
 auto NameOf=[&](FName Id){const auto* N=D.Nodes.Find(Id);return N?N->DisplayName.ToString():FString(TEXT("未知节点"));};
 const auto* Node=D.Nodes.Find(DetailNode);const auto* N=R.Nodes.Find(DetailNode);
 V.RightTitle=Node?Node->DisplayName.ToString():TEXT("选择一个节点");
 V.NodeArtIndex=DetailNode==Rules().HomeNode()?0:Node&&Node->Terrain.ToString().Contains(TEXT("山"))?1:N&&N->Owner==TEXT("ruler")?1:4;
 if(Node&&N)
 {
  const FString OwnerLabel=N->Owner==TEXT("player")?TEXT("我方领地"):N->Owner==TEXT("ruler")?TEXT("统治者领地"):TEXT("中立地区");
  V.OwnershipText=OwnerLabel;V.TerrainText=Node->Terrain.ToString();V.NodeUV=Node->Position/100.;
  V.NodeStateText=!Node->bHabitable?TEXT("不宜居 · 不可占领"):N->Owner==TEXT("ruler")?(Node->EnemyCount>0?TEXT("敌方驻守"):TEXT("敌方领地 · 暂无驻军")):N->Owner==TEXT("player")?TEXT("我方控制"):TEXT("宜居 · 可占领");
  V.EnemyCount=N->Owner==TEXT("ruler")?Node->EnemyCount:0;V.EnemyName=DetailNode==Rules().BossNode()?TEXT("王庭核心守卫"):TEXT("王庭守卫");
  V.EnemyHealth=Rules().P(DetailNode==Rules().BossNode()?TEXT("bossHP"):TEXT("enemyHP"));
  V.RightText=OwnerLabel+TEXT("  ·  ")+Node->Terrain.ToString()+TEXT("\n\n")+Node->Description.ToString();
  if(N->Owner==TEXT("ruler"))V.RightText+=FString::Printf(TEXT("\n\n敌方驻军  %d 名守卫\n生命 %.0f · 攻击 %.0f · 防御 %.0f"),Node->EnemyCount,Rules().P(DetailNode==Rules().BossNode()?TEXT("bossHP"):TEXT("enemyHP")),Rules().P(TEXT("enemyAtk")),Rules().P(TEXT("enemyDef")));
  if(Node->AnchorKind!=EPadmaMapAnchorKind::None)
   V.RightText+=R.CompletedNPCs.Contains(Node->NPCId)?TEXT("\n\n人物剧情 · 已完成"):R.SeenNPCs.Contains(Node->NPCId)?TEXT("\n\n人物剧情 · 已相遇"):TEXT("\n\n人物剧情 · 尚未相遇");
 }
 for(const auto& C:R.Cards)if(C.Zone==TEXT("world")&&C.Node==DetailNode)
 {
  const auto* Def=D.Cards.Find(C.DefId);if(!Def)continue;
  const bool Attached=!C.AttachedTo.IsNone();
  const bool Selected=SelectedCards.Contains(C.Id)||(Attached&&SelectedCards.Contains(C.AttachedTo));
  auto Entry=Button((Attached?FString(TEXT("↳ ")):FString())+Def->Family.ToString()+TEXT("  ")+Def->DisplayName.ToString(),TEXT("select-card"),Attached?C.AttachedTo:C.Id,NAME_None,
   Attached?TEXT("附着同行 · 长按详情"):FString::Printf(TEXT("生命 %.0f  ·  %s · 长按详情"),C.HP,Selected?TEXT("已选中"):TEXT("可选择")),true,Selected);
  Entry.bInspectable=true;Entry.InspectAction={TEXT("inspect-card"),C.Id};Entry.ArtIndex=PadmaCardArtIndex(C.DefId);V.Left.Add(Entry);
 }
 const FName SelectedA=SelectedACard();
 if(!SelectedCards.IsEmpty())
 {
  const auto* First=Rules().Card(SelectedCards[0]);
  V.MoveSource=First?First->Node:NAME_None;V.MovementRoute=TEXT("当前：")+(First?NameOf(First->Node):FString())+TEXT(" → ")+NameOf(DetailNode);
  TArray<FName> Moving=SelectedCards;for(const auto& C:R.Cards)if(!C.AttachedTo.IsNone()&&Moving.Contains(C.AttachedTo))Moving.AddUnique(C.Id);
  for(FName Id:Moving)if(const auto* C=Rules().Card(Id))if(const auto* Def=D.Cards.Find(C->DefId))
  {auto E=Button(Def->DisplayName.ToString(),TEXT("inspect-card"),Id,NAME_None,C->AttachedTo.IsNone()?TEXT("随队移动"):TEXT("附着同行"));E.bCard=true;E.bInspectable=true;E.InspectAction=E.Action;E.Badge=Def->Family.ToString();E.ArtIndex=PadmaCardArtIndex(C->DefId);V.MovingCards.Add(E);}
  FString Group=First?NameOf(First->Node):FString();
  Group+=TEXT(" → ")+NameOf(DetailNode)+TEXT("\n");
  for(FName Id:SelectedCards)if(const auto* C=Rules().Card(Id))if(const auto* Def=D.Cards.Find(C->DefId))
  {
   Group+=Def->DisplayName.ToString()+TEXT("  ");
   if(Def->Family==TEXT("A")){for(const auto& B:R.Cards)if(B.AttachedTo==C->Id)if(const auto* BD=D.Cards.Find(B.DefId))Group+=TEXT("＋")+BD->DisplayName.ToString()+TEXT("  ");}
  }
  V.Right.Add(Button(TEXT("待操作卡组"),TEXT("card-details"),SelectedCards[0],NAME_None,Group));
  if(SelectedHand.IsNone())
  {
   FPadmaCardMovePreview Preview;const auto Result=Rules().PreviewMove(SelectedCards,DetailNode,Preview);
   const bool Battle=Result.bOk&&N&&N->Owner==TEXT("ruler")&&(!R.MapLayout.bHexWilderness||(Node&&Node->EnemyCount>0));
   if(Battle)
   {
    V.Right.Add(Button(TEXT("回合战斗 · Encounter"),TEXT("mode"),TEXT("encounter"),NAME_None,TEXT("由移动的 C 卡参战"),true,BattleChoice==TEXT("encounter")));
    V.Right.Add(Button(TEXT("动作战斗 · ACT"),TEXT("mode"),TEXT("act"),NAME_None,TEXT("使用全局备战中的独立阵容"),true,BattleChoice==TEXT("act")));
   }
   V.Right.Add(Button(Battle?TEXT("移动并进入战斗"):TEXT("移动到此处"),TEXT("move"),DetailNode,NAME_None,
    Result.bOk?FString::Printf(TEXT("消耗计算力 %.0f"),Preview.Calc):Result.Text,Result.bOk));
   const auto* Def=First?D.Cards.Find(First->DefId):nullptr;
   if(Def&&(Def->Id==TEXT("vitruvian")||Def->Id==TEXT("banquet")))V.Right.Add(Button(TEXT("使用沙盘技能"),TEXT("ability"),First->Id));
   if(Def&&Def->Id==TEXT("banquet"))V.Right.Add(Button(TEXT("反向兑换"),TEXT("ability-reverse"),First->Id));
  }
  V.Right.Add(Button(TEXT("取消卡组选择"),TEXT("clear-cards")));
 }
 if(!SelectedHand.IsNone())if(const auto* C=Rules().Card(SelectedHand))if(const auto* Def=D.Cards.Find(C->DefId))
 {
  V.Right.Add(Button(TEXT("已选手牌 · ")+Def->DisplayName.ToString(),TEXT("card-details"),C->Id,NAME_None,
   Def->Family==TEXT("B")?TEXT("点击地图目标，再选择此地的 A 卡"):Def->Family==TEXT("A")?TEXT("点击地图目标，确认后部署"):TEXT("部署到正在查看的节点")));
  const bool Can=R.Calendar.Phase==1&&N&&N->Owner==TEXT("player");
  V.Right.Add(Button(Def->Family==TEXT("B")?TEXT("选择附着对象"):TEXT("部署到此处"),TEXT("deploy"),SelectedHand,DetailNode,
   Can?FString::Printf(TEXT("消耗心流 %.0f"),Def->Family==TEXT("C")?Rules().P(TEXT("deployC")):double(Def->Cost)):
    R.Calendar.Phase!=1?TEXT("晌午时可部署"):TEXT("需要我方领地"),Can));
  V.Right.Add(Button(TEXT("取消手牌选择"),TEXT("hand"),SelectedHand));
 }
 V.Right.Add(Button(TEXT("查看局部场景"),TEXT("scene"),DetailNode,NAME_None,TEXT(""),Node!=nullptr));
 if(DetailNode==Rules().HomeNode())V.Right.Add(Button(TEXT("进入归处"),TEXT("home")));
 if(Node&&(!Node->DialogueId.IsNone()||Node->Type==TEXT("story")||Node->Type==TEXT("forge")||R.MapLayout.Checkpoints.ContainsByPredicate([&](const auto& C){return C.NodeId==DetailNode;})))
  V.Right.Add(Button(Node->Type==TEXT("forge")?TEXT("使用熔炉补给"):TEXT("与此地人物交谈"),TEXT("operation"),DetailNode,NAME_None,
   N&&N->bUsed?TEXT("此处事件已完成"):TEXT("完成对话后结算"),N&&!N->bUsed&&N->Owner==TEXT("player")));
 if(!bDetailsOnly)
 {
 V.HandCount=R.Hand.Num();V.DeckCount=R.Deck.Num();V.DiscardCount=R.Discard.Num();
 V.HandFilters.Add(Button(TEXT("全部"),TEXT("hand-filter"),NAME_None,NAME_None,TEXT(""),true,HandFilter.IsNone()));
 for(FName Family:{FName(TEXT("A")),FName(TEXT("B")),FName(TEXT("C"))})V.HandFilters.Add(Button(Family.ToString(),TEXT("hand-filter"),Family,NAME_None,TEXT(""),true,HandFilter==Family));
 static const FName ArtIds[]={TEXT("dawn"),TEXT("earth"),TEXT("banquet"),TEXT("cherry"),TEXT("reed"),TEXT("cleopatra"),TEXT("vitruvian"),TEXT("dancer"),TEXT("door")};
 for(FName Id:R.Hand)if(const auto* C=Rules().Card(Id))if(const auto* Def=D.Cards.Find(C->DefId))
 {
  if(!HandFilter.IsNone()&&HandFilter!=Def->Family)continue;
  auto E=Button(Def->DisplayName.ToString(),TEXT("hand"),Id,NAME_None,
   FString::Printf(TEXT("心流 %.0f · %s"),Def->Family==TEXT("C")?Rules().P(TEXT("deployC")):double(Def->Cost),
    Def->Family==TEXT("A")?TEXT("部署"):Def->Family==TEXT("B")?TEXT("附着"):TEXT("部署")),true,Id==SelectedHand);
  E.bCard=true;E.bInspectable=true;E.bDraggable=Def->Family==TEXT("A")||Def->Family==TEXT("B");E.InspectAction={TEXT("inspect-card"),C->Id};E.Badge=Def->Family.ToString();for(int32 I=0;I<9;++I)if(Def->Id==ArtIds[I])E.ArtIndex=I;V.Hand.Add(E);
 }
 V.HandActions.Add(Button(TEXT("图鉴"),TEXT("codex")));
 V.HandActions.Add(Button(TEXT("合成"),TEXT("synthesis"),SelectedA,NAME_None,SelectedA.IsNone()?TEXT("先选择驻军中的 A 卡"):TEXT("查看概率与费用"),!SelectedA.IsNone()));
 const TCHAR* Next[]={TEXT("推进至晌午"),TEXT("推进至黄昏"),TEXT("迎接下一日")};
 const TCHAR* Hints[]={TEXT("黎明 · 整理手牌，准备出发"),TEXT("晌午 · 部署、移动与合成"),TEXT("黄昏 · 本日行动即将结束")};
 V.PhaseHint=Hints[FMath::Clamp(R.Calendar.Phase,0,2)];
 V.PhaseAction=Button(Next[FMath::Clamp(R.Calendar.Phase,0,2)],TEXT("advance"));
 }
 TMap<FName,int32> Indices;
 for(FName Id:D.NodeOrder)if(const auto* Def=D.Nodes.Find(Id))if(const auto* State=R.Nodes.Find(Id))
 {
  Indices.Add(Id,V.MinimapNodes.Num());
  V.MinimapNodes.Add({Id,Def->Position,State->Owner==TEXT("player")?FLinearColor(.2f,.8f,.65f):State->Owner==TEXT("ruler")?FLinearColor(.9f,.3f,.25f):FLinearColor(.75f,.65f,.4f),Id==DetailNode});
  auto& MN=V.MinimapNodes.Last();MN.Title=Def->DisplayName.ToString();MN.Owner=State->Owner;MN.bWilderness=Def->bWilderness;MN.bHabitable=Def->bHabitable;MN.HexQ=Def->HexQ;MN.HexR=Def->HexR;MN.bHovered=Id==HoverNode;
  MN.Garrison=State->Owner==TEXT("ruler")?Def->EnemyCount:0;MN.ArtIndex=MN.Garrison>0?6:INDEX_NONE;
  for(const auto& C:R.Cards)if(C.Zone==TEXT("world")&&C.Node==Id){++MN.Garrison;if(MN.ArtIndex==INDEX_NONE)MN.ArtIndex=PadmaCardArtIndex(C.DefId);}
 }
 for(const auto& Edge:D.Edges)if(Indices.Contains(Edge.From)&&Indices.Contains(Edge.To))
  V.MinimapEdges.Add({Indices[Edge.From],Indices[Edge.To],!Edge.UnlockFlag.IsNone()&&!R.Unlocks.Contains(Edge.UnlockFlag)});
 if(V.bPaintedTutorial&&!bDetailsOnly)
 {
  const int32 Priority[]={2,0,6,3,1,7,8,4,5};V.Hand.StableSort([&](const auto& A,const auto& B){return (A.ArtIndex>=0&&A.ArtIndex<9?Priority[A.ArtIndex]:99)<(B.ArtIndex>=0&&B.ArtIndex<9?Priority[B.ArtIndex]:99);});
 }
 if(!bDetailsOnly)V.Footer=V.bPaintedTutorial?TEXT("滚轮缩放 · 中键拖动 · WASD / 方向键平移 · Home 全览"):TEXT("滚轮缩放 · 中键拖动 · WASD 平移 · Q/E 旋转 · Home 全览");
}

void APadmaPlayerController::BuildHomeView(FPadmaGameView& V)
{
	const auto& D=Rules().Content();
	V.HandTitle=TEXT("阵容技能");V.HandHint=TEXT("保存阵容后用于出战");
	V.LeftTitle=Page==TEXT("scene")?TEXT("局部场景"):Page==TEXT("settings")?TEXT("全局备战"):TEXT("归处");
	V.Left.Add(Button(TEXT("节点细节"),TEXT("home-tab"),TEXT("node"),NAME_None,TEXT("实际放置内容"),true,HomeTab==TEXT("node")));
	V.Left.Add(Button(TEXT("主角展示"),TEXT("home-tab"),TEXT("protagonist"),NAME_None,TEXT("坐镇归处的主角"),true,HomeTab==TEXT("protagonist")));
	V.Left.Add(Button(TEXT("其他模式"),TEXT("home-tab"),TEXT("other"),NAME_None,TEXT("独立 ACT / FPS 招募编队"),true,HomeTab==TEXT("other")));
	V.Left.Add(Button(TEXT("返回地图"),TEXT("page"),TEXT("map")));
	if(Page==TEXT("scene")||HomeTab==TEXT("node"))
	{
		const auto* Node=Rules().Content().Nodes.Find(SelectedNode);
		V.RightTitle=Node?Node->DisplayName.ToString():TEXT("节点详情");V.RightText=TEXT("查看此地角色与建筑。返回沙盘后可选择卡组移动。");
		for(const auto& C:Rules().Run().Cards)if(C.Zone==TEXT("world")&&C.Node==SelectedNode)V.RightText+=TEXT("\n\n")+CardDescription(C.Id);
		if(SelectedNode==Rules().HomeNode())V.RightText+=TEXT("\n坐镇主角：")+Draft.HomeCharacter.ToString()+TEXT("\n核心：")+FString::SanitizeFloat(Rules().Run().PlayerCoreHP);
		V.Right.Add(Button(TEXT("查看地块场景"),TEXT("scene"),SelectedNode));
	}
	else if(HomeTab==TEXT("protagonist"))
	{
		V.RightTitle=TEXT("主角换装展示");V.RightText=TEXT("当前为外观预览；保存后生效。返回地图会放弃未保存预览。\n武器与姿态兼容性由配表校验。");
		for(const auto& P:D.HomeCharacters)V.Right.Add(Button(P.Value.DisplayName.ToString(),TEXT("draft"),P.Key,TEXT("home-character"),TEXT(""),true,Draft.HomeCharacter==P.Key));
		for(const auto& P:D.HomeOutfits)V.Right.Add(Button(P.Value.DisplayName.ToString(),TEXT("draft"),P.Key,TEXT("home-outfit"),TEXT(""),true,Draft.HomeOutfit==P.Key));
		for(const auto& P:D.HomeWeapons)V.Right.Add(Button(P.Value.DisplayName.ToString(),TEXT("draft"),P.Key,TEXT("home-weapon"),TEXT(""),true,Draft.HomeWeapon==P.Key));
		for(const auto& P:D.HomePoses)V.Right.Add(Button(P.Value.DisplayName.ToString(),TEXT("draft"),P.Key,TEXT("home-pose"),TEXT(""),true,Draft.HomePose==P.Key));
		V.Right.Add(Button(TEXT("保存展示配置"),TEXT("apply-preparation")));
	}
	else
	{
		V.RightTitle=TEXT("出战阵容");V.RightText=TEXT("ACT 使用此处保存的角色与武器。\n选择后点击「保存阵容配置」。");
		V.Left.Add(Button(TEXT("ACT"),TEXT("prep-mode"),TEXT("act"),NAME_None,TEXT(""),true,PreparationMode==TEXT("act")));
		V.Left.Add(Button(TEXT("FPS（配置预留）"),TEXT("prep-mode"),TEXT("fps"),NAME_None,TEXT(""),true,PreparationMode==TEXT("fps")));
		if(PreparationMode==TEXT("act"))
		{
			for(const auto& P:D.ACTCharacters){auto E=Button(P.Value.DisplayName.ToString(),TEXT("draft"),P.Key,TEXT("act-character"),P.Value.Description.ToString()+TEXT(" · 长按详情"),true,Draft.ACTCharacter==P.Key);E.bInspectable=true;E.InspectAction={TEXT("inspect-act-character"),P.Key};V.Right.Add(E);}
			for(const auto& P:D.ACTWeapons)V.Right.Add(Button(P.Value.DisplayName.ToString(),TEXT("draft"),P.Key,TEXT("act-weapon"),TEXT(""),true,Draft.ACTWeapon==P.Key));
			for(FName Id:D.SkillOrder)if(const auto* S=D.ACTSkills.Find(Id))V.Hand.Add(Button(S->DisplayName.ToString(),TEXT("none"),Id,NAME_None,S->Description.ToString(),false));
		}
		else
		{
			for(const auto& P:D.FPSCharacters)V.Right.Add(Button(P.Value.DisplayName.ToString(),TEXT("draft"),P.Key,TEXT("fps-character"),TEXT(""),true,Draft.FPSCharacter==P.Key));
			for(const auto& P:D.FPSWeapons)V.Right.Add(Button(P.Value.DisplayName.ToString(),TEXT("draft"),P.Key,TEXT("fps-weapon"),P.Value.Coverage.ToString(),true,Draft.FPSWeapon==P.Key));
		}
		V.Right.Add(Button(TEXT("保存阵容配置"),TEXT("apply-preparation")));
	}
}
void APadmaPlayerController::BuildCombatView(FPadmaGameView& V)
{
	if(!Combat)return;
	const bool ACT=Combat->GetMode()==EPadmaCombatMode::ACT;
	V.Heading+=ACT?TEXT(" · ACT"):TEXT(" · ENCOUNTER");
	V.LeftTitle=ACT?TEXT("战场单位"):TEXT("行动顺序");
	if(!ACT)
	{
		int32 i=0;for(FName Id:Combat->GetPredictedQueue(6))if(auto* U=Combat->GetUnit(Id))V.Left.Add(Button(FString::Printf(TEXT("%s %s"),i++==0?TEXT("▶ 当前"):TEXT("随后"),*U->Spec.DisplayName.ToString()),TEXT("target"),Id,NAME_None,TEXT(""),false,Id==Combat->GetActionOwner()));
	}
	for(const auto& U:Combat->GetSnapshots())
	{
		auto E=Button((U.bPlayer?TEXT("我方 · "):TEXT("敌方 · "))+U.DisplayName.ToString(),TEXT("target"),U.Id,NAME_None,FString::Printf(TEXT("生命 %.0f/%.0f · 长按查看详情"),U.Health,U.MaxHealth),true,U.Id==Combat->GetActionOwner()||AttackTargets.Contains(U.Id)||SkillTarget==U.Id);
		E.bInspectable=true;V.Left.Add(E);
	}
	const auto* Active=Combat->GetUnit(Combat->GetActionOwner());
	V.RightTitle=ACT?TEXT("ACT 操作"):Active?(Active->Spec.bPlayer?TEXT("轮到我方行动"):TEXT("敌方即将行动")):TEXT("战斗结束");
	V.RightText=ACT?TEXT("WASD / 方向键移动，点击场地寻点。\n空格普攻，Tab 开关基础技能库。\n卡库开启时世界 1/10 速度，禁止新移动／普攻。\n基础卡拖到目标，松手后确认。"):Active?Active->Spec.DisplayName.ToString()+FString::Printf(TEXT("\n逻辑时间 %.2f\n一次可选基础卡 + 普攻或防御。\n角色长按可查看敌我完整信息。"),Combat->GetActionTime()):TEXT("");
	if(!ACT)
	{
		V.Right.Add(Button(TEXT("角色普通攻击 → 选择目标"),TEXT("attack-arm"),NAME_None,NAME_None,FString::Printf(TEXT("目标数 %d"),AttackTargetCount),Active&&Active->Spec.bPlayer&&!bBattleFinished,bAttackArmed));
		if(Active&&Active->Spec.MaxTargets>1)V.Right.Add(Button(AttackTargetCount==1?TEXT("改选 2 个目标"):TEXT("改选 1 个目标"),TEXT("target-count")));
		V.Right.Add(Button(TEXT("防御 · 归墟 +2"),TEXT("guard"),NAME_None,NAME_None,TEXT("结束该单位行动"),Active&&Active->Spec.bPlayer&&!bBattleFinished));
	}
	V.Right.Add(Button(TEXT("Tab · 基础技能库"),TEXT("tab")));
	if(!ArmedSkill.IsNone())
	{
		V.RightText+=TEXT("\n\n选择卡牌：")+ArmedSkill.ToString()+TEXT("\n")+ (SkillTarget.IsNone()?TEXT("请选择合法作用单位／全场"):TEXT("目标已选，等待确认"));
		V.Right.Add(Button(TEXT("确认释放"),TEXT("skill-confirm"),NAME_None,NAME_None,TEXT("此时才扣费、弃牌和结算"),!SkillTarget.IsNone()));
	}
	if(!ArmedSkill.IsNone()||bAttackArmed)V.Right.Add(Button(TEXT("取消释放"),TEXT("cancel-target")));
	V.Right.Add(Button(TEXT("退出战斗并回滚"),TEXT("exit-battle"),NAME_None,NAME_None,TEXT("包含移动费用"),!bBattleFinished));
	V.bBlurWorld=ACT&&Combat->IsSkillLibraryOpen();
	int32 Slot=0;
	for(FName Id:Rules().Run().SkillSlots)
	{
		const FPadmaSkillEffectRowBase* S=ACT?static_cast<const FPadmaSkillEffectRowBase*>(Rules().Content().ACTSkills.Find(Id)):static_cast<const FPadmaSkillEffectRowBase*>(Rules().Content().EncounterSkills.Find(Id));if(!S)continue;
		const bool Enabled=Combat->IsBattleActive()&&Rules().Run().SkillsAvailable.Contains(Id)&&(ACT?Combat->IsSkillLibraryOpen():Active&&Active->Spec.bPlayer&&!Combat->IsCardUsed());
		auto E=Button(FString::Printf(TEXT("%d · %s"),++Slot,*S->DisplayName.ToString()),TEXT("skill"),Id,NAME_None,S->Description.ToString()+FString::Printf(TEXT("\n心流 %.0f · 计算力 %.0f"),S->FlowCost,S->CalculationCost),Enabled,ArmedSkill==Id);E.bDraggable=true;V.Hand.Add(E);
	}
}
bool APadmaPlayerController::HasBlockingOverlay() const
{
	return bTravelling||!OverlayContext().Key().IsNone()
		||(Session&&Rules().HasRun()&&(Rules().IsDialogue()||(!bBattleMap&&!Rules().Run().Outcome.IsNone())));
}
void APadmaPlayerController::NavigateTo(FName Destination)
{
	if(Destination==TEXT("map")||Page==TEXT("map"))Draft=Rules().Run().Preparation;
	Page=Destination;RestoreOverlay({});OverlayHistory.Reset();UpdateInspectionGate();
	if(Destination==TEXT("home")){SelectedNode=Rules().HomeNode();HomeTab=TEXT("node");}
	if(Destination==TEXT("settings"))HomeTab=TEXT("other");
	RefreshStage();bNeedsRefresh=true;
}
FName APadmaPlayerController::SelectedACard(FName Node) const
{
	for(FName Id:SelectedCards)if(const auto* Card=Rules().Card(Id))
		if(Card->Zone==TEXT("world")&&(Node.IsNone()||Card->Node==Node))
			if(const auto* Def=Rules().Content().Cards.Find(Card->DefId))if(Def->Family==TEXT("A"))return Id;
	return NAME_None;
}
void APadmaPlayerController::ReceiveIntent(const FPadmaUIAction& A)
{
	if(!Session||!Rules().HasRun()||bTravelling)return;
	const FName C=A.Command;
	if(C==TEXT("back")){EscapePressed();return;}
	if(!IsOverlayIntentAllowed(A))return;
	const auto Previous=OverlayContext();
	if(C==TEXT("none"))return;
	if(C==TEXT("map-seed")){MapSeedDraft=FCString::Atoi(*A.Id.ToString());return;}
	if(C==TEXT("close")){CloseOverlay();return;}
	else if(C==TEXT("menu")){ModalTitle=TEXT("菜单");ModalText=TEXT("进度与游戏选项");}
	else if(C==TEXT("mobility-menu")){ModalTitle=TEXT("试玩移动规则");ModalText=TEXT("高级试玩配置：点击切换可移动 / 不可移动 / 条件移动。每次修改立即生效。");}
	else if(C==TEXT("journal")){ModalTitle=TEXT("本局行记");ModalText.Reset();for(const auto& E:Rules().Run().Log)ModalText+=E.Text+TEXT("\n\n");}
	else if(C==TEXT("codex")){ModalTitle=TEXT("卡牌图鉴");ModalText.Reset();}
	else if(C==TEXT("card-details")||C==TEXT("inspect-card")){if(Rules().Card(A.Id)){RestoreOverlay({});DetailCard=A.Id;}}
	else if(C==TEXT("inspect-definition"))
	{
		const auto* Def=Rules().Content().Cards.Find(A.Id);
		const bool Known=Def&&(Def->Family!=TEXT("C")||Rules().Run().Cards.ContainsByPredicate([&](const auto& Card){return Card.DefId==A.Id&&(Card.Zone==TEXT("hand")||Card.Zone==TEXT("world"));})||Rules().Run().SynthesisHistory.ContainsByPredicate([&](const auto& S){return S.Selected==A.Id&&S.bSuccess;}));
		if(Known){RestoreOverlay({});DetailDefinition=A.Id;}
	}
	else if(C==TEXT("inspect-act-character")){if(Rules().Content().ACTCharacters.Contains(A.Id)){RestoreOverlay({});DetailACTCharacter=A.Id;}}
	else if(C==TEXT("hand-filter"))HandFilter=A.Id;
	else if(C==TEXT("return-world")){Travel(false);return;}
	else if(C==TEXT("new")){ModalTitle=TEXT("创建地图新局");ModalText=TEXT("固定 NPC 与主线人物的位置不会改变。种子只改变允许生成的支路和环境。开始后会替换当前未保存的新局进度。");}
	else if(C==TEXT("map-generate"))StartConfiguredMap(true);
	else if(C==TEXT("map-authored"))StartConfiguredMap(false);
	else if(C==TEXT("map-tutorial"))StartTutorialMap();
	else if(C==TEXT("map-zoom-in")){if(Screen)Screen->ZoomPaintedMap(1.12f);}
	else if(C==TEXT("map-zoom-out")){if(Screen)Screen->ZoomPaintedMap(1.f/1.12f);}
	else if(C==TEXT("map-fit"))bMapCameraReady=false; // Fit after the button releases its pointer capture.
	else if(C==TEXT("save")){ApplyResult(Session->SaveRun());if(ModalTitle==TEXT("菜单"))ModalTitle.Reset();}
	else if(C==TEXT("load")){ModalTitle=TEXT("读取存档");ModalText=TEXT("将用上次保存的进度替换当前局面。未保存的操作会丢失。");}
	else if(C==TEXT("load-confirm")){const auto Result=Session->LoadRun();ApplyResult(Result);ModalTitle.Reset();if(Result.bOk){SelectedCards.Reset();SelectedHand=NAME_None;SelectedNode=Rules().HomeNode();PreviewNode=SelectedNode;HoverNode=NAME_None;MapSeedDraft=Rules().Run().MapLayout.Seed;bMapCameraReady=false;NavigateTo(TEXT("map"));}}
	else if(C==TEXT("advance"))ApplyResult(Rules().Advance());
	else if(C==TEXT("select-node")){SelectMapTarget(A.Id);UpdateInspectionGate();return;}
	else if(C==TEXT("select-card"))
	{
		if(SelectedCards.Contains(A.Id))SelectedCards.Remove(A.Id);
		else if(const auto* Next=Rules().Card(A.Id))
		{
			if(!SelectedCards.IsEmpty())if(const auto* First=Rules().Card(SelectedCards[0]))if(First->Node!=Next->Node)SelectedCards.Reset();
			SelectedCards.Add(A.Id);
		}
		RefreshGraph();
	}
	else if(C==TEXT("clear-cards")){SelectedCards.Reset();RefreshGraph();}
	else if(C==TEXT("hand"))
	{
		const auto* Card=Rules().Card(A.Id);if(!Card||Card->Zone!=TEXT("hand"))return;
		SelectedHand=A.bDropped?A.Id:SelectedHand==A.Id?NAME_None:A.Id;
		if(A.bDropped)
		{
			const FName Node=Screen&&Screen->IsWorldPointerAvailable()?HitNode():NAME_None;
			if(!Node.IsNone())BeginPlacement(A.Id,Node);
			else Notice=TEXT("拖放已取消；选择地图地块后继续，尚未扣费。");
		}
	}
	else if(C==TEXT("mode"))BattleChoice=A.Id;
	else if(C==TEXT("move"))ApplyResult(Rules().Move(SelectedCards,A.Id,BattleChoice));
	else if(C==TEXT("deploy"))BeginPlacement(A.Id,A.Other);
	else if(C==TEXT("placement-confirm"))ConfirmPlacement();
	else if(C==TEXT("placement-attach"))ConfirmPlacement(A.Id);
	else if(C==TEXT("synthesis"))
	{
		if(!SelectedCards.IsEmpty())
		{
			SynthesisSource=A.Id.IsNone()?SelectedCards[0]:A.Id;
			FPadmaSynthesisRecord P;auto Result=Rules().PreviewSynthesis(SynthesisSource,P);ApplyResult(Result);
			if(Result.bOk)
			{
				ModalTitle=TEXT("合成预览");ModalText=FString::Printf(TEXT("费用：心流 %.0f，计算力 %.0f\nA、B 成败均弃置。成功 C 直接出现在当前节点。\n"),P.Flow,P.Calc);
				for(const auto& S:P.Candidates)ModalText+=FString::Printf(TEXT("\n%s：候选 %.1f%% · 成功 %.1f%%"),*Rules().Content().Cards.FindChecked(S.Id).DisplayName.ToString(),S.Probability*100,S.Success*100);
			}
		}
	}
	else if(C==TEXT("synthesis-confirm"))
	{
		ModalTitle.Reset();if(!SynthesisSource.IsNone()){auto Result=Rules().Synthesize(SynthesisSource);ApplyResult(Result);if(Result.bOk&&!Rules().Run().SynthesisHistory.IsEmpty()){const auto& P=Rules().Run().SynthesisHistory.Last();SelectedCards.Reset();if(!P.CreatedId.IsNone())SelectedCards.Add(P.CreatedId);}}SynthesisSource=NAME_None;
	}
	else if(C==TEXT("ability")||C==TEXT("ability-reverse"))ApplyResult(Rules().Ability(A.Id,C==TEXT("ability-reverse")));
	else if(C==TEXT("operation"))ApplyResult(Rules().Operation(A.Id));
	else if(C==TEXT("dialogue-next"))ApplyResult(Rules().DialogueNext(A.Id));
	else if(C==TEXT("dialogue-cancel"))ApplyResult(Rules().DialogueCancel());
	else if(C==TEXT("page"))NavigateTo(A.Id);
	else if(C==TEXT("home")){SelectedNode=Rules().HomeNode();NavigateTo(TEXT("home"));}
	else if(C==TEXT("settings"))NavigateTo(TEXT("settings"));
	else if(C==TEXT("scene")){SelectedNode=A.Id;NavigateTo(TEXT("scene"));}
	else if(C==TEXT("home-tab")){Page=A.Id==TEXT("other")?TEXT("settings"):TEXT("home");SelectedNode=Rules().HomeNode();HomeTab=A.Id;RefreshStage();}
	else if(C==TEXT("prep-mode")){PreparationMode=A.Id;RefreshStage();}
	else if(C==TEXT("draft"))
	{
		if(A.Other==TEXT("home-character"))Draft.HomeCharacter=A.Id;
		if(A.Other==TEXT("home-outfit"))Draft.HomeOutfit=A.Id;
		if(A.Other==TEXT("home-weapon"))Draft.HomeWeapon=A.Id;
		if(A.Other==TEXT("home-pose"))Draft.HomePose=A.Id;
		if(A.Other==TEXT("act-character"))Draft.ACTCharacter=A.Id;
		if(A.Other==TEXT("act-weapon"))Draft.ACTWeapon=A.Id;
		if(A.Other==TEXT("fps-character"))Draft.FPSCharacter=A.Id;
		if(A.Other==TEXT("fps-weapon"))Draft.FPSWeapon=A.Id;
		RefreshStage();
	}
	else if(C==TEXT("apply-preparation"))ApplyResult(Rules().ConfigurePreparation(Draft));
	else if(C==TEXT("mobility"))
	{
		if(const auto* Existing=Rules().Run().Mobility.Find(A.Id)){auto M=*Existing;M.Mode=M.Mode==TEXT("movable")?TEXT("immovable"):M.Mode==TEXT("immovable")?TEXT("conditional"):TEXT("movable");ApplyResult(Rules().ConfigureMobility(A.Id,M));}
	}
	else if(C==TEXT("inspect-unit")){if(Combat&&Combat->GetUnit(A.Id)){RestoreOverlay({});DetailUnit=A.Id;}}
	else if(C==TEXT("target"))SelectBattleTarget(A.Id);
	else if(C==TEXT("attack-arm"))
	{
		CancelTargeting();
		if(Combat)if(const auto* Actor=Combat->GetUnit(Combat->GetActionOwner());Actor&&Actor->Spec.bPlayer&&Actor->IsAlive())
		{
			int32 AliveEnemies=0;for(const auto& Unit:Combat->GetSnapshots())if(!Unit.bPlayer&&Unit.Health>0)++AliveEnemies;
			AttackTargetCount=FMath::Clamp(AttackTargetCount,1,FMath::Max(1,FMath::Min(Actor->Spec.MaxTargets,AliveEnemies)));
			bAttackArmed=AliveEnemies>0;Notice=TEXT("点击敌方目标立即攻击；取消不扣费。");
		}
	}
	else if(C==TEXT("target-count")){AttackTargetCount=AttackTargetCount==1?2:1;CancelTargeting();}
	else if(C==TEXT("guard")){FString Error;if(Combat&&!Combat->Guard(Error))Notice=Error;CancelTargeting();}
	else if(C==TEXT("tab"))TabPressed();
	else if(C==TEXT("cancel-target"))CancelTargeting();
	else if(C==TEXT("skill"))
	{
		if(Combat&&Combat->IsBattleActive())
		{
			CancelTargeting();ArmedSkill=A.Id;
			if(A.bDropped)
			{
				if(HasBlockingOverlay()||!Screen||!Screen->IsWorldPointerAvailable()){CancelTargeting();Notice=TEXT("已取消拖放，未扣费。");bNeedsRefresh=true;return;}
				SelectBattleTarget(HitUnit());
			}
			Notice=SkillTarget.IsNone()?TEXT("卡牌已选：点击目标／场地，再确认释放。"):TEXT("拖放目标已预览：点击确认释放。");
		}
	}
	else if(C==TEXT("skill-confirm"))
	{
		FString Error;if(Combat&&!Combat->UseSkill(ArmedSkill,SkillTarget==TEXT("arena")?NAME_None:SkillTarget,Error))Notice=Error;CancelTargeting();
	}
	else if(C==TEXT("exit-battle")){ModalTitle=TEXT("撤退确认");ModalText=TEXT("退出本场战斗，并恢复出发前的完整状态，包括移动费用。确定撤退？");}
	else if(C==TEXT("exit-confirm")){ModalTitle.Reset();if(Combat)Combat->ExitBattle();}
	const auto Next=OverlayContext();
	if(!Previous.Key().IsNone()&&!Next.Key().IsNone()&&Previous.Key()!=Next.Key())OverlayHistory.Add(Previous);
	if(Next.Key().IsNone())OverlayHistory.Reset();
	UpdateInspectionGate();bNeedsRefresh=true;
}
