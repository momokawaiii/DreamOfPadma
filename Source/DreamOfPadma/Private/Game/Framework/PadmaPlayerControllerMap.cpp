#include "Game/Framework/PadmaPlayerController.h"
#include "Game/Content/PadmaContentCatalog.h"
#include "Game/Run/PadmaRunSubsystem.h"
#include "Game/WorldMap/PadmaWorldMapAsset.h"
#include "World/Map/PadmaWorldMapActor.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Core/WorldMap/PadmaTutorialMap.h"
#include "Framework/Application/SlateApplication.h"

namespace
{
bool CardPointerCaptured(){return FSlateApplication::IsInitialized()&&FSlateApplication::Get().HasAnyMouseCaptor();}
}

FName APadmaPlayerController::MapDetailNode()const
{
    return Session&&Rules().Content().Nodes.Contains(PreviewNode)?PreviewNode:SelectedNode;
}
void APadmaPlayerController::UpdateMapHover(FName Node)
{
    if(!Session||!Rules().HasRun()||bBattleMap||Page!=TEXT("map")||HasBlockingOverlay()||bMouseHeld||bMapDrag||CardPointerCaptured())return;
    if(!Node.IsNone()&&!Rules().Content().Nodes.Contains(Node))return;
    if(HoverNode==Node)return;
    HoverNode=Node;
    // Hover may highlight a 3D tile, but only a click changes the pinned sidebar target.
    if(MapView)MapView->SetHoveredNode(Node);
}

bool APadmaPlayerController::IsMapPointerAvailable() const
{
    return !bBattleMap && Page==TEXT("map") && Session && !HasBlockingOverlay() && !bMouseHeld && !CardPointerCaptured() && Screen && Screen->IsWorldPointerAvailable();
}

bool APadmaPlayerController::IsMapKeyboardAvailable()const
{
    if(bBattleMap||Page!=TEXT("map")||!Session||!Rules().HasRun()||HasBlockingOverlay()
       ||bMouseHeld||bMapDrag||CardPointerCaptured())return false;
    if(FSlateApplication::IsInitialized())if(const auto Focus=FSlateApplication::Get().GetKeyboardFocusedWidget())
    {
        const FString Type=Focus->GetTypeAsString();
        if(Type.Contains(TEXT("EditableText"))||Type.Contains(TEXT("SpinBox")))return false;
    }
    return true;
}

void APadmaPlayerController::FitMapCamera()
{
    if(bMouseHeld||bMapDrag||CardPointerCaptured())return;
    if(Session&&Rules().HasRun()&&Rules().Run().MapLayout.bHexWilderness){if(Screen&&!HasBlockingOverlay())Screen->ResetPaintedMap();bMapCameraReady=true;return;}
    if (!MapView || Page != TEXT("map") || bBattleMap || (bMapCameraReady && HasBlockingOverlay())) return;
    const FBox Bounds = MapView->GetMapBounds();
    if (!Bounds.IsValid) return;
    MapCameraTarget = Bounds.GetCenter();
    MapCameraTarget.Z = 0;
    MapCameraYaw = 0;
    int32 Width, Height; GetViewportSize(Width, Height);
    if (Width <= 0 || Height <= 0) return;
    const float DPI = UWidgetLayoutLibrary::GetViewportScale(this);
    const FVector2D Area=Screen?Screen->GetWorldAreaSize():FVector2D::ZeroVector;
    if(Area.X<=0||Area.Y<=0)return;
    const float W = FMath::Max(300.f, float(Area.X * DPI));
    const float H = FMath::Max(260.f, float(Area.Y * DPI));
    const float TanHalfFOV = FMath::Tan(FMath::DegreesToRadians(24.f));
    const FVector Extent = Bounds.GetExtent();
    const float Horizontal = Extent.Y / TanHalfFOV * FMath::Max(1.f, Width / W);
    const float Vertical = Extent.X * .82f / TanHalfFOV * FMath::Max(1.f, float(Width) / H);
    MapCameraDistance = FMath::Clamp(FMath::Max(Horizontal, Vertical) * 1.12f, 1800.f, 22000.f);
    const FVector2D Center=Screen->GetWorldAreaCenter();
    const float Span=2*MapCameraDistance*TanHalfFOV;
    MapCameraTarget.Y+=(.5-Center.X)*Span;
    MapCameraTarget.X+=(Center.Y-.5)*Span*Height/Width/FMath::Sin(FMath::DegreesToRadians(55.f));
    bMapCameraReady = true;
    SetCamera(MapCameraTarget, MapCameraDistance, -55, MapCameraYaw);
}

void APadmaPlayerController::MapZoomIn()
{
    if(Session&&Rules().HasRun()&&Rules().Run().MapLayout.bHexWilderness){if(IsMapPointerAvailable())Screen->ZoomPaintedMap(1.12f);return;}
    if (IsMapPointerAvailable()) { MapCameraDistance = FMath::Max(900.f, MapCameraDistance * .87f); SetCamera(MapCameraTarget, MapCameraDistance, -55, MapCameraYaw); }
}
void APadmaPlayerController::MapZoomOut()
{
    if(Session&&Rules().HasRun()&&Rules().Run().MapLayout.bHexWilderness){if(IsMapPointerAvailable())Screen->ZoomPaintedMap(1.f/1.12f);return;}
    if (IsMapPointerAvailable()) { MapCameraDistance = FMath::Min(22000.f, MapCameraDistance / .87f); SetCamera(MapCameraTarget, MapCameraDistance, -55, MapCameraYaw); }
}
void APadmaPlayerController::MapDragStart()
{
    bMapDrag = IsMapPointerAvailable();
    float X, Y;
    if (GetMousePosition(X, Y)) MapDragPosition = FVector2D(X, Y);
}
void APadmaPlayerController::MapDragEnd() { bMapDrag = false; }
void APadmaPlayerController::TickMapCamera(float Delta)
{
    if(bMouseHeld||CardPointerCaptured()){bMapDrag=false;return;}
    if(Session&&Rules().HasRun()&&Rules().Run().MapLayout.bHexWilderness&&!bBattleMap)
    {
        if(HasBlockingOverlay()||Page!=TEXT("map")){bMapDrag=false;return;}
        if(!bMapCameraReady)FitMapCamera();
        if(bMapDrag&&Screen){float X,Y;if(GetMousePosition(X,Y)){const FVector2D P(X,Y);Screen->PanPaintedMap(P-MapDragPosition);MapDragPosition=P;}}
        // The painted map owns navigation keys through its CommonUI page.
        // Polling PlayerInput here can retain a key whose release Slate consumed.
        return;
    }
    if (!bMapCameraReady && !bBattleMap && Page == TEXT("map")) FitMapCamera();
    if (bBattleMap || Page != TEXT("map") || !bMapCameraReady || !Session || HasBlockingOverlay()) { bMapDrag = false; return; }
    const FVector Forward = FRotator(0, MapCameraYaw, 0).Vector();
    const FVector Right = FRotator(0, MapCameraYaw + 90, 0).Vector();
    FVector Motion = FVector::ZeroVector;
    if (IsInputKeyDown(EKeys::W) || IsInputKeyDown(EKeys::Up)) Motion += Forward;
    if (IsInputKeyDown(EKeys::S) || IsInputKeyDown(EKeys::Down)) Motion -= Forward;
    if (IsInputKeyDown(EKeys::D) || IsInputKeyDown(EKeys::Right)) Motion += Right;
    if (IsInputKeyDown(EKeys::A) || IsInputKeyDown(EKeys::Left)) Motion -= Right;
    MapCameraTarget += Motion.GetClampedToMaxSize(1) * Delta * MapCameraDistance * .5;
    if (IsInputKeyDown(EKeys::Q)) MapCameraYaw -= Delta * 35;
    if (IsInputKeyDown(EKeys::E)) MapCameraYaw += Delta * 35;
    if (bMapDrag)
    {
        float X, Y;
        if (GetMousePosition(X, Y))
        {
            FVector2D Mouse(X, Y), Shift = Mouse - MapDragPosition;
            MapCameraTarget += (-Right * Shift.X + Forward * Shift.Y) * MapCameraDistance * .0007;
            MapDragPosition = Mouse;
        }
    }
    if (MapView)
    {
        const FBox Bounds = MapView->GetMapBounds().ExpandBy(900);
        if (Bounds.IsValid) { MapCameraTarget.X = FMath::Clamp(MapCameraTarget.X, Bounds.Min.X, Bounds.Max.X); MapCameraTarget.Y = FMath::Clamp(MapCameraTarget.Y, Bounds.Min.Y, Bounds.Max.Y); }
    }
    SetCamera(MapCameraTarget, MapCameraDistance, -55, MapCameraYaw);
}

void APadmaPlayerController::StartConfiguredMap(bool bGenerate)
{
    FPadmaContentSnapshot Content;
    FString Error;
    if (!Catalog || !Catalog->Load(Content, Error)) { Notice = Error; bNeedsRefresh = true; return; }
    if (bGenerate)
    {
        const auto* Asset = Catalog->WorldMap.LoadSynchronous();
        FPadmaMapLayout Generated;
        if (!Asset || !FPadmaWorldMapGenerator::Generate(Asset->Template, Asset->GenerationSettings, MapSeedDraft, Generated, Error)
            || !FPadmaWorldMapGenerator::ApplyToContent(Generated, Content, Error))
        { Notice = Error.IsEmpty() ? TEXT("请先配置地图定义资产") : Error; bNeedsRefresh = true; return; }
    }
    const auto Result = Session->StartNewMap(Content);
    ApplyResult(Result);
    if (!Result.bOk) return;
    SelectedNode = Rules().HomeNode();
    PreviewNode=HoverNode=NAME_None;
    SelectedCards.Reset(); SelectedHand = NAME_None;
    Page = TEXT("map"); RestoreOverlay({}); OverlayHistory.Reset(); UpdateInspectionGate();
    Draft = Rules().Run().Preparation;
    MapSeedDraft = Rules().Run().MapLayout.Seed;
    bMapCameraReady = false;
    RefreshStage();
}

void APadmaPlayerController::StartTutorialMap()
{
    FPadmaContentSnapshot Content;FPadmaMapLayout Tutorial;FString Error;
    if(!Catalog||!Catalog->Load(Content,Error)){Notice=Error;bNeedsRefresh=true;return;}
    const FPadmaMapLayout AuthoredReference=Content.MapLayout;
    if(!FPadmaTutorialMapGenerator::Generate(Content.MapLayout,12345,Tutorial,Error)
       ||!FPadmaWorldMapGenerator::ApplyToContent(Tutorial,Content,Error))
    {Notice=Error;bNeedsRefresh=true;return;}
    const auto Result=Session->StartNewMap(Content,&AuthoredReference);ApplyResult(Result);if(!Result.bOk)return;
    SelectedNode=Rules().HomeNode();PreviewNode=HoverNode=NAME_None;SelectedCards.Reset();SelectedHand=NAME_None;
    Page=TEXT("map");RestoreOverlay({});OverlayHistory.Reset();UpdateInspectionGate();Draft=Rules().Run().Preparation;
    bMapCameraReady=false;RefreshStage();if(Screen)Screen->ResetPaintedMap();
}
