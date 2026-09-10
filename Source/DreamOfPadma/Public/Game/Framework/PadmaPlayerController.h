#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Engine/EngineBaseTypes.h"
#include "Core/Run/PadmaRunRules.h"
#include "UI/Screens/PadmaGameScreen.h"
#include "Gameplay/Combat/PadmaCombatTypes.h"
#include "PadmaPlayerController.generated.h"

class UPadmaContentCatalog;
class UPadmaRunSubsystem;
class UPadmaCombatComponent;
class APadmaWorldMapActor;
class ACameraActor;
class APadmaModelPresentationActor;
class UTextRenderComponent;
class ULevelStreamingDynamic;
struct FPadmaOverlayContext
{
 FString Title,Text;
 FName Card,Unit,Definition,ACTCharacter;
 FName Key() const
 {
  if(!Card.IsNone())return FName(*(TEXT("card:")+Card.ToString()));
  if(!Unit.IsNone())return FName(*(TEXT("unit:")+Unit.ToString()));
  if(!Definition.IsNone())return FName(*(TEXT("definition:")+Definition.ToString()));
  if(!ACTCharacter.IsNone())return FName(*(TEXT("character:")+ACTCharacter.ToString()));
  return FName(*Title);
 }
 bool HasDetails()const{return !Card.IsNone()||!Unit.IsNone()||!Definition.IsNone()||!ACTCharacter.IsNone();}
};

/** Composition and input adapter: durable rule state stays in the run subsystem. */
UCLASS()
class DREAMOFPADMA_API APadmaPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	APadmaPlayerController();
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float Delta) override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	void ReceiveIntent(const FPadmaUIAction& Action);
private:
	friend class FPadmaStrategyInteractionTest;
	friend class FPadmaPlacementInteractionTest;
	UPROPERTY() TObjectPtr<UPadmaContentCatalog> Catalog;
	UPROPERTY() TObjectPtr<UPadmaRunSubsystem> Session;
	UPROPERTY() TObjectPtr<UPadmaGameScreen> Screen;
	UPROPERTY() TObjectPtr<UPadmaCombatComponent> Combat;
	UPROPERTY() TObjectPtr<APadmaWorldMapActor> MapView;
	UPROPERTY() TObjectPtr<ACameraActor> ViewCamera;
	UPROPERTY() TArray<TObjectPtr<APadmaModelPresentationActor>> Displays;
	UPROPERTY() TMap<FName,TObjectPtr<UTextRenderComponent>> UnitLabels;
	UPROPERTY() TObjectPtr<ULevelStreamingDynamic> PreviewScene;
	FName SelectedNode=TEXT("home");
	FName HoverNode;
	FName PreviewNode;
	TArray<FName> SelectedCards;
	FName SelectedHand;
	FName PendingPlacementCard, PendingPlacementNode, PendingPlacementFamily;
	FName HandFilter;
	FName SynthesisSource;
	FName BattleChoice=TEXT("encounter");
	FName Page=TEXT("map");
	FName HomeTab=TEXT("node");
	FName PreparationMode=TEXT("act");
	FPadmaPreparationState Draft;
	FName ArmedSkill;
	FName SkillTarget;
	bool bAttackArmed=false;
	int32 AttackTargetCount=1;
	TArray<FName> AttackTargets;
	FName DetailUnit;
	FName DetailCard, DetailDefinition, DetailACTCharacter;
	TArray<FPadmaOverlayContext> OverlayHistory;
	FVector2D WorldPressPosition=FVector2D::ZeroVector;
	FName WorldPressUnit;
	FString ModalTitle;
	FString ModalText;
	FString Notice;
	bool bNeedsRefresh=true;
	bool bNeedsMapDetails=false;
	bool bBattleMap=false;
	bool bTravelling=false;
	bool bMouseHeld=false;
	bool bMouseInspected=false;
	bool bBattleFinished=false;
	double MouseDownTime=0;
	double LastHomeClick=0;
	double LastFeedbackTime=0;
	double CaptureStarted=0;
	double LastLabelRefresh=0;
	uint64 LastRevision=MAX_uint64;
	FName LastActor;
	FVector MapCameraTarget = FVector::ZeroVector;
	float MapCameraDistance = 4000;
	float MapCameraYaw = 0;
	bool bMapCameraReady = false;
	bool bMapDrag = false;
	FVector2D MapDragPosition = FVector2D::ZeroVector;
	int32 MapSeedDraft = 12345;
	FDelegateHandle TravelFailureHandle;
	TArray<FPadmaCombatUnitSnapshot> FinishedUnits;
	FPadmaRunRules& Rules() const;
	void RefreshView();
	void RefreshMapDetails();
	void RefreshGraph();
	void RefreshStage();
	void ClearDisplays();
	void SetCamera(const FVector& Target, float Distance, float Pitch, float Yaw);
	void FitMapCamera();
	void TickMapCamera(float Delta);
	void MapZoomIn();
	void MapZoomOut();
	void MapDragStart();
	void MapDragEnd();
	bool IsMapPointerAvailable() const;
	bool IsMapKeyboardAvailable() const;
	bool HasBlockingOverlay() const;
	void NavigateTo(FName Destination);
	FName SelectedACard(FName Node=NAME_None) const;
	void StartConfiguredMap(bool bGenerate);
	void StartTutorialMap();
	void UpdateMapHover(FName Node);
	FName MapDetailNode() const;
	void BuildEnvironment();
	void StartPendingBattle();
	void Travel(bool bToBattle);
	void TravelFailed(UWorld* FailedWorld,ETravelFailure::Type FailureType,const FString& Error);
	void OnBattleFinished(bool Won,const TArray<FPadmaCombatUnitSnapshot>& Units);
	void PlayReceipt(const FPadmaCombatReceipt& Receipt);
	FName HitNode() const;
	FName HitUnit() const;
	FVector HitGround() const;
	void MousePressed();
	void MouseReleased();
	void TabPressed();
	void EscapePressed();
	void Skill1();void Skill2();void Skill3();void Skill4();void Skill5();
	void ChooseSkill(int32 Slot);
	void SelectBattleTarget(FName Id);
	void CancelTargeting();
	void ApplyResult(const FPadmaRunCommandResult& Result);
	FPadmaUIEntry Button(const FString& Title,FName Command,FName Id=NAME_None,FName Other=NAME_None,const FString& Description=TEXT(""),bool Enabled=true,bool Selected=false) const;
	FString CardDescription(FName InstanceId) const;
	void BuildWorldView(FPadmaGameView& View, bool bDetailsOnly=false);
	void SelectMapTarget(FName Node);
	void BeginPlacement(FName Card, FName Node);
	void ConfirmPlacement(FName Anchor=NAME_None);
	void ClearPlacement();
	bool IsPlacementOverlay() const;
	FPadmaRunCommandResult PreviewPlacement(FName Card, FName Node, FName Anchor=NAME_None) const;
	void BuildPlacementView(FPadmaGameView& View) const;
	static float WarPlayerShare(double War, double Goal);
	void BuildHomeView(FPadmaGameView& View);
	void BuildCombatView(FPadmaGameView& View);
	void BuildInspectionView(FPadmaGameView& View) const;
	FPadmaOverlayContext OverlayContext()const;
	void RestoreOverlay(const FPadmaOverlayContext& Context);
	void CloseOverlay();
	void UpdateInspectionGate();
	bool IsOverlayIntentAllowed(const FPadmaUIAction& Action) const;
};
