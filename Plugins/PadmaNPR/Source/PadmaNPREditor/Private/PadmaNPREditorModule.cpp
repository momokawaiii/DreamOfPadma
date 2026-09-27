#include "PadmaNPRProfile.h"
#include "PadmaNPRStudioLibrary.h"
#include "SPadmaNPRCharacterStudio.h"
#include "Engine/Selection.h"
#include "Modules/ModuleManager.h"
#include "AssetRegistry/AssetData.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Editor.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "ToolMenus.h"
#include "Framework/Docking/TabManager.h"
#include "Styling/AppStyle.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSplitter.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Styling/StyleColors.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include "ImageUtils.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Tests/AutomationCommon.h"
#endif

#define LOCTEXT_NAMESPACE "PadmaNPRStudio"

namespace PadmaNPRStudio
{
static const FName TabId(TEXT("PadmaNPR.Studio"));

struct FPage
{
    FText Title;
    FText Summary;
    FText Body;
};

// The shell deliberately owns no renderer, material parameters or serialized profile schema.
class SStudio : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SStudio) {}
    SLATE_END_ARGS()

    bool SelectPage(int32 Index)
    {
        if (!Pages.IsValidIndex(Index)) { return false; }
        ActivePage = Index;
        return true;
    }

    void Construct(const FArguments&)
    {
        Pages = {
            { LOCTEXT("Overview", "概览"),
              LOCTEXT("OverviewSummary", "从一个可理解的小效果开始"),
              LOCTEXT("OverviewBody", "当前版本：Cloth + Character Core + 编辑器应用\n\n已提供：角色 DA 编辑、按槽应用、恢复材质、组件绑定和运行时参数隔离。\n\n学习顺序：遮罩与混色 → Cel / Rim / Matcap → Face SDF → Hair。母材质只有一个函数入口，算法在插件 HLSL 中。\n\n点击应用到角色后，仅替换 DA 中声明的槽位；恢复角色可撤销接入。普通 Substrate 可作为后续后端；不使用 UE 5.8 新 Toon 材质及其配套系统。") },
            { LOCTEXT("Character", "角色配置"),
              LOCTEXT("CharacterSummary", "角色 Profile / 材质槽绑定"),
              LOCTEXT("CharacterBody", "Style：共享风格默认值。\nCharacter Profile：角色与部位的艺术参数覆盖。\nBinding：网格槽位、贴图通道、头部骨骼与轴向。\n\nPadmaNPRProfile 支持九类参考槽：Face / Skin / Hair / Eye / Cloth / HairLine / Brow / EyeShadow / HairShadow。参考槽由 Reference Scalars / Vectors 控制，MI 绑定贴图。选中 DA 和关卡角色，点击应用到角色；修改 DA 后重新应用。\n\nCloth 可创建 PadmaClothProfile 数据资产：Defaults 保存默认值，SlotOverrides 按槽名覆盖勾选的分组。贴图和通道仍保存在 MI。\n\nCreate Cloth MID 函数显式应用 Profile；调用方负责赋给网格与恢复原材质。角色 DA 可在右侧编辑；Cloth DA 使用原生资产编辑器。角色组件提供 Key Light、Target Mesh 和 Profile 绑定。") },
            { LOCTEXT("Materials", "材质学习"),
              LOCTEXT("MaterialsSummary", "先看输入与输出，再读中间公式"),
              LOCTEXT("MaterialsBody", "建议顺序\n1. MF_MaskRemapped / MF_Mask_Adjust\n2. MF_ColorBlend\n3. M_Common_Cloth 的 Lam、Rim、Matcap 分支\n4. MF_SDF_Shadow 与 M_Common_Face\n5. M_Common_Hair\n6. Outline 与后处理\n\n每个效果记录：输入、坐标空间、数值范围、公式、最终输出。先连节点，再写等价 HLSL，最后比较画面。\n\n在内容浏览器选中材质或函数，点击右侧“读取所选资产”，再打开原生编辑器。Toon / ZMDRender 的源资产需要在对应工程中阅读；本插件不会复制它们。") },
            { LOCTEXT("Preview", "预览与调试"),
              LOCTEXT("PreviewSummary", "先使用现有关卡和材质预览"),
              LOCTEXT("PreviewBody", "专用角色预览尚未接入。请在现有关卡中调整组件指定的 Key Light。\n\n当前可使用原生材质编辑器预览，以及自己的学习关卡。\n\n后续验收场景：灯绕角色旋转、头部转动、相机远近变化、两个角色同屏。\n\n角色 DA 的 Debug：0 最终颜色、1 明暗分带、2 SDF、3 世界法线、4 刘海区域。修改后点击应用到角色。此处尚未创建相机、灯光或渲染 Pass。") },
            { LOCTEXT("Tools", "工具"),
              LOCTEXT("ToolsSummary", "等重复操作出现后，再把它做成工具"),
              LOCTEXT("ToolsBody", "应用时校验：槽名、头部骨骼、母材质类型、重复槽和派生 MI 循环引用。\n\n刘海使用单独区域遮罩；默认黑色不透。陈的区域按源 Hair P.R 取反烘焙。贴图采样类型仍需与源编码一致。\n\nHairShadow 当前是辅助几何透明层近似。屏幕空间描边、RDG、Atlas 和自定义 BSDF 尚未实现。") }
        };

        FDetailsViewArgs DetailsArgs;
        DetailsArgs.bAllowSearch = true;
        DetailsArgs.bHideSelectionTip = true;
        DetailsArgs.bLockable = false;
        DetailsArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
        Details = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor").CreateDetailView(DetailsArgs);
        Details->SetIsPropertyEditingEnabledDelegate(FIsPropertyEditingEnabled::CreateLambda([this] { return SelectedAsset.IsValid() && SelectedAsset->IsA<UPadmaNPRProfile>(); }));

        FToolBarBuilder Toolbar(TSharedPtr<FUICommandList>(), FMultiBoxCustomization::None);
        Toolbar.SetStyle(&FAppStyle::Get(), "SlimToolBar");
        Toolbar.AddToolBarButton(
            FUIAction(FExecuteAction::CreateSP(this, &SStudio::ReadSelectionAction)), NAME_None,
            LOCTEXT("ReadSelection", "读取所选"), LOCTEXT("ReadSelectionTip", "读取内容浏览器中选中的一个资产"),
            FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.BrowseContent"));
        Toolbar.AddToolBarButton(
            FUIAction(FExecuteAction::CreateLambda([this] { OpenAsset(); }), FCanExecuteAction::CreateLambda([this] { return SelectedAsset.IsValid(); })), NAME_None,
            LOCTEXT("OpenAsset", "打开资产"), LOCTEXT("OpenAssetTip", "在原生资产编辑器中打开，可在那里进行编辑"),
            FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Edit"));
        Toolbar.AddSeparator();
        Toolbar.AddToolBarButton(
            FUIAction(FExecuteAction::CreateLambda([this] {
                UObject* Asset=LoadObject<UObject>(nullptr,TEXT("/PadmaNPR/Materials/M_NPR_Cloth.M_NPR_Cloth"));
                if(Asset && GEditor) GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(Asset);
                else SelectionStatus=LOCTEXT("ClothMissing", "Cloth 模板尚未生成，请查看插件说明。");
            })), NAME_None, LOCTEXT("ClothCore", "Cloth 母材质"),
            LOCTEXT("ClothCoreTip", "打开通用 Cloth 的薄材质入口"),
            FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Edit"));
        Toolbar.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([this] {
            auto* Profile=Cast<UPadmaNPRProfile>(SelectedAsset.Get());
            auto* Actor=GEditor?Cast<AActor>(GEditor->GetSelectedActors()->GetTop(AActor::StaticClass())):nullptr;
            FString Error;auto* C=UPadmaNPRStudioLibrary::ApplyToActor(Profile,Actor,nullptr,nullptr,Error);
            SelectionStatus=FText::FromString(C?TEXT("已应用到所选角色。请保存派生 MI 和关卡；组件中可指定 Key Light。"):Error);
        })),NAME_None,LOCTEXT("ApplyCharacter","应用到角色"),LOCTEXT("ApplyCharacterTip","选择 DA 和关卡角色后应用；原始贴图 MI 保持不变"),FSlateIcon(FAppStyle::GetAppStyleSetName(),"Icons.Check"));
        Toolbar.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([this] {
            auto* Actor=GEditor?Cast<AActor>(GEditor->GetSelectedActors()->GetTop(AActor::StaticClass())):nullptr;
            FString Error;bool OK=UPadmaNPRStudioLibrary::RestoreActor(Actor,Error);
            SelectionStatus=FText::FromString(OK?TEXT("已恢复插件接入前的材质。"):Error);
        })),NAME_None,LOCTEXT("RestoreCharacter","恢复角色"),LOCTEXT("RestoreCharacterTip","恢复插件修改的槽位，保留其他修改"),FSlateIcon(FAppStyle::GetAppStyleSetName(),"Icons.Undo"));
        Toolbar.AddSeparator();
        Toolbar.AddToolBarButton(
            FUIAction(FExecuteAction::CreateSP(this, &SStudio::ClearSelection)), NAME_None,
            LOCTEXT("Clear", "清空"), LOCTEXT("ClearTip", "清空检查对象，不修改资产"),
            FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Delete"));

        TSharedRef<SHorizontalBox> Navigation = SNew(SHorizontalBox);
        for (int32 Index = 0; Index < Pages.Num(); ++Index)
        {
            Navigation->AddSlot().AutoWidth().Padding(2, 0)
            [ SNew(SCheckBox)
              .Style(FAppStyle::Get(), "ToggleButtonCheckbox")
              .Padding(FMargin(14, 7))
              .IsChecked_Lambda([this, Index] { return ActivePage == Index ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
              .OnCheckStateChanged_Lambda([this, Index](ECheckBoxState) { SelectPage(Index); })
              [ SNew(STextBlock).Text(Pages[Index].Title) ] ];
        }

        ChildSlot
        [ SNew(SBorder).BorderImage(FAppStyle::GetBrush("Brushes.Recessed")).Padding(0)
          [ SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight()
            [ SNew(SBorder).BorderImage(FAppStyle::GetBrush("Brushes.Panel")).Padding(FMargin(8, 2))
              [ SNew(SHorizontalBox)
                + SHorizontalBox::Slot().FillWidth(1) [ Toolbar.MakeWidget() ]
                + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(12, 0)
                [ SNew(STextBlock).Text(LOCTEXT("Stage", "PADMA NPR  /  STUDIO"))
                  .ColorAndOpacity(FStyleColors::ForegroundHover) ] ] ]
            + SVerticalBox::Slot().AutoHeight().Padding(4, 5, 4, 5) [ Navigation ]
            + SVerticalBox::Slot().FillHeight(1).Padding(4, 0)
            [ SNew(SSplitter).PhysicalSplitterHandleSize(5)
              + SSplitter::Slot().Value(0.62f)
              [ SNew(SSplitter).Orientation(Orient_Vertical).PhysicalSplitterHandleSize(5)
                + SSplitter::Slot().Value(0.57f)
                [ SNew(SVerticalBox)
                  + SVerticalBox::Slot().AutoHeight() [ PanelHeader(LOCTEXT("PreviewHeader", "预览"), "Icons.Visible") ]
                  + SVerticalBox::Slot().FillHeight(1)
                  [ SNew(SBorder).BorderImage(FAppStyle::GetBrush("Brushes.Recessed")).Padding(24)
                    [ SNew(SVerticalBox)
                      + SVerticalBox::Slot().FillHeight(1)
                      [ SNew(SSpacer) ]
                      + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0, 0, 0, 14)
                      [ SNew(SBox).WidthOverride(48).HeightOverride(48)
                        [ SNew(SImage).Image(FAppStyle::GetBrush("ClassThumbnail.MaterialInstanceConstant")) ] ]
                      + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
                      [ SNew(STextBlock).Text(LOCTEXT("PreviewEmpty", "角色预览待接入"))
                        .Font(FAppStyle::GetFontStyle("HeadingExtraSmall")) ]
                      + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0, 10)
                      [ SNew(STextBlock).Text(LOCTEXT("PreviewHint", "当前可通过顶部「打开资产」使用原生材质预览"))
                        .ColorAndOpacity(FStyleColors::ForegroundHover).AutoWrapText(true) ]
                      + SVerticalBox::Slot().FillHeight(1) [ SNew(SSpacer) ]
                      + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Left)
                      [ SNew(STextBlock).Text(LOCTEXT("PreviewState", "预览场景  ·  未创建"))
                        .ColorAndOpacity(FStyleColors::ForegroundHover) ] ] ] ]
                + SSplitter::Slot().Value(0.43f)
                [ SNew(SVerticalBox)
                  + SVerticalBox::Slot().AutoHeight() [ PanelHeader(LOCTEXT("WorkspaceHeader", "工作区"), "LevelEditor.Tabs.Details") ]
                  + SVerticalBox::Slot().FillHeight(1)
                  [ SNew(SBorder).BorderImage(FAppStyle::GetBrush("Brushes.Panel")).Padding(0)
                    [ SNew(SScrollBox)
                      + SScrollBox::Slot()
                      [ SNew(SBorder).BorderImage(FAppStyle::GetBrush("Brushes.Panel")).Padding(14)
                        [ SNew(SVerticalBox)
                          + SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 6)
                          [ SNew(STextBlock).Text_Lambda([this] { return Pages[ActivePage].Title; })
                            .Font(FAppStyle::GetFontStyle("HeadingExtraSmall")) ]
                          + SVerticalBox::Slot().AutoHeight()
                          [ SNew(STextBlock).Text_Lambda([this] { return Pages[ActivePage].Summary; })
                            .AutoWrapText(true).ColorAndOpacity(FStyleColors::ForegroundHover) ] ] ]
                      + SScrollBox::Slot()
                      [ SNew(SExpandableArea).InitiallyCollapsed(false)
                        .BorderImage(FAppStyle::GetBrush("DetailsView.CategoryTop"))
                        .HeaderPadding(FMargin(8, 6)).Padding(14)
                        .HeaderContent() [ SNew(STextBlock).Text(LOCTEXT("Guide", "工作说明"))
                          .Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont")) ]
                        .BodyContent() [ SNew(STextBlock).AutoWrapText(true)
                          .Text_Lambda([this] { return Pages[ActivePage].Body; }) ] ] ] ] ] ]
              + SSplitter::Slot().Value(0.38f)
              [ SNew(SVerticalBox)
                + SVerticalBox::Slot().AutoHeight() [ PanelHeader(LOCTEXT("DetailsHeader", "细节"), "LevelEditor.Tabs.Details") ]
                + SVerticalBox::Slot().AutoHeight()
                [ SNew(SBorder).BorderImage(FAppStyle::GetBrush("Brushes.Panel")).Padding(12)
                  [ SNew(SVerticalBox)
                    + SVerticalBox::Slot().AutoHeight()
                    [ SNew(STextBlock).Text(LOCTEXT("Inspector", "资产属性"))
                      .Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont")) ]
                    + SVerticalBox::Slot().AutoHeight().Padding(0, 6, 0, 0)
                    [ SNew(STextBlock).AutoWrapText(true).Text_Lambda([this] { return SelectionStatus; })
                      .ColorAndOpacity(FStyleColors::ForegroundHover) ] ] ]
                + SVerticalBox::Slot().AutoHeight()
                [ SNew(SExpandableArea).InitiallyCollapsed(false)
                  .BorderImage(FAppStyle::GetBrush("DetailsView.CategoryTop"))
                  .HeaderPadding(FMargin(8, 6)).Padding(0)
                  .HeaderContent() [ SNew(STextBlock).Text(LOCTEXT("Context", "检查上下文"))
                    .Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont")) ]
                  .BodyContent()
                  [ SNew(SVerticalBox)
                    + SVerticalBox::Slot().AutoHeight() [ PropertyRow(LOCTEXT("ModeLabel", "访问方式"), LOCTEXT("ModeValue", "角色 DA 可编辑 / 其他只读")) ]
                    + SVerticalBox::Slot().AutoHeight() [ PropertyRow(LOCTEXT("SourceLabel", "资产来源"), LOCTEXT("SourceValue", "内容浏览器选择")) ]
                    + SVerticalBox::Slot().AutoHeight() [ PropertyRow(LOCTEXT("ApplyLabel", "Cloth 配置"), LOCTEXT("Pending", "显式创建 MID")) ] ] ]
                + SVerticalBox::Slot().FillHeight(1)
                [ SNew(SBorder).BorderImage(FAppStyle::GetBrush("Brushes.Panel")).Padding(0)
                  [ Details.ToSharedRef() ] ] ] ]
            + SVerticalBox::Slot().AutoHeight().Padding(0, 4, 0, 0)
            [ SNew(SBorder).BorderImage(FAppStyle::GetBrush("Brushes.Panel")).Padding(FMargin(12, 6))
              [ SNew(SHorizontalBox)
                + SHorizontalBox::Slot().FillWidth(1)
                [ SNew(STextBlock).Text(LOCTEXT("Footer", "Cloth Core v1  ·  资产检查就绪"))
                  .ColorAndOpacity(FStyleColors::ForegroundHover) ]
                + SHorizontalBox::Slot().AutoWidth()
                [ SNew(STextBlock).Text(LOCTEXT("Version", "PadmaNPR  0.2"))
                  .ColorAndOpacity(FStyleColors::ForegroundHover) ] ] ] ] ];
        ClearSelection();
    }

private:
    TSharedRef<SWidget> PanelHeader(const FText& Title, const FName Icon)
    {
        return SNew(SBorder).BorderImage(FAppStyle::GetBrush("Brushes.Header")).Padding(FMargin(10, 7))
            [ SNew(SHorizontalBox)
              + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0, 0, 7, 0)
              [ SNew(SImage).Image(FAppStyle::GetBrush(Icon)) ]
              + SHorizontalBox::Slot().FillWidth(1).VAlign(VAlign_Center)
              [ SNew(STextBlock).Text(Title).Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont")) ] ];
    }

    TSharedRef<SWidget> PropertyRow(const FText& Label, const FText& Value)
    {
        return SNew(SBorder).BorderImage(FAppStyle::GetBrush("Brushes.Panel")).Padding(FMargin(14, 6))
            [ SNew(SHorizontalBox)
              + SHorizontalBox::Slot().FillWidth(0.45f).VAlign(VAlign_Center)
              [ SNew(STextBlock).Text(Label) ]
              + SHorizontalBox::Slot().FillWidth(0.55f)
              [ SNew(SBorder).BorderImage(FAppStyle::GetBrush("Brushes.Recessed")).Padding(FMargin(7, 3))
                [ SNew(STextBlock).Text(Value).ColorAndOpacity(FStyleColors::ForegroundHover) ] ] ];
    }

    void ReadSelectionAction() { ReadSelection(); }

    FReply ReadSelection()
    {
        TArray<FAssetData> Assets;
        FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser").Get().GetSelectedAssets(Assets);
        ClearSelection();
        if (Assets.Num() != 1)
        {
            SelectionStatus = LOCTEXT("SelectOne", "请在内容浏览器中只选一个资产，再点击读取。");
            return FReply::Handled();
        }
        UObject* Asset = Assets[0].GetAsset();
        if (!Asset)
        {
            SelectionStatus = LOCTEXT("LoadFailed", "资产未能加载，请在内容浏览器检查它是否可用。");
            return FReply::Handled();
        }
        SelectedAsset = Asset;
        Details->SetObject(Asset);
        SelectionStatus = FText::FromString(Asset->GetPathName());
        return FReply::Handled();
    }

    FReply OpenAsset()
    {
        if (GEditor && SelectedAsset.IsValid())
        {
            GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(SelectedAsset.Get());
        }
        return FReply::Handled();
    }

    void ClearSelection()
    {
        SelectedAsset.Reset();
        Details->SetObject(nullptr);
        SelectionStatus = LOCTEXT("Empty", "尚未选择资产。请在内容浏览器中选择一个资产，再点击「读取所选」。");
    }

    TArray<FPage> Pages;
    int32 ActivePage = 0;
    TSharedPtr<IDetailsView> Details;
    TWeakObjectPtr<UObject> SelectedAsset;
    FText SelectionStatus;
};
}

class FPadmaNPREditorModule : public IModuleInterface
{
public:
    virtual bool SupportsDynamicReloading() override { return false; }

    virtual void StartupModule() override
    {
        if (IsRunningCommandlet()) { return; }
        FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TEXT("PadmaNPR.LegacyStudio"),
            FOnSpawnTab::CreateLambda([](const FSpawnTabArgs&){return SNew(SDockTab).TabRole(ETabRole::NomadTab)[SNew(PadmaNPRStudio::SStudio)];}))
            .SetDisplayName(LOCTEXT("LegacyTab", "Padma NPR · 已有 DA / 参考版")).SetMenuType(ETabSpawnerMenuType::Hidden);
        FGlobalTabmanager::Get()->RegisterNomadTabSpawner(PadmaNPRStudio::TabId,
            FOnSpawnTab::CreateRaw(this, &FPadmaNPREditorModule::SpawnTab))
            .SetDisplayName(LOCTEXT("TabName", "Padma NPR Studio"))
            .SetMenuType(ETabSpawnerMenuType::Hidden);
        UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FPadmaNPREditorModule::RegisterMenus));
    }

    virtual void ShutdownModule() override
    {
        if (IsRunningCommandlet()) { return; }
        UToolMenus::UnRegisterStartupCallback(this);
        UToolMenus::UnregisterOwner(this);
        if (TSharedPtr<SDockTab> Tab = FGlobalTabmanager::Get()->FindExistingLiveTab(PadmaNPRStudio::TabId))
        {
            Tab->SetContent(SNullWidget::NullWidget);
            Tab->RequestCloseTab();
        }
        FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(PadmaNPRStudio::TabId);
        if(auto Tab=FGlobalTabmanager::Get()->FindExistingLiveTab(FName(TEXT("PadmaNPR.LegacyStudio")))){Tab->SetContent(SNullWidget::NullWidget);Tab->RequestCloseTab();}
        FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TEXT("PadmaNPR.LegacyStudio"));
    }

private:
    TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs&)
    {
        return SNew(SDockTab).TabRole(ETabRole::NomadTab) [ MakePadmaNPRCharacterStudio() ];
    }

    void RegisterMenus()
    {
        FToolMenuOwnerScoped Owner(this);
        UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
        Menu->FindOrAddSection("WindowLayout").AddMenuEntry(
            "PadmaNPRStudio", LOCTEXT("MenuTitle", "Padma NPR Studio"),
            LOCTEXT("MenuTip", "打开 NPR 学习与角色配置工作台"),
            FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"),
            FUIAction(FExecuteAction::CreateLambda([] { FGlobalTabmanager::Get()->TryInvokeTab(PadmaNPRStudio::TabId); })));
    }
};

IMPLEMENT_MODULE(FPadmaNPREditorModule, PadmaNPREditor)

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaNPRStudioSmokeTest, "PadmaNPR.Editor.StudioLifecycle",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPadmaNPRStudioSmokeTest::RunTest(const FString&)
{
    auto Tab = FGlobalTabmanager::Get()->TryInvokeTab(PadmaNPRStudio::TabId);
    if (!TestTrue(TEXT("Studio tab is registered and opens"), Tab.IsValid())) { return false; }
    TestTrue(TEXT("Repeated open reuses the same tab"), FGlobalTabmanager::Get()->TryInvokeTab(PadmaNPRStudio::TabId) == Tab);
    Tab->RequestCloseTab();
    ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(0.5f));
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this]() {
        auto Reopened = FGlobalTabmanager::Get()->TryInvokeTab(PadmaNPRStudio::TabId);
        TestTrue(TEXT("Studio reopens after close"), Reopened.IsValid());
        if (Reopened) { Reopened->RequestCloseTab(); }
        return true;
    }));

    if (FParse::Param(FCommandLine::Get(), TEXT("PadmaNPRCapture")))
    {
        auto Studio = SNew(PadmaNPRStudio::SStudio);
        const bool bCompact = FParse::Param(FCommandLine::Get(), TEXT("PadmaNPRCompact"));
        auto Window = SNew(SWindow).Title(FText::FromString(TEXT("Padma NPR Studio — verification")))
            .ClientSize(bCompact ? FVector2D(900, 620) : FVector2D(1280, 760)).SupportsMaximize(false).SupportsMinimize(false) [ Studio ];
        FSlateApplication::Get().AddWindow(Window);
        TestFalse(TEXT("Invalid page is rejected"), Studio->SelectPage(99));
        for (int32 Page = 0; Page < 5; ++Page)
        {
            ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this, Studio, Page]() {
                TestTrue(TEXT("Page selection succeeds"), Studio->SelectPage(Page));
                return true;
            }));
            ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(0.5f));
            ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this, Window, Page, bCompact]() {
                TArray<FColor> Pixels;
                FIntVector Size;
                if (TestTrue(TEXT("Slate screenshot captured"), FSlateApplication::Get().TakeScreenshot(Window, Pixels, Size)))
                {
                    const FString Directory = FPaths::ProjectDir() / TEXT("Artifacts/PadmaNPRStudio");
                    IFileManager::Get().MakeDirectory(*Directory, true);
                    TArray64<uint8> PNG;
                    FImageUtils::PNGCompressImageArray(Size.X, Size.Y, Pixels, PNG);
                    TestTrue(TEXT("Screenshot saved"), FFileHelper::SaveArrayToFile(PNG, *(Directory / FString::Printf(TEXT("%sPage-%d.png"), bCompact ? TEXT("Compact-") : TEXT(""), Page))));
                }
                return true;
            }));
        }
        ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([Window]() {
            Window->RequestDestroyWindow();
            return true;
        }));
    }
    return true;
}
#endif
#undef LOCTEXT_NAMESPACE
