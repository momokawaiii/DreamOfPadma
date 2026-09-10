#pragma once
#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Core/Content/PadmaContentTypes.h"
#include "PadmaGameScreen.generated.h"

class SVerticalBox;
class SOverlay;
class SConstraintCanvas;
class SBox;
class UTexture2D;
class UCommonActivatableWidgetStack;
class UPadmaActivatableLayer;
class UMaterialInstanceDynamic;
class SPadmaTutorialMap;
class SScrollBox;
class SPadmaWarBalance;
struct FSlateBrush;
struct FPadmaUIAction
{
	FName Command;
	FName Id;
	FName Other;
	bool bDropped=false;
	FVector2D ScreenPosition=FVector2D::ZeroVector;
};
struct FPadmaUIEntry
{
	FString Title;
	FString Description;
	FPadmaUIAction Action;
	FPadmaUIAction InspectAction;
	bool bEnabled=true;
	bool bSelected=false;
	bool bDraggable=false;
	bool bInspectable=false;
	bool bPrimary=false;
	bool bCard=false;
	int32 ArtIndex=INDEX_NONE;
	FString Badge;
};
struct FPadmaResourceView
{
	FString Label, Value, Hint;
	FLinearColor Color=FLinearColor::White;
};
struct FPadmaMinimapNode
{
	FName Id;
	FVector2D Position=FVector2D::ZeroVector;
	FLinearColor Color=FLinearColor::White;
	bool bSelected=false;
	FString Title;
	FName Owner;
	bool bWilderness=false, bHabitable=true, bHovered=false;
	int32 HexQ=0, HexR=0, Garrison=0, ArtIndex=INDEX_NONE;
};
struct FPadmaMinimapEdge { int32 From=0, To=0; bool bLocked=false; };
struct FPadmaDetailSection { FString Title, Text; };
struct FPadmaGameView
{
	bool bDetailOnly=false;
	FPadmaHexGridConfig HexGrid;
	FName PageKey=TEXT("map");
	TArray<FName> OverlayPath;
	FString Heading;
	FString Status;
	FString Notice;
	FString LeftTitle;
	FString RightTitle;
	FString RightText;
	FString Footer;
	FString HandTitle=TEXT("技能卡");
	FString HandHint=TEXT("选择目标后确认释放");
	TArray<FPadmaUIEntry> Navigation;
	TArray<FPadmaUIEntry> Left;
	TArray<FPadmaUIEntry> Right;
	TArray<FPadmaUIEntry> Hand;
	FString ModalTitle;
	FString ModalText;
	UTexture2D* ModalPortrait=nullptr;
	int32 ModalArtIndex=INDEX_NONE;
	FString ModalSubtitle, PortraitCaption;
	TArray<FPadmaDetailSection> DetailSections;
	TArray<FPadmaUIEntry> ModalCards;
	int32 NodeArtIndex=INDEX_NONE;
	TArray<FPadmaUIEntry> ModalActions;
	bool bBlurWorld=false;
	bool bWorldMap=false;
	bool bPaintedTutorial=false;
	FName DetailNode, MoveSource;
	FString TerrainText, OwnershipText, NodeStateText, EnemyName, MovementRoute;
	int32 EnemyCount=0;
	double EnemyHealth=0;
	FVector2D NodeUV=FVector2D(.5,.5);
	TArray<FPadmaUIEntry> MovingCards;
	bool bMapSeedInput=false;
	int32 MapSeed=12345;
	FString CalendarTitle, DayText, PhaseHint, WarText, ObjectiveText;
	int32 PhaseIndex=0;
	float WarProgress=0;
	TArray<FPadmaResourceView> Resources;
	TArray<FPadmaUIEntry> HandFilters, HandActions;
	FPadmaUIEntry PhaseAction;
	int32 HandCount=0, DeckCount=0, DiscardCount=0;
	TArray<FPadmaMinimapNode> MinimapNodes;
	TArray<FPadmaMinimapEdge> MinimapEdges;
};
struct FPadmaWorldLabelView
{
	FString Text;
	FVector2D Position;
	FLinearColor Color=FLinearColor::White;
};

/** Native CommonUI shell. It sends intent callbacks and owns no game state. */
UCLASS()
class DREAMOFPADMA_API UPadmaGameScreen : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	TFunction<void(const FPadmaUIAction&)> OnIntent;
	void Present(const FPadmaGameView& View);
	void PresentLabels(const TArray<FPadmaWorldLabelView>& Labels);
	bool IsWorldPointerAvailable() const;
	FVector2D GetWorldAreaSize() const;
	FVector2D GetWorldAreaCenter() const;
	bool HasModal() const { return !Current.ModalTitle.IsEmpty(); }
	int32 GetOverlayDepth() const;
	FName GetActiveOverlayKey() const;
	FName HitPaintedNode() const;
	void ZoomPaintedMap(float Multiplier);
	void PanPaintedMap(FVector2D Delta);
	void ResetPaintedMap();
	void ShutdownPresentation();
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual void NativeTick(const FGeometry& Geometry,float Delta) override;
private:
	friend class FPadmaHUDRetentionTest;
	friend class FPadmaUISessionReleaseTest;
	TSharedPtr<SOverlay> Layout;
	TSharedPtr<SOverlay> RootLayout;
	UPROPERTY(Transient) TObjectPtr<UCommonActivatableWidgetStack> PageStack;
	UPROPERTY(Transient) TObjectPtr<UCommonActivatableWidgetStack> OverlayStack;
	UPROPERTY(Transient) TObjectPtr<UPadmaActivatableLayer> PageLayer;
	UPROPERTY(Transient) TArray<TObjectPtr<UPadmaActivatableLayer>> OverlayLayers;
	TSharedPtr<SConstraintCanvas> Markers;
	TSharedPtr<SBox> WorldArea;
	TSharedPtr<SPadmaTutorialMap> PaintedMap;
	TSharedPtr<SPadmaTutorialMap> TutorialMini;
	TSharedPtr<SBox> TutorialHeader, TutorialDetails, TutorialHand, TutorialPhase;
	TSharedPtr<SBox> GenericDetails;
	TSharedPtr<SScrollBox> HandScroll;
	TSharedPtr<SPadmaWarBalance> WarBalance;
	UPROPERTY(Transient) TArray<TObjectPtr<UMaterialInstanceDynamic>> SkinMaterials;
	UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> WarMaterial;
	UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> RevealMaterial;
	UPROPERTY(Transient) TArray<TObjectPtr<UMaterialInstanceDynamic>> PortraitMaterials;
	UPROPERTY(Transient) TObjectPtr<UTexture2D> TutorialTexture;
	UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> TutorialMaterial;
	TSharedPtr<FSlateBrush> TutorialBrush;
	FPadmaGameView Current;
	FPadmaGameView DeferredView;
	bool bDeferredView=false;
	float PortraitRevealTime=1;
	UPROPERTY(Transient) TObjectPtr<UTexture2D> CurrentPortrait;
	UPROPERTY(Transient) TObjectPtr<UTexture2D> CardAtlas;
	UPROPERTY(Transient) TObjectPtr<UTexture2D> PanelTexture;
	UPROPERTY(Transient) TObjectPtr<UTexture2D> ButtonTexture;
	TSharedPtr<FSlateBrush> PortraitBrush;
	TArray<TSharedPtr<FSlateBrush>> ViewBrushes;
	void Build();
	void BuildTutorial();
	TSharedRef<SWidget> BuildTutorialHeader();
	TSharedRef<SWidget> BuildTutorialDetails();
	TSharedRef<SWidget> BuildTutorialHand();
	TSharedRef<SWidget> BuildTutorialPhase();
	TSharedRef<SWidget> BuildGenericDetails();
	void SyncLayers();
	TSharedRef<SWidget> BuildOverlay();
	TSharedPtr<FSlateBrush> ArtBrush(int32 Index, UTexture2D* Portrait=nullptr);
	TSharedRef<SWidget> Entry(const FPadmaUIEntry& Value);
	void Intent(const FPadmaUIAction& Action);
};
