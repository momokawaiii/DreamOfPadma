#include "Core/Run/PadmaRunRules.h"
#include "Math/RandomStream.h"

FPadmaRunCommandResult FPadmaRunRules::PreviewWildernessOccupation(FName Source, FName Node, FName Faction) const
{
    auto Reject=[](const TCHAR* Reason) { return FPadmaRunCommandResult{false,Reason,EPadmaRunCommandEffect::None}; };
    if (!Catalog.MapLayout.bHexWilderness || !bHasRun) return Reject(TEXT("当前不是教程荒野地图"));
    if (IsBattle() || IsDialogue() || !State.Outcome.IsNone()) return Reject(TEXT("当前世界状态不能扩张领地"));
    if (Faction!=TEXT("player") && Faction!=TEXT("ruler")) return Reject(TEXT("未知占领阵营"));
    const auto* From=Catalog.Nodes.Find(Source); const auto* To=Catalog.Nodes.Find(Node);
    const auto* FromState=State.Nodes.Find(Source); const auto* ToState=State.Nodes.Find(Node);
    if (!From || !To || !FromState || !ToState || FromState->Owner!=Faction) return Reject(TEXT("扩张来源不属于此阵营"));
    if (!From->bHabitable || !From->bTraversable || !To->bHabitable || !To->bTraversable || !To->bWilderness)
        return Reject(TEXT("只允许从可居住领地扩张到可通行荒野"));
    const auto* Edge=Catalog.Edges.FindByPredicate([&](const auto& E) { return E.From==Source && E.To==Node; });
    if (!Edge || (!Edge->UnlockFlag.IsNone() && !State.Unlocks.Contains(Edge->UnlockFlag))) return Reject(TEXT("没有已开放的明确相邻连接"));
    if (To->EnemyCount!=0 || (Faction==TEXT("ruler") && ToState->Owner!=TEXT("neutral"))
        || (Faction==TEXT("player") && ToState->Owner!=TEXT("neutral") && ToState->Owner!=TEXT("ruler")))
        return Reject(TEXT("目标不是此阵营可直接占领的无守军荒野"));
    if (State.Cards.ContainsByPredicate([Node](const auto& Card) { return Card.Zone==TEXT("world") && Card.Node==Node; }))
        return Reject(TEXT("目标荒野存在驻扎卡牌，不能直接覆盖占领"));
    return {true,TEXT("荒野占领条件满足"),EPadmaRunCommandEffect::None};
}

void FPadmaRunRules::ApplyWildernessOccupation(FName Node, FName Faction)
{
    const auto* Definition=Catalog.Nodes.Find(Node); auto* Runtime=State.Nodes.Find(Node);
    if (!Catalog.MapLayout.bHexWilderness || !Definition || !Runtime || !Definition->bWilderness
        || !Definition->bHabitable || !Definition->bTraversable || (Faction!=TEXT("player") && Faction!=TEXT("ruler"))) return;
    if (Faction==TEXT("player")) Runtime->bVisited=true;
    if (Runtime->Owner==Faction) return;
    Runtime->Owner=Faction;
    // Frontier ownership is persistent value state, with no strategic-war/core/resource reward.
    // Player entry reached this point through validated movement or a committed actual battle.
    Emit((Faction==TEXT("player")?FString(TEXT("我方占领荒野：")):FString(TEXT("王庭扩张荒野：")))+Definition->DisplayName.ToString(),
        Faction==TEXT("player")?FName(TEXT("success")):FName(TEXT("note")));
}

void FPadmaRunRules::AdvanceWildernessFrontier()
{
    if (!Catalog.MapLayout.bHexWilderness || State.Calendar.Phase!=2 || IsBattle() || IsDialogue() || !State.Outcome.IsNone()) return;
    TArray<FName> Candidates;
    for (const auto& Edge:Catalog.Edges)
        if (PreviewWildernessOccupation(Edge.From,Edge.To,TEXT("ruler")).bOk) Candidates.AddUnique(Edge.To);
    if (Candidates.IsEmpty()) return;
    Candidates.Sort([](FName A,FName B) { return A.LexicalLess(B); });
    // Separate deterministic decision stream; no topology, synthesis or combat random draws consumed.
    FRandomStream Decision(int32(uint32(State.MapLayout.Seed)^uint32(State.Calendar.AbsoluteDay)*2654435761u^0x51A17E31u));
    ApplyWildernessOccupation(Candidates[Decision.RandRange(0,Candidates.Num()-1)],TEXT("ruler"));
}
