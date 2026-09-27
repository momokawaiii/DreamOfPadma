#include "PadmaClothGraph.h"
#include "PadmaClothProfile.h"
#include "MaterialEditingLibrary.h"
#include "Materials/Material.h"
#include "Materials/MaterialFunction.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Materials/MaterialExpressionTextureBase.h"
#include "Materials/MaterialExpressionFunctionOutput.h"
#include "Materials/MaterialExpressionMaterialFunctionCall.h"
#include "Materials/MaterialExpressionMakeMaterialAttributes.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Texture.h"
#include "MaterialShared.h"
#include "ShaderCompiler.h"
#include "Misc/PackageName.h"
#include "UObject/SavePackage.h"
#include "HAL/IConsoleManager.h"

namespace PadmaClothGraph
{
namespace
{
template <class T> T *Add(UMaterialFunction *F, int32 X, int32 Y)
{
    return CastChecked<T>(UMaterialEditingLibrary::CreateMaterialExpressionInFunction(F, T::StaticClass(), X, Y));
}
UMaterialFunction *Function(const TCHAR *Name, const TCHAR *Description)
{
    const FString Path = FString(TEXT("/PadmaNPR/Functions/")) + Name;
    auto *F = NewObject<UMaterialFunction>(CreatePackage(*Path), Name, RF_Public | RF_Standalone);
    F->Description = Description;
    F->bExposeToLibrary = true;
    return F;
}
UMaterialExpressionFunctionOutput *Output(UMaterialFunction *F, FName Name, UMaterialExpression *Expression,
                                          int32 Index, int32 Order)
{
    auto *O = Add<UMaterialExpressionFunctionOutput>(F, 400, Order * 100);
    O->OutputName = Name;
    O->SortPriority = Order;
    O->A.Connect(Index, Expression);
    return O;
}
int32 OutputIndex(UMaterialExpressionMaterialFunctionCall *Call, FName Name)
{
    return Call->FunctionOutputs.IndexOfByPredicate(
        [&](const FFunctionExpressionOutput &O)
        { return O.ExpressionOutput && O.ExpressionOutput->OutputName == Name; });
}
bool Compile(UMaterial *M, FString &Message)
{
    M->UpdateCachedExpressionData();
    M->PostEditChange();
    M->EnsureIsComplete();
    GShaderCompilingManager->FinishAllCompilation();
    auto *R = M->GetMaterialResource(GMaxRHIShaderPlatform);
    if (!R || !R->GetGameThreadShaderMap() || !R->GetCompileErrors().IsEmpty())
    {
        Message = TEXT("Encapsulated material failed shader compilation. Restart before retrying newly created "
                       "function packages.");
        return false;
    }
    return true;
}
bool Save(UObject *Asset, bool bNew)
{
    if (bNew)
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
void ConnectMaster(UMaterial *M, UMaterialFunction *F)
{
    auto *Call = CastChecked<UMaterialExpressionMaterialFunctionCall>(UMaterialEditingLibrary::CreateMaterialExpression(
        M, UMaterialExpressionMaterialFunctionCall::StaticClass(), -320, 0));
    Call->SetMaterialFunction(F);
    M->bUseMaterialAttributes = true;
    M->GetEditorOnlyData()->MaterialAttributes.Connect(0, Call);
}
} // namespace

bool Encapsulate(UMaterial *M, FString &Message)
{
    if (!M)
    {
        Message = TEXT("Create/load the Cloth master first.");
        return false;
    }
    if (M->bUseMaterialAttributes)
    {
        Message = TEXT("Material already uses Material Attributes; nothing was changed.");
        return true;
    }
    auto Old = M->GetExpressionCollection().Expressions;
    UMaterialExpressionCustom *Core = nullptr;
    for (UMaterialExpression *E : Old)
        if (auto *C = Cast<UMaterialExpressionCustom>(E))
            if (C->Description == TEXT("Padma Cloth v1 - artistic DefaultLit hybrid"))
                Core = C;
    auto *Data = M->GetEditorOnlyData();
    // Do not silently discard a user-authored output, displacement or cutout branch.
    if (!Core || Data->BaseColor.Expression != Core || Data->EmissiveColor.Expression != Core ||
        Data->Normal.Expression != Core || Data->Metallic.Expression != Core || Data->Specular.Expression != Core ||
        Data->Roughness.Expression != Core || Data->BaseColor.OutputIndex != 0 ||
        Data->EmissiveColor.OutputIndex != 1 || Data->Normal.OutputIndex != 2 || Data->Metallic.OutputIndex != 3 ||
        Data->Specular.OutputIndex != 4 || Data->Roughness.OutputIndex != 5 || Data->OpacityMask.Expression ||
        Data->Opacity.Expression || Data->WorldPositionOffset.Expression || Data->AmbientOcclusion.Expression)
    {
        Message =
            TEXT("Unsupported edited graph: output wiring differs from the Cloth template. No assets were changed.");
        return false;
    }
    // Reject every extra connected output, including customized UVs and depth offset.
    for (int32 Property = 0; Property < MP_MAX; ++Property)
    {
        const auto P = static_cast<EMaterialProperty>(Property);
        if (P == MP_BaseColor || P == MP_EmissiveColor || P == MP_Normal || P == MP_Metallic || P == MP_Specular ||
            P == MP_Roughness)
            continue;
        const FExpressionInput *Input = M->GetExpressionInputForProperty(P);
        if (Input && Input->Expression)
        {
            Message = TEXT("Unsupported extra material output; source untouched.");
            return false;
        }
    }
    for (const TCHAR *Name : {TEXT("MF_PadmaClothBindings"), TEXT("MF_PadmaClothParameters"), TEXT("MF_PadmaCloth")})
    {
        const FString Path = FString(TEXT("/PadmaNPR/Functions/")) + Name;
        if (FPackageName::DoesPackageExist(Path) || FindPackage(nullptr, *Path))
        {
            Message = TEXT("Function targets already exist. No overwrite performed; inspect partial previous generation before retrying.");
            return false;
        }
    }
    TMap<FName, FLinearColor> ProfileNames;
    FPadmaClothSettings().ToMaterialParameters(ProfileNames);
    auto *Bindings = Function(
        TEXT("MF_PadmaClothBindings"),
        TEXT("Texture bindings, UVs, normal decode and world-space inputs. Edit textures/channels in the MI."));
    auto *Parameters = Function(
        TEXT("MF_PadmaClothParameters"),
        TEXT("Profile-owned artistic parameters. Runtime MID factory supplies these values; names remain stable."));
    auto *Surface = Function(TEXT("MF_PadmaCloth"),
                             TEXT("Cloth composition: bindings + profile -> PadmaCloth.ush -> Material Attributes."));
    TMap<UMaterialExpression *, UMaterialExpression *> Copies;
    TMap<UMaterialExpression *, bool> IsProfile;
    TArray<UObject *> NewTextures;
    TMap<UTexture *, UTexture *> TextureCopies;
    for (UMaterialExpression *E : Old)
    {
        if (E == Core)
            continue;
        const auto *V = Cast<UMaterialExpressionVectorParameter>(E);
        bool bProfile = V && ProfileNames.Contains(V->ParameterName);
        auto *F = bProfile ? Parameters : Bindings;
        auto *Copy = UMaterialEditingLibrary::DuplicateMaterialExpression(nullptr, F, E);
        Copy->Material = nullptr;
        Copy->Function = F;
        Copies.Add(E, Copy);
        IsProfile.Add(E, bProfile);
        if (auto *T = Cast<UMaterialExpressionTextureBase>(Copy))
        {
            if (T->Texture && T->Texture->GetPackage() == M->GetPackage())
            {
                if (!TextureCopies.Contains(T->Texture))
                {
                    const FString Name = TEXT("T_Cloth_") + T->Texture->GetName();
                    const FString Path = TEXT("/PadmaNPR/Textures/") + Name;
                    if (FPackageName::DoesPackageExist(Path) || FindPackage(nullptr, *Path))
                    {
                        Message = TEXT("Default texture target already exists; source untouched. Restart before "
                                       "resolving partial function generation.");
                        return false;
                    }
                    auto *CopyTexture = DuplicateObject<UTexture>(T->Texture, CreatePackage(*Path), FName(*Name));
                    CopyTexture->SetFlags(RF_Public | RF_Standalone);
                    TextureCopies.Add(T->Texture, CopyTexture);
                    NewTextures.Add(CopyTexture);
                }
                T->Texture = TextureCopies[T->Texture];
            }
        }
    }
    // Rewire every copied edge within its own function, preserving original output indices and masks.
    for (const auto &Pair : Copies)
    {
        for (int32 I = 0; FExpressionInput *Original = Pair.Key->GetInput(I); ++I)
        {
            FExpressionInput *CopyInput = Pair.Value->GetInput(I);
            *CopyInput = *Original;
            auto *Upstream = Original->Expression;
            if (!Upstream)
                continue;
            if (!Copies.Contains(Upstream) || IsProfile[Upstream] != IsProfile[Pair.Key])
            {
                Message = TEXT(
                    "Unexpected cross-group edge; source untouched. Restart to discard unsaved generated functions.");
                return false;
            }
            CopyInput->Expression = Copies[Upstream];
        }
    }
    int32 BindOrder = 0, ParamOrder = 0;
    for (const auto &I : Core->Inputs)
    {
        if (I.InputName.IsNone() || !I.Input.Expression)
            continue;
        auto *E = I.Input.Expression;
        if (!Copies.Contains(E))
        {
            Message = TEXT("Unsupported Custom input.");
            return false;
        }
        const bool bProfile = IsProfile[E];
        auto *Port = Output(bProfile ? Parameters : Bindings, I.InputName, Copies[E], I.Input.OutputIndex,
                            bProfile ? ParamOrder++ : BindOrder++);
        Port->A = I.Input;
        Port->A.Expression = Copies[E];
    }
    UMaterialEditingLibrary::LayoutMaterialFunctionExpressions(Bindings);
    UMaterialEditingLibrary::LayoutMaterialFunctionExpressions(Parameters);
    Bindings->PostEditChange();
    Parameters->PostEditChange();
    Bindings->UpdateDependentFunctionCandidates();
    Parameters->UpdateDependentFunctionCandidates();
    UMaterialEditingLibrary::UpdateMaterialFunction(Bindings);
    UMaterialEditingLibrary::UpdateMaterialFunction(Parameters);
    auto *BindCall = Add<UMaterialExpressionMaterialFunctionCall>(Surface, -1100, 0);
    BindCall->SetMaterialFunction(Bindings);
    auto *ParamCall = Add<UMaterialExpressionMaterialFunctionCall>(Surface, -700, 1100);
    ParamCall->SetMaterialFunction(Parameters);
    auto *NewCore = CastChecked<UMaterialExpressionCustom>(
        UMaterialEditingLibrary::DuplicateMaterialExpression(nullptr, Surface, Core));
    NewCore->Material = nullptr;
    NewCore->Function = Surface;
    NewCore->MaterialExpressionEditorX = -200;
    NewCore->MaterialExpressionEditorY = 0;
    for (auto &I : NewCore->Inputs)
    {
        if (I.InputName.IsNone() || !I.Input.Expression)
            continue;
        auto *Call = IsProfile[I.Input.Expression] ? ParamCall : BindCall;
        int32 Index = OutputIndex(Call, I.InputName);
        if (Index == INDEX_NONE)
        {
            Message = TEXT("Missing function output.");
            return false;
        }
        I.Input.Connect(Index, Call);
    }
    auto *Attributes = Add<UMaterialExpressionMakeMaterialAttributes>(Surface, 350, 0);
    auto PreserveOutput = [&](FExpressionInput &Destination, const FExpressionInput &Source)
    {
        Destination = Source;
        Destination.Expression = NewCore;
    };
    PreserveOutput(Attributes->BaseColor, Data->BaseColor);
    PreserveOutput(Attributes->EmissiveColor, Data->EmissiveColor);
    PreserveOutput(Attributes->Normal, Data->Normal);
    PreserveOutput(Attributes->Metallic, Data->Metallic);
    PreserveOutput(Attributes->Specular, Data->Specular);
    PreserveOutput(Attributes->Roughness, Data->Roughness);
    Output(Surface, TEXT("Cloth"), Attributes, 0, 0);
    auto SurfaceNodes = UMaterialEditingLibrary::GetMaterialFunctionExpressions(Surface);
    SurfaceNodes.Last()->MaterialExpressionEditorX = 750;
    Surface->PostEditChange();
    Surface->UpdateDependentFunctionCandidates();
    UMaterialEditingLibrary::UpdateMaterialFunction(Surface);
    // Validate a candidate before replacing even an in-memory source expression.
    auto *Candidate = NewObject<UMaterial>(GetTransientPackage());
    Candidate->SetShadingModel(M->GetShadingModels().GetFirstShadingModel());
    Candidate->BlendMode = M->BlendMode;
    Candidate->TwoSided = M->TwoSided;
    Candidate->bTangentSpaceNormal = M->bTangentSpaceNormal;
    ConnectMaster(Candidate, Surface);
    if (!Compile(Candidate, Message))
        return false;
    // Persist dependencies before replacing the source graph.
    M->Modify();
    for (auto *Asset : NewTextures)
        if (!Save(Asset, true))
        {
            Message = TEXT("Texture save failed; source untouched.");
            return false;
        }
    for (auto *F : {Bindings, Parameters, Surface})
        if (!Save(F, true))
        {
            Message = TEXT("Function save failed; source untouched. Partial new assets must be inspected.");
            return false;
        }
    // Iterate the snapshot: DeleteMaterialExpression removes elements from the live collection.
    for (UMaterialExpression* Expression : Old)
    {
        UMaterialEditingLibrary::DeleteMaterialExpression(M, Expression);
    }
    ConnectMaster(M, Surface);
    if (!Compile(M, Message))
    {
        Message = TEXT("Source reconnection failed; source package was not saved.");
        return false;
    }
    if (!Save(M, false))
    {
        Message = TEXT("Source save failed. Generated functions remain; inspect before retrying.");
        return false;
    }
    Message = FString::Printf(TEXT("Encapsulated %d original expressions into Bindings / Parameters / Cloth. Master "
                                   "now has one function call. Existing MI/DA values and shader code preserved."),
                              Old.Num());
    return true;
}
} // namespace PadmaClothGraph
static FAutoConsoleCommand RefactorCloth(
    TEXT("PadmaNPR.OrganizeClothGraph"),
    TEXT("Explicitly encapsulate the flat Cloth master without changing MI/Profile values."),
    FConsoleCommandDelegate::CreateLambda(
        []
        {
            auto *M = LoadObject<UMaterial>(nullptr, TEXT("/PadmaNPR/Materials/M_NPR_Cloth.M_NPR_Cloth"));
            FString Message;
            const bool Ok = PadmaClothGraph::Encapsulate(M, Message);
            UE_LOG(LogTemp, Display, TEXT("Padma Cloth organize [%s]: %s"), Ok ? TEXT("OK") : TEXT("FAILED"), *Message);
        }));
