#pragma once

#include "CoreMinimal.h"
#include "Presentation/Models/PadmaModelPresentation.h"
#include "PadmaPresentationCatalog.generated.h"

class UPadmaACTCharacterDefinition;
class UPadmaACTWeaponDefinition;

UENUM(BlueprintType)
enum class EPadmaPresentationSource : uint8 { Model = 0, ACTCharacter = 2, ACTWeapon = 3 };

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaPresentationKey
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Definition")
	EPadmaPresentationSource Domain = EPadmaPresentationSource::Model;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Definition")
	FName DefinitionId;
	bool operator==(const FPadmaPresentationKey& Other) const
	{
		return Domain == Other.Domain && DefinitionId == Other.DefinitionId;
	}
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaPresentationCatalogEntry
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Definition")
	FPadmaPresentationKey Key;
	/** Fill exactly the reference selected by Key.Domain. No override/fallback chain. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Source")
	TSoftObjectPtr<UPadmaModelDefinition> Model;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Source")
	TSoftObjectPtr<UPadmaACTCharacterDefinition> ACTCharacter;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Source")
	TSoftObjectPtr<UPadmaACTWeaponDefinition> ACTWeapon;
};

/** Game composition adapter, not a gameplay inventory or a second skill-table store. */
UCLASS(BlueprintType)
class DREAMOFPADMA_API UPadmaPresentationCatalog : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Catalog")
	TArray<FPadmaPresentationCatalogEntry> Entries;
	/** Explicit load boundary, never called every frame. None selects all ACT rows. */
	UFUNCTION(BlueprintCallable, Category = "Authoring Preview")
	FPadmaResolvedPresentation Resolve(FPadmaPresentationKey Key, FName SelectedRow) const;
};
