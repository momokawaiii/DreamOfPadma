#include "Misc/AutomationTest.h"
#include "World/PCG/PadmaLandscapeSurface.h"
#include "World/Map/PadmaMapVisualTheme.h"
#include "World/Map/PadmaWorldMapActor.h"

#if WITH_DEV_AUTOMATION_TESTS
namespace
{
void SampleLayout(TArray<FPadmaWorldNodeView>& Nodes, TArray<FPadmaWorldEdgeView>& Edges)
{
    FPadmaWorldNodeView Home;
    Home.Id=TEXT("home"); Home.Position=FVector(0,0,0); Home.Terrain=TEXT("plain");
    FPadmaWorldNodeView Shrine;
    Shrine.Id=TEXT("shrine"); Shrine.Position=FVector(0,900,0); Shrine.Terrain=TEXT("forest");
    Shrine.AnchorKind=EPadmaMapAnchorKind::MainStory; Shrine.NPCId=TEXT("watcher");
    FPadmaWorldNodeView Summit;
    Summit.Id=TEXT("summit"); Summit.Position=FVector(500,1600,0); Summit.Terrain=TEXT("mountain");
    Nodes={Home,Shrine,Summit}; Edges={{Home.Id,Shrine.Id,true},{Shrine.Id,Summit.Id,false}};
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaLandscapeContinuityTest,"DreamOfPadma.WorldMap.Landscape.ContinuousGroundAndFixedSites",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaLandscapeContinuityTest::RunTest(const FString& Parameters)
{
    TArray<FPadmaWorldNodeView> Nodes; TArray<FPadmaWorldEdgeView> Edges; SampleLayout(Nodes,Edges);
    const auto* Theme=GetDefault<UPadmaMapVisualTheme>();
    FPadmaLandscapeSurface Surface; Surface.Initialize(Nodes,Edges,Theme,731);
    TestTrue(TEXT("Expanded landscape bounds are valid"),Surface.Bounds.IsValid!=0);
    for (const auto& Node:Nodes)
    {
        const double Expected=Theme->ResolveTerrain(Node.Terrain).Height;
        for (const FVector2D Offset:{FVector2D(0,0),FVector2D(70,0),FVector2D(0,-70)})
            TestTrue(TEXT("Node foundations remain flat at authored terrain elevation"),
                FMath::IsNearlyEqual(Surface.Height(FVector2D(Node.Position)+Offset),Expected,.001));
    }
    TestEqual(TEXT("Fixed story anchor retains its XY coordinate"),Nodes[1].Position,FVector(0,900,0));
    TestEqual(TEXT("Fixed NPC identity is untouched"),Nodes[1].NPCId,FName(TEXT("watcher")));
    // Foundation aprons extend to Radius+105 (217 here). Only the road outside both
    // aprons is strictly linear; an approach intentionally eases onto its flat node.
    for (double Y=250.; Y<700.; Y+=70.)
    {
        const double Expected=FMath::Lerp(Theme->ResolveTerrain(TEXT("plain")).Height,Theme->ResolveTerrain(TEXT("forest")).Height,Y/900.);
        TestTrue(TEXT("Road centerline is graded between nodes"),FMath::IsNearlyEqual(Surface.Height(FVector2D(0,Y)),Expected,.001));
        TestFalse(TEXT("Road clear corridor rejects scenery"),Surface.CanDecorate(FVector2D(10,Y),30.,45.));
    }
    const double HomeHeight=Theme->ResolveTerrain(TEXT("plain")).Height;
    const double ShrineHeight=Theme->ResolveTerrain(TEXT("forest")).Height;
    TestTrue(TEXT("Home approach eases below the unflattened road grade"),
        Surface.Height(FVector2D(0,180))<FMath::Lerp(HomeHeight,ShrineHeight,180./900.));
    TestTrue(TEXT("Shrine approach eases above the unflattened road grade"),
        Surface.Height(FVector2D(0,720))>FMath::Lerp(HomeHeight,ShrineHeight,720./900.));
    for (const double Boundary:{Theme->HexRadius+8.,Theme->HexRadius+105.,900.-Theme->HexRadius-105.,900.-Theme->HexRadius-8.})
    {
        TestTrue(TEXT("Road and foundation apron meet continuously at both ends"),
            FMath::IsNearlyEqual(Surface.Height(FVector2D(0,Boundary-.01)),Surface.Height(FVector2D(0,Boundary+.01)),.001));
    }
    double Highest=-1000.; int32 LegalPoints=0;
    for (double X=Surface.Bounds.Min.X+20.; X<Surface.Bounds.Max.X-20.; X+=51.)
        for (double Y=Surface.Bounds.Min.Y+20.; Y<Surface.Bounds.Max.Y-20.; Y+=57.)
        {
            const FVector2D P(X,Y); const double H=Surface.Height(P); const FVector Normal=Surface.Normal(P);
            TestTrue(TEXT("Surface heights are finite"),FMath::IsFinite(H));
            TestTrue(TEXT("Surface normals are finite, unit and face upward"),!Normal.ContainsNaN()&&FMath::IsNearlyEqual(Normal.Size(),1.,.001)&&Normal.Z>0.);
            TestTrue(TEXT("Height is continuous across adjacent samples"),FMath::Abs(H-Surface.Height(P+FVector2D(.01,.01)))<.2);
            Highest=FMath::Max(Highest,H); LegalPoints+=Surface.CanDecorate(P,25.,30.)?1:0;
        }
    TestTrue(TEXT("Distant ridges have visible relief"),Highest>220.);
    TestTrue(TEXT("Controlled PCG retains usable planting sites"),LegalPoints>30);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaLandscapeRiverSeedTest,"DreamOfPadma.WorldMap.Landscape.RiverKeepoutAndCosmeticSeed",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaLandscapeRiverSeedTest::RunTest(const FString& Parameters)
{
    TArray<FPadmaWorldNodeView> Nodes; TArray<FPadmaWorldEdgeView> Edges; SampleLayout(Nodes,Edges);
    FPadmaLandscapeSurface First,Repeat,Different;
    First.Initialize(Nodes,Edges,nullptr,12345); Repeat.Initialize(Nodes,Edges,nullptr,12345); Different.Initialize(Nodes,Edges,nullptr,57391);
    int32 Differences=0;
    for (double Y=-300.; Y<1900.; Y+=63.)
    {
        const FVector2D River(First.RiverCenter(Y),Y);
        TestTrue(TEXT("River bed remains beneath the independent water surface"),First.Height(River)<FPadmaLandscapeSurface::WaterHeight);
        TestFalse(TEXT("PCG excludes water and bank footprint"),First.CanDecorate(River,30.,60.));
        const FVector2D P(680.,Y);
        TestEqual(TEXT("Same cosmetic seed reproduces the terrain"),First.Height(P),Repeat.Height(P));
        Differences+=FMath::Abs(First.Height(P)-Different.Height(P))>.001?1:0;
    }
    TestTrue(TEXT("Independent cosmetic seed changes scenery"),Differences>0);
    TestEqual(TEXT("Terrain generation cannot add graph nodes"),Nodes.Num(),3);
    TestEqual(TEXT("Locked directed route remains locked"),Edges[1].bUnlocked,false);
    TestFalse(TEXT("Node footprint rejects scenery regardless of slope"),First.CanDecorate(FVector2D(0,0),10.,60.));
    return true;
}
#endif
