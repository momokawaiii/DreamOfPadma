#include "PadmaClothAuthoring.h"
#include "PadmaClothGraph.h"
#include "PadmaClothProfile.h"
#include "MaterialEditingLibrary.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "Materials/MaterialExpressionTextureObjectParameter.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionTransform.h"
#include "Materials/MaterialExpressionConstant3Vector.h"
#include "Materials/MaterialExpressionVertexNormalWS.h"
#include "Materials/MaterialExpressionCameraVectorWS.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Texture2D.h"
#include "Misc/PackageName.h"
#include "UObject/SavePackage.h"
#include "ShaderCompiler.h"
#include "MaterialShared.h"
#include "HAL/IConsoleManager.h"

namespace PadmaClothAuthoring
{
template <class T> T *Node(UMaterial *M, int32 X, int32 Y)
{
    return CastChecked<T>(UMaterialEditingLibrary::CreateMaterialExpression(M, T::StaticClass(), X, Y));
}
UMaterial *BuildMaterial(UPackage *Package, FName Name)
{
    auto *M = NewObject<UMaterial>(Package, Name, RF_Public | RF_Standalone);
    M->SetShadingModel(MSM_DefaultLit);
    M->BlendMode = BLEND_Opaque;
    M->TwoSided = true;
    M->bTangentSpaceNormal = false;
    auto *Core = Node<UMaterialExpressionCustom>(M, 0, 0);
    Core->Description = TEXT("Padma Cloth v1 - artistic DefaultLit hybrid");
    Core->OutputType = CMOT_Float3;
    Core->IncludeFilePaths.Add(TEXT("/Plugin/PadmaNPR/Private/PadmaCloth.ush"));
    for (const auto &Pair : TArray<TPair<FName, ECustomMaterialOutputType>>{{TEXT("Emissive"), CMOT_Float3},
                                                                            {TEXT("NormalWS"), CMOT_Float3},
                                                                            {TEXT("Metallic"), CMOT_Float1},
                                                                            {TEXT("Specular"), CMOT_Float1},
                                                                            {TEXT("Roughness"), CMOT_Float1}})
    {
        FCustomOutput O;
        O.OutputName = Pair.Key;
        O.OutputType = Pair.Value;
        Core->AdditionalOutputs.Add(O);
    }
    auto Input = [&](const TCHAR *Name, UMaterialExpression *E, int32 Index = 0)
    {
        FCustomInput I;
        I.InputName = Name;
        I.Input.Connect(Index, E);
        Core->Inputs.Add(I);
    };
    auto Vector = [&](const TCHAR *Name, FLinearColor Value, const TCHAR *Group, int32 Y)
    {
        auto *P = Node<UMaterialExpressionVectorParameter>(M, -900, Y);
        P->ParameterName = Name;
        P->DefaultValue = Value;
        P->Group = Group;
        Input(Name, P, 5);
        return P;
    };
    auto Scalar = [&](const TCHAR *Name, float Value, int32 Y)
    {
        auto *P = Node<UMaterialExpressionScalarParameter>(M, -600, Y);
        P->ParameterName = Name;
        P->DefaultValue = Value;
        P->Group = TEXT("Binding / Debug");
        Input(Name, P);
        return P;
    };
    auto *UV = Node<UMaterialExpressionTextureCoordinate>(M, -1600, 0);
    auto *Scale = Node<UMaterialExpressionVectorParameter>(M, -1600, 150);
    Scale->ParameterName = TEXT("NPR_UVScale");
    Scale->DefaultValue = FLinearColor(1, 1, 0, 0);
    Scale->Group = TEXT("Binding");
    auto *UVAdapter = Node<UMaterialExpressionCustom>(M, -1400, 200);
    UVAdapter->OutputType = CMOT_Float2;
    UVAdapter->Code = TEXT("return UV * Scale.xy;");
    {
        FCustomInput I;
        I.InputName = TEXT("UV");
        I.Input.Connect(0, UV);
        UVAdapter->Inputs.Add(I);
        I.InputName = TEXT("Scale");
        I.Input.Connect(0, Scale);
        UVAdapter->Inputs.Add(I);
    }
    auto LinearDefault = [&](FName Name, FColor Color)
    {
        auto *T = NewObject<UTexture2D>(Package, Name, RF_Public);
        T->SRGB = false;
        T->CompressionSettings = TC_Masks;
        T->MipGenSettings = TMGS_NoMipmaps;
        T->Source.Init(1, 1, 1, 1, TSF_BGRA8, reinterpret_cast<const uint8 *>(&Color));
        T->PostEditChange();
        return T;
    };
    auto *DefaultPacked = LinearDefault(TEXT("DefaultPackedMask"), FColor(0, 0, 255, 0));
    auto *DefaultMasks = LinearDefault(TEXT("DefaultMatcapMasks"), FColor::White);
    auto *DefaultEncodedNormal = LinearDefault(TEXT("DefaultEncodedNormal"), FColor(128, 128, 0, 255));
    auto Sample = [&](const TCHAR *Name, const TCHAR *Default, EMaterialSamplerType Type, int32 Y)
    {
        auto *T = Node<UMaterialExpressionTextureSampleParameter2D>(M, -1200, Y);
        T->ParameterName = Name;
        T->Group = TEXT("Binding / Textures");
        T->Texture = LoadObject<UTexture2D>(nullptr, Default);
        check(T->Texture);
        T->SamplerType = Type;
        T->Coordinates.Connect(0, UVAdapter);
        return T;
    };
    Input(TEXT("Albedo"),
          Sample(TEXT("NPR_BaseColor"), TEXT("/Engine/EngineResources/WhiteSquareTexture.WhiteSquareTexture"),
                 SAMPLERTYPE_Color, 400));
    auto *Normal = Sample(TEXT("NPR_Normal"), TEXT("/Engine/EngineMaterials/DefaultNormal.DefaultNormal"),
                          SAMPLERTYPE_Normal, 650);
    auto *Encoded =
        Sample(TEXT("NPR_EncodedNormal"), TEXT("/Engine/EngineResources/WhiteSquareTexture.WhiteSquareTexture"),
               SAMPLERTYPE_Masks, 780);
    Encoded->Texture = DefaultEncodedNormal;
    auto *Encoding = Scalar(TEXT("NPR_NormalEncoding"), 0, 750);
    auto *FlipY = Scalar(TEXT("NPR_EncodedNormalFlipY"), 1, 850);
    auto *Decode = Node<UMaterialExpressionCustom>(M, -900, 650);
    Decode->OutputType = CMOT_Float3;
    Decode->IncludeFilePaths.Add(TEXT("/Plugin/PadmaNPR/Private/PadmaCloth.ush"));
    Decode->Code = TEXT("return Encoding>0.5 ? PadmaUnpackNormalRG(Raw.rg,FlipY) : UE_Normal;");
    for (const auto &P : TArray<TPair<FName, UMaterialExpression *>>{
             {TEXT("Encoding"), Encoding}, {TEXT("FlipY"), FlipY}, {TEXT("UE_Normal"), Normal}, {TEXT("Raw"), Encoded}})
    {
        FCustomInput I;
        I.InputName = P.Key;
        I.Input.Connect(0, P.Value);
        Decode->Inputs.Add(I);
    }
    auto *NWorld = Node<UMaterialExpressionTransform>(M, -650, 650);
    NWorld->TransformSourceType = TRANSFORMSOURCE_Tangent;
    NWorld->TransformType = TRANSFORM_World;
    NWorld->Input.Connect(0, Decode);
    Input(TEXT("MappedN"), NWorld);
    auto *Packed = Sample(TEXT("NPR_PackedMask"), TEXT("/Engine/EngineResources/WhiteSquareTexture.WhiteSquareTexture"),
                          SAMPLERTYPE_Masks, 1000);
    Packed->Texture = DefaultPacked;
    Input(TEXT("Packed"), Packed, 5);
    auto *Masks = Sample(TEXT("NPR_MatcapMasks"), TEXT("/Engine/EngineResources/WhiteSquareTexture.WhiteSquareTexture"),
                         SAMPLERTYPE_Masks, 1250);
    Masks->Texture = DefaultMasks;
    Input(TEXT("MatcapMasks"), Masks, 5);
    Input(TEXT("GeometricN"), Node<UMaterialExpressionVertexNormalWS>(M, -650, 1300));
    Input(TEXT("V"), Node<UMaterialExpressionCameraVectorWS>(M, -650, 1450));
    auto *Up = Node<UMaterialExpressionConstant3Vector>(M, -1200, 1450);
    Up->Constant = FLinearColor(0, 1, 0);
    auto *UpWorld = Node<UMaterialExpressionTransform>(M, -900, 1450);
    UpWorld->TransformSourceType = TRANSFORMSOURCE_View;
    UpWorld->TransformType = TRANSFORM_World;
    UpWorld->Input.Connect(0, Up);
    Input(TEXT("CameraUp"), UpWorld);
    Vector(TEXT("NPR_LightDirectionWS"), FLinearColor(0.5f, 0.5f, 1, 0), TEXT("Runtime / Lighting"), 1650);
    Vector(TEXT("NPR_AOChannel"), FLinearColor(0, 0, 1, 0), TEXT("Binding / Channels"), 1800);
    Vector(TEXT("NPR_MetalChannel"), FLinearColor(1, 0, 0, 0), TEXT("Binding / Channels"), 1950);
    Vector(TEXT("NPR_RoughnessChannel"), FLinearColor(0, 0, 0, 1), TEXT("Binding / Channels"), 2100);
    Scalar(TEXT("NPR_IsGlossiness"), 1, 2200);
    Scalar(TEXT("NPR_Debug"), 0, 2300);
    for (int32 Index = 1; Index <= 2; ++Index)
    {
        auto *T = Node<UMaterialExpressionTextureObjectParameter>(M, -1200, 2400 + Index * 170);
        T->ParameterName = FName(*FString::Printf(TEXT("NPR_Matcap0%d"), Index));
        T->Group = TEXT("Binding / Textures");
        T->Texture =
            LoadObject<UTexture2D>(nullptr, TEXT("/Engine/EngineResources/WhiteSquareTexture.WhiteSquareTexture"));
        T->SamplerType = SAMPLERTYPE_Color;
        Input(*FString::Printf(TEXT("Matcap%d"), Index), T);
    }
    TMap<FName, FLinearColor> Parameters;
    FPadmaClothSettings().ToMaterialParameters(Parameters);
    TArray<FName> Keys;
    Parameters.GetKeys(Keys);
    Keys.Sort(FNameLexicalLess());
    int32 Y = 3000;
    for (FName Key : Keys)
    {
        Vector(*Key.ToString(), Parameters[Key], TEXT("Profile / Applied by Cloth MID factory"), Y);
        Y += 160;
    }
    Core->Code = TEXT(R"HLSL(
float3 L=PadmaSafeNormal(NPR_LightDirectionWS.xyz,float3(0,0,1));
float3 VN=PadmaSafeNormal(V,float3(0,0,1));
float3 N=PadmaBuildClothNormal(GeometricN,MappedN,VN,L,NPR_NormalControls.xyz);
float2 uv=PadmaMatcapUV(N,VN,CameraUp);
float3 mc1=Texture2DSample(Matcap1,Matcap1Sampler,uv).rgb;
float3 mc2=Texture2DSample(Matcap2,Matcap2Sampler,uv).rgb;
float roughMask=saturate(dot(Packed,NPR_RoughnessChannel));
roughMask=lerp(roughMask,1.0-roughMask,saturate(NPR_IsGlossiness));
FPadmaClothResult c=PadmaEvaluateCloth(Albedo,GeometricN,MappedN,VN,L,
 saturate(dot(Packed,NPR_AOChannel)),saturate(dot(Packed,NPR_MetalChannel)),roughMask,
 MatcapMasks.r,MatcapMasks.g,mc1,mc2,NPR_Tint,NPR_NormalControls,NPR_Lam,NPR_ShadowColor,
 NPR_AOControls,NPR_RimShadow,NPR_RimShadowColor,NPR_RimHighlight,NPR_RimHighlightColor,
 NPR_Spec,NPR_SpecBaseColor,NPR_SpecMetalColor,NPR_MatcapControls,NPR_MatcapTint1,NPR_MatcapTint2,NPR_Surface,(int)NPR_Debug);
Emissive=c.Emissive;NormalWS=c.NormalWS;Metallic=c.Surface.x;Specular=c.Surface.y;Roughness=c.Surface.z;
return c.BaseColor;
)HLSL");
    Core->RebuildOutputs();
    UMaterialEditingLibrary::ConnectMaterialProperty(Core, TEXT(""), MP_BaseColor);
    UMaterialEditingLibrary::ConnectMaterialProperty(Core, TEXT("Emissive"), MP_EmissiveColor);
    UMaterialEditingLibrary::ConnectMaterialProperty(Core, TEXT("NormalWS"), MP_Normal);
    UMaterialEditingLibrary::ConnectMaterialProperty(Core, TEXT("Metallic"), MP_Metallic);
    UMaterialEditingLibrary::ConnectMaterialProperty(Core, TEXT("Specular"), MP_Specular);
    UMaterialEditingLibrary::ConnectMaterialProperty(Core, TEXT("Roughness"), MP_Roughness);
    M->PostEditChange();
    return M;
}
bool SaveNew(UObject *Asset)
{
    FAssetRegistryModule::AssetCreated(Asset);
    Asset->MarkPackageDirty();
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public | RF_Standalone;
    Args.SaveFlags = SAVE_NoError;
    return UPackage::SavePackage(Asset->GetPackage(), Asset,
                                 *FPackageName::LongPackageNameToFilename(Asset->GetPackage()->GetName(),
                                                                          FPackageName::GetAssetPackageExtension()),
                                 Args);
}
bool CreateTemplates(FString &Message)
{
    const TCHAR *MasterPath = TEXT("/PadmaNPR/Materials/M_NPR_Cloth");
    const TCHAR *MIPath = TEXT("/PadmaNPR/Materials/MI_NPR_Cloth_Template");
    const TCHAR *ProfilePath = TEXT("/PadmaNPR/Profiles/DA_Cloth_Default");
    // Never replace the user's edited template. Incomplete prior sets require explicit manual resolution.
    for (const TCHAR *P : {MasterPath, MIPath, ProfilePath})
    {
        if (FPackageName::DoesPackageExist(P) || FindPackage(nullptr, P))
        {
            Message = TEXT("Cloth template assets already exist; no assets were changed.");
            return false;
        }
    }
    UMaterial *M = BuildMaterial(CreatePackage(MasterPath), TEXT("M_NPR_Cloth"));
    if (GShaderCompilingManager)
        GShaderCompilingManager->FinishAllCompilation();
    const FMaterialResource *Resource = M->GetMaterialResource(GMaxRHIShaderPlatform);
    if (!Resource || Resource->GetCompileErrors().Num() > 0)
    {
        Message = TEXT("Cloth shader compilation failed; templates were not saved. Restart the editor after fixing the "
                       "shader to clear the failed in-memory packages.");
        return false;
    }
    if (!PadmaClothGraph::Encapsulate(M, Message))
        return false;
    FAssetRegistryModule::AssetCreated(M);
    auto *MI = NewObject<UMaterialInstanceConstant>(CreatePackage(MIPath), TEXT("MI_NPR_Cloth_Template"),
                                                    RF_Public | RF_Standalone);
    MI->SetParentEditorOnly(M);
    MI->PostEditChange();
    if (!SaveNew(MI))
    {
        Message = TEXT("Master saved, MI save failed.");
        return false;
    }
    auto *DA =
        NewObject<UPadmaClothProfile>(CreatePackage(ProfilePath), TEXT("DA_Cloth_Default"), RF_Public | RF_Standalone);
    if (!SaveNew(DA))
    {
        Message = TEXT("Materials saved, Profile save failed.");
        return false;
    }
    Message = TEXT(
        "Created /PadmaNPR/Materials and /PadmaNPR/Profiles Cloth templates. No character materials were changed.");
    return true;
}
} // namespace PadmaClothAuthoring
static FAutoConsoleCommand CreateClothCommand(TEXT("PadmaNPR.CreateClothTemplates"),
                                              TEXT("Create new Cloth templates; never overwrite existing assets."),
                                              FConsoleCommandDelegate::CreateLambda(
                                                  []
                                                  {
                                                      FString Message;
                                                      const bool Ok = PadmaClothAuthoring::CreateTemplates(Message);
                                                      UE_LOG(LogTemp, Display, TEXT("Padma Cloth [%s]: %s"),
                                                             Ok ? TEXT("OK") : TEXT("Skipped/Failed"), *Message);
                                                  }));
