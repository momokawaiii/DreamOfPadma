#include "Animation/AnimSequence.h"
#include "Components/SkeletalMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "Editor.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Misc/AutomationTest.h"
#include "PadmaNPRComponent.h"
#include "PadmaNPRProfile.h"
#include "PadmaNPRStudioLibrary.h"

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaReferenceProfileContractTest,
                                 "PadmaNPR.Reference.ProfileContract",
                                 EAutomationTestFlags::EditorContext |
                                     EAutomationTestFlags::EngineFilter)

bool FPadmaReferenceProfileContractTest::RunTest(const FString &Parameters) {
  auto *Profile = NewObject<UPadmaNPRProfile>();
  FPadmaNPRSlot &Slot = Profile->Slots.AddDefaulted_GetRef();
  Slot.SlotName = TEXT("Hair");
  Slot.Type = EPadmaNPRSurface::Hair;
  Slot.bReferenceResponse = true;
  Slot.Material = NewObject<UMaterial>(Profile, TEXT("M_NPR_Hair_Reference"));
  Slot.ReferenceScalars.Add(TEXT("KK_Power"), 200.f);
  Slot.ReferenceVectors.Add(TEXT("Lam_ShadowColor"),
                            FLinearColor(.4f, .4f, .4f, 1));
  TMap<FName, FLinearColor> Values;
  Slot.GetParameters(1, Values);
  TestEqual(TEXT("Reference map is authoritative"), Values.Num(), 1);
  TestFalse(TEXT("Generic tint is not injected"),
            Values.Contains(TEXT("NPR_Tint")));
  FString Error;
  TestTrue(TEXT("Matching reference material accepted"),
           Profile->Validate(Error));
  Slot.bReferenceResponse = false;
  TestFalse(TEXT("Reference master cannot bind as generic"),
            Profile->Validate(Error));
  Slot.bReferenceResponse = true;
  Slot.ReferenceScalars.Add(NAME_None, 1.f);
  TestFalse(TEXT("Unnamed scalar rejected"), Profile->Validate(Error));
  Slot.bReferenceResponse = false;
  Slot.Type = EPadmaNPRSurface::Cloth;
  Slot.Material = NewObject<UMaterial>(Profile, TEXT("M_NPR_Cloth"));
  TestFalse(TEXT("Legacy cloth cannot consume character generic parameters"),
            Profile->Validate(Error));
  return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaReferenceBindingTest,
                                 "PadmaNPR.Reference.Binding",
                                 EAutomationTestFlags::EditorContext |
                                     EAutomationTestFlags::EngineFilter)
bool FPadmaReferenceBindingTest::RunTest(const FString &Parameters) {
  auto *MeshAsset = LoadObject<USkeletalMesh>(
      nullptr, TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Art/Meshes/"
                    "SK_Chen_FullCharacter_CM.SK_Chen_FullCharacter_CM"));
  auto *Master = LoadObject<UMaterial>(
      nullptr,
      TEXT("/PadmaNPR/Materials/M_NPR_Hair_Reference.M_NPR_Hair_Reference"));
  if (!TestNotNull(TEXT("Mesh fixture"), MeshAsset) ||
      !TestNotNull(TEXT("Reference master"), Master))
    return false;
  auto *P = NewObject<UPadmaNPRProfile>(
      CreatePackage(TEXT("/Temp/PadmaReferenceBinding")), TEXT("Profile"));
  auto &S = P->Slots.AddDefaulted_GetRef();
  S.SlotName = TEXT("M_actor_chen_hair_01");
  S.Type = EPadmaNPRSurface::Hair;
  S.Material = Master;
  S.bReferenceResponse = true;
  S.ReferenceScalars.Add(TEXT("ReferenceBangOpacity"), .7f);
  P->ReferenceSDFAngleCurve = NewObject<UCurveFloat>(P);
  P->ReferenceSDFAngleCurve->FloatCurve.AddKey(0, .2f);
  P->ReferenceSDFAngleCurve->FloatCurve.AddKey(1, .8f);
  auto MakeMesh = [&](AActor *A) {
    auto *M = NewObject<USkeletalMeshComponent>(A);
    A->AddInstanceComponent(M);
    A->SetRootComponent(M);
    M->SetSkeletalMeshAsset(MeshAsset);
    M->RegisterComponent();
    return M;
  };
  auto *W = GEditor->GetEditorWorldContext().World();
  auto *A = W->SpawnActor<AActor>();
  auto *M = MakeMesh(A);
  FString Error;
  auto *C = UPadmaNPRStudioLibrary::ApplyToActor(P, A, M, nullptr, Error);
  if (TestNotNull(*Error, C)) {
    const int I = M->GetMaterialIndex(S.SlotName);
    float Value = 0;
    M->GetMaterial(I)->GetScalarParameterValue(
        FMaterialParameterInfo(TEXT("ReferenceBangOpacity")), Value);
    TestEqual(TEXT("Editor scalar override"), Value, .7f);
    S.ReferenceScalars.Reset();
    UPadmaNPRStudioLibrary::ApplyToActor(P, A, M, nullptr, Error);
    M->GetMaterial(I)->GetScalarParameterValue(
        FMaterialParameterInfo(TEXT("ReferenceBangOpacity")), Value);
    TestEqual(TEXT("Removed override restores parent default"), Value, 1.f);
    TestEqual(TEXT("SDF curve valid bit"), M->GetCustomPrimitiveData().Data[11],
              1.f);
    const auto Forward = M->GetSocketTransform(P->HeadSocket).GetRotation() *
                         P->HeadAxisCorrection.Quaternion();
    P->FallbackLightDirection = Forward.GetForwardVector();
    C->UpdateFrameData();
    TestTrue(TEXT("Front light evaluates first curve key"),
             FMath::IsNearlyEqual(
                 FMath::Abs(M->GetCustomPrimitiveData().Data[7]), .2f, .001f));
    P->FallbackLightDirection = -Forward.GetForwardVector();
    C->UpdateFrameData();
    TestTrue(TEXT("Back light evaluates last curve key"),
             FMath::IsNearlyEqual(
                 FMath::Abs(M->GetCustomPrimitiveData().Data[7]), .8f, .001f));
    P->bReferencePlanarSDF = true;
    const FVector Diagonal =
        Forward.GetForwardVector() + Forward.GetRightVector();
    P->FallbackLightDirection = Diagonal;
    C->UpdateFrameData();
    const float PlanarValue = M->GetCustomPrimitiveData().Data[7];
    P->FallbackLightDirection = Diagonal + Forward.GetUpVector() * 3;
    C->UpdateFrameData();
    TestTrue(TEXT("Planar SDF ignores elevation at fixed azimuth"),
             FMath::IsNearlyEqual(M->GetCustomPrimitiveData().Data[7],
                                  PlanarValue, .001f));
    P->FallbackLightDirection = Forward.GetUpVector();
    C->UpdateFrameData();
    TestTrue(TEXT("Vertical light has finite SDF fallback"),
             FMath::IsFinite(M->GetCustomPrimitiveData().Data[7]));
    P->ReferenceSDFAngleCurve = nullptr;
    C->UpdateFrameData();
    TestEqual(TEXT("Removing curve clears validity"),
              M->GetCustomPrimitiveData().Data[11], 0.f);
    auto *Idle = LoadObject<UAnimSequence>(
        nullptr,
        TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Sequences/"
             "Imported/AS_Chen_IdleBase_CM.AS_Chen_IdleBase_CM"));
    if (TestNotNull(TEXT("Actual idle pose fixture"), Idle)) {
      M->SetAnimationMode(EAnimationMode::AnimationSingleNode);
      M->SetAnimation(Idle);
      M->SetPosition(0, false);
      M->TickAnimation(0, false);
      M->RefreshBoneTransforms();
      const FTransform FirstHead = M->GetSocketTransform(P->HeadSocket);
      M->SetPosition(1, false);
      M->TickAnimation(0, false);
      M->RefreshBoneTransforms();
      const FTransform NextHead = M->GetSocketTransform(P->HeadSocket);
      C->UpdateFrameData();
      TestFalse(TEXT("Idle animation changes head pose"),
                FirstHead.Equals(NextHead, .0001f));
      const FVector Expected =
          (NextHead.GetRotation() * P->HeadAxisCorrection.Quaternion())
              .GetForwardVector();
      const auto &D = M->GetCustomPrimitiveData().Data;
      TestTrue(TEXT("Frame basis follows evaluated animated head"),
               FVector(D[4], D[5], D[6]).Equals(Expected, .001f));
    }
    UPadmaNPRStudioLibrary::RestoreActor(A, Error);
  }
  W->DestroyActor(A);
  UWorld::InitializationValues Init;
  Init.AllowAudioPlayback(false)
      .CreatePhysicsScene(false)
      .ShouldSimulatePhysics(false)
      .CreateNavigation(false)
      .CreateAISystem(false);
  auto *Game = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr,
                                   true, ERHIFeatureLevel::Num, &Init);
  if (TestNotNull(TEXT("Runtime world"), Game)) {
    A = Game->SpawnActor<AActor>();
    M = MakeMesh(A);
    S.ReferenceScalars.Add(TEXT("ReferenceBangOpacity"), .65f);
    C = NewObject<UPadmaNPRComponent>(A);
    A->AddInstanceComponent(C);
    C->TargetMesh = M;
    C->Profile = P;
    C->RegisterComponent();
    auto *MID = Cast<UMaterialInstanceDynamic>(
        M->GetMaterial(M->GetMaterialIndex(S.SlotName)));
    if (TestNotNull(TEXT("Runtime MID"), MID)) {
      float Value = 0;
      MID->GetScalarParameterValue(
          FMaterialParameterInfo(TEXT("ReferenceBangOpacity")), Value);
      TestEqual(TEXT("Runtime scalar matches profile"), Value, .65f);
    }
    C->RestoreMaterials();
    Game->DestroyWorld(false);
  }
  return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaChenReferenceConfiguration,
                                 "PadmaNPR.Reference.ChenConfiguration",
                                 EAutomationTestFlags::EditorContext |
                                     EAutomationTestFlags::EngineFilter)
bool FPadmaChenReferenceConfiguration::RunTest(const FString &Parameters) {
  auto *P = LoadObject<UPadmaNPRProfile>(
      nullptr, TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Art/Profiles/"
                    "DA_Chen_NPR_Reference.DA_Chen_NPR_Reference"));
  if (!TestNotNull(TEXT("Saved Chen reference profile"), P))
    return false;
  FString Error;
  TestTrue(TEXT("Profile validation"), P->Validate(Error));
  TestEqual(TEXT("All nine target slots"), P->Slots.Num(), 9);
  TestTrue(TEXT("Head-plane SDF enabled"), P->bReferencePlanarSDF);
  TestNotNull(TEXT("Weighted SDF response curve"),
              P->ReferenceSDFAngleCurve.Get());
  TSet<EPadmaNPRSurface> Types;
  for (const auto &S : P->Slots) {
    TestTrue(TEXT("Every slot uses its reference contract"),
             S.bReferenceResponse);
    Types.Add(S.Type);
    if (S.Type == EPadmaNPRSurface::Skin || S.Type == EPadmaNPRSurface::Cloth) {
      const float *Dry = S.ReferenceScalars.Find(TEXT("RainStrengh"));
      TestTrue(TEXT("Dry baseline is explicit"), Dry && *Dry == 0.f);
    }
    if (S.Type == EPadmaNPRSurface::Face) {
      const FLinearColor *Control =
          S.ReferenceVectors.Find(TEXT("ReferenceFaceControls"));
      TestTrue(TEXT("Face transition and top-light controls bound"),
               Control && Control->R > 0 && Control->R < .1f &&
                   Control->B == 1 && Control->A == 0);
    }
  }
  TestEqual(TEXT("Each of the nine distinct surfaces is covered"), Types.Num(),
            9);
  return true;
}

#endif
