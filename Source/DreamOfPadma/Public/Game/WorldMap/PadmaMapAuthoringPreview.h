#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PadmaMapAuthoringPreview.generated.h"
class UPadmaWorldMapAsset;
class UPadmaMapVisualTheme;
class APadmaWorldMapActor;

/** Transient editor handle; identity stays in the definition asset. */
UCLASS(NotBlueprintable)
class DREAMOFPADMA_API APadmaMapNodeHandle : public AActor
{
    GENERATED_BODY()
public:
    APadmaMapNodeHandle();
    UPROPERTY(VisibleAnywhere, Category="Map Editing") FName NodeId;
};

/** Scene editing adapter. Runtime gameplay never reads these actors. */
UCLASS()
class DREAMOFPADMA_API APadmaMapAuthoringPreview : public AActor
{
    GENERATED_BODY()
public:
    APadmaMapAuthoringPreview();
    UPROPERTY(EditAnywhere, Category="Map Editing") TObjectPtr<UPadmaWorldMapAsset> Map;
    UPROPERTY(EditAnywhere, Category="Map Editing") TObjectPtr<UPadmaMapVisualTheme> Theme;
    UPROPERTY(EditAnywhere, Category="Map Editing") bool bEditGeneratedLayout = false;
    UPROPERTY(VisibleAnywhere, Transient, Category="Map Editing") FString Status;
    UFUNCTION(CallInEditor, BlueprintCallable, Category="Map Editing") void RebuildPreview();
    UFUNCTION(CallInEditor, BlueprintCallable, Category="Map Editing") void ApplyNodeTransforms();
    UFUNCTION(CallInEditor, BlueprintCallable, Category="Map Editing") void ClearPreview();
    virtual void Destroyed() override;
private:
    UPROPERTY(Transient) TObjectPtr<APadmaWorldMapActor> View;
    UPROPERTY(Transient) TArray<TObjectPtr<APadmaMapNodeHandle>> Handles;
};
