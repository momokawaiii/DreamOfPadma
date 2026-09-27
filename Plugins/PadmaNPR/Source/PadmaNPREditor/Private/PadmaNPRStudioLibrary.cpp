#include "PadmaNPRStudioLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "MaterialEditingLibrary.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInterface.h"
#include "Misc/PackageName.h"
#include "PadmaNPRComponent.h"
#include "PadmaNPRProfile.h"
#include "ScopedTransaction.h"
UPadmaNPRComponent *
UPadmaNPRStudioLibrary::ApplyToActor(UPadmaNPRProfile *P, AActor *Actor,
                                     USkeletalMeshComponent *Mesh,
                                     ADirectionalLight *Light, FString &Error) {
  Error.Reset();
  if (!P || !Actor || !Actor->GetWorld() || Actor->GetWorld()->IsGameWorld()) {
    Error = TEXT("Choose a Profile and an editor level actor.");
    return nullptr;
  }
  if (!P->Validate(Error))
    return nullptr;
  if (!Mesh)
    Mesh = Actor->FindComponentByClass<USkeletalMeshComponent>();
  if (!Mesh || Mesh->GetOwner() != Actor ||
      !Mesh->DoesSocketExist(P->HeadSocket)) {
    Error = TEXT("Invalid target mesh or missing head bone.");
    return nullptr;
  }
  for (const auto &S : P->Slots)
    if (Mesh->GetMaterialIndex(S.SlotName) < 0) {
      Error = TEXT("Missing slot: ") + S.SlotName.ToString();
      return nullptr;
    }
  // Validate the entire operation before mutating actor/assets.
  for (const auto &S : P->Slots) {
    const FString Name =
        TEXT("MI_") + P->GetName() + TEXT("_") + S.SlotName.ToString();
    const FString Target =
        FPackageName::GetLongPackagePath(P->GetPackage()->GetName()) +
        TEXT("/Generated/") + Name;
    TSet<UMaterialInterface *> Visited;
    for (UMaterialInterface *Parent = S.Material; Parent;) {
      if (Visited.Contains(Parent) ||
          Parent->GetPackage()->GetName() == Target) {
        Error = TEXT(
            "Source material would create a generated-instance parent cycle.");
        return nullptr;
      }
      Visited.Add(Parent);
      auto *MI = Cast<UMaterialInstance>(Parent);
      Parent = MI ? MI->Parent.Get() : nullptr;
    }
  }
  auto *Component = Actor->FindComponentByClass<UPadmaNPRComponent>();
  if (Component && Component->TargetMesh && Component->TargetMesh != Mesh) {
    Error = TEXT("Restore the current binding before changing target mesh.");
    return nullptr;
  }
  const FScopedTransaction Transaction(NSLOCTEXT(
      "PadmaNPR", "ApplyProfileTransaction", "Apply Padma NPR profile"));
  Actor->Modify();
  Mesh->Modify();
  if (!Component) {
    Component =
        NewObject<UPadmaNPRComponent>(Actor, NAME_None, RF_Transactional);
    Actor->AddInstanceComponent(Component);
    Component->TargetMesh = Mesh;
    Component->RegisterComponent();
  }
  Component->Modify();
  Component->RestoreMaterials();
  // Restore removed slots before replacing the profile. Do not overwrite
  // independent user edits.
  for (const auto &Pair : Component->EditorApplied) {
    int I = Mesh->GetMaterialIndex(Pair.Key);
    if (I >= 0 && Mesh->GetMaterial(I) == Pair.Value)
      Mesh->SetMaterial(I, Component->EditorOriginals.FindRef(Pair.Key));
  }
  for (const auto &S : P->Slots) {
    int I = Mesh->GetMaterialIndex(S.SlotName);
    Component->EditorOriginals.Add(S.SlotName, Mesh->GetMaterial(I));
  }
  Component->EditorApplied.Reset();
  for (const auto &S : P->Slots) {
    const FString Name =
        TEXT("MI_") + P->GetName() + TEXT("_") + S.SlotName.ToString();
    const FString Path =
        FPackageName::GetLongPackagePath(P->GetPackage()->GetName()) +
        TEXT("/Generated/") + Name;
    auto *MI = LoadObject<UMaterialInstanceConstant>(
        nullptr, *(Path + TEXT(".") + Name));
    if (!MI) {
      MI = NewObject<UMaterialInstanceConstant>(CreatePackage(*Path), *Name,
                                                RF_Public | RF_Standalone |
                                                    RF_Transactional);
      FAssetRegistryModule::AssetCreated(MI);
    }
    MI->Modify();
    MI->SetParentEditorOnly(S.Material);
    // Generated instances are owned by this profile. Clearing prevents removed
    // overrides from silently surviving a subsequent Apply.
    MI->ClearParameterValuesEditorOnly();
    TMap<FName, FLinearColor> Values;
    S.GetParameters(P->ProfileId, Values);
    for (const auto &Pair : Values)
      MI->SetVectorParameterValueEditorOnly(Pair.Key, Pair.Value);
    if (S.bReferenceResponse)
      for (const auto &Pair : S.ReferenceScalars)
        MI->SetScalarParameterValueEditorOnly(Pair.Key, Pair.Value);
    UMaterialEditingLibrary::UpdateMaterialInstance(MI);
    MI->MarkPackageDirty();
    const int Index = Mesh->GetMaterialIndex(S.SlotName);
    if (!Component->EditorOriginals.Contains(S.SlotName))
      Component->EditorOriginals.Add(S.SlotName, Mesh->GetMaterial(Index));
    Component->EditorApplied.Add(S.SlotName, MI);
    Mesh->SetMaterial(Index, MI);
  }
  Component->Profile = P;
  Component->TargetMesh = Mesh;
  if (Light || !Component->KeyLight)
    Component->KeyLight = Light;
  Component->ApplyProfile(Error);
  Actor->MarkPackageDirty();
  return Component;
}
bool UPadmaNPRStudioLibrary::RestoreActor(AActor *Actor, FString &Error) {
  Error.Reset();
  auto *C = Actor ? Actor->FindComponentByClass<UPadmaNPRComponent>() : nullptr;
  if (!C || !C->TargetMesh) {
    Error = TEXT("Actor has no Padma NPR binding.");
    return false;
  }
  const FScopedTransaction Transaction(NSLOCTEXT(
      "PadmaNPR", "RestoreProfileTransaction", "Restore Padma NPR materials"));
  Actor->Modify();
  C->Modify();
  C->TargetMesh->Modify();
  C->RestoreMaterials();
  for (const auto &Pair : C->EditorApplied) {
    int I = C->TargetMesh->GetMaterialIndex(Pair.Key);
    if (I >= 0 && C->TargetMesh->GetMaterial(I) == Pair.Value)
      C->TargetMesh->SetMaterial(I, C->EditorOriginals.FindRef(Pair.Key));
  }
  C->EditorOriginals.Reset();
  C->EditorApplied.Reset();
  C->Profile = nullptr;
  C->TargetMesh = nullptr;
  Actor->MarkPackageDirty();
  return true;
}

#include "Engine/Texture2D.h"
UTexture2D *UPadmaNPRStudioLibrary::BakeInvertedRedMask(UTexture2D *Source,
                                                        const FString &Path,
                                                        FString &Error) {
  Error.Reset();
  if (!Source || Source->Source.GetFormat() != TSF_BGRA8 ||
      !FPackageName::IsValidLongPackageName(Path) ||
      !Path.StartsWith(TEXT("/Game/"))) {
    Error = TEXT("Requires BGRA8 source and a new /Game package path.");
    return nullptr;
  }
  if (FPackageName::DoesPackageExist(Path) || FindPackage(nullptr, *Path)) {
    Error = TEXT("Target already exists; bake to a new path.");
    return nullptr;
  }
  TArray64<uint8> Bytes;
  if (!Source->Source.GetMipData(Bytes, 0)) {
    Error = TEXT("Texture source mip unavailable.");
    return nullptr;
  }
  for (int64 I = 0; I < Bytes.Num(); I += 4) {
    uint8 Mask = 255 - Bytes[I + 2];
    Bytes[I] = Mask;
    Bytes[I + 1] = Mask;
    Bytes[I + 2] = Mask;
    Bytes[I + 3] = 255;
  }
  auto *T = NewObject<UTexture2D>(CreatePackage(*Path),
                                  *FPackageName::GetShortName(Path),
                                  RF_Public | RF_Standalone | RF_Transactional);
  T->SRGB = false;
  T->CompressionSettings = TC_BC7;
  T->Source.Init(Source->Source.GetSizeX(), Source->Source.GetSizeY(), 1, 1,
                 TSF_BGRA8, Bytes.GetData());
  T->PostEditChange();
  FAssetRegistryModule::AssetCreated(T);
  T->MarkPackageDirty();
  return T;
}
