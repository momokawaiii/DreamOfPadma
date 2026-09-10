#include "Core/WorldMap/PadmaTutorialMap.h"
#include "Core/WorldMap/PadmaWorldMapDefinition.h"
#include "Math/RandomStream.h"

namespace
{
const FIntPoint Neighbors[] = {{1,0},{0,1},{-1,1},{-1,0},{0,-1},{1,-1}};
bool Fail(FString& Error, const FString& Message) { Error=Message; return false; }
bool IsOldBranch(FName Id)
{
    const FString Name=Id.ToString();
    return Name.StartsWith(TEXT("generated-b"))||Name.StartsWith(TEXT("path."))||Name.StartsWith(TEXT("wild."));
}
TSet<FName> ReachWithoutFlag(const FPadmaMapLayout& Layout, FName BlockedFlag, bool bOpenOnly=false)
{
    TSet<FName> Seen={Layout.HomeNode}; TArray<FName> Queue={Layout.HomeNode};
    for (int32 I=0; I<Queue.Num(); ++I)
        for (const auto& Edge:Layout.Edges)
            if (Edge.From==Queue[I] && (BlockedFlag.IsNone() || Edge.UnlockFlag!=BlockedFlag)
                && (!bOpenOnly || Edge.UnlockFlag.IsNone()) && !Seen.Contains(Edge.To))
            { Seen.Add(Edge.To); Queue.Add(Edge.To); }
    return Seen;
}
double PaintingDistance(const FVector2D& A, const FVector2D& B, const FPadmaHexGridConfig& Grid)
{
    const FVector2D Delta=A-B;
    return Delta.X*Delta.X+FMath::Square(Delta.Y/Grid.ProjectionY);
}
TMap<FName,FString> BuildLockPartitions(const FPadmaMapLayout& Layout)
{
    // Minimal sets of flags needed by an authored home-to-node path. Equal sets of
    // alternatives mean equal availability for every possible flag combination, including OR gates.
    TMap<FName,TArray<TArray<FName>>> Requirements; TMap<FName,int32> Indegree;
    for (const auto& Node:Layout.Nodes) Indegree.Add(Node.Id,0);
    for (const auto& Edge:Layout.Edges) ++Indegree.FindChecked(Edge.To);
    Requirements.FindOrAdd(Layout.HomeNode).Add(TArray<FName>());
    TArray<FName> Queue;
    for (const auto& Node:Layout.Nodes) if (Indegree.FindChecked(Node.Id)==0) Queue.Add(Node.Id);
    auto Subset=[](const TArray<FName>& A,const TArray<FName>& B)
    { for (FName Flag:A) if (!B.Contains(Flag)) return false; return true; };
    for (int32 I=0; I<Queue.Num(); ++I)
        for (const auto& Edge:Layout.Edges)
        {
            if (Edge.From!=Queue[I]) continue;
            auto& Destination=Requirements.FindOrAdd(Edge.To);
            const auto Paths=Requirements.FindRef(Edge.From);
            for (auto Path:Paths)
            {
                if (!Edge.UnlockFlag.IsNone()) Path.AddUnique(Edge.UnlockFlag);
                Path.Sort([](FName A,FName B) { return A.LexicalLess(B); });
                if (Destination.ContainsByPredicate([&](const auto& Existing) { return Subset(Existing,Path); })) continue;
                Destination.RemoveAll([&](const auto& Existing) { return Subset(Path,Existing); });
                Destination.Add(MoveTemp(Path));
            }
            if (--Indegree.FindChecked(Edge.To)==0) Queue.Add(Edge.To);
        }
    TMap<FName,FString> Result;
    for (const auto& Node:Layout.Nodes)
    {
        TArray<FString> Alternatives;
        for (const auto& Path:Requirements.FindChecked(Node.Id))
        {
            FString Key;
            for (FName Flag:Path) { const FString Name=Flag.ToString().ToLower(); Key+=FString::FromInt(Name.Len())+TEXT(":")+Name; }
            Alternatives.Add(Key);
        }
        Alternatives.Sort(); FString Key;
        for (const FString& Alternative:Alternatives) Key+=FString::FromInt(Alternative.Len())+TEXT(":")+Alternative;
        Result.Add(Node.Id,Key);
    }
    return Result;
}
}

bool FPadmaTutorialMapGenerator::GridForVersion(int32 GeneratorVersion, FPadmaHexGridConfig& Out)
{
    if (GeneratorVersion<1 || GeneratorVersion>CurrentGeneratorVersion) return false;
    FPadmaHexGridConfig Grid;
    if (GeneratorVersion>=2)
    {
        Grid.Radius=10.; Grid.MinQ=-5; Grid.MaxQ=6; Grid.MaxR=9;
    }
    Out=Grid; return true;
}

FVector2D FPadmaTutorialMapGenerator::CellCenter(const FPadmaHexGridConfig& Grid, int32 Q, int32 R)
{
    return FVector2D(Grid.Origin.X+FMath::Sqrt(3.)*Grid.Radius*(Q+R*.5),
        Grid.Origin.Y+1.5*Grid.Radius*Grid.ProjectionY*R);
}

FVector2D FPadmaTutorialMapGenerator::CellCorner(const FPadmaHexGridConfig& Grid, int32 Q, int32 R, int32 CornerIndex)
{
    const double Angle=FMath::DegreesToRadians(30.+60.*CornerIndex);
    return CellCenter(Grid,Q,R)+FVector2D(Grid.Radius*FMath::Cos(Angle),Grid.Radius*Grid.ProjectionY*FMath::Sin(Angle));
}

bool FPadmaTutorialMapGenerator::ContainsCell(const FPadmaHexGridConfig& Grid, int32 Q, int32 R)
{
    if (Q<Grid.MinQ || Q>Grid.MaxQ || R<Grid.MinR || R>Grid.MaxR) return false;
    const FVector2D Center=CellCenter(Grid,Q,R);
    return Center.X>=Grid.MinU && Center.X<=Grid.MaxU && Center.Y>=0. && Center.Y<=100.;
}

bool FPadmaTutorialMapGenerator::ContainsPoint(const FPadmaHexGridConfig& Grid, int32 Q, int32 R, FVector2D PaintingPoint)
{
    if (!ContainsCell(Grid,Q,R) || Grid.ProjectionY<=0.) return false;
    const FVector2D Delta=PaintingPoint-CellCenter(Grid,Q,R);
    const double X=FMath::Abs(Delta.X),Y=FMath::Abs(Delta.Y)/Grid.ProjectionY;
    return X<=FMath::Sqrt(3.)*Grid.Radius*.5 && Y<=Grid.Radius-X/FMath::Sqrt(3.);
}

bool FPadmaTutorialMapGenerator::FixedPosition(FName Id, FVector2D& Out)
{
    if (Id==TEXT("home")) Out=FVector2D(10.5,48.);
    else if (Id==TEXT("fire")) Out=FVector2D(21.4,71.8);
    else if (Id==TEXT("story")) Out=FVector2D(48.8,27.4);
    else if (Id==TEXT("plain")) Out=FVector2D(48.2,51.5);
    else if (Id==TEXT("boss")) Out=FVector2D(85.,15.);
    else if (Id==TEXT("gate")) Out=FVector2D(77.5,41.5);
    else if (Id==TEXT("forge")) Out=FVector2D(76.5,70.5);
    else return false;
    return true;
}

bool FPadmaTutorialMapGenerator::Generate(const FPadmaMapLayout& Source, int32 Seed, FPadmaMapLayout& Out, FString& Error, int32 GeneratorVersion)
{
    Error.Reset();
    FPadmaHexGridConfig Grid;
    if (!GridForVersion(GeneratorVersion,Grid)) return Fail(Error,TEXT("Unsupported tutorial grid generator version."));
    if (!FPadmaWorldMapGenerator::Validate(Source,Error)) return false;
    FPadmaMapLayout Basis=Source;
    Basis.bHexWilderness=false;
    Basis.Nodes.RemoveAll([](const auto& Node) { return IsOldBranch(Node.Id)&&Node.AnchorKind==EPadmaMapAnchorKind::None; });
    Basis.Nodes.Sort([](const auto& A,const auto& B) { return A.Id.LexicalLess(B.Id); });
    TSet<FName> FixedIds;
    for (auto& Node:Basis.Nodes)
    {
        if (!FixedPosition(Node.Id,Node.Position)) return Fail(Error,TEXT("Tutorial has no approved painting position for authored node: ")+Node.Id.ToString());
        if(GeneratorVersion>=3 && Node.Id==TEXT("fire"))
        {
            Node.InitialOwner=TEXT("ruler");Node.EnemyCount=1;
            Node.Description=FText::FromString(TEXT("归处附近的敌方哨站，驻守 1 名守卫。部署存活 C 卡后从归处移动至此，可进入 Encounter。"));
        }
        FixedIds.Add(Node.Id);
    }
    for (FName Id:{FName(TEXT("home")),FName(TEXT("fire")),FName(TEXT("story")),FName(TEXT("plain")),FName(TEXT("boss")),FName(TEXT("gate")),FName(TEXT("forge"))})
        if (!FixedIds.Contains(Id)) return Fail(Error,TEXT("Tutorial requires the existing authored key node: ")+Id.ToString());
    if (Source.HomeNode!=TEXT("home")||Source.BossNode!=TEXT("boss")) return Fail(Error,TEXT("Tutorial terminal identities must be home and boss."));
    Basis.Edges.RemoveAll([&](const auto& Edge) { return !FixedIds.Contains(Edge.From)||!FixedIds.Contains(Edge.To)||Edge.Id.ToString().StartsWith(TEXT("hex.")); });
    Basis.Edges.Sort([](const auto& A,const auto& B) { return A.Id.LexicalLess(B.Id); });
    if (!FPadmaWorldMapGenerator::Validate(Basis,Error)) return false;

    // Wilderness connects only nodes with the same authored unlock availability.
    TArray<FName> Flags;
    for (const auto& Edge:Basis.Edges) if (!Edge.UnlockFlag.IsNone()) Flags.AddUnique(Edge.UnlockFlag);
    Flags.Sort([](FName A,FName B) { return A.LexicalLess(B); });
    const TMap<FName,FString> FixedPartitions=BuildLockPartitions(Basis);

    FPadmaMapLayout Candidate=Basis;
    Candidate.MapId=TEXT("tutorial-painted-v1"); Candidate.bHexWilderness=true;
    Candidate.Seed=Seed; Candidate.Version=1; Candidate.GeneratorVersion=GeneratorVersion; Candidate.HexGrid=Grid;
    Candidate.Nodes.Reset(); Candidate.Edges.Reset();
    TMap<FIntPoint,int32> CellIndices;
    TArray<FString> Partitions;
    for (int32 R=Grid.MinR; R<=Grid.MaxR; ++R)
    {
        for (int32 Q=Grid.MinQ; Q<=Grid.MaxQ; ++Q)
        {
            if (!ContainsCell(Grid,Q,R)) continue;
            const FVector2D UV=CellCenter(Grid,Q,R);
            FPadmaWorldNodeDefinitionRow Node;
            Node.Id=FName(*FString::Printf(TEXT("wild.q%+03d.r%02d"),Q,R));
            Node.Position=UV; Node.HexQ=Q; Node.HexR=R; Node.bWilderness=true;
            Node.Type=TEXT("wilderness"); Node.InitialOwner=TEXT("neutral"); Node.EnemyCount=0;
            FRandomStream Random(int32(uint32(Seed)^uint32(Q)*73856093u^uint32(R)*19349663u));
            const double Variation=Random.FRand();
            const double RiverY=82.+FMath::Sin(UV.X*.13)*3.;
            if (UV.Y<9. || UV.X>91. || (UV.X<9. && UV.Y<36.))
            {
                Node.Terrain=TEXT("山地"); Node.Elevation=.78+Variation*.2;
                Node.bHabitable=false; Node.bTraversable=false;
            }
            else if (UV.X>25.&&UV.X<79.&&FMath::Abs(UV.Y-RiverY)<3.7)
            {
                Node.Terrain=TEXT("水域"); Node.Elevation=.03;
                Node.bHabitable=false; Node.bTraversable=false;
            }
            else if (UV.X<44.||UV.Y>67.)
            {
                Node.Terrain=Variation>.43?FName(TEXT("林地")):FName(TEXT("平原")); Node.Elevation=.12+Variation*.22;
            }
            else
            {
                Node.Terrain=Variation>.73?FName(TEXT("丘陵")):FName(TEXT("荒原")); Node.Elevation=.22+Variation*.25;
            }
            Node.DisplayName=FText::FromString(Node.Terrain.ToString()+TEXT(" · ")+FString::FromInt(Candidate.Nodes.Num()+1));
            Node.Description=FText::FromString(Node.bHabitable?TEXT("可居住荒野；按明确相邻连接移动并占领，荒野不产生战局天平奖励。"):
                TEXT("不可居住地形；不能移动、部署或占领。"));
            Node.Provenance=TEXT("TASK-051 deterministic tutorial hex wilderness");
            const FPadmaWorldNodeDefinitionRow* Nearest=nullptr; double Distance=TNumericLimits<double>::Max();
            for (const auto& Fixed:Basis.Nodes)
            {
                const double D=PaintingDistance(UV,Fixed.Position,Grid);
                if (D<Distance) { Distance=D; Nearest=&Fixed; }
            }
            Partitions.Add(FixedPartitions.FindChecked(Nearest->Id));
            CellIndices.Add(FIntPoint(Q,R),Candidate.Nodes.Num()); Candidate.Nodes.Add(MoveTemp(Node));
        }
    }

    TSet<int32> Reserved;
    for (const auto& Fixed:Basis.Nodes)
    {
        int32 Best=INDEX_NONE; double Distance=TNumericLimits<double>::Max();
        for (int32 I=0; I<Candidate.Nodes.Num(); ++I)
        {
            if (Reserved.Contains(I)) continue;
            const double D=PaintingDistance(Fixed.Position,Candidate.Nodes[I].Position,Grid);
            if (D<Distance) { Distance=D; Best=I; }
        }
        if (Best==INDEX_NONE) return Fail(Error,TEXT("No free hex cell for tutorial landmark."));
        const int32 Q=Candidate.Nodes[Best].HexQ,R=Candidate.Nodes[Best].HexR;
        Candidate.Nodes[Best]=Fixed;
        auto& Node=Candidate.Nodes[Best]; Node.HexQ=Q; Node.HexR=R; Node.bWilderness=false;
        Node.bHabitable=true; Node.bTraversable=true; Node.Elevation=.2;
        Partitions[Best]=FixedPartitions.FindChecked(Fixed.Id); Reserved.Add(Best);
    }

    // Multi-source routing within each lock partition repairs only corridors needed to join
    // habitable cells to their authored region. A ford/pass becomes ordinary traversable ground.
    TArray<int32> Parent; Parent.Init(INDEX_NONE,Candidate.Nodes.Num()); TArray<int32> Queue;
    for (int32 I=0; I<Candidate.Nodes.Num(); ++I) if (Reserved.Contains(I)) { Parent[I]=I; Queue.Add(I); }
    for (int32 I=0; I<Queue.Num(); ++I)
    {
        const int32 Current=Queue[I]; const auto& Node=Candidate.Nodes[Current];
        for (const FIntPoint Delta:Neighbors)
        {
            const int32* Next=CellIndices.Find(FIntPoint(Node.HexQ,Node.HexR)+Delta);
            if (Next && Parent[*Next]==INDEX_NONE && Partitions[*Next]==Partitions[Current])
            { Parent[*Next]=Current; Queue.Add(*Next); }
        }
    }
    for (int32 I=0; I<Candidate.Nodes.Num(); ++I)
    {
        auto& Node=Candidate.Nodes[I];
        if (Parent[I]==INDEX_NONE)
        {
            Node.bHabitable=false; Node.bTraversable=false; Node.Terrain=TEXT("山地"); Node.Elevation=.85;
            Node.Description=FText::FromString(TEXT("封闭山地；不生成跨越剧情分区的道路。"));
            continue;
        }
        if (!Node.bHabitable) continue;
        int32 Current=I;
        while (Parent[Current]!=Current)
        {
            auto& Corridor=Candidate.Nodes[Current];
            if (!Corridor.bTraversable)
            {
                Corridor.bTraversable=true; Corridor.bHabitable=true; Corridor.Terrain=TEXT("平原"); Corridor.Elevation=.16;
                Corridor.DisplayName=FText::FromString(TEXT("山河通道"));
                Corridor.Description=FText::FromString(TEXT("连通本剧情分区的可居住通道；与普通荒野使用相同移动和占领规则。"));
            }
            Current=Parent[Current];
        }
    }

    TSet<FString> Pairs;
    for (const auto& Edge:Basis.Edges)
    { Candidate.Edges.Add(Edge); Pairs.Add(Edge.From.ToString()+TEXT(">")+Edge.To.ToString()); }
    for (int32 I=0; I<Candidate.Nodes.Num(); ++I)
    {
        const auto& A=Candidate.Nodes[I]; if (!A.bTraversable) continue;
        for (const FIntPoint Delta:Neighbors)
        {
            const int32* Next=CellIndices.Find(FIntPoint(A.HexQ,A.HexR)+Delta);
            if (!Next||!Candidate.Nodes[*Next].bTraversable||Partitions[I]!=Partitions[*Next]) continue;
            const auto& B=Candidate.Nodes[*Next]; const FString Pair=A.Id.ToString()+TEXT(">")+B.Id.ToString();
            if (Pairs.Contains(Pair)) continue;
            FPadmaWorldEdgeDefinitionRow Edge;
            Edge.Id=FName(*(TEXT("hex.")+A.Id.ToString()+TEXT(".")+B.Id.ToString())); Edge.From=A.Id; Edge.To=B.Id;
            Edge.Provenance=TEXT("TASK-051 explicit adjacent tutorial hexes within authored lock partition");
            Candidate.Edges.Add(Edge); Pairs.Add(Pair);
        }
    }
    Candidate.Nodes.Sort([](const auto& A,const auto& B) { return A.Id.LexicalLess(B.Id); });
    Candidate.Edges.Sort([](const auto& A,const auto& B) { return A.Id.LexicalLess(B.Id); });
    Candidate.Checkpoints.Sort([](const auto& A,const auto& B) { return A.Id.LexicalLess(B.Id); });
    const auto OpenBefore=ReachWithoutFlag(Basis,NAME_None,true),OpenAfter=ReachWithoutFlag(Candidate,NAME_None,true);
    for (FName Id:FixedIds)
        if (!OpenBefore.Contains(Id)&&OpenAfter.Contains(Id)) return Fail(Error,TEXT("Tutorial wilderness bypasses initially locked authored routes."));
    for (FName Flag:Flags)
    {
        const auto Before=ReachWithoutFlag(Basis,Flag),After=ReachWithoutFlag(Candidate,Flag);
        for (FName Id:FixedIds)
            if (!Before.Contains(Id)&&After.Contains(Id)) return Fail(Error,TEXT("Tutorial wilderness bypasses authored flag cut: ")+Flag.ToString());
    }
    if (!FPadmaWorldMapGenerator::Validate(Candidate,Error)) return false;
    Out=MoveTemp(Candidate); return true;
}
