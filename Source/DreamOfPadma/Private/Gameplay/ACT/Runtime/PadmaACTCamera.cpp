#include "Gameplay/ACT/Runtime/PadmaACTCamera.h"
#include "Gameplay/ACT/Runtime/PadmaACTActions.h"
#include "Gameplay/ACT/Runtime/PadmaACTMelee.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

namespace
{
float Ease(float Delta,float Time) { return 1-FMath::Exp(-FMath::Max(0.f,Delta)/FMath::Max(.001f,Time)); }
float HorizontalFOV(float Vertical,float Aspect) { return FMath::RadiansToDegrees(2*FMath::Atan(FMath::Tan(FMath::DegreesToRadians(Vertical*.5f))*Aspect)); }
double Damp(double Current,double Target,double& Velocity,float Time,float Delta)
{
    const double Omega=2/FMath::Max(.001f,Time), E=FMath::Exp(-Omega*Delta), Offset=Current-Target, Temp=(Velocity+Omega*Offset)*Delta;
    Velocity=(Velocity-Omega*Temp)*E;return Target+(Offset+Temp)*E;
}
}
UPadmaACTCameraComponent::UPadmaACTCameraComponent()
{
    PrimaryComponentTick.bCanEverTick=true;PrimaryComponentTick.bStartWithTickEnabled=false;PrimaryComponentTick.TickGroup=TG_PostPhysics;
    bAutoActivate=false;bUsePawnControlRotation=false;
    bOverrideAspectRatioAxisConstraint=true;AspectRatioAxisConstraint=AspectRatio_MaintainYFOV;
}
APadmaCombatUnit* UPadmaACTCameraComponent::Unit() const { return Cast<APadmaCombatUnit>(GetOwner()); }
bool UPadmaACTCameraComponent::Configure(UPadmaACTCameraDefinition* D)
{
    if (!D || D->Orbit.Num()<2 || !Unit()) return false;
    Profile=D;SetAbsolute(true,true,true);SetActive(true);SetComponentTickEnabled(true);
    AddTickPrerequisiteComponent(Unit()->GetMesh());
    if (!LockMarker)
    {
        LockMarker=NewObject<UStaticMeshComponent>(GetOwner(),TEXT("ACTLockMarker"));
        LockMarker->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Plane.Plane")));
        LockMarker->SetCollisionEnabled(ECollisionEnabled::NoCollision);LockMarker->SetCastShadow(false);
        LockMarker->SetMaterial(0,D->LockMarkerMaterial.LoadSynchronous());LockMarker->SetWorldScale3D(FVector(.3f));
        LockMarker->RegisterComponent();LockMarker->SetVisibility(false);
    }
    ResetView();return true;
}
bool UPadmaACTCameraComponent::IsUsable() const
{
    auto* U=Unit();auto* B=U ? U->GetBattle() : nullptr;
    return Profile && U && U->Spec.bPlayer && U->IsAlive() && B && B->IsBattleActive() && B->GetMode()==EPadmaCombatMode::ACT;
}
void UPadmaACTCameraComponent::ResetView()
{
    ClearLock();CameraAction.Reset();CameraElapsed=ReturnRemaining=CollisionDistance=0;bReady=bActionOverriding=false;SinceLook=0;YawSpeed=0;FollowVelocity=FVector::ZeroVector;
    if (Profile) { Yaw=SmoothedYaw=Unit()->GetActorRotation().Yaw;Vertical=SmoothedVertical=Profile->InitialVertical;ZoomScale=SmoothedZoom=Profile->InitialZoom; }
}
void UPadmaACTCameraComponent::Look(FVector2D Delta)
{
    if (!IsUsable() || Delta.ContainsNaN() || Delta.IsNearlyZero() || GetLockedTarget() || (CameraAction.IsValid() && CameraElapsed<CameraAction->CameraDuration)) return;
    Yaw=FRotator::NormalizeAxis(Yaw+Delta.X*Profile->MouseYawSensitivity);
    Vertical=FMath::Clamp(Vertical-Delta.Y*Profile->MouseVerticalSensitivity,Profile->MinVertical,Profile->MaxVertical);
    SinceLook=0;YawSpeed=0;
}
void UPadmaACTCameraComponent::Zoom(float Steps)
{
    if (!IsUsable() || !FMath::IsFinite(Steps)) return;
    if (GetLockedTarget()) { SwitchLock(Steps);return; }
    ZoomScale=FMath::Clamp(ZoomScale-Steps*Profile->ZoomStep,Profile->MinZoom,Profile->MaxZoom);
}
FVector UPadmaACTCameraComponent::CameraRelativeMovement(FVector2D Input) const
{
    const FRotator Basis(0,SmoothedYaw,0);
    return (Basis.Vector()*Input.X+FRotationMatrix(Basis).GetUnitAxis(EAxis::Y)*Input.Y).GetClampedToMaxSize(1);
}
void UPadmaACTCameraComponent::HandleInput(APlayerController* PC,bool Enabled)
{
    if (!PC || !IsUsable()) return;
    float X=0,Y=0;PC->GetInputMouseDelta(X,Y);
    if (!Enabled || Unit()->GetActions()->IsGameplayInputLocked()) return;
    Look(FVector2D(X,Y));
    if (PC->WasInputKeyJustPressed(EKeys::MiddleMouseButton)) ToggleLock();
    if (PC->WasInputKeyJustPressed(EKeys::MouseScrollUp)) Zoom(1);
    if (PC->WasInputKeyJustPressed(EKeys::MouseScrollDown)) Zoom(-1);
}
bool UPadmaACTCameraComponent::VisibleTarget(APadmaCombatUnit* T) const
{
    FCollisionQueryParams Params(SCENE_QUERY_STAT(ACTLockSight),false,Unit());Params.AddIgnoredActor(T);
    FHitResult Hit;
    return !GetWorld()->LineTraceSingleByChannel(Hit,GetComponentLocation(),T->GetActorLocation(),ECC_Visibility,Params);
}
bool UPadmaACTCameraComponent::IsValidTarget(APadmaCombatUnit* T,float Range,bool Sight) const
{
    return IsValid(T) && T!=Unit() && T->IsAlive() && !T->IsCardSource() && T->Spec.bPlayer!=Unit()->Spec.bPlayer
        && T->GetBattle()==Unit()->GetBattle() && FVector::DistSquared(T->GetActorLocation(),Unit()->GetActorLocation())<=FMath::Square(Range)
        && (!Sight || VisibleTarget(T));
}
APadmaCombatUnit* UPadmaACTCameraComponent::GetLockedTarget() const
{
    return IsUsable() && IsValidTarget(LockedTarget.Get(),Profile->UnlockRange,false) ? LockedTarget.Get() : nullptr;
}
APadmaCombatUnit* UPadmaACTCameraComponent::SelectTarget(float Direction) const
{
    if (!IsUsable()) return nullptr;
    APadmaCombatUnit* Best=nullptr;float BestScore=MAX_flt;
    const float CurrentAngle=LockedTarget.IsValid() ? (LockedTarget->GetActorLocation()-GetComponentLocation()).Rotation().Yaw : GetComponentRotation().Yaw;
    for (APadmaCombatUnit* T:Unit()->GetBattle()->GetUnits())
    {
        if (!IsValidTarget(T,Profile->LockRange,true) || (Direction!=0 && T==LockedTarget.Get())) continue;
        const FVector To=(T->GetActorLocation()-GetComponentLocation()).GetSafeNormal();
        const float Dot=FVector::DotProduct(GetForwardVector(),To);
        if (Dot<FMath::Cos(FMath::DegreesToRadians(Profile->AcquireHalfAngle))) continue;
        const float Angle=FMath::FindDeltaAngleDegrees(CurrentAngle,To.Rotation().Yaw);
        if (Direction!=0 && Angle*Direction<=0) continue;
        const float Score=Direction!=0 ? FMath::Abs(Angle) : (1-Dot)*1000+FVector::Dist(T->GetActorLocation(),Unit()->GetActorLocation())*.001f;
        if (Score<BestScore || (Score==BestScore && Best && T->Spec.Id.LexicalLess(Best->Spec.Id))) {Best=T;BestScore=Score;}
    }
    return Best;
}
void UPadmaACTCameraComponent::SetLock(APadmaCombatUnit* T)
{
    // Input may acquire another enemy before the camera tick observes a destroyed target.
    if (!LockedTarget.IsExplicitlyNull() && !LockedTarget.IsValid()) ClearLock();
    const bool Entering=LockedTarget.IsExplicitlyNull();
    LockedTarget=T;ObscuredTime=LockAge=0;
    if (Entering && T && Unit()) Unit()->GetMelee()->EnterWeaponCombat();
}
void UPadmaACTCameraComponent::ClearLock()
{
    if (!LockedTarget.IsExplicitlyNull() && Unit()) Unit()->GetMelee()->LeaveWeaponCombat();
    LockedTarget.Reset();ObscuredTime=LockAge=0;Yaw=SmoothedYaw;Vertical=SmoothedVertical;SinceLook=0;
    if (LockMarker) LockMarker->SetVisibility(false);
}
bool UPadmaACTCameraComponent::ToggleLock()
{
    if (!IsUsable()) return false;
    if (LockedTarget.IsValid()) {ClearLock();return true;}
    auto* Target=SelectTarget();if (!Target) return false;SetLock(Target);return true;
}
bool UPadmaACTCameraComponent::SwitchLock(float Direction)
{
    if (!GetLockedTarget() || FMath::IsNearlyZero(Direction)) return false;
    if (auto* Target=SelectTarget(Direction)) {SetLock(Target);return true;}return false;
}
FPadmaACTOrbitSample UPadmaACTCameraComponent::SampleOrbit(float Value) const
{
    const auto& S=Profile->Orbit;int32 I=0;while(I+1<S.Num()-1 && S[I+1].Vertical<Value) ++I;
    const float T=FMath::Clamp((Value-S[I].Vertical)/FMath::Max(.0001f,S[I+1].Vertical-S[I].Vertical),0.f,1.f);
    FPadmaACTOrbitSample R;R.Height=FMath::Lerp(S[I].Height,S[I+1].Height,T);R.Distance=FMath::Lerp(S[I].Distance,S[I+1].Distance,T);R.VerticalFOV=FMath::Lerp(S[I].VerticalFOV,S[I+1].VerticalFOV,T);return R;
}
void UPadmaACTCameraComponent::ApplyActionCamera(float Delta,FTransform& View,float& FOV)
{
    auto* Active=Unit()->GetActions()->GetActiveDefinition();
    if (!Active || Active->CameraSamples.Num()<2 || Active->CameraDuration<=0)
    {
        if (CameraAction.IsValid()) {if(bActionOverriding) ReturnRemaining=.2f;CameraAction.Reset();bActionOverriding=false;}
        if (ReturnRemaining>0) {ReturnRemaining=FMath::Max(0.f,ReturnRemaining-Delta);FTransform Blend;Blend.Blend(LastActionView,View,1-ReturnRemaining/.2f);View=Blend;FOV=FMath::Lerp(LastActionFOV,FOV,1-ReturnRemaining/.2f);}
        return;
    }
    const uint64 Generation=Unit()->GetMelee()->GetActionGeneration();
    if (CameraAction!=Active || CameraGeneration!=Generation) {CameraAction=Active;CameraGeneration=Generation;CameraElapsed=0;ReturnRemaining=0;}
    CameraElapsed+=Delta;
    bActionOverriding=CameraElapsed<Active->CameraDuration;
    if (!bActionOverriding) return;
    const auto& S=Active->CameraSamples;int32 I=0;while(I+1<S.Num()-1 && S[I+1].Time<CameraElapsed) ++I;
    const float T=FMath::Clamp((CameraElapsed-S[I].Time)/FMath::Max(.0001f,S[I+1].Time-S[I].Time),0.f,1.f);
    const FTransform Local(FQuat::Slerp(S[I].Rotation.Quaternion(),S[I+1].Rotation.Quaternion(),T),FMath::Lerp(S[I].Position,S[I+1].Position,T));
    const FTransform Cinematic=Local*Unit()->GetMesh()->GetComponentTransform();
    const float Blend=FMath::SmoothStep(0.f,1.f,(CameraElapsed-(Active->CameraDuration-Active->CameraEaseOut))/FMath::Max(.001f,Active->CameraEaseOut));
    FTransform Result;Result.Blend(Cinematic,View,Blend);View=Result;
    FOV=FMath::Lerp(HorizontalFOV(FMath::Lerp(S[I].VerticalFOV,S[I+1].VerticalFOV,T),AspectRatio),FOV,Blend);
    LastActionView=View;LastActionFOV=FOV;
}
void UPadmaACTCameraComponent::TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Tick)
{
    Super::TickComponent(Delta,Type,Tick);
    if (!IsUsable()) {ClearLock();CameraAction.Reset();ReturnRemaining=0;return;}
    auto* U=Unit();SinceLook+=Delta;
    if (!LockedTarget.IsExplicitlyNull())
    {
        if (!GetLockedTarget()) ClearLock();
        else {ObscuredTime=VisibleTarget(LockedTarget.Get()) ? 0 : ObscuredTime+Delta;if(ObscuredTime>Profile->OcclusionGrace) ClearLock();}
    }
    auto* Target=GetLockedTarget();
    if (Target) {LockAge+=Delta;Yaw=(Target->GetActorLocation()-U->GetActorLocation()).Rotation().Yaw;Vertical=Profile->LockVertical;}
    else if (SinceLook>Profile->AutoYawDelay && U->GetVelocity().Size2D()>20 && !U->GetActions()->GetActiveDefinition())
    {
        YawSpeed=FMath::Min(Profile->AutoYawSpeed,YawSpeed+Profile->AutoYawAcceleration*Delta);
        Yaw=FMath::FixedTurn(Yaw,U->GetActorRotation().Yaw,YawSpeed*Delta);
    }
    const float YawAlpha=Target ? Ease(Delta,LockAge<Profile->LockEnterTime ? Profile->LockEnterTime*.3f : Profile->LockDamping) : 1-FMath::Exp(-Profile->HorizontalTweenSpeed*Delta);
    SmoothedYaw=FRotator::NormalizeAxis(SmoothedYaw+FMath::FindDeltaAngleDegrees(SmoothedYaw,Yaw)*YawAlpha);
    SmoothedVertical=FMath::Lerp(SmoothedVertical,Vertical,1-FMath::Exp(-Profile->VerticalTweenSpeed*Delta));
    SmoothedZoom=FMath::Lerp(SmoothedZoom,FMath::Clamp(ZoomScale,Target ? Profile->LockMinZoom : Profile->MinZoom,Target ? Profile->LockMaxZoom : Profile->MaxZoom),Ease(Delta,Profile->ZoomDamping));
    FVector Feet=U->GetActorLocation()-FVector(0,0,U->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
    if (!bReady) {FollowPosition=Feet;bReady=true;}
    FollowPosition.X=Damp(FollowPosition.X,Feet.X,FollowVelocity.X,Profile->FollowDamping,Delta);
    FollowPosition.Y=Damp(FollowPosition.Y,Feet.Y,FollowVelocity.Y,Profile->FollowDamping,Delta);
    FollowPosition.Z=Damp(FollowPosition.Z,Feet.Z,FollowVelocity.Z,Profile->VerticalDamping,Delta);
    const auto Orbit=SampleOrbit(SmoothedVertical);
    const FVector Pivot=FollowPosition+FVector(0,0,Profile->ShoulderHeight);
    const FVector Desired=Pivot+FVector(0,0,Orbit.Height*SmoothedZoom)-FRotator(0,SmoothedYaw,0).Vector()*Orbit.Distance*SmoothedZoom;
    FVector Aim=FollowPosition+FVector(0,0,Profile->LookHeight);
    if (Target) Aim=FMath::Lerp(Aim,Target->GetActorLocation(),Profile->LockLookWeight);
    FCollisionQueryParams Query(SCENE_QUERY_STAT(ACTCameraCollision),false,U);
    for (APadmaCombatUnit* Other:U->GetBattle()->GetUnits()) Query.AddIgnoredActor(Other);
    FHitResult Hit;const FVector Arm=Desired-Pivot;float Length=Arm.Size();
    if (GetWorld()->SweepSingleByChannel(Hit,Pivot,Desired,FQuat::Identity,ECC_Camera,FCollisionShape::MakeSphere(FMath::Max(1.f,Profile->CollisionRadius)),Query)) Length=FMath::Max(0.f,Length*Hit.Time-2);
    if(CollisionDistance<=0 || Length<CollisionDistance) CollisionDistance=Length;
    else CollisionDistance=FMath::Lerp(CollisionDistance,Length,Ease(Delta,Profile->CollisionReleaseDamping));
    const FVector Position=Pivot+Arm.GetSafeNormal()*CollisionDistance;
    FTransform View((Aim-Position).Rotation(),Position);float FOV=HorizontalFOV(Orbit.VerticalFOV,FMath::Max(.1f,AspectRatio));
    ApplyActionCamera(Delta,View,FOV);SetWorldTransform(View);SetFieldOfView(FOV);
    if(LockMarker)
    {
        LockMarker->SetVisibility(Target!=nullptr);
        if(Target) {const FVector P=Target->GetActorLocation()+FVector(0,0,Target->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()+25);LockMarker->SetWorldLocationAndRotation(P,FRotationMatrix::MakeFromZ(GetComponentLocation()-P).Rotator());}
    }
}
