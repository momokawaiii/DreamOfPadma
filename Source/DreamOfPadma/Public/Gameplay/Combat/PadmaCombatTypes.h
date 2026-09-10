#pragma once

#include "CoreMinimal.h"
#include "Core/Content/PadmaContentTypes.h"
#include "PadmaCombatTypes.generated.h"

class UAnimInstance;
class UAnimMontage;
class USkeletalMesh;
class UStaticMesh;
class UPadmaACTCharacterDefinition;

UENUM(BlueprintType)
enum class EPadmaCombatMode : uint8 { Encounter, ACT };

UENUM(BlueprintType)
enum class EPadmaCombatSource : uint8 { Unit, Card, Environment };

/** Presentation is supplied by Game composition; none of these assets authorize hits. */
USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaCombatPresentation
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere) TSoftObjectPtr<USkeletalMesh> Model;
	UPROPERTY(EditAnywhere) TSoftClassPtr<UAnimInstance> AnimationClass;
	UPROPERTY(EditAnywhere) TSoftObjectPtr<UStaticMesh> StaticModel;
	UPROPERTY(EditAnywhere) TSoftObjectPtr<UAnimMontage> AttackMontage;
	UPROPERTY(EditAnywhere) TSoftObjectPtr<UPadmaACTCharacterDefinition> ACTDefinition;
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaCombatUnitSpec
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere) FName Id;
	UPROPERTY(EditAnywhere) FName DefinitionId;
	UPROPERTY(EditAnywhere) FText DisplayName;
	UPROPERTY(EditAnywhere) bool bPlayer = true;
	UPROPERTY(EditAnywhere) bool bCore = false;
	UPROPERTY(EditAnywhere) FName Family = TEXT("C");
	UPROPERTY(EditAnywhere) FName Attribute;
	UPROPERTY(EditAnywhere) float Health = 1;
	UPROPERTY(EditAnywhere) float MaxHealth = 1;
	UPROPERTY(EditAnywhere) float Attack = 0;
	UPROPERTY(EditAnywhere) float Defense = 0;
	UPROPERTY(EditAnywhere) float AttackCost = 0;
	UPROPERTY(EditAnywhere) int32 MaxTargets = 1;
	UPROPERTY(EditAnywhere) float Speed = 1;
	UPROPERTY(EditAnywhere) float AttackRange = 105;
	UPROPERTY(EditAnywhere) float AttackInterval = 0.55f;
	UPROPERTY(EditAnywhere) float InitialAttackDelay = 0;
	UPROPERTY(EditAnywhere) FVector Location = FVector::ZeroVector;
	UPROPERTY(EditAnywhere) FPadmaCombatPresentation Presentation;
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaCombatUnitSnapshot
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) FName Id;
	UPROPERTY(BlueprintReadOnly) FName DefinitionId;
	UPROPERTY(BlueprintReadOnly) FText DisplayName;
	UPROPERTY(BlueprintReadOnly) bool bPlayer = false;
	UPROPERTY(BlueprintReadOnly) bool bCore = false;
	UPROPERTY(BlueprintReadOnly) float Health = 0;
	UPROPERTY(BlueprintReadOnly) float MaxHealth = 0;
	UPROPERTY(BlueprintReadOnly) float Shield = 0;
	UPROPERTY(BlueprintReadOnly) float Block = 0;
	UPROPERTY(BlueprintReadOnly) float Attack = 0;
	UPROPERTY(BlueprintReadOnly) float Defense = 0;
	UPROPERTY(BlueprintReadOnly) float ReadyTime = 0;
	UPROPERTY(BlueprintReadOnly) bool bActed = false;
	UPROPERTY(BlueprintReadOnly) float WindupRemaining = 0;
	UPROPERTY(BlueprintReadOnly) FVector Location = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaCombatReceipt
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) EPadmaCombatMode Mode = EPadmaCombatMode::Encounter;
	UPROPERTY(BlueprintReadOnly) EPadmaCombatSource SourceKind = EPadmaCombatSource::Unit;
	UPROPERTY(BlueprintReadOnly) FName SourceId;
	UPROPERTY(BlueprintReadOnly) FName TargetId;
	UPROPERTY(BlueprintReadOnly) FName ActionOwnerId;
	UPROPERTY(BlueprintReadOnly) FName Effect;
	UPROPERTY(BlueprintReadOnly) float Amount = 0;
	UPROPERTY(BlueprintReadOnly) float Absorbed = 0;
	UPROPERTY(BlueprintReadOnly) bool bBlocked = false;
	UPROPERTY(BlueprintReadOnly) bool bTrueDamage = false;
	UPROPERTY(BlueprintReadOnly) int32 Wave = 0;
	UPROPERTY(BlueprintReadOnly) FPadmaCombatUnitSnapshot Before;
	UPROPERTY(BlueprintReadOnly) FPadmaCombatUnitSnapshot After;
};

/** The two typed maps deliberately do not resolve through each other. */
USTRUCT(BlueprintType)
struct DREAMOFPADMA_API FPadmaCombatSetup
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere) EPadmaCombatMode Mode = EPadmaCombatMode::Encounter;
	UPROPERTY(EditAnywhere) TArray<FPadmaCombatUnitSpec> Units;
	UPROPERTY(EditAnywhere) TMap<FName, FPadmaEncounterSkillEffectRow> EncounterSkills;
	UPROPERTY(EditAnywhere) TMap<FName, FPadmaACTSkillEffectRow> ACTSkills;
	UPROPERTY(EditAnywhere) int32 Era = 0;
	UPROPERTY(EditAnywhere) float EnemyTurnDelay = 0.9f;
	UPROPERTY(EditAnywhere) float EnemyAttackInterval = 1.5f;
	/** Accepted HTML arena coordinates; root may offset the entire arena. */
	UPROPERTY(EditAnywhere) FVector ArenaOrigin = FVector::ZeroVector;
	UPROPERTY(EditAnywhere) FVector2D ArenaMinimum = FVector2D(35,55);
	UPROPERTY(EditAnywhere) FVector2D ArenaMaximum = FVector2D(765,435);
};
