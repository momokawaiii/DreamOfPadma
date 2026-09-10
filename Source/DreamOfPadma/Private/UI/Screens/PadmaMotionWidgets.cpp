#include "UI/Screens/PadmaMotionWidgets.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"

void SPadmaMotionElement::Construct(const FArguments& Args)
{
 Delay=Args._Delay;Duration=FMath::Max(.01f,Args._Duration);Distance=Args._Distance;
 ChildSlot[Args._Content.Widget];SetRenderOpacity(0);
 SetRenderTransform(FSlateRenderTransform(FVector2D(Distance,0)));
}
void SPadmaMotionElement::Tick(const FGeometry& G,double Time,float Delta)
{
 SCompoundWidget::Tick(G,Time,Delta);
 if(Elapsed>=Delay+Duration)return;
 Elapsed+=Delta;const float T=FMath::Clamp((Elapsed-Delay)/Duration,0.f,1.f);
 const float Ease=1.f-FMath::Pow(1.f-T,3.f);
 SetRenderOpacity(FMath::Clamp(T*2.f,0.f,1.f));
 SetRenderTransform(FSlateRenderTransform(FVector2D(Distance*(1-Ease),0)));
}
void SPadmaWarBalance::Construct(const FArguments& Args)
{
 Material=Args._Material;Brush.SetResourceObject(Args._Material);Brush.ImageSize=FVector2D(490,32);
 SetCanTick(true);
}
void SPadmaWarBalance::SetBalance(float Share){Target=FMath::Clamp(Share,0.f,1.f);}
void SPadmaWarBalance::Tick(const FGeometry& G,double Time,float Delta)
{
 SLeafWidget::Tick(G,Time,Delta);
 const float Next=FMath::FInterpTo(Display,Target,Delta,5.f);
 if(!FMath::IsNearlyEqual(Display,Next,.00001f)){Display=Next;if(Material.IsValid())Material->SetScalarParameterValue(TEXT("Balance"),Display);}
 if(Material.IsValid())Material->SetScalarParameterValue(TEXT("AspectRatio"),G.GetLocalSize().X/FMath::Max(1.,G.GetLocalSize().Y));
}
int32 SPadmaWarBalance::OnPaint(const FPaintArgs&,const FGeometry& G,const FSlateRect&,FSlateWindowElementList& Out,int32 Layer,const FWidgetStyle& Style,bool)const
{
 const FVector2D Size=G.GetLocalSize();const float W=Size.X,H=Size.Y;
 const FLinearColor Gold(.76f,.63f,.39f,1),Text(.88f,.87f,.78f,1);
 const auto* White=FCoreStyle::Get().GetBrush("WhiteBrush");
 FSlateDrawElement::MakeBox(Out,Layer,G.ToPaintGeometry(),Brush.GetResourceObject()?&Brush:White,ESlateDrawEffect::None,Brush.GetResourceObject()?Style.GetColorAndOpacityTint():FLinearColor(.025f,.11f,.12f));
 auto Lines=[&](TArray<FVector2D> P,float Thickness=1.f){FSlateDrawElement::MakeLines(Out,Layer+2,G.ToPaintGeometry(),P,ESlateDrawEffect::None,Gold,true,Thickness);};
 // Central balance glyph is geometry, so it remains crisp at every DPI.
 const FVector2D C(W*.5f,H*.49f);const float Tilt=(Display-.5f)*8;
 FSlateDrawElement::MakeBox(Out,Layer+1,G.ToPaintGeometry(FVector2D(54,H),FSlateLayoutTransform(FVector2D(W*.5f-27,0))),White,ESlateDrawEffect::None,FLinearColor(.008f,.022f,.033f,.96f));
 Lines({C+FVector2D(0,-11),C+FVector2D(0,10)},1.4f);
 Lines({C+FVector2D(-18,-5-Tilt),C+FVector2D(18,-5+Tilt)},1.4f);
 Lines({C+FVector2D(-8,11),C+FVector2D(8,11)},1.4f);
 for(int32 Side:{-1,1}){const FVector2D Arm=C+FVector2D(Side*15,-5+Side*Tilt);Lines({Arm+FVector2D(-6,12),Arm,Arm+FVector2D(6,12),Arm+FVector2D(-6,12)});}
 const auto Font=FCoreStyle::GetDefaultFontStyle("Regular",12);
 FSlateDrawElement::MakeText(Out,Layer+3,G.ToPaintGeometry(FVector2D(165,20),FSlateLayoutTransform(FVector2D(12,5))),FString::Printf(TEXT("我方  %.0f"),Target*100),Font,ESlateDrawEffect::None,Text);
 FSlateDrawElement::MakeText(Out,Layer+3,G.ToPaintGeometry(FVector2D(165,20),FSlateLayoutTransform(FVector2D(W-95,5))),FString::Printf(TEXT("王庭  %.0f"),(1-Target)*100),Font,ESlateDrawEffect::None,Text);
 return Layer+3;
}
