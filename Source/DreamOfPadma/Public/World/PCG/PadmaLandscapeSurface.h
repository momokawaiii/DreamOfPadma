#pragma once

#include "CoreMinimal.h"

class UPadmaMapVisualTheme;
struct FPadmaWorldNodeView;
struct FPadmaWorldEdgeView;

/** A cosmetic height field. Its inputs are copies; it never changes graph coordinates or travel. */
class DREAMOFPADMA_API FPadmaLandscapeSurface
{
public:
    void Initialize(const TArray<FPadmaWorldNodeView>& Nodes, const TArray<FPadmaWorldEdgeView>& Edges,
        const UPadmaMapVisualTheme* Theme, int32 CosmeticSeed);
    double Height(const FVector2D& Position) const;
    FVector Normal(const FVector2D& Position) const;
    FLinearColor Color(const FVector2D& Position) const;
    double RiverCenter(double Y) const;
    double RiverHalfWidth(double Y) const;
    double RiverDistance(const FVector2D& Position) const;
    bool CanDecorate(const FVector2D& Position, double Footprint, double MaxSlopeDegrees) const;
    double ForestDensity(const FVector2D& Position) const;
    FBox Bounds = FBox(ForceInit);
    bool bRiver = false;
    static constexpr double WaterHeight = 9.;
private:
    struct FSite { FVector2D Position; double Height; FLinearColor Color; bool bForest; };
    struct FRoute { int32 From; int32 To; };
    TArray<FSite> Sites;
    TArray<FRoute> Routes;
    double Radius = 112., Margin = 520., Hills = 80., Mountains = 310., Phase = 0.;
    double Noise(const FVector2D& Position) const;
};
