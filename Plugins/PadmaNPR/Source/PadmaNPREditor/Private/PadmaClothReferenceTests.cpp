#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "PadmaClothProfile.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialExpressionCustom.h"
#include "MaterialEditingLibrary.h"
#include "MaterialShared.h"
#include "ShaderCompiler.h"
#include "Engine/TextureRenderTarget2D.h"
#include "RenderingThread.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Editor.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaReferenceContractTest,"PadmaNPR.Cloth.ReferenceContract",EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaReferenceContractTest::RunTest(const FString&)
{
    auto* Master=LoadObject<UMaterial>(nullptr,TEXT("/PadmaNPR/Materials/M_NPR_Cloth_Reference_Masked.M_NPR_Cloth_Reference_Masked"));
    if(!TestNotNull(TEXT("Reference master exists"),Master))return false;
    TestEqual(TEXT("Alpha cutout backend"),Master->BlendMode,BLEND_Masked);
    TestTrue(TEXT("Two-sided garment"),Master->TwoSided!=0);
    TestEqual(TEXT("Single public graph entry"),Master->GetExpressionCollection().Expressions.Num(),1);
    auto* Profile=NewObject<UPadmaClothProfile>();FString Error;
    TestNull(TEXT("Legacy profile cannot silently feed reference variant"),UPadmaClothLibrary::CreateClothMID(GetTransientPackage(),Master,Profile,TEXT("Coat"),Error));
    Profile->Defaults.Reference.Enabled=true;
    auto* MID=UPadmaClothLibrary::CreateClothMID(GetTransientPackage(),Master,Profile,TEXT("Coat"),Error);
    if(TestNotNull(TEXT("Reference parameters resolve"),MID))
    {
        FLinearColor Value;
        MID->GetVectorParameterValue(FMaterialParameterInfo(TEXT("NPR_ReferenceControls")),Value);
        TestEqual(TEXT("Color exponent packed"),Value.R,1.3f);
    }
    FPadmaClothSlotOverride Slot;Slot.SlotName=TEXT("Coat");Slot.bOverrideReference=true;
    Slot.Reference=Profile->Defaults.Reference;Slot.Reference.NativeRoughness=0.7f;Profile->SlotOverrides.Add(Slot);
    FPadmaClothSettings Resolved;
    TestTrue(TEXT("Reference slot override resolves"),Profile->Resolve(TEXT("Coat"),Resolved,Error));
    TestEqual(TEXT("Reference slot isolation"),Resolved.Reference.NativeRoughness,0.7f);
    Profile->Resolve(TEXT("Other"),Resolved,Error);
    TestEqual(TEXT("Default stays unchanged"),Resolved.Reference.NativeRoughness,1.f);
    Profile->Defaults.Reference.RoughnessScale=0;
    TestFalse(TEXT("Invalid reference roughness rejected"),Profile->Resolve(TEXT("Other"),Resolved,Error));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaReferenceGPUTest,"PadmaNPR.Cloth.ReferenceGPU",EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaReferenceGPUTest::RunTest(const FString&)
{
    auto* M=NewObject<UMaterial>(GetTransientPackage());M->SetShadingModel(MSM_Unlit);
    auto* C=CastChecked<UMaterialExpressionCustom>(UMaterialEditingLibrary::CreateMaterialExpression(M,UMaterialExpressionCustom::StaticClass()));
    C->OutputType=CMOT_Float3;C->IncludeFilePaths.Add(TEXT("/Plugin/PadmaNPR/Private/PadmaClothReference.ush"));
    C->Code=TEXT(R"HLSL(
float2 uv=PadmaReferenceMatcapUV(float3(1,0,0),float3(0,-1,0),float4(1,.194203,.735,.5));
float rough=PadmaReferenceSurfaceRoughness(.5,float4(0,1,.8,.3));
float3 blend=PadmaReferenceBlend(float3(.2,.3,.4),float4(2,2,2,0),1);
float highlight=PadmaReferenceHighlight(float3(0,0,1),float3(0,0,1),float3(0,0,1),.5,1,1,1,float4(1,1,.57,0),float2(.272,.085334));
float blocked=PadmaReferenceHighlight(float3(0,0,1),float3(0,0,1),float3(0,0,1),.5,0,1,1,float4(1,1,.57,0),float2(.272,.085334));
float2 pole=PadmaReferenceMatcapUV(float3(0,0,1),float3(0,0,1),float4(1,1,0,0));
return float3(all(abs(uv-float2(-1.735,-.5))<1e-4)?1:0,
 abs(rough-.27)<1e-4 && all(abs(blend-float3(.4,.6,.8))<1e-4)?1:0,
 highlight>.99 && blocked==0 && all(abs(pole)<1e-4)?1:0);
)HLSL");
    UMaterialEditingLibrary::ConnectMaterialProperty(C,TEXT(""),MP_EmissiveColor);M->PostEditChange();M->EnsureIsComplete();GShaderCompilingManager->FinishAllCompilation();
    auto* Resource=M->GetMaterialResource(GMaxRHIShaderPlatform);
    if(!TestTrue(TEXT("Reference shader compiles"),Resource&&Resource->GetCompileErrors().IsEmpty()&&Resource->GetGameThreadShaderMap()))return false;
    auto* RT=NewObject<UTextureRenderTarget2D>();RT->RenderTargetFormat=RTF_RGBA16f;RT->InitAutoFormat(16,16);RT->UpdateResourceImmediate(true);
    UKismetRenderingLibrary::DrawMaterialToRenderTarget(GEditor->GetEditorWorldContext().World(),RT,M);FlushRenderingCommands();
    TArray<FLinearColor> Pixels;
    if(TestTrue(TEXT("Reference GPU readback"),RT->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(Pixels))&&!Pixels.IsEmpty())
    {
        AddInfo(FString::Printf(TEXT("Reference RGB %.6f %.6f %.6f"),Pixels[0].R,Pixels[0].G,Pixels[0].B));
        TestTrue(TEXT("UV, blend, roughness and highlight fixtures"),Pixels[0].R>.99&&Pixels[0].G>.99&&Pixels[0].B>.99);
    }
    return true;
}
#endif
