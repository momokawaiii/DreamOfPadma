#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "PadmaClothProfile.h"
#include "PadmaClothGraph.h"
#include "Materials/MaterialExpressionMaterialFunctionCall.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialExpressionCustom.h"
#include "MaterialEditingLibrary.h"
#include "MaterialShared.h"
#include "ShaderCompiler.h"
#include "Engine/TextureRenderTarget2D.h"
#include "CanvasTypes.h"
#include "CanvasItem.h"
#include "RenderingThread.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Editor.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaClothProfileTest, "PadmaNPR.Cloth.ProfileOwnership",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPadmaClothProfileTest::RunTest(const FString &)
{
    auto *Profile = NewObject<UPadmaClothProfile>();
    Profile->Defaults.Matcap.Strength01 = 0.2f;
    FPadmaClothSlotOverride Coat;
    Coat.SlotName = TEXT("Coat");
    Coat.bOverrideMatcap = true;
    Coat.Matcap.Strength01 = 0.8f;
    Coat.Specular.BaseRoughness = 0.9f; // disabled group must not leak into the resolved profile.
    Profile->SlotOverrides.Add(Coat);
    FString Error;
    FPadmaClothSettings Resolved;
    TestTrue(TEXT("Coat resolves"), Profile->Resolve(TEXT("Coat"), Resolved, Error));
    TestEqual(TEXT("Enabled group overrides"), Resolved.Matcap.Strength01, 0.8f);
    TestEqual(TEXT("Disabled group inherits"), Resolved.Specular.BaseRoughness, 0.5f);
    TestTrue(TEXT("Other slots resolve"), Profile->Resolve(TEXT("Stocking"), Resolved, Error));
    TestEqual(TEXT("Other slot stays default"), Resolved.Matcap.Strength01, 0.2f);
    Profile->SlotOverrides.Add(Coat);
    TestFalse(TEXT("Duplicate slot rejected"), Profile->Resolve(TEXT("Coat"), Resolved, Error));
    Profile->SlotOverrides.Pop();
    Profile->Defaults.Diffuse.Feather = 0;
    TestFalse(TEXT("Zero denominator rejected"), Profile->Resolve(TEXT("Coat"), Resolved, Error));
    Profile->Defaults.Diffuse.Feather = 1;
    auto *Parent = LoadObject<UMaterialInterface>(
        nullptr, TEXT("/PadmaNPR/Materials/MI_NPR_Cloth_Template.MI_NPR_Cloth_Template"));
    if (!TestNotNull(TEXT("Authored Cloth template exists"), Parent))
        return false;
    auto *A = UPadmaClothLibrary::CreateClothMID(GetTransientPackage(), Parent, Profile, TEXT("Coat"), Error);
    auto *B = UPadmaClothLibrary::CreateClothMID(GetTransientPackage(), Parent, Profile, TEXT("Stocking"), Error);
    if (TestNotNull(TEXT("Coat MID"), A) && TestNotNull(TEXT("Stocking MID"), B))
    {
        TestTrue(TEXT("MIDs are independently owned"), A != B);
        FLinearColor AV, BV, PV;
        A->GetVectorParameterValue(FMaterialParameterInfo(TEXT("NPR_MatcapControls")), AV);
        B->GetVectorParameterValue(FMaterialParameterInfo(TEXT("NPR_MatcapControls")), BV);
        Parent->GetVectorParameterValue(FMaterialParameterInfo(TEXT("NPR_MatcapControls")), PV);
        TestEqual(TEXT("Coat applied"), AV.R, 0.8f);
        TestEqual(TEXT("Other slot applied"), BV.R, 0.2f);
        TestEqual(TEXT("Parent remains unchanged"), PV.R, 0.f);
    }
    auto *Wrong = UMaterial::GetDefaultMaterial(MD_Surface);
    TestNull(TEXT("Incompatible parent rejected"),
             UPadmaClothLibrary::CreateClothMID(GetTransientPackage(), Wrong, Profile, TEXT("Coat"), Error));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaClothGPUTest, "PadmaNPR.Cloth.GPUMath",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPadmaClothGPUTest::RunTest(const FString &)
{
    auto *M = NewObject<UMaterial>(GetTransientPackage());
    M->SetShadingModel(MSM_Unlit);
    auto *C = CastChecked<UMaterialExpressionCustom>(
        UMaterialEditingLibrary::CreateMaterialExpression(M, UMaterialExpressionCustom::StaticClass()));
    C->OutputType = CMOT_Float3;
    C->IncludeFilePaths.Add(TEXT("/Plugin/PadmaNPR/Private/PadmaCloth.ush"));
    C->Code = TEXT(R"HLSL(
float p04=PadmaGGXNDF(1,0.04),p05=PadmaGGXNDF(1,0.05),p10=PadmaGGXNDF(1,0.1);
float back=PadmaReferenceGGXNDF(float3(0,0,1),float3(0,0,-1),0.1);
float nearBack=PadmaReferenceGGXNDF(float3(0,0,1),normalize(float3(0.0001,0,-1)),0.1);
bool ndf=p04>p05 && p05>p10 && abs(p04/124339.8-1)<0.001 && back<0.001 && nearBack<0.001;
bool masks=abs(PadmaMaskRemap(0.4,0.5,0.1,1)-0.7)<0.0001 &&
 abs(PadmaMaskAdjust(0.4,0.5,0.1)-0.1)<0.0001 && PadmaMaskRemap(0,0.5,0,0)==1;
float3 blend=PadmaColorBlend(float3(0.2,0.4,0.6),float4(0.5,0.5,0.5,0),1);
float3 decoded=PadmaUnpackNormalRG(float2(0.5,0.5),1);
float2 uv=PadmaMatcapUV(float3(0,0,1),float3(0,0,1),float3(0,0,1));
bool others=all(abs(blend-float3(0.1,0.2,0.3))<0.0001) && decoded.z>0.999 && all(abs(uv-0.5)<0.0001);
return float3(ndf?1:0,masks?1:0,others?1:0);
)HLSL");
    UMaterialEditingLibrary::ConnectMaterialProperty(C, TEXT(""), MP_EmissiveColor);
    M->PostEditChange();
    GShaderCompilingManager->FinishAllCompilation();
    auto *Resource = M->GetMaterialResource(GMaxRHIShaderPlatform);
    if (!TestTrue(TEXT("GPU fixture compiled"),
                  Resource && Resource->GetCompileErrors().IsEmpty() && Resource->GetGameThreadShaderMap()))
        return false;
    auto *RT = NewObject<UTextureRenderTarget2D>();
    RT->RenderTargetFormat = RTF_RGBA16f;
    RT->InitAutoFormat(16, 16);
    RT->UpdateResourceImmediate(true);
    auto *Target = RT->GameThread_GetRenderTargetResource();
    UKismetRenderingLibrary::DrawMaterialToRenderTarget(GEditor->GetEditorWorldContext().World(), RT, M);
    FlushRenderingCommands();
    TArray<FLinearColor> Pixels;
    if (TestTrue(TEXT("Read back actual GPU result"), Target->ReadLinearColorPixels(Pixels)) && Pixels.Num() > 0)
    {
        AddInfo(FString::Printf(TEXT("GPU fixture RGB = %.6f %.6f %.6f"), Pixels[0].R, Pixels[0].G, Pixels[0].B));
        TestTrue(TEXT("GGX boundaries on GPU"), Pixels[0].R > 0.99f);
        TestTrue(TEXT("Both source remap conventions on GPU"), Pixels[0].G > 0.99f);
        TestTrue(TEXT("Blend / normal / degenerate Matcap basis on GPU"), Pixels[0].B > 0.99f);
    }
    return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaClothGraphTest, "PadmaNPR.Cloth.GraphContract",
                                EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPadmaClothGraphTest::RunTest(const FString &)
{
    auto* Master = LoadObject<UMaterial>(nullptr, TEXT("/PadmaNPR/Materials/M_NPR_Cloth.M_NPR_Cloth"));
    if (!TestNotNull(TEXT("Cloth master loads"), Master)) return false;
    TestTrue(TEXT("Master uses attributes"), Master->bUseMaterialAttributes != 0);
    TestEqual(TEXT("One public function entry"), Master->GetExpressionCollection().Expressions.Num(), 1);
    TestNotNull(TEXT("Output is a function"), Cast<UMaterialExpressionMaterialFunctionCall>(Master->GetEditorOnlyData()->MaterialAttributes.Expression));
    TMap<FName, FLinearColor> Expected;
    FPadmaClothSettings().ToMaterialParameters(Expected);
    for (const auto& Pair : Expected)
    {
        FLinearColor Value;
        TestTrue(*Pair.Key.ToString(), Master->GetVectorParameterValue(FMaterialParameterInfo(Pair.Key), Value));
        TestEqual(*Pair.Key.ToString(), Value, Pair.Value);
    }
    TArray<FMaterialParameterInfo> Infos;
    TArray<FGuid> IDs;
    Master->GetAllTextureParameterInfo(Infos, IDs);
    for (const TCHAR* Name : {TEXT("NPR_BaseColor"), TEXT("NPR_Normal"), TEXT("NPR_EncodedNormal"), TEXT("NPR_PackedMask"), TEXT("NPR_MatcapMasks"), TEXT("NPR_Matcap01"), TEXT("NPR_Matcap02")})
        TestTrue(Name, Infos.ContainsByPredicate([&](const FMaterialParameterInfo& I) { return I.Name == Name; }));
    Master->EnsureIsComplete();
    GShaderCompilingManager->FinishAllCompilation();
    auto* Resource = Master->GetMaterialResource(GMaxRHIShaderPlatform);
    TestTrue(TEXT("Nested functions compile"), Resource && Resource->GetCompileErrors().IsEmpty() && Resource->GetGameThreadShaderMap());

    auto* Edited = NewObject<UMaterial>(GetTransientPackage());
    auto* C = CastChecked<UMaterialExpressionCustom>(UMaterialEditingLibrary::CreateMaterialExpression(Edited, UMaterialExpressionCustom::StaticClass()));
    C->Description = TEXT("Padma Cloth v1 - artistic DefaultLit hybrid");
    auto* D = Edited->GetEditorOnlyData();
    D->BaseColor.Expression = C; D->BaseColor.OutputIndex = 0;
    D->EmissiveColor.Expression = C; D->EmissiveColor.OutputIndex = 1;
    D->Normal.Expression = C; D->Normal.OutputIndex = 2;
    D->Metallic.Expression = C; D->Metallic.OutputIndex = 3;
    D->Specular.Expression = C; D->Specular.OutputIndex = 4;
    D->Roughness.Expression = C; D->Roughness.OutputIndex = 5;
    D->PixelDepthOffset.Expression = C;
    FString Message;
    TestFalse(TEXT("Extra output rejected"), PadmaClothGraph::Encapsulate(Edited, Message));
    TestTrue(TEXT("Specific extra-output guard"), Message.Contains(TEXT("extra material output")));
    TestTrue(TEXT("User output preserved"), D->PixelDepthOffset.Expression == C);
    TestFalse(TEXT("User graph not converted"), Edited->bUseMaterialAttributes != 0);
    return true;
}
#endif
