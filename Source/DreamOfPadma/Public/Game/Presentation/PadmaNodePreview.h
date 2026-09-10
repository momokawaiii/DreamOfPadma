#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Containers/Ticker.h"
#include "Game/Presentation/PadmaPresentationCatalog.h"
#include "PadmaNodePreview.generated.h"

class UCameraComponent;

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaSceneSlot
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene")
	FName SlotId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene")
	FTransform Transform = FTransform::Identity;
};

/** Static scene layout only; no current occupants or card state. */
UCLASS(BlueprintType)
class DREAMOFPADMA_API UPadmaNodeSceneDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene")
	FName SceneDefinitionId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene")
	TSoftObjectPtr<UWorld> Level;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene")
	TArray<FPadmaSceneSlot> Slots;
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaPreviewBinding
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Preview")
	FName SlotId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Preview")
	FName EntityInstanceId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Preview")
	FPadmaPresentationKey Definition;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Preview")
	FName SelectedSkillRow;
};

/** Independent authoring fixture. These bindings are NEVER authoritative occupancy. */
UCLASS(BlueprintType)
class DREAMOFPADMA_API APadmaNodePreview : public AActor
{
	GENERATED_BODY()
public:
	APadmaNodePreview();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Authoring Preview")
	FName PreviewNodeId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Authoring Preview")
	TSoftObjectPtr<UPadmaNodeSceneDefinition> SceneDefinition;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Authoring Preview")
	TSoftObjectPtr<UPadmaPresentationCatalog> Catalog;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Authoring Preview")
	TArray<FPadmaPreviewBinding> PreviewBindings;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Authoring Preview")
	TArray<FText> Diagnostics;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Authoring Preview")
	TArray<TObjectPtr<APadmaModelPresentationActor>> Presenters;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Authoring Preview")
	TObjectPtr<UCameraComponent> PreviewCamera;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Authoring Preview")
	bool RebuildPreview();
	/** Preview-only edit; caller supplies an identity, never a catalog index. */
	UFUNCTION(BlueprintCallable, Category = "Authoring Preview")
	bool SetPreviewSelection(FName SlotId, FPadmaPresentationKey Definition);
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	virtual void Destroyed() override;
private:
	void ClearPresenters();
	void StopCapture();
	FTSTicker::FDelegateHandle CaptureTicker;
	FTimerHandle CaptureExitTimer;
};

/** Preview camera has no player avatar; all visible entities come from bindings. */
UCLASS()
class DREAMOFPADMA_API APadmaModelPreviewGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	APadmaModelPreviewGameMode();
};
