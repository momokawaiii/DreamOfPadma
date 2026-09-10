#include "Game/Framework/PadmaPlayerController.h"
#include "Game/Content/PadmaContentCatalog.h"
#include "Game/Run/PadmaRunSubsystem.h"
#include "World/Map/PadmaWorldMapActor.h"
#include "World/Map/PadmaMapVisualTheme.h"
#include "Game/WorldMap/PadmaWorldMapAsset.h"
#include "Core/WorldMap/PadmaTutorialMap.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Gameplay/ACT/Authoring/PadmaACTAuthoring.h"
#include "Presentation/Models/PadmaModelPresentation.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Animation/AnimSequence.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Misc/PackageName.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Engine/GameViewportClient.h"
#include "HighResScreenshot.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Framework/Application/SlateApplication.h"

APadmaPlayerController::APadmaPlayerController()
{
	PrimaryActorTick.bCanEverTick=true;
}
FPadmaRunRules& APadmaPlayerController::Rules() const { return Session->Rules(); }
void APadmaPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if(GEngine)TravelFailureHandle=GEngine->OnTravelFailure().AddUObject(this,&APadmaPlayerController::TravelFailed);
	bShowMouseCursor=true;
	FInputModeGameAndUI Input;Input.SetHideCursorDuringCapture(false);Input.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);SetInputMode(Input);
	Catalog=LoadObject<UPadmaContentCatalog>(nullptr,TEXT("/Game/Padma/MVP/Playable/Definitions/DA_PlayableCatalog.DA_PlayableCatalog"));
	Session=GetGameInstance()->GetSubsystem<UPadmaRunSubsystem>();
	Screen=CreateWidget<UPadmaGameScreen>(this,UPadmaGameScreen::StaticClass());
	if(Screen) { Screen->OnIntent=[this](const FPadmaUIAction& A){ ReceiveIntent(A); };Screen->AddToViewport();Screen->ActivateWidget(); }
	FString Error;FPadmaContentSnapshot Content;
	if(!Catalog||!Catalog->Load(Content,Error))
	{
		ModalTitle=TEXT("内容加载失败");ModalText=Error.IsEmpty()?TEXT("先运行 AuthorPlayableDemo.py 创建可玩目录。"):Error;
		UE_LOG(LogTemp,Error,TEXT("[Padma046] %s"),*ModalText);
		FPadmaGameView V;V.Heading=TEXT("归梦 · UE Demo");V.ModalTitle=ModalTitle;V.ModalText=ModalText;if(Screen)Screen->Present(V);return;
	}
	const bool bExistingRun=Session->Rules().HasRun();
	if(!bExistingRun)
	{
		const FPadmaMapLayout AuthoredReference=Content.MapLayout;
		FString Scenario;FParse::Value(FCommandLine::Get(),TEXT("PadmaDemoScenario="),Scenario);
		const bool LegacyScenario=Scenario==TEXT("encounter")||Scenario==TEXT("act")||Scenario==TEXT("roundtrip");
		FPadmaMapLayout Tutorial;
		if(!LegacyScenario&&!FParse::Param(FCommandLine::Get(),TEXT("PadmaLegacyMap")))
		{
			if(FPadmaTutorialMapGenerator::Generate(Content.MapLayout,12345,Tutorial,Error))FPadmaWorldMapGenerator::ApplyToContent(Tutorial,Content,Error);
			if(!Error.IsEmpty())UE_LOG(LogTemp,Error,TEXT("[PadmaTutorial] %s"),*Error);
		}
		Session->Initialize(Content,&AuthoredReference);ApplyResult(Session->StartNew());
	}
	SelectedNode=Rules().HomeNode();
	MapSeedDraft=Rules().Run().MapLayout.Seed;
	Draft=Rules().Run().Preparation;
	bBattleMap=GetWorld()->GetMapName().Contains(TEXT("L_PadmaBattle"));
	if(!bBattleMap&&Rules().IsBattle()){Rules().FinishBattle(false);Notice=TEXT("未进入预期战斗关卡，已恢复战前状态。");}
	ViewCamera=GetWorld()->SpawnActor<ACameraActor>();
	SetViewTarget(ViewCamera);
	BuildEnvironment();
	if(bBattleMap&&Rules().IsBattle()) StartPendingBattle();
	else if(bBattleMap) { Travel(false);return; }
	else { MapView=GetWorld()->SpawnActor<APadmaWorldMapActor>();RefreshGraph(); }
	RefreshView();
	CaptureStarted=FPlatformTime::Seconds();
#if !UE_BUILD_SHIPPING
	FString Scenario;
	FParse::Value(FCommandLine::Get(),TEXT("PadmaDemoScenario="),Scenario);
	if(!bBattleMap&&!bExistingRun)
	{
		if(Scenario==TEXT("home")){Page=TEXT("home");HomeTab=TEXT("protagonist");RefreshStage();bNeedsRefresh=true;}
		else if(Scenario==TEXT("details")||Scenario==TEXT("layers"))
		{
			if(Scenario==TEXT("layers")){ReceiveIntent({TEXT("menu")});RefreshView();ReceiveIntent({TEXT("codex")});RefreshView();ReceiveIntent({TEXT("inspect-definition"),TEXT("dancer")});}
			else for(const auto& Card:Rules().Run().Cards)if(Card.DefId==TEXT("dancer")&&Card.Zone==TEXT("hand")){ReceiveIntent({TEXT("inspect-card"),Card.Id});break;}
			bNeedsRefresh=true;
		}
		else if(Scenario==TEXT("encounter")||Scenario==TEXT("act")||Scenario==TEXT("roundtrip"))
		{
			FName Dancer;
			for(const auto& Card:Rules().Run().Cards)if(Card.DefId==TEXT("dancer")&&Card.Zone==TEXT("hand")){Dancer=Card.Id;break;}
			if(Rules().Advance().bOk&&Rules().Deploy(Dancer,TEXT("home")).bOk&&Rules().Move({Dancer},TEXT("fire")).bOk&&Rules().Move({Dancer},TEXT("plain")).bOk)
				ApplyResult(Rules().Move({Dancer},TEXT("gate"),Scenario==TEXT("act")?FName(TEXT("act")):FName(TEXT("encounter"))));
		}
	}
	if(!Scenario.IsEmpty())UE_LOG(LogTemp,Display,TEXT("[PadmaUI] Scenario=%s page=%s existing=%d"),*Scenario,*Page.ToString(),bExistingRun);
	if(bBattleMap&&Scenario==TEXT("roundtrip")&&Combat&&Combat->IsBattleActive())
	{
		FTimerHandle SmokeReturn;
		GetWorld()->GetTimerManager().SetTimer(SmokeReturn,[this]{Combat->ExitBattle();UE_LOG(LogTemp,Display,TEXT("[Padma046] Roundtrip rollback pending=%d"),Rules().IsBattle());Travel(false);},1.f,false);
	}
#endif
	UE_LOG(LogTemp,Display,TEXT("[Padma046] Native playable ready: %s / cards %d"),*GetWorld()->GetMapName(),Rules().Run().Cards.Num());
}
void APadmaPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindKey(EKeys::LeftMouseButton,IE_Pressed,this,&APadmaPlayerController::MousePressed);
	InputComponent->BindKey(EKeys::LeftMouseButton,IE_Released,this,&APadmaPlayerController::MouseReleased);
	InputComponent->BindKey(EKeys::MouseScrollUp,IE_Pressed,this,&APadmaPlayerController::MapZoomIn);
	InputComponent->BindKey(EKeys::MouseScrollDown,IE_Pressed,this,&APadmaPlayerController::MapZoomOut);
	InputComponent->BindKey(EKeys::MiddleMouseButton,IE_Pressed,this,&APadmaPlayerController::MapDragStart);
	InputComponent->BindKey(EKeys::MiddleMouseButton,IE_Released,this,&APadmaPlayerController::MapDragEnd);
	InputComponent->BindKey(EKeys::Home,IE_Pressed,this,&APadmaPlayerController::FitMapCamera);
	InputComponent->BindKey(EKeys::Tab,IE_Pressed,this,&APadmaPlayerController::TabPressed);
	InputComponent->BindKey(EKeys::Escape,IE_Pressed,this,&APadmaPlayerController::EscapePressed);
	InputComponent->BindKey(EKeys::One,IE_Pressed,this,&APadmaPlayerController::Skill1);
	InputComponent->BindKey(EKeys::Two,IE_Pressed,this,&APadmaPlayerController::Skill2);
	InputComponent->BindKey(EKeys::Three,IE_Pressed,this,&APadmaPlayerController::Skill3);
	InputComponent->BindKey(EKeys::Four,IE_Pressed,this,&APadmaPlayerController::Skill4);
	InputComponent->BindKey(EKeys::Five,IE_Pressed,this,&APadmaPlayerController::Skill5);
}
void APadmaPlayerController::SetCamera(const FVector& Target,float Distance,float Pitch,float Yaw)
{
	if(!ViewCamera)return;
	const FRotator R(Pitch,Yaw,0);
	ViewCamera->SetActorRotation(R);ViewCamera->SetActorLocation(Target-R.Vector()*Distance);
	ViewCamera->GetCameraComponent()->SetFieldOfView(48);
	if(!bBattleMap)
	{
		auto& Settings=ViewCamera->GetCameraComponent()->PostProcessSettings;
		Settings.bOverride_AutoExposureBias=true;Settings.AutoExposureBias=-.4f;
	}
}
void APadmaPlayerController::BuildEnvironment()
{
	auto* Light=GetWorld()->SpawnActor<ADirectionalLight>(FVector(0,0,1000),FRotator(-55,-35,0));
	if(Light){Light->GetLightComponent()->SetIntensity(5);Light->GetLightComponent()->SetLightColor(bBattleMap?FLinearColor(1,.91f,.75f):FLinearColor(1,.98f,.91f));}
	auto* Sky=GetWorld()->SpawnActor<ASkyLight>();
	if(Sky){Sky->GetLightComponent()->SetIntensity(1);Sky->GetLightComponent()->SetMobility(EComponentMobility::Movable);}
	auto* Floor=GetWorld()->SpawnActor<AStaticMeshActor>(FVector(0,800,bBattleMap?-35:-280),FRotator::ZeroRotator);
	if(Floor)
	{
		auto* Mesh=Floor->GetStaticMeshComponent();
		Mesh->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube")));
		Mesh->SetWorldScale3D(FVector(90,90,.4));Mesh->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
		if(auto* Mat=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Padma/MVP/Playable/Presentation/M_PadmaGround.M_PadmaGround")))Mesh->SetMaterial(0,Mat);
	}
}
void APadmaPlayerController::ClearDisplays()
{
	for(APadmaModelPresentationActor* A:Displays)if(IsValid(A))A->Destroy();Displays.Reset();
	if(PreviewScene){PreviewScene->SetShouldBeLoaded(false);PreviewScene->SetShouldBeVisible(false);PreviewScene->SetIsRequestingUnloadAndRemoval(true);PreviewScene=nullptr;}
}
void APadmaPlayerController::RefreshGraph()
{
	if(!MapView||!Session||!Rules().HasRun())return;
	const auto& R=Rules().Run();const auto& D=Rules().Content();
	if(R.MapLayout.bHexWilderness){MapView->SetVisible(false);return;}
	MapView->ConfigureTheme(Catalog->MapVisualTheme.LoadSynchronous(),R.MapLayout.Seed);
	TArray<FPadmaWorldNodeView> Nodes;TArray<FPadmaWorldEdgeView> Edges;
	for(FName Id:D.NodeOrder)
	{
		const auto* Def=D.Nodes.Find(Id);const auto* State=R.Nodes.Find(Id);if(!Def||!State)continue;
		FPadmaWorldNodeView V;V.Id=Id;V.Name=Def->DisplayName;V.Position=APadmaWorldMapActor::NodeLocation(Def->Position);
		V.Terrain=Def->Terrain;V.Type=Def->Type;V.Owner=State->Owner;V.AnchorKind=Def->AnchorKind;
		V.NPCId=Def->NPCId;V.bAnchorCompleted=R.CompletedNPCs.Contains(Def->NPCId);
		V.Color=State->Owner==TEXT("player")?FLinearColor(.12f,.3f,.22f):State->Owner==TEXT("ruler")?FLinearColor(.38f,.16f,.1f):FLinearColor(.21f,.25f,.19f);
		int32 Count=0;for(const auto& C:R.Cards)if(C.Zone==TEXT("world")&&C.Node==Id)++Count;
		V.FriendlyCount=Count;V.EnemyCount=State->Owner==TEXT("ruler")?Def->EnemyCount:0;
		V.Detail=FString::Printf(TEXT("%s · 我方 %d · 敌方 %d"),*Def->Terrain.ToString(),Count,State->Owner==TEXT("ruler")?Def->EnemyCount:0);Nodes.Add(V);
	}
	for(const auto& E:D.Edges)Edges.Add({E.From,E.To,E.UnlockFlag.IsNone()||R.Unlocks.Contains(E.UnlockFlag)});
	MapView->ShowGraph(Nodes,Edges,SelectedNode);
	MapView->SetHoveredNode(HoverNode);
	const auto* Moving=SelectedCards.Num()?Rules().Card(SelectedCards[0]):nullptr;
	MapView->SetMoveSource(Moving?Moving->Node:NAME_None);
	if(Page==TEXT("map")){if(!bMapCameraReady)FitMapCamera();else SetCamera(MapCameraTarget,MapCameraDistance,-55,MapCameraYaw);}
}
void APadmaPlayerController::RefreshStage()
{
	ClearDisplays();
	if(Page==TEXT("map")){if(MapView)MapView->SetVisible(true);RefreshGraph();return;}
	if(MapView)MapView->SetVisible(false);
	SetCamera(FVector(0,3650,90),900,-15,0);
	auto Spawn=[this](FName ModelId,FVector Where,float Scale=1.f)
	{
		auto* A=GetWorld()->SpawnActor<APadmaModelPresentationActor>(Where,FRotator(0,180,0));if(!A)return;
		auto P=Catalog->ResolveModel(ModelId);
		if(!P.bVisualReady)
		{
			P.bVisualReady=true;P.StaticModel=LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
			P.ModelTransform=FTransform(FRotator::ZeroRotator,FVector(0,0,75),FVector(.5,.5,1.5));
		}
		A->ApplyPresentation(P);A->SetActorScale3D(FVector(Scale));Displays.Add(A);
		if(const auto* Binding=Catalog->FindModelBinding(ModelId))
			if(auto* Pose=Binding->Pose.LoadSynchronous())
				if(P.SkeletalModel&&Pose->GetSkeleton()==P.SkeletalModel->GetSkeleton())A->SkeletalVisual->PlayAnimation(Pose,true);
	};
	const auto& D=Rules().Content();
	if(Page==TEXT("scene"))
	{
		if(const auto* Node=Catalog->FindNodePresentation(SelectedNode))
		{
			if(!Node->Scene.IsNull())
			{
				bool Loaded=false;PreviewScene=ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(this,Node->Scene,FVector(0,3650,0),FRotator::ZeroRotator,Loaded);
				if(!Loaded)Notice=TEXT("地块场景加载失败，请检查 Scene 软引用。");
			}
			if(!Node->GroundModelId.IsNone())Spawn(Node->GroundModelId,FVector(0,3650,-20));
			if(!Node->BuildingModelId.IsNone())Spawn(Node->BuildingModelId,FVector(0,3800,0),1.2f);
		}
		int32 i=0;for(const auto& C:Rules().Run().Cards)if(C.Zone==TEXT("world")&&C.Node==SelectedNode)
			if(const auto* Def=D.Cards.Find(C.DefId))Spawn(Def->ModelId,FVector(0,3520+140*i++,0));
		if(SelectedNode==Rules().HomeNode())if(const auto* Def=D.HomeCharacters.Find(Rules().Run().Preparation.HomeCharacter))Spawn(Def->ModelId,FVector(0,3650,0));
	}
	else if(HomeTab==TEXT("other"))
	{
		if(PreparationMode==TEXT("act"))
		{
			const auto* Row=D.ACTCharacters.Find(Draft.ACTCharacter);if(Row)Spawn(Row->ModelId,FVector(0,3650,0));
			if(auto* Def=Catalog->ResolveACTCharacter(Draft.ACTCharacter);Def&&!Def->Model.IsNull()&&Displays.Num())
			{
				FPadmaResolvedPresentation P;P.SkeletalModel=Def->Model.LoadSynchronous();P.AnimationClass=Def->AnimationClass.LoadSynchronous();P.bVisualReady=P.SkeletalModel!=nullptr;
				if(P.bVisualReady)Displays.Last()->ApplyPresentation(P);
			}
			if(const auto* W=D.ACTWeapons.Find(Draft.ACTWeapon))Spawn(W->ModelId,FVector(0,3740,0),.7f);
		}
		else
		{
			if(const auto* C=D.FPSCharacters.Find(Draft.FPSCharacter))Spawn(C->ModelId,FVector(0,3650,0));
			if(const auto* W=D.FPSWeapons.Find(Draft.FPSWeapon))Spawn(W->ModelId,FVector(0,3740,0),.7f);
		}
	}
	else
	{
		if(const auto* C=D.HomeCharacters.Find(Draft.HomeCharacter))Spawn(C->ModelId,FVector(0,3650,0));
		// An authored outfit model replaces the body; an empty fixture keeps the labelled placeholder.
		if(const auto* O=D.HomeOutfits.Find(Draft.HomeOutfit);O&&!O->ModelId.IsNone()&&Catalog->ResolveModel(O->ModelId).SkeletalModel&&Displays.Num())
			Displays[0]->ApplyPresentation(Catalog->ResolveModel(O->ModelId));
		if(const auto* W=D.HomeWeapons.Find(Draft.HomeWeapon))Spawn(W->ModelId,FVector(0,3740,0),.7f);
		if(const auto* P=D.HomePoses.Find(Draft.HomePose))
			if(const auto* B=Catalog->FindModelBinding(P->ModelId))
				if(auto* Pose=B->Pose.LoadSynchronous();Pose&&Displays.Num()&&Displays[0]->Presentation.SkeletalModel&&Pose->GetSkeleton()==Displays[0]->Presentation.SkeletalModel->GetSkeleton())
					Displays[0]->SkeletalVisual->PlayAnimation(Pose,true);
	}
}
FName APadmaPlayerController::HitNode() const
{
	if(Session&&Rules().HasRun()&&Rules().Run().MapLayout.bHexWilderness&&Screen)return Screen->HitPaintedNode();
	FHitResult Hit;const_cast<APadmaPlayerController*>(this)->GetHitResultUnderCursor(ECC_Visibility,true,Hit);
	if(const auto* T=Cast<APadmaWorldNodeActor>(Hit.GetActor()))return T->NodeId;return NAME_None;
}
FName APadmaPlayerController::HitUnit() const
{
	FHitResult Hit;const_cast<APadmaPlayerController*>(this)->GetHitResultUnderCursor(ECC_Visibility,true,Hit);
	if(const auto* U=Cast<APadmaCombatUnit>(Hit.GetActor()))return U->Spec.Id;return NAME_None;
}
FVector APadmaPlayerController::HitGround() const
{
	FVector Start,Dir;const_cast<APadmaPlayerController*>(this)->DeprojectMousePositionToWorld(Start,Dir);
	if(FMath::Abs(Dir.Z)>KINDA_SMALL_NUMBER)return Start+Dir*((90-Start.Z)/Dir.Z);return FVector::ZeroVector;
}
void APadmaPlayerController::MousePressed()
{
 bMouseHeld=!HasBlockingOverlay()&&Screen&&Screen->IsWorldPointerAvailable();bMouseInspected=false;MouseDownTime=FPlatformTime::Seconds();
 float X=0,Y=0;GetMousePosition(X,Y);WorldPressPosition=FVector2D(X,Y);WorldPressUnit=HitUnit();
}
void APadmaPlayerController::MouseReleased()
{
	const bool WasHeld=bMouseHeld;bMouseHeld=false;
	if(!WasHeld||bMouseInspected||HasBlockingOverlay()||!Screen||!Screen->IsWorldPointerAvailable()||!Session||!Rules().HasRun())return;
	if(Combat&&Combat->IsBattleActive())
	{
		FName Id=HitUnit();
		if(bAttackArmed||!ArmedSkill.IsNone())SelectBattleTarget(Id);
		else if(Combat->GetMode()==EPadmaCombatMode::ACT)Combat->MoveTo(HitGround());
	}
	else if(Page==TEXT("map"))
	{
		// Release is still inside the viewport's capture callback. The held world press
		// and current HUD/modal checks above establish ownership without rejecting that capture.
		if(Rules().IsDialogue())return;
		const FName Id=HitNode();if(Id.IsNone())return;
		SelectMapTarget(Id);
	}
}
void APadmaPlayerController::TabPressed()
{
	if(!HasBlockingOverlay()&&Combat&&Combat->IsBattleActive()&&!bBattleFinished)
	{Combat->SetSkillLibraryOpen(!Combat->IsSkillLibraryOpen());CancelTargeting();bNeedsRefresh=true;}
}
void APadmaPlayerController::EscapePressed()
{
	if(Session&&Rules().IsDialogue())ApplyResult(Rules().DialogueCancel());
	else if(!OverlayContext().Key().IsNone())CloseOverlay();
	else if(!ArmedSkill.IsNone()||bAttackArmed)CancelTargeting();
	else if(Combat&&Combat->IsSkillLibraryOpen())TabPressed();
	else if(Page!=TEXT("map")&&!bBattleMap)NavigateTo(TEXT("map"));
	else if(!bBattleFinished){ModalTitle=TEXT("菜单");ModalText=TEXT("进度与游戏选项");}
	bNeedsRefresh=true;
}
void APadmaPlayerController::Skill1(){ChooseSkill(0);}void APadmaPlayerController::Skill2(){ChooseSkill(1);}
void APadmaPlayerController::Skill3(){ChooseSkill(2);}void APadmaPlayerController::Skill4(){ChooseSkill(3);}void APadmaPlayerController::Skill5(){ChooseSkill(4);}
void APadmaPlayerController::ChooseSkill(int32 Slot)
{
	if(!HasBlockingOverlay()&&Session&&Rules().Run().SkillSlots.IsValidIndex(Slot))ReceiveIntent({TEXT("skill"),Rules().Run().SkillSlots[Slot]});
}
void APadmaPlayerController::CancelTargeting(){ArmedSkill=NAME_None;SkillTarget=NAME_None;bAttackArmed=false;AttackTargets.Reset();bNeedsRefresh=true;}
void APadmaPlayerController::ApplyResult(const FPadmaRunCommandResult& Result)
{
	Notice=Result.Text;bNeedsRefresh=true;
	if(!Result.bOk)UE_LOG(LogTemp,Warning,TEXT("[Padma046] Command: %s"),*Notice);
	if(Result.bOk&&Result.Effect==EPadmaRunCommandEffect::BattleRequested)Travel(true);
}
void APadmaPlayerController::Travel(bool ToBattle)
{
	if(bTravelling)return;
	const FString Package=ToBattle?TEXT("/Game/Padma/MVP/Playable/Maps/L_PadmaBattle"):TEXT("/Game/Padma/MVP/Playable/Maps/L_PadmaWorld");
	if(!FPackageName::DoesPackageExist(Package)){TravelFailed(GetWorld(),ETravelFailure::PackageMissing,TEXT("缺少地图：")+Package);return;}
	bTravelling=true;UGameplayStatics::OpenLevel(this,FName(*Package));
}
void APadmaPlayerController::TravelFailed(UWorld* FailedWorld,ETravelFailure::Type FailureType,const FString& Error)
{
	if(FailedWorld&&FailedWorld!=GetWorld())return;
	bTravelling=false;
	if(Session&&Rules().IsBattle())Rules().FinishBattle(false);
	Notice=TEXT("关卡切换失败，已恢复可操作状态：")+Error;bNeedsRefresh=true;
	UE_LOG(LogTemp,Warning,TEXT("[Padma046] %s"),*Notice);
}
void APadmaPlayerController::PlayerTick(float Delta)
{
	Super::PlayerTick(Delta);
	TickMapCamera(Delta);
	if(!Session||!Rules().HasRun()||bTravelling)return;
	if(bMouseHeld&&!HasBlockingOverlay()&&!bMouseInspected)
	{
		float X=0,Y=0;GetMousePosition(X,Y);const FVector2D Position(X,Y);
		if(FVector2D::Distance(Position,WorldPressPosition)>12)bMouseHeld=false;
		else if(FPlatformTime::Seconds()-MouseDownTime>=.5&&!WorldPressUnit.IsNone()&&HitUnit()==WorldPressUnit){bMouseInspected=true;ReceiveIntent({TEXT("inspect-unit"),WorldPressUnit});}
	}
	if(Combat&&Combat->IsBattleActive())
	{
		const bool ACT=Combat->GetMode()==EPadmaCombatMode::ACT;
		if(ACT)
		{
			FVector2D V((IsInputKeyDown(EKeys::D)||IsInputKeyDown(EKeys::Right)?1:0)-(IsInputKeyDown(EKeys::A)||IsInputKeyDown(EKeys::Left)?1:0),
				(IsInputKeyDown(EKeys::S)||IsInputKeyDown(EKeys::Down)?1:0)-(IsInputKeyDown(EKeys::W)||IsInputKeyDown(EKeys::Up)?1:0));
			Combat->SetMoveInput(HasBlockingOverlay()?FVector2D::ZeroVector:V);
			Combat->SetAttackHeld(!HasBlockingOverlay()&&IsInputKeyDown(EKeys::SpaceBar));
		}
		if(LastActor!=Combat->GetActionOwner()){LastActor=Combat->GetActionOwner();AttackTargetCount=1;CancelTargeting();}
	}
	else if(Page==TEXT("map"))
	{
		if(!bMouseHeld&&!bMapDrag&&!FSlateApplication::Get().HasAnyMouseCaptor())UpdateMapHover(IsMapPointerAvailable()?HitNode():NAME_None);
	}
	if(LastRevision!=Rules().Revision()){LastRevision=Rules().Revision();bNeedsRefresh=true;if(!bBattleMap)RefreshGraph();}
	// A hand release must reach its original widget before an ordinary refresh can
	// replace it. Modal activation (including hold inspection) may interrupt capture.
	const bool PointerBusy=bMouseHeld||bMapDrag||(FSlateApplication::IsInitialized()&&FSlateApplication::Get().HasAnyMouseCaptor());
	const bool Rebuild=bNeedsRefresh&&(!PointerBusy||HasBlockingOverlay());
	if(Rebuild){bNeedsRefresh=false;bNeedsMapDetails=false;RefreshView();}
	else if(bNeedsMapDetails&&!bNeedsRefresh&&!PointerBusy){bNeedsMapDetails=false;RefreshMapDetails();}
	// Project Chinese labels through the UI font fallback, independently of world model fonts.
	const double LabelNow=FPlatformTime::Seconds();
	if(Screen&&(Rebuild||LabelNow-LastLabelRefresh>.05))
	{
		LastLabelRefresh=LabelNow;TArray<FPadmaWorldLabelView> Labels;
		const float Scale=FMath::Max(.01f,UWidgetLayoutLibrary::GetViewportScale(this));
		auto Add=[&](const FVector& World,const FString& Text,FLinearColor Color)
		{
			FVector2D Position;if(ProjectWorldLocationToScreen(World,Position,true))Labels.Add({Text,Position/Scale,Color});
		};
		if(Combat&&Combat->IsBattleActive())
		{
			for(APadmaCombatUnit* U:Combat->GetUnits())if(IsValid(U))
			{
				const auto S=U->Snapshot();
				Add(U->GetActorLocation()+FVector(0,0,125),FString::Printf(TEXT("%s%s\n%.0f/%.0f  归墟 %.0f  抗衡 %.0f"),LastActor==S.Id?TEXT("正在行动 · "):TEXT(""),*S.DisplayName.ToString(),S.Health,S.MaxHealth,S.Shield,S.Block),S.bPlayer?FLinearColor(.55f,1,.7f):FLinearColor(1,.65f,.4f));
			}
		}
		else if(Page==TEXT("map")&&!bBattleMap&&!Rules().Run().MapLayout.bHexWilderness)
			for(const auto& Pair:Rules().Content().Nodes)
			{
				const auto& N=Pair.Value;
				Add(APadmaWorldMapActor::NodeLocation(N.Position)+FVector(0,0,160),N.DisplayName.ToString(),Pair.Key==SelectedNode?FLinearColor(.7f,1,.75f):FLinearColor(.93f,.91f,.78f));
			}
		Screen->PresentLabels(Labels);
	}
#if !UE_BUILD_SHIPPING
	FString Capture;
	if(CaptureStarted>0&&FParse::Value(FCommandLine::Get(),TEXT("PadmaPlayableCapture="),Capture)&&FPlatformTime::Seconds()-CaptureStarted>8)
	{
		UE_LOG(LogTemp,Display,TEXT("[PadmaUI] Capture page=%s battle=%d overlays=%d active=%s"),*Page.ToString(),bBattleMap,Screen?Screen->GetOverlayDepth():0,Screen?*Screen->GetActiveOverlayKey().ToString():TEXT("none"));
		CaptureStarted=0;FScreenshotRequest::RequestScreenshot(Capture,true,false);
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]{GetWorld()->GetTimerManager().SetTimerForNextTick([]{FPlatformMisc::RequestExit(false);});});
	}
#endif
}
void APadmaPlayerController::EndPlay(const EEndPlayReason::Type Reason)
{
	if(GEngine)GEngine->OnTravelFailure().Remove(TravelFailureHandle);
	if(Screen){Screen->ShutdownPresentation();Screen=nullptr;}
	if(Combat){Combat->OnFinished=nullptr;Combat->OnReceipt.Clear();Combat->OnChanged.Clear();Combat->ExitBattle();}
	// Closing the game during a battle must not leave a pending transaction in a surviving PIE instance.
	if(Session&&Rules().IsBattle()&&!bTravelling)Rules().FinishBattle(false);
	ClearDisplays();Super::EndPlay(Reason);
}
