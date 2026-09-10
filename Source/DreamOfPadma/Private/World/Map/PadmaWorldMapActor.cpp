#include "World/Map/PadmaWorldMapActor.h"
#include "World/Map/PadmaMapVisualTheme.h"
#include "World/PCG/PadmaDecorationPoints.h"
#include "Presentation/Models/PadmaModelPresentation.h"
#include "ProceduralMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Materials/MaterialInterface.h"
#include "Math/RandomStream.h"
#include "Misc/SecureHash.h"
#include "PCGComponent.h"
#include "PCGGraph.h"
#include "PCGNode.h"
#include "Elements/PCGStaticMeshSpawner.h"
#include "MeshSelectors/PCGMeshSelectorWeighted.h"

namespace
{
struct FMapMesh
{
    TArray<FVector> Vertices, Normals;
    TArray<int32> Triangles;
    TArray<FVector2D> UV;
    TArray<FLinearColor> Colors;
    TArray<FProcMeshTangent> Tangents;
    void Triangle(const FVector& A, const FVector& B, const FVector& C, FLinearColor Color)
    {
        const int32 Start = Vertices.Num();
        const FVector Normal = FVector::CrossProduct(B-A, C-A).GetSafeNormal();
        // UE procedural faces use clockwise front winding (see GenerateBoxMesh).
        // Keep the authored outward normal and reverse indices to match it;
        // otherwise a two-sided material flips top-face lighting toward the ground.
        Vertices.Append({A,B,C}); Triangles.Append({Start,Start+2,Start+1});
        for (int32 I=0; I<3; ++I) { Normals.Add(Normal); Colors.Add(Color); UV.Add(FVector2D(0,0)); }
    }
    void Quad(const FVector& A, const FVector& B, const FVector& C, const FVector& D, FLinearColor Color)
    { Triangle(A,B,C,Color); Triangle(A,C,D,Color); }
    void Prism(FVector Center, double Radius, double Bottom, double Top, FLinearColor Color, int32 Sides=6)
    {
        for (int32 I=0; I<Sides; ++I)
        {
            const double A=2*PI*I/Sides, B=2*PI*(I+1)/Sides;
            const FVector P=Center+FVector(FMath::Cos(A)*Radius,FMath::Sin(A)*Radius,Top);
            const FVector Q=Center+FVector(FMath::Cos(B)*Radius,FMath::Sin(B)*Radius,Top);
            Triangle(Center+FVector(0,0,Top),P,Q,Color);
            Quad(P,Center+FVector(P.X-Center.X,P.Y-Center.Y,Bottom),Center+FVector(Q.X-Center.X,Q.Y-Center.Y,Bottom),Q,Color*.64f);
        }
    }
    void Pyramid(FVector Center, double Radius, double Height, FLinearColor Color, int32 Sides=6)
    {
        for (int32 I=0; I<Sides; ++I)
        {
            const double A=2*PI*I/Sides, B=2*PI*(I+1)/Sides;
            const FVector P=Center+FVector(FMath::Cos(A)*Radius,FMath::Sin(A)*Radius,0);
            const FVector Q=Center+FVector(FMath::Cos(B)*Radius,FMath::Sin(B)*Radius,0);
            Triangle(Height>=0?P:Q,Height>=0?Q:P,Center+FVector(0,0,Height),Color*(I%2?.88f:1.f));
        }
    }
    void Box(FVector Center, FVector Half, FLinearColor Color)
    {
        const FVector A=Center+FVector(-Half.X,-Half.Y,-Half.Z), B=Center+FVector(Half.X,-Half.Y,-Half.Z);
        const FVector C=Center+FVector(Half.X,Half.Y,-Half.Z), D=Center+FVector(-Half.X,Half.Y,-Half.Z);
        const FVector H(0,0,2*Half.Z);
        Quad(A+H,B+H,C+H,D+H,Color); Quad(A,B,B+H,A+H,Color*.72f);
        Quad(B,C,C+H,B+H,Color*.82f); Quad(C,D,D+H,C+H,Color*.64f); Quad(D,A,A+H,D+H,Color*.76f);
    }
    void Ring(double Radius, double Width, double Height, FLinearColor Color)
    {
        for (int32 I=0; I<6; ++I)
        {
            const double A=PI/3*I, B=PI/3*(I+1);
            const FVector P(FMath::Cos(A),FMath::Sin(A),0), Q(FMath::Cos(B),FMath::Sin(B),0), Z(0,0,Height);
            Quad(P*Radius+Z,Q*Radius+Z,Q*(Radius-Width)+Z,P*(Radius-Width)+Z,Color);
        }
    }
    void Commit(UProceduralMeshComponent* Component, UMaterialInterface* Material, bool bCollision=false)
    {
        if (Vertices.IsEmpty()) { Component->ClearAllMeshSections(); return; }
        // VertexColor material inputs are linear. Do not gamma-encode them while packing FColor.
        Component->CreateMeshSection_LinearColor(0,Vertices,Triangles,Normals,UV,Colors,Tangents,bCollision,false);
        if (Material) Component->SetMaterial(0,Material);
    }
};

UMaterialInterface* VertexMaterial(const UPadmaMapVisualTheme* Theme)
{
    if (Theme && !Theme->VertexColorMaterial.IsNull())
        if (UMaterialInterface* Material=Theme->VertexColorMaterial.LoadSynchronous()) return Material;
    return LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Padma/MVP/Playable/Presentation/M_PadmaVertexColor.M_PadmaVertexColor"));
}
const UPadmaMapVisualTheme* ThemeOrDefault(const UPadmaMapVisualTheme* Theme)
{ return Theme ? Theme : GetDefault<UPadmaMapVisualTheme>(); }
FLinearColor OwnerColor(FName Owner)
{
    if (Owner==TEXT("player")) return FLinearColor(.16f,.78f,.84f);
    if (Owner==TEXT("ruler")) return FLinearColor(.83f,.3f,.24f);
    return FLinearColor(.66f,.66f,.49f);
}
void Tree(FMapMesh& Mesh,FVector P,double Size)
{
    Mesh.Prism(P,6*Size,0,33*Size,FLinearColor(.28f,.2f,.12f));
    Mesh.Pyramid(P+FVector(0,0,15*Size),28*Size,65*Size,FLinearColor(.08f,.23f,.15f));
    Mesh.Pyramid(P+FVector(0,0,42*Size),20*Size,42*Size,FLinearColor(.13f,.34f,.22f));
}
FString MakeLayoutKey(const TArray<FPadmaWorldNodeView>& Nodes,const TArray<FPadmaWorldEdgeView>& Edges,const UPadmaMapVisualTheme* Theme,int32 Seed)
{
    TArray<FString> Parts;
    for (const auto& Node:Nodes)
        Parts.Add(FString::Printf(TEXT("N:%s:%s:%s:%.17g,%.17g,%.17g:%d:%s"),*Node.Id.ToString(),*Node.Terrain.ToString(),*Node.Type.ToString(),
            Node.Position.X,Node.Position.Y,Node.Position.Z,int32(Node.AnchorKind),*Node.NPCId.ToString()));
    for (const auto& Edge:Edges) Parts.Add(TEXT("E:")+Edge.From.ToString()+TEXT(":")+Edge.To.ToString());
    Parts.Sort();
    const auto* T=ThemeOrDefault(Theme);
    Parts.Add(FString::Printf(TEXT("T:%s:%d:%.9g:%.9g:%d:%.9g:%.9g:%s:%s"),*T->GetPathName(),Seed,T->HexRadius,T->FieldMargin,T->DecorationCount,
        T->DecorationScaleMin,T->DecorationScaleMax,*T->VertexColorMaterial.ToString(),*T->DecorationMaterial.ToString()));
    Parts.Add(FString::Printf(TEXT("L:%.9g:%.9g:%.9g:%d:%.9g:%s"),T->SurfaceGridSize,T->HillRelief,T->MountainRelief,
        T->bShowRiver,T->DecorationMaxSlopeDegrees,*T->WaterMaterial.ToString()));
    for (const auto& Terrain:T->TerrainVisuals)
        Parts.Add(Terrain.TerrainId.ToString()+Terrain.Color.ToString()+FString::SanitizeFloat(Terrain.Height)+Terrain.GroundMaterial.ToString()+Terrain.GroundMesh.ToString());
    for (const auto& Binding:T->NodeBindings)
        Parts.Add(Binding.NodeId.ToString()+Binding.GroundModel.ToString()+Binding.BuildingModel.ToString()+Binding.NPCModel.ToString());
    for (const auto& Mesh:T->DecorationMeshes) Parts.Add(Mesh.ToString());
    const FTCHARToUTF8 Utf8(*FString::Join(Parts,TEXT("|")));
    FMD5 Hash; Hash.Update(reinterpret_cast<const uint8*>(Utf8.Get()),Utf8.Length());
    uint8 Digest[16]; Hash.Final(Digest);
    return BytesToHex(Digest,UE_ARRAY_COUNT(Digest));
}
}

APadmaWorldNodeActor::APadmaWorldNodeActor()
{
    PrimaryActorTick.bCanEverTick=false;
    Tile=CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("HexTerrain")); SetRootComponent(Tile);
    Tile->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Tile->SetCollisionResponseToAllChannels(ECR_Ignore); Tile->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
    Feedback=CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("SelectionRim")); Feedback->SetupAttachment(Tile);
    Feedback->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Markers=CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GarrisonAndNPC")); Markers->SetupAttachment(Tile);
    Markers->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Caption=CreateDefaultSubobject<UTextRenderComponent>(TEXT("Name")); Caption->SetupAttachment(Tile); Caption->SetVisibility(false);
    Detail=CreateDefaultSubobject<UTextRenderComponent>(TEXT("Occupancy")); Detail->SetupAttachment(Tile); Detail->SetVisibility(false);
}

void APadmaWorldNodeActor::BuildStatic(const FPadmaWorldNodeView& View,UPadmaMapVisualTheme* Theme)
{
    NodeId=View.Id; MarkerKey.Reset(); FeedbackKey.Reset();
    const auto* T=ThemeOrDefault(Theme);
    const auto Terrain=T->ResolveTerrain(View.Terrain);
    const double Radius=FMath::Clamp(T->HexRadius,60.,200.);
    FVector Location=View.Position;
    const auto* Map=Cast<APadmaWorldMapActor>(GetOwner());
    Location.Z=Map?Map->SampleSurfaceHeight(FVector2D(View.Position)):FMath::Clamp(Terrain.Height,0.,180.)+View.Position.Z;
    SetActorLocation(Location);
    SurfaceHeight=.6;
    FMapMesh Geometry;
    // A flush query surface keeps node picking independent of decorative terrain and PCG.
    // There are no elevated hexagonal terrain walls beneath the landmarks.
    for (int32 I=0; I<6; ++I)
    {
        const double A=PI/3*I, B=PI/3*(I+1);
        Geometry.Triangle(FVector(0,0,SurfaceHeight),FVector(FMath::Cos(A)*Radius,FMath::Sin(A)*Radius,SurfaceHeight),
            FVector(FMath::Cos(B)*Radius,FMath::Sin(B)*Radius,SurfaceHeight),Terrain.Color);
    }
    if (Map)
        for (int32 I=0; I<Geometry.Vertices.Num(); ++I)
            Geometry.Colors[I]=Map->SampleSurfaceColor(FVector2D(Geometry.Vertices[I]+Location));
    const auto* Binding=T->FindBinding(View.Id);
    auto AddModel=[&](const TSoftObjectPtr<UPadmaModelDefinition>& Reference,FVector Offset)->bool
    {
        UPadmaModelDefinition* Definition=Reference.LoadSynchronous();
        if (!Definition) return false;
        FPadmaResolvedPresentation Resolved;
        Resolved.StaticModel=Definition->StaticModel.LoadSynchronous(); Resolved.SkeletalModel=Definition->SkeletalModel.LoadSynchronous();
        Resolved.AnimationClass=Definition->AnimationClass.LoadSynchronous(); Resolved.ModelTransform=Definition->ModelTransform;
        Resolved.DefinitionId=Definition->VisualDefinitionId; Resolved.bVisualReady=bool(Resolved.StaticModel)||bool(Resolved.SkeletalModel);
        if (!Resolved.bVisualReady) return false;
        FActorSpawnParameters SpawnParameters;
        SpawnParameters.Owner=this;
        if (HasAnyFlags(RF_Transient)) SpawnParameters.ObjectFlags|=RF_Transient;
        auto* Actor=GetWorld()->SpawnActor<APadmaModelPresentationActor>(SpawnParameters);
        if (!Actor) return false;
        Actor->AttachToActor(this,FAttachmentTransformRules::KeepRelativeTransform); Actor->SetActorRelativeLocation(Offset);
        Actor->ApplyPresentation(Resolved); Actor->SetActorEnableCollision(false); Models.Add(Actor); return true;
    };
    bool bBuilding=false;
    if (Binding)
    {
        AddModel(Binding->GroundModel,FVector(0,0,SurfaceHeight));
        bBuilding=AddModel(Binding->BuildingModel,FVector(0,0,SurfaceHeight));
        AddModel(Binding->NPCModel,FVector(-40,25,SurfaceHeight));
    }
    if (!Terrain.GroundMesh.IsNull())
    {
        auto* Mesh=NewObject<UStaticMeshComponent>(this); Mesh->SetStaticMesh(Terrain.GroundMesh.LoadSynchronous());
        Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); Mesh->SetupAttachment(Tile); Mesh->RegisterComponent();
        Mesh->SetRelativeLocation(FVector(0,0,SurfaceHeight));
        if (!Terrain.GroundMaterial.IsNull()) Mesh->SetMaterial(0,Terrain.GroundMaterial.LoadSynchronous());
    }
    const FVector Center(0,0,SurfaceHeight);
    if (!bBuilding)
    {
        if (View.Type==TEXT("home")||View.Type==TEXT("boss")||View.Type==TEXT("core")||View.Type==TEXT("rulerCore"))
        {
            const FLinearColor Stone=(View.Type==TEXT("home")||View.Type==TEXT("core"))?FLinearColor(.67f,.66f,.49f):FLinearColor(.43f,.4f,.43f);
            Geometry.Prism(Center,57,0,13,Stone);
            Geometry.Box(Center+FVector(0,0,37),FVector(34,34,24),Stone);
            Geometry.Pyramid(Center+FVector(0,0,61),58,38,FLinearColor(.2f,.32f,.31f),4);
            for (double Side:{-1.,1.})
            {
                Geometry.Box(Center+FVector(0,Side*57,25),FVector(18,18,25),Stone*.9f);
                Geometry.Pyramid(Center+FVector(0,Side*57,50),29,25,FLinearColor(.18f,.29f,.28f),4);
            }
        }
        else if (View.Type==TEXT("gate"))
        {
            for (double Side:{-1.,1.}) Geometry.Box(Center+FVector(0,Side*42,40),FVector(25,19,40),FLinearColor(.43f,.48f,.46f));
            Geometry.Box(Center+FVector(0,0,84),FVector(29,67,10),FLinearColor(.25f,.34f,.32f));
        }
        else if (View.Type==TEXT("forge"))
        {
            Geometry.Box(Center+FVector(0,0,24),FVector(43,35,24),FLinearColor(.39f,.36f,.31f));
            Geometry.Prism(Center+FVector(20,-12,0),15,45,108,FLinearColor(.3f,.31f,.3f));
            Geometry.Pyramid(Center+FVector(-20,0,48),32,24,FLinearColor(.69f,.36f,.17f),4);
        }
        else if (View.Type==TEXT("fire"))
        {
            Geometry.Prism(Center,42,0,13,FLinearColor(.39f,.38f,.32f));
            Geometry.Pyramid(Center+FVector(0,0,13),20,60,FLinearColor(.95f,.49f,.13f),5);
        }
        else if (View.Terrain==TEXT("forest")||View.Terrain==TEXT("林地"))
        {
            Tree(Geometry,Center+FVector(-35,-24,0),.85); Tree(Geometry,Center+FVector(26,25,0),1.05); Tree(Geometry,Center+FVector(34,-31,0),.7);
        }
        else if (View.Terrain==TEXT("mountain")||View.Terrain==TEXT("hill"))
        {
            Geometry.Pyramid(Center+FVector(-18,-16,0),55,97,FLinearColor(.46f,.49f,.48f),5);
            Geometry.Pyramid(Center+FVector(30,22,0),38,62,FLinearColor(.57f,.6f,.57f),5);
        }
        else if (View.Type!=TEXT("story"))
        {
            Geometry.Pyramid(Center+FVector(-26,8,0),24,27,FLinearColor(.38f,.43f,.37f),5);
            Geometry.Pyramid(Center+FVector(28,-9,0),16,20,FLinearColor(.43f,.48f,.4f),5);
        }
    }
    Geometry.Commit(Tile,Terrain.GroundMaterial.IsNull()?VertexMaterial(T):Terrain.GroundMaterial.LoadSynchronous(),true);
}

void APadmaWorldNodeActor::SetView(const FPadmaWorldNodeView& View,bool bSelected,bool bHovered,UPadmaMapVisualTheme* Theme)
{
    Caption->SetText(View.Name); Detail->SetText(FText::FromString(View.Detail));
    const auto* T=ThemeOrDefault(Theme);
    const double Radius=FMath::Clamp(T->HexRadius,60.,200.);
    const FString NewFeedback=FString::Printf(TEXT("%d:%d:%s:%s:%s"),bSelected,bHovered,*View.Owner.ToString(),*T->SelectedColor.ToString(),*T->HoverColor.ToString());
    if (NewFeedback!=FeedbackKey)
    {
        FeedbackKey=NewFeedback; FMapMesh Mesh;
        Mesh.Ring(Radius+2,bSelected?8:bHovered?6:3,SurfaceHeight+1,bSelected?T->SelectedColor:bHovered?T->HoverColor:OwnerColor(View.Owner)*.72f);
        if (const auto* Map=Cast<APadmaWorldMapActor>(GetOwner()))
            for (FVector& Vertex:Mesh.Vertices)
                Vertex.Z=Map->SampleSurfaceHeight(FVector2D(Vertex+GetActorLocation()))-GetActorLocation().Z+1.6;
        Mesh.Commit(Feedback,VertexMaterial(T));
    }
    const FString NewMarkers=FString::Printf(TEXT("%s:%d:%d:%d:%d"),*View.Owner.ToString(),View.FriendlyCount,View.EnemyCount,int32(View.AnchorKind),View.bAnchorCompleted);
    if (NewMarkers==MarkerKey) return;
    MarkerKey=NewMarkers; FMapMesh Mesh;
    for (int32 I=0; I<FMath::Clamp(View.FriendlyCount,0,6); ++I)
    {
        const FVector P(-Radius*.58+(I%3)*20,-Radius*.46+(I/3)*21,SurfaceHeight);
        Mesh.Prism(P,7,0,18,FLinearColor(.13f,.71f,.8f)); Mesh.Pyramid(P+FVector(0,0,18),9,12,FLinearColor(.54f,.92f,.94f),4);
    }
    for (int32 I=0; I<FMath::Clamp(View.EnemyCount,0,6); ++I)
    {
        const FVector P(-Radius*.58+(I%3)*20,Radius*.45-(I/3)*21,SurfaceHeight);
        Mesh.Prism(P,7,0,18,FLinearColor(.68f,.25f,.2f)); Mesh.Pyramid(P+FVector(0,0,18),9,12,FLinearColor(.97f,.58f,.33f),4);
    }
    if (View.AnchorKind!=EPadmaMapAnchorKind::None)
    {
        const FLinearColor Color=View.bAnchorCompleted?FLinearColor(.21f,.84f,.65f):View.AnchorKind==EPadmaMapAnchorKind::MainStory?FLinearColor(1.f,.73f,.25f):FLinearColor(.4f,.82f,.97f);
        const FVector P(-26,10,SurfaceHeight);
        Mesh.Prism(P,10,0,38,FLinearColor(.54f,.49f,.36f));
        Mesh.Pyramid(P+FVector(0,0,38),19,26,Color,4);
        Mesh.Pyramid(P+FVector(0,0,89),14,-19,Color,4);
        Mesh.Pyramid(P+FVector(0,0,89),14,19,Color,4);
    }
    Mesh.Commit(Markers,VertexMaterial(T));
}
void APadmaWorldNodeActor::SetVisible(bool bVisible)
{
    SetActorHiddenInGame(!bVisible); SetActorEnableCollision(bVisible);
    for (APadmaModelPresentationActor* Model:Models) if (IsValid(Model)) Model->SetActorHiddenInGame(!bVisible);
}
void APadmaWorldNodeActor::EndPlay(const EEndPlayReason::Type Reason)
{
    for (APadmaModelPresentationActor* Model:Models) if (IsValid(Model)) Model->Destroy();
    Models.Reset(); Super::EndPlay(Reason);
}
void APadmaWorldNodeActor::Destroyed()
{
    for (APadmaModelPresentationActor* Model:Models) if (IsValid(Model)) Model->Destroy();
    Models.Reset(); Super::Destroyed();
}

APadmaWorldMapActor::APadmaWorldMapActor()
{
    PrimaryActorTick.bCanEverTick=false;
    BoundsVolume=CreateDefaultSubobject<UBoxComponent>(TEXT("MapPCGBounds")); SetRootComponent(BoundsVolume);
    BoundsVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision); BoundsVolume->SetHiddenInGame(true); BoundsVolume->SetBoxExtent(FVector(1000,1000,300));
    TerrainField=CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ContinuousHexField")); TerrainField->SetupAttachment(BoundsVolume); TerrainField->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WaterGeometry=CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("RiverSurface")); WaterGeometry->SetupAttachment(BoundsVolume); WaterGeometry->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RoadGeometry=CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("DirectedRoads")); RoadGeometry->SetupAttachment(BoundsVolume); RoadGeometry->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    DecorationPCG=CreateDefaultSubobject<UPCGComponent>(TEXT("MapDecorationPCG"));
    DecorationPCG->GenerationTrigger=EPCGComponentGenerationTrigger::GenerateOnDemand; DecorationPCG->bIsComponentPartitioned=false;
}
FVector APadmaWorldMapActor::NodeLocation(const FVector2D& Position)
{ return FVector((Position.Y-320)*2.5,(Position.X-150)*2.5,0); }

void APadmaWorldMapActor::ConfigureTheme(UPadmaMapVisualTheme* Theme,int32 Seed)
{
    const bool bChanged=VisualTheme!=Theme||DecorationSeed!=Seed;
    VisualTheme=Theme; DecorationSeed=Seed;
    if (bChanged) LayoutKey.Reset();
}
void APadmaWorldMapActor::ShowGraph(const TArray<FPadmaWorldNodeView>& Nodes,const TArray<FPadmaWorldEdgeView>& Edges,FName Selected)
{
    SelectedNode=Selected; LastNodes=Nodes; LastEdges=Edges;
    const FString NewKey=MakeLayoutKey(Nodes,Edges,VisualTheme,DecorationSeed);
    if (NewKey!=LayoutKey) { LayoutKey=NewKey; RebuildStatic(); }
    RefreshDynamic();
}
void APadmaWorldMapActor::RebuildStatic()
{
    for (auto& Pair:Tiles) if (IsValid(Pair.Value)) Pair.Value->Destroy();
    Tiles.Reset(); RoadKey.Reset(); TerrainField->ClearAllMeshSections(); RoadGeometry->ClearAllMeshSections(); WaterGeometry->ClearAllMeshSections();
    DecorationPCG->CancelGeneration(); DecorationPCG->CleanupLocalImmediate(true);
    GeneratedDecorationInstances=0; TerrainTriangleCount=0; MapBounds=FBox(ForceInit);
    if (LastNodes.IsEmpty()) return;
    const auto* T=ThemeOrDefault(VisualTheme);
    Landscape.Initialize(LastNodes,LastEdges,T,DecorationSeed);
    MapBounds=Landscape.Bounds;
    SetActorLocation(MapBounds.GetCenter()); BoundsVolume->SetBoxExtent(MapBounds.GetExtent());
    // Shared vertices and sampled normals form one continuous surface; bound pathological layouts.
    const double Spacing=FMath::Max(FMath::Clamp(T->SurfaceGridSize,16.,100.),FMath::Max(MapBounds.GetSize().X,MapBounds.GetSize().Y)/220.);
    const int32 Columns=FMath::CeilToInt(MapBounds.GetSize().X/Spacing), Rows=FMath::CeilToInt(MapBounds.GetSize().Y/Spacing);
    FMapMesh Field;
    for (int32 Row=0; Row<=Rows; ++Row)
    {
        for (int32 Column=0; Column<=Columns; ++Column)
        {
            const FVector2D P(FMath::Lerp(MapBounds.Min.X,MapBounds.Max.X,double(Column)/Columns),
                FMath::Lerp(MapBounds.Min.Y,MapBounds.Max.Y,double(Row)/Rows));
            Field.Vertices.Add(FVector(P,Landscape.Height(P))-GetActorLocation());
            const FVector Normal=Landscape.Normal(P);
            Field.Normals.Add(Normal); Field.Colors.Add(Landscape.Color(P)); Field.UV.Add(P/256.);
            Field.Tangents.Add(FProcMeshTangent(FVector(Normal.Z,0,-Normal.X).GetSafeNormal(),false));
            if (Column<Columns&&Row<Rows)
            {
                const int32 A=Row*(Columns+1)+Column, B=A+1, C=A+Columns+1, D=C+1;
                Field.Triangles.Append({A,D,B,A,C,D});
            }
        }
    }
    TerrainTriangleCount=Field.Triangles.Num()/3;
    // A dark earthen edge closes the terrain; it is one perimeter, not individual tile sides.
    auto AddSkirt=[&](const FVector2D& A,const FVector2D& B)
    {
        const FVector PA=FVector(A,Landscape.Height(A))-GetActorLocation(), PB=FVector(B,Landscape.Height(B))-GetActorLocation();
        Field.Quad(PA,FVector(A,-55)-GetActorLocation(),FVector(B,-55)-GetActorLocation(),PB,FLinearColor(.13f,.105f,.063f));
    };
    for (int32 I=0; I<Columns; ++I)
    {
        const double A=FMath::Lerp(MapBounds.Min.X,MapBounds.Max.X,double(I)/Columns),B=FMath::Lerp(MapBounds.Min.X,MapBounds.Max.X,double(I+1)/Columns);
        AddSkirt(FVector2D(A,MapBounds.Min.Y),FVector2D(B,MapBounds.Min.Y));
        AddSkirt(FVector2D(B,MapBounds.Max.Y),FVector2D(A,MapBounds.Max.Y));
    }
    for (int32 I=0; I<Rows; ++I)
    {
        const double A=FMath::Lerp(MapBounds.Min.Y,MapBounds.Max.Y,double(I)/Rows),B=FMath::Lerp(MapBounds.Min.Y,MapBounds.Max.Y,double(I+1)/Rows);
        AddSkirt(FVector2D(MapBounds.Max.X,A),FVector2D(MapBounds.Max.X,B));
        AddSkirt(FVector2D(MapBounds.Min.X,B),FVector2D(MapBounds.Min.X,A));
    }
    // Some auxiliary faces have flat normals and no authored tangent; provide a full buffer.
    while (Field.Tangents.Num()<Field.Vertices.Num()) Field.Tangents.Add(FProcMeshTangent(1,0,0));
    Field.Commit(TerrainField,VertexMaterial(T));
    FMapMesh Water;
    if (Landscape.bRiver)
    {
        const int32 Segments=FMath::CeilToInt(MapBounds.GetSize().Y/18.);
        for (int32 I=0; I<Segments; ++I)
        {
            const double YA=FMath::Lerp(MapBounds.Min.Y,MapBounds.Max.Y,double(I)/Segments),YB=FMath::Lerp(MapBounds.Min.Y,MapBounds.Max.Y,double(I+1)/Segments);
            const double XA=Landscape.RiverCenter(YA),XB=Landscape.RiverCenter(YB),WA=Landscape.RiverHalfWidth(YA),WB=Landscape.RiverHalfWidth(YB);
            auto P=[&](double X,double Y,double Lift=0.) { return FVector(X,Y,FPadmaLandscapeSurface::WaterHeight+Lift)-GetActorLocation(); };
            Water.Quad(P(XA-WA,YA),P(XA+WA,YA),P(XB+WB,YB),P(XB-WB,YB),FLinearColor(.055f,.29f,.3f));
            for (double Side:{-1.,1.})
                Water.Quad(P(XA+Side*(WA-3),YA,.2),P(XA+Side*WA,YA,.2),P(XB+Side*WB,YB,.2),P(XB+Side*(WB-3),YB,.2),FLinearColor(.22f,.44f,.38f));
            if (I%5==0)
                Water.Quad(P(XA-WA*.35,YA,.3),P(XA+WA*.3,YA+1,.3),P(XA+WA*.25,YA+2.4,.3),P(XA-WA*.4,YA+1.4,.3),FLinearColor(.22f,.45f,.43f));
        }
    }
    Water.Commit(WaterGeometry,T->WaterMaterial.IsNull()?VertexMaterial(T):T->WaterMaterial.LoadSynchronous());
    for (const auto& View:LastNodes)
    {
        FActorSpawnParameters SpawnParameters;
        SpawnParameters.Owner=this;
        if (HasAnyFlags(RF_Transient)) SpawnParameters.ObjectFlags|=RF_Transient;
        auto* Tile=GetWorld()->SpawnActor<APadmaWorldNodeActor>(SpawnParameters);
        if (!Tile) continue;
        Tile->AttachToActor(this,FAttachmentTransformRules::KeepWorldTransform);
        Tile->BuildStatic(View,VisualTheme); Tile->SetVisible(bMapVisible); Tiles.Add(View.Id,Tile);
    }
    ++LayoutBuildCount;
    UE_LOG(LogTemp,Display,TEXT("[PadmaMap] layout-build=%d nodes=%d edges=%d terrain-triangles=%d river=%d bounds=%s"),LayoutBuildCount,LastNodes.Num(),LastEdges.Num(),TerrainTriangleCount,Landscape.bRiver,*MapBounds.ToString());
    GenerateDecoration();
}
void APadmaWorldMapActor::RefreshDynamic()
{
    for (const auto& View:LastNodes)
        if (auto* Tile=Tiles.Find(View.Id); Tile&&IsValid(*Tile)) (*Tile)->SetView(View,View.Id==SelectedNode,View.Id==HoveredNode,VisualTheme);
    RebuildRoads();
}
void APadmaWorldMapActor::SetHoveredNode(FName NodeId)
{ if (HoveredNode!=NodeId) { HoveredNode=NodeId; RefreshDynamic(); } }
void APadmaWorldMapActor::SetMoveSource(FName NodeId)
{ if (MoveSource!=NodeId) { MoveSource=NodeId; RoadKey.Reset(); RebuildRoads(); } }
void APadmaWorldMapActor::SetVisible(bool bVisible)
{
    bMapVisible=bVisible; SetActorHiddenInGame(!bVisible);
    for (auto& Pair:Tiles) if (IsValid(Pair.Value)) Pair.Value->SetVisible(bVisible);
}
void APadmaWorldMapActor::RebuildRoads()
{
    FString NewKey=MoveSource.ToString();
    for (const auto& Edge:LastEdges) NewKey+=Edge.From.ToString()+Edge.To.ToString()+(Edge.bUnlocked?TEXT("1"):TEXT("0"));
    if (NewKey==RoadKey) return;
    RoadKey=NewKey; FMapMesh Mesh;
    const double Radius=FMath::Clamp(ThemeOrDefault(VisualTheme)->HexRadius,60.,200.);
    for (const auto& Edge:LastEdges)
    {
        const auto* From=Tiles.Find(Edge.From); const auto* To=Tiles.Find(Edge.To);
        if (!From||!To||!IsValid(*From)||!IsValid(*To)) continue;
        FVector A=(*From)->GetActorLocation(), B=(*To)->GetActorLocation();
        A.Z=0; B.Z=0;
        const FVector Direction=(B-A).GetSafeNormal();
        if (FVector::Dist2D(A,B)<Radius*1.7) continue;
        A+=Direction*(Radius*.92); B-=Direction*(Radius*.92);
        const FVector Side(-Direction.Y,Direction.X,0);
        const bool bActive=Edge.bUnlocked&&Edge.From==MoveSource;
        const FLinearColor Color=bActive?FLinearColor(.22f,.65f,.59f):Edge.bUnlocked?FLinearColor(.43f,.31f,.15f):FLinearColor(.38f,.18f,.10f);
        const double Width=bActive?10:Edge.bUnlocked?8:6;
        auto Ground=[&](FVector P,double Lift=1.7) { P.Z=Landscape.Height(FVector2D(P))+Lift; return P-GetActorLocation(); };
        const int32 Segments=FMath::Max(1,FMath::CeilToInt(FVector::Dist2D(A,B)/20.));
        for (int32 I=0; I<Segments; ++I)
        {
            const FVector P=FMath::Lerp(A,B,double(I)/Segments),Q=FMath::Lerp(A,B,double(I+1)/Segments);
            Mesh.Quad(Ground(P-Side*(Width+4)),Ground(Q-Side*(Width+4)),Ground(Q+Side*(Width+4)),Ground(P+Side*(Width+4)),FLinearColor(.16f,.14f,.065f));
            Mesh.Quad(Ground(P-Side*Width,2),Ground(Q-Side*Width,2),Ground(Q+Side*Width,2),Ground(P+Side*Width,2),Color*(I%3==0?.94f:1.f));
        }
        const FVector Arrow=FMath::Lerp(A,B,.63);
        Mesh.Triangle(Ground(Arrow+Direction*17,3),Ground(Arrow-Direction*12+Side*16,3),Ground(Arrow-Direction*12-Side*16,3),Color*1.35f);
        if (!Edge.bUnlocked)
        {
            const FVector Stop=FMath::Lerp(A,B,.35);
            Mesh.Quad(Ground(Stop-Side*13-Direction*3,3),Ground(Stop+Side*13-Direction*3,3),Ground(Stop+Side*13+Direction*3,3),Ground(Stop-Side*13+Direction*3,3),FLinearColor(.93f,.46f,.27f));
        }
    }
    Mesh.Commit(RoadGeometry,VertexMaterial(VisualTheme));
}

void APadmaWorldMapActor::GenerateDecoration()
{
    RequestedDecorationPoints=0;
    if (!GetWorld()||!MapBounds.IsValid) return;
    const auto* T=ThemeOrDefault(VisualTheme);
    DecorationGraph=NewObject<UPCGGraph>(this,NAME_None,RF_Transient);
    UPadmaDecorationPointsSettings* SourceSettings=nullptr;
    UPCGNode* SourceNode=DecorationGraph->AddNodeOfType(SourceSettings);
    UPCGStaticMeshSpawnerSettings* SpawnerSettings=nullptr;
    UPCGNode* SpawnerNode=DecorationGraph->AddNodeOfType(SpawnerSettings);
    if (!SourceNode||!SpawnerNode||!SourceSettings||!SpawnerSettings) return;
    SpawnerSettings->SetMeshSelectorType(UPCGMeshSelectorWeighted::StaticClass());
    auto* Selector=Cast<UPCGMeshSelectorWeighted>(SpawnerSettings->MeshSelectorParameters);
    if (!Selector) return;
    TArray<TSoftObjectPtr<UStaticMesh>> Meshes=T->DecorationMeshes;
    if (Meshes.IsEmpty()) Meshes.Add(TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Engine/BasicShapes/Cone.Cone"))));
    double MeshFootprint=0, MeshBottom=0;
    for (const auto& Reference:Meshes)
    {
        UStaticMesh* LoadedMesh=Reference.LoadSynchronous();
        if (!LoadedMesh) continue;
        MeshFootprint=FMath::Max(MeshFootprint,LoadedMesh->GetBounds().Origin.Size2D()+LoadedMesh->GetBounds().BoxExtent.Size2D());
        MeshBottom=FMath::Min(MeshBottom,LoadedMesh->GetBounds().Origin.Z-LoadedMesh->GetBounds().BoxExtent.Z);
        FPCGMeshSelectorWeightedEntry Entry; Entry.Weight=1; Entry.Descriptor.StaticMesh=Reference;
        Entry.Descriptor.bUseDefaultCollision=false; Entry.Descriptor.BodyInstance.SetCollisionProfileName(TEXT("NoCollision"));
        Entry.Descriptor.bGenerateOverlapEvents=false;
        if (!T->DecorationMaterial.IsNull()) Entry.Descriptor.OverrideMaterials.Add(T->DecorationMaterial);
        Selector->MeshEntries.Add(Entry);
    }
    if (Selector->MeshEntries.IsEmpty()) { UE_LOG(LogTemp,Warning,TEXT("[PadmaMapPCG] No loadable decoration meshes.")); return; }
    FRandomStream Random(DecorationSeed^0x41C64E6D);
    const int32 Count=FMath::Clamp(T->DecorationCount,0,2400);
    const double MinScale=FMath::Clamp(T->DecorationScaleMin,.05,3.), MaxScale=FMath::Clamp(T->DecorationScaleMax,MinScale,3.);
    const double Keepout=MeshFootprint*MaxScale;
    TArray<FVector2D> Accepted;
    for (int32 Attempt=0; Attempt<Count*48&&SourceSettings->Points.Num()<Count; ++Attempt)
    {
        FVector P(Random.FRandRange(MapBounds.Min.X+30,MapBounds.Max.X-30),Random.FRandRange(MapBounds.Min.Y+30,MapBounds.Max.Y-30),0);
        const FVector2D XY(P);
        if (!Landscape.CanDecorate(XY,Keepout,T->DecorationMaxSlopeDegrees)||Random.FRand()>Landscape.ForestDensity(XY)) continue;
        // Small minimum spacing permits overlapping crowns while preventing coincident trunks.
        if (Accepted.ContainsByPredicate([&](const FVector2D& Other) { return FVector2D::DistSquared(XY,Other)<FMath::Square(Keepout*.58); })) continue;
        const double Scale=Random.FRandRange(MinScale,MaxScale);
        P.Z=Landscape.Height(XY)-MeshBottom*Scale-1.5;
        FPCGPoint Point; Point.Transform=FTransform(FRotator(0,Random.FRandRange(0,360),0),P,FVector(Scale));
        Point.Density=1; Point.Seed=Random.RandRange(1,MAX_int32); Point.BoundsMin=FVector(-50); Point.BoundsMax=FVector(50);
        SourceSettings->Points.Add(Point); Accepted.Add(XY);
    }
    RequestedDecorationPoints=SourceSettings->Points.Num();
    DecorationGraph->AddEdge(SourceNode,PCGPinConstants::DefaultOutputLabel,SpawnerNode,PCGPinConstants::DefaultInputLabel);
    DecorationGraph->AddEdge(SpawnerNode,PCGPinConstants::DefaultOutputLabel,DecorationGraph->GetOutputNode(),PCGPinConstants::DefaultOutputLabel);
    DecorationPCG->OnPCGGraphGeneratedDelegate.RemoveAll(this);
    DecorationPCG->OnPCGGraphGeneratedDelegate.AddUObject(this,&APadmaWorldMapActor::OnDecorationGenerated);
    DecorationPCG->Seed=DecorationSeed; DecorationPCG->SetGraphLocal(DecorationGraph);
    UE_LOG(LogTemp,Display,TEXT("[PadmaMapPCG] requested seed=%d points=%d meshes=%d graph=%s"),DecorationSeed,RequestedDecorationPoints,Selector->MeshEntries.Num(),*DecorationGraph->GetName());
    DecorationPCG->GenerateLocal(true);
}
void APadmaWorldMapActor::OnDecorationGenerated(UPCGComponent* Component)
{
    if (Component!=DecorationPCG) return;
    GeneratedDecorationInstances=0;
    TArray<UInstancedStaticMeshComponent*> Instances; GetComponents(Instances);
    for (auto* ISM:Instances)
    {
        ISM->SetCollisionEnabled(ECollisionEnabled::NoCollision); ISM->SetGenerateOverlapEvents(false);
        GeneratedDecorationInstances+=ISM->GetInstanceCount();
    }
    UE_LOG(LogTemp,Display,TEXT("[PadmaMapPCG] completed seed=%d points=%d instances=%d components=%d layout-build=%d"),
        DecorationSeed,RequestedDecorationPoints,GeneratedDecorationInstances,Instances.Num(),LayoutBuildCount);
}
void APadmaWorldMapActor::ClearGraph()
{
    DecorationPCG->OnPCGGraphGeneratedDelegate.RemoveAll(this); DecorationPCG->CancelGeneration(); DecorationPCG->CleanupLocalImmediate(true);
    for (auto& Pair:Tiles) if (IsValid(Pair.Value)) Pair.Value->Destroy();
    Tiles.Reset(); LastNodes.Reset(); LastEdges.Reset(); LayoutKey.Reset(); RoadKey.Reset();
    TerrainField->ClearAllMeshSections(); RoadGeometry->ClearAllMeshSections(); WaterGeometry->ClearAllMeshSections(); DecorationGraph=nullptr;
    GeneratedDecorationInstances=0; TerrainTriangleCount=0; MapBounds=FBox(ForceInit);
}
void APadmaWorldMapActor::EndPlay(const EEndPlayReason::Type Reason)
{ ClearGraph(); Super::EndPlay(Reason); }
void APadmaWorldMapActor::Destroyed()
{ ClearGraph(); Super::Destroyed(); }
