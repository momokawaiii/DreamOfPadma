#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/WorldMap/PadmaWorldMapDefinition.h"
#include "PadmaWorldMapAsset.generated.h"

/** Editable authored graph plus an optional frozen generated result. */
UCLASS(BlueprintType)
class DREAMOFPADMA_API UPadmaWorldMapAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Map")
	FPadmaMapLayout Template;
	/** Optional authoring inputs. Import explicitly; the frozen Template remains runtime authority. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story Authoring")
	TArray<TSoftObjectPtr<UDataTable>> CheckpointTables;
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Story Authoring")
	void ImportCheckpointTables();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation")
	FPadmaMapGenerationSettings GenerationSettings;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation")
	int32 Seed = 12345;
	/** Can be edited manually; validation still enforces authored anchor identity and location. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation")
	FPadmaMapLayout GeneratedLayout;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation")
	bool bUseGenerated = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Validation")
	FString ValidationError;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Validation")
	bool bLastValidationSucceeded = false;

	const FPadmaMapLayout& GetActiveLayout() const { return bUseGenerated ? GeneratedLayout : Template; }
	UFUNCTION(BlueprintPure, Category = "World Map")
	FPadmaMapLayout GetLayout() const { return GetActiveLayout(); }
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Generation")
	void Generate();
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Validation")
	void Validate();
	/** Restores the editable output to the authored template and switches back to it. */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Generation")
	void RestoreTemplate();
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
