#include "PadmaClothProfile.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
void FPadmaClothSettings::ToMaterialParameters(TMap<FName, FLinearColor> &Out) const
{
    Out.Reset();
    Out.Add(TEXT("NPR_Tint"), Base.Tint);
    Out.Add(TEXT("NPR_NormalControls"), FLinearColor(Normal.MapWeight, Normal.CameraWeight, Normal.LightWeight, 0));
    Out.Add(TEXT("NPR_Lam"),
            FLinearColor(Diffuse.Threshold, Diffuse.Feather, Diffuse.Strength, static_cast<float>(Diffuse.Mode)));
    Out.Add(TEXT("NPR_ShadowColor"), Diffuse.ShadowColor);
    Out.Add(TEXT("NPR_AOControls"), FLinearColor(AO.Smooth, AO.Offset, AO.Strength, 0));
    Out.Add(TEXT("NPR_RimShadow"), FLinearColor(RimShadow.Threshold, RimShadow.Feather, RimShadow.Strength, 0));
    Out.Add(TEXT("NPR_RimShadowColor"), RimShadow.Color);
    Out.Add(TEXT("NPR_RimHighlight"),
            FLinearColor(RimHighlight.Threshold, RimHighlight.Feather, RimHighlight.Strength, 0));
    Out.Add(TEXT("NPR_RimHighlightColor"), RimHighlight.Color);
    Out.Add(TEXT("NPR_Spec"),
            FLinearColor(Specular.BaseRoughness, Specular.MetalRoughness, Specular.RemapSmooth, Specular.RemapOffset));
    Out.Add(TEXT("NPR_SpecBaseColor"), Specular.BaseColor);
    Out.Add(TEXT("NPR_SpecMetalColor"), Specular.MetalColor);
    Out.Add(TEXT("NPR_MatcapControls"), FLinearColor(Matcap.Strength01, Matcap.Strength02, 0, 0));
    Out.Add(TEXT("NPR_MatcapTint1"), Matcap.Tint01);
    Out.Add(TEXT("NPR_MatcapTint2"), Matcap.Tint02);
    Out.Add(TEXT("NPR_Surface"),
            FLinearColor(Surface.MetallicScale, Surface.Specular, Surface.EmissiveWeight, Specular.Strength));
    if (Reference.Enabled)
    {
        Out.Add(TEXT("NPR_ReferenceControls"), FLinearColor(Reference.BaseColorPower, Reference.ExposureCompensation, Reference.MetalMaskSmooth, Reference.MetalMaskOffset));
        Out.Add(TEXT("NPR_BranchNormals"), FLinearColor(Reference.LamNormalDetail, Reference.RimShadowNormalDetail, Reference.RimHighlightNormalDetail, Reference.MatcapNormalDetail));
        Out.Add(TEXT("NPR_ReferenceGGX"), FLinearColor(Reference.GGXNormalDetail, Reference.GGXCameraWeight, Reference.GGXMetalStrength, Reference.GGXLamStrength));
        Out.Add(TEXT("NPR_ReferenceSurface"), FLinearColor(Reference.NativeMetallic, Reference.NativeRoughness, Reference.RoughnessMapEndpoint, Reference.RoughnessScale));
        Out.Add(TEXT("NPR_ReferenceAOColor"), Reference.AOColor);
        Out.Add(TEXT("NPR_MatcapUV"), Reference.MatcapUV);
        Out.Add(TEXT("NPR_ToneControls"), FLinearColor(Reference.InverseTonemapStrength,0,0,0));
    }
}
bool UPadmaClothProfile::Resolve(FName Slot, FPadmaClothSettings &Out, FString &Error) const
{
    Error.Reset();
    Out = Defaults;
    TSet<FName> Seen;
    for (const auto &Override : SlotOverrides)
    {
        if (Override.SlotName.IsNone() || Seen.Contains(Override.SlotName))
        {
            Error = TEXT("Cloth overrides require unique non-empty material slot names.");
            return false;
        }
        Seen.Add(Override.SlotName);
        if (Override.SlotName != Slot)
            continue;
        if (Override.bOverrideBase)
            Out.Base = Override.Base;
        if (Override.bOverrideNormal)
            Out.Normal = Override.Normal;
        if (Override.bOverrideDiffuse)
            Out.Diffuse = Override.Diffuse;
        if (Override.bOverrideAO)
            Out.AO = Override.AO;
        if (Override.bOverrideRimShadow)
            Out.RimShadow = Override.RimShadow;
        if (Override.bOverrideRimHighlight)
            Out.RimHighlight = Override.RimHighlight;
        if (Override.bOverrideSpecular)
            Out.Specular = Override.Specular;
        if (Override.bOverrideMatcap)
            Out.Matcap = Override.Matcap;
        if (Override.bOverrideSurface)
            Out.Surface = Override.Surface;
        if (Override.bOverrideReference)
            Out.Reference = Override.Reference;
    }
    TMap<FName, FLinearColor> Values;
    Out.ToMaterialParameters(Values);
    for (const auto &Value : Values)
    {
        const FLinearColor &C = Value.Value;
        if (!FMath::IsFinite(C.R) || !FMath::IsFinite(C.G) || !FMath::IsFinite(C.B) || !FMath::IsFinite(C.A))
        {
            Error = TEXT("Cloth parameters must be finite.");
            return false;
        }
    }
    if (Out.Diffuse.Feather <= 0 || Out.AO.Smooth <= 0 || Out.Specular.RemapSmooth <= 0 || Out.RimShadow.Feather <= 0 ||
        Out.RimHighlight.Feather <= 0 || Out.Specular.BaseRoughness <= 0 || Out.Specular.MetalRoughness <= 0)
    {
        Error = TEXT("Feather, remap widths and roughness must be positive.");
        return false;
    }
    if (Out.Reference.Enabled && (Out.Reference.BaseColorPower <= 0 || Out.Reference.NativeRoughness <= 0 ||
        Out.Reference.RoughnessMapEndpoint <= 0 || Out.Reference.RoughnessScale <= 0))
    {
        Error = TEXT("Reference color power and roughness terms must be positive.");
        return false;
    }
    return true;
}
TMap<FName, FLinearColor> UPadmaClothLibrary::GetClothParameters(const UPadmaClothProfile* Profile, FName SlotName, FString& Error)
{
    TMap<FName, FLinearColor> Values;
    FPadmaClothSettings Settings;
    Error.Reset();
    if (!Profile) Error = TEXT("Profile is required.");
    else if (Profile->Resolve(SlotName, Settings, Error)) Settings.ToMaterialParameters(Values);
    return Values;
}
UMaterialInstanceDynamic *UPadmaClothLibrary::CreateClothMID(UObject *Owner, UMaterialInterface *TextureBinding,
                                                             const UPadmaClothProfile *Profile, FName SlotName,
                                                             FString &Error)
{
    Error.Reset();
    if (!Owner || !TextureBinding || !Profile)
    {
        Error = TEXT("Owner, texture-binding material and Profile are required.");
        return nullptr;
    }
    FPadmaClothSettings Settings;
    if (!Profile->Resolve(SlotName, Settings, Error))
        return nullptr;
    TMap<FName, FLinearColor> Values;
    Settings.ToMaterialParameters(Values);
    TArray<FMaterialParameterInfo> Infos;
    TArray<FGuid> Ids;
    TextureBinding->GetAllVectorParameterInfo(Infos, Ids);
    const bool IsReferenceMaterial = Infos.ContainsByPredicate([](const FMaterialParameterInfo& Info) { return Info.Name == TEXT("NPR_ReferenceControls"); });
    if (IsReferenceMaterial != Settings.Reference.Enabled)
    {
        Error = TEXT("Reference response must match the selected material variant.");
        return nullptr;
    }
    for (const auto &Value : Values)
    {
        if (!Infos.ContainsByPredicate(
                [&](const FMaterialParameterInfo &Info)
                {
                    return Info.Name == Value.Key && Info.Association == EMaterialParameterAssociation::GlobalParameter;
                }))
        {
            Error = FString::Printf(TEXT("Incompatible Cloth parent: missing %s"), *Value.Key.ToString());
            return nullptr;
        }
    }
    auto *MID = UMaterialInstanceDynamic::Create(TextureBinding, Owner);
    if (!MID)
    {
        Error = TEXT("Could not allocate Cloth MID.");
        return nullptr;
    }
    for (const auto &Value : Values)
        MID->SetVectorParameterValue(Value.Key, Value.Value);
    return MID;
}
