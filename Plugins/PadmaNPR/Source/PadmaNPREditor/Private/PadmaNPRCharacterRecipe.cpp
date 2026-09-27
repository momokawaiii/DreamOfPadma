#include "PadmaNPRCharacterRecipe.h"
#include "Animation/AnimSequence.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Texture2D.h"
#include "MaterialEditingLibrary.h"
#include "MaterialShared.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionCameraVectorWS.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "Materials/MaterialExpressionTransform.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Materials/MaterialExpressionVertexTangentWS.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Misc/PackageName.h"
#include "Misc/ScopeExit.h"
#include "PadmaNPRProfile.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "ShaderCompiler.h"
#include "UObject/SavePackage.h"

namespace {
template <class T> T *Node(UMaterial *M) {
  const int32 Index = M->GetExpressions().Num();
  return CastChecked<T>(UMaterialEditingLibrary::CreateMaterialExpression(
      M, T::StaticClass(), -1800 + (Index % 5) * 350, (Index / 5) * 240));
}
void Input(UMaterialExpressionCustom *C, FName Name, UMaterialExpression *E,
           int32 Output = 0) {
  FCustomInput I;
  I.InputName = Name;
  I.Input.Connect(Output, E);
  C->Inputs.Add(I);
}
UMaterialExpressionVectorParameter *Vector(UMaterial *M, FName Name,
                                           FLinearColor Value, int32 CPD = -1) {
  auto *N = Node<UMaterialExpressionVectorParameter>(M);
  N->ParameterName = Name;
  N->DefaultValue = Value;
  if (CPD >= 0) {
    N->bUseCustomPrimitiveData = true;
    N->PrimitiveDataIndex = CPD;
  }
  return N;
}
UMaterialExpressionTextureSampleParameter2D *
Sample(UMaterial *M, FName Name, UTexture2D *T, UMaterialExpression *UV) {
  auto *N = Node<UMaterialExpressionTextureSampleParameter2D>(M);
  N->ParameterName = Name;
  N->Texture = T;
  N->SamplerType =
      T->CompressionSettings == TC_Normalmap
          ? SAMPLERTYPE_Normal
          : (T->SRGB ? SAMPLERTYPE_Color : SAMPLERTYPE_LinearColor);
  if (T->CompressionSettings == TC_Masks)
    N->SamplerType = SAMPLERTYPE_Masks;
  if (UV)
    N->Coordinates.Connect(0, UV);
  return N;
}
bool Save(UObject *O) {
  O->MarkPackageDirty();
  FSavePackageArgs A;
  A.TopLevelFlags = RF_Public | RF_Standalone;
  A.SaveFlags = SAVE_NoError;
  return UPackage::SavePackage(
      O->GetPackage(), O,
      *FPackageName::LongPackageNameToFilename(
          O->GetPackage()->GetName(), FPackageName::GetAssetPackageExtension()),
      A);
}
} // namespace
void UPadmaNPRCharacterRecipe::ReadSlots() {
  TArray<FPadmaManualSlot> Old = Slots;
  Slots.Reset();
  if (!Mesh)
    return;
  for (const auto &M : Mesh->GetMaterials()) {
    FPadmaManualSlot S;
    S.SlotName = M.MaterialSlotName;
    if (auto *Existing = Old.FindByPredicate(
            [&](const auto &V) { return V.SlotName == S.SlotName; }))
      S = *Existing;
    Slots.Add(S);
  }
  if (HeadBone.IsNone() && Mesh->GetRefSkeleton().GetNum())
    HeadBone = Mesh->GetRefSkeleton().GetBoneName(0);
}
bool UPadmaNPRCharacterRecipe::Validate(FString &Error) const {
  Error.Reset();
  if (!Mesh) {
    Error = TEXT("请选择角色骨骼网格。");
    return false;
  }
  if (Mesh->GetRefSkeleton().FindBoneIndex(HeadBone) == INDEX_NONE) {
    Error = TEXT("请选择网格中存在的头部骨骼；无 SDF 时可选择根骨骼。");
    return false;
  }
  if (Animation && Animation->GetSkeleton() != Mesh->GetSkeleton()) {
    Error = TEXT("预览动画与网格骨架不一致。");
    return false;
  }
  TSet<FName> Names;
  int32 Count = 0;
  for (const auto &S : Slots) {
    if (Names.Contains(S.SlotName) ||
        !Mesh->GetMaterials().ContainsByPredicate(
            [&](const auto &M) { return M.MaterialSlotName == S.SlotName; })) {
      Error = TEXT("槽名重复或不属于当前网格，请重新读取槽位。");
      return false;
    }
    Names.Add(S.SlotName);
    if (S.Surface == EPadmaManualSurface::KeepOriginal)
      continue;
    ++Count;
    if (auto *Render = Mesh->GetResourceForRendering())
      for (const auto &LOD : Render->LODRenderData)
        if (S.UVChannel >= int32(LOD.GetNumTexCoords())) {
          Error =
              S.SlotName.ToString() + TEXT(": 某个 LOD 没有选择的 UV 通道。");
          return false;
        }
    for (auto *T :
         {S.BaseColor.Get(), S.Normal.Get(), S.Roughness.Get(),
          S.Metallic.Get(), S.AO.Get(), S.FaceSDF.Get(), S.Matcap.Get()})
      if (T && T->VirtualTextureStreaming) {
        Error =
            S.SlotName.ToString() +
            TEXT(": 此版本不支持 Virtual Texture，请使用普通 Texture2D 副本。");
        return false;
      }
    if (S.UVChannel < 0 || S.UVChannel > 3 || !FMath::IsFinite(S.Feather) ||
        S.Feather < .001f || !FMath::IsFinite(S.Threshold) ||
        !FMath::IsFinite(S.Highlight) || !FMath::IsFinite(S.Rim) ||
        !FMath::IsFinite(S.MatcapStrength) ||
        !FMath::IsFinite(S.NormalStrength) ||
        !FMath::IsFinite(S.ConstantRoughness) ||
        !FMath::IsFinite(S.ConstantMetallic) || S.UVScale.ContainsNaN() ||
        S.UVOffset.ContainsNaN() || !FMath::IsFinite(S.Tint.R) ||
        !FMath::IsFinite(S.Tint.G) || !FMath::IsFinite(S.Tint.B) ||
        !FMath::IsFinite(S.ShadowColor.R) ||
        !FMath::IsFinite(S.ShadowColor.G) ||
        !FMath::IsFinite(S.ShadowColor.B)) {
      Error = S.SlotName.ToString() + TEXT(": 非法数值或 UV 通道。");
      return false;
    }
    for (auto *T :
         {S.Roughness.Get(), S.Metallic.Get(), S.AO.Get(), S.FaceSDF.Get()})
      if (T && (T->SRGB || T->CompressionSettings == TC_Normalmap)) {
        Error = S.SlotName.ToString() +
                TEXT(": 数值遮罩必须关闭 sRGB，且不能使用 Normal "
                     "压缩。请使用独立副本调整导入设置。");
        return false;
      }
    if (S.Normal && S.Normal->CompressionSettings != TC_Normalmap) {
      Error = S.SlotName.ToString() +
              TEXT(": 此入口仅接受 UE 标准切线 Normal 压缩；自定义 RG "
                   "图请先转换，或使用原参考材质。");
      return false;
    }
    for (auto *T : {S.BaseColor.Get(), S.Matcap.Get()})
      if (T && T->CompressionSettings == TC_Normalmap) {
        Error = S.SlotName.ToString() + TEXT(": 颜色图不能使用 Normal 压缩。");
        return false;
      }
    if (S.FaceSDF && S.Surface != EPadmaManualSurface::Face) {
      Error = S.SlotName.ToString() + TEXT(": SDF 只能用于 Face 类型。");
      return false;
    }
  }
  if (!Count) {
    Error = TEXT("至少将一个槽位从 Keep Original 改为材质类型。");
    return false;
  }
  return true;
}
UPadmaNPRProfile *UPadmaNPRCharacterRecipe::Build(bool bSave, FString &Error) {
  if (!Validate(Error))
    return nullptr;
  if (bSave && (!OutputFolder.StartsWith(TEXT("/Game/")) ||
                !FPackageName::IsValidLongPackageName(OutputFolder))) {
    Error = TEXT("输出目录必须是有效的 /Game/... 路径。");
    return nullptr;
  }
  TArray<FString> Names = {TEXT("T_NPR_LinearWhite"), TEXT("DA_NPR_Character"),
                           TEXT("DA_NPR_Recipe")};
  for (int32 I = 0; I < Slots.Num(); ++I)
    if (Slots[I].Surface != EPadmaManualSurface::KeepOriginal) {
      Names.Add(FString::Printf(TEXT("M_NPR_Manual_%d"), I));
      Names.Add(FString::Printf(TEXT("MI_NPR_Source_%d"), I));
    }
  if (bSave)
    for (const auto &N : Names) {
      FString Path = OutputFolder / N;
      if (FPackageName::DoesPackageExist(Path) ||
          StaticFindObject(UObject::StaticClass(), nullptr,
                           *(Path + TEXT(".") + N))) {
        Error = TEXT("输出已存在，请选择新目录；不会覆盖已有配置：") + Path;
        return nullptr;
      }
    }
  TArray<UObject *> Created;
  TSet<UObject *> Saved;
  bool Complete = false;
  ON_SCOPE_EXIT {
    if (!Complete)
      for (auto *O : Created)
        if (!Saved.Contains(O)) {
          O->ClearFlags(RF_Public | RF_Standalone);
          O->Rename(nullptr, GetTransientPackage(),
                    REN_DontCreateRedirectors | REN_NonTransactional);
        }
  };
  auto Make = [&](UClass *Class, const FString &Name) -> UObject * {
    UObject *O =
        bSave ? NewObject<UObject>(CreatePackage(*(OutputFolder / Name)), Class,
                                   *Name, RF_Public | RF_Standalone)
              : NewObject<UObject>(GetTransientPackage(), Class);
    Created.Add(O);
    return O;
  };
  auto *White = CastChecked<UTexture2D>(
      Make(UTexture2D::StaticClass(), TEXT("T_NPR_LinearWhite")));
  uint8 Bytes[4] = {255, 255, 255, 255};
  White->SRGB = false;
  White->CompressionSettings = TC_Masks;
  White->Source.Init(1, 1, 1, 1, TSF_BGRA8, Bytes);
  White->PostEditChange();
  auto *ColorWhite = LoadObject<UTexture2D>(
      nullptr,
      TEXT("/Engine/EngineResources/WhiteSquareTexture.WhiteSquareTexture"));
  auto *Flat = LoadObject<UTexture2D>(
      nullptr, TEXT("/Engine/EngineMaterials/DefaultNormal.DefaultNormal"));
  if (!ColorWhite || !Flat) {
    Error = TEXT("缺少引擎默认纹理。");
    return nullptr;
  }
  auto *P = CastChecked<UPadmaNPRProfile>(
      Make(UPadmaNPRProfile::StaticClass(), TEXT("DA_NPR_Character")));
  P->HeadSocket = HeadBone;
  P->HeadAxisCorrection = HeadAxes;
  P->FallbackLightDirection = -FRotator(LightPitch, LightYaw, 0).Vector();
  for (int32 Index = 0; Index < Slots.Num(); ++Index) {
    const auto &S = Slots[Index];
    if (S.Surface == EPadmaManualSurface::KeepOriginal)
      continue;
    auto *M = CastChecked<UMaterial>(
        Make(UMaterial::StaticClass(),
             FString::Printf(TEXT("M_NPR_Manual_%d"), Index)));
    // Transient preview names must also satisfy the backend validator.
    if (!bSave)
      M->Rename(*MakeUniqueObjectName(GetTransientPackage(),
                                      UMaterial::StaticClass(),
                                      TEXT("M_NPR_Manual_Preview"))
                     .ToString());
    M->SetShadingModel(MSM_Unlit);
    M->TwoSided = true;
    M->SetUsageByFlag(MATUSAGE_SkeletalMesh, true);
    auto *TexCoord = Node<UMaterialExpressionTextureCoordinate>(M);
    TexCoord->CoordinateIndex = S.UVChannel;
    auto *UV = Node<UMaterialExpressionCustom>(M);
    UV->OutputType = CMOT_Float2;
    UV->Code = TEXT("return U*Transform.xy+Transform.zw;");
    Input(UV, TEXT("U"), TexCoord);
    Input(UV, TEXT("Transform"),
          Vector(M, TEXT("InputUV"),
                 FLinearColor(S.UVScale.X, S.UVScale.Y, S.UVOffset.X,
                              S.UVOffset.Y)),
          5);
    auto *Mirror = Node<UMaterialExpressionCustom>(M);
    Mirror->OutputType = CMOT_Float2;
    Mirror->Code = TEXT("return float2(1-U.x,U.y);");
    Input(Mirror, TEXT("U"), UV);
    auto *Base = Sample(M, TEXT("BaseColorTex"),
                        S.BaseColor ? S.BaseColor.Get() : ColorWhite, UV);
    auto *Normal =
        Sample(M, TEXT("NormalTex"), S.Normal ? S.Normal.Get() : Flat, UV);
    auto *Adjust = Node<UMaterialExpressionCustom>(M);
    Adjust->OutputType = CMOT_Float3;
    Adjust->Code = TEXT(
        "return "
        "normalize(float3(N.xy*Config.x*float2(1,Config.y),max(N.z,.001)));");
    Input(Adjust, TEXT("N"), Normal);
    Input(Adjust, TEXT("Config"),
          Vector(M, TEXT("NormalConfig"),
                 FLinearColor(S.NormalStrength, S.bFlipNormalGreen ? -1 : 1, 0,
                              0)));
    auto *World = Node<UMaterialExpressionTransform>(M);
    World->TransformSourceType = TRANSFORMSOURCE_Tangent;
    World->TransformType = TRANSFORM_World;
    World->Input.Connect(0, Adjust);
    auto *Core = Node<UMaterialExpressionCustom>(M);
    Core->OutputType = CMOT_Float3;
    Core->IncludeFilePaths.Add(
        TEXT("/Plugin/PadmaNPR/Private/PadmaManualCore.ush"));
    Core->Code =
        TEXT("return "
             "PadmaManualEvaluate(Base,N,T,V,L,F,R,Up,Tint,Shade,Style,Surface,"
             "Controls,Rough,Metal,AO,SDF,Mirror,Matcap);");
    Input(Core, TEXT("Base"), Base);
    Input(Core, TEXT("N"), World);
    Input(Core, TEXT("T"), Node<UMaterialExpressionVertexTangentWS>(M));
    Input(Core, TEXT("V"), Node<UMaterialExpressionCameraVectorWS>(M));
    Input(Core, TEXT("L"),
          Vector(M, TEXT("ManualLight"),
                 FLinearColor(P->FallbackLightDirection.X,
                              P->FallbackLightDirection.Y,
                              P->FallbackLightDirection.Z, 1),
                 0),
          5);
    Input(Core, TEXT("F"),
          Vector(M, TEXT("ManualForward"), FLinearColor(1, 0, 0, 0), 4));
    Input(Core, TEXT("R"),
          Vector(M, TEXT("ManualRight"), FLinearColor(0, 1, 0, 0), 8));
    Input(Core, TEXT("Up"),
          Vector(M, TEXT("ManualUp"), FLinearColor(0, 0, 1, 0), 12));
    Input(Core, TEXT("Tint"), Vector(M, TEXT("ManualTint"), S.Tint), 5);
    Input(Core, TEXT("Shade"), Vector(M, TEXT("ManualShade"), S.ShadowColor),
          5);
    Input(Core, TEXT("Style"),
          Vector(M, TEXT("ManualStyle"),
                 FLinearColor(S.Threshold, S.Feather, S.Rim, S.Highlight)),
          5);
    Input(Core, TEXT("Surface"),
          Vector(M, TEXT("ManualSurface"),
                 FLinearColor((int32)S.Surface, S.Matcap ? S.MatcapStrength : 0,
                              0, 0)),
          5);
    Input(Core, TEXT("Controls"),
          Vector(M, TEXT("ManualControls"),
                 FLinearColor(S.FaceSDF ? 1 : 0, S.bMirrorSDF ? 1 : 0, 0, 0)),
          5);
    auto Mask = [&](FName Name, UTexture2D *Texture,
                    EPadmaTextureChannel Channel, float Constant,
                    bool Invert = false,
                    UMaterialExpression *Coords = nullptr) {
      auto *C = Node<UMaterialExpressionCustom>(M);
      C->OutputType = CMOT_Float1;
      C->Code =
          Texture
              ? FString::Printf(TEXT("return %sX.%s;"),
                                Invert ? TEXT("1-") : TEXT(""),
                                *FString(TEXT("rgba")).Mid((int32)Channel, 1))
              : FString::Printf(TEXT("return %.8ff;"), Constant);
      Input(C, TEXT("X"),
            Sample(M, Name, Texture ? Texture : White, Coords ? Coords : UV),
            5);
      Input(Core, Name, C);
    };
    Mask(TEXT("Rough"), S.Roughness, S.RoughnessChannel, S.ConstantRoughness,
         S.bGlossiness);
    Mask(TEXT("Metal"), S.Metallic, S.MetallicChannel, S.ConstantMetallic);
    Mask(TEXT("AO"), S.AO, S.AOChannel, 1);
    Mask(TEXT("SDF"), S.FaceSDF, S.SDFChannel, 1);
    Mask(TEXT("Mirror"), S.FaceSDF, S.SDFChannel, 1, false, Mirror);
    auto *ViewN = Node<UMaterialExpressionTransform>(M);
    ViewN->TransformSourceType = TRANSFORMSOURCE_World;
    ViewN->TransformType = TRANSFORM_View;
    ViewN->Input.Connect(0, World);
    auto *MatUV = Node<UMaterialExpressionCustom>(M);
    MatUV->OutputType = CMOT_Float2;
    MatUV->Code = TEXT("return N.xy*float2(.5,-.5)+.5;");
    Input(MatUV, TEXT("N"), ViewN);
    Input(Core, TEXT("Matcap"),
          Sample(M, TEXT("MatcapTex"), S.Matcap ? S.Matcap.Get() : ColorWhite,
                 MatUV));
    UMaterialEditingLibrary::ConnectMaterialProperty(Core, TEXT(""),
                                                     MP_EmissiveColor);
    M->PreEditChange(nullptr);
    M->PostEditChange();
    auto *MI = CastChecked<UMaterialInstanceConstant>(
        Make(UMaterialInstanceConstant::StaticClass(),
             FString::Printf(TEXT("MI_NPR_Source_%d"), Index)));
    MI->SetParentEditorOnly(M);
    UMaterialEditingLibrary::UpdateMaterialInstance(MI);
    FPadmaNPRSlot Slot;
    Slot.SlotName = S.SlotName;
    Slot.Material = MI;
    Slot.bReferenceResponse = true;
    Slot.bManualResponse = true;
    Slot.Type =
        S.Surface == EPadmaManualSurface::Hair   ? EPadmaNPRSurface::Hair
        : S.Surface == EPadmaManualSurface::Face ? EPadmaNPRSurface::Face
        : S.Surface == EPadmaManualSurface::Eye  ? EPadmaNPRSurface::Eye
        : S.Surface == EPadmaManualSurface::Brow ? EPadmaNPRSurface::Brow
        : S.Surface == EPadmaManualSurface::Skin ? EPadmaNPRSurface::Skin
                                                 : EPadmaNPRSurface::Cloth;
    Slot.ReferenceVectors.Add(TEXT("ManualTint"), S.Tint);
    Slot.ReferenceVectors.Add(TEXT("ManualShade"), S.ShadowColor);
    Slot.ReferenceVectors.Add(
        TEXT("ManualStyle"),
        FLinearColor(S.Threshold, S.Feather, S.Rim, S.Highlight));
    Slot.ReferenceVectors.Add(
        TEXT("ManualSurface"),
        FLinearColor((int32)S.Surface, S.Matcap ? S.MatcapStrength : 0, 0, 0));
    P->Slots.Add(Slot);
  }
  if (!P->Validate(Error))
    return nullptr;
  if (GShaderCompilingManager)
    GShaderCompilingManager->FinishAllCompilation();
  for (auto *O : Created)
    if (auto *M = Cast<UMaterial>(O)) {
      auto *R = M->GetMaterialResource(GMaxRHIShaderPlatform);
      if (!R || !R->GetCompileErrors().IsEmpty() ||
          !R->GetGameThreadShaderMap()) {
        Error = TEXT("材质编译失败：") + M->GetName();
        if (R)
          Error += FString::Join(R->GetCompileErrors(), TEXT("\n"));
        return nullptr;
      }
    }
  if (bSave) {
    auto *Recipe = DuplicateObject<UPadmaNPRCharacterRecipe>(
        this, CreatePackage(*(OutputFolder / TEXT("DA_NPR_Recipe"))),
        TEXT("DA_NPR_Recipe"));
    Recipe->SetFlags(RF_Public | RF_Standalone);
    Created.Add(Recipe);
    for (auto *O : Created) {
      if (!Save(O)) {
        Error = TEXT("部分输出已生成，保存失败：") + O->GetPathName();
        return nullptr;
      }
      Saved.Add(O);
      FAssetRegistryModule::AssetCreated(O);
    }
  }
  Complete = true;
  return P;
}
