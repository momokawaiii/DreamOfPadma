#include "PadmaNPRComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "Engine/DirectionalLight.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "PadmaNPRProfile.h"
UPadmaNPRComponent::UPadmaNPRComponent() {
  PrimaryComponentTick.bCanEverTick = true;
  PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
  bTickInEditor = true;
}
bool UPadmaNPRComponent::ValidateBinding(FString &Error) const {
  if (!Profile || !TargetMesh) {
    Error = TEXT("Assign Profile and Target Mesh.");
    return false;
  }
  if (!Profile->Validate(Error))
    return false;
  if (!TargetMesh->DoesSocketExist(Profile->HeadSocket)) {
    Error = TEXT("Head socket/bone does not exist.");
    return false;
  }
  for (const auto &S : Profile->Slots)
    if (TargetMesh->GetMaterialIndex(S.SlotName) < 0) {
      Error = TEXT("Mesh has no slot: ") + S.SlotName.ToString();
      return false;
    }
  return true;
}
void UPadmaNPRComponent::BeginBinding() {
  if (BoundMesh.Get() == TargetMesh)
    return;
  if (BoundMesh.IsValid())
    RestoreMaterials();
  BoundMesh = TargetMesh;
  if (TargetMesh) {
    PreviousPrimitiveData = TargetMesh->GetCustomPrimitiveData().Data;
    AddTickPrerequisiteComponent(TargetMesh);
  }
}
bool UPadmaNPRComponent::ApplyProfile(FString &Error) {
  if (!ValidateBinding(Error)) {
    RestoreMaterials();
    return false;
  }
  // Editor uses persistent generated MICs. Transient MIDs must never be
  // serialized into a level.
  if (!GetWorld() || !GetWorld()->IsGameWorld()) {
    BeginBinding();
    bBindingActive = true;
    UpdateFrameData();
    return true;
  }
  RestoreMaterials();
  BeginBinding();
  Originals.SetNum(TargetMesh->GetNumMaterials());
  Active.SetNum(Originals.Num());
  for (const auto &S : Profile->Slots) {
    int32 I = TargetMesh->GetMaterialIndex(S.SlotName);
    Originals[I] = TargetMesh->GetMaterial(I);
    auto *MID = UMaterialInstanceDynamic::Create(S.Material, this);
    TMap<FName, FLinearColor> P;
    S.GetParameters(Profile->ProfileId, P);
    for (const auto &Pair : P)
      MID->SetVectorParameterValue(Pair.Key, Pair.Value);
    if (S.bReferenceResponse)
      for (const auto &Pair : S.ReferenceScalars)
        MID->SetScalarParameterValue(Pair.Key, Pair.Value);
    Active[I] = MID;
    TargetMesh->SetMaterial(I, MID);
  }
  bBindingActive = true;
  UpdateFrameData();
  return true;
}
void UPadmaNPRComponent::RestoreMaterials() {
  if (auto *M = BoundMesh.Get()) {
    for (int32 I = 0; I < Active.Num(); ++I)
      if (Active[I] && M->GetMaterial(I) == Active[I])
        M->SetMaterial(I, Originals[I]);
    // Restore only our reserved range; preserve other owners' data outside it.
    TArray<float> V;
    V.SetNumZeroed(20);
    for (int32 I = 0; I < FMath::Min(20, PreviousPrimitiveData.Num()); ++I)
      V[I] = PreviousPrimitiveData[I];
    M->SetCustomPrimitiveDataFloatArray(0, V);
    RemoveTickPrerequisiteComponent(M);
  }
  bBindingActive = false;
  Active.Reset();
  Originals.Reset();
  PreviousPrimitiveData.Reset();
  BoundMesh.Reset();
}
void UPadmaNPRComponent::UpdateFrameData() {
  if (!bBindingActive || !Profile || !TargetMesh ||
      !TargetMesh->DoesSocketExist(Profile->HeadSocket))
    return;
  BeginBinding();
  const FTransform Head = TargetMesh->GetSocketTransform(Profile->HeadSocket);
  const FQuat Q = Head.GetRotation() * Profile->HeadAxisCorrection.Quaternion();
  FVector L = IsValid(KeyLight) ? -KeyLight->GetActorForwardVector()
                                : Profile->FallbackLightDirection;
  L = L.GetSafeNormal(SMALL_NUMBER, FVector::UpVector);
  float ReferenceSDF = 0.f;
  float ReferenceSDFValid = 0.f;
  if (Profile->ReferenceSDFAngleCurve) {
    const FVector HeadUp = Q.GetUpVector();
    const FVector SDFLight =
        Profile->bReferencePlanarSDF
            ? FVector::VectorPlaneProject(L, HeadUp).GetSafeNormal(
                  1.e-4f, Q.GetForwardVector())
            : L;
    const float Angle =
        FMath::Acos(FMath::Clamp(
            FVector::DotProduct(SDFLight, Q.GetForwardVector()), -1.0, 1.0)) /
        PI;
    const float Value = Profile->ReferenceSDFAngleCurve->GetFloatValue(Angle);
    if (FMath::IsFinite(Value)) {
      const FVector Side =
          FVector::CrossProduct(-SDFLight, Q.GetForwardVector());
      const double Sign = Profile->bReferencePlanarSDF
                              ? FVector::DotProduct(Side, HeadUp)
                              : Side.Z;
      ReferenceSDF = Sign > 0 ? Value : -Value;
      ReferenceSDFValid = 1.f;
    }
  }
  TargetMesh->SetCustomPrimitiveDataVector4(0, FVector4(L, 1));
  TargetMesh->SetCustomPrimitiveDataVector4(
      4, FVector4(Q.GetForwardVector(), ReferenceSDF));
  TargetMesh->SetCustomPrimitiveDataVector4(
      8, FVector4(Q.GetRightVector(), ReferenceSDFValid));
  TargetMesh->SetCustomPrimitiveDataVector4(12, FVector4(Q.GetUpVector(), 0));
  TargetMesh->SetCustomPrimitiveDataVector4(16,
                                            FVector4(Head.GetLocation(), 0));
}
void UPadmaNPRComponent::OnRegister() {
  Super::OnRegister();
  if (!TargetMesh && GetOwner())
    TargetMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
  if (Profile) {
    FString Error;
    ApplyProfile(Error);
  }
}
void UPadmaNPRComponent::OnUnregister() {
  RestoreMaterials();
  Super::OnUnregister();
}
void UPadmaNPRComponent::TickComponent(float D, ELevelTick T,
                                       FActorComponentTickFunction *F) {
  Super::TickComponent(D, T, F);
  UpdateFrameData();
}
