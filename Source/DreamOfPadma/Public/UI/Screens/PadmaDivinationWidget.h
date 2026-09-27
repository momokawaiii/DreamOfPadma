#pragma once
#include "UI/Screens/PadmaTarotSession.h"
#include "CoreMinimal.h"
#include "Engine/FontFace.h"
#include "Engine/Texture2D.h"
#include "Rendering/RenderingCommon.h"
#include "Widgets/SCompoundWidget.h"
#include "UObject/StrongObjectPtr.h"
#include "UI/Screens/PadmaDivinationStyle.h"

struct FPadmaOriginalMesh
{
 FString Name;
 FName Texture;
 TArray<FVector2f> Positions,UVs;
 TArray<SlateIndex> Indices;
 float Alpha=1;
 FLinearColor Color=FLinearColor::White;
};
struct FPadmaOriginalClip
{
 float Duration=1;
 bool bLoop=false;
 TArray<TArray<FPadmaOriginalMesh>> Frames;
};
class FJsonObject;
class SPadmaDivinationCanvas;
class UMaterialInstanceDynamic;

enum class EPadmaDivinationPhase : uint8 { Enter, AwaitReveal, Reveal, Result, Closing, Closed };

/** Interruptible presentation clock, independent of frame rate and gameplay state. */
struct FPadmaDivinationPlayback
{
 EPadmaDivinationPhase Phase=EPadmaDivinationPhase::Enter;
 EPadmaDivinationPhase ClosingFrom=EPadmaDivinationPhase::Enter;
 float Time=0, AmbientTime=0, CloseTime=0, Rate=1;
 bool bPaused=false;
 bool Reveal();
 bool Close();
 void Replay();
 void Seek(float Seconds, const UPadmaDivinationStyle& Style);
 bool Advance(float Delta, const UPadmaDivinationStyle& Style);
};

class SPadmaDivinationWidget : public SCompoundWidget
{
public:
 SLATE_BEGIN_ARGS(SPadmaDivinationWidget) : _TitleIntro(false) {}
  SLATE_ARGUMENT(bool, TitleIntro)
  SLATE_EVENT(FSimpleDelegate, OnClosed)
 SLATE_END_ARGS()
 void Construct(const FArguments& Args);
 virtual ~SPadmaDivinationWidget() override;
 virtual void Tick(const FGeometry&,double,float) override;
 virtual bool SupportsKeyboardFocus()const override{return true;}
 virtual FReply OnMouseButtonDown(const FGeometry&,const FPointerEvent&)override;
 virtual FReply OnKeyDown(const FGeometry&,const FKeyEvent&)override;
 void HandlePointer(FVector2D Point);
 FName GetHeroClipName()const;
 float GetHeroTime()const{return HeroTime;}
 virtual FReply OnMouseButtonUp(const FGeometry&,const FPointerEvent&)override;
 virtual void OnMouseCaptureLost(const FCaptureLostEvent&)override;
 bool SelectSpread(int32 Index);
 void OpenGallery();
 bool SelectGalleryCard(int32 Index);
 void ConfirmDivination(bool Accept);
 void SwitchDetail(int32 Step);
 bool IsJudge()const{return bJudge;}
 bool IsHelp()const{return bHelp;}
 static FVector2D GalleryPosition(float Index,float Selected);
 virtual FReply OnMouseWheel(const FGeometry&,const FPointerEvent&)override;
 virtual FReply OnMouseMove(const FGeometry&,const FPointerEvent&)override;
 bool IsGallery()const{return bGallery;}
 int32 GetGalleryCard()const{return GalleryCard;}
 bool IsGalleryReversed()const{return bGalleryReverse;}
 int32 GetDetailSlot()const{return DetailSlot;}
 const FPadmaTarotSession& GetTarot()const{return Tarot;}
 void RequestClose();
 void ContinuePresentation();
 bool IsSelection() const { return bSelection; }
 bool IsTitleIntro() const { return bTitleIntro; }
 const FPadmaDivinationPlayback& GetPlayback() const { return Playback; }
private:
 friend class SPadmaDivinationCanvas;
 friend class FPadmaTarotDragTest;
 friend class FPadmaTarotBoardDragTest;
 FPadmaDivinationPlayback Playback;
 TStrongObjectPtr<UPadmaDivinationStyle> Theme;
 TStrongObjectPtr<UMaterialInstanceDynamic> Water;
 TArray<TStrongObjectPtr<UMaterialInstanceDynamic>> CardMaterials;
 TStrongObjectPtr<UMaterialInstanceDynamic> CardBack;
 FSlateBrush WaterBrush, BackBrush, ShadowBrush;
 TArray<FSlateBrush> CardBrushes;
 TSharedPtr<SPadmaDivinationCanvas> Canvas;
 FSimpleDelegate OnClosed;
 FVector2D Parallax=FVector2D::ZeroVector;
 TMap<FName,FSlateBrush> OriginalBrushes;
 TArray<TStrongObjectPtr<UTexture2D>> OriginalTextures;
 TArray<FPadmaOriginalMesh> ClockMeshes;
 TArray<TArray<FPadmaOriginalMesh>> HeroFrames;
 float HeroDuration=6.6667f;
 TMap<FName,FPadmaOriginalClip> HeroClips;
 float HeroTime=0;
 bool bHeroFrozen=false,bPointerPressed=false;
 FVector2D PointerDownPoint=FVector2D::ZeroVector;
 bool bGalleryCardDrag=false,bGalleryScrollDrag=false,bPointerMoved=false;
 FVector2D GalleryTilt=FVector2D::ZeroVector,GalleryTiltTarget=FVector2D::ZeroVector,DragStartTilt=FVector2D::ZeroVector;
 struct FCardDragPose { FVector2D Tilt=FVector2D::ZeroVector,Target=FVector2D::ZeroVector; };
 TMap<int32,FCardDragPose> BoardCardPoses;
 int32 BoardDragSlot=INDEX_NONE;
 void ReleaseCardDrag();
 float RevealHover=0,RevealTint=1,RevealTintFrom=1,RevealTintTarget=1,RevealTintTime=0;
 TSharedPtr<FJsonObject> StarConfig;
 TStrongObjectPtr<UFontFace> SerifFace;
 TSharedPtr<FCompositeFont> Serif;
 TArray<TStrongObjectPtr<UFontFace>> OriginalFontFaces;
 TMap<FName,TSharedPtr<FCompositeFont>> OriginalFonts;
 bool bSelection=true, bTitleIntro=false, bIntroFrozen=false;
 float IntroTime=0;
 FPadmaTarotSession Tarot;
 bool bGallery=false,bGalleryReverse=false;
 int32 GalleryCard=0,DetailSlot=INDEX_NONE,HoveredTab=INDEX_NONE;
 float GalleryFlip=.666667f,TabTime=.667f;
 TSharedPtr<FJsonObject> SourcePages,DetailGeometry;
 TMap<FName,TArray<FVector2D>> HeroEntryAngles;
 TMap<FName,TArray<FPadmaOriginalMesh>> WhitePanels;
 TMap<FName,FSlateBrush> PageEffectBrushes;
 TMap<FName,FPadmaOriginalMesh> PageEffectMeshes;
 TArray<TStrongObjectPtr<UMaterialInstanceDynamic>> PageEffectMaterials;
 bool bJudge=false,bHelp=false,bPageFrozen=false;
 float PageTime=0,GalleryScroll=0,GalleryTarget=0,DragStartScroll=0;
 FString PageAnimation;
 float PageActionDuration=0;
 int32 PageAction=0;
 void BeginPageAction(const TCHAR* Animation,float Duration,int32 Action);
 FString SourceHit(const FString& Page,FVector2D Hit)const;
 int32 CardAt(FVector2D Point)const;
 int32 GalleryCardAt(FVector2D Point)const;
 bool GalleryMainCardAt(FVector2D Point)const;
};
