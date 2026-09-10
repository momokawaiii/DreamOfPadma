#include "Game/WorldMap/PadmaMapAuthoringPreview.h"
#include "Game/WorldMap/PadmaWorldMapAsset.h"
#include "World/Map/PadmaWorldMapActor.h"
#include "World/Map/PadmaMapVisualTheme.h"
#include "Components/ArrowComponent.h"
#include "Engine/World.h"

APadmaMapNodeHandle::APadmaMapNodeHandle()
{
    auto* Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("NodeHandle"));
    SetRootComponent(Arrow); Arrow->ArrowColor = FColor(255,210,95); Arrow->ArrowSize = 2;
    Arrow->SetRelativeRotation(FRotator(-90,0,0));
#if WITH_EDITORONLY_DATA
    bIsEditorOnlyActor = true;
#endif
}
APadmaMapAuthoringPreview::APadmaMapAuthoringPreview()
{
    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
#if WITH_EDITORONLY_DATA
    bIsEditorOnlyActor = true;
#endif
}
void APadmaMapAuthoringPreview::ClearPreview()
{
    for (auto Handle : Handles) if (IsValid(Handle)) Handle->Destroy();
    Handles.Reset();
    if (IsValid(View)) View->Destroy();
    View = nullptr;
}
void APadmaMapAuthoringPreview::Destroyed() { ClearPreview(); Super::Destroyed(); }
void APadmaMapAuthoringPreview::RebuildPreview()
{
#if WITH_EDITOR
    if (!Map || !GetWorld() || GetWorld()->IsGameWorld()) { Status=TEXT("Select a map asset in an editor world."); return; }
    const auto& Layout = bEditGeneratedLayout ? Map->GeneratedLayout : Map->Template;
    if (!FPadmaWorldMapGenerator::Validate(Layout, Status)) return;
    ClearPreview();
    FActorSpawnParameters Spawn; Spawn.ObjectFlags = RF_Transient;
    View = GetWorld()->SpawnActor<APadmaWorldMapActor>(Spawn);
    if (!View) { Status=TEXT("Cannot create preview."); return; }
    View->SetActorLabel(TEXT("Map preview (transient)"));
    View->ConfigureTheme(Theme, Layout.Seed);
    TArray<FPadmaWorldNodeView> Nodes;
    TArray<FPadmaWorldEdgeView> Edges;
    for (const auto& Node : Layout.Nodes)
    {
        FPadmaWorldNodeView Value;
        Value.Id=Node.Id;Value.Name=Node.DisplayName;Value.Position=APadmaWorldMapActor::NodeLocation(Node.Position);
        Value.Terrain=Node.Terrain;Value.Type=Node.Type;Value.Owner=Node.InitialOwner;Value.AnchorKind=Node.AnchorKind;Value.NPCId=Node.NPCId;Value.EnemyCount=Node.EnemyCount;
        Nodes.Add(Value);
        auto* Handle=GetWorld()->SpawnActor<APadmaMapNodeHandle>(Value.Position+FVector(0,0,180),FRotator::ZeroRotator,Spawn);
        if(Handle){Handle->NodeId=Node.Id;Handle->SetActorLabel(TEXT("Edit node: ")+Node.Id.ToString());Handles.Add(Handle);}
    }
    for(const auto& Edge:Layout.Edges) Edges.Add({Edge.From,Edge.To,Edge.UnlockFlag.IsNone()});
    View->ShowGraph(Nodes,Edges,NAME_None);
    Status=TEXT("Move the yellow node handles. Apply Node Transforms validates before changing the asset; save it explicitly.");
#endif
}
void APadmaMapAuthoringPreview::ApplyNodeTransforms()
{
#if WITH_EDITOR
    if(!Map || !GetWorld() || GetWorld()->IsGameWorld())return;
    auto Candidate=bEditGeneratedLayout?Map->GeneratedLayout:Map->Template;
    if(Handles.Num()!=Candidate.Nodes.Num()){Status=TEXT("Rebuild preview before applying edits.");return;}
    for(const auto Handle:Handles)
    {
        if(!IsValid(Handle)){Status=TEXT("Missing handle; rebuild preview.");return;}
        auto* Node=Candidate.Nodes.FindByPredicate([&](const auto& N){return N.Id==Handle->NodeId;});
        if(!Node){Status=TEXT("Handle identity no longer exists.");return;}
        const FVector P=Handle->GetActorLocation();
        Node->Position=FVector2D(P.Y/2.5+150,P.X/2.5+320);
    }
    if(!FPadmaWorldMapGenerator::Validate(Candidate,Status))return;
    if(bEditGeneratedLayout && !FPadmaWorldMapGenerator::ValidateGenerated(Map->Template,Candidate,Status))return;
    Map->Modify();
    if(bEditGeneratedLayout)Map->GeneratedLayout=MoveTemp(Candidate);else Map->Template=MoveTemp(Candidate);
    Map->MarkPackageDirty();
    Status=TEXT("Node positions applied. Save the map Data Asset. Generated-layout editing preserves fixed anchors.");
#endif
}
