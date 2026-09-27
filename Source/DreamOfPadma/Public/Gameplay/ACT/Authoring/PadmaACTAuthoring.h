#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "Core/Cards/PadmaCardMobility.h"
#include "Gameplay/Combat/PadmaCombatTypes.h"
#include "PadmaACTAuthoring.generated.h"

class UPadmaACTMeleeDefinition;
class UAnimInstance;
class UAnimMontage;
class USkeletalMesh;
class UStaticMesh;
class APadmaCombatUnit;

UENUM(BlueprintType)
enum class EPadmaACTActionKind : uint8 { Legacy, Attack, Skill, Jump, Plunge, Dodge, PerfectDodge, Execution };

/** Sampled presentation camera in character-mesh coordinates; never drives settlement. */
USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaACTCameraSample
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Time = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector Position = FVector::ZeroVector;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FRotator Rotation = FRotator::ZeroRotator;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float VerticalFOV = 45;
};

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

    /** Legacy preserves the existing card executor. Character actions use the native montage ability. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Action") EPadmaACTActionKind ActionKind = EPadmaACTActionKind::Legacy;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Presentation") TArray<FPadmaACTCameraSample> CameraSamples;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Presentation") float CameraDuration = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Presentation") float CameraEaseOut = .5f;
    /** Draw for weapon actions; locomotion actions may keep equipment at its stowed mounts. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Presentation") bool bDrawWeapons = true;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Action") FName StartSection;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Action") FName NextComboId;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Action") FName PerfectDodgeId;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Action", meta=(ClampMin="0.1", ClampMax="4")) float PlayRate = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Action", meta=(ClampMin="0")) float DamageScale = 1.f;
    /** Selects the target mitigation channel for ACT contact settlement. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Action") EPadmaACTDamageKind DamageKind = EPadmaACTDamageKind::Physical;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Action", meta=(ClampMin="0")) float CooldownSeconds = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Action", meta=(ClampMin="0")) float FlowCost = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Action", meta=(ClampMin="0")) float CalculationCost = 0.f;
    /** Explicit override for airborne plunge; ordinary actions use priority and Montage transition windows. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Action") bool bCanInterrupt = false;
    /** Native source skill interrupt priority; strictly higher priority can interrupt exclusive time. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Action") int32 InterruptPriority = 0;
    /** Snapshot a primary target at activation for source direct-target damage; absence never blocks activation. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Action") bool bUsePrimaryTarget = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Action", meta=(ClampMin="0")) float InputCacheSeconds = .3f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(ClampMin="0")) float DodgeSpeed = 600.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(ClampMin="0")) float PlungeSpeed = 1200.f;
    /** Capsule-foot clearance from blocking world geometry; zero retains the legacy airborne-only gate. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(ClampMin="0")) float MinPlungeHeight = 0.f;
    /** Additional safety for movement/jump after the authored recovery window opens. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement") bool bWaitForAfterimagesBeforeLocomotion = false;
    /** Block all player input through the Input.All ANS and live/pending afterimage retirement. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement") bool bBlockAllInputUntilAfterimagesRetire = false;
    /** Suspend gravity until the Montage's Plunge Descend notify. Always restored on exit. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement") bool bPlungeHoldUntilNotify = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Targeting", meta=(ClampMin="1")) float TargetRange = 250.f;
    // Retained only to deserialize older prototypes. Execution has no HP activation/settlement gate.
    UPROPERTY() float ExecutionHealthFraction = .25f;
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

	/** Optional animated ACT blade/FX profile; no Encounter fallback. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ACT Melee")
	TSoftObjectPtr<UPadmaACTMeleeDefinition> MeleeProfile;

	/** Optional for a static preview. Runtime animation readiness is checked later. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	TSoftClassPtr<UAnimInstance> AnimationClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ACT Runtime") TSoftClassPtr<APadmaCombatUnit> CharacterClass;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ACT Camera") TSoftObjectPtr<class UPadmaACTCameraDefinition> CameraProfile;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ACT Runtime") bool bUseCharacterActions = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ACT Runtime", meta=(ClampMin="1")) float WalkSpeed = 160.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ACT Runtime", meta=(ClampMin="1")) float RunSpeed = 450.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ACT Runtime", meta=(ClampMin="1")) float JumpSpeed = 600.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ACT Runtime", meta=(ClampMin="1")) float CapsuleRadius = 22.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ACT Runtime", meta=(ClampMin="1")) float CapsuleHalfHeight = 88.f;

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
