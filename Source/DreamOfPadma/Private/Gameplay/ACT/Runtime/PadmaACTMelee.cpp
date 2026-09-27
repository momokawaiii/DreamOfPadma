#include "Gameplay/ACT/Runtime/PadmaACTMelee.h"
#include "Gameplay/ACT/Runtime/PadmaACTImpactLocation.h"
#include "Components/CapsuleComponent.h"
#include "Gameplay/ACT/Runtime/PadmaACTAnimInstance.h"
#include "Gameplay/ACT/Runtime/PadmaACTActions.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Engine/SkeletalMesh.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Components/PoseableMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

UPadmaACTMeleeComponent::UPadmaACTMeleeComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
}
bool UPadmaACTMeleeComponent::Configure(UPadmaACTMeleeDefinition* Definition)
{
    auto* Unit = Cast<APadmaCombatUnit>(GetOwner());
    if (!Unit || !Definition || IsConfigured() || !FMath::IsFinite(Definition->TraceRadius) || Definition->TraceRadius < 1) return false;
    auto* Montage = Definition->AttackMontage.LoadSynchronous();
    if (!Montage || Montage->GetSectionIndex(Definition->AttackSection) == INDEX_NONE || !Unit->GetMesh()->GetSkeletalMeshAsset() || Definition->Weapons.IsEmpty()) return false;
    for (const auto& W : Definition->Weapons)
        if (!W.Mesh.LoadSynchronous() || !Unit->GetMesh()->DoesSocketExist(W.Socket) || W.BladeBase.ContainsNaN() || W.BladeTip.ContainsNaN() || FVector::Distance(W.BladeBase,W.BladeTip) > 1000) return false;
    for (const auto& W : Definition->Weapons)
        if (!W.StowedSocket.IsNone() && (!Unit->GetMesh()->DoesSocketExist(W.StowedSocket) || W.StowedTransform.ContainsNaN())) return false;
    if (Definition->bWeaponLifecycle && (!Definition->DrawWeaponMontage.LoadSynchronous() || !Definition->SheatheWeaponMontage.LoadSynchronous()
        || !FMath::IsFinite(Definition->DrawnHoldSeconds) || Definition->DrawnHoldSeconds<0
        || !FMath::IsFinite(Definition->StowedHoldSeconds) || Definition->StowedHoldSeconds<0
        || !FMath::IsFinite(Definition->WeaponDissolveSeconds) || Definition->WeaponDissolveSeconds<=0)) return false;
    Profile = Definition;
    AttackMontage = Montage;
    ActiveSection = Profile->AttackSection;
    auto* Mesh = Unit->GetMesh();
    if (!Unit->PreservesScenePresentation()) Mesh->SetRelativeRotation(Profile->MeshRelativeRotation);
    Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
    Mesh->bEnableUpdateRateOptimizations = false;
    auto* Character = Unit->Spec.Presentation.ACTDefinition.LoadSynchronous();
    if (!Character || !Character->bUseCharacterActions)
    {
        Mesh->SetAnimInstanceClass(UPadmaACTAnimInstance::StaticClass());
        auto* Anim = Cast<UPadmaACTAnimInstance>(Mesh->GetAnimInstance());
        if (!Anim) { Profile = nullptr; AttackMontage = nullptr; return false; }
        Anim->IdleAnimation = Profile->IdleAnimation.LoadSynchronous();
    }
    else if (!Mesh->GetAnimInstance()) { Profile = nullptr; AttackMontage = nullptr; return false; }
    AddTickPrerequisiteComponent(Mesh);
    TArray<UStaticMeshComponent*> SceneComponents; Unit->GetComponents(SceneComponents);
    for (int32 Index=0;Index<Profile->Weapons.Num();++Index)
    {
        const auto& W=Profile->Weapons[Index];
        UStaticMeshComponent* Component=nullptr;
        const FName Tag(*FString::Printf(TEXT("Padma.SceneWeapon.%d"),Index));
        if (Unit->PreservesScenePresentation())
            for (auto* C:SceneComponents) if (C->ComponentHasTag(Tag)) {Component=C;break;}
        if (Component && !SceneWeaponOffsets.Contains(Component))
        {
            SceneWeaponOffsets.Add(Component,Component->GetRelativeTransform().GetRelativeTransform(W.StowedSocket.IsNone()?FTransform::Identity:W.StowedTransform));
            SceneWeaponStowedSockets.Add(Component,Component->GetAttachSocketName());
        }
        if (!Component)
        {
        Component = NewObject<UStaticMeshComponent>(Unit);
        Component->SetStaticMesh(W.Mesh.LoadSynchronous());
        Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Component->SetupAttachment(Mesh, W.Socket);
        Component->RegisterComponent();
        Unit->AddInstanceComponent(Component);
        }
        for (int32 Slot=0; Slot<W.PresentationMaterials.Num() && Slot<Component->GetNumMaterials(); ++Slot)
            if (auto* Material=W.PresentationMaterials[Slot].LoadSynchronous())
                if (!Unit->PreservesScenePresentation()) Component->CreateDynamicMaterialInstance(Slot,Material);
        if (Unit->PreservesScenePresentation())
            for (int32 Slot=0;Slot<Component->GetNumMaterials();++Slot)
                if (Component->GetMaterial(Slot) && !Cast<UMaterialInstanceDynamic>(Component->GetMaterial(Slot)))
                    Component->CreateDynamicMaterialInstance(Slot);
        Weapons.Add(Component);
    }
    // Reset retained scene components even when the logical flag was already stowed.
    CompleteWeaponMontage(false);
    // Configure can replace an existing presentation AnimInstance after ASC initialization.
    Unit->GetAbilitySystemComponent()->RefreshAbilityActorInfo();
    return true;
}
void UPadmaACTMeleeComponent::ResetConfiguration()
{
    StopWeaponMontage(); FinishAttack(true);
    for(UStaticMeshComponent* W:Weapons) if(IsValid(W))
    {
        bool Scene=false; for(const FName Tag:W->ComponentTags) if(Tag.ToString().StartsWith(TEXT("Padma.SceneWeapon."))) Scene=true;
        if(!Scene) W->DestroyComponent();
    }
    Weapons.Reset(); PreviousWeapons.Reset(); Profile=nullptr; AttackMontage=nullptr;
    WeaponPhase=EWeaponPhase::Stowed; WeaponPhaseAge=0; bWeaponsDrawn=false;
    bShowcasePresentation=bShowcaseRestWeapons=bActionDrewWeapons=bWeaponIdleReleased=false;
}
bool UPadmaACTMeleeComponent::BeginAttack()
{
    auto* Unit = Cast<APadmaCombatUnit>(GetOwner());
    if (!Profile || bAttacking || !Unit || !Unit->IsAlive() || !Unit->GetBattle() || !Unit->GetBattle()->IsBattleActive() || Unit->GetBattle()->GetMode() != EPadmaCombatMode::ACT) return false;
    bAttacking = true;
    const auto* Action = Unit->GetActions()->GetActiveDefinition();
    PrepareWeaponAction(!Action || Action->bDrawWeapons);
    ++ActionGeneration;
    bCompletionPending = false;
    MontageInstanceID = INDEX_NONE;
    AttackCycle = 0;
    ConfirmedContacts = SpawnedHitEffects = SpawnedActionEffects = 0;
    Unit->CombatVelocity = FVector::ZeroVector;
    ResetCycle();
    return true;
}
bool UPadmaACTMeleeComponent::BeginAction(UAnimMontage* Montage,FName Section,float Scale)
{
    if (!Montage || bAttacking || !FMath::IsFinite(Scale) || Scale<0) return false;
    AttackMontage=Montage; ActiveSection=Section; DamageScale=Scale;
    return BeginAttack();
}
void UPadmaACTMeleeComponent::ResetCycle()
{
    ClearActionEffects(true);
    Windows.Reset();
    bComboQueued = bComboOpen = false;
    PreviousMontagePosition = 0;
    PreviousWeapons.Reset();
    for (auto Weapon : Weapons) PreviousWeapons.Add(Weapon->GetComponentTransform());
    ++AttackCycle;
}
float UPadmaACTMeleeComponent::GetAttackTime() const
{
    const auto* Unit = Cast<APadmaCombatUnit>(GetOwner());
    auto* Anim = Unit ? Unit->GetMesh()->GetAnimInstance() : nullptr;
    if (!bAttacking || !Anim) return 0.f;
    if (const auto* Instance = Anim->GetMontageInstanceForID(MontageInstanceID)) return Instance->GetPosition();
    return bCompletionPending ? AttackMontage->GetPlayLength() : Anim->Montage_GetPosition(AttackMontage);
}
void UPadmaACTMeleeComponent::CaptureMontageInstance()
{
    auto* Unit = CastChecked<APadmaCombatUnit>(GetOwner());
    auto* Anim = Unit->GetMesh()->GetAnimInstance();
    if (auto* Instance = Anim ? Anim->GetActiveInstanceForMontage(AttackMontage) : nullptr)
        MontageInstanceID = Instance->GetInstanceID();
}
void UPadmaACTMeleeComponent::SetComboWindow(bool bOpen, float Start, float End)
{
    if (!bAttacking) return;
    bComboOpen = bOpen; ComboStart = Start; ComboEnd = End;
}
bool UPadmaACTMeleeComponent::IsComboWindowOpen() const
{
    const float Position = GetAttackTime();
    return bAttacking && !bCompletionPending && bComboOpen && Position >= ComboStart && Position < ComboEnd;
}
bool UPadmaACTMeleeComponent::AcceptsNotify(const UAnimSequenceBase* Animation) const
{
    return bAttacking && Animation == AttackMontage;
}
bool UPadmaACTMeleeComponent::SetAttackPlayRate(float Rate)
{
    if (!FMath::IsFinite(Rate) || Rate < .1f || Rate > 4.f) return false;
    PlayRate = Rate;
    if (auto* Unit = Cast<APadmaCombatUnit>(GetOwner()); bAttacking && Unit)
        Unit->GetAbilitySystemComponent()->CurrentMontageSetPlayRate(Rate);
    for (auto Effect : Effects) if (IsValid(Effect)) Effect->SetCustomTimeDilation(Rate);
    return true;
}
UStaticMeshComponent* UPadmaACTMeleeComponent::GetWeapon(int32 Index) const { return Weapons.IsValidIndex(Index) ? Weapons[Index] : nullptr; }
void UPadmaACTMeleeComponent::SetWeaponsDrawn(bool bDrawn)
{
    auto* Unit = Cast<APadmaCombatUnit>(GetOwner());
    if (!Profile || !Unit || !IsValid(Unit->GetMesh()) || bWeaponsDrawn == bDrawn) return;
    bWeaponsDrawn = bDrawn;
    for (int32 I = 0; I < Weapons.Num(); ++I)
    {
        SetWeaponMount(I,bDrawn);
    }
    // A mount change is not blade travel: never sweep from the back into the hand.
    for (int32 I = 0; I < PreviousWeapons.Num(); ++I)
        PreviousWeapons[I] = Weapons[I]->GetComponentTransform();
}
int32 UPadmaACTMeleeComponent::GetLiveActionEffectCount() const
{
    int32 Count = 0;
    for (const auto Effect : Effects) if (IsValid(Effect)) ++Count;
    for (const auto Effect : RetiringEffects) if (IsValid(Effect)) ++Count;
    for (const auto Ghost : Afterimages) if (IsValid(Ghost)) ++Count;
    for (const auto Mesh : AnimatedEffects) if (IsValid(Mesh)) ++Count;
    return Count;
}
bool UPadmaACTMeleeComponent::HasLiveAfterimages() const
{
    for (auto Ghost:Afterimages) if (IsValid(Ghost)) return true;
    for (const auto& Effect:PendingEffects) if (!Effect.bStop && !Effect.Config.AfterimageMaterial.IsNull()) return true;
    return false;
}
bool UPadmaACTMeleeComponent::RequestCombo()
{
    if (!IsComboWindowOpen()) return false;
    bComboQueued = true;
    return true;
}
void UPadmaACTMeleeComponent::BeginHitWindow(const UObject* Key, const FPadmaACTMeleeWindow& Window, float Start, float End)
{
    if (!bAttacking || !Weapons.IsValidIndex(Window.WeaponIndex) || !FMath::IsFinite(Window.DamageMultiplier) || Window.DamageMultiplier < 0 || !FMath::IsFinite(Window.AreaRadius) || Window.AreaRadius<0 || Window.AreaOffset.ContainsNaN() || End <= Start || Windows.Contains(Key)) return;
    auto State = MakeShared<FWindowState>();
    State->Config = Window; State->Start = Start; State->End = End;
    Windows.Add(Key, State);
}
void UPadmaACTMeleeComponent::EndHitWindow(const UObject* Key)
{
    // Retain through post-pose tracing even when Begin+End crossed in one engine frame.
    if (const auto* State = Windows.Find(Key)) (*State)->bClosing = true;
}
void UPadmaACTMeleeComponent::QueueActionEffect(const FPadmaACTMeleeFX& Effect, bool bStop)
{
    if (bAttacking) PendingEffects.Add({Effect,bStop});
}
void UPadmaACTMeleeComponent::PlayActionEffect(const FPendingEffect& Pending)
{
    const auto& FX = Pending.Config;
    if (Pending.bStop)
    {
        for (auto C : Effects) if (IsValid(C) && C->GetAsset() == FX.System.Get()) C->Deactivate();
        for (int32 I=AnimatedEffects.Num()-1; I>=0; --I)
            if (IsValid(AnimatedEffects[I]) && AnimatedEffects[I]->GetSkeletalMeshAsset()==FX.AnimatedMesh.Get())
            { AnimatedEffects[I]->DestroyComponent(); AnimatedEffects.RemoveAt(I); AnimatedEffectAges.RemoveAt(I); AnimatedEffectFades.RemoveAt(I); }
        return;
    }
    auto* Unit = CastChecked<APadmaCombatUnit>(GetOwner());
    if (auto* Material = FX.AfterimageMaterial.LoadSynchronous())
    {
        if (!FMath::IsFinite(FX.AfterimageLifetime) || FX.AfterimageLifetime <= 0) return;
        auto* Ghost = NewObject<UPoseableMeshComponent>(Unit);
        Ghost->SetSkinnedAssetAndUpdate(Unit->GetMesh()->GetSkeletalMeshAsset());
        Ghost->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Ghost->SetCastShadow(false);
        Ghost->RegisterComponent();
        Ghost->SetWorldTransform(Unit->GetMesh()->GetComponentTransform());
        Ghost->CopyPoseFromSkeletalComponent(Unit->GetMesh());
        Ghost->RefreshBoneTransforms();
        Ghost->SetComponentTickEnabled(false);
        auto* MID = UMaterialInstanceDynamic::Create(Material, Ghost);
        MID->SetScalarParameterValue(TEXT("GhostAge"), 0);
        for (int32 Slot=0; Slot<Ghost->GetNumMaterials(); ++Slot) Ghost->SetMaterial(Slot, MID);
        Afterimages.Add(Ghost); AfterimageAges.Add(0); AfterimageLifetimes.Add(FX.AfterimageLifetime);
        // Source rendererMask=-1 includes the equipped meshes as well as the body.
        for (auto Weapon : Weapons)
        {
            if (!IsValid(Weapon) || !Weapon->GetStaticMesh()) continue;
            auto* WeaponGhost=NewObject<UStaticMeshComponent>(Unit);
            WeaponGhost->SetStaticMesh(Weapon->GetStaticMesh());
            WeaponGhost->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            WeaponGhost->SetCastShadow(false);
            WeaponGhost->RegisterComponent();
            WeaponGhost->SetWorldTransform(Weapon->GetComponentTransform());
            for (int32 Slot=0; Slot<WeaponGhost->GetNumMaterials(); ++Slot) WeaponGhost->SetMaterial(Slot,MID);
            Afterimages.Add(WeaponGhost); AfterimageAges.Add(0); AfterimageLifetimes.Add(FX.AfterimageLifetime);
        }
        ++SpawnedActionEffects;
        return;
    }
    auto* Parent = FX.WeaponIndex < 0 ? static_cast<USceneComponent*>(Unit->GetMesh()) : GetWeapon(FX.WeaponIndex);
    if (auto* Mesh=FX.AnimatedMesh.LoadSynchronous())
    {
        auto* Animation=FX.MeshAnimation.LoadSynchronous();
        if (!Animation || !Parent || FX.RelativeTransform.ContainsNaN() || Animation->GetSkeleton()!=Mesh->GetSkeleton()) return;
        auto* C=NewObject<USkeletalMeshComponent>(Unit);
        C->SetSkeletalMesh(Mesh); C->SetCollisionEnabled(ECollisionEnabled::NoCollision); C->SetCastShadow(false);
        C->RegisterComponent();
        if (FX.bFollowMount) { C->AttachToComponent(Parent,FAttachmentTransformRules::KeepRelativeTransform,FX.MountSocket); C->SetRelativeTransform(FX.RelativeTransform); }
        else C->SetWorldTransform(FX.RelativeTransform * Parent->GetSocketTransform(FX.MountSocket));
        C->PlayAnimation(Animation,false); C->SetPosition(0,false);
        for (int32 Slot=0;Slot<C->GetNumMaterials();++Slot) C->CreateAndSetMaterialInstanceDynamic(Slot);
        C->SetComponentTickEnabled(false); C->RefreshBoneTransforms();
        AnimatedEffects.Add(C); AnimatedEffectAges.Add(0); ++SpawnedActionEffects;
        AnimatedEffectFades.Add(FVector2f(FX.MeshFadeStart,FX.MeshFadeDuration));
        return;
    }
    auto* System = FX.System.LoadSynchronous();
    if (!System || !Parent || FX.RelativeTransform.ContainsNaN()) return;
    UNiagaraComponent* C = nullptr;
    if (FX.bFollowMount)
    {
        C = UNiagaraFunctionLibrary::SpawnSystemAttached(System, Parent, FX.MountSocket,
            FX.RelativeTransform.GetLocation(), FX.RelativeTransform.Rotator(), EAttachLocation::KeepRelativeOffset, true);
        if (C) C->SetRelativeScale3D(FX.RelativeTransform.GetScale3D());
    }
    else
    {
        const FTransform Transform = FX.RelativeTransform * Parent->GetSocketTransform(FX.MountSocket);
        C = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, System, Transform.GetLocation(), Transform.Rotator(), Transform.GetScale3D());
    }
    if (C) { C->SetCustomTimeDilation(PlayRate); Effects.Add(C); ++SpawnedActionEffects; }
}
void UPadmaACTMeleeComponent::TraceWindow(const TSharedPtr<FWindowState>& State, float Position)
{
    const uint64 Generation=ActionGeneration;
    const auto Window = State->Config;
    const float Start = State->Start, End = State->End;
    if (Position <= Start || PreviousMontagePosition >= End || Position <= PreviousMontagePosition) return;
    auto* Unit = CastChecked<APadmaCombatUnit>(GetOwner());
    auto* Battle = Unit->GetBattle();
    if (!Battle || !Battle->IsBattleActive()) return;
    const auto& Blade = Profile->Weapons[Window.WeaponIndex];
    const auto& Last = PreviousWeapons[Window.WeaponIndex];
    const auto Current = Weapons[Window.WeaponIndex]->GetComponentTransform();
    const float A = FMath::Clamp((Start-PreviousMontagePosition)/(Position-PreviousMontagePosition),0.f,1.f);
    const float B = FMath::Clamp((End-PreviousMontagePosition)/(Position-PreviousMontagePosition),0.f,1.f);
    const bool Volume=Window.bPrimaryTargetOnly || Window.AreaRadius>0 || !Window.Shapes.IsEmpty();
    const int32 Steps = Volume ? 0 : FMath::Max(1,FMath::CeilToInt(FVector::Distance(Blade.BladeBase,Blade.BladeTip)/(Profile->TraceRadius*1.5f)));
    FCollisionObjectQueryParams Objects(ECC_Pawn);
    FCollisionQueryParams Params(SCENE_QUERY_STAT(PadmaAnimatedBlade),false,Unit);
    for (int32 I=0; I<=Steps && bAttacking; ++I)
    {
        const FVector Local = FMath::Lerp(Blade.BladeBase,Blade.BladeTip,Steps>0 ? float(I)/Steps : 0.f);
        const FVector P = Last.TransformPosition(Local), Q = Current.TransformPosition(Local);
        TArray<FHitResult> Results;
        const FVector Center=Unit->GetActorTransform().TransformPosition(Window.AreaOffset);
        if (Window.bPrimaryTargetOnly)
        {
            if (auto* Target=Unit->GetActions()->GetPrimaryTarget(); IsValid(Target))
                Results.Add(FHitResult(Target,Target->GetCapsuleComponent(),Target->GetActorLocation(),(Unit->GetActorLocation()-Target->GetActorLocation()).GetSafeNormal()));
        }
        else if (!Window.Shapes.IsEmpty())
        {
            for (const auto& Shape:Window.Shapes)
            {
                if (Shape.Center.ContainsNaN() || Shape.Size.ContainsNaN() || Shape.Rotation.ContainsNaN()
                    || !FMath::IsFinite(Shape.Radius) || Shape.Radius<0 || (Shape.Radius==0 && Shape.Size.GetMin()<=0)) continue;
                FVector Offset=Shape.Center; Offset.Z-=Unit->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
                const FVector Position3D=Unit->GetActorTransform().TransformPosition(Offset);
                TArray<FHitResult> ShapeHits;
                const FCollisionShape Geometry=Shape.Radius>0 ? FCollisionShape::MakeSphere(Shape.Radius) : FCollisionShape::MakeBox(Shape.Size*.5f);
                GetWorld()->SweepMultiByObjectType(ShapeHits,Position3D,Position3D,Unit->GetActorQuat()*Shape.Rotation.Quaternion(),Objects,Geometry,Params);
                Results.Append(ShapeHits);
            }
        }
        else GetWorld()->SweepMultiByObjectType(Results,Window.AreaRadius>0 ? Center : FMath::Lerp(P,Q,A),Window.AreaRadius>0 ? Center : FMath::Lerp(P,Q,B),FQuat::Identity,Objects,FCollisionShape::MakeSphere(Window.AreaRadius>0 ? Window.AreaRadius : Profile->TraceRadius),Params);
        for (const auto& Hit : Results)
        {
            auto* Target = Cast<APadmaCombatUnit>(Hit.GetActor());
            if (!Target || !Target->IsAlive() || Target->Spec.bPlayer == Unit->Spec.bPlayer || State->Contacts.Contains(Target) || !Unit->GetActions()->AllowsContact(Target)) continue;
            // Record before damage/callbacks to prevent duplicate reentrant contacts.
            State->Contacts.Add(Target);
            if (!Battle->ResolveMeleeContact(Unit,Hit,Window.DamageMultiplier*DamageScale)) continue;
            if (Generation!=ActionGeneration) return;
            ++ConfirmedContacts;
            // Settlement delegates may cancel GAS or exit the battle synchronously.
            if (!bAttacking || !IsValid(Unit) || !IsValid(Battle) || !Battle->IsBattleActive()) break;
            if (Window.HitEffect.IsNull() && Window.bSuppressFallbackHitEffect) continue;
            const auto& ContactEffect = Window.HitEffect.IsNull() ? Profile->HitEffect : Window.HitEffect;
            if (auto* FX = ContactEffect.LoadSynchronous())
            {
                if (!IsValid(Target)) continue;
                const FVector Point = PadmaACT::ResolveImpactLocation(Hit,*Target,Window.HitSocket,Volume);
                if (Point.ContainsNaN()) continue;
                FRotator Facing = Hit.ImpactNormal.Rotation();
                if (Window.bFaceTargetPlanar && IsValid(Target))
                {
                    FVector Direction = Point - Unit->GetActorLocation();
                    Direction.Z = 0;
                    if (!Direction.IsNearlyZero())
                        Facing = (Direction.Rotation().Quaternion() * Profile->MeshRelativeRotation.Quaternion()).Rotator();
                }
                if (auto* C = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,FX,Point,Facing))
                { ++SpawnedHitEffects; }
            }
        }
    }
}

void UPadmaACTMeleeComponent::ClearActionEffects(bool bAllowParticleTail)
{
    PendingEffects.Reset();
    for (auto Effect : Effects) if (IsValid(Effect))
    {
        if (bAllowParticleTail)
        {
            Effect->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
            Effect->Deactivate();
            RetiringEffects.Add(Effect); RetiringEffectAges.Add(0);
        }
        else Effect->DestroyComponent();
    }
    Effects.Reset();
    if (!bAllowParticleTail)
    {
        for (auto Effect : RetiringEffects) if (IsValid(Effect)) Effect->DestroyComponent();
        RetiringEffects.Reset(); RetiringEffectAges.Reset();
    }
    for (auto Ghost : Afterimages) if (IsValid(Ghost)) Ghost->DestroyComponent();
    Afterimages.Reset(); AfterimageAges.Reset(); AfterimageLifetimes.Reset();
    for (auto Mesh : AnimatedEffects) if (IsValid(Mesh)) Mesh->DestroyComponent();
    AnimatedEffects.Reset(); AnimatedEffectAges.Reset(); AnimatedEffectFades.Reset();
}
void UPadmaACTMeleeComponent::FinishAttack(bool bCancelled)
{
    const bool WasAttacking = bAttacking;
    bAttacking = bComboQueued = bComboOpen = bCompletionPending = false;
    Windows.Reset();
    ClearActionEffects(!bCancelled);
    if (WasAttacking) FinishWeaponAction();
    // Confirmed world-owned impacts keep their independent Niagara lifetime.
    if (WasAttacking) OnAttackFinished.Broadcast(bCancelled);
}
void UPadmaACTMeleeComponent::TickComponent(float Delta, ELevelTick Type, FActorComponentTickFunction* Tick)
{
    Super::TickComponent(Delta,Type,Tick);
    auto* Unit = Cast<APadmaCombatUnit>(GetOwner());
    if (!Unit || !Unit->IsAlive() || !Unit->GetBattle() || !Unit->GetBattle()->IsBattleActive()) { FinishAttack(true); TickWeaponPresentation(Delta); return; }
    TickWeaponPresentation(Delta);
    for (int32 I=RetiringEffects.Num()-1;I>=0;--I)
    {
        auto* Effect=RetiringEffects[I].Get();
        RetiringEffectAges[I]+=Delta*(IsValid(Effect) ? Effect->GetCustomTimeDilation() : 1.f);
        if (!IsValid(Effect) || Effect->IsComplete() || RetiringEffectAges[I]>8.f)
        {
            if (IsValid(Effect)) Effect->DestroyComponent();
            RetiringEffects.RemoveAt(I); RetiringEffectAges.RemoveAt(I);
        }
    }
    if (!bAttacking) return;
    for (auto Weapon : Weapons) Weapon->UpdateComponentToWorld();
    const float Position = GetAttackTime();
    for (int32 I=0; I<AnimatedEffects.Num(); ++I)
        if (auto* Mesh=AnimatedEffects[I].Get(); IsValid(Mesh))
        {
            AnimatedEffectAges[I] += Delta * PlayRate;
            Mesh->SetPosition(AnimatedEffectAges[I],false);
            Mesh->TickAnimation(0,false); Mesh->RefreshBoneTransforms();
            const auto Fade=AnimatedEffectFades[I];
            const float Opacity=Fade.X<0 ? 1.f : 1.f-FMath::Clamp((AnimatedEffectAges[I]-Fade.X)/FMath::Max(.001f,Fade.Y),0.f,1.f);
            for (int32 Slot=0;Slot<Mesh->GetNumMaterials();++Slot)
                if (auto* MID=Cast<UMaterialInstanceDynamic>(Mesh->GetMaterial(Slot))) MID->SetScalarParameterValue(TEXT("SourceOpacity"),Opacity);
        }
    // Visual particle age only; spawning remains exclusively Montage-notify driven.
    for (int32 I=Afterimages.Num()-1; I>=0; --I)
    {
        AfterimageAges[I] += Delta * PlayRate;
        auto* Ghost=Afterimages[I].Get();
        if (!IsValid(Ghost) || AfterimageAges[I] >= AfterimageLifetimes[I])
        {
            if (IsValid(Ghost)) Ghost->DestroyComponent();
            Afterimages.RemoveAt(I); AfterimageAges.RemoveAt(I); AfterimageLifetimes.RemoveAt(I);
        }
        else if (auto* MID=Cast<UMaterialInstanceDynamic>(Ghost->GetMaterial(0)))
            MID->SetScalarParameterValue(TEXT("GhostAge"), AfterimageAges[I]/AfterimageLifetimes[I]);
    }
    const uint64 Generation=ActionGeneration;
    auto Queued = MoveTemp(PendingEffects);
    PendingEffects.Reset();
    for (const auto& Effect : Queued) if (bAttacking) PlayActionEffect(Effect);
    // Snapshot retains state through reentrant GAS cancellation / battle exit during damage.
    TArray<TSharedPtr<FWindowState>> Active;
    Windows.GenerateValueArray(Active);
    for (const auto& State : Active)
    {
        if (bAttacking) TraceWindow(State, Position);
        if (Generation!=ActionGeneration) return;
    }
    for (auto It = Windows.CreateIterator(); It; ++It)
        if (It.Value()->bClosing || Position >= It.Value()->End) It.RemoveCurrent();
    PreviousMontagePosition = Position;
    for (int32 I=0; I<Weapons.Num(); ++I) PreviousWeapons[I] = Weapons[I]->GetComponentTransform();
    if (bAttacking && bCompletionPending) { FinishAttack(false); return; }
    if (bAttacking && bComboQueued)
    {
        // Repeat Attack01 in the SAME GAS task / Montage instance. Additional combo moves can be sections.
        ResetCycle();
        Unit->GetAbilitySystemComponent()->CurrentMontageJumpToSection(Profile->AttackSection);
    }
}
void UPadmaACTMeleeComponent::EndPlay(EEndPlayReason::Type Reason)
{
    StopWeaponMontage();
    FinishAttack(true);
    Super::EndPlay(Reason);
}
