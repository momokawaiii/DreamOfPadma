#include "Core/Run/PadmaRunRules.h"
#include "Core/WorldMap/PadmaWorldMapDefinition.h"
#include "Core/WorldMap/PadmaTutorialMap.h"

namespace
{
FPadmaRunCommandResult Ok(const FString& Text, EPadmaRunCommandEffect Effect = EPadmaRunCommandEffect::WorldChanged)
{
    return {true, Text, Effect};
}
FPadmaRunCommandResult Fail(const FString& Text) { return {false, Text, EPadmaRunCommandEffect::None}; }
bool NonNegative(double Value) { return FMath::IsFinite(Value) && Value >= 0; }
const FPadmaDialogueLineRow* FindLine(const FPadmaContentSnapshot& Catalog, FName Node, FName Line)
{
    for (const auto& Pair : Catalog.Dialogues)
        if (Pair.Value.DialogueId == Node && Pair.Value.LineId == Line) return &Pair.Value;
    return nullptr;
}
FPadmaRunCommandResult CheckProfile(const FPadmaContentSnapshot& Catalog, const TMap<FName, double>& Profile)
{
    if (Profile.Num() != Catalog.Profiles.Num()) return Fail(TEXT("参数表字段不兼容"));
    for (const auto& Pair : Catalog.Profiles)
    {
        const double* Value = Profile.Find(Pair.Key);
        if (!Value || !FMath::IsFinite(*Value) || *Value < Pair.Value.Minimum || *Value > Pair.Value.Maximum)
            return Fail(TEXT("参数超出范围：") + Pair.Key.ToString());
        if ((Pair.Key == TEXT("seed") || Pair.Key == TEXT("dailyDraw")) && FMath::FloorToDouble(*Value) != *Value)
            return Fail(TEXT("随机种子与每日抽牌数必须为整数"));
    }
    if (Profile.FindRef(TEXT("flowStart")) > Profile.FindRef(TEXT("flowCap")) || Profile.FindRef(TEXT("calcStart")) > Profile.FindRef(TEXT("calcCap")))
        return Fail(TEXT("初始资源不能超过上限"));
    return Ok(TEXT("参数有效"));
}
}

void FPadmaRunRules::Initialize(const FPadmaContentSnapshot& InContent, const FPadmaMapLayout* AuthoredReference)
{
    Catalog = InContent;
    ConfiguredMapLayout = AuthoredReference ? *AuthoredReference : InContent.MapLayout;
    State = {};
    Battle = {};
    BattleSnapshot.Reset();
    DialogueState = {};
    bHasRun = false;
    ++StateRevision;
}

FPadmaRunCommandResult FPadmaRunRules::Atomic(TFunctionRef<FPadmaRunCommandResult()> Command)
{
    const FPadmaRunState Before = State;
    const FPadmaBattleRequest BeforeBattle = Battle;
    const TOptional<FPadmaRunState> BeforeSnapshot = BattleSnapshot;
    const FPadmaDialogueState BeforeDialogue = DialogueState;
    const bool bBeforeHasRun = bHasRun;
    bCounterLimitReached = false;
    FPadmaRunCommandResult Result = Command();
    if (bCounterLimitReached) Result = Fail(TEXT("本局计数已达支持上限；操作未提交"));
    if (!Result.bOk)
    {
        State = Before;
        Battle = BeforeBattle;
        BattleSnapshot = BeforeSnapshot;
        DialogueState = BeforeDialogue;
        bHasRun = bBeforeHasRun;
    }
    LastNotice = Result.Text;
    ++StateRevision;
    return Result;
}

FPadmaRunCommandResult FPadmaRunRules::Start(const TMap<FName, double>& Overrides)
{
    return Atomic([&]()
    {
        if (IsDialogue() || IsBattle()) return Fail(TEXT("请先结束对话或退出战斗"));
        FString Error;
        if (!Catalog.Validate(Error)) return Fail(TEXT("内容表未就绪：") + Error);
        for (FName Id : {FName(TEXT("dawn")), FName(TEXT("earth")), FName(TEXT("banquet")), FName(TEXT("cherry")), FName(TEXT("reed")), FName(TEXT("cleopatra")), FName(TEXT("vitruvian")), FName(TEXT("dancer")), FName(TEXT("door"))})
            if (!Catalog.Cards.Contains(Id)) return Fail(TEXT("开局卡牌定义缺失：") + Id.ToString());
        TMap<FName, double> Profile;
        for (const auto& Pair : Catalog.Profiles) Profile.Add(Pair.Key, Pair.Value.Value);
        for (const auto& Pair : Overrides) Profile.Add(Pair.Key, Pair.Value);
        auto Checked = CheckProfile(Catalog, Profile);
        if (!Checked.bOk) return Checked;
        State = {};
        State.MapLayout = Catalog.MapLayout;
        State.Profile = MoveTemp(Profile);
        State.RNG = static_cast<uint32>(P(TEXT("seed")));
        State.Resources.Flow = P(TEXT("flowStart"));
        State.Resources.Calc = P(TEXT("calcStart"));
        State.Resources.Faith = P(TEXT("faith"));
        State.Resources.Entropy = P(TEXT("entropy"));
        State.RulerFaith = State.Resources.Faith;
        State.PlayerCoreHP = State.RulerCoreHP = P(TEXT("coreHP"));
        State.Preparation.ACTHP = P(TEXT("actHP"));
        Checked = ValidatePreparation(State.Preparation);
        if (!Checked.bOk) return Checked;
        State.SkillSlots = State.SkillsAvailable = Catalog.SkillOrder;
        for (const auto& Pair : Catalog.Nodes)
        {
            FPadmaWorldNodeState Node;
            Node.Owner = Pair.Value.InitialOwner;
            Node.bVisited = Pair.Key == HomeNode();
            State.Nodes.Add(Pair.Key, Node);
        }
        for (const auto& Pair : Catalog.Cards)
        {
            FPadmaCardMobilityPolicy Policy;
            const auto& D = Pair.Value;
            Policy.Mode = !D.Mobility.bConfigured ? FName(TEXT("unconfigured")) :
                D.Mobility.Policy == EPadmaCardMobility::Immovable ? FName(TEXT("immovable")) :
                D.Mobility.Policy == EPadmaCardMobility::Conditional ? FName(TEXT("conditional")) : FName(TEXT("movable"));
            Policy.MinFaith = D.MinFaith;
            Policy.ForbiddenTerrain = D.ForbiddenTerrain.IsNone() ? FString() : D.ForbiddenTerrain.ToString();
            State.Mobility.Add(Pair.Key, Policy);
        }
        // Exact accepted HTML opening/deck order. No C-card grant by preparation selection.
        for (FName Id : {FName(TEXT("dawn")), FName(TEXT("cherry")), FName(TEXT("dancer")), FName(TEXT("earth")), FName(TEXT("reed")), FName(TEXT("banquet")), FName(TEXT("cleopatra"))})
            CreateCard(Id, TEXT("hand"));
        for (FName Id : {FName(TEXT("dawn")), FName(TEXT("cherry")), FName(TEXT("earth")), FName(TEXT("reed")), FName(TEXT("banquet")), FName(TEXT("cleopatra")), FName(TEXT("dawn")), FName(TEXT("cherry")), FName(TEXT("dancer")), FName(TEXT("earth")), FName(TEXT("reed")), FName(TEXT("banquet")), FName(TEXT("cleopatra")), FName(TEXT("vitruvian"))})
            CreateCard(Id, TEXT("deck"));
        bHasRun = true;
        ResolveStoryCheckpoint(HomeNode());
        Emit(TEXT("新局开始：UE Demo 临时试玩参数；第 1 天黎明。进入晌午后部署 A、附着 B。"));
        return Ok(TEXT("试玩已开始"));
    });
}

const FPadmaCardInstance* FPadmaRunRules::Card(FName Id) const
{
    return State.Cards.FindByPredicate([Id](const FPadmaCardInstance& Value) { return Value.Id == Id; });
}
FPadmaCardInstance* FPadmaRunRules::MutableCard(FName Id)
{
    return State.Cards.FindByPredicate([Id](const FPadmaCardInstance& Value) { return Value.Id == Id; });
}
double FPadmaRunRules::P(FName Key) const
{
    const double* Value = State.Profile.Find(Key);
    return Value ? *Value : Catalog.P(Key);
}
void FPadmaRunRules::Emit(const FString& Text, FName Kind)
{
    if (State.Sequence >= MAX_int32 - 1) { bCounterLimitReached = true; return; }
    FPadmaRunLogEntry Entry;
    Entry.Id = ++State.Sequence;
    Entry.Text = Text;
    Entry.Kind = Kind;
    State.Log.Add(MoveTemp(Entry));
    if (State.Log.Num() > 120) State.Log.RemoveAt(0);
}
double FPadmaRunRules::Random()
{
    State.RNG = State.RNG * 1664525u + 1013904223u;
    return static_cast<double>(State.RNG) / 4294967296.0;
}
FPadmaRunCommandResult FPadmaRunRules::World(bool bNoon, bool bAllowDialogue) const
{
    if (!bHasRun) return Fail(TEXT("请先开始新局"));
    if (IsBattle()) return Fail(TEXT("战斗中沙盘时间冻结"));
    if (!bAllowDialogue && IsDialogue()) return Fail(TEXT("请先结束或取消对话"));
    if (!State.Outcome.IsNone()) return Fail(TEXT("本局已结束，请保存记录或开始新局"));
    if (bNoon && State.Calendar.Phase != 1) return Fail(TEXT("此沙盘操作在晌午执行；请推进阶段"));
    return Ok(TEXT("可执行"));
}
bool FPadmaRunRules::CanPay(double Flow, double Calc) const
{
    return bHasRun && NonNegative(Flow) && NonNegative(Calc) && State.Resources.Flow >= Flow && State.Resources.Calc >= Calc;
}
void FPadmaRunRules::Gain(double Flow, double Calc)
{
    State.Resources.Flow = FMath::Clamp(State.Resources.Flow + Flow, 0.0, P(TEXT("flowCap")));
    State.Resources.Calc = FMath::Clamp(State.Resources.Calc + Calc, 0.0, P(TEXT("calcCap")));
}
FPadmaRunCommandResult FPadmaRunRules::TryPay(double Flow, double Calc)
{
    return Atomic([&]()
    {
        if (!IsBattle()) return Fail(TEXT("战斗资源命令只能由当前战斗使用"));
        if (!CanPay(Flow, Calc)) return Fail(TEXT("资源不足或费用无效"));
        State.Resources.Flow -= Flow;
        State.Resources.Calc -= Calc;
        return Ok(TEXT("费用已支付"));
    });
}
FName FPadmaRunRules::CreateCard(FName DefId, FName Zone, FName Node)
{
    if (State.NextId >= MAX_int32 - 1 || State.Cards.Num() >= 2000) { bCounterLimitReached = true; return NAME_None; }
    const auto& Definition = Catalog.Cards.FindChecked(DefId);
    FPadmaCardInstance Value;
    Value.Id = FName(*FString::Printf(TEXT("card-%d"), State.NextId++));
    Value.DefId = DefId;
    Value.Field = Definition.Field;
    Value.HP = Definition.Health > 0 ? Definition.Health : Definition.Stability > 0 ? Definition.Stability : 1;
    Value.Attack = Definition.Attack;
    const FName Id = Value.Id;
    State.Cards.Add(Value);
    SetZone(State.Cards.Last(), Zone);
    if (Zone == TEXT("world"))
    {
        State.Cards.Last().Node = Node;
        State.Cards.Last().DeployedDay = State.Calendar.AbsoluteDay;
        OnPlayed(Id);
    }
    return Id;
}
void FPadmaRunRules::SetZone(FPadmaCardInstance& Value, FName Zone)
{
    State.Deck.Remove(Value.Id);
    State.Hand.Remove(Value.Id);
    State.Discard.Remove(Value.Id);
    Value.Zone = Zone;
    if (Zone != TEXT("world"))
    {
        Value.Node = NAME_None;
        Value.AttachedTo = NAME_None;
        if (Zone == TEXT("deck")) State.Deck.Add(Value.Id);
        else if (Zone == TEXT("hand")) State.Hand.Add(Value.Id);
        else if (Zone == TEXT("discard")) State.Discard.Add(Value.Id);
    }
}
void FPadmaRunRules::OnPlayed(FName Id)
{
    const auto& D = Catalog.Cards.FindChecked(Card(Id)->DefId);
    for (auto& Unit : State.Cards)
    {
        if (Unit.Zone != TEXT("world")) continue;
        if (Unit.DefId == TEXT("vitruvian") && (D.Tags.FindRef(TEXT("繁荣")) || D.Tags.FindRef(TEXT("欲望"))))
        {
            Unit.Attack += 1;
            Emit(TEXT("食烟火：") + Unit.Id.ToString() + TEXT(" 永久攻击 +1"));
        }
        if (Unit.DefId == TEXT("cherry") && !Unit.AttachedTo.IsNone() && D.Kind == TEXT("person")) Gain(1);
    }
}

FPadmaRunCommandResult FPadmaRunRules::Deploy(FName Id, FName Node, FName Anchor)
{
    return Atomic([&]()
    {
        auto Check = World(true);
        if (!Check.bOk) return Check;
        auto* C = MutableCard(Id);
        const auto* N = State.Nodes.Find(Node);
        if (!C || C->Zone != TEXT("hand")) return Fail(TEXT("只能部署手牌"));
        if (!N || N->Owner != TEXT("player")) return Fail(TEXT("只能在己方有效地块部署"));
        if (Catalog.MapLayout.bHexWilderness && (!Catalog.Nodes.FindChecked(Node).bHabitable || !Catalog.Nodes.FindChecked(Node).bTraversable))
            return Fail(TEXT("此地形不能部署卡牌"));
        const auto& D = Catalog.Cards.FindChecked(C->DefId);
        if (D.Family == TEXT("B"))
        {
            const auto* A = Card(Anchor);
            if (!A || A->Zone != TEXT("world") || A->Node != Node || Catalog.Cards.FindChecked(A->DefId).Family != TEXT("A"))
                return Fail(TEXT("请选择此地块的 A 作为附着对象"));
            if (State.Cards.ContainsByPredicate([Anchor](const auto& V) { return V.AttachedTo == Anchor; }))
                return Fail(TEXT("本版仅覆盖一张 A 附着一张 B"));
            C->AttachedTo = Anchor;
        }
        const double Cost = D.Family == TEXT("C") ? P(TEXT("deployC")) : D.Cost;
        if (!CanPay(Cost)) return Fail(TEXT("部署心流不足"));
        State.Resources.Flow -= Cost;
        SetZone(*C, TEXT("world"));
        C->Node = Node;
        C->DeployedDay = State.Calendar.AbsoluteDay;
        OnPlayed(Id);
        Emit(D.DisplayName.ToString() + TEXT(" 部署于 ") + Catalog.Nodes.FindChecked(Node).DisplayName.ToString());
        return Ok(TEXT("已部署 ") + D.DisplayName.ToString());
    });
}

FPadmaRunCommandResult FPadmaRunRules::Advance()
{
    return Atomic([&]()
    {
        auto Check = World();
        if (!Check.bOk) return Check;
        auto& T = State.Calendar;
        if (T.Phase < 2) ++T.Phase;
        else
        {
            if (T.AbsoluteDay == MAX_int32) return Fail(TEXT("日历已达支持上限；阶段未推进"));
            T.Phase = 0;
            ++T.Day;
            ++T.AbsoluteDay;
            if (T.Day > 7) { T.Day = 1; ++T.Era; Emit(TEXT("时代推进")); }
            if (T.Era > 7) { T.Era = 1; ++T.Chapter; Emit(TEXT("章节推进")); }
            double Extra = 0;
            for (const auto& Pair : Catalog.Nodes)
                if (Pair.Value.Type == TEXT("fire") && State.Nodes.FindChecked(Pair.Key).Owner == TEXT("player")) Extra += P(TEXT("fireIncome"));
            for (auto& C : State.Cards)
            {
                if (C.Zone != TEXT("world")) continue;
                const auto& D = Catalog.Cards.FindChecked(C.DefId);
                const int32 Age = T.AbsoluteDay - C.DeployedDay;
                if (D.DailyFlow && (!D.DailyLimit || Age <= D.DailyLimit)) Extra += D.DailyFlow;
                if (C.DefId == TEXT("banquet") && Age > 0 && Age % 3 == 0) State.PlayerCoreHP = FMath::Min(P(TEXT("coreHP")), State.PlayerCoreHP + 1);
                if (D.Family == TEXT("C")) C.HP = FMath::Min(static_cast<double>(D.Health), C.HP + 1);
            }
            State.Preparation.ACTHP = FMath::Min(P(TEXT("actHP")), State.Preparation.ACTHP + 1);
            const double Flow = P(TEXT("flowCap")) * P(TEXT("dailyFraction")) + Extra;
            const double Calc = P(TEXT("calcCap")) * FMath::Max(0.5, P(TEXT("dailyFraction")) * (1 - State.Resources.Entropy / 200));
            Gain(Flow, Calc);
            int32 Drawn = 0;
            while (Drawn < P(TEXT("dailyDraw")) && !State.Deck.IsEmpty())
            {
                SetZone(*MutableCard(State.Deck[0]), TEXT("hand"));
                ++Drawn;
            }
            Emit(FString::Printf(TEXT("黎明结算：心流恢复 %.1f、计算力恢复 %.1f（不超上限），抽取 %d 张 ABC。"), Flow, Calc, Drawn));
            if (State.Deck.IsEmpty()) Emit(TEXT("ABC 牌堆已空：本版不自动回收 ABC 弃牌，完整耗尽规则待确认。"), TEXT("note"));
        }
        if (T.Phase == 1)
        {
            for (auto& B : State.Cards)
            {
                if (B.Zone != TEXT("world") || B.AttachedTo.IsNone() || B.LastAccumulatedDay == T.AbsoluteDay) continue;
                const auto* A = Card(B.AttachedTo);
                if (!A) return Fail(TEXT("附着引用无效"));
                B.Field += Catalog.Cards.FindChecked(A->DefId).Tendency;
                B.LastAccumulatedDay = T.AbsoluteDay;
                Emit(Catalog.Cards.FindChecked(B.DefId).DisplayName.ToString() + FString::Printf(TEXT(" 晌午累加场域 → [%.0f, %.0f]"), B.Field.X, B.Field.Y));
            }
        }
        if (T.Phase == 2) AdvanceWildernessFrontier();
        const FString Phase = Catalog.Phases.IsValidIndex(T.Phase) ? Catalog.Phases[T.Phase].ToString() : FString::FromInt(T.Phase);
        Emit(FString::Printf(TEXT("进入第 %d 天 · "), T.Day) + Phase);
        return Ok(Phase);
    });
}

double FPadmaRunRules::FaithBonus(double X)
{
    if (X >= 95) return 10 + X - 95;
    if (X >= 80) return 4 + 0.4 * (X - 80);
    if (X >= 40) return -4 + 0.175 * (X - 40);
    if (X >= 20) return -15 + 0.55 * (X - 20);
    return -33 + 0.9 * X;
}
FPadmaRunCommandResult FPadmaRunRules::PreviewSynthesis(FName Id, FPadmaSynthesisRecord& Out) const
{
    Out = {};
    const auto* A = Card(Id);
    if (!A || A->Zone != TEXT("world") || Catalog.Cards.FindChecked(A->DefId).Family != TEXT("A")) return Fail(TEXT("请选择已部署的 A"));
    const FPadmaCardInstance* B = nullptr;
    for (const auto& C : State.Cards)
    {
        if (C.Zone == TEXT("world") && C.AttachedTo == A->Id)
        {
            if (B) return Fail(TEXT("本版需要一张 A 与一张附着 B"));
            B = &C;
        }
    }
    if (!B) return Fail(TEXT("需要 A 与一张附着 B"));
    const auto& AD = Catalog.Cards.FindChecked(A->DefId);
    const auto& BD = Catalog.Cards.FindChecked(B->DefId);
    TMap<FName, double> Combined;
    for (const auto* D : {&AD, &BD}) for (const auto& Tag : D->Tags) Combined.FindOrAdd(Tag.Key) += Tag.Value / 100.0;
    double MaxScore = -DBL_MAX;
    // Authoring order is part of seeded replay: never iterate the card TMap for roulette.
    for (FName CandidateId : Catalog.CardOrder)
    {
        const auto& D = Catalog.Cards.FindChecked(CandidateId);
        if (D.Family != TEXT("C")) continue;
        FPadmaSynthesisCandidate C;
        C.Id = CandidateId;
        for (const auto& Tag : D.Tags) C.Score += Combined.FindRef(Tag.Key) * Tag.Value / 100.0;
        if (C.Score <= 0) continue;
        MaxScore = FMath::Max(MaxScore, C.Score);
        C.Distance = FMath::Abs(B->Field.X - D.Target.X) + FMath::Abs(B->Field.Y - D.Target.Y);
        C.Affinity = AD.Affinity;
        C.Entropy = -FMath::Pow(State.Resources.Entropy / 10, 1.5);
        C.Faith = FaithBonus(State.Resources.Faith);
        C.RawSuccess = (100 - C.Distance + C.Affinity + C.Entropy + C.Faith) / 100;
        C.Success = FMath::Clamp(C.RawSuccess, 0.0, 1.0);
        Out.Candidates.Add(C);
    }
    if (Out.Candidates.IsEmpty()) return Fail(TEXT("本版没有共同标签候选；无标签兜底规则尚未接入"));
    double Sum = 0;
    for (auto& C : Out.Candidates) { C.Probability = FMath::Exp((C.Score - MaxScore) / P(TEXT("temperature"))); Sum += C.Probability; }
    for (auto& C : Out.Candidates) C.Probability /= Sum;
    Out.A = A->Id;
    Out.B = B->Id;
    Out.Node = A->Node;
    Out.Field = B->Field;
    Out.Flow = P(TEXT("synthesisFlow"));
    Out.Calc = P(TEXT("synthesisCalc"));
    return Ok(TEXT("合成预览"), EPadmaRunCommandEffect::None);
}
FPadmaRunCommandResult FPadmaRunRules::Synthesize(FName Id)
{
    return Atomic([&]()
    {
        auto Check = World(true);
        if (!Check.bOk) return Check;
        FPadmaSynthesisRecord Record;
        if (State.SynthesisHistory.Num() >= 2000) return Fail(TEXT("合成记录已达本版支持上限"));
        Check = PreviewSynthesis(Id, Record);
        if (!Check.bOk) return Check;
        if (!Catalog.Nodes.Contains(Record.Node) || Card(Record.B)->Node != Record.Node) return Fail(TEXT("合成材料必须位于同一有效节点"));
        if (!CanPay(Record.Flow, Record.Calc)) return Fail(TEXT("合成资源不足"));
        State.Resources.Flow -= Record.Flow;
        State.Resources.Calc -= Record.Calc;
        Record.SelectionRoll = Random();
        const FPadmaSynthesisCandidate* Selected = &Record.Candidates.Last();
        double At = 0;
        for (const auto& C : Record.Candidates) { At += C.Probability; if (Record.SelectionRoll < At) { Selected = &C; break; } }
        Record.Selected = Selected->Id;
        Record.SuccessRoll = Random();
        Record.bSuccess = Record.SuccessRoll < Selected->Success;
        const bool bCherry = Card(Record.B)->DefId == TEXT("cherry");
        SetZone(*MutableCard(Record.A), TEXT("discard"));
        SetZone(*MutableCard(Record.B), TEXT("discard"));
        if (Record.bSuccess)
        {
            Record.CreatedId = CreateCard(Record.Selected, TEXT("world"), Record.Node);
            if (bCherry) State.Resources.Entropy = FMath::Max(0.0, State.Resources.Entropy - 3);
        }
        State.SynthesisHistory.Add(Record);
        const FString Text = (Record.bSuccess ? TEXT("合成成功：") + Catalog.Cards.FindChecked(Record.Selected).DisplayName.ToString() + TEXT(" 已直接出现于") + Catalog.Nodes.FindChecked(Record.Node).DisplayName.ToString() : FString(TEXT("合成失败：无奖励，费用不退"))) + TEXT("；A、B 已进入弃牌堆。");
        Emit(Text, Record.bSuccess ? FName(TEXT("success")) : FName(TEXT("note")));
        return Ok(Text);
    });
}

FPadmaRunCommandResult FPadmaRunRules::PreviewMove(const TArray<FName>& Ids, FName Node, FPadmaCardMovePreview& Out) const
{
    Out = {};
    const auto* Target = Catalog.Nodes.Find(Node);
    if (!Target || !State.Nodes.Contains(Node)) return Fail(TEXT("未知目标地块"));
    if (Catalog.MapLayout.bHexWilderness && (!Target->bHabitable || !Target->bTraversable)) return Fail(TEXT("目标地形不可居住或通行"));
    if (Ids.IsEmpty()) return Fail(TEXT("请选择要移动的卡牌"));
    for (FName Id : Ids) { if (!Card(Id)) return Fail(TEXT("找不到卡牌实例")); Out.Ids.AddUnique(Id); }
    const TArray<FName> Chosen = Out.Ids;
    for (FName Id : Chosen) if (!Card(Id)->AttachedTo.IsNone()) Out.Ids.AddUnique(Card(Id)->AttachedTo);
    for (const auto& C : State.Cards) if (!C.AttachedTo.IsNone() && Out.Ids.Contains(C.AttachedTo)) Out.Ids.AddUnique(C.Id);
    Out.Source = Card(Out.Ids[0])->Node;
    Out.Target = Node;
    for (FName Id : Out.Ids)
    {
        const auto* C = Card(Id);
        if (!C || C->Zone != TEXT("world") || C->Node != Out.Source) return Fail(TEXT("整组卡牌必须在同一来源地块"));
    }
    const auto* Source = State.Nodes.Find(Out.Source);
    if (!Source || Source->Owner != TEXT("player")) return Fail(TEXT("来源地块不属于己方"));
    if (Catalog.MapLayout.bHexWilderness && (!Catalog.Nodes.FindChecked(Out.Source).bHabitable || !Catalog.Nodes.FindChecked(Out.Source).bTraversable))
        return Fail(TEXT("来源地形不可通行"));
    const auto* Edge = Catalog.Edges.FindByPredicate([&](const auto& E) { return E.From == Out.Source && E.To == Node; });
    if (!Edge) return Fail(TEXT("没有这条预设单向道路，视觉相邻不代表可通行"));
    if (!Edge->UnlockFlag.IsNone() && !State.Unlocks.Contains(Edge->UnlockFlag)) return Fail(TEXT("道路前置剧情尚未满足：") + Edge->UnlockFlag.ToString());
    if (Catalog.MapLayout.bHexWilderness && Target->bWilderness && State.Nodes.FindChecked(Node).Owner!=TEXT("player") && Target->EnemyCount==0)
    {
        const auto Occupation=PreviewWildernessOccupation(Out.Source,Node,TEXT("player"));
        if (!Occupation.bOk) return Occupation;
    }
    Out.bEnemy = State.Nodes.FindChecked(Node).Owner == TEXT("ruler")
        && !(Catalog.MapLayout.bHexWilderness && Target->bWilderness && Target->EnemyCount==0);
    for (FName Id : Out.Ids)
    {
        const auto& D = Catalog.Cards.FindChecked(Card(Id)->DefId);
        const auto* Policy = State.Mobility.Find(D.Id);
        FPadmaCardMoveCheck Check;
        Check.Id = Id;
        if (!Policy || (Policy->Mode != TEXT("movable") && Policy->Mode != TEXT("immovable") && Policy->Mode != TEXT("conditional"))) Check.Reason = TEXT("移动规则未配置");
        else if (Policy->Mode == TEXT("immovable")) Check.Reason = TEXT("不可移动");
        else if (Policy->Mode == TEXT("conditional") && State.Resources.Faith < Policy->MinFaith) Check.Reason = FString::Printf(TEXT("需要信仰 ≥ %.0f"), Policy->MinFaith);
        else if (Policy->Mode == TEXT("conditional") && !Policy->ForbiddenTerrain.IsEmpty() && FName(*Policy->ForbiddenTerrain) == Target->Terrain) Check.Reason = TEXT("禁止进入") + Policy->ForbiddenTerrain;
        Check.bAllowed = Check.Reason.IsEmpty();
        Check.Calc = P(D.Family == TEXT("A") ? TEXT("moveA") : D.Family == TEXT("B") ? TEXT("moveB") : D.Kind == TEXT("building") ? TEXT("moveBuilding") : TEXT("movePerson"));
        Out.Calc += Check.Calc;
        Out.Checks.Add(MoveTemp(Check));
    }
    return Ok(TEXT("移动预览"), EPadmaRunCommandEffect::None);
}
FPadmaRunCommandResult FPadmaRunRules::Move(const TArray<FName>& Ids, FName Node, FName Mode)
{
    return Atomic([&]()
    {
        auto Check = World();
        if (!Check.bOk) return Check;
        FPadmaCardMovePreview Preview;
        Check = PreviewMove(Ids, Node, Preview);
        if (!Check.bOk) return Check;
        for (const auto& C : Preview.Checks) if (!C.bAllowed) return Fail(C.Id.ToString() + TEXT("：") + C.Reason);
        if (!Preview.bEnemy && State.Calendar.Phase != 1) return Fail(TEXT("非战斗移动在晌午执行"));
        if (Preview.bEnemy)
        {
            if (Mode != TEXT("encounter") && Mode != TEXT("act")) return Fail(TEXT("请选择 Encounter 或 ACT；FPS 仅可配置编队"));
            bool bLivingC = false;
            for (FName Id : Preview.Ids) if (Catalog.Cards.FindChecked(Card(Id)->DefId).Family == TEXT("C") && Card(Id)->HP > 0) bLivingC = true;
            if (Mode == TEXT("encounter") && !bLivingC) return Fail(TEXT("Encounter 移动组需要存活 C 单位"));
            if (Mode == TEXT("act") && State.Preparation.ACTHP <= 0) return Fail(TEXT("ACT 出战角色没有生命"));
        }
        if (!CanPay(0, Preview.Calc)) return Fail(TEXT("移动计算力不足"));
        const FPadmaRunState Snapshot = State; // Must precede every movement cost and battle log.
        State.Resources.Calc -= Preview.Calc;
        if (Preview.bEnemy)
        {
            BattleSnapshot = Snapshot;
            Battle.Mode = Mode;
            Battle.Node = Node;
            Battle.Source = Preview.Source;
            Battle.Ids = Preview.Ids;
            Battle.MovementCalc = Preview.Calc;
            Emit(TEXT("进入 ") + Mode.ToString().ToUpper() + TEXT(" @ ") + Catalog.Nodes.FindChecked(Node).DisplayName.ToString() + TEXT("；沙盘冻结，保存完整入战前快照。"));
            return Ok(TEXT("进入战斗，日历已冻结"), EPadmaRunCommandEffect::BattleRequested);
        }
        for (FName Id : Preview.Ids) MutableCard(Id)->Node = Node;
        Capture(Node);
        Emit(FString::Printf(TEXT("移动完成：%d 张卡 → "), Preview.Ids.Num()) + Catalog.Nodes.FindChecked(Node).DisplayName.ToString() + FString::Printf(TEXT("，计算力 −%.0f"), Preview.Calc));
        return Ok(TEXT("已移动到 ") + Catalog.Nodes.FindChecked(Node).DisplayName.ToString());
    });
}
void FPadmaRunRules::Capture(FName Id)
{
    if (Catalog.MapLayout.bHexWilderness && Catalog.Nodes.FindChecked(Id).bWilderness)
    {
        ApplyWildernessOccupation(Id,TEXT("player"));
        return;
    }
    auto& Node = State.Nodes.FindChecked(Id);
    Node.bVisited = true;
    if (Node.Owner != TEXT("player"))
    {
        Node.Owner = TEXT("player");
        State.War += P(Catalog.Nodes.FindChecked(Id).Type == TEXT("gate") ? TEXT("gateWar") : TEXT("captureWar"));
        if (Id == BossNode()) State.RulerCoreHP = FMath::Max(0.0, State.RulerCoreHP - P(TEXT("coreStrike")));
        Emit(TEXT("占领 ") + Catalog.Nodes.FindChecked(Id).DisplayName.ToString() + FString::Printf(TEXT("，战局天平 %.0f"), State.War), TEXT("success"));
    }
    ResolveStoryCheckpoint(Id);
    EvaluateOutcome();
    if (State.Outcome.IsNone() && (!Catalog.Nodes.FindChecked(Id).NPCId.IsNone()
        || Catalog.MapLayout.Checkpoints.ContainsByPredicate([Id](const auto& C) { return C.NodeId == Id; })))
        BeginNodeDialogue(Id);
}
void FPadmaRunRules::EvaluateOutcome()
{
    if (State.RulerCoreHP <= 0) State.Outcome = TEXT("core");
    else if (State.War >= P(TEXT("warGoal"))) State.Outcome = TEXT("balance");
}

FPadmaRunCommandResult FPadmaRunRules::Operation(FName Node)
{
    return Atomic([&]()
    {
        auto Check = World(true);
        if (!Check.bOk) return Check;
        const auto* N = State.Nodes.Find(Node);
        if (!N || N->Owner != TEXT("player")) return Fail(TEXT("先占领此地块"));
        if (N->bUsed) return Fail(TEXT("此地块的一次性事件已经使用"));
        if (BeginNodeDialogue(Node)) return Ok(TEXT("对话已开始；完成确认前不结算奖励"), EPadmaRunCommandEffect::DialogueChanged);
        return Fail(TEXT("此地块没有可用的一次性操作"));
    });
}
FPadmaRunCommandResult FPadmaRunRules::DialogueNext(FName Choice)
{
    return Atomic([&]()
    {
        auto Check = World(true, true);
        if (!Check.bOk) return Check;
        const auto* Line = FindLine(Catalog, DialogueState.DialogueId, DialogueState.Line);
        if (!Line) return Fail(TEXT("当前没有有效对话"));
        FName Next;
        if (!Line->Choices.IsEmpty())
        {
            const auto* Selected = Line->Choices.FindByPredicate([Choice](const auto& C) { return C.Id == Choice; });
            if (!Selected) return Fail(TEXT("请选择当前对话提供的选项"));
            if (Selected->bCancel) { DialogueState = {}; return Ok(TEXT("已离开对话；没有领取奖励或消耗事件"), EPadmaRunCommandEffect::DialogueChanged); }
            if (Selected->bComplete)
            {
                auto* N = State.Nodes.Find(DialogueState.Node);
                if (!N || N->Owner != TEXT("player") || N->bUsed) return Fail(TEXT("此事件已使用或不属于己方"));
                const auto& Definition = Catalog.Nodes.FindChecked(DialogueState.Node);
                N->bUsed = true;
                if (!Definition.NPCId.IsNone())
                {
                    State.SeenNPCs.AddUnique(Definition.NPCId);
                    State.CompletedNPCs.AddUnique(Definition.NPCId);
                }
                if (!Definition.CompletionFlag.IsNone()) State.Unlocks.AddUnique(Definition.CompletionFlag);
                if (Definition.Type == TEXT("story"))
                {
                    State.bStoryMet = true;
                    if (Definition.CompletionFlag.IsNone()) State.Unlocks.AddUnique(TEXT("story-road"));
                    Gain(0, P(TEXT("storyCalc")));
                }
                else if (Definition.Type == TEXT("forge")) Gain(P(TEXT("forgeFlow")));
                Emit(TEXT("已完成：") + Definition.DisplayName.ToString(), TEXT("success"));
                DialogueState = {};
                return Ok(TEXT("地块事件已结算"), EPadmaRunCommandEffect::DialogueChanged);
            }
            Next = Selected->Next;
        }
        else
        {
            if (!Choice.IsNone()) return Fail(TEXT("当前对话不接受选项"));
            Next = Line->Next;
        }
        if (!FindLine(Catalog, DialogueState.DialogueId, Next)) return Fail(TEXT("对话没有有效后继"));
        DialogueState.Line = Next;
        return Ok(TEXT("对话继续"), EPadmaRunCommandEffect::DialogueChanged);
    });
}
FPadmaRunCommandResult FPadmaRunRules::DialogueCancel()
{
    return Atomic([&]()
    {
        if (!IsDialogue()) return Fail(TEXT("当前没有进行中的对话"));
        DialogueState = {};
        return Ok(TEXT("已离开对话；没有领取奖励或消耗事件"), EPadmaRunCommandEffect::DialogueChanged);
    });
}
FPadmaRunCommandResult FPadmaRunRules::Ability(FName Id, bool bReverse)
{
    return Atomic([&]()
    {
        auto Check = World(true);
        if (!Check.bOk) return Check;
        auto* C = MutableCard(Id);
        if (!C || C->Zone != TEXT("world")) return Fail(TEXT("技能需要已部署卡牌"));
        if (C->DefId == TEXT("vitruvian"))
        {
            if (!CanPay(1)) return Fail(TEXT("心流不足"));
            State.Resources.Flow -= 1;
            for (auto& Ally : State.Cards)
            {
                const auto& D = Catalog.Cards.FindChecked(Ally.DefId);
                if (Ally.Zone == TEXT("world") && D.Family == TEXT("C")) Ally.HP = FMath::Min(static_cast<double>(D.Health), Ally.HP + 1);
            }
            Emit(TEXT("完美肉体：己方已部署非核心 C 恢复 1 生命"));
        }
        else if (C->DefId == TEXT("banquet"))
        {
            const FString Era = FString::Printf(TEXT("%d:%d"), State.Calendar.Chapter, State.Calendar.Era);
            if (C->ActiveEra == Era) return Fail(TEXT("希律一诺本时代已经使用"));
            if (!CanPay(bReverse ? 0 : 4, bReverse ? 4 : 0)) return Fail(TEXT("兑换资源不足"));
            if (bReverse) { State.Resources.Calc -= 4; Gain(4); }
            else { State.Resources.Flow -= 4; Gain(0, 4); }
            C->ActiveEra = Era;
            Emit(bReverse ? TEXT("希律一诺：4 计算力换心流") : TEXT("希律一诺：4 心流换计算力"));
        }
        else return Fail(TEXT("此卡的沙盘主动技能尚未接入本版"));
        return Ok(TEXT("沙盘主动技能已生效"));
    });
}

FPadmaRunCommandResult FPadmaRunRules::ValidatePreparation(const FPadmaPreparationState& V) const
{
    if (!Catalog.ACTCharacters.Contains(V.ACTCharacter) || !Catalog.ACTWeapons.Contains(V.ACTWeapon)) return Fail(TEXT("ACT 角色或武器 ID 无效"));
    if (!Catalog.HomeCharacters.Contains(V.HomeCharacter) || !Catalog.HomeOutfits.Contains(V.HomeOutfit) || !Catalog.HomePoses.Contains(V.HomePose)) return Fail(TEXT("归处展示 ID 无效"));
    const auto* Weapon = Catalog.HomeWeapons.Find(V.HomeWeapon);
    if (!Weapon || !Weapon->CompatiblePoseIds.Contains(V.HomePose)) return Fail(TEXT("归处武器与姿态不兼容"));
    if (!Catalog.FPSCharacters.Contains(V.FPSCharacter) || !Catalog.FPSWeapons.Contains(V.FPSWeapon)) return Fail(TEXT("FPS 编队 ID 无效"));
    if (!FMath::IsFinite(V.ACTHP) || V.ACTHP <= 0) return Fail(TEXT("ACT 生命无效"));
    return Ok(TEXT("配置有效"));
}
FPadmaRunCommandResult FPadmaRunRules::ConfigurePreparation(const FPadmaPreparationState& Preparation)
{
    return Atomic([&]()
    {
        auto Check = World();
        if (!Check.bOk) return Check;
        Check = ValidatePreparation(Preparation);
        if (!Check.bOk) return Check;
        const double HP = State.Preparation.ACTHP;
        State.Preparation = Preparation;
        State.Preparation.ACTHP = HP; // Selection never heals, mutates ABC cards, or changes character stats.
        Emit(TEXT("配置已应用：独立 ACT 阵容、归处展示与 FPS 展示编队。外观选择不提供属性。"));
        return Ok(TEXT("配置已应用"));
    });
}
FPadmaRunCommandResult FPadmaRunRules::ConfigureMobility(FName DefId, const FPadmaCardMobilityPolicy& Policy)
{
    return Atomic([&]()
    {
        auto Check = World();
        if (!Check.bOk) return Check;
        if (!Catalog.Cards.Contains(DefId)) return Fail(TEXT("无效卡牌定义"));
        if (Policy.Mode != TEXT("movable") && Policy.Mode != TEXT("immovable") && Policy.Mode != TEXT("conditional") && Policy.Mode != TEXT("unconfigured")) return Fail(TEXT("无效移动配置"));
        if (!NonNegative(Policy.MinFaith) || Policy.MinFaith > 100) return Fail(TEXT("移动信仰条件须为 0–100"));
        bool bTerrainKnown = Policy.ForbiddenTerrain.IsEmpty();
        for (const auto& Pair : Catalog.Nodes) if (Pair.Value.Terrain == FName(*Policy.ForbiddenTerrain)) bTerrainKnown = true;
        if (!bTerrainKnown) return Fail(TEXT("无效地形条件"));
        State.Mobility.Add(DefId, Policy);
        Emit(TEXT("卡牌移动规则已更新：") + DefId.ToString());
        return Ok(TEXT("移动配置已应用"));
    });
}

FPadmaRunCommandResult FPadmaRunRules::ConsumeBasicSkill(FName SkillId)
{
    return Atomic([&]()
    {
        if (!IsBattle()) return Fail(TEXT("基础技能只能在局部战斗使用"));
        const FPadmaSkillEffectRowBase* Skill = Battle.Mode == TEXT("act") ? static_cast<const FPadmaSkillEffectRowBase*>(Catalog.ACTSkills.Find(SkillId)) : static_cast<const FPadmaSkillEffectRowBase*>(Catalog.EncounterSkills.Find(SkillId));
        if (!Skill || !State.SkillsAvailable.Contains(SkillId)) return Fail(TEXT("未知技能或此技能已在弃牌堆"));
        if (State.SkillsAvailable.Num() == 1 && State.SkillCycles >= MAX_int32 - 1) return Fail(TEXT("技能回收次数已达支持上限"));
        if (!CanPay(Skill->FlowCost, Skill->CalculationCost)) return Fail(TEXT("基础技能资源不足"));
        State.Resources.Flow -= Skill->FlowCost;
        State.Resources.Calc -= Skill->CalculationCost;
        State.SkillsAvailable.Remove(SkillId);
        State.SkillsDiscard.Add(SkillId);
        if (State.SkillsAvailable.IsEmpty())
        {
            State.SkillsAvailable = State.SkillsDiscard;
            State.SkillsDiscard.Reset();
            for (int32 I = State.SkillsAvailable.Num() - 1; I > 0; --I)
            {
                const int32 J = FMath::FloorToInt(Random() * (I + 1));
                State.SkillsAvailable.Swap(I, J);
            }
            ++State.SkillCycles;
        }
        return Ok(TEXT("基础技能费用与卡库已结算"));
    });
}
FPadmaRunCommandResult FPadmaRunRules::ChangeFaith(double PlayerDelta, double RulerDelta)
{
    return Atomic([&]()
    {
        if (!IsBattle() || !FMath::IsFinite(PlayerDelta) || !FMath::IsFinite(RulerDelta)) return Fail(TEXT("无效战斗信仰命令"));
        State.Resources.Faith = FMath::Clamp(State.Resources.Faith + PlayerDelta, 0.0, 100.0);
        State.RulerFaith = FMath::Clamp(State.RulerFaith + RulerDelta, 0.0, 100.0);
        return Ok(TEXT("信仰已更新"));
    });
}
FPadmaRunCommandResult FPadmaRunRules::FinishBattle(bool bWon, const TMap<FName, double>& UnitHP, double ACTHP, int32 Actions, double Seconds)
{
    return Atomic([&]()
    {
        if (!IsBattle() || !BattleSnapshot.IsSet()) return Fail(TEXT("当前没有待结算战斗"));
        if (!bWon)
        {
            State = BattleSnapshot.GetValue();
            Battle = {};
            BattleSnapshot.Reset();
            return Ok(TEXT("已完整恢复入战前状态，包括移动费用、卡库、随机流和日志"), EPadmaRunCommandEffect::BattleRolledBack);
        }
        if (Actions < 0 || !NonNegative(Seconds)) return Fail(TEXT("战斗结果计数无效"));
        if (State.BattlesWon.Num() >= 2000) return Fail(TEXT("战果记录已达本版支持上限；可退出并恢复入战前状态"));
        for (const auto& Pair : UnitHP)
        {
            const auto* C = Card(Pair.Key);
            if (!C || !Battle.Ids.Contains(Pair.Key) || Catalog.Cards.FindChecked(C->DefId).Family != TEXT("C") || !NonNegative(Pair.Value) || Pair.Value > Catalog.Cards.FindChecked(C->DefId).Health)
                return Fail(TEXT("战斗结果包含无效单位或生命"));
        }
        if (Battle.Mode == TEXT("encounter"))
        {
            bool bLiving = false;
            for (FName Id : Battle.Ids)
            {
                if (Catalog.Cards.FindChecked(Card(Id)->DefId).Family != TEXT("C")) continue;
                const double* HP = UnitHP.Find(Id);
                if (!HP) return Fail(TEXT("Encounter 战果缺少出战 C 的生命"));
                bLiving |= *HP > 0;
            }
            if (!bLiving) return Fail(TEXT("胜利战果必须有存活出战单位"));
        }
        else if (!FMath::IsFinite(ACTHP) || ACTHP <= 0 || ACTHP > P(TEXT("actHP")) || !UnitHP.IsEmpty()) return Fail(TEXT("ACT 战果生命或单位域无效"));
        const FName Target = Battle.Node;
        for (FName Id : Battle.Ids)
        {
            auto* C = MutableCard(Id);
            if (const double* HP = UnitHP.Find(Id))
            {
                C->HP = *HP;
                if (*HP <= 0) { SetZone(*C, TEXT("discard")); continue; }
            }
            C->Node = Target;
        }
        if (Battle.Mode == TEXT("act")) State.Preparation.ACTHP = ACTHP;
        FPadmaBattleRecord Record;
        Record.Mode = Battle.Mode;
        Record.Node = Target;
        Record.Actions = Actions;
        Record.Seconds = Seconds;
        State.BattlesWon.Add(Record);
        Capture(Target);
        Emit(TEXT("胜利，提交战果并返回沙盘"), TEXT("success"));
        Battle = {};
        BattleSnapshot.Reset();
        return Ok(TEXT("胜利，已返回沙盘"), EPadmaRunCommandEffect::BattleCommitted);
    });
}

FPadmaRunCommandResult FPadmaRunRules::CheckSaveBoundary() const
{
    if (!bHasRun) return Fail(TEXT("没有本局状态"));
    if (IsBattle()) return Fail(TEXT("请先结束或退出战斗，再在沙盘安全边界保存"));
    if (IsDialogue()) return Fail(TEXT("请先结束或取消对话，再在沙盘安全边界保存"));
    return Ok(TEXT("可以保存"), EPadmaRunCommandEffect::None);
}

FPadmaRunCommandResult FPadmaRunRules::ValidateState(const FPadmaRunState& R) const
{
    if (R.Version != 1) return Fail(TEXT("存档版本不兼容"));
    if (FPadmaWorldMapGenerator::Signature(R.MapLayout) != FPadmaWorldMapGenerator::Signature(Catalog.MapLayout))
        return Fail(TEXT("存档与当前本局地图定义不一致"));
    auto Checked = CheckProfile(Catalog, R.Profile);
    if (!Checked.bOk) return Checked;
    const auto Value = [&R](FName Key) { return R.Profile.FindChecked(Key); };
    const auto& T = R.Calendar;
    if (T.Chapter < 1 || T.Era < 1 || T.Era > 7 || T.Day < 1 || T.Day > 7 || T.Phase < 0 || T.Phase > 2 || T.AbsoluteDay < 1 || static_cast<int64>(T.AbsoluteDay) != (static_cast<int64>(T.Chapter) - 1) * 49 + (T.Era - 1) * 7 + T.Day)
        return Fail(TEXT("存档日历无效"));
    if (!NonNegative(R.Resources.Flow) || R.Resources.Flow > Value(TEXT("flowCap")) || !NonNegative(R.Resources.Calc) || R.Resources.Calc > Value(TEXT("calcCap")) || !NonNegative(R.Resources.Faith) || R.Resources.Faith > 100 || !NonNegative(R.Resources.Entropy) || R.Resources.Entropy > 100 || !NonNegative(R.RulerFaith) || R.RulerFaith > 100)
        return Fail(TEXT("存档资源无效"));
    if (!NonNegative(R.PlayerCoreHP) || R.PlayerCoreHP > Value(TEXT("coreHP")) || !NonNegative(R.RulerCoreHP) || R.RulerCoreHP > Value(TEXT("coreHP")) || !NonNegative(R.War)) return Fail(TEXT("存档核心或战局天平无效"));
    const FName ExpectedOutcome = R.RulerCoreHP <= 0 ? FName(TEXT("core")) : R.War >= Value(TEXT("warGoal")) ? FName(TEXT("balance")) : NAME_None;
    if (R.Outcome != ExpectedOutcome) return Fail(TEXT("存档终局状态不一致"));
    if (R.Cards.Num() > 2000 || R.NextId < 1 || R.NextId == MAX_int32 || R.Sequence < 0 || R.Sequence == MAX_int32 || R.Log.Num() > 120 || R.SkillCycles < 0 || R.SkillCycles == MAX_int32) return Fail(TEXT("存档计数或大小无效"));
    if (R.Nodes.Num() != Catalog.Nodes.Num()) return Fail(TEXT("存档地图不兼容"));
    for (const auto& Pair : Catalog.Nodes)
    {
        const auto* N = R.Nodes.Find(Pair.Key);
        if (!N || (N->Owner != TEXT("player") && N->Owner != TEXT("neutral") && N->Owner != TEXT("ruler"))) return Fail(TEXT("存档地图 ID 或所有者无效"));
        if (Catalog.MapLayout.bHexWilderness && (!Pair.Value.bHabitable || !Pair.Value.bTraversable)
            && (N->Owner!=TEXT("neutral") || N->bVisited || N->bUsed)) return Fail(TEXT("存档不可居住地块含有占领或访问状态"));
        if (N->bUsed && Pair.Value.DialogueId.IsNone() && Pair.Value.Type != TEXT("story") && Pair.Value.Type != TEXT("forge")
            && !Catalog.MapLayout.Checkpoints.ContainsByPredicate([&](const auto& C) { return C.NodeId == Pair.Key; })) return Fail(TEXT("存档事件 ID 无效"));
    }
    TSet<FName> Ids;
    int32 MaxInstanceId = 0;
    for (const auto& C : R.Cards)
    {
        const auto* D = Catalog.Cards.Find(C.DefId);
        const FString IdText = C.Id.ToString();
        const FString Number = IdText.Mid(5);
        if (!D || Ids.Contains(C.Id) || !IdText.StartsWith(TEXT("card-")) || Number.IsEmpty() || !Number.IsNumeric()) return Fail(TEXT("存档卡牌定义或 ID 无效"));
        const int64 NumberValue = FCString::Atoi64(*Number);
        if (NumberValue <= 0 || NumberValue >= MAX_int32 || FString::Printf(TEXT("card-%lld"), NumberValue) != IdText) return Fail(TEXT("存档卡牌实例 ID 无效"));
        MaxInstanceId = FMath::Max(MaxInstanceId, static_cast<int32>(NumberValue));
        Ids.Add(C.Id);
        if (C.Zone != TEXT("world") && C.Zone != TEXT("deck") && C.Zone != TEXT("hand") && C.Zone != TEXT("discard")) return Fail(TEXT("存档卡牌分区无效"));
        if (!NonNegative(C.HP) || !NonNegative(C.Attack) || !FMath::IsFinite(C.Field.X) || !FMath::IsFinite(C.Field.Y) || C.DeployedDay < 0 || C.DeployedDay > T.AbsoluteDay || C.LastAccumulatedDay < 0 || C.LastAccumulatedDay > T.AbsoluteDay)
            return Fail(TEXT("存档卡牌数值无效"));
        const double MaxHP = D->Health > 0 ? D->Health : D->Stability > 0 ? D->Stability : 1;
        if (C.HP > MaxHP || (C.Zone != TEXT("discard") && C.HP <= 0)) return Fail(TEXT("存档卡牌生命无效"));
        if (C.Zone == TEXT("world"))
        {
            if (!R.Nodes.Contains(C.Node) || C.DeployedDay < 1) return Fail(TEXT("存档世界卡节点无效"));
            if (Catalog.MapLayout.bHexWilderness && (!Catalog.Nodes.FindChecked(C.Node).bHabitable || !Catalog.Nodes.FindChecked(C.Node).bTraversable
                || R.Nodes.FindChecked(C.Node).Owner!=TEXT("player"))) return Fail(TEXT("存档卡牌不在己方可居住地块"));
            if (D->Family == TEXT("B") && C.AttachedTo.IsNone()) return Fail(TEXT("世界 B 必须附着 A"));
        }
        else if (!C.Node.IsNone() || !C.AttachedTo.IsNone()) return Fail(TEXT("存档非世界卡含有节点或附着引用"));
    }
    if (R.NextId <= MaxInstanceId) return Fail(TEXT("存档下一个卡牌 ID 冲突"));
    for (FName Zone : {FName(TEXT("deck")), FName(TEXT("hand")), FName(TEXT("discard"))})
    {
        const auto& List = Zone == TEXT("deck") ? R.Deck : Zone == TEXT("hand") ? R.Hand : R.Discard;
        TSet<FName> Seen;
        for (FName Id : List)
        {
            const auto* C = R.Cards.FindByPredicate([Id](const auto& V) { return V.Id == Id; });
            if (!C || C->Zone != Zone || Seen.Contains(Id)) return Fail(TEXT("存档卡库分区不一致"));
            Seen.Add(Id);
        }
        for (const auto& C : R.Cards) if (C.Zone == Zone && !Seen.Contains(C.Id)) return Fail(TEXT("存档卡库遗漏实例"));
    }
    TSet<FName> Anchors;
    for (const auto& C : R.Cards)
    {
        if (C.AttachedTo.IsNone()) continue;
        const auto* A = R.Cards.FindByPredicate([&C](const auto& V) { return V.Id == C.AttachedTo; });
        if (!A || A->Id == C.Id || C.Zone != TEXT("world") || A->Zone != TEXT("world") || A->Node != C.Node || Catalog.Cards.FindChecked(A->DefId).Family != TEXT("A") || Catalog.Cards.FindChecked(C.DefId).Family != TEXT("B") || Anchors.Contains(A->Id)) return Fail(TEXT("存档附着引用无效"));
        Anchors.Add(A->Id);
    }
    if (R.SkillSlots != Catalog.SkillOrder) return Fail(TEXT("存档共享槽位不兼容"));
    TSet<FName> SkillIds;
    for (const auto* List : {&R.SkillsAvailable, &R.SkillsDiscard})
        for (FName Id : *List) { if (!Catalog.SkillOrder.Contains(Id) || SkillIds.Contains(Id)) return Fail(TEXT("存档基础技能卡库无效")); SkillIds.Add(Id); }
    if (SkillIds.Num() != Catalog.SkillOrder.Num() || R.SkillsAvailable.IsEmpty()) return Fail(TEXT("存档基础技能卡库不完整"));
    Checked = ValidatePreparation(R.Preparation);
    if (!Checked.bOk || R.Preparation.ACTHP > Value(TEXT("actHP"))) return Fail(TEXT("存档准备配置无效：") + Checked.Text);
    if (R.Mobility.Num() != Catalog.Cards.Num()) return Fail(TEXT("存档移动规则不完整"));
    for (const auto& Pair : R.Mobility)
    {
        const auto& Policy = Pair.Value;
        if (!Catalog.Cards.Contains(Pair.Key) || (Policy.Mode != TEXT("movable") && Policy.Mode != TEXT("immovable") && Policy.Mode != TEXT("conditional") && Policy.Mode != TEXT("unconfigured")) || !NonNegative(Policy.MinFaith) || Policy.MinFaith > 100) return Fail(TEXT("存档移动规则无效"));
        bool bTerrainKnown = Policy.ForbiddenTerrain.IsEmpty();
        for (const auto& Node : Catalog.Nodes) if (Node.Value.Terrain == FName(*Policy.ForbiddenTerrain)) bTerrainKnown = true;
        if (!bTerrainKnown) return Fail(TEXT("存档移动地形 ID 无效"));
    }
    Checked = ValidateStoryState(R);
    if (!Checked.bOk) return Checked;
    int32 PreviousLogId = 0;
    for (const auto& Entry : R.Log)
    {
        if (Entry.Id <= PreviousLogId || Entry.Id > R.Sequence || Entry.Text.Len() > 8192 || (Entry.Kind != TEXT("info") && Entry.Kind != TEXT("success") && Entry.Kind != TEXT("note"))) return Fail(TEXT("存档日志无效"));
        PreviousLogId = Entry.Id;
    }
    if (R.SynthesisHistory.Num() > 2000 || R.BattlesWon.Num() > 2000) return Fail(TEXT("存档历史过大"));
    for (const auto& Record : R.SynthesisHistory)
    {
        if (!Ids.Contains(Record.A) || !Ids.Contains(Record.B) || Record.A == Record.B || !Catalog.Nodes.Contains(Record.Node) || !Catalog.Cards.Contains(Record.Selected) || Catalog.Cards.FindChecked(Record.Selected).Family != TEXT("C") || (Record.bSuccess ? !Ids.Contains(Record.CreatedId) : !Record.CreatedId.IsNone()) || !NonNegative(Record.Flow) || !NonNegative(Record.Calc) || !NonNegative(Record.SelectionRoll) || Record.SelectionRoll >= 1 || !NonNegative(Record.SuccessRoll) || Record.SuccessRoll >= 1 || !FMath::IsFinite(Record.Field.X) || !FMath::IsFinite(Record.Field.Y)) return Fail(TEXT("存档合成历史引用或数值无效"));
        const auto* A = R.Cards.FindByPredicate([&Record](const auto& C) { return C.Id == Record.A; });
        const auto* B = R.Cards.FindByPredicate([&Record](const auto& C) { return C.Id == Record.B; });
        if (!A || !B || A->Zone != TEXT("discard") || B->Zone != TEXT("discard") || Catalog.Cards.FindChecked(A->DefId).Family != TEXT("A") || Catalog.Cards.FindChecked(B->DefId).Family != TEXT("B")) return Fail(TEXT("存档合成材料不一致"));
        if (Record.bSuccess)
        {
            const auto* Created = R.Cards.FindByPredicate([&Record](const auto& C) { return C.Id == Record.CreatedId; });
            if (!Created || Created->DefId != Record.Selected) return Fail(TEXT("存档合成产物不一致"));
        }
        TSet<FName> Candidates;
        double ProbabilitySum = 0;
        bool bHasSelected = false;
        for (const auto& C : Record.Candidates)
        {
            if (!Catalog.Cards.Contains(C.Id) || Catalog.Cards.FindChecked(C.Id).Family != TEXT("C") || Candidates.Contains(C.Id) || !NonNegative(C.Score) || !NonNegative(C.Probability) || C.Probability > 1 || !NonNegative(C.Distance) || !FMath::IsFinite(C.Affinity) || !FMath::IsFinite(C.Entropy) || !FMath::IsFinite(C.Faith) || !FMath::IsFinite(C.RawSuccess) || !NonNegative(C.Success) || C.Success > 1) return Fail(TEXT("存档合成候选无效"));
            Candidates.Add(C.Id);
            ProbabilitySum += C.Probability;
            if (C.Id == Record.Selected) { bHasSelected = true; if (Record.bSuccess != (Record.SuccessRoll < C.Success)) return Fail(TEXT("存档合成结果不一致")); }
        }
        if (!bHasSelected || !FMath::IsNearlyEqual(ProbabilitySum, 1.0, 0.000001)) return Fail(TEXT("存档合成候选概率不完整"));
    }
    for (const auto& Record : R.BattlesWon)
        if ((Record.Mode != TEXT("encounter") && Record.Mode != TEXT("act")) || !Catalog.Nodes.Contains(Record.Node) || Record.Actions < 0 || !NonNegative(Record.Seconds)) return Fail(TEXT("存档战果无效"));
    return Ok(TEXT("存档有效"), EPadmaRunCommandEffect::None);
}
FPadmaRunCommandResult FPadmaRunRules::Restore(const FPadmaRunState& Candidate)
{
    if (IsBattle() || IsDialogue()) return Fail(TEXT("请先结束战斗或对话，再读取存档"));
    FPadmaContentSnapshot RestoredContent = Catalog;
    FPadmaRunState Restored = Candidate;
    FString Error;
    if (!Candidate.MapLayout.MapId.IsNone())
    {
        const bool Legacy = Candidate.MapLayout.MapId == TEXT("legacy-html-v1");
        const FPadmaMapLayout* Reference = Legacy ? &Catalog.LegacyMapLayout : &ConfiguredMapLayout;
        FPadmaMapLayout TutorialReference;
        if (!Legacy && Candidate.MapLayout.bHexWilderness
            && (!ConfiguredMapLayout.bHexWilderness || ConfiguredMapLayout.GeneratorVersion != Candidate.MapLayout.GeneratorVersion))
        {
            // Derive compatibility from approved configuration, never from the saved node definitions.
            if (!FPadmaTutorialMapGenerator::Generate(ConfiguredMapLayout,FPadmaTutorialMapGenerator::DefaultSeed,TutorialReference,Error,
                Candidate.MapLayout.GeneratorVersion))
                return Fail(TEXT("无法从已配置地图验证教程存档：") + Error);
            Reference = &TutorialReference;
        }
        if (Reference->MapId.IsNone()) return Fail(TEXT("没有此存档所需的可信地图配置引用"));
        if (!FPadmaWorldMapGenerator::ValidateGenerated(*Reference, Candidate.MapLayout, Error))
            return Fail(TEXT("存档地图与当前固定锚点不兼容：") + Error);
        if (!FPadmaWorldMapGenerator::ApplyToContent(Candidate.MapLayout, RestoredContent, Error)) return Fail(Error);
    }
    else if (!Catalog.MapLayout.MapId.IsNone())
    {
        if (!FPadmaWorldMapGenerator::ApplyToContent(Catalog.LegacyMapLayout, RestoredContent, Error)) return Fail(TEXT("旧地图存档无法恢复：") + Error);
        Restored.MapLayout = Catalog.LegacyMapLayout;
    }
    if (!RestoredContent.Validate(Error)) return Fail(TEXT("内容表未就绪：") + Error);
    FPadmaRunRules CheckedRules;
    CheckedRules.Initialize(RestoredContent, &ConfiguredMapLayout);
    auto Check = CheckedRules.ValidateState(Restored);
    if (!Check.bOk) return Check;
    Catalog = MoveTemp(RestoredContent);
    return Atomic([&]()
    {
        State = MoveTemp(Restored);
        bHasRun = true;
        Battle = {};
        BattleSnapshot.Reset();
        DialogueState = {};
        return Ok(TEXT("已恢复存档中的固定地图与剧情分支"));
    });
}
