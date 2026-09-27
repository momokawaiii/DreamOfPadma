#include "PadmaClothProfile.h"
#include "MaterialEditingLibrary.h"
#include "Materials/Material.h"
#include "Materials/MaterialFunction.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "Materials/MaterialExpressionFunctionOutput.h"
#include "Materials/MaterialExpressionMaterialFunctionCall.h"
#include "Materials/MaterialExpressionMakeMaterialAttributes.h"
#include "Materials/MaterialExpressionSkyAtmosphereLightDirection.h"
#include "Materials/MaterialExpressionEyeAdaptationInverse.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "MaterialShared.h"
#include "ShaderCompiler.h"
#include "Misc/PackageName.h"
#include "UObject/SavePackage.h"
#include "HAL/IConsoleManager.h"

namespace PadmaClothReferenceAuthoring
{
namespace
{
template<class T> T* Node(UMaterialFunction* F, int32 X, int32 Y)
{
    return CastChecked<T>(UMaterialEditingLibrary::CreateMaterialExpressionInFunction(F, T::StaticClass(), X, Y));
}
void Port(UMaterialFunction* F, FName Name, UMaterialExpression* E, int32 Index, int32 Order)
{
    auto* O=Node<UMaterialExpressionFunctionOutput>(F, 500, Order*100);
    O->OutputName=Name; O->SortPriority=Order; O->A.Connect(Index,E);
}
UMaterialFunction* Clone(const TCHAR* Source, const TCHAR* Name)
{
    auto* Original=LoadObject<UMaterialFunction>(nullptr,Source);
    if(!Original)return nullptr;
    FString Path=FString(TEXT("/PadmaNPR/Functions/"))+Name;
    return DuplicateObject<UMaterialFunction>(Original,CreatePackage(*Path),Name);
}
bool Save(UObject* A)
{
    A->SetFlags(RF_Public|RF_Standalone);FAssetRegistryModule::AssetCreated(A);A->MarkPackageDirty();
    FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;Args.SaveFlags=SAVE_NoError;
    return UPackage::SavePackage(A->GetPackage(),A,*FPackageName::LongPackageNameToFilename(A->GetPackage()->GetName(),FPackageName::GetAssetPackageExtension()),Args);
}
void Update(UMaterialFunction* F)
{
    F->PostEditChange(); F->UpdateDependentFunctionCandidates(); UMaterialEditingLibrary::UpdateMaterialFunction(F);
}
}
bool Create(FString& Error)
{
    for(const TCHAR* Path:{TEXT("/PadmaNPR/Functions/MF_PadmaClothReferenceBindings"),TEXT("/PadmaNPR/Functions/MF_PadmaClothReferenceParameters"),TEXT("/PadmaNPR/Functions/MF_PadmaClothReference"),TEXT("/PadmaNPR/Materials/M_NPR_Cloth_Reference_Masked")})
        if(FPackageName::DoesPackageExist(Path)||FindPackage(nullptr,Path)){Error=TEXT("Reference targets exist; no overwrite.");return false;}
    auto* Bindings=Clone(TEXT("/PadmaNPR/Functions/MF_PadmaClothBindings.MF_PadmaClothBindings"),TEXT("MF_PadmaClothReferenceBindings"));
    auto* Params=Clone(TEXT("/PadmaNPR/Functions/MF_PadmaClothParameters.MF_PadmaClothParameters"),TEXT("MF_PadmaClothReferenceParameters"));
    if(!Bindings||!Params){Error=TEXT("Create legacy templates first.");return false;}
    UMaterialExpressionTextureSampleParameter2D* Albedo=nullptr;
    for(UMaterialExpression* E:Bindings->GetExpressionCollection().Expressions)
        if(auto* T=Cast<UMaterialExpressionTextureSampleParameter2D>(E);T&&T->ParameterName==TEXT("NPR_BaseColor"))Albedo=T;
    if(!Albedo){Error=TEXT("Missing base texture binding.");return false;}
    Port(Bindings,TEXT("Alpha"),Albedo,4,100);
    auto* Light=Node<UMaterialExpressionSkyAtmosphereLightDirection>(Bindings,-500,2200);
    Light->LightIndex=0;
    Port(Bindings,TEXT("SceneLight"),Light,0,101);
    FPadmaClothSettings Settings;Settings.Reference.Enabled=true;
    TMap<FName,FLinearColor> Values;Settings.ToMaterialParameters(Values);
    TMap<FName,FLinearColor> OldValues;FPadmaClothSettings().ToMaterialParameters(OldValues);
    TArray<FName> Keys;Values.GetKeys(Keys);Keys.Sort(FNameLexicalLess());int32 Order=100;
    for(FName Key:Keys)if(!OldValues.Contains(Key))
    {
        auto* P=Node<UMaterialExpressionVectorParameter>(Params,-200,Order*70);
        P->ParameterName=Key;P->DefaultValue=Values[Key];P->Group=TEXT("Profile / Reference response");
        Port(Params,Key,P,5,Order++);
    }
    UMaterialEditingLibrary::LayoutMaterialFunctionExpressions(Bindings);
    UMaterialEditingLibrary::LayoutMaterialFunctionExpressions(Params);Update(Bindings);Update(Params);
    auto* F=NewObject<UMaterialFunction>(CreatePackage(TEXT("/PadmaNPR/Functions/MF_PadmaClothReference")),TEXT("MF_PadmaClothReference"),RF_Public|RF_Standalone);
    F->Description=TEXT("Reference Cloth: separate branch normals, source remaps/Matcap, native surface and compensated outputs.");
    auto* B=Node<UMaterialExpressionMaterialFunctionCall>(F,-1200,0);B->SetMaterialFunction(Bindings);
    auto* P=Node<UMaterialExpressionMaterialFunctionCall>(F,-800,1200);P->SetMaterialFunction(Params);
    auto* C=Node<UMaterialExpressionCustom>(F,-150,0);C->Description=TEXT("Padma reference Cloth / HLSL");C->OutputType=CMOT_Float3;
    C->IncludeFilePaths.Add(TEXT("/Plugin/PadmaNPR/Private/PadmaClothReference.ush"));
    for(auto* Call:{B,P})for(int32 I=0;I<Call->FunctionOutputs.Num();++I)
    {
        const FName Name=Call->FunctionOutputs[I].ExpressionOutput->OutputName;
        // User-added display ports (for example "Output OpacityMask") are not HLSL identifiers.
        const FString Identifier=Name.ToString();
        bool Valid=!Identifier.IsEmpty();for(TCHAR Ch:Identifier)Valid &= FChar::IsAlnum(Ch)||Ch==TEXT('_');
        if(Valid)
        {FCustomInput Input;Input.InputName=Name;Input.Input.Connect(I,Call);C->Inputs.Add(Input);}
    }
    for(const auto& Pair:TArray<TPair<FName,ECustomMaterialOutputType>>{{TEXT("Emissive"),CMOT_Float3},{TEXT("NormalWS"),CMOT_Float3},{TEXT("Metallic"),CMOT_Float1},{TEXT("Specular"),CMOT_Float1},{TEXT("Roughness"),CMOT_Float1},{TEXT("Opacity"),CMOT_Float1},{TEXT("ExposureWeight"),CMOT_Float1}})
    {FCustomOutput O;O.OutputName=Pair.Key;O.OutputType=Pair.Value;C->AdditionalOutputs.Add(O);}
    C->Code=TEXT(R"HLSL(
float3 Vn=PadmaSafeNormal(V,float3(0,0,1));
float3 L=PadmaSafeNormal(SceneLight,PadmaSafeNormal(NPR_LightDirectionWS.xyz,float3(0,0,1)));
float3 N=PadmaBuildClothNormal(GeometricN,MappedN,Vn,L,NPR_NormalControls.xyz);
float3 McN=PadmaReferenceBranchNormal(GeometricN,N,NPR_BranchNormals.w);
float2 uv=PadmaReferenceMatcapUV(McN,Vn,NPR_MatcapUV);
float3 mc1=Texture2DSample(Matcap1,Matcap1Sampler,uv).rgb;
float3 mc2=Texture2DSample(Matcap2,Matcap2Sampler,uv).rgb;
float roughMask=saturate(dot(Packed,NPR_RoughnessChannel));
roughMask=lerp(roughMask,1-roughMask,saturate(NPR_IsGlossiness));
FPadmaClothResult R=PadmaEvaluateReferenceCloth(Albedo,GeometricN,N,Vn,L,
 saturate(dot(Packed,NPR_AOChannel)),saturate(dot(Packed,NPR_MetalChannel)),roughMask,
 MatcapMasks.r,MatcapMasks.g,mc1,mc2,NPR_Tint,NPR_Lam,NPR_ShadowColor,NPR_AOControls,
 NPR_RimShadow,NPR_RimShadowColor,NPR_RimHighlight,NPR_RimHighlightColor,NPR_Spec,NPR_SpecBaseColor,
 NPR_MatcapControls,NPR_MatcapTint1,NPR_MatcapTint2,NPR_Surface,
 NPR_ReferenceControls,NPR_BranchNormals,NPR_ReferenceGGX,NPR_ReferenceSurface,NPR_ReferenceAOColor,NPR_ToneControls,(int)NPR_Debug);
Emissive=R.Emissive;NormalWS=R.NormalWS;Metallic=R.Surface.x;Specular=R.Surface.y;Roughness=R.Surface.z;
Opacity=Alpha;ExposureWeight=NPR_ReferenceControls.y;
return R.BaseColor;
)HLSL");
    C->RebuildOutputs();
    auto* Base=Node<UMaterialExpressionEyeAdaptationInverse>(F,400,0);Base->LightValueInput.Connect(0,C);Base->AlphaInput.Connect(7,C);
    auto* Emissive=Node<UMaterialExpressionEyeAdaptationInverse>(F,400,200);Emissive->LightValueInput.Connect(1,C);Emissive->AlphaInput.Connect(7,C);
    auto* Attr=Node<UMaterialExpressionMakeMaterialAttributes>(F,750,0);
    Attr->BaseColor.Connect(0,Base);Attr->EmissiveColor.Connect(0,Emissive);Attr->Normal.Connect(2,C);
    Attr->Metallic.Connect(3,C);Attr->Specular.Connect(4,C);Attr->Roughness.Connect(5,C);Attr->OpacityMask.Connect(6,C);
    Port(F,TEXT("Cloth"),Attr,0,0);Update(F);
    auto* M=NewObject<UMaterial>(CreatePackage(TEXT("/PadmaNPR/Materials/M_NPR_Cloth_Reference_Masked")),TEXT("M_NPR_Cloth_Reference_Masked"),RF_Public|RF_Standalone);
    M->SetShadingModel(MSM_DefaultLit);M->BlendMode=BLEND_Masked;M->OpacityMaskClipValue=0.177f;M->TwoSided=true;M->bTangentSpaceNormal=false;M->bUseMaterialAttributes=true;
    auto* Call=CastChecked<UMaterialExpressionMaterialFunctionCall>(UMaterialEditingLibrary::CreateMaterialExpression(M,UMaterialExpressionMaterialFunctionCall::StaticClass(),-350,0));
    Call->SetMaterialFunction(F);M->GetEditorOnlyData()->MaterialAttributes.Connect(0,Call);M->UpdateCachedExpressionData();M->PostEditChange();M->EnsureIsComplete();GShaderCompilingManager->FinishAllCompilation();
    auto* Resource=M->GetMaterialResource(GMaxRHIShaderPlatform);
    if(!Resource||!Resource->GetCompileErrors().IsEmpty()||!Resource->GetGameThreadShaderMap()){Error=TEXT("Reference shader compilation failed; restart after fixing.");return false;}
    for(UObject* A:TArray<UObject*>{Bindings,Params,F,M})if(!Save(A)){Error=TEXT("Partial save; inspect generated assets before retry.");return false;}
    Error=TEXT("Created masked reference Cloth variant. No project character assignments changed.");return true;
}
}
static FAutoConsoleCommand CreateReference(TEXT("PadmaNPR.CreateReferenceCloth"),TEXT("Create the reference Cloth variant without replacing legacy assets."),FConsoleCommandDelegate::CreateLambda([]
{
    FString Message;bool OK=PadmaClothReferenceAuthoring::Create(Message);
    UE_LOG(LogTemp,Display,TEXT("Padma Reference Cloth [%s]: %s"),OK?TEXT("OK"):TEXT("FAILED"),*Message);
}));
