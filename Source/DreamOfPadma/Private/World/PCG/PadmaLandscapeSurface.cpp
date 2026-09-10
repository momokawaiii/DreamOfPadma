#include "World/PCG/PadmaLandscapeSurface.h"
#include "World/Map/PadmaWorldMapActor.h"
#include "World/Map/PadmaMapVisualTheme.h"

namespace
{
double Smooth(double A, double B, double Value)
{
    const double T = FMath::Clamp((Value - A) / (B - A), 0., 1.);
    return T * T * (3. - 2. * T);
}
double SegmentDistance(const FVector2D& P, const FVector2D& A, const FVector2D& B, double& Alpha)
{
    const FVector2D Delta = B - A;
    Alpha = FMath::Clamp(FVector2D::DotProduct(P - A, Delta) / FMath::Max(Delta.SizeSquared(), 1.), 0., 1.);
    return FVector2D::Distance(P, A + Delta * Alpha);
}
}

void FPadmaLandscapeSurface::Initialize(const TArray<FPadmaWorldNodeView>& Nodes,
    const TArray<FPadmaWorldEdgeView>& Edges, const UPadmaMapVisualTheme* Theme, int32 CosmeticSeed)
{
    const auto* T = Theme ? Theme : GetDefault<UPadmaMapVisualTheme>();
    Sites.Reset(); Routes.Reset(); Bounds = FBox(ForceInit);
    Radius = FMath::Clamp(T->HexRadius, 60., 200.); Margin = FMath::Clamp(T->FieldMargin, 100., 1000.);
    Hills = FMath::Clamp(T->HillRelief, 0., 400.); Mountains = FMath::Clamp(T->MountainRelief, 0., 700.);
    Phase = double(uint32(CosmeticSeed) ^ 0x6C8E9CF5u) / double(MAX_uint32) * 1000.;
    // Keep the water corridor beyond the fixed graph envelope, even for small authored margins.
    bRiver = T->bShowRiver && Margin >= 350.;
    TMap<FName, int32> Indices;
    for (const auto& Node : Nodes)
    {
        const auto Terrain = T->ResolveTerrain(Node.Terrain);
        Indices.Add(Node.Id, Sites.Num());
        Sites.Add({FVector2D(Node.Position), FMath::Clamp(Terrain.Height, 0., 180.) + Node.Position.Z,
            Terrain.Color, Node.Terrain == TEXT("forest") || Node.Terrain == TEXT("林地")});
        Bounds += Node.Position;
    }
    for (const auto& Edge : Edges)
    {
        const int32* A = Indices.Find(Edge.From); const int32* B = Indices.Find(Edge.To);
        if (A && B) Routes.Add({*A, *B});
    }
    if (Bounds.IsValid)
    {
        Bounds = Bounds.ExpandBy(FVector(Margin, Margin, 0));
        Bounds.Min.Z = -60.; Bounds.Max.Z = Mountains + Hills + 220.;
    }
}

double FPadmaLandscapeSurface::Noise(const FVector2D& P) const
{
    return FMath::PerlinNoise2D(P / 320. + FVector2D(Phase, Phase * .73)) * .68
        + FMath::PerlinNoise2D(P / 125. + FVector2D(Phase * .41, -Phase)) * .24
        + FMath::PerlinNoise2D(P / 48. + FVector2D(-Phase, Phase * .19)) * .08;
}

double FPadmaLandscapeSurface::RiverCenter(double Y) const
{
    return Bounds.Min.X + Margin * .30 + FMath::Sin(Y / 310. + Phase) * Margin * .10
        + FMath::Sin(Y / 117. + Phase * .7) * 14.;
}
double FPadmaLandscapeSurface::RiverHalfWidth(double Y) const
{ return 34. + 7. * FMath::Sin(Y / 180. + Phase); }
double FPadmaLandscapeSurface::RiverDistance(const FVector2D& P) const
{ return bRiver ? FMath::Abs(P.X - RiverCenter(P.Y)) - RiverHalfWidth(P.Y) : TNumericLimits<double>::Max(); }

double FPadmaLandscapeSurface::Height(const FVector2D& P) const
{
    if (Sites.IsEmpty()) return 0.;
    double WeightedHeight = 0., WeightSum = 0.;
    for (const FSite& Site : Sites)
    {
        const double Weight = 1. / FMath::Square(FVector2D::DistSquared(P, Site.Position) + 90000.);
        WeightedHeight += Site.Height * Weight; WeightSum += Weight;
    }
    const double N = Noise(P);
    double Result = WeightedHeight / WeightSum + Hills * (.48 + N);
    const double RidgeCenter = Bounds.Max.X - Margin * .34 + FMath::Sin(P.Y / 420. + Phase * .4) * 75.;
    const double Ridge = FMath::Exp(-FMath::Square((P.X - RidgeCenter) / (Margin * .38)));
    const double Peaks = .56 + .44 * FMath::Abs(FMath::Sin(P.Y / 185. + Phase) + N * .7);
    Result += Mountains * Ridge * Peaks;
    // Graded roads blend smoothly back into the hills. Nodes override their approaches last.
    for (const FRoute& Route : Routes)
    {
        double Alpha;
        const FSite& A = Sites[Route.From]; const FSite& B = Sites[Route.To];
        const double Distance = SegmentDistance(P, A.Position, B.Position, Alpha);
        Result = FMath::Lerp(FMath::Lerp(A.Height, B.Height, Alpha), Result, Smooth(24., 105., Distance));
    }
    for (const FSite& Site : Sites)
        Result = FMath::Lerp(Site.Height, Result, Smooth(Radius + 8., Radius + 105., FVector2D::Distance(P, Site.Position)));
    if (bRiver)
    {
        const double Distance = RiverDistance(P);
        Result = FMath::Lerp(1.5 + .6 * N, Result, Smooth(-6., 70., Distance));
    }
    return Result;
}

FVector FPadmaLandscapeSurface::Normal(const FVector2D& P) const
{
    constexpr double Step = 4.;
    return FVector(Height(P - FVector2D(Step, 0)) - Height(P + FVector2D(Step, 0)),
        Height(P - FVector2D(0, Step)) - Height(P + FVector2D(0, Step)), Step * 2.).GetSafeNormal();
}

FLinearColor FPadmaLandscapeSurface::Color(const FVector2D& P) const
{
    FLinearColor Result(0, 0, 0, 0); double WeightSum = 0.;
    for (const FSite& Site : Sites)
    {
        const double Weight = 1. / FMath::Square(FVector2D::DistSquared(P, Site.Position) + 75000.);
        Result += Site.Color * Weight; WeightSum += Weight;
    }
    Result = WeightSum > 0. ? Result / WeightSum : FLinearColor(.25f, .36f, .16f);
    const double H = Height(P), Slope = 1. - Normal(P).Z;
    const double Rock = FMath::Max(Smooth(.08, .32, Slope), Smooth(145., 285., H) * .92);
    // Cooler exposed ridges with irregular strata, retaining vegetation on gentler slopes.
    // Detail changes albedo only: foundations, road grading and planting slope checks stay fixed.
    const double Grain = FMath::PerlinNoise2D(P / 39. + FVector2D(Phase * .8, -Phase));
    const double Strata = Smooth(.18, .76, FMath::Sin(H * .10 + P.Y * .019 + Noise(P) * 5.));
    FLinearColor Stone = FMath::Lerp(FLinearColor(.16f,.19f,.19f), FLinearColor(.30f,.34f,.34f), Strata);
    Stone *= .92 + Grain * .24;
    Result = FMath::Lerp(Result, Stone, Rock);
    Result = FMath::Lerp(Result, FLinearColor(.55f,.64f,.64f), Smooth(450., 550., H) * .4);
    if (bRiver) Result = FMath::Lerp(FLinearColor(.30f, .285f, .15f), Result, Smooth(0., 40., RiverDistance(P)));
    Result *= .94 + Noise(P) * .20; Result.A = 1.;
    return Result;
}

bool FPadmaLandscapeSurface::CanDecorate(const FVector2D& P, double Footprint, double MaxSlopeDegrees) const
{
    if (!Bounds.IsValid || !Bounds.IsInsideXY(FVector(P, 0)) || RiverDistance(P) < Footprint + 14.) return false;
    for (const FSite& Site : Sites)
        if (FVector2D::Distance(P, Site.Position) < Radius + Footprint + 18.) return false;
    for (const FRoute& Route : Routes)
    {
        double Alpha;
        if (SegmentDistance(P, Sites[Route.From].Position, Sites[Route.To].Position, Alpha) < Footprint + 30.) return false;
    }
    return Normal(P).Z >= FMath::Cos(FMath::DegreesToRadians(FMath::Clamp(MaxSlopeDegrees, 0., 60.)));
}

double FPadmaLandscapeSurface::ForestDensity(const FVector2D& P) const
{
    double Density = .06 + .78 * Smooth(-.02, .28, Noise(P));
    for (const FSite& Site : Sites)
        if (Site.bForest) Density = FMath::Max(Density, 1. - FVector2D::Distance(P, Site.Position) / 520.);
    return Density * (1. - Smooth(190., 310., Height(P)));
}
