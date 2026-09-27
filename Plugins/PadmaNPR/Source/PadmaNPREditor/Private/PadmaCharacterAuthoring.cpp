#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include "HAL/IConsoleManager.h"
#include "MaterialEditingLibrary.h"
#include "MaterialShared.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionCameraVectorWS.h"
#include "Materials/MaterialExpressionConstant3Vector.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Materials/MaterialExpressionEyeAdaptationInverse.h"
#include "Materials/MaterialExpressionFunctionOutput.h"
#include "Materials/MaterialExpressionMakeMaterialAttributes.h"
#include "Materials/MaterialExpressionMaterialFunctionCall.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionTextureObjectParameter.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "Materials/MaterialExpressionTransform.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Materials/MaterialExpressionVertexNormalWS.h"
#include "Materials/MaterialFunction.h"
#include "Misc/PackageName.h"
#include "PadmaNPRProfile.h"
#include "ShaderCompiler.h"
#include "UObject/SavePackage.h"

namespace PadmaCharacterAuthoring {
template <class T> T *Node(UMaterialFunction *F, int X = 0, int Y = 0) {
  return CastChecked<T>(
      UMaterialEditingLibrary::CreateMaterialExpressionInFunction(
          F, T::StaticClass(), X, Y));
}
bool Save(UObject *A) {
  A->MarkPackageDirty();
  FAssetRegistryModule::AssetCreated(A);
  FSavePackageArgs Args;
  Args.TopLevelFlags = RF_Public | RF_Standalone;
  return UPackage::SavePackage(
      A->GetPackage(), A,
      *FPackageName::LongPackageNameToFilename(
          A->GetPackage()->GetName(), FPackageName::GetAssetPackageExtension()),
      Args);
}
void Neutral(const TCHAR *Name, FColor Color) {
  FString Path = FString(TEXT("/PadmaNPR/Textures/")) + Name;
  if (FPackageName::DoesPackageExist(Path))
    return;
  auto *T = NewObject<UTexture2D>(CreatePackage(*Path), Name,
                                  RF_Public | RF_Standalone);
  T->SRGB = false;
  T->CompressionSettings = TC_Default;
  T->MipGenSettings = TMGS_NoMipmaps;
  T->Source.Init(1, 1, 1, 1, TSF_BGRA8,
                 reinterpret_cast<const uint8 *>(&Color));
  T->PostEditChange();
  Save(T);
}
bool Create(EPadmaNPRSurface Type, FString &Error) {
  Neutral(TEXT("T_NPR_WhiteData"), FColor::White);
  Neutral(TEXT("T_NPR_BlackData"), FColor::Black);
  Neutral(TEXT("T_NPR_GrayData"), FColor(128, 128, 128, 255));
  const FString Label =
      StaticEnum<EPadmaNPRSurface>()->GetNameStringByValue((int64)Type);
  const FString MP = TEXT("/PadmaNPR/Materials/M_NPR_") + Label,
                FP = TEXT("/PadmaNPR/Functions/MF_Padma") + Label;
  if (FPackageName::DoesPackageExist(MP) &&
      FPackageName::DoesPackageExist(FP)) {
    Error = TEXT("Already exists: ") + MP;
    return true;
  }
  if (FPackageName::DoesPackageExist(MP) ||
      FPackageName::DoesPackageExist(FP) || FindPackage(nullptr, *MP) ||
      FindPackage(nullptr, *FP)) {
    Error =
        TEXT(
            "Partial or unsaved target exists; inspect before regenerating: ") +
        MP;
    return false;
  }
  auto *F = NewObject<UMaterialFunction>(CreatePackage(*FP),
                                         *FPackageName::GetShortName(FP),
                                         RF_Public | RF_Standalone);
  auto *C = Node<UMaterialExpressionCustom>(F, 300, 0);
  C->OutputType = CMOT_Float3;
  C->Description = TEXT("Padma ") + Label + TEXT(" / modular HLSL");
  for (const TCHAR *Include :
       {TEXT("PadmaCharacterCommon"), TEXT("PadmaFace"), TEXT("PadmaSkin"),
        TEXT("PadmaHair"), TEXT("PadmaEye")})
    C->IncludeFilePaths.Add(FString(TEXT("/Plugin/PadmaNPR/Private/")) +
                            Include + TEXT(".ush"));
  auto Input = [&](FName Name, UMaterialExpression *E, int Index = 0) {
    FCustomInput I;
    I.InputName = Name;
    I.Input.Connect(Index, E);
    C->Inputs.Add(I);
  };
  FPadmaNPRSlot Defaults;
  Defaults.Type = Type;
  TMap<FName, FLinearColor> Values;
  Defaults.GetParameters(1, Values);
  int Y = 0;
  TArray<FName> Keys;
  Values.GetKeys(Keys);
  Keys.Sort(FNameLexicalLess());
  for (FName K : Keys) {
    auto *P = Node<UMaterialExpressionVectorParameter>(F, -800, Y);
    Y += 150;
    P->ParameterName = K;
    P->DefaultValue = Values[K];
    P->Group = TEXT("Profile / generated values");
    Input(K, P, 5);
  }
  const TArray<FName> FrameNames = {"FrameLight", "HeadForward", "HeadRight",
                                    "HeadUp", "HeadOrigin"};
  const TArray<FLinearColor> FrameDefaults = {
      FLinearColor(.3f, .5f, 1, 0), FLinearColor(1, 0, 0, 0),
      FLinearColor(0, 1, 0, 0), FLinearColor(0, 0, 1, 0), FLinearColor::Black};
  for (int I = 0; I < FrameNames.Num(); ++I) {
    auto *P = Node<UMaterialExpressionVectorParameter>(F, -1100, Y);
    Y += 150;
    P->ParameterName = FrameNames[I];
    P->DefaultValue = FrameDefaults[I];
    P->bUseCustomPrimitiveData = true;
    P->PrimitiveDataIndex = I * 4;
    Input(FrameNames[I], P, 5);
  }
  auto *UV = Node<UMaterialExpressionTextureCoordinate>(F, -1300, 0);
  Input("UV", UV);
  auto Tex = [&](const TCHAR *Name, const TCHAR *Path,
                 EMaterialSamplerType Sampler, bool Object = false) {
    if (Object) {
      auto *T = Node<UMaterialExpressionTextureObjectParameter>(F, -1000, Y);
      Y += 180;
      T->ParameterName = Name;
      T->Texture = LoadObject<UTexture>(nullptr, Path);
      T->SamplerType = Sampler;
      T->Group = TEXT("Resources");
      Input(Name, T);
    } else {
      auto *T = Node<UMaterialExpressionTextureSampleParameter2D>(F, -1000, Y);
      Y += 180;
      T->ParameterName = Name;
      T->Texture = LoadObject<UTexture>(nullptr, Path);
      T->SamplerType = Sampler;
      T->Group = TEXT("Resources");
      T->Coordinates.Connect(0, UV);
      Input(Name, T, 5);
    }
  };
  Tex(TEXT("NPR_BaseColor"),
      TEXT("/Engine/EngineResources/WhiteSquareTexture.WhiteSquareTexture"),
      SAMPLERTYPE_Color, true);
  Tex(TEXT("NPR_Normal"),
      TEXT("/Engine/EngineMaterials/DefaultNormal.DefaultNormal"),
      SAMPLERTYPE_Normal);
  Tex(TEXT("NPR_PackedMask"),
      TEXT("/PadmaNPR/Textures/T_NPR_WhiteData.T_NPR_WhiteData"),
      SAMPLERTYPE_LinearColor);
  Tex(TEXT("NPR_SDF"),
      TEXT("/PadmaNPR/Textures/T_NPR_WhiteData.T_NPR_WhiteData"),
      SAMPLERTYPE_LinearColor, true);
  Tex(TEXT("NPR_Shift"),
      TEXT("/PadmaNPR/Textures/T_NPR_GrayData.T_NPR_GrayData"),
      SAMPLERTYPE_LinearColor);
  Tex(TEXT("NPR_RegionMask"),
      TEXT("/PadmaNPR/Textures/T_NPR_BlackData.T_NPR_BlackData"),
      SAMPLERTYPE_LinearColor);
  Tex(TEXT("NPR_Matcap"),
      TEXT("/Engine/EngineResources/WhiteSquareTexture.WhiteSquareTexture"),
      SAMPLERTYPE_Color, true);
  auto *GN = Node<UMaterialExpressionVertexNormalWS>(F, -1000, Y);
  Y += 150;
  Input("GN", GN);
  auto *V = Node<UMaterialExpressionCameraVectorWS>(F, -1000, Y);
  Y += 150;
  Input("V", V);
  // A normal texture is already decoded by its Normal sampler.
  UMaterialExpression *NormalInput =
      C->Inputs
          .FindByPredicate([](const FCustomInput &I) {
            return I.InputName == TEXT("NPR_Normal");
          })
          ->Input.Expression;
  auto *N = Node<UMaterialExpressionTransform>(F, -400, Y);
  N->TransformSourceType = TRANSFORMSOURCE_Tangent;
  N->TransformType = TRANSFORM_World;
  N->Input.Connect(0, NormalInput);
  Input("MappedN", N);
  auto *Tangent = Node<UMaterialExpressionConstant3Vector>(F, -800, Y + 200);
  Tangent->Constant = FLinearColor(0, 1, 0);
  auto *TW = Node<UMaterialExpressionTransform>(F, -400, Y + 200);
  TW->TransformSourceType = TRANSFORMSOURCE_Tangent;
  TW->TransformType = TRANSFORM_World;
  TW->Input.Connect(0, Tangent);
  Input("StrandT", TW);
  for (auto Pair : TArray<TPair<FName, ECustomMaterialOutputType>>{
           {TEXT("Opacity"), CMOT_Float1}, {TEXT("Exposure"), CMOT_Float1}}) {
    FCustomOutput O;
    O.OutputName = Pair.Key;
    O.OutputType = Pair.Value;
    C->AdditionalOutputs.Add(O);
  }
  C->Code = FString::Printf(TEXT("const int SurfaceType=%d;\n"), (int)Type) +
            TEXT(R"SHADER(
float3 N=PadmaSafeNormal(MappedN,GN),vn=PadmaSafeNormal(V,float3(0,0,1));
float3 L=PadmaSafeNormal(FrameLight.xyz,float3(.3,.5,1));
float3 F=PadmaSafeNormal(HeadForward.xyz,float3(1,0,0));
float3 R=PadmaSafeNormal(HeadRight.xyz,float3(0,1,0));
float3 U=PadmaSafeNormal(HeadUp.xyz,float3(0,0,1));
float2 baseUV=UV;
if(SurfaceType==3)baseUV+=float2(dot(vn,R),dot(vn,U))*NPR_Surface.z;
float4 base=Texture2DSample(NPR_BaseColor,NPR_BaseColorSampler,baseUV);
float band=PadmaBand(saturate(dot(N,L)),NPR_Cel.x,NPR_Cel.y);
float sdf=1;
if(SurfaceType==0){float2 uv=UV;if(dot(L,R)<0)uv.x=1-uv.x;sdf=0;
 [unroll]for(int x=-1;x<=1;++x)[unroll]for(int y=-1;y<=1;++y)sdf+=Texture2DSample(NPR_SDF,NPR_SDFSampler,uv+float2(x,y)*NPR_Face.w).b/9;
 band=lerp(band,PadmaFaceSDF(sdf,L,F,R,U,NPR_Face),saturate(NPR_Surface.x));}
float ao=SurfaceType==2?NPR_PackedMask.b:1;
float3 color=PadmaCharacterCel(base.rgb*NPR_Tint.rgb,band,ao,NPR_Shade.rgb,NPR_Fill,NPR_Cel.z);
if(SurfaceType==0||SurfaceType==1)color+=PadmaSkinResponse(base.rgb,N,L,NPR_Skin,NPR_Surface.y);
if(SurfaceType==2){float3 T=PadmaSafeNormal(StrandT-N*dot(StrandT,N),R);color+=PadmaHairResponse(N,T,vn,L,NPR_Shift.r,NPR_PackedMask.g,NPR_HairLobes,NPR_HairShift,NPR_HairTint.rgb);}
if(SurfaceType==3){float2 uv=PadmaCharacterMatcapUV(N,vn);float3 mc=Texture2DSample(NPR_Matcap,NPR_MatcapSampler,uv).rgb;color+=PadmaEyeResponse(N,vn,L,mc,NPR_Eye);}
color+=base.rgb*pow(1-saturate(dot(N,vn)),4)*NPR_Cel.w*saturate(dot(N,L));
float height=dot(LWCToFloat(GetWorldPosition(Parameters))-HeadOrigin.xyz,U);
float bangRegion=saturate(NPR_RegionMask.r)*(1-smoothstep(NPR_HairTint.a,NPR_HairTint.a+2,height));
Opacity=1;
if(SurfaceType==2){float front=lerp(1,saturate(dot(vn,F)),NPR_Alpha.y);Opacity=1-bangRegion*front*(1-NPR_Alpha.x);}
if(SurfaceType==4){color=NPR_Shade.rgb;Opacity=NPR_Alpha.z*saturate(dot(L,F)*.5+.5);}
if(NPR_Alpha.w==1)color=band.xxx;
if(NPR_Alpha.w==2)color=sdf.xxx;
if(NPR_Alpha.w==3)color=N*.5+.5;
if(NPR_Alpha.w==4)color=bangRegion.xxx;
Exposure=NPR_Surface.w;
return max(color,0);
)SHADER");
  C->RebuildOutputs();
  auto *Exposure = Node<UMaterialExpressionEyeAdaptationInverse>(F, 600, 0);
  Exposure->LightValueInput.Connect(0, C);
  Exposure->AlphaInput.Connect(2, C);
  auto *Attr = Node<UMaterialExpressionMakeMaterialAttributes>(F, 900, 0);
  Attr->EmissiveColor.Connect(0, Exposure);
  Attr->Opacity.Connect(1, C);
  Attr->OpacityMask.Connect(1, C);
  if (Type == EPadmaNPRSurface::Hair) {
    auto *D = Node<UMaterialExpressionMaterialFunctionCall>(F, 600, 400);
    D->SetMaterialFunction(LoadObject<UMaterialFunction>(
        nullptr, TEXT("/Engine/Functions/Engine_MaterialFunctions02/Utility/"
                      "DitherTemporalAA.DitherTemporalAA")));
    D->FunctionInputs[0].Input.Connect(1, C);
    Attr->OpacityMask.Connect(0, D);
  }
  auto *O = Node<UMaterialExpressionFunctionOutput>(F, 1200, 0);
  O->OutputName = TEXT("Surface");
  O->A.Connect(0, Attr);
  F->PostEditChange();
  F->UpdateDependentFunctionCandidates();
  UMaterialEditingLibrary::UpdateMaterialFunction(F);
  auto *M =
      NewObject<UMaterial>(CreatePackage(*MP), *FPackageName::GetShortName(MP),
                           RF_Public | RF_Standalone);
  M->SetShadingModel(MSM_Unlit);
  M->bUseMaterialAttributes = true;
  M->TwoSided = true;
  M->BlendMode =
      Type == EPadmaNPRSurface::HairShadow
          ? BLEND_Translucent
          : (Type == EPadmaNPRSurface::Hair ? BLEND_Masked : BLEND_Opaque);
  auto *Call = CastChecked<UMaterialExpressionMaterialFunctionCall>(
      UMaterialEditingLibrary::CreateMaterialExpression(
          M, UMaterialExpressionMaterialFunctionCall::StaticClass(), -350, 0));
  Call->SetMaterialFunction(F);
  M->GetEditorOnlyData()->MaterialAttributes.Connect(0, Call);
  M->UpdateCachedExpressionData();
  M->PostEditChange();
  M->EnsureIsComplete();
  GShaderCompilingManager->FinishAllCompilation();
  auto *Resource = M->GetMaterialResource(GMaxRHIShaderPlatform);
  if (!Resource || !Resource->GetCompileErrors().IsEmpty() ||
      !Resource->GetGameThreadShaderMap()) {
    Error = TEXT("Shader compilation failed: ") + Label;
    return false;
  }
  return Save(F) && Save(M);
}
} // namespace PadmaCharacterAuthoring
static FAutoConsoleCommand GenerateCharacter(
    TEXT("PadmaNPR.CreateCharacterTemplates"),
    TEXT("Create missing character materials without overwriting existing "
         "packages."),
    FConsoleCommandDelegate::CreateLambda([] {
      for (int I = 0; I < 5; ++I) {
        FString Error;
        bool OK = PadmaCharacterAuthoring::Create((EPadmaNPRSurface)I, Error);
        UE_LOG(LogTemp, Display, TEXT("Padma Character %d [%s]: %s"), I,
               OK ? TEXT("OK") : TEXT("FAILED"), *Error);
        if (!OK)
          break;
      }
    }));
