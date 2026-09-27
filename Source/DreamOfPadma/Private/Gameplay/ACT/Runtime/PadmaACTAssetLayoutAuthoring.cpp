#if WITH_EDITOR
#include "HAL/IConsoleManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "UObject/ObjectRedirector.h"
#include "Settings/LevelEditorViewportSettings.h"

static FAutoConsoleCommand PadmaACTPrepareRename(
    TEXT("Padma.ACT.PrepareLayoutRename"), TEXT("Clear ACT viewport-history references in this process only."),
    FConsoleCommandDelegate::CreateLambda([]()
    {
        if (!FParse::Param(FCommandLine::Get(), TEXT("PadmaApplyACTLayout"))) return;
        auto* Settings = GetMutableDefault<ULevelEditorViewportSettings>();
        for (auto It = Settings->EditorViews.CreateIterator(); It; ++It)
            if (It.Key().ToSoftObjectPath().ToString().StartsWith(TEXT("/Game/Sandbox/ACT/"))) It.RemoveCurrent();
        // No SaveConfig: retain the user's history on disk.
    }));
// Explicit offline authoring operation. No runtime dependency and no broad package deletion.
static FAutoConsoleCommand PadmaACTFixRedirectors(
    TEXT("Padma.ACT.FixLayoutRedirectors"), TEXT("Fix ACT-only redirectors; requires -PadmaApplyACTLayout."),
    FConsoleCommandDelegate::CreateLambda([]()
    {
        if (!FParse::Param(FCommandLine::Get(), TEXT("PadmaApplyACTLayout")))
        {
            UE_LOG(LogTemp, Error, TEXT("ACT redirector fix requires explicit -PadmaApplyACTLayout"));
            return;
        }
        auto& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
        Registry.SearchAllAssets(true);
        TArray<FAssetData> Assets;
        Registry.GetAssetsByPath(TEXT("/Game/Sandbox/ACT"), Assets, true);
        TArray<UObjectRedirector*> Redirectors;
        for (const auto& Asset : Assets)
            if (Asset.AssetClassPath == UObjectRedirector::StaticClass()->GetClassPathName())
                if (auto* Redirector = Cast<UObjectRedirector>(Asset.GetAsset())) Redirectors.Add(Redirector);
        FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get().FixupReferencers(
            Redirectors, false, ERedirectFixupMode::DeleteFixedUpRedirectors);
        UE_LOG(LogTemp, Display, TEXT("ACT redirector fix processed %d packages; verify fresh registry next"), Redirectors.Num());
    }));
#endif
