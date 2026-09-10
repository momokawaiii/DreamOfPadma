#pragma once

#include "Core/Content/PadmaContentTypes.h"

/** Fixed painting anchors plus deterministic, value-only hex wilderness. No world or asset access. */
struct DREAMOFPADMA_API FPadmaTutorialMapGenerator
{
    static constexpr int32 CurrentGeneratorVersion = 3;
    static constexpr int32 DefaultSeed = 12345;
    /** Approved version presets; unknown versions leave Out unchanged. V1 has 162 cells, v2 has 55. */
    static bool GridForVersion(int32 GeneratorVersion, FPadmaHexGridConfig& Out);
    static FVector2D CellCenter(const FPadmaHexGridConfig& Grid, int32 Q, int32 R);
    /** Pointy-top corners in painting units, starting at 30 degrees; CornerIndex must be 0..5. */
    static FVector2D CellCorner(const FPadmaHexGridConfig& Grid, int32 Q, int32 R, int32 CornerIndex);
    static bool ContainsCell(const FPadmaHexGridConfig& Grid, int32 Q, int32 R);
    static bool ContainsPoint(const FPadmaHexGridConfig& Grid, int32 Q, int32 R, FVector2D PaintingPoint);
    static bool FixedPosition(FName Id, FVector2D& Out);
    /** New maps default to v2. Explicit v1 is for compatibility; on failure Out is unchanged. */
    static bool Generate(const FPadmaMapLayout& Source, int32 Seed, FPadmaMapLayout& Out, FString& Error,
        int32 GeneratorVersion = CurrentGeneratorVersion);
};
