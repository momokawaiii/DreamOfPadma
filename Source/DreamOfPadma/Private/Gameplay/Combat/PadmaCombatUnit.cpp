#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Gameplay/Combat/PadmaCombatAttributes.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "Gameplay/Encounter/PadmaEncounterAbility.h"
#include "Gameplay/ACT/Runtime/PadmaACTAbility.h"
#include "Gameplay/ACT/Authoring/PadmaACTAuthoring.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"

APadmaCombatUnit::APadmaCombatUnit()
{
	PrimaryActorTick.bCanEverTick = false;
	AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("NativeASC"));
	AbilitySystem->SetIsReplicated(false);
	Attributes = CreateDefaultSubobject<UPadmaCombatAttributes>(TEXT("CombatAttributes"));
	GetCapsuleComponent()->InitCapsuleSize(18, 36);
	// Deterministic Demo planar motor is owned by its ACT component, not autonomous Character movement.
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);
	Placeholder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaceholderVisual"));
	Placeholder->SetupAttachment(GetRootComponent());
	Placeholder->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cylinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (Cylinder.Succeeded()) Placeholder->SetStaticMesh(Cylinder.Object);
	Placeholder->SetRelativeScale3D(FVector(.36, .36, .72));
	Placeholder->SetRelativeLocation(FVector(0,0,-54)); // Demo arena unit centers are 90 above the floor.
}

void APadmaCombatUnit::InitializeCombat(UPadmaCombatComponent* Battle, const FPadmaCombatUnitSpec& InSpec, EPadmaCombatMode Mode, bool bSourceOnly)
{
	Combat = Battle;
	Spec = InSpec;
	bCardSource = bSourceOnly;
	bGASCleaned = false;
	ReadyTime = 100.f / FMath::Max(1.f, Spec.Speed);
	AttackCooldown = Spec.InitialAttackDelay;
	AbilitySystem->InitAbilityActorInfo(this, this);
	AbilitySystem->AddAttributeSetSubobject(Attributes.Get());
	ApplyAttribute(this, UPadmaCombatAttributes::GetMaxHealthAttribute(), Spec.MaxHealth, true);
	ApplyAttribute(this, UPadmaCombatAttributes::GetHealthAttribute(), Spec.Health, true);
	ApplyAttribute(this, UPadmaCombatAttributes::GetShieldAttribute(), 0, true);
	ApplyAttribute(this, UPadmaCombatAttributes::GetBlockAttribute(), 0, true);
	TSubclassOf<UGameplayAbility> AbilityClass = Mode == EPadmaCombatMode::Encounter
		? UPadmaEncounterAbility::StaticClass() : UPadmaACTAbility::StaticClass();
	ModeAbility = AbilitySystem->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1));
	SetActorLocation(Spec.Location);
	SetActorEnableCollision(!bSourceOnly);
	SetActorHiddenInGame(bSourceOnly);
	if (bSourceOnly) return;
	if (Mode == EPadmaCombatMode::ACT && !Spec.Presentation.ACTDefinition.IsNull())
	{
		if (auto* Definition = Spec.Presentation.ACTDefinition.LoadSynchronous())
		{
			if (Spec.Presentation.Model.IsNull()) Spec.Presentation.Model = Definition->Model;
			if (Spec.Presentation.AnimationClass.IsNull()) Spec.Presentation.AnimationClass = Definition->AnimationClass;
		}
	}
	if (auto* LoadedSkeletal = Spec.Presentation.Model.LoadSynchronous())
	{
		GetMesh()->SetSkeletalMesh(LoadedSkeletal);
		GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
		if (auto* Animation = Spec.Presentation.AnimationClass.LoadSynchronous()) GetMesh()->SetAnimInstanceClass(Animation);
		Placeholder->SetVisibility(false);
	}
	else if (auto* LoadedStatic = Spec.Presentation.StaticModel.LoadSynchronous()) Placeholder->SetStaticMesh(LoadedStatic);
}

void APadmaCombatUnit::ApplyAttribute(APadmaCombatUnit* Source, const FGameplayAttribute& Attribute, float Magnitude, bool bOverride)
{
	if (!Source || !Source->AbilitySystem || !AbilitySystem) return;
	auto* Effect = NewObject<UPadmaCombatAttributeEffect>(GetTransientPackage());
	FGameplayModifierInfo Modifier;
	Modifier.Attribute = Attribute;
	Modifier.ModifierOp = bOverride ? EGameplayModOp::Override : EGameplayModOp::Additive;
	Modifier.ModifierMagnitude = FScalableFloat(Magnitude);
	Effect->Modifiers.Add(Modifier);
	FGameplayEffectContextHandle Context = Source->AbilitySystem->MakeEffectContext();
	Context.AddSourceObject(Source);
	FGameplayEffectSpec EffectSpec(Effect, Context, 1.f);
	Source->AbilitySystem->ApplyGameplayEffectSpecToTarget(EffectSpec, AbilitySystem);
}

float APadmaCombatUnit::Health() const { return Attributes ? Attributes->GetHealth() : 0; }

FPadmaCombatUnitSnapshot APadmaCombatUnit::Snapshot() const
{
	FPadmaCombatUnitSnapshot Result;
	Result.Id = Spec.Id;
	Result.DefinitionId = Spec.DefinitionId;
	Result.DisplayName = Spec.DisplayName;
	Result.bPlayer = Spec.bPlayer;
	Result.bCore = Spec.bCore;
	Result.Health = Health();
	Result.MaxHealth = Attributes->GetMaxHealth();
	Result.Shield = Attributes->GetShield();
	Result.Block = Attributes->GetBlock();
	Result.Attack = Spec.Attack;
	Result.Defense = Spec.Defense;
	Result.ReadyTime = ReadyTime;
	Result.bActed = bActed;
	Result.WindupRemaining = WindupRemaining;
	Result.Location = GetActorLocation();
	return Result;
}

void APadmaCombatUnit::PlayAttackPresentation()
{
	if (auto* Montage = Spec.Presentation.AttackMontage.LoadSynchronous()) PlayAnimMontage(Montage);
}

void APadmaCombatUnit::CleanupGAS()
{
	if (bGASCleaned) return;
	bGASCleaned = true;
	CombatVelocity = FVector::ZeroVector;
	WindupRemaining = 0;
	StopAnimMontage();
	AbilitySystem->CancelAllAbilities();
	AbilitySystem->ClearAllAbilities();
	AbilitySystem->RemoveActiveEffects(FGameplayEffectQuery());
	AbilitySystem->ClearActorInfo();
	ModeAbility = FGameplayAbilitySpecHandle();
}

void APadmaCombatUnit::EndPlay(const EEndPlayReason::Type Reason)
{
	CleanupGAS();
	Super::EndPlay(Reason);
}
