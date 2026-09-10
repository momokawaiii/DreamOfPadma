#include "Core/Run/PadmaRunRules.h"

FName FPadmaRunRules::DialogueForNode(FName Node) const
{
    for (const auto& Checkpoint : Catalog.MapLayout.Checkpoints)
    {
        if (Checkpoint.NodeId != Node) continue;
        if (const bool* Matched = State.StoryCheckpoints.Find(Checkpoint.Id))
        {
            const FName Branch = *Matched ? Checkpoint.MatchedDialogueId : Checkpoint.MissedDialogueId;
            if (!Branch.IsNone()) return Branch;
        }
    }
    const auto* Definition = Catalog.Nodes.Find(Node);
    return Definition && !Definition->DialogueId.IsNone() ? Definition->DialogueId : Node;
}

void FPadmaRunRules::ResolveStoryCheckpoint(FName Node)
{
    for (const auto& Checkpoint : Catalog.MapLayout.Checkpoints)
    {
        if (Checkpoint.NodeId != Node || State.StoryCheckpoints.Contains(Checkpoint.Id)) continue;
        const bool Matched = (Checkpoint.bRequireCompletion ? State.CompletedNPCs : State.SeenNPCs).Contains(Checkpoint.NPCId);
        State.StoryCheckpoints.Add(Checkpoint.Id, Matched);
        State.Unlocks.AddUnique(Matched ? Checkpoint.MatchedFlag : Checkpoint.MissedFlag);
        Emit(TEXT("剧情检查点：") + Checkpoint.Id.ToString() + (Matched ? TEXT(" · 人物前置已满足") : TEXT(" · 进入未相遇／未完成分支")), TEXT("note"));
    }
}

bool FPadmaRunRules::BeginNodeDialogue(FName Node)
{
    const auto* Definition = Catalog.Nodes.Find(Node);
    const auto* Runtime = State.Nodes.Find(Node);
    if (!Definition || !Runtime || Runtime->bUsed || Runtime->Owner != TEXT("player") || IsDialogue()) return false;
    if (!Definition->NPCId.IsNone()) State.SeenNPCs.AddUnique(Definition->NPCId);
    // Arrival records an encounter at every phase; node operations still require noon.
    if (State.Calendar.Phase != 1) return false;
    const FName Id = DialogueForNode(Node);
    for (const auto& Pair : Catalog.Dialogues)
    {
        if (Pair.Value.DialogueId == Id && Pair.Value.bStart)
        {
            DialogueState.Node = Node;
            DialogueState.Line = Pair.Value.LineId;
            DialogueState.DialogueId = Id;
            return true;
        }
    }
    return false;
}

FPadmaRunCommandResult FPadmaRunRules::ValidateStoryState(const FPadmaRunState& R) const
{
    auto Fail = [](const FString& Message) { return FPadmaRunCommandResult{false, Message, EPadmaRunCommandEffect::None}; };
    TSet<FName> NPCs;
    TSet<FName> ExpectedFlags;
    for (const auto& Pair : Catalog.Nodes)
    {
        const auto& Def = Pair.Value;
        if (!Def.NPCId.IsNone()) NPCs.Add(Def.NPCId);
        const auto* Node = R.Nodes.Find(Pair.Key);
        if (!Node || !Node->bUsed) continue;
        if (!Def.CompletionFlag.IsNone()) ExpectedFlags.Add(Def.CompletionFlag);
        else if (Def.Type == TEXT("story")) ExpectedFlags.Add(TEXT("story-road"));
        if (!Def.NPCId.IsNone() && !R.CompletedNPCs.Contains(Def.NPCId)) return Fail(TEXT("存档人物完成记录缺失"));
    }
    TSet<FName> Seen;
    for (FName NPC : R.SeenNPCs)
    {
        if (!NPCs.Contains(NPC) || Seen.Contains(NPC)) return Fail(TEXT("存档人物相遇记录无效"));
        Seen.Add(NPC);
    }
    TSet<FName> Completed;
    for (FName NPC : R.CompletedNPCs)
    {
        if (!Seen.Contains(NPC) || Completed.Contains(NPC)) return Fail(TEXT("存档人物完成记录无效"));
        Completed.Add(NPC);
    }
    for (const auto& Pair : R.StoryCheckpoints)
    {
        const auto* Definition = Catalog.MapLayout.Checkpoints.FindByPredicate([&](const auto& V) { return V.Id == Pair.Key; });
        if (!Definition || !R.Nodes.Contains(Definition->NodeId) || !R.Nodes.FindChecked(Definition->NodeId).bVisited)
            return Fail(TEXT("存档剧情检查点无效"));
        if (Pair.Value && !(Definition->bRequireCompletion ? Completed : Seen).Contains(Definition->NPCId))
            return Fail(TEXT("存档剧情前置记录缺失"));
        ExpectedFlags.Add(Pair.Value ? Definition->MatchedFlag : Definition->MissedFlag);
    }
    for (const auto& Definition : Catalog.MapLayout.Checkpoints)
    {
        if (R.Nodes.FindChecked(Definition.NodeId).bVisited && !R.StoryCheckpoints.Contains(Definition.Id))
            return Fail(TEXT("已到达的剧情检查点缺少分支记录"));
    }
    TSet<FName> Flags;
    for (FName Flag : R.Unlocks)
    {
        if (!ExpectedFlags.Contains(Flag) || Flags.Contains(Flag)) return Fail(TEXT("存档剧情／道路标记无效"));
        Flags.Add(Flag);
    }
    if (Flags.Num() != ExpectedFlags.Num()) return Fail(TEXT("存档剧情／道路标记遗漏"));
    if (Catalog.MapLayout.MapId.IsNone() || Catalog.MapLayout.MapId == TEXT("legacy-html-v1"))
    {
        const auto* Story = R.Nodes.Find(TEXT("story"));
        if (!Story || Story->bUsed != R.bStoryMet || R.bStoryMet != Flags.Contains(TEXT("story-road")))
            return Fail(TEXT("旧存档剧情结算不一致"));
    }
    return {true, TEXT("剧情状态有效"), EPadmaRunCommandEffect::None};
}
