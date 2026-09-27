#if WITH_DEV_AUTOMATION_TESTS
#include "Components/SkeletalMeshComponent.h"
#include "Editor.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "MaterialEditingLibrary.h"
#include "MaterialShared.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Misc/AutomationTest.h"
#include "PadmaNPRComponent.h"
#include "PadmaNPRProfile.h"
#include "PadmaNPRStudioLibrary.h"
#include "RenderingThread.h"
#include "ShaderCompiler.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaCharacterContract,
                                 "PadmaNPR.Character.Binding",
                                 EAutomationTestFlags::EditorContext |
                                     EAutomationTestFlags::ProductFilter)
bool FPadmaCharacterContract::RunTest(const FString &) {
  auto *MeshAsset = LoadObject<USkeletalMesh>(
      nullptr, TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Art/Meshes/"
                    "SK_Chen_FullCharacter_CM.SK_Chen_FullCharacter_CM"));
  auto *Master = LoadObject<UMaterial>(
      nullptr, TEXT("/PadmaNPR/Materials/M_NPR_Face.M_NPR_Face"));
  if (!TestNotNull(TEXT("Chen fixture"), MeshAsset) ||
      !TestNotNull(TEXT("Face material"), Master))
    return false;
  auto *W = GEditor->GetEditorWorldContext().World();
  auto *A = W->SpawnActor<AActor>();
  auto *B = W->SpawnActor<AActor>();
  auto MakeMesh = [&](AActor *Owner) {
    auto *M = NewObject<USkeletalMeshComponent>(Owner);
    Owner->AddInstanceComponent(M);
    Owner->SetRootComponent(M);
    M->SetSkeletalMeshAsset(MeshAsset);
    M->RegisterComponent();
    return M;
  };
  auto *M = MakeMesh(A);
  auto *N = MakeMesh(B);
  auto *P = NewObject<UPadmaNPRProfile>(
      CreatePackage(TEXT("/Temp/PadmaNPRBindingTest")), TEXT("Profile"));
  FPadmaNPRSlot S;
  S.Material = Master;
  S.SlotName = TEXT("M_actor_chen_face_01");
  P->Slots.Add(S);
  FString Error;
  const int I = M->GetMaterialIndex(S.SlotName);
  auto *Original = M->GetMaterial(I);
  M->SetCustomPrimitiveDataFloat(0, 42);
  N->SetCustomPrimitiveDataFloat(0, 17);
  auto *C = UPadmaNPRStudioLibrary::ApplyToActor(P, A, M, nullptr, Error);
  if (TestNotNull(*Error, C)) {
    TestTrue(TEXT("Editor stores persistent material"),
             !M->GetMaterial(I)->IsA<UMaterialInstanceDynamic>());
    TestEqual(TEXT("Other actor CPD untouched"),
              N->GetCustomPrimitiveData().Data[0], 17.f);
    auto *Applied = M->GetMaterial(I);
    C->RestoreMaterials();
    C->UpdateFrameData();
    TestEqual(TEXT("Restore remains stopped after update"),
              M->GetCustomPrimitiveData().Data[0], 42.f);
    auto *External = UMaterialInstanceDynamic::Create(Master, A);
    M->SetMaterial(I, External);
    UPadmaNPRStudioLibrary::ApplyToActor(P, A, M, nullptr, Error);
    UPadmaNPRStudioLibrary::RestoreActor(A, Error);
    TestEqual(TEXT("Restore preserves latest external replacement"),
              M->GetMaterial(I), (UMaterialInterface *)External);
    P->Slots[0].Material = Applied;
    TestNull(TEXT("Generated parent cycle rejected"),
             UPadmaNPRStudioLibrary::ApplyToActor(P, A, M, nullptr, Error));
    P->Slots[0].Material = Master;
    P->Slots[0].Skin.Wrap=-1;
    TestFalse(TEXT("Invalid Blueprint wrap rejected"),P->Validate(Error));
    P->Slots[0].Skin.Wrap=.35f;
    auto* Alternate=MakeMesh(A);
    TestNotNull(TEXT("Restore permits another mesh on same actor"),UPadmaNPRStudioLibrary::ApplyToActor(P,A,Alternate,nullptr,Error));
    UPadmaNPRStudioLibrary::RestoreActor(A,Error);
    P->Slots.Add(S);
    TestFalse(TEXT("Duplicate slot rejected before writes"),
              P->Validate(Error));
  }
  W->DestroyActor(A);
  W->DestroyActor(B);
  // Exercise the runtime path in an isolated game world, with two independent
  // owners.
  UWorld::InitializationValues Init;
  Init.AllowAudioPlayback(false)
      .CreatePhysicsScene(false)
      .ShouldSimulatePhysics(false)
      .CreateNavigation(false)
      .CreateAISystem(false);
  auto *Game = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr,
                                   true, ERHIFeatureLevel::Num, &Init);
  if (TestNotNull(TEXT("Runtime test world"), Game)) {
    auto *GA = Game->SpawnActor<AActor>();
    auto *GB = Game->SpawnActor<AActor>();
    auto *MA = MakeMesh(GA);
    auto *MB = MakeMesh(GB);
    auto *PA = NewObject<UPadmaNPRProfile>();
    PA->Slots.Add(S);
    auto *PB = NewObject<UPadmaNPRProfile>();
    PB->Slots.Add(S);
    PB->Slots[0].Tint = FLinearColor::Red;
    auto Bind = [&](AActor *Owner, USkeletalMeshComponent *Mesh,
                    UPadmaNPRProfile *Profile) {
      auto *C = NewObject<UPadmaNPRComponent>(Owner);
      Owner->AddInstanceComponent(C);
      C->TargetMesh = Mesh;
      C->Profile = Profile;
      C->RegisterComponent();
      return C;
    };
    auto *CA = Bind(GA, MA, PA);
    auto *CB = Bind(GB, MB, PB);
    auto *MIDa = Cast<UMaterialInstanceDynamic>(MA->GetMaterial(I));
    auto *MIDb = Cast<UMaterialInstanceDynamic>(MB->GetMaterial(I));
    TestNotNull(TEXT("Game creates MID"), MIDa);
    TestTrue(TEXT("Each actor owns a separate MID"),
             MIDa && MIDb && MIDa != MIDb);
    if (MIDa && MIDb) {
      FLinearColor TA, TB;
      MIDa->GetVectorParameterValue(FMaterialParameterInfo("NPR_Tint"), TA);
      MIDb->GetVectorParameterValue(FMaterialParameterInfo("NPR_Tint"), TB);
      TestEqual(TEXT("First profile untouched"), TA, FLinearColor::White);
      TestEqual(TEXT("Second profile override"), TB, FLinearColor::Red);
    }
    const FVector PreviousForward(MA->GetCustomPrimitiveData().Data[4],
                                  MA->GetCustomPrimitiveData().Data[5],
                                  MA->GetCustomPrimitiveData().Data[6]);
    const auto OtherData = MB->GetCustomPrimitiveData().Data;
    GA->SetActorRotation(FRotator(0, 90, 0));
    CA->UpdateFrameData();
    const FVector NextForward(MA->GetCustomPrimitiveData().Data[4],
                              MA->GetCustomPrimitiveData().Data[5],
                              MA->GetCustomPrimitiveData().Data[6]);
    TestTrue(TEXT("Head basis follows owner transform"),
             !PreviousForward.Equals(NextForward, .01f));
    TestTrue(TEXT("Second actor frame data unchanged"),
             OtherData == MB->GetCustomPrimitiveData().Data);
    CA->RestoreMaterials();
    CA->UpdateFrameData();
    TestEqual(TEXT("Runtime restore"), MA->GetMaterial(I), Original);
    TestEqual(TEXT("Other actor MID survives restore"), MB->GetMaterial(I),
              (UMaterialInterface *)MIDb);
    Game->DestroyWorld(false);
  }

  for (int K = 0; K < 5; ++K) {
    FString Path = TEXT("/PadmaNPR/Materials/M_NPR_") +
                   StaticEnum<EPadmaNPRSurface>()->GetNameStringByValue(K);
    auto *Material = LoadObject<UMaterial>(nullptr, *Path);
    if (TestNotNull(*Path, Material)) {
      TestEqual(TEXT("Thin master"),
                Material->GetExpressionCollection().Expressions.Num(), 1);
      Material->EnsureIsComplete();
      TestTrue(TEXT("Compiled material"),
               Material->GetMaterialResource(GMaxRHIShaderPlatform)
                   ->GetCompileErrors()
                   .IsEmpty());
    }
  }
  return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaCharacterGPU, "PadmaNPR.Character.GPU",
                                 EAutomationTestFlags::EditorContext |
                                     EAutomationTestFlags::ProductFilter)
bool FPadmaCharacterGPU::RunTest(const FString &) {
  auto *M = NewObject<UMaterial>();
  M->SetShadingModel(MSM_Unlit);
  auto *C = CastChecked<UMaterialExpressionCustom>(
      UMaterialEditingLibrary::CreateMaterialExpression(
          M, UMaterialExpressionCustom::StaticClass()));
  C->OutputType = CMOT_Float3;
  for (const TCHAR *I :
       {TEXT("PadmaCharacterCommon"), TEXT("PadmaFace"), TEXT("PadmaHair"),
        TEXT("PadmaSkin"), TEXT("PadmaEye"), TEXT("PadmaReferenceSurfaces")})
    C->IncludeFilePaths.Add(FString(TEXT("/Plugin/PadmaNPR/Private/")) + I +
                            TEXT(".ush"));
  C->Code = TEXT(R"HLSL(
float3 F=float3(1,0,0),R=float3(0,1,0),U=float3(0,0,1);float4 ctl=float4(0,.04,1,0);
float front=PadmaFaceSDF(.5,F,F,R,U,ctl),back=PadmaFaceSDF(.5,-F,F,R,U,ctl);
float left=PadmaFaceSDF(.5,normalize(float3(.2,1,.2)),F,R,U,ctl),right=PadmaFaceSDF(.5,normalize(float3(.2,-1,.2)),F,R,U,ctl);
float topA=PadmaFaceSDF(.2,normalize(float3(.00001,0,1)),F,R,U,ctl),topB=PadmaFaceSDF(.2,normalize(float3(-.00001,0,1)),F,R,U,ctl);
float wide=PadmaHairLobe(R,U,U,0,40),narrow=PadmaHairLobe(R,U,normalize(float3(0,1,1)),0,200);
float3 eye=PadmaEyeResponse(U,U,U,0,float4(0,0,128,.4));
bool referenceBand=PadmaFaceShadowBand(.5,.3,.05,0)<.001 && abs(PadmaFaceShadowBand(.5,.5,.05,0)-.5)<.001 && PadmaFaceShadowBand(.5,.7,.05,0)>.999;
bool referenceStep=PadmaFaceShadowBand(.5,.5,0,0)==1 && PadmaFaceShadowBand(.5,.49,0,0)==0 && PadmaFaceShadowBand(.5,.5,1e-10,0)==1;
bool referenceTop=PadmaFaceOverheadShadow(1,1,1)<.001 && PadmaFaceOverheadShadow(1,0,1)>.999 && PadmaFaceOverheadShadow(1,1,0)>.999;
return float3(front>.99&&back<.01&&abs(left-right)<.001&&abs(topA-topB)<.001&&referenceBand&&referenceStep&&referenceTop?1:0,wide>.99&&narrow<.01?1:0,all(abs(eye-.4)<.001)?1:0);
)HLSL");
  UMaterialEditingLibrary::ConnectMaterialProperty(C, TEXT(""),
                                                   MP_EmissiveColor);
  M->PostEditChange();
  M->EnsureIsComplete();
  GShaderCompilingManager->FinishAllCompilation();
  if (!TestTrue(TEXT("Shader compiles"),
                M->GetMaterialResource(GMaxRHIShaderPlatform)
                    ->GetCompileErrors()
                    .IsEmpty()))
    return false;
  auto *RT = NewObject<UTextureRenderTarget2D>();
  RT->RenderTargetFormat = RTF_RGBA16f;
  RT->InitAutoFormat(16, 16);
  RT->UpdateResourceImmediate(true);
  UKismetRenderingLibrary::DrawMaterialToRenderTarget(
      GEditor->GetEditorWorldContext().World(), RT, M);
  FlushRenderingCommands();
  TArray<FLinearColor> Pixels;
  RT->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(Pixels);
  if (!TestTrue(TEXT("GPU readback"), Pixels.Num() > 0))
    return false;
  TestTrue(TEXT("SDF directions/overhead"), Pixels[0].R > .99);
  TestTrue(TEXT("Hair anisotropic lobe"), Pixels[0].G > .99);
  TestTrue(TEXT("Eye independent highlight"), Pixels[0].B > .99);
  return true;
}
#endif
