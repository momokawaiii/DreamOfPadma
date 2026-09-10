#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Core/Cards/PadmaCardMobility.h"
#include "PadmaContentTypes.generated.h"

/** Immutable authoring values. No Actor, presentation asset, or gameplay executor references. */
USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaCardDefinitionRow : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Family;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Attribute;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Kind;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText Icon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText SourcePage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText Coverage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName ModelId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	int32 Cost = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	int32 Stability = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	int32 Affinity = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	int32 DailyFlow = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	int32 DailyLimit = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	int32 Attack = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	int32 Defense = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	int32 Health = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	int32 AttackCost = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	int32 MaxTargets = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FVector2D Tendency = FVector2D::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FVector2D Field = FVector2D::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FVector2D Target = FVector2D::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	TMap<FName, int32> Tags;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FPadmaCardMobilityDefinition Mobility;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	double MinFaith = 50.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName ForbiddenTerrain;
	/** Source provenance; values are the accepted HTML fixture, not final balance. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FString Provenance = TEXT("TASK-046 temporary UE Demo / Prototypes/HTML/data.js");
};

/** An anchor fixes authored identity and location; it does not imply a mandatory route. */
UENUM(BlueprintType)
enum class EPadmaMapAnchorKind : uint8
{
	None,
	FixedNPC,
	MainStory
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaWorldNodeDefinitionRow : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText Subtitle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Type;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Terrain;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FVector2D Position = FVector2D::ZeroVector;
	/** Tutorial hex values are authoritative only when MapLayout.bHexWilderness is enabled. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hex Map")
	bool bHabitable = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hex Map")
	bool bTraversable = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hex Map")
	bool bWilderness = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hex Map", meta = (ClampMin = "0", ClampMax = "1"))
	double Elevation = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hex Map")
	int32 HexQ = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hex Map")
	int32 HexR = 0;
	/** Optional authoring grouping. Stage alone never locks movement or story. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map", meta = (ClampMin = "0"))
	int32 Stage = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	EPadmaMapAnchorKind AnchorKind = EPadmaMapAnchorKind::None;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	FName NPCId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	FName DialogueId;
	/** Set only when this node's encounter completes; no automatic progression requirement. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	FName CompletionFlag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText Icon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName InitialOwner;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	int32 EnemyCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText Description;
	/** Source provenance; values are the accepted HTML fixture, not final balance. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FString Provenance = TEXT("TASK-046 temporary UE Demo / Prototypes/HTML/data.js");
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaWorldEdgeDefinitionRow : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName From;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName To;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName UnlockFlag;
	/** Source provenance; values are the accepted HTML fixture, not final balance. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FString Provenance = TEXT("TASK-046 temporary UE Demo / Prototypes/HTML/data.js");
};

/** A one-time story branch decision at an authored checkpoint, never inferred from travel. */
USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaStoryCheckpointRow : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story Checkpoint")
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story Checkpoint")
	FName NodeId;
	/** Identity of an anchored NPC whose earlier encounter is tested at this checkpoint. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story Checkpoint")
	FName NPCId;
	/** True tests completed encounter; false tests whether the NPC has been seen. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story Checkpoint")
	bool bRequireCompletion = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story Checkpoint")
	FName MatchedFlag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story Checkpoint")
	FName MissedFlag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story Checkpoint")
	FName MatchedDialogueId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story Checkpoint")
	FName MissedDialogueId;
};

/** Serializable map values frozen into a run. Edges are explicitly directed. */
/** Frozen projected grid values. Defaults reproduce tutorial generator v1 when absent in an older save. */
USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaHexGridConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	double Radius = 5.7;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	double ProjectionY = .65;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	FVector2D Origin = FVector2D(5., 4.);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	int32 MinQ = -9;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	int32 MaxQ = 10;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	int32 MinR = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	int32 MaxR = 16;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	double MinU = 3.;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	double MaxU = 97.;

	bool operator==(const FPadmaHexGridConfig& Other) const
	{
		return Radius == Other.Radius && ProjectionY == Other.ProjectionY && Origin == Other.Origin
			&& MinQ == Other.MinQ && MaxQ == Other.MaxQ && MinR == Other.MinR && MaxR == Other.MaxR
			&& MinU == Other.MinU && MaxU == Other.MaxU;
	}
	bool operator!=(const FPadmaHexGridConfig& Other) const { return !(*this == Other); }
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaMapLayout
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	FName MapId;
	/** Opt-in tutorial topology: explicit six-neighbor edges may contain cycles. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	bool bHexWilderness = false;
	/** Serialized projection shared by generation, presentation and save validation. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	FPadmaHexGridConfig HexGrid;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map", meta = (ClampMin = "1"))
	int32 Version = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map", meta = (ClampMin = "1"))
	int32 GeneratorVersion = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	int32 Seed = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	FName HomeNode;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	FName BossNode;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map", meta = (TitleProperty = "Id"))
	TArray<FPadmaWorldNodeDefinitionRow> Nodes;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map", meta = (TitleProperty = "Id"))
	TArray<FPadmaWorldEdgeDefinitionRow> Edges;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map", meta = (TitleProperty = "Id"))
	TArray<FPadmaStoryCheckpointRow> Checkpoints;
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaBalanceParameterRow : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	double Value = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	double Minimum = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	double Maximum = 0;
	/** Source provenance; values are the accepted HTML fixture, not final balance. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FString Provenance = TEXT("TASK-046 temporary UE Demo / Prototypes/HTML/data.js");
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaACTCharacterDefinitionRow : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FString Palette;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Trait;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName ModelId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName DefinitionId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName SpeedBonusTerrain;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName TraitDisabledTerrain;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	double TerrainSpeedMultiplier = 1.0;
	/** Source provenance; values are the accepted HTML fixture, not final balance. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FString Provenance = TEXT("TASK-046 temporary UE Demo / Prototypes/HTML/data.js");
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaACTWeaponDefinitionRow : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	double DamageMultiplier = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	double RangeMultiplier = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	double IntervalMultiplier = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName ModelId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName DefinitionId;
	/** Source provenance; values are the accepted HTML fixture, not final balance. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FString Provenance = TEXT("TASK-046 temporary UE Demo / Prototypes/HTML/data.js");
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaAppearanceDefinitionRow : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Domain;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FString Palette;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FString Hair;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	TArray<FName> CompatiblePoseIds;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName ModelId;
	/** Source provenance; values are the accepted HTML fixture, not final balance. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FString Provenance = TEXT("TASK-046 temporary UE Demo / Prototypes/HTML/data.js");
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaFPSCharacterDefinitionRow : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FString Palette;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName ModelId;
	/** Source provenance; values are the accepted HTML fixture, not final balance. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FString Provenance = TEXT("TASK-046 temporary UE Demo / Prototypes/HTML/data.js");
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaFPSWeaponDefinitionRow : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName ModelId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText Coverage;
	/** Source provenance; values are the accepted HTML fixture, not final balance. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FString Provenance = TEXT("TASK-046 temporary UE Demo / Prototypes/HTML/data.js");
};

/** Neutral values copied from the explicitly configured mode table. Not an executor. */
USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaSkillEffectRowBase : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText Icon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText SourcePage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Target;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	double FlowCost = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	double CalculationCost = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText CostSource;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName EffectId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	double Magnitude = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	int32 RepeatCount = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	bool bTrueDamage = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	bool bFaithIfUnacted = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName DefinitionId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FString Provenance = TEXT("TASK-046 temporary UE Demo / Prototypes/HTML/data.js and engine.js");
};

/** Intentionally different reflected row types: Encounter and ACT cannot exchange tables. */
USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaEncounterSkillEffectRow : public FPadmaSkillEffectRowBase
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaACTSkillEffectRow : public FPadmaSkillEffectRowBase
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaDialogueChoice
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText Label;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Next;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	bool bComplete = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	bool bCancel = false;

};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaDialogueLineRow : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName DialogueId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName LineId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText Title;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	bool bStart = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText Speaker;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FText Text;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Next;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	TArray<FPadmaDialogueChoice> Choices;

};

/** Fully checked value snapshot. Published atomically by the Game catalog. */
struct DREAMOFPADMA_API FPadmaContentSnapshot
{
	/** Empty MapId accepts legacy table-only fixtures; configured maps are validated as one layout. */
	FPadmaMapLayout MapLayout;
	/** Original table graph retained only to migrate a pre-layout save without substituting new topology. */
	FPadmaMapLayout LegacyMapLayout;
	TMap<FName, FPadmaCardDefinitionRow> Cards;
	TMap<FName, FPadmaWorldNodeDefinitionRow> Nodes;
	TArray<FPadmaWorldEdgeDefinitionRow> Edges;
	TMap<FName, FPadmaBalanceParameterRow> Profiles;
	TMap<FName, FPadmaEncounterSkillEffectRow> EncounterSkills;
	TMap<FName, FPadmaACTSkillEffectRow> ACTSkills;
	TMap<FName, FPadmaACTCharacterDefinitionRow> ACTCharacters;
	TMap<FName, FPadmaACTWeaponDefinitionRow> ACTWeapons;
	TMap<FName, FPadmaAppearanceDefinitionRow> HomeCharacters;
	TMap<FName, FPadmaAppearanceDefinitionRow> HomeOutfits;
	TMap<FName, FPadmaAppearanceDefinitionRow> HomeWeapons;
	TMap<FName, FPadmaAppearanceDefinitionRow> HomePoses;
	TMap<FName, FPadmaFPSCharacterDefinitionRow> FPSCharacters;
	TMap<FName, FPadmaFPSWeaponDefinitionRow> FPSWeapons;
	TMap<FName, FPadmaDialogueLineRow> Dialogues;
	TArray<FName> CardOrder;
	TArray<FName> NodeOrder;
	TArray<FName> SkillOrder;
	TArray<FName> Eras;
	TArray<FName> Phases;
	/** Unknown keys fail loudly instead of silently introducing zero-valued rules. */
	double P(FName Id) const;
	bool Validate(FString& Error) const;
};
