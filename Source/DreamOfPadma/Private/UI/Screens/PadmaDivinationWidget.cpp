#include "UI/Screens/PadmaDivinationWidget.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture2D.h"
#include "InputCoreTypes.h"
#include "Engine/FontFace.h"
#include "Fonts/CompositeFont.h"
#include "Widgets/SLeafWidget.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/Application/SlateApplication.h"
#include "Rendering/DrawElements.h"
#include "Rendering/SlateRenderer.h"
#include "Fonts/FontMeasure.h"
#include "Styling/CoreStyle.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace
{
float Unit(float V){return FMath::Clamp(V,0.f,1.f);}
float Ramp(float T,float A,float B){return Unit((T-A)/FMath::Max(.001f,B-A));}
float Ease(float T){return 1-FMath::Pow(1-Unit(T),3.f);}
FLinearColor Alpha(FLinearColor C,float A){C.A*=Unit(A);return C;}
FVector2D GalleryProject(FVector2D Center,float U,FVector2D Local)
{
 const FQuat Start(.130036598,.408752306,.063744875,.901081920),End(.156163798,.319519241,.025294724,.934280739);
 const FQuat Rotation=FQuat::FastLerp(Start,End,Unit(U)).GetNormalized();
 const FVector Q=Rotation.RotateVector(FVector(Local.X,Local.Y,0));
 return FVector2D(800,450)+(Center+FVector2D(Q.X,Q.Y)-FVector2D(800,450))*(1400/FMath::Max(350.,1400+Q.Z));
}

// Clip the source triangles, including their atlas UVs, to the original expanding stencil circle.
void ClipConvex(TArray<FSlateVertex>& Vertices,TArray<SlateIndex>& Indices,const TArray<FVector2f>& Boundary)
{
 const int32 Segments=Boundary.Num();
 auto Cross=[](FVector2f A,FVector2f B){return A.X*B.Y-A.Y*B.X;};
 auto Interpolate=[](const FSlateVertex& A,const FSlateVertex& B,float T)
 {
  FSlateVertex V=A;V.Position=FMath::Lerp(A.Position,B.Position,T);V.MaterialTexCoords=FMath::Lerp(A.MaterialTexCoords,B.MaterialTexCoords,T);
  for(int32 K=0;K<4;++K)V.TexCoords[K]=FMath::Lerp(A.TexCoords[K],B.TexCoords[K],T);
  return V;
 };
 TArray<FSlateVertex> Clipped;TArray<SlateIndex> Faces;
 for(int32 I=0;I+2<Indices.Num();I+=3)
 {
  TArray<FSlateVertex> Polygon={Vertices[Indices[I]],Vertices[Indices[I+1]],Vertices[Indices[I+2]]};
  for(int32 Edge=0;Edge<Segments&&!Polygon.IsEmpty();++Edge)
  {
   TArray<FSlateVertex> Output;const auto Start=Boundary[Edge],Direction=Boundary[(Edge+1)%Segments]-Start;
   auto Previous=Polygon.Last();float PreviousSide=Cross(Direction,Previous.Position-Start);
   for(const auto& Current:Polygon)
   {
    const float Side=Cross(Direction,Current.Position-Start);
    if((Side>=0)!=(PreviousSide>=0))Output.Add(Interpolate(Previous,Current,PreviousSide/(PreviousSide-Side)));
    if(Side>=0)Output.Add(Current);Previous=Current;PreviousSide=Side;
   }
   Polygon=MoveTemp(Output);
  }
  if(Polygon.Num()<3)continue;
  const SlateIndex Base=SlateIndex(Clipped.Num());Clipped.Append(Polygon);
  for(int32 K=1;K<Polygon.Num()-1;++K)Faces.Append({Base,SlateIndex(Base+K),SlateIndex(Base+K+1)});
 }
 Vertices=MoveTemp(Clipped);Indices=MoveTemp(Faces);
}
void ClipOutside(TArray<FSlateVertex>& Vertices,TArray<SlateIndex>& Indices,const TArray<FVector2f>& Boundary)
{
 auto Cross=[](FVector2f A,FVector2f B){return A.X*B.Y-A.Y*B.X;};
 auto Half=[&](const TArray<FSlateVertex>& Polygon,FVector2f Start,FVector2f Direction,bool Inside)
 {
  TArray<FSlateVertex> Result;if(Polygon.IsEmpty())return Result;auto Previous=Polygon.Last();float PreviousSide=Cross(Direction,Previous.Position-Start);
  for(const auto& Current:Polygon){float Side=Cross(Direction,Current.Position-Start);if((Side>=0)!=(PreviousSide>=0)){float T=PreviousSide/(PreviousSide-Side);auto V=Previous;V.Position=FMath::Lerp(Previous.Position,Current.Position,T);V.MaterialTexCoords=FMath::Lerp(Previous.MaterialTexCoords,Current.MaterialTexCoords,T);for(int32 K=0;K<4;++K)V.TexCoords[K]=FMath::Lerp(Previous.TexCoords[K],Current.TexCoords[K],T);Result.Add(V);}if((Side>=0)==Inside)Result.Add(Current);Previous=Current;PreviousSide=Side;}return Result;
 };
 TArray<FSlateVertex> Output;TArray<SlateIndex> Faces;
 for(int32 I=0;I+2<Indices.Num();I+=3)
 {
  TArray<FSlateVertex> Remaining={Vertices[Indices[I]],Vertices[Indices[I+1]],Vertices[Indices[I+2]]};
  for(int32 Edge=0;Edge<Boundary.Num()&&!Remaining.IsEmpty();++Edge)
  {
   const auto Start=Boundary[Edge],Direction=Boundary[(Edge+1)%Boundary.Num()]-Start;const auto Outside=Half(Remaining,Start,Direction,false);
   if(Outside.Num()>2){SlateIndex Base=Output.Num();Output.Append(Outside);for(int32 K=1;K+1<Outside.Num();++K)Faces.Append({Base,SlateIndex(Base+K),SlateIndex(Base+K+1)});}
   Remaining=Half(Remaining,Start,Direction,true);
  }
 }
 Vertices=MoveTemp(Output);Indices=MoveTemp(Faces);
}
void ClipHeroIris(const FGeometry& Geometry,TArray<FSlateVertex>& Vertices,TArray<SlateIndex>& Indices,FVector2D Center,float Radius)
{
 TArray<FVector2f> Boundary;
 for(int32 I=0;I<64;++I){float Angle=I*UE_TWO_PI/64;Boundary.Add(TransformPoint(Geometry.GetAccumulatedRenderTransform(),FVector2f(Center+FVector2D(FMath::Cos(Angle),FMath::Sin(Angle))*Radius)));}
 ClipConvex(Vertices,Indices,Boundary);
}


}

bool FPadmaDivinationPlayback::Reveal()
{
 if(Phase!=EPadmaDivinationPhase::AwaitReveal)return false;
 Phase=EPadmaDivinationPhase::Reveal;Time=0;bPaused=false;return true;
}
bool FPadmaDivinationPlayback::Close()
{
 if(Phase==EPadmaDivinationPhase::Closing||Phase==EPadmaDivinationPhase::Closed)return false;
 ClosingFrom=Phase;Phase=EPadmaDivinationPhase::Closing;CloseTime=0;return true;
}
void FPadmaDivinationPlayback::Replay()
{
 if(Phase==EPadmaDivinationPhase::Closing||Phase==EPadmaDivinationPhase::Closed)return;
 Phase=EPadmaDivinationPhase::Enter;Time=AmbientTime=CloseTime=0;bPaused=false;
}
void FPadmaDivinationPlayback::Seek(float Seconds,const UPadmaDivinationStyle& S)
{
 if(Phase==EPadmaDivinationPhase::Closing||Phase==EPadmaDivinationPhase::Closed)return;
 const float Enter=FMath::Max(.1f,S.EnterDuration),Reveal=FMath::Max(.1f,S.RevealDuration);
 Seconds=FMath::Clamp(Seconds,0.f,Enter+Reveal);bPaused=true;AmbientTime=Seconds;
 if(Seconds<Enter){Phase=EPadmaDivinationPhase::Enter;Time=Seconds;}
 else if(Seconds>=Enter+Reveal){Phase=EPadmaDivinationPhase::Result;Time=Reveal;}
 else {Phase=EPadmaDivinationPhase::Reveal;Time=Seconds-Enter;}
}
bool FPadmaDivinationPlayback::Advance(float Delta,const UPadmaDivinationStyle& S)
{
 if(Phase==EPadmaDivinationPhase::Closed)return false;
 if(Phase==EPadmaDivinationPhase::Closing)
 {
  CloseTime+=FMath::Max(0.f,Delta);
  if(CloseTime>=FMath::Max(.05f,S.CloseDuration)){Phase=EPadmaDivinationPhase::Closed;return true;}
  return false;
 }
 if(bPaused)return false;
 Delta=FMath::Max(0.f,Delta)*FMath::Clamp(Rate*S.PlaybackRate,.025f,6.f);AmbientTime+=Delta;
 if(Phase==EPadmaDivinationPhase::Enter)
 {
  Time=FMath::Min(Time+Delta,FMath::Max(.1f,S.EnterDuration));
  if(Time>=FMath::Max(.1f,S.EnterDuration))Phase=EPadmaDivinationPhase::AwaitReveal;
 }
 else if(Phase==EPadmaDivinationPhase::Reveal)
 {
  Time=FMath::Min(Time+Delta,FMath::Max(.1f,S.RevealDuration));
  if(Time>=FMath::Max(.1f,S.RevealDuration))Phase=EPadmaDivinationPhase::Result;
 }
 return false;
}

class SPadmaDivinationCanvas : public SLeafWidget
{
public:
 SLATE_BEGIN_ARGS(SPadmaDivinationCanvas) {} SLATE_ARGUMENT(SPadmaDivinationWidget*, Owner) SLATE_END_ARGS()
 SPadmaDivinationWidget* Owner=nullptr;
 void Construct(const FArguments& A){Owner=A._Owner;SetVisibility(EVisibility::HitTestInvisible);}
 virtual FVector2D ComputeDesiredSize(float)const override{return FVector2D(1600,900);}
 virtual int32 OnPaint(const FPaintArgs&,const FGeometry& G,const FSlateRect&,FSlateWindowElementList& Out,int32 L,const FWidgetStyle& Style,bool)const override
 {
  if(!Owner||!Owner->Theme.IsValid())return L;
  const auto& S=*Owner->Theme;const auto& P=Owner->Playback;
  const auto Phase=P.Phase==EPadmaDivinationPhase::Closing?P.ClosingFrom:P.Phase;
  const bool Result=Phase==EPadmaDivinationPhase::Reveal||Phase==EPadmaDivinationPhase::Result;
  const bool Selection=Owner->bSelection;
  const float T=Result?P.Time*2.267f/FMath::Max(.1f,S.RevealDuration):0;
  const float E=Result?2.f:P.Time*2.f/FMath::Max(.1f,S.EnterDuration);
  const float A=Style.GetColorAndOpacityTint().A;
  const FLinearColor White=FLinearColor::White,Black(.0001f,.0002f,.002f),Blue(.002f,.025f,.82f),Cyan(.005f,.72f,1.f);
  const auto* Brush=FCoreStyle::Get().GetBrush("WhiteBrush");
  const FVector2D Parallax=FVector2D::ZeroVector;
  auto Box=[&](FVector2D Pos,FVector2D Size,FLinearColor C,int32 Layer)
  {if(C.A>.001f)FSlateDrawElement::MakeBox(Out,Layer,G.ToPaintGeometry(Size,FSlateLayoutTransform(Pos)),Brush,ESlateDrawEffect::None,Alpha(C,A));};
  auto Line=[&](TArray<FVector2D> Points,FLinearColor C,float Width,int32 Layer)
  {if(C.A>.001f)FSlateDrawElement::MakeLines(Out,Layer,G.ToPaintGeometry(),Points,ESlateDrawEffect::None,Alpha(C,A),true,Width);};
  auto Poly=[&](const TArray<FVector2D>& Points,FLinearColor C,int32 Layer)
  {
   if(C.A<=.001f||Points.Num()<3)return;
   TArray<FSlateVertex> V;TArray<SlateIndex> I;
   for(auto Pnt:Points)V.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(G.GetAccumulatedRenderTransform(),FVector2f(Pnt),FVector2f(.5,.5),Alpha(C,A).ToFColor(false)));
   for(int32 N=1;N<Points.Num()-1;++N)I.Append({0,SlateIndex(N),SlateIndex(N+1)});
   FSlateDrawElement::MakeCustomVerts(Out,Layer,FSlateApplication::Get().GetRenderer()->GetResourceHandle(*Brush),V,I,nullptr,0,0);
  };
  auto Text=[&](const FString& Value,FVector2D Pos,int32 Size,FLinearColor C,int32 Layer,float Angle=0.f,FVector2D Scale=FVector2D(1,1),bool Serif=false)
  {
   if(C.A<=.001f)return;
   bool Chinese=false;for(TCHAR Ch:Value){if(Ch>=0x3400){Chinese=true;break;}}
   FName FontKey=Chinese?(Size>=35?TEXT("Heavy"):TEXT("Medium")):(Serif?TEXT("LabelSerif"):TEXT("None"));
   const auto Face=Owner->OriginalFonts.FindRef(FontKey);
   FSlateFontInfo Font=Owner->bTitleIntro&&Serif&&Owner->Serif.IsValid()?FSlateFontInfo(Owner->Serif,float(Size)*.75f):
    Face.IsValid()?FSlateFontInfo(Face,float(Size)*.75f):FCoreStyle::GetDefaultFontStyle("Bold",Size*.75f);
   // The original Chinese faces have a 500-unit line gap; align ink to the authored top edge.
   if(Chinese&&Face.IsValid()&&!Owner->bTitleIntro)Pos.Y-=Size*.5f;
   const auto Transform=FSlateRenderTransform(Concatenate(FScale2D(Scale),FQuat2D(FMath::DegreesToRadians(Angle)),Pos));
   FSlateDrawElement::MakeText(Out,Layer,G.MakeChild(FVector2D(1600,900),FSlateLayoutTransform(),Transform,FVector2D::ZeroVector).ToPaintGeometry(),Value,Font,ESlateDrawEffect::None,Alpha(C,A));
  };
  auto Ring=[&](FVector2D C,float R,FLinearColor Color,float Width,int32 Layer,float Tilt=0.f)
  {
   TArray<FVector2D> Pts;for(int32 I=0;I<=160;++I){float V=I*UE_TWO_PI/160;FVector2D Pt(FMath::Cos(V)*R,FMath::Sin(V)*R);Pt.X+=Pt.Y*Tilt;Pts.Add(C+Pt);}Line(Pts,Color,Width,Layer);
  };
  auto Plane=[&](const FSlateBrush& B,FVector2D Center,FVector2D Size,FVector Degrees,FVector Offset,float Scale,float Fade,int32 Layer,bool ReverseUV=false)
  {
   if(Fade<=.001f)return;
   const FQuat Q=FQuat(FVector(0,0,1),FMath::DegreesToRadians(Degrees.Z))*FQuat(FVector(0,1,0),FMath::DegreesToRadians(Degrees.Y))*FQuat(FVector(1,0,0),FMath::DegreesToRadians(Degrees.X));
   const bool Back=Q.RotateVector(FVector(0,0,1)).Z<0;
   const FSlateBrush& Use=Back?Owner->BackBrush:B;
   TArray<FSlateVertex> V;TArray<SlateIndex> Idx;constexpr int32 NX=12,NY=18;
   for(int32 Y=0;Y<=NY;++Y)for(int32 X=0;X<=NX;++X)
   {
    FVector2f UV(float(X)/NX,float(Y)/NY);
    FVector Pt=Q.RotateVector(FVector((UV.X-.5)*Size.X,(UV.Y-.5)*Size.Y,0)*Scale)+Offset;
    float Perspective=1500/FMath::Max(350.f,1500+float(Pt.Z));
    FVector2D Screen=Center+FVector2D(Pt.X,Pt.Y)*Perspective+Parallax*8;
    if(ReverseUV&&!Back)UV=FVector2f(1-UV.X,1-UV.Y);if(Back)UV.X=1-UV.X;
    V.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(G.GetAccumulatedRenderTransform(),FVector2f(Screen),FVector4f(UV.X,UV.Y,1,1),UV,Alpha(White,Fade*A).ToFColor(false)));
   }
   for(int32 Y=0;Y<NY;++Y)for(int32 X=0;X<NX;++X){SlateIndex K=Y*(NX+1)+X;Idx.Append({K,SlateIndex(K+1),SlateIndex(K+NX+2),K,SlateIndex(K+NX+2),SlateIndex(K+NX+1)});}
   FSlateDrawElement::MakeCustomVerts(Out,Layer,FSlateApplication::Get().GetRenderer()->GetResourceHandle(Use),V,Idx,nullptr,0,0);
  };
  auto Original=[&](FName Name,FVector2D Pos,FVector2D Size,float Fade,int32 Layer,FLinearColor Color=FLinearColor::White)
  {
   const auto* B=Owner->OriginalBrushes.Find(Name);if(!B||Fade<.001f)return;
   FSlateDrawElement::MakeBox(Out,Layer,G.ToPaintGeometry(Size,FSlateLayoutTransform(Pos)),B,ESlateDrawEffect::None,Alpha(Color,Fade*A));
  };
  auto DrawMesh=[&](const FPadmaOriginalMesh& M,const FPadmaOriginalMesh* Next,float Mix,FVector2D Origin,float Scale,float Angle,float Fade,int32 Layer,bool IsHero,bool Mirror=false)
  {
   const auto* B=Owner->OriginalBrushes.Find(M.Texture);if(M.Texture==TEXT("white"))B=Brush;if(!B||Fade<.001f)return;
   TArray<FSlateVertex> V;V.Reserve(M.Positions.Num());const float Co=FMath::Cos(Angle),Si=FMath::Sin(Angle);
   for(int32 I=0;I<M.Positions.Num();++I)
   {
    FVector2f Pt=M.Positions[I];if(Next&&Next->Positions.Num()==M.Positions.Num())Pt=FMath::Lerp(Pt,Next->Positions[I],Mix);
    FVector2D Pnt=IsHero?FVector2D(Pt.X,Pt.Y):FVector2D(Pt)-FVector2D(850,445);
    if(Mirror)Pnt.X=-Pnt.X;
    Pnt*=Scale;Pnt=Origin+FVector2D(Pnt.X*Co-Pnt.Y*Si,Pnt.X*Si+Pnt.Y*Co)+Parallax*10;
    const auto UV=M.UVs[I];const FLinearColor Tint=M.Texture==TEXT("white")?Black:(Next?FMath::Lerp(M.Color,Next->Color,Mix):M.Color);const float MeshAlpha=Next?FMath::Lerp(M.Alpha,Next->Alpha,Mix):M.Alpha;
    V.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(G.GetAccumulatedRenderTransform(),FVector2f(Pnt),FVector4f(UV.X,UV.Y,1,1),UV,Alpha(Tint,MeshAlpha*Fade*A).ToFColor(false)));
   }
   TArray<SlateIndex> Indices=M.Indices;
   if(IsHero&&Owner->HeroTime<.583333f)
   {
    const bool Special=Owner->Tarot.Selected==5;const float Time=Owner->HeroTime;
    const float Middle=Special?1542.033f:1409.773f,End=Special?2357.647f:2150.111f;
    const float Diameter=Time<.216667f?FMath::Lerp(109.024f,Middle,Ramp(Time,0,.216667f)):FMath::Lerp(Middle,End,Ramp(Time,.216667f,.583333f));
    ClipHeroIris(G,V,Indices,Special?FVector2D(427.5,446.25):FVector2D(670,-35),Diameter*.625f);
   }
   if(!Indices.IsEmpty())FSlateDrawElement::MakeCustomVerts(Out,Layer,FSlateApplication::Get().GetRenderer()->GetResourceHandle(*B),V,Indices,nullptr,0,0);
  };
  auto DrawHero=[&](int32 Layer)
  {
   const auto* Clip=Owner->HeroClips.Find(Owner->GetHeroClipName());
   if(!Clip||Clip->Frames.IsEmpty())return;
   const bool Special=Owner->Tarot.Selected==5;
   const float Duration=FMath::Max(.01f,Clip->Duration);
   float Time=Owner->HeroTime;if(Clip->bLoop)Time=FMath::Max(0.f,Time-1.7667f);
   const float Index=Clip->bLoop?FMath::Fmod(Time,Duration)/Duration*(Clip->Frames.Num()-1):Unit(Time/Duration)*(Clip->Frames.Num()-1);
   const int32 F=FMath::Clamp(int32(Index),0,Clip->Frames.Num()-1),N=Clip->bLoop?(F+1)%Clip->Frames.Num():FMath::Min(F+1,Clip->Frames.Num()-1);
   const auto& Now=Clip->Frames[F];const auto& Next=Clip->Frames[N];
   // Unity's 1280 x 720 RectTransform anchors, mirrored normal scale and Z rotation converted to screen space.
   const FVector2D Origin=Special?FVector2D(260,1583.75):FVector2D(1387.375,-183.9625);
   const float Scale=Special?.925f:.6875f;
   float Degrees=Special?.19977f:-115.83f;
   if(const auto* Keys=Owner->HeroEntryAngles.Find(Special?TEXT("special"):TEXT("normal"));Keys&&!Keys->IsEmpty())
   {
    Degrees=Keys->Last().Y;
    for(int32 K=1;K<Keys->Num();++K)if(Owner->HeroTime<=(*Keys)[K].X){Degrees=FMath::Lerp(float((*Keys)[K-1].Y),float((*Keys)[K].Y),Ramp(Owner->HeroTime,(*Keys)[K-1].X,(*Keys)[K].X));break;}
   }
   const float Angle=FMath::DegreesToRadians(Degrees);
   const auto* Drop=Owner->HeroClips.Find(Special?TEXT("drop_special"):TEXT("drop_normal"));
   const float Crossfade=Unit(Time/.8f);
   for(int32 I=0;I<Now.Num();++I)
   {
    if(Clip->bLoop&&Crossfade<1&&Drop&&!Drop->Frames.IsEmpty()&&Drop->Frames.Last().IsValidIndex(I)&&Next.IsValidIndex(I))
    {
     // Original AnimationState mixDuration=0.8: retain the outgoing drop pose while loop time advances.
     FPadmaOriginalMesh Blended=Now[I];const auto& From=Drop->Frames.Last()[I];const auto& To=Next[I];
     if(From.Positions.Num()==Blended.Positions.Num()&&To.Positions.Num()==Blended.Positions.Num())for(int32 K=0;K<Blended.Positions.Num();++K)Blended.Positions[K]=FMath::Lerp(From.Positions[K],FMath::Lerp(Now[I].Positions[K],To.Positions[K],Index-F),Crossfade);
     Blended.Color=FMath::Lerp(From.Color,FMath::Lerp(Now[I].Color,To.Color,Index-F),Crossfade);
     Blended.Alpha=FMath::Lerp(From.Alpha,FMath::Lerp(Now[I].Alpha,To.Alpha,Index-F),Crossfade);
     DrawMesh(Blended,nullptr,0,Origin,Scale,Angle,1,Layer,true,!Special);
    }
    else DrawMesh(Now[I],Next.IsValidIndex(I)?&Next[I]:nullptr,Index-F,Origin,Scale,Angle,1,Layer,true,!Special);
   }
  };
  auto OriginalTurn=[&](FName Name,FVector2D Pivot,FVector2D Size,FVector2D UVPivot,float Degrees,float Scale,float Fade,int32 Layer,FLinearColor Color=FLinearColor::White)
  {
   const auto* B=Owner->OriginalBrushes.Find(Name);if(!B)return;
   const auto Transform=FSlateRenderTransform(Concatenate(FVector2D(-Size.X*UVPivot.X,-Size.Y*UVPivot.Y),FScale2D(Scale),FQuat2D(FMath::DegreesToRadians(Degrees)),Pivot));
   FSlateDrawElement::MakeBox(Out,Layer,G.MakeChild(Size,FSlateLayoutTransform(),Transform,FVector2D::ZeroVector).ToPaintGeometry(),B,ESlateDrawEffect::None,Alpha(Color,Fade*A));
  };

  Box(FVector2D::ZeroVector,FVector2D(1600,900),Black,L);
  if(Owner->Water.IsValid())FSlateDrawElement::MakeBox(Out,L+1,G.ToPaintGeometry(),&Owner->WaterBrush,ESlateDrawEffect::None,Alpha(White,A));
  auto BackMark=[&](int32 Layer)
  {Original(TEXT("btn_back"),FVector2D(25,17),FVector2D(65,75),1,Layer);};
  if(Owner->bHelp)
  {
   Box(FVector2D::ZeroVector,FVector2D(1600,900),Alpha(Black,.9f),L+40);
   Original(TEXT("act54side_guide_1"),FVector2D(350,0),FVector2D(900,900),1,L+41);
   BackMark(L+42);return L+43;
  }
  const int32 PageBaseLayer=L+100;
  auto DrawSourcePage=[&]() -> int32
  {
   const int32 L=PageBaseLayer;
   const bool Intro=Owner->bTitleIntro,Gallery=Owner->bGallery,Judge=Owner->bJudge;
   const float PageClock=Intro?Owner->IntroTime:Owner->PageTime;
   const FString PageKey=Intro?TEXT("intro"):Judge?TEXT("judge"):Gallery?TEXT("gallery"):TEXT("detail");
   const auto Page=Owner->SourcePages->GetObjectField(PageKey);
   const auto& Elements=Page->GetArrayField(TEXT("elements"));
   const TArray<TSharedPtr<FJsonValue>>* Rows=&Page->GetArrayField(TEXT("rest"));
   const TArray<TSharedPtr<FJsonValue>>* NextRows=Rows;float Mix=0;
   const auto Animations=Page->GetObjectField(TEXT("animations"));
   const FString Entry=Intro?TEXT("act54side_entry_anim"):!Owner->PageAnimation.IsEmpty()?Owner->PageAnimation:Judge?TEXT("act54side_divination_start_tips_in"):Gallery?TEXT("act54side_gallery_entry"):TEXT("act54side_ divination_info_dialog_in");
   const TSharedPtr<FJsonObject>* Animation=nullptr;
   if(Animations->TryGetObjectField(Entry,Animation))
   {
    const float Duration=(*Animation)->GetNumberField(TEXT("duration"));const auto& Frames=(*Animation)->GetArrayField(TEXT("frames"));
    if((PageClock<Duration||!Owner->PageAnimation.IsEmpty())&&!Frames.IsEmpty())
    {
     const float Frame=Unit(PageClock/Duration)*(Frames.Num()-1);const int32 I=int32(Frame);Mix=Frame-I;
     Rows=&Frames[I]->AsArray();NextRows=&Frames[FMath::Min(I+1,Frames.Num()-1)]->AsArray();
    }
   }
   const int32 CardIndex=Gallery?Owner->GalleryCard:Owner->DetailSlot!=INDEX_NONE?Owner->Tarot.CurrentCards()[Owner->DetailSlot].Card:0;
   const auto& Card=Owner->Tarot.Cards[CardIndex];
   const bool Reverse=Gallery?Owner->bGalleryReverse:Owner->DetailSlot!=INDEX_NONE&&Owner->Tarot.CurrentCards()[Owner->DetailSlot].bReversed;
   if(Gallery)
   {
    const auto* B=Owner->PageEffectBrushes.Find(TEXT("gallery/pane_bg/group_water_circle/bg_underwater"));
    if(B)if(auto* MID=Cast<UMaterialInstanceDynamic>(B->GetResourceObject()))for(int32 J=0;J<Elements.Num();++J)
    {
     const FString Path=Elements[J]->AsObject()->GetStringField(TEXT("path"));FString Param;
     if(Path.EndsWith(TEXT("/bg_underwater")))Param=TEXT("MaskSelf");
     else if(Path.EndsWith(TEXT("/water_circle")))Param=TEXT("MaskCircle");
     else if(Path.EndsWith(TEXT("/text_tarot_set")))Param=TEXT("MaskTarot");
     else if(Path.EndsWith(TEXT("/text_card_set")))Param=TEXT("MaskCard");
     if(Param.IsEmpty())continue;
     const auto& Row=(*Rows)[J]->AsArray();const auto& Next=(*NextRows)[J]->AsArray();
     auto N=[&](int32 K){return FMath::Lerp(Row[K]->AsNumber(),Next[K]->AsNumber(),double(Mix));};
     MID->SetVectorParameterValue(FName(*(Param+TEXT("Rect"))),FLinearColor(N(0),N(1),N(2),N(3)));
     MID->SetScalarParameterValue(FName(*(Param+TEXT("Angle"))),N(6));
     if(Param==TEXT("MaskCircle"))MID->SetVectorParameterValue(TEXT("MaskFlowVertex"),FLinearColor(N(9),N(10),N(11),N(12)));
    }
   }
   // The original detail color plane reads the five-vertex Shape1 stencil.
   TArray<FVector2f> DetailBoundary;float DetailAlpha=1;
   if(!Intro&&!Gallery&&!Judge)
   {
    const auto* Shape=Owner->PageEffectMeshes.Find(TEXT("detail/container/group_bg/part_bg/water"));
    for(int32 J=0;Shape&&J<Elements.Num();++J)if(Elements[J]->AsObject()->GetStringField(TEXT("path")).EndsWith(TEXT("part_bg/water")))
    {
     const auto& Row=(*Rows)[J]->AsArray();const auto& Next=(*NextRows)[J]->AsArray();
     auto N=[&](int32 K){return FMath::Lerp(Row[K]->AsNumber(),Next[K]->AsNumber(),double(Mix));};
     DetailAlpha=N(12);const float R=FMath::DegreesToRadians(N(6));
     for(int32 K:{0,3,4,1,2}){FVector2D V=FVector2D(Shape->Positions[K])*N(2);V=FVector2D(N(0),N(1))+FVector2D(V.X*FMath::Cos(R)-V.Y*FMath::Sin(R),V.X*FMath::Sin(R)+V.Y*FMath::Cos(R));DetailBoundary.Add(TransformPoint(G.GetAccumulatedRenderTransform(),FVector2f(V)));}
     break;
    }
   }
   TMap<int32,TArray<FVector2f>> StencilMasks;
   for(int32 I=0;I<Elements.Num();++I)
   {
    auto J=Elements[I]->AsObject();const FString Path=J->GetStringField(TEXT("path"));
    const auto& Now=(*Rows)[I]->AsArray();const auto& Next=(*NextRows)[I]->AsArray();
    auto V=[&](int32 K){return float(FMath::Lerp(Now[K]->AsNumber(),Next[K]->AsNumber(),double(Mix)));};
    // Packed row: position 0-1, size 2-3, rotation 4-6, pivot 7-8, RGBA 9-12, active 13.
    if(Now.Num()!=14||Next.Num()!=14)continue;
    if(V(13)<.5f||V(12)<.001f)continue;
    if(Intro&&Path==TEXT("group_title/title_ch_copy"))continue;
    if((Path.Contains(TEXT("pos_icon_reversed"))||Path.EndsWith(TEXT("/pos_reversed"))||Path.EndsWith(TEXT("/group_position/reversed")))&&!Reverse)continue;
    if((Path.Contains(TEXT("pos_icon_upright"))||Path.EndsWith(TEXT("/pos_upright"))||Path.EndsWith(TEXT("/group_position/upright")))&&Reverse)continue;
    // Keep the six source drawings, but interpolate every display frame instead of hard-cutting at 12 Hz.
    float WaterWeight=1;
    if(Path.Contains(TEXT("water_draw_frames/f")))
    {
     const float WaterPhase=FMath::Fmod(FMath::Max(0.f,Owner->bPageFrozen?Owner->PageTime:P.AmbientTime)*12,6.f);
     const int32 Current=int32(WaterPhase),NextFrame=(Current+1)%6,Frame=FCString::Atoi(*Path.Right(2))-1;
     const float Blend=WaterPhase-Current;
     WaterWeight=Frame==Current?1-Blend:Frame==NextFrame?Blend:0;
     if(WaterWeight<=0)continue;
     // Compensate the under-layer's alpha so overlapping water does not pulse in opacity.
     const int32 TopFrame=FMath::Max(Current,NextFrame);
     if(Frame!=TopFrame){const float TopWeight=TopFrame==Current?1-Blend:Blend;WaterWeight/=FMath::Max(.001f,1-V(12)*A*TopWeight);}
    }
    const FVector2D Pivot(V(0),V(1)),Size(V(2),V(3)),UVPivot(V(7),V(8));
    const FVector2D Pos=Pivot-Size*UVPivot;
    const FLinearColor Tint(V(9),V(10),V(11),V(12)*WaterWeight);const int32 Layer=L+3+I;
    const FLinearColor PaintTint=Intro&&Path.Contains(TEXT("group_title/title_ch"))?White:FLinearColor::FromSRGBColor(Tint.ToFColor(false));
    if(Path.EndsWith(TEXT("part_bg/color/color_round"))&&DetailBoundary.Num()==5)
    {
     // Source part_bg/bg is a material-free UIMesh, distinct from the stencil-only color/mask.
     TArray<FSlateVertex> Vertices;TArray<SlateIndex> Indices;
     for(auto Point:DetailBoundary)Vertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(FSlateRenderTransform(),Point,FVector2f(.5,.5),FColor(1,6,105,FMath::RoundToInt(186*DetailAlpha*A))));
     for(int32 K=1;K<4;++K)Indices.Append({0,SlateIndex(K),SlateIndex(K+1)});
     FSlateDrawElement::MakeCustomVerts(Out,Layer,FSlateApplication::Get().GetRenderer()->GetResourceHandle(*Brush),Vertices,Indices,nullptr,0,0);
    }

    TSharedPtr<FJsonObject> Props=MakeShared<FJsonObject>();
    if(Intro&&Animation&&(*Animation)->HasField(TEXT("properties")))
    {
     const auto& Frames=(*Animation)->GetArrayField(TEXT("properties"));const float F=Unit(PageClock/6.5f)*(Frames.Num()-1);const int32 K=int32(F);const auto& P0=Frames[K]->AsArray()[I]->AsObject();const auto& P1=Frames[FMath::Min(K+1,Frames.Num()-1)]->AsArray()[I]->AsObject();
     for(const auto& Pair:P0->Values){double End=Pair.Value->AsNumber();P1->TryGetNumberField(Pair.Key,End);Props->SetNumberField(Pair.Key,FMath::Lerp(Pair.Value->AsNumber(),End,double(F-K)));}
    }
    auto Property=[&](const TCHAR* Name,float Default){double Value=Default;Props->TryGetNumberField(Name,Value);return float(Value);};
    auto ScreenPoint=[&](FVector2D Local){const float R=FMath::DegreesToRadians(V(6));return TransformPoint(G.GetAccumulatedRenderTransform(),FVector2f(Pivot+FVector2D(Local.X*FMath::Cos(R)-Local.Y*FMath::Sin(R),Local.X*FMath::Sin(R)+Local.Y*FMath::Cos(R))));};
    auto ApplyStencil=[&](TArray<FSlateVertex>& Vertices,TArray<SlateIndex>& Indices)
    {
     if(!Intro||!J->HasField(TEXT("stencil")))return;
     for(const auto& Value:J->GetArrayField(TEXT("stencil")))
     {
      auto Op=Value->AsObject();const int32 Channel=Op->GetIntegerField(TEXT("_readChannel")),Compare=Op->GetIntegerField(TEXT("_comp"));
      if(const auto* Boundary=StencilMasks.Find(Channel))
      {if(Compare==1)ClipConvex(Vertices,Indices,*Boundary);else if(Compare==2)ClipOutside(Vertices,Indices,*Boundary);}
     }
    };
    bool ColorWrite=true;
    if(Intro&&J->HasField(TEXT("stencil")))for(const auto& Value:J->GetArrayField(TEXT("stencil")))
    {
     auto Op=Value->AsObject();ColorWrite=(Op->GetIntegerField(TEXT("_colorMask"))&14)!=0;const int32 Channel=Op->GetIntegerField(TEXT("_writeChannel"));
     if(Channel>0)
     {
      TArray<FVector2f> Boundary;
      if(J->HasField(TEXT("sourceShapeCircle")))
      {
       const auto Shape=J->GetObjectField(TEXT("sourceShapeCircle"));float Radius=Shape->GetNumberField(TEXT("radius"));
       if(J->HasField(TEXT("floatBindings")))for(const auto& Pair:J->GetObjectField(TEXT("floatBindings"))->Values)if(Pair.Value->AsString()==TEXT("_OutStep"))Radius=Property(*Pair.Key,Radius);
       for(int32 K=0;K<64;++K){const float R=K*UE_TWO_PI/64;Boundary.Add(ScreenPoint(FVector2D(FMath::Cos(R)*Size.X,FMath::Sin(R)*Size.Y)*Radius));}
      }
      else for(FVector2D UV:{FVector2D(0,0),FVector2D(1,0),FVector2D(1,1),FVector2D(0,1)})Boundary.Add(ScreenPoint((UV-UVPivot)*Size));
      StencilMasks.Add(Channel,MoveTemp(Boundary));
     }
    }
    if(!ColorWrite)continue;
    FString Art=J->GetStringField(TEXT("art")),Value=J->GetStringField(TEXT("text"));
    if(!Intro&&(Path.EndsWith(TEXT("text_tarot_name"))||Path.EndsWith(TEXT("text_title"))))Value=Card.Name;
    if(Path.EndsWith(TEXT("text_char_name"))||Path.EndsWith(TEXT("text_charaname")))Value=Card.Character;
    if(Path.EndsWith(TEXT("text_tarot_desc"))||Path.EndsWith(TEXT("/text_info")))Value=Reverse?Card.Reversed:Card.Upright;
    if(Path.EndsWith(TEXT("text_info_01")))Value=TEXT("新的问题将得到新的回答\n确定占卜吗？");
    if(Gallery&&Path.EndsWith(TEXT("text_tips_desc")))continue; // Drawn with the resolved tips layout below.
    if(Path.EndsWith(TEXT("tarot_num_img")))Art=FString::Printf(TEXT("tarot_%d_num"),CardIndex+1);
    if(Path.EndsWith(TEXT("tarot_card_rotate"))||Path.EndsWith(TEXT("card_part/pos/rotation/root")))
    {
     const float Flip=Ease(Owner->GalleryFlip/.666667f);const float Turn=Gallery?(Reverse?180*Flip:180*(1-Flip)):0;
     if(!Gallery&&Owner->DetailGeometry.IsValid())
     {
      auto Clip=Owner->DetailGeometry->GetArrayField(TEXT("clips"))[0]->AsObject();
      for(const auto& Candidate:Owner->DetailGeometry->GetArrayField(TEXT("clips"))){auto C=Candidate->AsObject();if(C->GetStringField(TEXT("name"))==Entry&&(Owner->PageTime<C->GetNumberField(TEXT("duration"))||!Owner->PageAnimation.IsEmpty())){Clip=C;break;}}
      const auto& Frames=Clip->GetArrayField(TEXT("frames"));const float Duration=Clip->GetNumberField(TEXT("duration"));
      const float Frame=(Clip->GetStringField(TEXT("name"))==TEXT("card_loop")?FMath::Fmod(FMath::Max(0.f,Owner->PageTime),Duration)/Duration:Unit(Owner->PageTime/Duration))*(Frames.Num()-1);const int32 K=int32(Frame);const auto& C0=Frames[K]->AsObject()->GetArrayField(TEXT("corners"));const auto& C1=Frames[FMath::Min(K+1,Frames.Num()-1)]->AsObject()->GetArrayField(TEXT("corners"));
      FVector Corners[4];for(int32 CornerIndex=0;CornerIndex<4;++CornerIndex){const auto& Q=C0[CornerIndex]->AsArray();const auto& N=C1[CornerIndex]->AsArray();Corners[CornerIndex]=FVector(FMath::Lerp(Q[0]->AsNumber(),N[0]->AsNumber(),double(Frame-K)),FMath::Lerp(Q[1]->AsNumber(),N[1]->AsNumber(),double(Frame-K)),FMath::Lerp(Q[2]->AsNumber(),N[2]->AsNumber(),double(Frame-K)));}
      TArray<FSlateVertex> Vertices;TArray<SlateIndex> Indices;constexpr int32 NX=12,NY=18;
      for(int32 Y=0;Y<=NY;++Y)for(int32 X=0;X<=NX;++X)
      {
       FVector2f UV(float(X)/NX,float(Y)/NY);FVector Q=FMath::Lerp(FMath::Lerp(Corners[0],Corners[1],UV.X),FMath::Lerp(Corners[3],Corners[2],UV.X),UV.Y);
       const float Perspective=1400/FMath::Max(350.,1400+Q.Z);const FVector2D Location=FVector2D(800,450)+(FVector2D(Q.X,Q.Y)-FVector2D(800,450))*Perspective;
       if(Reverse)UV=FVector2f(1-UV.X,1-UV.Y);
       Vertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(G.GetAccumulatedRenderTransform(),FVector2f(Location),FVector4f(UV.X,UV.Y,1,1),UV,Alpha(White,V(12)*A).ToFColor(false)));
      }
      for(int32 Y=0;Y<NY;++Y)for(int32 X=0;X<NX;++X){SlateIndex VertexIndex=Y*(NX+1)+X;Indices.Append({VertexIndex,SlateIndex(VertexIndex+1),SlateIndex(VertexIndex+NX+2),VertexIndex,SlateIndex(VertexIndex+NX+2),SlateIndex(VertexIndex+NX+1)});}
      FSlateDrawElement::MakeCustomVerts(Out,Layer,FSlateApplication::Get().GetRenderer()->GetResourceHandle(Owner->CardBrushes[CardIndex]),Vertices,Indices,nullptr,0,0);
     }
     else Plane(Owner->CardBrushes[CardIndex],Pivot,FVector2D(340,516)*Size.X/100,FVector(V(4)+(Gallery?Owner->GalleryTilt.Y:0),V(5)+(Gallery?Owner->GalleryTilt.X:0),V(6)+Turn),FVector::ZeroVector,1,V(12),Layer,!Gallery&&Reverse);
    }
    else if(J->HasField(TEXT("effect"))&&Owner->PageEffectBrushes.Contains(FName(*J->GetStringField(TEXT("effect")))))
    {
     const FName Key(*J->GetStringField(TEXT("effect")));const auto& EffectBrush=Owner->PageEffectBrushes.FindChecked(Key);
     if(auto* MID=Cast<UMaterialInstanceDynamic>(EffectBrush.GetResourceObject()))
     {MID->SetVectorParameterValue(TEXT("VertexColor"),Key==TEXT("gallery/pane_bg/group_water_circle/water_circle")?PaintTint:Tint);if(Intro&&J->HasField(TEXT("floatBindings")))for(const auto& Pair:J->GetObjectField(TEXT("floatBindings"))->Values){double Number;if(Props->TryGetNumberField(Pair.Key,Number))MID->SetScalarParameterValue(FName(*Pair.Value->AsString()),Number);}}
     const auto* Mesh=Owner->PageEffectMeshes.Find(Key);TArray<FSlateVertex> Vertices;TArray<SlateIndex> Indices;
     const float Angle=FMath::DegreesToRadians(V(6)),Co=FMath::Cos(Angle),Si=FMath::Sin(Angle);
     auto Vertex=[&](FVector2f P,FVector2f UV)
     {
      const FVector2D Location=Pivot+FVector2D(P.X*Co-P.Y*Si,P.X*Si+P.Y*Co);
      Vertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(G.GetAccumulatedRenderTransform(),FVector2f(Location),FVector4f(UV.X,UV.Y,1,1),UV,Alpha(White,A).ToFColor(false)));
     };
     if(Mesh)
     {
      for(int32 N=0;N<Mesh->Positions.Num();++N)Vertex(Mesh->Positions[N]*Size.X,Mesh->UVs[N]);
      Indices=Mesh->Indices;
     }
     else
     {
      for(FVector2f UV:{FVector2f(0,0),FVector2f(1,0),FVector2f(1,1),FVector2f(0,1)})Vertex(FVector2f((UV.X-UVPivot.X)*Size.X,(UV.Y-UVPivot.Y)*Size.Y),UV);
      Indices={0,1,2,0,2,3};
     }
     ApplyStencil(Vertices,Indices);
     if(Path.EndsWith(TEXT("color/color_round"))&&DetailBoundary.Num()==5)ClipConvex(Vertices,Indices,DetailBoundary);
     FSlateDrawElement::MakeCustomVerts(Out,Layer,FSlateApplication::Get().GetRenderer()->GetResourceHandle(EffectBrush),Vertices,Indices,nullptr,0,0);
    }
    else if(Intro&&Value.IsEmpty())
    {
     const FSlateBrush* B=Art.IsEmpty()?Brush:Owner->OriginalBrushes.Find(FName(*Art));
     if(B)
     {
      FVector2D Min(0,0),Max(1,1);
      if(J->HasField(TEXT("sourceGraphic")))
      {
       auto Graphic=J->GetObjectField(TEXT("sourceGraphic"));if(Graphic->GetIntegerField(TEXT("m_Type"))==3)
       {const float Fill=Property(TEXT("m_FillAmount"),Graphic->GetNumberField(TEXT("m_FillAmount")));const int32 Method=Graphic->GetIntegerField(TEXT("m_FillMethod")),Origin=Property(TEXT("m_FillOrigin"),Graphic->GetIntegerField(TEXT("m_FillOrigin")));if(Method==0){if(Origin==0)Max.X=Fill;else Min.X=1-Fill;}else if(Method==1){if(Origin==0)Min.Y=1-Fill;else Max.Y=Fill;}else if(Fill<.001f)Max=Min;}
      }
      TArray<FSlateVertex> Vertices;TArray<SlateIndex> Indices={0,1,2,0,2,3};
      for(FVector2D UV:{Min,FVector2D(Max.X,Min.Y),Max,FVector2D(Min.X,Max.Y)})Vertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(FSlateRenderTransform(),ScreenPoint((UV-UVPivot)*Size),FVector2f(UV),Alpha(Tint,A).ToFColor(false)));
      ApplyStencil(Vertices,Indices);FSlateDrawElement::MakeCustomVerts(Out,Layer,FSlateApplication::Get().GetRenderer()->GetResourceHandle(*B),Vertices,Indices,nullptr,0,0);
     }
    }
    else if(Judge&&Path.EndsWith(TEXT("clock/image")))
    {
     // Source clock sits inside RectMask2D, including its authored 920 x 73 softness.
     FVector2D MaskCenter(804.5,481.5),MaskSize(1328.75,396);
     for(int32 ElementIndex=0;ElementIndex<Elements.Num();++ElementIndex)if(Elements[ElementIndex]->AsObject()->GetStringField(TEXT("path")).EndsWith(TEXT("group_bg/bg_in")))
     {const auto& R=(*Rows)[ElementIndex]->AsArray();const auto& N=(*NextRows)[ElementIndex]->AsArray();auto Q=[&](int32 K){return FMath::Lerp(R[K]->AsNumber(),N[K]->AsNumber(),double(Mix));};MaskCenter=FVector2D(Q(0),Q(1));MaskSize=FVector2D(Q(2),Q(3)*316.8/416);break;}
     if(const auto* B=Owner->OriginalBrushes.Find(FName(*Art)))
     {
      TArray<FSlateVertex> Vertices;TArray<SlateIndex> Indices;constexpr int32 Grid=32;
      const float R=FMath::DegreesToRadians(V(6));
      for(int32 Y=0;Y<=Grid;++Y)for(int32 X=0;X<=Grid;++X)
      {
       const FVector2f UV(float(X)/Grid,float(Y)/Grid);const FVector2D Pnt=(FVector2D(UV)-UVPivot)*Size;
       const FVector2D Location=Pivot+FVector2D(Pnt.X*FMath::Cos(R)-Pnt.Y*FMath::Sin(R),Pnt.X*FMath::Sin(R)+Pnt.Y*FMath::Cos(R));
       const FVector2D D=Location-MaskCenter,Distance=MaskSize*.5-FVector2D(FMath::Abs(D.X),FMath::Abs(D.Y));
       const float Fade=Unit(Distance.X*2/(1150+2))*Unit(Distance.Y*2/(91.25+2));
       Vertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(G.GetAccumulatedRenderTransform(),FVector2f(Location),FVector4f(UV.X,UV.Y,1,1),UV,Alpha(Tint,Fade*A).ToFColor(false)));
      }
      for(int32 Y=0;Y<Grid;++Y)for(int32 X=0;X<Grid;++X){SlateIndex K=Y*(Grid+1)+X;Indices.Append({K,SlateIndex(K+1),SlateIndex(K+Grid+2),K,SlateIndex(K+Grid+2),SlateIndex(K+Grid+1)});}
      FSlateDrawElement::MakeCustomVerts(Out,Layer,FSlateApplication::Get().GetRenderer()->GetResourceHandle(*B),Vertices,Indices,nullptr,0,0);
     }
    }
    else if(Path.EndsWith(TEXT("group_tips/tips_back")))
    {
     // Source Image is sliced: 84 x 32, border (left=41, right=10).
     // Its layout group fits the label with 45 / 12 px padding and centers it vertically.
     constexpr float SourceScale=1.25f;
     const FString Label=TEXT("完成战斗关卡，获取随机塔罗牌");
     const auto Font=FSlateFontInfo(Owner->OriginalFonts.FindRef(TEXT("Medium")),18.f*SourceScale*.75f);
     const auto Measure=FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
     const FVector2D TextSize=Measure->Measure(Label,Font);
     const float Width=TextSize.X+(45+12)*SourceScale;
     const FVector2D TopLeft=Pivot-FVector2D(Width,Size.Y*.5f);
     if(const auto* B=Owner->OriginalBrushes.Find(FName(*Art)))
     {
      const float Xs[]={0,41*SourceScale,Width-10*SourceScale,Width};
      const float Us[]={0,41.f/84,74.f/84,1};
      TArray<FSlateVertex> Vertices;TArray<SlateIndex> Indices;
      for(int32 Y=0;Y<2;++Y)for(int32 X=0;X<4;++X)
      {
       const FVector2f UV(Us[X],float(Y));
       Vertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(G.GetAccumulatedRenderTransform(),FVector2f(TopLeft+FVector2D(Xs[X],Y*Size.Y)),FVector4f(UV.X,UV.Y,1,1),UV,Alpha(PaintTint,A).ToFColor(false)));
      }
      for(int32 X=0;X<3;++X)Indices.Append({SlateIndex(X),SlateIndex(X+1),SlateIndex(X+5),SlateIndex(X),SlateIndex(X+5),SlateIndex(X+4)});
      // Raw UI imports may already hold display-space RGB; avoid a second gamma conversion.
      const auto* Texture=Cast<UTexture2D>(B->GetResourceObject());
      const auto Effects=Texture&&!Texture->SRGB?ESlateDrawEffect::NoGamma:ESlateDrawEffect::None;
      FSlateDrawElement::MakeCustomVerts(Out,L+95,FSlateApplication::Get().GetRenderer()->GetResourceHandle(*B),Vertices,Indices,nullptr,0,0,Effects);
     }
     const FVector2D LabelPos(TopLeft.X+45*SourceScale,Pivot.Y-TextSize.Y*.5f-18.f*SourceScale*.25f);
     const auto LabelTint=FLinearColor::FromSRGBColor(FColor(188,188,188));
     FSlateDrawElement::MakeText(Out,L+96,G.ToPaintGeometry(TextSize,FSlateLayoutTransform(LabelPos)),Label,Font,ESlateDrawEffect::None,Alpha(LabelTint,V(12)*A));
    }
    else if(!Art.IsEmpty())OriginalTurn(FName(*Art),Pivot,Size,UVPivot,V(6),1,1,Layer,PaintTint);
    else if(Path.EndsWith(TEXT("pane_bg/back")))Box(Pos,Size,PaintTint,Layer);
    if(!Value.IsEmpty())
    {
     const FName FontKey=J->GetStringField(TEXT("font")).Contains(TEXT("Heavy"))?TEXT("Heavy"):TEXT("Medium");
     auto Face=Owner->OriginalFonts.FindRef(FontKey);const float FontSize=J->GetNumberField(TEXT("fontSize"))*1.25f;
     FSlateFontInfo Font=Face.IsValid()?FSlateFontInfo(Face,FontSize*.75f):FCoreStyle::GetDefaultFontStyle("Bold",FontSize*.75f);
     const auto Measure=FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
     const float Width=Size.X>1?Size.X:600;const int32 Align=J->GetIntegerField(TEXT("align"));
     TArray<FString> Lines;FString Current;
     for(TCHAR Ch:Value)
     {
      if(Ch=='\r')continue;
      if(Ch=='\n'){Lines.Add(Current);Current.Empty();continue;}
      if(!Current.IsEmpty()&&Measure->Measure(Current+FString::Chr(Ch),Font).X>Width){Lines.Add(Current);Current.Empty();}
      Current+=Ch;
     }
     if(!Current.IsEmpty())Lines.Add(Current);
     float Y=Align>=3?FMath::Max(0.f,(Size.Y-Lines.Num()*FontSize*1.35f)*.5f):0;
     for(const auto& LineText:Lines)
     {
      const float W=Measure->Measure(LineText,Font).X;float X=Align%3==1?(Width-W)*.5f:Align%3==2?Width-W:0;
      if(Size.X<1)X=-W;
      const FVector2D Local(X-Size.X*UVPivot.X,Y-Size.Y*UVPivot.Y-FontSize*.5f);
      const FShear2D Shear(J->GetBoolField(TEXT("italic"))?-.16f:0,0);
      const auto Transform=FSlateRenderTransform(Concatenate(Shear,Local,FQuat2D(FMath::DegreesToRadians(V(6))),Pivot));
      FSlateDrawElement::MakeText(Out,Intro&&Path.Contains(TEXT("group_title/title_ch"))?L+Elements.Num()+5:Layer,G.MakeChild(FVector2D(1600,900),FSlateLayoutTransform(),Transform,FVector2D::ZeroVector).ToPaintGeometry(),LineText,Font,ESlateDrawEffect::None,Alpha(PaintTint,A));
      Y+=FontSize*1.35f;
     }
    }
   }
   if(Gallery)
   {
    // The authored Bezier cards cross the screen midpoint; clip only at the full canvas.
    Out.PushClip(FSlateClippingZone(G.GetLayoutBoundingRect()));
    for(int32 I=0;I<8;++I)
    {
     const auto Center=Owner->GalleryPosition(I,Owner->GalleryScroll);const float U=Unit(.331f+.16f*(I-Owner->GalleryScroll));
     if(Center.Y<-280||Center.Y>1180)continue;
     auto ItemPoint=[&](FVector Point){return GalleryProject(Center,U,FVector2D(Point.X,Point.Y));};
     auto ItemPlane=[&](const FSlateBrush& CardArt,FVector2D CardSize,float Opacity,int32 CardLayer,bool Sliced=false)
     {
      TArray<FSlateVertex> Vertices;TArray<SlateIndex> Indices;const int32 NX=Sliced?3:12,NY=Sliced?3:18;
      for(int32 Y=0;Y<=NY;++Y)for(int32 X=0;X<=NX;++X){FVector2f UV(float(X)/NX,float(Y)/NY);FVector2D Local(UV);
       if(Sliced){const float PX[]={0,42.5f/float(CardSize.X),1-45.f/float(CardSize.X),1},PY[]={0,45.f/float(CardSize.Y),1-42.5f/float(CardSize.Y),1};const float UX[]={0,34.f/132,96.f/132,1},UY[]={0,36.f/108,74.f/108,1};Local=FVector2D(PX[X],PY[Y]);UV=FVector2f(UX[X],UY[Y]);}
       const auto Location=GalleryProject(Center,U,(Local-FVector2D(.5,.5))*CardSize);Vertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(G.GetAccumulatedRenderTransform(),FVector2f(Location),FVector4f(UV.X,UV.Y,1,1),UV,Alpha(White,Opacity*A).ToFColor(false)));}
      for(int32 Y=0;Y<NY;++Y)for(int32 X=0;X<NX;++X){SlateIndex K=Y*(NX+1)+X;Indices.Append({K,SlateIndex(K+1),SlateIndex(K+NX+2),K,SlateIndex(K+NX+2),SlateIndex(K+NX+1)});}
      FSlateDrawElement::MakeCustomVerts(Out,CardLayer,FSlateApplication::Get().GetRenderer()->GetResourceHandle(CardArt),Vertices,Indices,nullptr,0,0);
     };
     if(I==CardIndex)if(const auto* Glow=Owner->OriginalBrushes.Find(TEXT("ui_card_seleted_light")))ItemPlane(*Glow,FVector2D(365.75,529.75),.8f,L+80,true);
     ItemPlane(Owner->CardBrushes[I],FVector2D(318.75,483.75),1,L+81+I);
     const auto NameCenter=ItemPoint(FVector(0,-290.75,0));const auto NameRight=ItemPoint(FVector(1,-290.75,0))-NameCenter;
     const float NameAngle=FMath::RadiansToDegrees(FMath::Atan2(NameRight.Y,NameRight.X));
     auto NameFont=FSlateFontInfo(Owner->OriginalFonts.FindRef(TEXT("Medium")),22.5f);
     const float NameWidth=FSlateApplication::Get().GetRenderer()->GetFontMeasureService()->Measure(Owner->Tarot.Cards[I].Name,NameFont).X;
     const FVector2D NameOrigin=NameCenter-FVector2D(FMath::Cos(FMath::DegreesToRadians(NameAngle)),FMath::Sin(FMath::DegreesToRadians(NameAngle)))*NameWidth*.5;
     Text(Owner->Tarot.Cards[I].Name,NameOrigin+FVector2D(0,3),30,Black,L+90,NameAngle);
     Text(Owner->Tarot.Cards[I].Name,NameOrigin,30,White,L+91,NameAngle);
    }
    Out.PopClip();
   }
   return L+Elements.Num()+100;
  };
  const bool HasSourcePage=Owner->Tarot.IsReady()&&Owner->SourcePages.IsValid();
  if(HasSourcePage&&(Owner->bGallery||Owner->bTitleIntro))return DrawSourcePage();

  if(Selection)
  {
   // Selection has the original bright cobalt backdrop, distinct from the dark question stage.
   TArray<FSlateVertex> Backdrop;
   const FVector2D Corners[]={{0,0},{1600,0},{1600,900},{0,900}};
   const FLinearColor Colors[]={{.09f,.10f,.42f},{.20f,.36f,.90f},{.025f,.025f,.20f},{.025f,.025f,.20f}};
   const FLinearColor NormalColors[]={{.02f,.18f,.8f},{.25f,.91f,1.f},{.13f,.19f,.85f},{.02f,.045f,.66f}};
   for(int32 I=0;I<4;++I)Backdrop.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(G.GetAccumulatedRenderTransform(),FVector2f(Corners[I]),FVector2f(.5,.5),Alpha(Owner->Tarot.Selected==5?Colors[I]:NormalColors[I],A).ToFColor(false)));
   FSlateDrawElement::MakeCustomVerts(Out,L+2,FSlateApplication::Get().GetRenderer()->GetResourceHandle(*Brush),Backdrop,TArray<SlateIndex>{0,1,2,0,2,3},nullptr,0,0);
   Original(TEXT("water_1"),FVector2D(800+FMath::Sin(P.AmbientTime*.2f)*15,-32),FVector2D(1205,305),.3f,L+3,Blue);
   if(const auto* Frames=Owner->WhitePanels.Find(Owner->Tarot.Selected==5?TEXT("special"):TEXT("normal"));Frames&&!Frames->IsEmpty())
   {
    const float Frame=Unit(Owner->HeroTime/.5f)*(Frames->Num()-1);const int32 K=int32(Frame);const auto& M=(*Frames)[K];const auto& N=(*Frames)[FMath::Min(K+1,Frames->Num()-1)];TArray<FSlateVertex> Vertices;
    for(int32 J=0;J<M.Positions.Num();++J){const auto V=FMath::Lerp(M.Positions[J],N.Positions[J],Frame-K);Vertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(G.GetAccumulatedRenderTransform(),V,M.UVs[J],Alpha(White,A).ToFColor(false)));}
    FSlateDrawElement::MakeCustomVerts(Out,L+4,FSlateApplication::Get().GetRenderer()->GetResourceHandle(*Brush),Vertices,M.Indices,nullptr,0,0);
   }
   Original(TEXT("bg_tarot"),FVector2D(32.5,543.75),FVector2D(875,425),1,L+5);
   DrawHero(L+6);
   const FName Words[]={TEXT("the_flow"),TEXT("the_gun"),TEXT("the_crepe"),TEXT("the_gate"),TEXT("the_halo")};
   const FVector2D Positions[]={{1275,190},{1325,290},{1225,365},{1230,465},{1275,555}};
   const FVector2D Sizes[]={{310,140},{260,110},{385,125},{325,90},{305,90}};
   for(int32 I=0;I<5;++I)
   {
    const auto Pnt=Positions[I];const bool Selected=Owner->Tarot.Selected==I;
    if(!Selected&&Owner->HoveredTab==I)Box(Pnt,FVector2D(335,76),Alpha(White,.12f),L+7);
    if(Selected)
    {
     const float Enter=Ease(Owner->TabTime/.25f),Y=Pnt.Y+(1-Enter)*25;
     Poly({{Pnt.X-94,Y+148},{1607,Y+12},{1566,Y+148}},Cyan,L+7);
     Poly({{Pnt.X-90,Y+147},{1610,Y+4},{1550,Y+126}},White,L+8);
    }
    Original(Words[I],Pnt,Sizes[I],1,L+9,Selected?FLinearColor(.95f,.025f,.055f):FLinearColor(.08f,.57f,.72f));
    if(Selected)Original(FName(*(TEXT("sub_")+Words[I].ToString())),Pnt+FVector2D(150,80),FVector2D(115,45),1,L+10);
   }
   Original(TEXT("back_special"),FVector2D(1200,700),FVector2D(415,135),1,L+9,Cyan);
   BackMark(L+8);
   Original(TEXT("btn_tips"),FVector2D(112,20),FVector2D(45,64),1,L+9);
   Original(TEXT("back_gallery"),FVector2D(1377,17),FVector2D(190,75),1,L+9);
   Original(TEXT("icon_gallery"),FVector2D(1499,27),FVector2D(50,50),1,L+10,Cyan);
   if(Owner->Tarot.Selected==5)
   {
    Original(TEXT("special_text_back_selected"),FVector2D(1185,812),FVector2D(460,50),1,L+30);
    Original(TEXT("selected_triangle"),FVector2D(1227,821),FVector2D(25,25),1,L+31,Black);
    Original(TEXT("img_text_special"),FVector2D(1262,818),FVector2D(105,30),1,L+31,Black);
    Original(TEXT("sp_text_selected"),FVector2D(1233,869),FVector2D(80,15),1,L+31);
    Box(FVector2D(1323,875),FVector2D(277,2),White,L+31);
   }
   else {Original(TEXT("special_text_back"),FVector2D(1268,812),FVector2D(350,50),1,L+31);Original(TEXT("img_text_special"),FVector2D(1350,820),FVector2D(105,30),1,L+32);}
   if(Owner->Tarot.Selected==5||Owner->Tarot.CanDivine())
   {
   // The source Selectable tints five graphics together over 0.1 seconds.
   const float ButtonScale=1;
   const FLinearColor ButtonTint(Owner->RevealTint,Owner->RevealTint,Owner->RevealTint,1);
   const FVector2D ButtonOrigin(121.85125,782.5);
   auto ButtonPoint=[&](FVector2D Offset){return ButtonOrigin+Offset*ButtonScale;};
   if(Owner->Tarot.CanDivine())
   {
   OriginalTurn(TEXT("clock_graph"),ButtonPoint(FVector2D(-36.125,40.75)),FVector2D(280,285),FVector2D(.5,.5),0,ButtonScale,1,L+32,ButtonTint);
   OriginalTurn(TEXT("pointer_static"),ButtonPoint(FVector2D(34.125,-17.375)),FVector2D(197.276,152.185),FVector2D(.5,.5),0,ButtonScale,1,L+33,ButtonTint);
   const FVector2D HandPivot=ButtonPoint(FVector2D(-38.25,36));
   OriginalTurn(TEXT("pointer_hour"),HandPivot,FVector2D(25,150),FVector2D(.5,.94772),-23.21f+P.AmbientTime*3.75f,ButtonScale,1,L+34,FLinearColor(.4f,.607843f,.992157f)*ButtonTint);
   OriginalTurn(TEXT("pointer_minute"),HandPivot,FVector2D(20,170),FVector2D(.5,.955),73.28f+P.AmbientTime*45,ButtonScale,1,L+34,FLinearColor(.4f,.607843f,.992157f)*ButtonTint);
   OriginalTurn(Owner->Tarot.CanDivine()?TEXT("icon_show_start"):TEXT("unable"),ButtonPoint(FVector2D(-25.375,29.99875)),FVector2D(170,175),FVector2D(.5,.5),0,ButtonScale,1,L+35,ButtonTint);
   }
   else OriginalTurn(TEXT("unable"),FVector2D(85.7263,823.25),FVector2D(280,285),FVector2D(.5,.5),0,1,1,L+35);
   if(Owner->StarConfig.IsValid()&&Owner->Tarot.CanDivine())
   {
    const auto* StarBrush=Owner->OriginalBrushes.Find(TEXT("announce_star"));
    auto Curve=[&](const TCHAR* Name,float Age)
    {
     const auto& Keys=Owner->StarConfig->GetArrayField(Name);auto AKey=Keys[0]->AsObject();
     for(int32 K=1;K<Keys.Num();++K)
     {
      auto BKey=Keys[K]->AsObject();const float TA=AKey->GetNumberField(TEXT("time")),TB=BKey->GetNumberField(TEXT("time"));
      if(Age<=TB)
      {
       const float U=Ramp(Age,TA,TB),VA=AKey->GetNumberField(TEXT("value")),VB=BKey->GetNumberField(TEXT("value"));
       if(AKey->HasField(TEXT("outSlope")))return FMath::CubicInterp(VA,float(AKey->GetNumberField(TEXT("outSlope")))*(TB-TA),VB,float(BKey->GetNumberField(TEXT("inSlope")))*(TB-TA),U);
       return FMath::Lerp(VA,VB,U);
      }
      AKey=BKey;
     }
     return float(AKey->GetNumberField(TEXT("value")));
    };
    // Prewarm and emission/lifetime/size/alpha match the original. Random realizations are local.
    const float ParticleTime=P.AmbientTime+5;const int32 Last=FMath::FloorToInt(ParticleTime*5);
    for(int32 I=Last-10;I<=Last&&StarBrush;++I)
    {
     FRandomStream Random(54401+I*7919);const float Life=Random.FRandRange(1,2),Age=ParticleTime-I*.2f;
     if(Age<0||Age>=Life)continue;
     const float U=Age/Life,Size=Random.FRandRange(.4f,.8f)*125*Curve(TEXT("sizeOverLifetimeKeys"),U);
     const float Opacity=Curve(TEXT("alphaOverLifetimeKeys"),U),Angle=Random.FRandRange(-.2640683f,.2687807f);
     const FVector2D Start(Random.FRandRange(-.5f,.5f)*125,Random.FRandRange(-.5f,.5f)*125);
     const FVector2D Velocity(Random.FRandRange(.09f,.4f)*125,-Random.FRandRange(.3f,.43f)*125);
     const FVector2D Noise(FMath::PerlinNoise2D(FVector2D(I*.71f,Age*.5f)),FMath::PerlinNoise2D(FVector2D(I*.37f+19,Age*.5f)));
     const FVector2D Center=ButtonOrigin+FVector2D(-28.75,30)+Start+Velocity*Age+Noise*12.5;
     const int32 Tile=Random.RandRange(0,1);TArray<FSlateVertex> Verts;
     for(FVector2f UV:{FVector2f(0,0),FVector2f(1,0),FVector2f(1,1),FVector2f(0,1)})
     {
      const FVector2D Local((UV.X-.5f)*Size,(UV.Y-.5f)*Size);
      const FVector2D Pos=Center+FVector2D(Local.X*FMath::Cos(Angle)-Local.Y*FMath::Sin(Angle),Local.X*FMath::Sin(Angle)+Local.Y*FMath::Cos(Angle));
      UV.X=(Tile+UV.X)*.5f;
      Verts.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(G.GetAccumulatedRenderTransform(),FVector2f(Pos),FVector4f(UV.X,UV.Y,1,1),UV,Alpha(White,Opacity*A).ToFColor(false)));
     }
     FSlateDrawElement::MakeCustomVerts(Out,L+36,FSlateApplication::Get().GetRenderer()->GetResourceHandle(*StarBrush),Verts,TArray<SlateIndex>{0,1,2,0,2,3},nullptr,0,0);
    }
   }
  }
   }
  if(!Selection)Original(TEXT("entry_bg"),FVector2D::ZeroVector,FVector2D(1600,900),Result?.32f*(1-Ramp(T,0,.567f)):.32f,L+2);
  const float CA=Selection?0:Result?1-Ramp(T,.4f,.567f):Ramp(E,0,.333f);
  if(CA>.001f)
  {
   const float Scale=Result?1+2.2f*FMath::Pow(Ramp(T,.017f,.567f),3):FMath::Lerp(.1f,1.f,Ease(E/1.833f));
   const float Spin=Result?Ramp(T,0,.567f)*Ramp(T,0,.567f)*1.8f:-(1-Ease(E/1.833f))*.9f;
   for(int32 I=0;I<Owner->ClockMeshes.Num();++I)
   {
    const auto& M=Owner->ClockMeshes[I];float Move=Spin;if(M.Name.StartsWith(TEXT("num_"))||M.Name==TEXT("clock_mesh"))Move+=.24f;
    if(M.Name==TEXT("secend"))Move+=P.AmbientTime*UE_TWO_PI/60;
    else if(M.Name==TEXT("minute"))Move+=P.AmbientTime*UE_TWO_PI/3600;
    else if(M.Name==TEXT("hour"))Move+=P.AmbientTime*UE_TWO_PI/43200;
    float Fade=CA;if(M.Name.StartsWith(TEXT("num_")))Fade*=Result?1-Ramp(T,.217f,.567f):Ramp(E,.065f+((I*7)%12)*.067f,.72f+((I*7)%12)*.067f);
    DrawMesh(M,nullptr,0,FVector2D(900,445),Scale,Move,Fade,L+4,false);
   }
   // Expanding diagonal clock spokes beneath the original perspective word sprites.
   Poly({{0,0},{185,0},{788,422}},Alpha(Blue,CA*.85f),L+3);
   Poly({{900,485},{1600,710},{1600,820}},Alpha(Blue,CA*.65f),L+3);
   Poly({{1090,0},{1188,0},{866,402}},Alpha(Blue,CA*.7f),L+3);
   Poly({{802,488},{420,900},{220,900}},Alpha(Blue,CA*.9f),L+3);
   Poly({{0,475},{0,605},{780,476}},Alpha(Black,CA*.7f),L+3);
   const float TA=Result?1-Ramp(T,.36f,.567f):Ramp(E,.2f,1.65f);
   const float Away=Result?Ease(Ramp(T,0,.567f))*550:0;
   Original(TEXT("text_01"),FVector2D(418-Away,270)+Parallax*23,FVector2D(505,255),TA,L+8);
   Original(TEXT("text_02"),FVector2D(775+Away,355)+Parallax*28,FVector2D(505,190),TA,L+8);
  }
  if((Result||Selection)&&Owner->Tarot.IsReady())
  {
   const auto& Spread=Owner->Tarot.CurrentSpread();const auto& Draws=Owner->Tarot.CurrentCards();
   const auto& Layout=Selection?Spread.Selection:Spread.Result;
   const float Fade=Selection?Ease(Owner->TabTime/.667f):Ramp(T,.267f,.783f);
   if(Result)
   {
    Original(TEXT("spread_shape_bg"),FVector2D::ZeroVector,FVector2D(1600,900),.5f*Fade,L+2,Black);
    Original(TEXT("water_1"),FVector2D(FMath::Sin(P.AmbientTime*.1f)*50-30,0),FVector2D(1680,340),.12f*Fade,L+3,Cyan);
    const float EnterX=(1-Ease(Ramp(T,.267f,1.05f)))*850;
    Original(Spread.TitleArt,FVector2D(200+EnterX,450),FVector2D(1530,616),Fade,L+9,Black);
    Text(Spread.Name,FVector2D(1045,681),57,Alpha(White,Ramp(T,.85f,1.3f)),L+25,-12);
    Line({{1025,773},{1630,643}},Alpha(White,Ramp(T,.7f,1.4f)),2,L+25);
   }
   for(int32 I=0;I<Draws.Num();++I)
   {
    const auto& D=Layout[I];const auto& Draw=Draws[I];const auto& Slot=Spread.Slots[I];
    const float Start=.35f+I*.066667f;const float U=Selection?Ease(Owner->TabTime/.667f):Ease(Ramp(T,Start,Start+.833333f));
    const float CF=Selection?Fade:Ramp(T,Start,Start+.32f);
    const FVector Initial=I?FVector(695,0,60):FVector(-80,480,-32);
    const FVector Offset=Selection?FVector((1-U)*-100,0,0):FVector(I?550:0,I?577:0,I?400:-862)*(1-U);
    const auto Tilt=Owner->BoardCardPoses.FindRef(I).Tilt;
    const FVector Degrees=D.Rotation+Initial*(Selection?0:1-U)+FVector(Parallax.Y*2+Tilt.Y,Parallax.X*3+Tilt.X,0);
    const float Scale=Selection?1:FMath::Lerp(I?.45199f:4.562f,1.f,U);
    if(Selection||T>Start+.65f)
    {
     const FVector2D Shadow=D.Position+FVector2D(14,17);
     Plane(Owner->ShadowBrush,Shadow,D.Size,Degrees,FVector::ZeroVector,1,CF*.2f,L+10);
    }
    Plane(Owner->CardBrushes[Draw.Card],D.Position,D.Size,Degrees,Offset,Scale,CF,L+11+I,Draw.bReversed);
    const float Label=Selection?Fade:Ramp(T,Start+.68f,Start+.70f);
    const FVector2D Base=D.Label;
    Box(Base,FVector2D(130,26),Alpha(Black,Label),L+22);Box(Base+FVector2D(0,29),FVector2D(165,22),Alpha(Black,Label),L+22);
    Text(Slot.Name,Base+FVector2D(7,0),22,Alpha(White,Label),L+23,-3);
    Text(Slot.English,Base+FVector2D(7,28),16,Alpha(White,Label*.68f),L+23,0,FVector2D(1,1),true);
    if(!Selection)for(int32 Row=0;Row<2;++Row)
    {
     const float Local=T-Start-.45f-Row*.13f;const float Fill=Local<.25f?Ease(Ramp(Local,0,.25f)):1-Ease(Ramp(Local,.25f,.42f));
     const float Width=Row?165:130;Box(Base+FVector2D(Local<.25f?0:Width*(1-Fill),Row*29),FVector2D(Width*Fill,Row?22:26),Black,L+24);
    }
   }
  }

  if(HasSourcePage&&(Owner->DetailSlot!=INDEX_NONE||Owner->bJudge))return DrawSourcePage();
  if(!Selection)Text(TEXT(">> 点击以继续"),FVector2D(1390,48),17,Alpha(White,.55f),L+26);
  return L+38;
 }
};

void SPadmaDivinationWidget::Construct(const FArguments& Args)
{
 OnClosed=Args._OnClosed;bTitleIntro=Args._TitleIntro;bSelection=!bTitleIntro;
 auto* Loaded=LoadObject<UPadmaDivinationStyle>(nullptr,TEXT("/Game/Padma/UI/Divination/DA_DivinationStyleArk.DA_DivinationStyleArk"));
 Theme.Reset(Loaded?Loaded:NewObject<UPadmaDivinationStyle>());
 for(const auto& Pair:Theme->OriginalArt)
 {
  auto* Tex=Pair.Value.LoadSynchronous();if(!Tex)continue;
  OriginalTextures.Emplace(Tex);FSlateBrush B;B.SetResourceObject(Tex);B.ImageSize=FVector2D(Tex->GetSizeX(),Tex->GetSizeY());OriginalBrushes.Add(Pair.Key,B);
 }
 FString CatalogError;if(!Tarot.Load(Theme->OriginalLayoutJson,CatalogError))UE_LOG(LogTemp,Error,TEXT("[PadmaDivination] %s"),*CatalogError);
 if(const auto* Shadow=OriginalBrushes.Find(TEXT("tarot_shadow")))ShadowBrush=*Shadow;else ShadowBrush=*FCoreStyle::Get().GetBrush("WhiteBrush");
 TSharedPtr<FJsonObject> Scene;
 if(FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Theme->OriginalLayoutJson),Scene)&&Scene.IsValid())
 {
  auto ReadMeshes=[](const TArray<TSharedPtr<FJsonValue>>& Rows)
  {
   TArray<FPadmaOriginalMesh> Meshes;
   for(const auto& Value:Rows)
   {
    auto J=Value->AsObject();FPadmaOriginalMesh M;M.Name=J->GetStringField(TEXT("name"));M.Texture=FName(*J->GetStringField(TEXT("texture")));M.Alpha=J->GetNumberField(TEXT("alpha"));
    const TArray<TSharedPtr<FJsonValue>>* Color=nullptr;if(J->TryGetArrayField(TEXT("color"),Color)&&Color->Num()==3)M.Color=FLinearColor((*Color)[0]->AsNumber(),(*Color)[1]->AsNumber(),(*Color)[2]->AsNumber(),1);
    for(const auto& Row:J->GetArrayField(TEXT("vertices"))){const auto& V=Row->AsArray();M.Positions.Add(FVector2f(V[0]->AsNumber(),V[1]->AsNumber()));M.UVs.Add(FVector2f(V[2]->AsNumber(),V[3]->AsNumber()));}
    for(const auto& Idx:J->GetArrayField(TEXT("indices")))M.Indices.Add(SlateIndex(Idx->AsNumber()));
    Meshes.Add(MoveTemp(M));
   }
   return Meshes;
  };
  const TSharedPtr<FJsonObject>* WhiteSource=nullptr;
  if(Scene->TryGetObjectField(TEXT("selectionWhite"),WhiteSource))for(const auto& Pair:(*WhiteSource)->Values)WhitePanels.Add(FName(*Pair.Key),ReadMeshes(Pair.Value->AsArray()));
  const TSharedPtr<FJsonObject>* DetailData=nullptr;if(Scene->TryGetObjectField(TEXT("detailGeometry"),DetailData))DetailGeometry=*DetailData;
  const TSharedPtr<FJsonObject>* Angles=nullptr;
  if(Scene->TryGetObjectField(TEXT("heroEntryAngles"),Angles))for(const auto& Entry:(*Angles)->Values)
  {TArray<FVector2D> Keys;for(const auto& Key:Entry.Value->AsArray()){const auto& V=Key->AsArray();Keys.Add(FVector2D(V[0]->AsNumber(),V[1]->AsNumber()));}HeroEntryAngles.Add(FName(*Entry.Key),MoveTemp(Keys));}
  const TSharedPtr<FJsonObject>* Stars=nullptr;if(Scene->TryGetObjectField(TEXT("announceParticles"),Stars))StarConfig=*Stars;
  const TSharedPtr<FJsonObject>* Pages=nullptr;
  if(Scene->TryGetObjectField(TEXT("sourcePages"),Pages))SourcePages=*Pages;
  ClockMeshes=ReadMeshes(Scene->GetArrayField(TEXT("clock")));
  const TSharedPtr<FJsonObject>* Clips=nullptr;
  if(Scene->TryGetObjectField(TEXT("heroClips"),Clips))
  {
   const auto Templates=ReadMeshes(Scene->GetArrayField(TEXT("heroMeshTemplates")));
   for(const auto& Pair:(*Clips)->Values)
   {
    auto J=Pair.Value->AsObject();FPadmaOriginalClip Clip;Clip.Duration=J->GetNumberField(TEXT("duration"));Clip.bLoop=J->GetBoolField(TEXT("loop"));
    for(const auto& Pose:J->GetArrayField(TEXT("frames")))
    {
     const auto Frame=Pose->AsObject();const auto& Positions=Frame->GetArrayField(TEXT("p"));const auto& Colors=Frame->GetArrayField(TEXT("c"));
     TArray<FPadmaOriginalMesh> Meshes=Templates;int32 V=0,C=0;
     for(auto& M:Meshes)
     {
      for(auto& Pos:M.Positions){Pos=FVector2f(Positions[V]->AsNumber(),Positions[V+1]->AsNumber());V+=2;}
      M.Color=FLinearColor(Colors[C]->AsNumber(),Colors[C+1]->AsNumber(),Colors[C+2]->AsNumber(),1);M.Alpha=Colors[C+3]->AsNumber();C+=4;
     }
     Clip.Frames.Add(MoveTemp(Meshes));
    }
    HeroClips.Add(FName(*Pair.Key),MoveTemp(Clip));
   }
  }
 }

 if(Scene.IsValid())
 {
  const TSharedPtr<FJsonObject>* Effects=nullptr;
  if(Scene->TryGetObjectField(TEXT("pageEffects"),Effects))for(const auto& Entry:(*Effects)->Values)
  {
   auto J=Entry.Value->AsObject();const FName Key(*Entry.Key);const FName Shader(*J->GetStringField(TEXT("shader")));
   auto* Base=Theme->PageMaterials.FindRef(Shader).LoadSynchronous();if(!Base)continue;
   auto* MID=UMaterialInstanceDynamic::Create(Base,GetTransientPackage());PageEffectMaterials.Emplace(MID);
   for(const auto& Value:J->GetObjectField(TEXT("floats"))->Values)MID->SetScalarParameterValue(FName(*Value.Key),Value.Value->AsNumber());
   for(const auto& Value:J->GetObjectField(TEXT("vectors"))->Values)
   {const auto& V=Value.Value->AsArray();MID->SetVectorParameterValue(FName(*Value.Key),FLinearColor(V[0]->AsNumber(),V[1]->AsNumber(),V[2]->AsNumber(),V[3]->AsNumber()));}
   for(const auto& Value:J->GetObjectField(TEXT("textures"))->Values)MID->SetTextureParameterValue(FName(*Value.Key),Theme->OriginalArt.FindRef(FName(*Value.Value->AsString())).LoadSynchronous());
   FSlateBrush B;B.SetResourceObject(MID);PageEffectBrushes.Add(Key,B);
   const FString MeshKey=J->GetStringField(TEXT("mesh"));
   if(!MeshKey.IsEmpty())
   {
    auto Mesh=Scene->GetObjectField(TEXT("pageMeshes"))->GetObjectField(MeshKey);FPadmaOriginalMesh M;
    for(const auto& Pos:Mesh->GetArrayField(TEXT("positions"))){const auto& V=Pos->AsArray();M.Positions.Add(FVector2f(V[0]->AsNumber(),V[1]->AsNumber()));}
    for(const auto& UV:Mesh->GetArrayField(TEXT("uvs"))){const auto& V=UV->AsArray();M.UVs.Add(FVector2f(V[0]->AsNumber(),V[1]->AsNumber()));}
    for(const auto& Index:Mesh->GetArrayField(TEXT("indices")))M.Indices.Add(SlateIndex(Index->AsNumber()));
    PageEffectMeshes.Add(Key,MoveTemp(M));
   }
  }
 }
 for(const auto& Entry:Theme->OriginalFonts)
 {
  auto* FaceObject=Entry.Value.LoadSynchronous();if(!FaceObject)continue;
  OriginalFontFaces.Emplace(FaceObject);auto Composite=MakeShared<FCompositeFont>();
  FTypefaceEntry Face;Face.Name=TEXT("Default");Face.Font=FFontData(FaceObject);Composite->DefaultTypeface.Fonts.Add(Face);
  OriginalFonts.Add(Entry.Key,Composite);
 }
 SerifFace.Reset(Theme->SerifFont.LoadSynchronous());
 if(SerifFace.IsValid()){Serif=MakeShared<FCompositeFont>();FTypefaceEntry Face;Face.Name=TEXT("Default");Face.Font=FFontData(SerifFace.Get());Serif->DefaultTypeface.Fonts.Add(Face);}
 if(auto* Base=Theme->WaterMaterial.LoadSynchronous())
 {
  Water.Reset(UMaterialInstanceDynamic::Create(Base,GetTransientPackage()));WaterBrush.SetResourceObject(Water.Get());WaterBrush.ImageSize=FVector2D(1600,900);
  Water->SetVectorParameterValue(TEXT("Accent"),Theme->Accent);Water->SetScalarParameterValue(TEXT("WaterStrength"),Theme->WaterStrength);
 }
 auto* CardBase=Theme->CardMaterial.LoadSynchronous();
 for(int32 I=0;I<8;++I)
 {
  auto* MID=CardBase?UMaterialInstanceDynamic::Create(CardBase,GetTransientPackage()):nullptr;
  if(MID){MID->SetTextureParameterValue(TEXT("CardFace"),Theme->OriginalArt.FindRef(FName(*FString::Printf(TEXT("tarot_%d"),I+1))).LoadSynchronous());}
  CardMaterials.Emplace(MID);FSlateBrush B;B.SetResourceObject(MID);B.ImageSize=FVector2D(340,516);CardBrushes.Add(B);
 }
 if(CardBase)
 {
  CardBack.Reset(UMaterialInstanceDynamic::Create(CardBase,GetTransientPackage()));CardBack->SetScalarParameterValue(TEXT("Back"),1);BackBrush.SetResourceObject(CardBack.Get());BackBrush.ImageSize=FVector2D(290,445);
 }
 if(!Water.IsValid()||!CardBase||!SerifFace.IsValid())UE_LOG(LogTemp,Warning,TEXT("[PadmaDivination] Run AuthorDivinationOriginals.py: missing presentation asset."));
 ChildSlot[SNew(SBorder).Padding(0).BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush")).BorderBackgroundColor(FLinearColor::Black)
  [SNew(SScaleBox).Stretch(EStretch::ScaleToFit)[SNew(SBox).WidthOverride(1600).HeightOverride(900)[SAssignNew(Canvas,SPadmaDivinationCanvas).Owner(this)]]]];
 SetCanTick(true);
#if !UE_BUILD_SHIPPING
 float Preview=-1;
 if(FParse::Value(FCommandLine::Get(),TEXT("PadmaDivinationTime="),Preview)&&Preview>=0){bSelection=false;Playback.Seek(Preview,*Theme);}
 if(FParse::Value(FCommandLine::Get(),TEXT("PadmaIntroTime="),Preview)&&Preview>=0){IntroTime=Preview;bIntroFrozen=true;}
#endif
#if !UE_BUILD_SHIPPING
 if(FParse::Param(FCommandLine::Get(),TEXT("PadmaTarotFresh")))Tarot.SetNormalRevealed(false);
 int32 SpreadIndex=-1;if(FParse::Value(FCommandLine::Get(),TEXT("PadmaTarotSpread="),SpreadIndex)&&Tarot.IsReady())Tarot.Select(SpreadIndex);
 bJudge=FParse::Param(FCommandLine::Get(),TEXT("PadmaTarotJudge"));
 bHelp=FParse::Param(FCommandLine::Get(),TEXT("PadmaTarotHelp"));
 if(FParse::Value(FCommandLine::Get(),TEXT("PadmaPageTime="),PageTime))bPageFrozen=true;
 bGallery=FParse::Param(FCommandLine::Get(),TEXT("PadmaTarotGallery"));
 FParse::Value(FCommandLine::Get(),TEXT("PadmaGalleryCard="),GalleryCard);GalleryCard=FMath::Clamp(GalleryCard,0,7);GalleryScroll=GalleryTarget=GalleryCard;
 FParse::Value(FCommandLine::Get(),TEXT("PadmaGalleryScroll="),GalleryTarget);GalleryScroll=GalleryTarget;
 if(FParse::Value(FCommandLine::Get(),TEXT("PadmaHeroTime="),HeroTime))bHeroFrozen=true;
 int32 DetailIndex=-1;if(FParse::Value(FCommandLine::Get(),TEXT("PadmaTarotDetail="),DetailIndex)&&Tarot.IsReady()&&Tarot.CurrentCards().IsValidIndex(DetailIndex))DetailSlot=DetailIndex;
#endif
 UE_LOG(LogTemp,Display,TEXT("[PadmaDivination] Open title=%d cards=%d water=%d serif=%d"),bTitleIntro,CardBrushes.Num(),Water.IsValid(),SerifFace.IsValid());
}
SPadmaDivinationWidget::~SPadmaDivinationWidget(){if(Canvas)Canvas->Owner=nullptr;}
void SPadmaDivinationWidget::ContinuePresentation()
{
 if(Playback.Phase==EPadmaDivinationPhase::Closing||Playback.Phase==EPadmaDivinationPhase::Closed)return;
 if(bTitleIntro){RequestClose();return;}
 if(bHelp||bGallery||DetailSlot!=INDEX_NONE||bJudge)return;
 if(bSelection)
 {
  if(!Tarot.CanDivine())return;
  if(Tarot.NeedsConfirmation()){bJudge=true;PageTime=0;PageAnimation.Empty();return;}
  bSelection=false;Playback.Replay();return;
 }
 if(Playback.Phase==EPadmaDivinationPhase::Result){bSelection=true;HeroTime=0;Playback.Replay();return;}
 if(Playback.Phase==EPadmaDivinationPhase::AwaitReveal&&Tarot.Draw())Playback.Reveal();
}
void SPadmaDivinationWidget::BeginPageAction(const TCHAR* Animation,float Duration,int32 Action)
{PageAnimation=Animation;PageTime=0;PageActionDuration=Duration;PageAction=Action;}
void SPadmaDivinationWidget::ConfirmDivination(bool Accept)
{
 if(!bJudge||PageAction!=0)return;
 BeginPageAction(TEXT("act54side_divination_start_tips_out"),.533333f,Accept?3:2);
}
void SPadmaDivinationWidget::SwitchDetail(int32 Step)
{
 if(DetailSlot==INDEX_NONE||PageAction!=0)return;
 BeginPageAction(Step<0?TEXT("act54side_ divination_info_dialog_prev_hide"):TEXT("act54side_ divination_info_dialog_next_hide"),.333333f,Step<0?4:5);
}
void SPadmaDivinationWidget::RequestClose()
{
 if(bHelp){bHelp=false;return;}
 if(bJudge){ConfirmDivination(false);return;}
 if(DetailSlot!=INDEX_NONE){if(PageAction==0)BeginPageAction(TEXT("act54side_ divination_info_dialog_out"),.333333f,1);return;}
 if(bGallery){bGallery=false;return;}
 if(Playback.Close())UE_LOG(LogTemp,Display,TEXT("[PadmaDivination] Closing"));
}
void SPadmaDivinationWidget::Tick(const FGeometry& G,double Now,float Delta)
{
 SCompoundWidget::Tick(G,Now,Delta);
 float PlaybackDelta=Delta;
 HoveredTab=INDEX_NONE;
 if(Canvas&&bSelection&&!bGallery&&DetailSlot==INDEX_NONE)
 {
  const FVector2D Hit=Canvas->GetCachedGeometry().AbsoluteToLocal(FSlateApplication::Get().GetCursorPos());
  if(Hit.X>=1190&&Hit.Y>=680)HoveredTab=5;
  else if(Hit.X>=1210&&Hit.Y>=205&&Hit.Y<680)HoveredTab=FMath::Clamp(int32((Hit.Y-205)/91),0,4);
  const bool Interactive=HoveredTab!=INDEX_NONE||(Hit.X>1360&&Hit.Y<105)||(Hit.X<260&&Hit.Y>670)||(Hit.X<140&&Hit.Y<130)||CardAt(Hit)!=INDEX_NONE;
  SetCursor(Interactive?EMouseCursor::Hand:EMouseCursor::Default);
 }
 if(!bHeroFrozen)HeroTime+=FMath::Max(0.f,Delta)*Theme->PlaybackRate;
 if(Canvas){const FVector2D Hit=Canvas->GetCachedGeometry().AbsoluteToLocal(FSlateApplication::Get().GetCursorPos());RevealHover=FMath::FInterpTo(RevealHover,bSelection&&!bGallery&&DetailSlot==INDEX_NONE&&Hit.X<260&&Hit.Y>670?1.f:0.f,Delta,12.f);}
 const bool HeldReveal=bPointerPressed&&PointerDownPoint.X<260&&PointerDownPoint.Y>670;
 const float TintTarget=HeldReveal?.7843137f:RevealHover>.5f?.9607843f:1.f;
 if(TintTarget!=RevealTintTarget){RevealTintFrom=RevealTint;RevealTintTarget=TintTarget;RevealTintTime=0;}
 RevealTintTime+=FMath::Max(0.f,Delta);RevealTint=FMath::Lerp(RevealTintFrom,RevealTintTarget,Unit(RevealTintTime/.1f));
 if(!bPageFrozen)PageTime+=FMath::Max(0.f,Delta);
 if(PageAction!=0&&PageTime>=PageActionDuration)
 {
  const int32 Action=PageAction;PageAction=0;PageAnimation.Empty();PageTime=0;
  if(Action==6)PageTime=999;
  if(Action==1)DetailSlot=INDEX_NONE;
  else if(Action==2||Action==3)
  {
   bJudge=false;
   if(Action==3&&Tarot.CanDivine()){bSelection=false;Playback.Replay();PlaybackDelta=0;}
  }
  else if(Action==4||Action==5)
  {
   const int32 Count=Tarot.CurrentCards().Num();DetailSlot=(DetailSlot+(Action==4?Count-1:1))%Count;
   BeginPageAction(Action==4?TEXT("act54side_ divination_info_dialog_prev_show"):TEXT("act54side_ divination_info_dialog_next_show"),.416667f,6);
  }
 }
 if(!bPointerPressed)GalleryScroll=FMath::FInterpTo(GalleryScroll,GalleryTarget,Delta,16.6667f);
 // Exponential smoothing is stable across refresh rates; release eases back to the authored pose.
 GalleryTilt=FMath::Lerp(GalleryTilt,GalleryTiltTarget,1-FMath::Exp(-16.f*FMath::Max(0.f,Delta)));
 for(auto& Pair:BoardCardPoses)Pair.Value.Tilt=FMath::Lerp(Pair.Value.Tilt,Pair.Value.Target,1-FMath::Exp(-16.f*FMath::Max(0.f,Delta)));
 TabTime+=FMath::Max(0.f,Delta);GalleryFlip+=FMath::Max(0.f,Delta);
 if(bTitleIntro&&!bIntroFrozen&&Playback.Phase!=EPadmaDivinationPhase::Closing){IntroTime+=FMath::Max(0.f,Delta);if(IntroTime>=Theme->StartupDuration)RequestClose();}
 const bool Closed=Playback.Advance(PlaybackDelta,*Theme);
 if(Playback.Phase==EPadmaDivinationPhase::Closing||Closed)SetRenderOpacity(1-Unit(Playback.CloseTime/FMath::Max(.05f,Theme->CloseDuration)));else SetRenderOpacity(1);
 if(!Playback.bPaused&&!bIntroFrozen&&Playback.Phase!=EPadmaDivinationPhase::Closing)
 {
  const FVector2D Size=G.GetLocalSize(),Mouse=G.AbsoluteToLocal(FSlateApplication::Get().GetCursorPos());
  FVector2D Target(FMath::Clamp(Mouse.X/FMath::Max(1.,Size.X)*2-1,-1.,1.),FMath::Clamp(Mouse.Y/FMath::Max(1.,Size.Y)*2-1,-1.,1.));
  Parallax=FMath::Vector2DInterpTo(Parallax,Target*(Theme->ParallaxDegrees/6.5f),Delta,5.f);
 }
 const auto Phase=Playback.Phase==EPadmaDivinationPhase::Closing?Playback.ClosingFrom:Playback.Phase;
 const bool R=Phase==EPadmaDivinationPhase::Reveal||Phase==EPadmaDivinationPhase::Result;
 if(Water.IsValid())
 {
  Water->SetScalarParameterValue(TEXT("PreviewTime"),bTitleIntro?IntroTime:Playback.AmbientTime);
  Water->SetScalarParameterValue(TEXT("RevealPhase"),bSelection?1:R?Ramp(Playback.Time*2.267f/FMath::Max(.1f,Theme->RevealDuration),0,.95f):0);
 }
 for(auto& MID:PageEffectMaterials)if(MID.IsValid())MID->SetScalarParameterValue(TEXT("PreviewTime"),bTitleIntro?IntroTime:bPageFrozen?PageTime:Playback.AmbientTime);
 for(int32 CardIndex=0;CardIndex<CardMaterials.Num();++CardIndex)if(auto& MID=CardMaterials[CardIndex];MID.IsValid())
 {
  FVector2D Tilt=bGallery?GalleryTilt:FVector2D::ZeroVector;
  if(!bGallery&&Tarot.IsReady())for(const auto& Pair:BoardCardPoses)
   if(Tarot.CurrentCards().IsValidIndex(Pair.Key)&&Tarot.CurrentCards()[Pair.Key].Card==CardIndex)Tilt=Pair.Value.Tilt;
  MID->SetScalarParameterValue(TEXT("PreviewTime"),Playback.AmbientTime);
  MID->SetScalarParameterValue(TEXT("Tilt"),Parallax.X+Parallax.Y+Tilt.X/30+Tilt.Y/18);
 }
 if(Canvas)Canvas->Invalidate(EInvalidateWidgetReason::Paint);
 if(Closed){const auto Callback=OnClosed;OnClosed.Unbind();Callback.ExecuteIfBound();}
}
bool SPadmaDivinationWidget::SelectSpread(int32 Index)
{
 if(!bSelection||bGallery||bJudge||bHelp||DetailSlot!=INDEX_NONE||Playback.Phase==EPadmaDivinationPhase::Closing)return false;
 if(Tarot.Selected==Index)return true;
 const bool WasSpecial=Tarot.Selected==5;
 if(!Tarot.Select(Index))return false;BoardCardPoses.Empty();BoardDragSlot=INDEX_NONE;if(WasSpecial!=(Tarot.Selected==5))HeroTime=0;TabTime=0;return true;
}
void SPadmaDivinationWidget::OpenGallery()
{
 if(bSelection&&!bJudge&&!bHelp&&DetailSlot==INDEX_NONE&&Tarot.IsReady()){bGallery=true;GalleryCard=0;GalleryScroll=GalleryTarget=0;bGalleryReverse=false;GalleryFlip=.666667f;PageTime=0;PageAnimation.Empty();}
}
int32 SPadmaDivinationWidget::CardAt(FVector2D Point)const
{
 if(!Tarot.IsReady())return INDEX_NONE;
 const auto& Layout=bSelection?Tarot.CurrentSpread().Selection:Tarot.CurrentSpread().Result;
 for(int32 I=FMath::Min(Layout.Num(),Tarot.CurrentCards().Num())-1;I>=0;--I)
 {
  const auto& D=Layout[I];const auto Tilt=BoardCardPoses.FindRef(I).Tilt;const FVector Degrees=D.Rotation+FVector(Parallax.Y*2+Tilt.Y,Parallax.X*3+Tilt.X,0);
  const FQuat Q=FQuat(FVector(0,0,1),FMath::DegreesToRadians(Degrees.Z))*FQuat(FVector(0,1,0),FMath::DegreesToRadians(Degrees.Y))*FQuat(FVector(1,0,0),FMath::DegreesToRadians(Degrees.X));
  TArray<FVector2D> Quad;
  for(const FVector2D UV:{FVector2D(0,0),FVector2D(1,0),FVector2D(1,1),FVector2D(0,1)})
  {const FVector P=Q.RotateVector(FVector((UV.X-.5)*D.Size.X,(UV.Y-.5)*D.Size.Y,0));Quad.Add(D.Position+FVector2D(P.X,P.Y)*(1500/FMath::Max(350.f,1500+float(P.Z)))+Parallax*8);}
  bool Positive=false,Negative=false;
  for(int32 N=0;N<4;++N){const FVector2D A=Quad[(N+1)%4]-Quad[N],B=Point-Quad[N];const double Cross=A.X*B.Y-A.Y*B.X;Positive|=Cross>0;Negative|=Cross<0;}
  if(!(Positive&&Negative))return I;
 }
 return INDEX_NONE;
}
int32 SPadmaDivinationWidget::GalleryCardAt(FVector2D Hit)const
{
  for(int32 I=7;I>=0;--I)
  {
   const auto Center=GalleryPosition(I,GalleryScroll);const float U=.331f+.16f*(I-GalleryScroll);TArray<FVector2D> Quad;
   for(FVector2D UV:{FVector2D(0,0),FVector2D(1,0),FVector2D(1,1),FVector2D(0,1)})Quad.Add(GalleryProject(Center,U,(UV-FVector2D(.5,.5))*FVector2D(318.75,483.75)));
   bool Positive=false,Negative=false;for(int32 K=0;K<4;++K){const auto Edge=Quad[(K+1)%4]-Quad[K],Point=Hit-Quad[K];const auto Cross=Edge.X*Point.Y-Edge.Y*Point.X;Positive|=Cross>0;Negative|=Cross<0;}
   if(!(Positive&&Negative))return I;
  }
 return INDEX_NONE;
}
bool SPadmaDivinationWidget::GalleryMainCardAt(FVector2D Hit)const
{
 if(!bGallery||!SourcePages.IsValid())return false;
 const auto Page=SourcePages->GetObjectField(TEXT("gallery"));const auto& Elements=Page->GetArrayField(TEXT("elements"));
 const auto& Rows=Page->GetArrayField(TEXT("rest"));
 for(int32 I=0;I<Elements.Num();++I)
 {
  if(!Elements[I]->AsObject()->GetStringField(TEXT("path")).EndsWith(TEXT("tarot_card_rotate")))continue;
  const auto& R=Rows[I]->AsArray();auto V=[&](int32 K){return R[K]->AsNumber();};
  const float Flip=Ease(GalleryFlip/.666667f),Turn=bGalleryReverse?180*Flip:180*(1-Flip);
  const FQuat Q=FQuat(FVector(0,0,1),FMath::DegreesToRadians(V(6)+Turn))*FQuat(FVector(0,1,0),FMath::DegreesToRadians(V(5)+GalleryTilt.X))*FQuat(FVector(1,0,0),FMath::DegreesToRadians(V(4)+GalleryTilt.Y));
  TArray<FVector2D> Quad;
  for(FVector2D UV:{FVector2D(0,0),FVector2D(1,0),FVector2D(1,1),FVector2D(0,1)})
  {
   const FVector2D Local=(UV-FVector2D(.5,.5))*FVector2D(340,516)*V(2)/100;
   const FVector Pt=Q.RotateVector(FVector(Local.X,Local.Y,0));
   Quad.Add(FVector2D(V(0),V(1))+FVector2D(Pt.X,Pt.Y)*(1500/FMath::Max(350.,1500+Pt.Z))+Parallax*8);
  }
  bool Positive=false,Negative=false;
  for(int32 K=0;K<4;++K){const auto Edge=Quad[(K+1)%4]-Quad[K],Point=Hit-Quad[K];const auto Cross=Edge.X*Point.Y-Edge.Y*Point.X;Positive|=Cross>0;Negative|=Cross<0;}
  return !(Positive&&Negative);
 }
 return false;
}
void SPadmaDivinationWidget::HandlePointer(FVector2D Hit)
{
 if(Playback.Phase==EPadmaDivinationPhase::Closing||Playback.Phase==EPadmaDivinationPhase::Closed)return;
 if(bTitleIntro){ContinuePresentation();return;}
 if(PageAction!=0)return;
 const bool Back=Hit.X<100&&Hit.Y<130;
 if(bHelp){bHelp=false;return;}
 if(bJudge)
 {
  const auto Target=SourceHit(TEXT("judge"),Hit);
  if(Target.Contains(TEXT("btn_confirm")))ConfirmDivination(true);
  else if(Target.Contains(TEXT("btn_cancel")))ConfirmDivination(false);
  return;
 }
 if(DetailSlot!=INDEX_NONE)
 {
  const auto Target=SourceHit(TEXT("detail"),Hit);
  if(Target.Contains(TEXT("btn_close"))||Back)RequestClose();
  else if(Target.Contains(TEXT("btn_switch_left")))SwitchDetail(-1);
  else if(Target.Contains(TEXT("btn_switch_right")))SwitchDetail(1);
  return;
 }
 if(bGallery)
 {
  const auto Target=SourceHit(TEXT("gallery"),Hit);
  if(Back||Target.Contains(TEXT("btn_back"))){bGallery=false;return;}
  if(Target.Contains(TEXT("btn_tarot_rotate"))){bGalleryReverse=!bGalleryReverse;GalleryFlip=0;return;}
  const int32 CardIndex=GalleryCardAt(Hit);if(CardIndex!=INDEX_NONE)SelectGalleryCard(CardIndex);
  return;
 }
 if(bSelection)
 {
  if(Back){RequestClose();return;}
  if(Hit.X>=100&&Hit.X<175&&Hit.Y<130){bHelp=true;return;}
  if(Hit.X>1360&&Hit.Y<105){OpenGallery();return;}
  if(Hit.X>=1190&&Hit.Y>=680){SelectSpread(5);return;}
  if(Hit.X>=1210&&Hit.Y>=205&&Hit.Y<680){SelectSpread(FMath::Clamp(int32((Hit.Y-205)/91),0,4));return;}
  if(Hit.X<260&&Hit.Y>670){ContinuePresentation();return;}
  if(TabTime>=.667f){DetailSlot=CardAt(Hit);PageTime=0;PageAnimation.Empty();}
  return;
 }
 if(Playback.Phase==EPadmaDivinationPhase::Result)
 {DetailSlot=CardAt(Hit);PageTime=0;PageAnimation.Empty();if(DetailSlot!=INDEX_NONE)return;}
 ContinuePresentation();
}
FName SPadmaDivinationWidget::GetHeroClipName()const
{
 const bool Special=Tarot.Selected==5;
 return HeroTime<1.7667f?(Special?TEXT("drop_special"):TEXT("drop_normal")):(Special?TEXT("loop_special"):TEXT("loop_normal"));
}
FReply SPadmaDivinationWidget::OnMouseButtonDown(const FGeometry&,const FPointerEvent& Event)
{
 if(FParse::Param(FCommandLine::Get(),TEXT("unattended"))&&FString(FCommandLine::Get()).Contains(TEXT("PadmaPlayableCapture=")))return FReply::Unhandled();
 if(Event.GetEffectingButton()!=EKeys::LeftMouseButton)return FReply::Unhandled();
 bPointerPressed=true;bPointerMoved=false;DragStartScroll=GalleryScroll;DragStartTilt=GalleryTilt;
 if(Canvas)PointerDownPoint=Canvas->GetCachedGeometry().AbsoluteToLocal(Event.GetScreenSpacePosition());
 const bool CanDrag=bGallery&&!bHelp&&!bJudge&&DetailSlot==INDEX_NONE&&PageAction==0&&SourceHit(TEXT("gallery"),PointerDownPoint).IsEmpty();
 bGalleryCardDrag=CanDrag&&GalleryMainCardAt(PointerDownPoint);
 bGalleryScrollDrag=CanDrag&&!bGalleryCardDrag&&(PointerDownPoint.X>=800||GalleryCardAt(PointerDownPoint)!=INDEX_NONE);
 // Only settled board cards own this gesture; the clock/reveal sequence and modal controls keep their clicks.
 const bool BoardReady=bSelection&&!bGallery&&!bTitleIntro&&!bHelp&&!bJudge&&DetailSlot==INDEX_NONE&&PageAction==0&&TabTime>=.667f&&Playback.Phase!=EPadmaDivinationPhase::Closing;
 const bool OverControl=PointerDownPoint.X>=1190||(PointerDownPoint.X<175&&PointerDownPoint.Y<130)||(PointerDownPoint.X<260&&PointerDownPoint.Y>670);
 BoardDragSlot=BoardReady&&!OverControl?CardAt(PointerDownPoint):INDEX_NONE;
 if(BoardDragSlot!=INDEX_NONE)DragStartTilt=BoardCardPoses.FindOrAdd(BoardDragSlot).Tilt;
 return FReply::Handled().CaptureMouse(SharedThis(this)).SetUserFocus(SharedThis(this),EFocusCause::Mouse);
}
FReply SPadmaDivinationWidget::OnMouseButtonUp(const FGeometry&,const FPointerEvent& Event)
{
 if(FParse::Param(FCommandLine::Get(),TEXT("unattended"))&&FString(FCommandLine::Get()).Contains(TEXT("PadmaPlayableCapture=")))return FReply::Unhandled();
 if(Event.GetEffectingButton()!=EKeys::LeftMouseButton)return FReply::Unhandled();
 if(bPointerPressed&&Canvas)
 {
  const auto Up=Canvas->GetCachedGeometry().AbsoluteToLocal(Event.GetScreenSpacePosition());
  if(!bPointerMoved&&FVector2D::DistSquared(Up,PointerDownPoint)<2500)HandlePointer(Up);
 }
 ReleaseCardDrag();return FReply::Handled().ReleaseMouseCapture();
}
void SPadmaDivinationWidget::ReleaseCardDrag()
{
 bPointerPressed=bGalleryCardDrag=bGalleryScrollDrag=false;GalleryTiltTarget=FVector2D::ZeroVector;
 if(auto* Pose=BoardCardPoses.Find(BoardDragSlot))Pose->Target=FVector2D::ZeroVector;
 BoardDragSlot=INDEX_NONE;
}
void SPadmaDivinationWidget::OnMouseCaptureLost(const FCaptureLostEvent& Event)
{ReleaseCardDrag();SCompoundWidget::OnMouseCaptureLost(Event);}
FReply SPadmaDivinationWidget::OnKeyDown(const FGeometry&,const FKeyEvent& Event)
{
 if(FParse::Param(FCommandLine::Get(),TEXT("unattended"))&&FString(FCommandLine::Get()).Contains(TEXT("PadmaPlayableCapture=")))return FReply::Unhandled();
 const auto Key=Event.GetKey();
 if(Key==EKeys::Escape){RequestClose();return FReply::Handled();}
 if(Key==EKeys::Left||Key==EKeys::Right)
 {
  const int32 Step=Key==EKeys::Left?-1:1;
  if(bGallery)SelectGalleryCard(FMath::Clamp(GalleryCard+Step,0,7));
  else if(DetailSlot!=INDEX_NONE)SwitchDetail(Step);
  else if(bSelection)SelectSpread((Tarot.Selected+Step+6)%6);
  return FReply::Handled();
 }
 if(Key==EKeys::Enter||Key==EKeys::SpaceBar)
 {
  if(bHelp){bHelp=false;return FReply::Handled();}
  if(bJudge){ConfirmDivination(true);return FReply::Handled();}
  if(bGallery){bGalleryReverse=!bGalleryReverse;GalleryFlip=0;}
  else if(DetailSlot==INDEX_NONE)ContinuePresentation();
  return FReply::Handled();
 }
 return FReply::Unhandled();
}

FVector2D SPadmaDivinationWidget::GalleryPosition(float Index,float Selected)
{
 const float U=.331f+.16f*(Index-Selected);
 const FVector2D A(526.119,143.675),B(397.421,-403.329),C(76.148,-828.316),D(-282.904,-1011.028);
 FVector2D V;
 if(U<0)V=A+(B-A)*(3*U);
 else if(U>1)V=D+(D-C)*(3*(U-1));
 else V=A*FMath::Pow(1-U,3.f)+B*(3*U*FMath::Square(1-U))+C*(3*U*U*(1-U))+D*(U*U*U);
 return FVector2D(800+V.X*1.25,-V.Y*1.25);
}
bool SPadmaDivinationWidget::SelectGalleryCard(int32 Index)
{
 if(!bGallery||!Tarot.Cards.IsValidIndex(Index))return false;
 GalleryCard=Index;GalleryTarget=Index;return true;
}
FString SPadmaDivinationWidget::SourceHit(const FString& Key,FVector2D Hit)const
{
 if(!SourcePages.IsValid())return {};
 const auto Page=SourcePages->GetObjectField(Key);const auto& Rows=Page->GetArrayField(TEXT("rest"));const auto& Elements=Page->GetArrayField(TEXT("elements"));
 for(int32 I=Elements.Num()-1;I>=0;--I)
 {
  const auto J=Elements[I]->AsObject();const FString Path=J->GetStringField(TEXT("path"));
  if(J->GetArrayField(TEXT("click")).IsEmpty()&&!Path.EndsWith(TEXT("btn_tarot_rotate")))continue;
  const auto& R=Rows[I]->AsArray();auto V=[&](int32 K){return R[K]->AsNumber();};
  const double Angle=FMath::DegreesToRadians(-V(6));const FVector2D Local=Hit-FVector2D(V(0),V(1));
  const FVector2D P(Local.X*FMath::Cos(Angle)-Local.Y*FMath::Sin(Angle)+V(2)*V(7),Local.X*FMath::Sin(Angle)+Local.Y*FMath::Cos(Angle)+V(3)*V(8));
  if(P.X>=0&&P.Y>=0&&P.X<=V(2)&&P.Y<=V(3))return Path;
 }
 return {};
}
FReply SPadmaDivinationWidget::OnMouseWheel(const FGeometry&,const FPointerEvent& Event)
{
 if(!bGallery)return FReply::Unhandled();
 GalleryTarget=FMath::Clamp(GalleryTarget+(Event.GetWheelDelta()>0?-1:1),0.f,7.f);return FReply::Handled();
}
FReply SPadmaDivinationWidget::OnMouseMove(const FGeometry&,const FPointerEvent& Event)
{
 if(!bPointerPressed||!Canvas)return FReply::Unhandled();
 const auto P=Canvas->GetCachedGeometry().AbsoluteToLocal(Event.GetScreenSpacePosition());
 const FVector2D Travel=P-PointerDownPoint;
 bPointerMoved|=Travel.SizeSquared()>64;
 if(BoardDragSlot!=INDEX_NONE)
 {
  BoardCardPoses.FindOrAdd(BoardDragSlot).Target=FVector2D(FMath::Clamp(DragStartTilt.X+Travel.X*.18,-30.,30.),FMath::Clamp(DragStartTilt.Y-Travel.Y*.12,-18.,18.));
  return FReply::Handled();
 }
 if(bGalleryCardDrag)
 {
  // Original attitude controller limits: horizontal 30 degrees, vertical 18 degrees.
  GalleryTiltTarget=FVector2D(FMath::Clamp(DragStartTilt.X+Travel.X*.18,-30.,30.),FMath::Clamp(DragStartTilt.Y-Travel.Y*.12,-18.,18.));
  return FReply::Handled();
 }
 if(!bGalleryScrollDrag)return FReply::Unhandled();
 const float Raw=DragStartScroll-(P.Y-PointerDownPoint.Y)/265.f;
 GalleryScroll=Raw<0?Raw*.218f:Raw>7?7+(Raw-7)*.218f:Raw;
 GalleryTarget=FMath::Clamp(GalleryScroll,0.f,7.f);return FReply::Handled();
}
