#include "SPadmaNPRCharacterStudio.h"
#include "Animation/AnimSequence.h"
#include "Components/SkeletalMeshComponent.h"
#include "ContentBrowserModule.h"
#include "Editor.h"
#include "EditorViewportClient.h"
#include "Engine/DirectionalLight.h"
#include "Engine/Selection.h"
#include "Engine/SkeletalMesh.h"
#include "Framework/Docking/TabManager.h"
#include "IContentBrowserSingleton.h"
#include "IDetailsView.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "PadmaNPRCharacterRecipe.h"
#include "PadmaNPRComponent.h"
#include "PadmaNPRProfile.h"
#include "PadmaNPRStudioLibrary.h"
#include "PreviewScene.h"
#include "PropertyEditorModule.h"
#include "SEditorViewport.h"
#include "Styling/AppStyle.h"
#include "UObject/StrongObjectPtr.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSplitter.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

class FPadmaManualViewportClient : public FEditorViewportClient {
public:
  FPadmaManualViewportClient(FPreviewScene *Scene,
                             const TSharedRef<SEditorViewport> &Widget)
      : FEditorViewportClient(nullptr, Scene, Widget) {
    SetViewMode(VMI_Lit);
    SetRealtime(true);
    ExposureSettings.bFixed = true;
    ExposureSettings.FixedEV100 = 0;
    SetViewLocation(FVector(250, 0, 100));
    SetViewRotation(FRotator(0, 180, 0));
  }
  TWeakObjectPtr<USkeletalMeshComponent> Mesh;
  TWeakObjectPtr<UPadmaNPRCharacterRecipe> Recipe;
  virtual void Tick(float Delta) override {
    FEditorViewportClient::Tick(Delta);
    auto *M = Mesh.Get();
    auto *R = Recipe.Get();
    if (!M || !R)
      return;
    M->bPauseAnims = !R->bPlayAnimation;
    if (R->bPlayAnimation) {
      M->TickAnimation(Delta, false);
      M->RefreshBoneTransforms();
    }
    const auto H = M->GetSocketTransform(R->HeadBone);
    const auto Q = H.GetRotation() * R->HeadAxes.Quaternion();
    const FVector L = -FRotator(R->LightPitch, R->LightYaw, 0).Vector();
    M->SetCustomPrimitiveDataVector4(0, FVector4(L, 1));
    M->SetCustomPrimitiveDataVector4(4, FVector4(Q.GetForwardVector(), 0));
    M->SetCustomPrimitiveDataVector4(8, FVector4(Q.GetRightVector(), 0));
    M->SetCustomPrimitiveDataVector4(12, FVector4(Q.GetUpVector(), 0));
    M->SetCustomPrimitiveDataVector4(16, FVector4(H.GetLocation(), 0));
  }
};
class SPadmaManualViewport : public SEditorViewport {
public:
  SLATE_BEGIN_ARGS(SPadmaManualViewport) {}
  SLATE_END_ARGS()
  void Construct(const FArguments &) {
    Scene = MakeUnique<FPreviewScene>(FPreviewScene::ConstructionValues());
    SEditorViewport::Construct(SEditorViewport::FArguments());
  }
  void Show(UPadmaNPRCharacterRecipe *R, UPadmaNPRProfile *P) {
    Profile.Reset(P);
    if (!Mesh.IsValid()) {
      Mesh.Reset(NewObject<USkeletalMeshComponent>());
      Scene->AddComponent(Mesh.Get(), FTransform::Identity);
    }
    Mesh->SetSkeletalMesh(R->Mesh);
    Mesh->EmptyOverrideMaterials();
    if (P)
      for (const auto &S : P->Slots) {
        auto *MID = UMaterialInstanceDynamic::Create(S.Material, Mesh.Get());
        TMap<FName, FLinearColor> Params;
        S.GetParameters(P->ProfileId, Params);
        for (const auto &V : Params)
          MID->SetVectorParameterValue(V.Key, V.Value);
        Mesh->SetMaterial(Mesh->GetMaterialIndex(S.SlotName), MID);
      }
    Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
    Mesh->SetAnimation(R->Animation);
    if (R->Animation)
      Mesh->Play(true);
    Client->Mesh = Mesh.Get();
    Client->Recipe = R;
    Mesh->UpdateBounds();
    const auto Center = Mesh->Bounds.Origin;
    const double Distance = FMath::Max(100., Mesh->Bounds.BoxExtent.Z * 2.4);
    Client->SetViewLocation(Center + FVector(0, Distance, 0));
    Client->SetViewRotation(FRotator(0, -90, 0));
    Client->SetLookAtLocation(Center);
    Invalidate();
  }

protected:
  virtual TSharedRef<FEditorViewportClient>
  MakeEditorViewportClient() override {
    Client =
        MakeShared<FPadmaManualViewportClient>(Scene.Get(), SharedThis(this));
    return Client.ToSharedRef();
  }

private:
  TUniquePtr<FPreviewScene> Scene;
  TStrongObjectPtr<USkeletalMeshComponent> Mesh;
  TStrongObjectPtr<UPadmaNPRProfile> Profile;
  TSharedPtr<FPadmaManualViewportClient> Client;
};
class SPadmaManualStudio : public SCompoundWidget {
public:
  SLATE_BEGIN_ARGS(SPadmaManualStudio) : _InitialRecipe(nullptr) {}
  SLATE_ARGUMENT(UPadmaNPRCharacterRecipe *, InitialRecipe)
  SLATE_END_ARGS() void Construct(const FArguments &Args) {
    Recipe.Reset(Args._InitialRecipe
                     ? DuplicateObject<UPadmaNPRCharacterRecipe>(
                           Args._InitialRecipe, GetTransientPackage())
                     : NewObject<UPadmaNPRCharacterRecipe>());
    FDetailsViewArgs A;
    A.bAllowSearch = true;
    A.bHideSelectionTip = true;
    A.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    Details = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(
                  "PropertyEditor")
                  .CreateDetailView(A);
    Details->SetObject(Recipe.Get());
    Details->OnFinishedChangingProperties().AddSP(this,
                                                  &SPadmaManualStudio::Changed);
    ChildSlot
        [SNew(SVerticalBox) +
         SVerticalBox::Slot().AutoHeight().Padding(
             10)[SNew(SHorizontalBox) +
                 SHorizontalBox::Slot().FillWidth(
                     1)[SNew(STextBlock)
                            .Text(FText::FromString(
                                TEXT("Padma NPR  ·  手动角色接入")))
                            .Font(FAppStyle::GetFontStyle(
                                "PropertyWindow.BoldFont"))] +
                 SHorizontalBox::Slot().AutoWidth()[Button(
                     TEXT("已有 DA / 参考版"),
                     [] {
                       FGlobalTabmanager::Get()->TryInvokeTab(
                           FName(TEXT("PadmaNPR.LegacyStudio")));
                     })]] +
         SVerticalBox::Slot().AutoHeight().Padding(6)
             [SNew(SHorizontalBox) +
              SHorizontalBox::Slot().AutoWidth().Padding(
                  3)[Button(TEXT("读取选中网格 / 接入配置"),
                            [this] { LoadSelected(); })] +
              SHorizontalBox::Slot().AutoWidth().Padding(3)[Button(
                  TEXT("读取材质槽"),
                  [this] {
                    Generated.Reset();
                    PreviewProfile.Reset();
                    Recipe->ReadSlots();
                    Details->ForceRefresh();
                    Viewport->Show(Recipe.Get(), nullptr);
                    Message = TEXT(
                        "请选择槽位类型并填写贴图。Keep Original 不参与生成。");
                  })] +
              SHorizontalBox::Slot().AutoWidth().Padding(
                  3)[Button(TEXT("构建 NPR 预览"), [this] { Preview(); })] +
              SHorizontalBox::Slot().AutoWidth().Padding(
                  3)[Button(TEXT("生成 NPR 角色"), [this] { Generate(); })] +
              SHorizontalBox::Slot().AutoWidth().Padding(
                  3)[Button(TEXT("应用到选中角色"), [this] { Apply(); })]] +
         SVerticalBox::Slot().FillHeight(
             1)[SNew(SSplitter) +
                SSplitter::Slot().Value(
                    .58f)[SAssignNew(Viewport, SPadmaManualViewport)] +
                SSplitter::Slot().Value(.42f)[Details.ToSharedRef()]] +
         SVerticalBox::Slot().AutoHeight().Padding(
             8)[SNew(STextBlock).AutoWrapText(true).Text_Lambda([this] {
           return FText::FromString(Message);
         })] +
         SVerticalBox::Slot().AutoHeight().Padding(
             8)[SNew(STextBlock)
                    .AutoWrapText(true)
                    .Text(FText::FromString(TEXT(
                        "基础版：不透明、主光方向 "
                        "Cel；无场景接收阴影。辅助透明槽请保留原材质。修改贴图/"
                        "风格后构建预览；灯方向实时更新。生成使用新目录，不覆盖"
                        "已有资产。")))]];
    if (Args._InitialRecipe)
      Preview();
  }

private:
  TSharedRef<SWidget> Button(const TCHAR *Label, TFunction<void()> Action) {
    return SNew(SButton)
        .Text(FText::FromString(Label))
        .OnClicked_Lambda([Action] {
          Action();
          return FReply::Handled();
        });
  }
  void Changed(const FPropertyChangedEvent &E) {
    if (E.GetPropertyName() !=
            GET_MEMBER_NAME_CHECKED(UPadmaNPRCharacterRecipe, Animation) &&
        E.GetPropertyName() !=
            GET_MEMBER_NAME_CHECKED(UPadmaNPRCharacterRecipe, bPlayAnimation) &&
        E.GetPropertyName() !=
            GET_MEMBER_NAME_CHECKED(UPadmaNPRCharacterRecipe, LightYaw) &&
        E.GetPropertyName() !=
            GET_MEMBER_NAME_CHECKED(UPadmaNPRCharacterRecipe, LightPitch)) {
      Generated.Reset();
      Message = TEXT("输入已改变，请构建预览并重新生成后应用。");
    }

    if (E.GetPropertyName() ==
        GET_MEMBER_NAME_CHECKED(UPadmaNPRCharacterRecipe, Mesh)) {
      Generated.Reset();
      PreviewProfile.Reset();
      Recipe->ReadSlots();
      Details->ForceRefresh();
      Viewport->Show(Recipe.Get(), nullptr);
    }
    if (E.GetPropertyName() ==
        GET_MEMBER_NAME_CHECKED(UPadmaNPRCharacterRecipe, Animation))
      Viewport->Show(Recipe.Get(), PreviewProfile.Get());
  }
  void LoadSelected() {
    Generated.Reset();
    PreviewProfile.Reset();
    TArray<FAssetData> Assets;
    FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser")
        .Get()
        .GetSelectedAssets(Assets);
    for (const auto &A : Assets) {
      if (auto *R = Cast<UPadmaNPRCharacterRecipe>(A.GetAsset())) {
        Recipe.Reset(DuplicateObject<UPadmaNPRCharacterRecipe>(
            R, GetTransientPackage()));
        Details->SetObject(Recipe.Get());
        Viewport->Show(Recipe.Get(), nullptr);
        Message = TEXT("已读取接入配置。再次生成请更换输出目录。");
        return;
      }
      if (auto *M = Cast<USkeletalMesh>(A.GetAsset())) {
        Recipe->Mesh = M;
        Recipe->Slots.Reset();
        Recipe->HeadBone = NAME_None;
        Generated.Reset();
        PreviewProfile.Reset();
        Recipe->ReadSlots();
        Details->ForceRefresh();
        Viewport->Show(Recipe.Get(), nullptr);
        Message = TEXT("已读取网格。逐槽指定类型、贴图和通道，再构建预览。");
        return;
      }
    }
    Message = TEXT("请在内容浏览器选中骨骼网格或 DA_NPR_Recipe。");
  }
  void Preview() {
    FString Error;
    auto *P = Recipe->Build(false, Error);
    if (!P) {
      Message = Error;
      return;
    }
    PreviewProfile.Reset(P);
    Viewport->Show(Recipe.Get(), P);
    Message =
        TEXT("预览已更新。鼠标操作视口；Preview 分组可旋转灯光、选择动画。");
  }
  void Generate() {
    FString Error;
    auto *P = Recipe->Build(true, Error);
    if (!P) {
      Message = Error;
      return;
    }
    Generated.Reset(P);
    PreviewProfile.Reset(P);
    Viewport->Show(Recipe.Get(), P);
    Message = TEXT("已生成并保存：") + P->GetPathName() +
              TEXT("。选中关卡角色后点击应用；无需修改原网格。");
  }
  void Apply() {
    if (!Generated.IsValid()) {
      Message = TEXT("请先生成 NPR 角色。");
      return;
    }
    AActor *Actor = GEditor->GetSelectedActors()->GetTop<AActor>();
    if (!Actor) {
      Message = TEXT("请选择关卡角色。");
      return;
    }
    TArray<USkeletalMeshComponent *> Meshes;
    Actor->GetComponents(Meshes);
    USkeletalMeshComponent *Target = nullptr;
    int32 Count = 0;
    for (auto *M : Meshes)
      if (M->GetSkeletalMeshAsset() == Recipe->Mesh) {
        Target = M;
        ++Count;
      }
    if (Count != 1) {
      Message = TEXT("角色必须只有一个匹配的 Mesh 组件，避免应用到错误网格。");
      return;
    }
    FString Error;
    if (UPadmaNPRStudioLibrary::ApplyToActor(Generated.Get(), Actor, Target,
                                             nullptr, Error))
      Message = TEXT("已应用。请保存生成的 MIC 和关卡；在组件上选择 Key Light "
                     "后检查 PIE。");
    else
      Message = Error;
  }
  TStrongObjectPtr<UPadmaNPRCharacterRecipe> Recipe;
  TStrongObjectPtr<UPadmaNPRProfile> Generated, PreviewProfile;
  TSharedPtr<IDetailsView> Details;
  TSharedPtr<SPadmaManualViewport> Viewport;
  FString Message = TEXT(
      "选择网格 → 读取槽位 → 手动指定贴图语义 → 构建预览 → 生成 NPR 角色。");
};
TSharedRef<SWidget> MakePadmaNPRCharacterStudio() {
  return SNew(SPadmaManualStudio);
}

#if WITH_DEV_AUTOMATION_TESTS
#include "Framework/Application/SlateApplication.h"
#include "ImageUtils.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Tests/AutomationCommon.h"
#include "Widgets/SWindow.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaManualViewportTest,
                                 "PadmaNPR.Editor.ManualViewport",
                                 EAutomationTestFlags::EditorContext |
                                     EAutomationTestFlags::ProductFilter)
bool FPadmaManualViewportTest::RunTest(const FString &) {
  auto *R = NewObject<UPadmaNPRCharacterRecipe>();
  R->Mesh = LoadObject<USkeletalMesh>(
      nullptr, TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Art/Meshes/"
                    "SK_Chen_FullCharacter_CM.SK_Chen_FullCharacter_CM"));
  if (!TestNotNull(TEXT("Chen fixture mesh"), R->Mesh.Get()))
    return false;
  R->HeadBone = TEXT("NoseMd02Joint");
  R->ReadSlots();
  for (auto &S : R->Slots) {
    const FString Name = S.SlotName.ToString();
    if (Name.Contains(TEXT("shadow")))
      continue;
    S.Surface = Name.Contains(TEXT("hair"))   ? EPadmaManualSurface::Hair
                : Name.Contains(TEXT("face")) ? EPadmaManualSurface::Face
                : Name.Contains(TEXT("brow")) ? EPadmaManualSurface::Brow
                : Name.Contains(TEXT("iris")) ? EPadmaManualSurface::Eye
                                              : EPadmaManualSurface::Cloth;
    FString Tex = Name.Contains(TEXT("body"))    ? TEXT("body")
                  : Name.Contains(TEXT("cloth")) ? TEXT("cloth")
                  : Name.Contains(TEXT("hair"))  ? TEXT("hair")
                  : Name.Contains(TEXT("iris"))  ? TEXT("iris")
                                                 : TEXT("face");
    S.BaseColor = LoadObject<UTexture2D>(
        nullptr, *(TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Art/Textures/"
                        "T_actor_chen_") +
                   Tex + TEXT("_01_D")));
  }
  auto Window =
      SNew(SWindow)
          .Title(FText::FromString(TEXT("Padma NPR · Manual validation")))
          .ClientSize(
              FVector2D(1400, 900))[SNew(SPadmaManualStudio).InitialRecipe(R)];
  FSlateApplication::Get().AddWindow(Window);
  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(5));
  ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this, Window] {
    TArray<FColor> Pixels;
    FIntVector Size;
    if (TestTrue(
            TEXT("Actual Slate viewport screenshot"),
            FSlateApplication::Get().TakeScreenshot(Window, Pixels, Size))) {
      TArray64<uint8> PNG;
      FImageUtils::PNGCompressImageArray(Size.X, Size.Y, Pixels, PNG);
      TestTrue(TEXT("Saved screenshot"),
               FFileHelper::SaveArrayToFile(
                   PNG, *(FPaths::ProjectDir() /
                          TEXT("Artifacts/PadmaNPRSDF/manual-studio.png"))));
    }
    Window->RequestDestroyWindow();
    return true;
  }));
  return true;
}
#endif
