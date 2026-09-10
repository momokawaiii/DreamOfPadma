#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "Core/Cards/PadmaCardMobility.h"
#include "PadmaACTAuthoring.generated.h"

class UAnimInstance;
class UAnimMontage;
class USkeletalMesh;
class UStaticMesh;

/** Authoring diagnostics only. Success does not certify an executable GAS loadout. */
USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaACTAuthoringReport
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "ACT Authoring")
	bool bValid = false;

	UPROPERTY(BlueprintReadOnly, Category = "ACT Authoring")
	TArray<FText> Errors;
};

UCLASS(BlueprintType)
class DREAMOFPADMA_API UPadmaACTSkillDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FName DefinitionId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity", meta = (ClampMin = "1"))
	int32 ContentVersion = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Display")
	FText DisplayName;

	/** Resolved by the ACT runtime's registry, never by an Encounter fallback. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ACT Binding")
	FName AbilityImplementationId;

	/** Optional presentation; does not authorize hits, costs or timing. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	TSoftObjectPtr<UAnimMontage> Montage;
};

/** ACT-only index; no shared Encounter effects or invented numerical defaults. */
USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaACTSkillRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ACT Skill")
	FName SkillId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ACT Skill")
	FName ActivationBindingId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ACT Skill")
	TSoftObjectPtr<UPadmaACTSkillDefinition> Definition;
};

/** Data describing a trait restriction. Evaluating battle legality is not authoring. */
USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaACTTraitTerrainRestriction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Restriction")
	FName TraitId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Restriction")
	TArray<FName> DisallowedTerrainIds;
};

UCLASS(BlueprintType)
class DREAMOFPADMA_API UPadmaACTCharacterDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	/** Card metadata only; an ACT selection does not imply sandbox deployment. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Movement")
	FPadmaCardMobilityDefinition Mobility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FName DefinitionId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity", meta = (ClampMin = "1"))
	int32 ContentVersion = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Display")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	TSoftObjectPtr<USkeletalMesh> Model;

	/** Optional for a static preview. Runtime animation readiness is checked later. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	TSoftClassPtr<UAnimInstance> AnimationClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ACT Skills",
		meta = (RequiredAssetDataTags = "RowStructure=/Script/DreamOfPadma.PadmaACTSkillRow"))
	TSoftObjectPtr<UDataTable> SkillTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ACT Traits")
	TArray<FName> TraitIds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ACT Traits")
	TArray<FPadmaACTTraitTerrainRestriction> TerrainRestrictions;
};

UCLASS(BlueprintType)
class DREAMOFPADMA_API UPadmaACTWeaponDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FName DefinitionId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity", meta = (ClampMin = "1"))
	int32 ContentVersion = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Display")
	FText DisplayName;

	/** Assign exactly one visual type. Equipment rules belong to the roster task. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	TSoftObjectPtr<UStaticMesh> StaticModel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	TSoftObjectPtr<USkeletalMesh> SkeletalModel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	FName AttachmentSocket;
};

/**
 * Read-only validation input. Declared executor IDs are authoring vocabulary,
 * not evidence that native GAS implementations have been registered.
 */
USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaACTAuthoringReferences
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ACT References")
	TSet<FName> TerrainIds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ACT References")
	TSet<FName> AbilityImplementationIds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ACT References")
	TSet<FName> ActivationBindingIds;
};

/** One editable validation entry point; owns static references, never roster/run state. */
UCLASS(BlueprintType)
class DREAMOFPADMA_API UPadmaACTAuthoringCatalog : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ACT Authoring")
	TArray<TSoftObjectPtr<UPadmaACTCharacterDefinition>> Characters;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ACT Authoring")
	TArray<TSoftObjectPtr<UPadmaACTWeaponDefinition>> Weapons;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ACT Authoring")
	FPadmaACTAuthoringReferences References;

	UFUNCTION(BlueprintCallable, Category = "ACT Authoring")
	FPadmaACTAuthoringReport ValidateCatalog() const;

	/** Select this catalog in the editor, then press Validate Authoring. */
	UFUNCTION(CallInEditor, Category = "ACT Authoring")
	void ValidateAuthoring() const;
};
