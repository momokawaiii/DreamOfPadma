#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/Content/PadmaContentTypes.h"
#include "Presentation/Models/PadmaModelPresentation.h"
#include "PadmaContentCatalog.generated.h"

class UPadmaACTCharacterDefinition;
class UPadmaACTWeaponDefinition;
class UPadmaACTSkillDefinition;
class UTexture2D;
class UAnimSequence;
class UAnimMontage;
class UNiagaraSystem;
class ULevelSequence;
class UPadmaWorldMapAsset;
class UPadmaMapVisualTheme;

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaModelBinding
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	TSoftObjectPtr<UPadmaModelDefinition> Definition;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Art Hooks")
	TSoftObjectPtr<UTexture2D> Portrait;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Art Hooks")
	TSoftObjectPtr<UAnimSequence> Pose;
};

/** Optional presentation references. An effect asset never decides hits or settlement. */
USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaEffectBinding
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Art Hooks")
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Art Hooks")
	TSoftObjectPtr<UTexture2D> Icon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Art Hooks")
	TSoftObjectPtr<UNiagaraSystem> Niagara;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Art Hooks")
	TSoftObjectPtr<ULevelSequence> LevelSequence;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Art Hooks")
	TSoftObjectPtr<UAnimMontage> Montage;
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaNodePresentationBinding
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Art Hooks")
	FName NodeId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Art Hooks")
	FName GroundModelId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Art Hooks")
	FName BuildingModelId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Art Hooks")
	FName EffectId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Art Hooks")
	TSoftObjectPtr<UWorld> Scene;
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaACTCharacterBinding
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	TSoftObjectPtr<UPadmaACTCharacterDefinition> Definition;
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaACTWeaponBinding
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	TSoftObjectPtr<UPadmaACTWeaponDefinition> Definition;
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaACTSkillBinding
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	TSoftObjectPtr<UPadmaACTSkillDefinition> Definition;
};

/** Small Demo load boundary. Tables can be split by era/package without changing runtime IDs. */
UCLASS(BlueprintType)
class DREAMOFPADMA_API UPadmaContentCatalog : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Map")
    TSoftObjectPtr<UPadmaWorldMapAsset> WorldMap;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Map")
    TSoftObjectPtr<UPadmaMapVisualTheme> MapVisualTheme;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	int32 ContentVersion = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	FString Provenance = TEXT("TASK-046 temporary UE Demo; accepted HTML fixture values");
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tables", meta = (RequiredAssetDataTags = "RowStructure=/Script/DreamOfPadma.PadmaCardDefinitionRow"))
	TArray<TSoftObjectPtr<UDataTable>> CardTables;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tables", meta = (RequiredAssetDataTags = "RowStructure=/Script/DreamOfPadma.PadmaWorldNodeDefinitionRow"))
	TArray<TSoftObjectPtr<UDataTable>> NodeTables;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tables", meta = (RequiredAssetDataTags = "RowStructure=/Script/DreamOfPadma.PadmaWorldEdgeDefinitionRow"))
	TArray<TSoftObjectPtr<UDataTable>> EdgeTables;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tables", meta = (RequiredAssetDataTags = "RowStructure=/Script/DreamOfPadma.PadmaBalanceParameterRow"))
	TArray<TSoftObjectPtr<UDataTable>> ProfileTables;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tables", meta = (RequiredAssetDataTags = "RowStructure=/Script/DreamOfPadma.PadmaEncounterSkillEffectRow"))
	TArray<TSoftObjectPtr<UDataTable>> EncounterSkillTables;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tables", meta = (RequiredAssetDataTags = "RowStructure=/Script/DreamOfPadma.PadmaACTSkillEffectRow"))
	TArray<TSoftObjectPtr<UDataTable>> ACTSkillTables;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tables", meta = (RequiredAssetDataTags = "RowStructure=/Script/DreamOfPadma.PadmaACTCharacterDefinitionRow"))
	TArray<TSoftObjectPtr<UDataTable>> ACTCharacterTables;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tables", meta = (RequiredAssetDataTags = "RowStructure=/Script/DreamOfPadma.PadmaACTWeaponDefinitionRow"))
	TArray<TSoftObjectPtr<UDataTable>> ACTWeaponTables;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tables", meta = (RequiredAssetDataTags = "RowStructure=/Script/DreamOfPadma.PadmaAppearanceDefinitionRow"))
	TArray<TSoftObjectPtr<UDataTable>> HomeCharacterTables;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tables", meta = (RequiredAssetDataTags = "RowStructure=/Script/DreamOfPadma.PadmaAppearanceDefinitionRow"))
	TArray<TSoftObjectPtr<UDataTable>> HomeOutfitTables;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tables", meta = (RequiredAssetDataTags = "RowStructure=/Script/DreamOfPadma.PadmaAppearanceDefinitionRow"))
	TArray<TSoftObjectPtr<UDataTable>> HomeWeaponTables;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tables", meta = (RequiredAssetDataTags = "RowStructure=/Script/DreamOfPadma.PadmaAppearanceDefinitionRow"))
	TArray<TSoftObjectPtr<UDataTable>> HomePoseTables;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tables", meta = (RequiredAssetDataTags = "RowStructure=/Script/DreamOfPadma.PadmaFPSCharacterDefinitionRow"))
	TArray<TSoftObjectPtr<UDataTable>> FPSCharacterTables;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tables", meta = (RequiredAssetDataTags = "RowStructure=/Script/DreamOfPadma.PadmaFPSWeaponDefinitionRow"))
	TArray<TSoftObjectPtr<UDataTable>> FPSWeaponTables;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tables", meta = (RequiredAssetDataTags = "RowStructure=/Script/DreamOfPadma.PadmaDialogueLineRow"))
	TArray<TSoftObjectPtr<UDataTable>> DialogueTables;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ordering")
	TArray<FName> CardOrder;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ordering")
	TArray<FName> NodeOrder;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ordering")
	TArray<FName> SkillOrder;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ordering")
	TArray<FName> Eras;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ordering")
	TArray<FName> Phases;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Assets")
	TArray<FPadmaModelBinding> Models;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Assets")
	TArray<FPadmaEffectBinding> Effects;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Assets")
	TArray<FPadmaNodePresentationBinding> NodePresentation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Assets")
	TArray<FPadmaACTCharacterBinding> ACTCharacterDefinitions;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Assets")
	TArray<FPadmaACTWeaponBinding> ACTWeaponDefinitions;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Assets")
	TArray<FPadmaACTSkillBinding> ACTSkillDefinitions;

	/** Synchronous once at an explicit boundary. On failure Out is untouched. */
	bool Load(FPadmaContentSnapshot& Out, FString& Error) const;
	UFUNCTION(BlueprintCallable, Category = "Content")
	bool ValidateCatalog(FString& Error) const;
	UFUNCTION(BlueprintCallable, Category = "Content")
	FPadmaResolvedPresentation ResolveModel(FName Id) const;
	/** Null on missing/duplicate ID; optional soft art refs remain caller-loaded. */
	const FPadmaModelBinding* FindModelBinding(FName Id) const;
	const FPadmaEffectBinding* FindEffectBinding(FName Id) const;
	const FPadmaNodePresentationBinding* FindNodePresentation(FName NodeId) const;
	UFUNCTION(BlueprintCallable, Category = "Content")
	UPadmaACTCharacterDefinition* ResolveACTCharacter(FName Id) const;
	UFUNCTION(BlueprintCallable, Category = "Content")
	UPadmaACTWeaponDefinition* ResolveACTWeapon(FName Id) const;
	UFUNCTION(BlueprintCallable, Category = "Content")
	UPadmaACTSkillDefinition* ResolveACTSkill(FName Id) const;
};
