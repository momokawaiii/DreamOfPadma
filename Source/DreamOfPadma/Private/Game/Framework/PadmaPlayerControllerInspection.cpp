#include "Game/Framework/PadmaPlayerController.h"
#include "Game/Content/PadmaContentCatalog.h"
#include "Game/Run/PadmaRunSubsystem.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "UI/Screens/PadmaCardArt.h"
#include "Engine/Texture2D.h"

FPadmaOverlayContext APadmaPlayerController::OverlayContext()const
{
 return {ModalTitle,ModalText,DetailCard,DetailUnit,DetailDefinition,DetailACTCharacter};
}
void APadmaPlayerController::RestoreOverlay(const FPadmaOverlayContext& C)
{
 ClearPlacement();
 ModalTitle=C.Title;ModalText=C.Text;DetailCard=C.Card;DetailUnit=C.Unit;DetailDefinition=C.Definition;DetailACTCharacter=C.ACTCharacter;
}
void APadmaPlayerController::CloseOverlay()
{
 if(bBattleFinished)return;
 RestoreOverlay(OverlayHistory.IsEmpty()?FPadmaOverlayContext():OverlayHistory.Pop());
 UpdateInspectionGate();bMouseHeld=false;bNeedsRefresh=true;
}
void APadmaPlayerController::UpdateInspectionGate()
{
 if(Combat)Combat->SetInspectionOpen(OverlayContext().HasDetails()||OverlayHistory.ContainsByPredicate([](const auto& C){return C.HasDetails();}));
}
bool APadmaPlayerController::IsOverlayIntentAllowed(const FPadmaUIAction& A)const
{
 if(!HasBlockingOverlay())return true;
 const FName C=A.Command;
 if(C==TEXT("back")||C==TEXT("close"))return true;
 if(Rules().IsDialogue())return C==TEXT("dialogue-next")||C==TEXT("dialogue-cancel");
 if(OverlayContext().HasDetails())return false;
 if(bBattleFinished)return C==TEXT("return-world");
 if(!Rules().Run().Outcome.IsNone())return C==TEXT("save")||C==TEXT("new")||C==TEXT("map-seed")||C==TEXT("map-generate")||C==TEXT("map-authored")||C==TEXT("map-tutorial");
 if(ModalTitle==TEXT("菜单"))return C==TEXT("save")||C==TEXT("load")||C==TEXT("new")||C==TEXT("journal")||C==TEXT("codex")||C==TEXT("mobility-menu");
 if(ModalTitle==TEXT("卡牌图鉴"))return C==TEXT("inspect-definition");
 if(ModalTitle==TEXT("读取存档"))return C==TEXT("load-confirm");
 if(ModalTitle==TEXT("撤退确认"))return C==TEXT("exit-confirm");
 if(ModalTitle==TEXT("合成预览"))return C==TEXT("synthesis-confirm");
 if(IsPlacementOverlay())return PendingPlacementFamily==TEXT("A")?C==TEXT("placement-confirm"):C==TEXT("placement-attach");
 if(ModalTitle==TEXT("试玩移动规则"))return C==TEXT("mobility");
 if(ModalTitle==TEXT("创建地图新局"))return C==TEXT("map-seed")||C==TEXT("map-generate")||C==TEXT("map-authored")||C==TEXT("map-tutorial");
 return false;
}
void APadmaPlayerController::BuildInspectionView(FPadmaGameView& V)const
{
 if(!OverlayContext().HasDetails())return;
 const auto& Data=Rules().Content();
 const FPadmaCardInstance* Card=DetailCard.IsNone()?nullptr:Rules().Card(DetailCard);
 const APadmaCombatUnit* Unit=Combat&&!DetailUnit.IsNone()?Combat->GetUnit(DetailUnit):nullptr;
 const FName Definition=Card?Card->DefId:Unit?Unit->Spec.DefinitionId:!DetailDefinition.IsNone()?DetailDefinition:DetailACTCharacter;
 const auto* Def=Data.Cards.Find(Definition);const auto* CharacterRow=Data.ACTCharacters.Find(Definition);
 V.ModalCards.Reset();V.ModalText.Reset();V.ModalActions={Button(TEXT("返回  ·  Esc"),TEXT("close"))};
 V.ModalTitle=Def?Def->DisplayName.ToString():CharacterRow?CharacterRow->DisplayName.ToString():Unit?Unit->Spec.DisplayName.ToString():TEXT("详情不可用");
 const FName Model=Def?Def->ModelId:CharacterRow?CharacterRow->ModelId:FName(TEXT("guard"));
 if(Catalog)if(const auto* Binding=Catalog->FindModelBinding(Model))V.ModalPortrait=Binding->Portrait.LoadSynchronous();
 V.ModalArtIndex=PadmaCardArtIndex(Definition);
 V.PortraitCaption=V.ModalPortrait?TEXT("角色立绘"):V.ModalArtIndex!=INDEX_NONE?TEXT("卡牌插画 · 独立角色立绘尚未配置"):TEXT("独立角色立绘尚未配置");
 V.ModalSubtitle=(Unit?FString(Unit->Spec.bPlayer?TEXT("我方角色  /  "):TEXT("敌方角色  /  ")):Card?FString(TEXT("卡牌实例  /  ")):FString(TEXT("定义资料  /  ")))
  +(Def?Def->Family.ToString()+TEXT(" 类  /  "):FString())+TEXT("等级未配置");
 if(Unit)
 {
  const auto S=Unit->Snapshot();
  V.DetailSections.Add({TEXT("当前属性"),FString::Printf(TEXT("生命  %.1f / %.1f\n攻击  %.1f     防御  %.1f\n归墟  %.1f     抗衡  %.0f\n状态  %s"),S.Health,S.MaxHealth,S.Attack,S.Defense,S.Shield,S.Block,S.Health<=0?TEXT("已倒下"):TEXT("存活"))});
  if(Combat->GetMode()==EPadmaCombatMode::Encounter)
  {
   V.DetailSections.Add({TEXT("行动与普通攻击"),FString::Printf(TEXT("行动速度  %.1f     就绪时间  %.2f\n普攻费用  %.0f     目标上限  %d\n本轮%s行动。选择目标后才提交攻击。"),Unit->Spec.Speed,S.ReadyTime,Unit->Spec.AttackCost,Unit->Spec.MaxTargets,S.bActed?TEXT("已"):TEXT("未"))});
   V.ModalSubtitle+=TEXT("  /  敌方自动行动等待中");
  }
  else
  {
   V.DetailSections.Add({TEXT("动作与普通攻击"),FString::Printf(TEXT("速度  %.1f     攻击距离  %.0f\n攻击间隔  %.2f 秒     当前前摇  %.2f 秒\n普通攻击按当前武器配置执行。"),Unit->Spec.Speed,Unit->Spec.AttackRange,Unit->Spec.AttackInterval,S.WindupRemaining)});
   V.ModalSubtitle+=TEXT("  /  战场计时继续");
  }
 }
 else if(Card&&Def)
 {
  FString Stats=FString::Printf(TEXT("生命  %.0f / %d     攻击  %.0f     防御  %d\n部署费用  %.0f 心流"),Card->HP,Def->Health,Card->Attack,Def->Defense,Def->Family==TEXT("C")?Rules().P(TEXT("deployC")):double(Def->Cost));
  if(Def->Family!=TEXT("C"))Stats+=FString::Printf(TEXT("     稳定性  %d     亲和性  %d"),Def->Stability,Def->Affinity);
  V.DetailSections.Add({TEXT("当前属性"),Stats});
  FString Place=Card->Zone==TEXT("hand")?TEXT("手牌"):Card->Zone==TEXT("world")?TEXT("已部署"):Card->Zone==TEXT("discard")?TEXT("弃牌堆"):TEXT("牌库");
  if(const auto* Node=Data.Nodes.Find(Card->Node))Place+=TEXT(" · ")+Node->DisplayName.ToString();
  if(const auto* A=Rules().Card(Card->AttachedTo))if(const auto* AD=Data.Cards.Find(A->DefId))Place+=TEXT("\n附着于：")+AD->DisplayName.ToString();
  if(!Card->ActiveEra.IsEmpty())Place+=TEXT("\n生效时代：")+Card->ActiveEra;
  V.DetailSections.Add({TEXT("位置与状态"),Place});
 }
 else if(Def)
 {
  FString Stats=FString::Printf(TEXT("最大生命  %d     基础攻击  %d     防御  %d\n部署费用  %.0f 心流"),Def->Health,Def->Attack,Def->Defense,Def->Family==TEXT("C")?Rules().P(TEXT("deployC")):double(Def->Cost));
  if(Def->Family!=TEXT("C"))Stats+=FString::Printf(TEXT("     稳定性  %d     亲和性  %d"),Def->Stability,Def->Affinity);
  V.DetailSections.Add({TEXT("定义属性 · 无当前实例"),Stats});
 }
 else if(CharacterRow)
 {
  V.DetailSections.Add({TEXT("备战配置预览"),FString::Printf(TEXT("保存的 ACT 生命  %.0f\n%s\n攻防与武器效果在进入战斗时组装；当前没有战斗实例。"),Rules().Run().Preparation.ACTHP,*CharacterRow->Description.ToString())});
 }
 if(Def)
 {
  FString Ability;
  if(Def->Family==TEXT("C")&&!Unit)Ability=FString::Printf(TEXT("普通攻击：费用 %d，最多 %d 个目标。进入遭遇战后使用。\n"),Def->AttackCost,Def->MaxTargets);
  if(Definition==TEXT("vitruvian"))Ability+=TEXT("完美肉体：晌午消耗 1 心流，所有己方已部署非核心 C 卡恢复 1 生命，不超过上限。\n");
  if(Definition==TEXT("banquet"))Ability+=TEXT("希律一诺：晌午以 4 心流兑换 4 计算力，或反向兑换；每时代一次。每三日恢复归处核心 1 点生命。\n");
  if(Def->DailyFlow)Ability+=FString::Printf(TEXT("每日心流 +%d%s\n"),Def->DailyFlow,Def->DailyLimit?*FString::Printf(TEXT("，持续 %d 日"),Def->DailyLimit):TEXT(""));
  if(Ability.IsEmpty())Ability=TEXT("本版未配置额外角色主动技能。\n");
  V.DetailSections.Add({TEXT("已接入能力"),Ability});
  V.DetailSections.Add({TEXT("能力说明与实现范围"),Def->Coverage.ToString()});
  if(Def->Family==TEXT("A"))V.DetailSections.Add({TEXT("合成倾向"),FString::Printf(TEXT("倾向  [%.0f, %.0f]\n附着 B 卡后可预览合成。"),Def->Tendency.X,Def->Tendency.Y)});
  if(Def->Family==TEXT("B")){const auto Field=Card?Card->Field:Def->Field;V.DetailSections.Add({TEXT("场域"),FString::Printf(TEXT("当前场域  [%.0f, %.0f]\n部署需要同节点的 A 卡，随附着对象同行。"),Field.X,Field.Y)});}
  if(const auto* Mobility=Rules().Run().Mobility.Find(Definition))
  {
   FString Movement=Mobility->Mode==TEXT("movable")?TEXT("可移动"):Mobility->Mode==TEXT("immovable")?TEXT("不可移动"):FString::Printf(TEXT("条件移动：信仰 ≥ %.0f"),Mobility->MinFaith);
   if(!Mobility->ForbiddenTerrain.IsEmpty())Movement+=TEXT("\n禁止地形：")+Mobility->ForbiddenTerrain;
   V.DetailSections.Add({TEXT("移动限制"),Movement});
  }
 }
 if(CharacterRow)
 {
  FString Description=CharacterRow->Description.ToString();
  if(!CharacterRow->SpeedBonusTerrain.IsNone())Description+=FString::Printf(TEXT("\n%s速度倍率 %.2f"),*CharacterRow->SpeedBonusTerrain.ToString(),CharacterRow->TerrainSpeedMultiplier);
  if(!CharacterRow->TraitDisabledTerrain.IsNone())Description+=TEXT("\n特性失效地形：")+CharacterRow->TraitDisabledTerrain.ToString();
  V.DetailSections.Add({TEXT("角色特性"),Description});
 }
 const bool ACT=Unit?Combat->GetMode()==EPadmaCombatMode::ACT:CharacterRow!=nullptr;
 FString Shared=TEXT("以下属于玩家共享基础卡库，与角色自身能力分开。\n");
 for(FName Id:Rules().Run().SkillSlots)
 {
  const FPadmaSkillEffectRowBase* Skill=ACT?static_cast<const FPadmaSkillEffectRowBase*>(Data.ACTSkills.Find(Id)):static_cast<const FPadmaSkillEffectRowBase*>(Data.EncounterSkills.Find(Id));if(!Skill)continue;
  const FString Target=Skill->Target==TEXT("enemy")?TEXT("敌方"):Skill->Target==TEXT("ally")?TEXT("我方"):Skill->Target==TEXT("all")?TEXT("全场"):Skill->Target.ToString();
  Shared+=FString::Printf(TEXT("\n%s  ·  心流 %.0f / 计算力 %.0f\n目标：%s\n%s\n"),*Skill->DisplayName.ToString(),Skill->FlowCost,Skill->CalculationCost,*Target,*Skill->Description.ToString());
 }
 V.DetailSections.Add({ACT?TEXT("玩家共享技能库 · ACT"):TEXT("玩家共享技能库 · Encounter"),Shared});
 V.DetailSections.Add({TEXT("等级与成长"),TEXT("等级、经验和升级成长尚未配置。")});
}
