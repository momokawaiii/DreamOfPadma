using UnrealBuildTool;

public class PadmaNPREditor : ModuleRules
{
    public PadmaNPREditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateDependencyModuleNames.AddRange(new[] {
            "Core", "CoreUObject", "Engine", "Slate", "SlateCore", "UnrealEd",
            "PadmaNPRRuntime", "RenderCore", "RHI", "MaterialEditor", "ToolMenus", "ContentBrowser", "AssetRegistry", "PropertyEditor"
        });
    }
}
