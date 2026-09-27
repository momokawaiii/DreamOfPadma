#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Gameplay/ACT/Runtime/PadmaACTMelee.h"
#include "Gameplay/ACT/Runtime/PadmaACTActions.h"
#include "Gameplay/ACT/Runtime/PadmaACTCamera.h"
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
	PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;
	Melee = CreateDefaultSubobject<UPadmaACTMeleeComponent>(TEXT("ACTMelee"));
    Actions = CreateDefaultSubobject<UPadmaACTActionsComponent>(TEXT("ACTActions"));
    ACTCamera=CreateDefaultSubobject<UPadmaACTCameraComponent>(TEXT("ACTCamera"));
    ACTCamera->SetupAttachment(GetRootComponent());
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

void APadmaCombatUnit::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    // Bootstrap an empty Blueprint instance only; reconstruction never replaces authored overrides.
    if (!GetMesh()->GetSkeletalMeshAsset() && (!SceneSpec.Presentation.ACTDefinition.IsNull() || !SceneSpec.Presentation.Model.IsNull()))
        ApplyScenePresentation();
    Placeholder->SetVisibility(GetMesh()->GetSkeletalMeshAsset()==nullptr);
}

void APadmaCombatUnit::ApplyScenePresentation()
{
    Modify(); GetMesh()->Modify();
    auto* D=SceneSpec.Presentation.ACTDefinition.LoadSynchronous();
    auto* Model=D ? D->Model.LoadSynchronous() : SceneSpec.Presentation.Model.LoadSynchronous();
    if (Model) GetMesh()->SetSkeletalMesh(Model);
    if (auto* Anim=D ? D->AnimationClass.LoadSynchronous() : SceneSpec.Presentation.AnimationClass.LoadSynchronous()) GetMesh()->SetAnimInstanceClass(Anim);
    GetMesh()->SetRelativeLocation(FVector(0,0,-90));
    Placeholder->SetVisibility(!Model);
    if (D)
    {
        GetCapsuleComponent()->SetCapsuleSize(D->CapsuleRadius,D->CapsuleHalfHeight);
        GetMesh()->SetRelativeLocation(FVector(0,0,-D->CapsuleHalfHeight));
        if (auto* Profile=D->MeleeProfile.LoadSynchronous())
        {
            GetMesh()->SetRelativeRotation(Profile->MeshRelativeRotation);
            TArray<UStaticMeshComponent*> Components; GetComponents(Components);
            for (int32 Index=0;Index<Profile->Weapons.Num();++Index)
            {
                const auto& W=Profile->Weapons[Index];
                const FName Tag(*FString::Printf(TEXT("Padma.SceneWeapon.%d"),Index));
                UStaticMeshComponent* C=nullptr;
                for (auto* Existing:Components) if (Existing->ComponentHasTag(Tag)) { C=Existing; break; }
                if (!C) { C=NewObject<UStaticMeshComponent>(this,NAME_None,RF_Transactional); C->ComponentTags.Add(Tag); AddInstanceComponent(C); C->RegisterComponent(); }
                C->Modify(); C->SetStaticMesh(W.Mesh.LoadSynchronous()); C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
                C->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale,W.StowedSocket.IsNone()?W.Socket:W.StowedSocket);
                C->SetRelativeTransform(W.StowedSocket.IsNone()?FTransform::Identity:W.StowedTransform);
                for(int32 Slot=0;Slot<W.PresentationMaterials.Num();++Slot) if(auto* Material=W.PresentationMaterials[Slot].LoadSynchronous()) C->SetMaterial(Slot,Material);
            }
        }
    }
}

void APadmaCombatUnit::InitializeCombat(UPadmaCombatComponent* Battle, const FPadmaCombatUnitSpec& InSpec, EPadmaCombatMode Mode, bool bSourceOnly, bool bPreservePresentation)
{
	Combat = Battle;
	Spec = InSpec;
	bCardSource = bSourceOnly;
	bGASCleaned = false;
	bScenePresentation = bPreservePresentation;
	bActed=false; WindupRemaining=0; CombatVelocity=FVector::ZeroVector;
	ReadyTime = 100.f / FMath::Max(1.f, Spec.Speed);
	AttackCooldown = Spec.InitialAttackDelay;
	AbilitySystem->InitAbilityActorInfo(this, this);
	AbilitySystem->AddAttributeSetSubobject(Attributes.Get());
	ApplyAttribute(this, UPadmaCombatAttributes::GetMaxHealthAttribute(), Spec.MaxHealth, true, TEXT("GE.Padma.Attribute.Init"));
	ApplyAttribute(this, UPadmaCombatAttributes::GetHealthAttribute(), Spec.Health, true, TEXT("GE.Padma.Attribute.Init"));
	ApplyAttribute(this, UPadmaCombatAttributes::GetShieldAttribute(), 0, true, TEXT("GE.Padma.Attribute.Init"));
	ApplyAttribute(this, UPadmaCombatAttributes::GetBlockAttribute(), 0, true, TEXT("GE.Padma.Attribute.Init"));
	TSubclassOf<UGameplayAbility> AbilityClass = Mode == EPadmaCombatMode::Encounter
		? UPadmaEncounterAbility::StaticClass() : UPadmaACTAbility::StaticClass();
	ModeAbility = AbilitySystem->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1));
	if (!bScenePresentation) SetActorLocation(Spec.Location);
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
	if (!bScenePresentation)
	{
	if (auto* LoadedSkeletal = Spec.Presentation.Model.LoadSynchronous())
	{
		GetMesh()->SetSkeletalMesh(LoadedSkeletal);
		GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
		if (auto* Animation = Spec.Presentation.AnimationClass.LoadSynchronous()) GetMesh()->SetAnimInstanceClass(Animation);
		Placeholder->SetVisibility(false);
	}
	else if (auto* LoadedStatic = Spec.Presentation.StaticModel.LoadSynchronous()) Placeholder->SetStaticMesh(LoadedStatic);
	}
	Placeholder->SetVisibility(GetMesh()->GetSkeletalMeshAsset()==nullptr);
	if (Mode == EPadmaCombatMode::ACT)
		if (auto* Definition = Spec.Presentation.ACTDefinition.LoadSynchronous())
		{
			if (auto* Profile = Definition->MeleeProfile.LoadSynchronous()) ensureMsgf(Melee->Configure(Profile), TEXT("Invalid ACT melee profile"));
            if (Definition->bUseCharacterActions) ensureMsgf(Actions->Configure(Definition),TEXT("Invalid ACT character actions"));
            if (Spec.bPlayer && Definition->bUseCharacterActions && !Definition->CameraProfile.IsNull())
                ensureMsgf(ACTCamera->Configure(Definition->CameraProfile.LoadSynchronous()),TEXT("Invalid ACT camera profile"));
		}
}

void APadmaCombatUnit::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);
    Actions->Landed();
}
void APadmaCombatUnit::CalcCamera(float DeltaTime,FMinimalViewInfo& OutResult)
{
    if(ACTCamera && ACTCamera->IsConfigured()) ACTCamera->GetCameraView(DeltaTime,OutResult);
    else Super::CalcCamera(DeltaTime,OutResult);
}

void APadmaCombatUnit::ApplyAttribute(APadmaCombatUnit* Source, const FGameplayAttribute& Attribute, float Magnitude, bool bOverride, FName GameplayEffectId)
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
	if (Combat.IsValid()) Combat->RecordGameplayEffect(Source, this, Attribute, Magnitude, bOverride, GameplayEffectId);
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
	Result.MagicDefense = Spec.MagicDefense;
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
	Melee->FinishAttack(true);
	CombatVelocity = FVector::ZeroVector;
    if (Actions->IsConfigured()) { GetCharacterMovement()->StopMovementImmediately(); GetCharacterMovement()->DisableMovement(); SetActorTickEnabled(false); }
	WindupRemaining = 0;
	StopAnimMontage();
	AbilitySystem->CancelAllAbilities();
	AbilitySystem->ClearAllAbilities();
	AbilitySystem->RemoveActiveEffects(FGameplayEffectQuery());
	AbilitySystem->ClearActorInfo();
	Actions->ResetConfiguration();
	Melee->ResetConfiguration();
	ACTCamera->ResetView(); ACTCamera->SetComponentTickEnabled(false);
	Combat.Reset();
	ModeAbility = FGameplayAbilitySpecHandle();
}

void APadmaCombatUnit::EndPlay(const EEndPlayReason::Type Reason)
{
	CleanupGAS();
	Super::EndPlay(Reason);
}
