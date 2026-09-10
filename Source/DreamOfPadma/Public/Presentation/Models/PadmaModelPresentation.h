#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameFramework/Actor.h"
#include "PadmaModelPresentation.generated.h"

class UAnimInstance;
class USkeletalMesh;
class UStaticMesh;
class UStaticMeshComponent;
class USkeletalMeshComponent;

/** Presentation only; no movement, card inventory, skill effects or occupancy. */
UCLASS(BlueprintType)
class DREAMOFPADMA_API UPadmaModelDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FName VisualDefinitionId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	int32 ContentVersion = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Display")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Model")
	TSoftObjectPtr<UStaticMesh> StaticModel;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Model")
	TSoftObjectPtr<USkeletalMesh> SkeletalModel;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Model")
	TSoftClassPtr<UAnimInstance> AnimationClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Model")
	FTransform ModelTransform = FTransform::Identity;
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaPresentationSkillView
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "Details")
	FName RowName;
	UPROPERTY(BlueprintReadOnly, Category = "Details")
	FName SkillId;
	UPROPERTY(BlueprintReadOnly, Category = "Details")
	FText DisplayName;
	/** Empty means source has no description, never an invented effect. */
	UPROPERTY(BlueprintReadOnly, Category = "Details")
	FText Description;
};

/** Loaded immutable projection, with strong references retained by its presenter. */
USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaResolvedPresentation
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "Presentation")
	bool bVisualReady = false;
	UPROPERTY(BlueprintReadOnly, Category = "Presentation")
	bool bSkillDetailsReady = false;
	UPROPERTY(BlueprintReadOnly, Category = "Presentation")
	FName DefinitionId;
	UPROPERTY(BlueprintReadOnly, Category = "Presentation")
	FText DisplayName;
	UPROPERTY(BlueprintReadOnly, Category = "Presentation")
	TObjectPtr<UStaticMesh> StaticModel;
	UPROPERTY(BlueprintReadOnly, Category = "Presentation")
	TObjectPtr<USkeletalMesh> SkeletalModel;
	UPROPERTY(BlueprintReadOnly, Category = "Presentation")
	TSubclassOf<UAnimInstance> AnimationClass;
	UPROPERTY(BlueprintReadOnly, Category = "Presentation")
	FTransform ModelTransform = FTransform::Identity;
	UPROPERTY(BlueprintReadOnly, Category = "Presentation")
	TArray<FPadmaPresentationSkillView> Skills;
	UPROPERTY(BlueprintReadOnly, Category = "Presentation")
	TArray<FText> VisualErrors;
	UPROPERTY(BlueprintReadOnly, Category = "Presentation")
	TArray<FText> SkillErrors;
};

/** Reusable projection Actor. It knows no ACT/Demo/catalog or world-state types. */
UCLASS(BlueprintType)
class DREAMOFPADMA_API APadmaModelPresentationActor : public AActor
{
	GENERATED_BODY()
public:
	APadmaModelPresentationActor();
	void ApplyPresentation(const FPadmaResolvedPresentation& InPresentation);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Presentation")
	FPadmaResolvedPresentation Presentation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Presentation")
	TObjectPtr<UStaticMeshComponent> StaticVisual;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Presentation")
	TObjectPtr<USkeletalMeshComponent> SkeletalVisual;
};
