#pragma once

#include "CoreMinimal.h"
#include "Core/Content/PadmaContentTypes.h"
#include "PadmaWorldMapDefinition.generated.h"

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaMapGenerationSettings
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation", meta = (ClampMin = "0", ClampMax = "64"))
	int32 BranchCountMin = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation", meta = (ClampMin = "0", ClampMax = "64"))
	int32 BranchCountMax = 3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation", meta = (ClampMin = "1", ClampMax = "8"))
	int32 BranchLengthMin = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation", meta = (ClampMin = "1", ClampMax = "8"))
	int32 BranchLengthMax = 3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation", meta = (ClampMin = "0.0"))
	double BranchSpread = 12.0;
	/** Empty uses each branch source node's terrain. Authored nodes are never randomized. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation")
	TArray<FName> TerrainChoices;
};

/** Value-only authoring and deterministic generation; no world or presentation access. */
struct DREAMOFPADMA_API FPadmaWorldMapGenerator
{
	static constexpr int32 CurrentGeneratorVersion = 1;
	static constexpr int32 MaxNodes = 1024;
	static constexpr int32 MaxEdges = 8192;

	static bool Validate(const FPadmaMapLayout& Layout, FString& Error);
	/** Also verifies that every authored anchor keeps its identity, role and exact coordinates. */
	static bool ValidateGenerated(const FPadmaMapLayout& Template, const FPadmaMapLayout& Generated, FString& Error);
	/** On any failure Out is untouched. Branches cannot cross an authored locked-edge cut. */
	static bool Generate(const FPadmaMapLayout& Template, const FPadmaMapGenerationSettings& Settings,
		int32 Seed, FPadmaMapLayout& Out, FString& Error);
	/** Replaces only map values after successful map validation. */
	static bool ApplyToContent(const FPadmaMapLayout& Layout, FPadmaContentSnapshot& Content, FString& Error);
	/** Stable gameplay identity, independent of array/map iteration order and display culture. */
	static FString Signature(const FPadmaMapLayout& Layout);
};
