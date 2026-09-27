#include "PadmaNPRProfile.h"
#include "Materials/Material.h"
#include "Materials/MaterialInterface.h"
void FPadmaNPRSlot::GetParameters(int32 ProfileId,
                                  TMap<FName, FLinearColor> &O) const {
  O.Reset();
  if (bReferenceResponse) {
    O = ReferenceVectors;
    return;
  }
  O.Add("NPR_Identity",
        FLinearColor((float)Type, MaterialId, MaterialRegionId, ProfileId));
  O.Add("NPR_Tint", Tint);
  O.Add("NPR_Shade", ShadowColor);
  O.Add("NPR_Fill",
        FLinearColor(FillColor.R, FillColor.G, FillColor.B, FillIntensity));
  O.Add("NPR_Cel", FLinearColor(Threshold, Feather, AOWeight, RimStrength));
  O.Add("NPR_Face", FLinearColor(Face.ThresholdOffset, Face.Feather,
                                 Face.OverheadBlend, Face.BlurUV));
  O.Add("NPR_Skin", FLinearColor(Skin.ScatterColor.R, Skin.ScatterColor.G,
                                 Skin.ScatterColor.B, Skin.Warmth));
  O.Add("NPR_HairLobes", FLinearColor(Hair.WidePower, Hair.NarrowPower,
                                      Hair.WideStrength, Hair.NarrowStrength));
  O.Add("NPR_HairShift", FLinearColor(Hair.WideShift, Hair.NarrowShift,
                                      Hair.StrandNoise, Hair.Backlit));
  O.Add("NPR_HairTint",
        FLinearColor(Hair.HighlightColor.R, Hair.HighlightColor.G,
                     Hair.HighlightColor.B, Hair.BangMaxHeight));
  O.Add("NPR_Eye", FLinearColor(Eye.MatcapWeight, Eye.Fresnel,
                                Eye.SpecularPower, Eye.SpecularStrength));
  O.Add("NPR_Surface", FLinearColor(Face.SDFWeight, Skin.Wrap, Eye.IrisDepth,
                                    ExposureCompensation));
  O.Add("NPR_Alpha", FLinearColor(Hair.BangOpacity, Hair.FrontViewOnly,
                                  HelperOpacity, Debug));
}
bool UPadmaNPRProfile::Validate(FString &Error) const {
  Error.Reset();
  TSet<FName> Names;
  if (Slots.IsEmpty()) {
    Error = TEXT("Profile has no surface bindings.");
    return false;
  }
  if (FallbackLightDirection.ContainsNaN() ||
      FallbackLightDirection.IsNearlyZero() ||
      HeadAxisCorrection.ContainsNaN()) {
    Error = TEXT("Invalid light direction or head axes.");
    return false;
  }
  for (const auto &S : Slots) {
    if (S.SlotName.IsNone() || Names.Contains(S.SlotName) || !S.Material) {
      Error = TEXT("Missing material/slot or duplicate slot name.");
      return false;
    }
    Names.Add(S.SlotName);
    if (!S.bReferenceResponse && (S.Type == EPadmaNPRSurface::Cloth ||
                                  S.Type == EPadmaNPRSurface::HairLine ||
                                  S.Type == EPadmaNPRSurface::Brow ||
                                  S.Type == EPadmaNPRSurface::EyeShadow)) {
      Error = TEXT("This surface requires Reference Response. Use "
                   "PadmaClothProfile for the legacy cloth interface.");
      return false;
    }
    const FString Expected =
        FString(TEXT("M_NPR_")) +
        StaticEnum<EPadmaNPRSurface>()->GetNameStringByValue((int64)S.Type) +
        (S.bReferenceResponse ? TEXT("_Reference") : TEXT(""));
    if (!S.Material->GetMaterial() ||
        S.Material->GetMaterial()->GetName() != Expected) {
      Error = FString::Printf(TEXT("%s requires %s parent."),
                              *S.SlotName.ToString(), *Expected);
      return false;
    }
    TMap<FName, FLinearColor> P;
    S.GetParameters(ProfileId, P);
    if (S.bReferenceResponse) {
      for (const auto &Pair : S.ReferenceScalars) {
        if (Pair.Key.IsNone() || !FMath::IsFinite(Pair.Value)) {
          Error = TEXT("Invalid reference scalar parameter.");
          return false;
        }
      }
      for (const auto &Pair : P) {
        if (Pair.Key.IsNone()) {
          Error = TEXT("Reference vector parameter has no name.");
          return false;
        }
      }
    }
    for (const auto &Pair : P)
      for (int I = 0; I < 4; ++I)
        if (!FMath::IsFinite(Pair.Value.Component(I))) {
          Error = TEXT("Non-finite profile parameter.");
          return false;
        }
    if (S.bReferenceResponse)
      continue;
    if (S.Skin.Wrap < 0 || S.Skin.Wrap > 1 || S.Hair.BangOpacity < 0 ||
        S.Hair.BangOpacity > 1 || S.Hair.FrontViewOnly < 0 ||
        S.Hair.FrontViewOnly > 1 || S.HelperOpacity < 0 ||
        S.HelperOpacity > 1) {
      Error = TEXT("Wrap and opacity controls must be in [0,1].");
      return false;
    }
    if (S.Feather <= 0 || S.Face.Feather <= 0 || S.Hair.WidePower < 1 ||
        S.Hair.NarrowPower < 1 || S.Eye.SpecularPower < 1) {
      Error = TEXT("Invalid feather or highlight exponent.");
      return false;
    }
  }
  return true;
}
