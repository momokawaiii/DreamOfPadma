#include "UI/Screens/PadmaDivinationStyle.h"

UPadmaDivinationStyle::UPadmaDivinationStyle()
{
 Heading=NSLOCTEXT("Padma.Divination","HeadingV2","今日答案！");
 ResultHeading=NSLOCTEXT("Padma.Divination","ResultV2","正与反");
 Accent=FLinearColor(.002f,.032f,1.f);
 SerifFont=TSoftObjectPtr<UFontFace>(FSoftObjectPath(TEXT("/Game/ThirdParty/NotoSerif/FF_NotoSerifSC.FF_NotoSerifSC")));
 CardAtlas=TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/Padma/UI/Textures/T_CardIllustrations.T_CardIllustrations")));
 WaterMaterial=TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/Padma/UI/Divination/Materials/MI_DivinationWaterArk.MI_DivinationWaterArk")));
 CardMaterial=TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/Padma/UI/Divination/Materials/MI_DivinationCardArk.MI_DivinationCardArk")));
 for(int32 I=0;I<2;++I)
 {
  FPadmaDivinationCard C;C.Title=FText::FromString(I?TEXT("所失"):TEXT("所得"));C.Subtitle=FText::FromString(I?TEXT("Cost"):TEXT("Gain"));C.ArtIndex=I?7:0;
  C.Destination=I?FVector2D(-105,-8):FVector2D(-510,-62);
  C.CardSize=I?FVector2D(280,400):FVector2D(290,445);
  C.StartTime=I?.416667f:.35f;C.Duration=.833333f;C.StartScale=I?.45199f:4.562f;
  C.StartOffset=I?FVector(550,577,400):FVector(0,0,-862);
  C.RotationDegrees=I?FVector(695,0,60):FVector(-80,480,-32);
  C.FinalRotation=I?FVector(-31.71156,-30.89448,-18.25034):FVector(0,0,-11);C.bReversed=I==0;
  FRichCurve* Curve=C.Travel.GetRichCurve();
  auto A=Curve->AddKey(0,0);auto B=Curve->AddKey(.46f,.896f);auto D=Curve->AddKey(1,1);
  Curve->SetKeyInterpMode(A,RCIM_Cubic);Curve->SetKeyInterpMode(B,RCIM_Cubic);Curve->SetKeyInterpMode(D,RCIM_Cubic);Curve->AutoSetTangents();Cards.Add(C);
 }
}
