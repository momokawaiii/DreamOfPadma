#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"
class FPadmaNPRRuntimeModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        const auto Plugin = IPluginManager::Get().FindPlugin(TEXT("PadmaNPR"));
        check(Plugin.IsValid());
        AddShaderSourceDirectoryMapping(TEXT("/Plugin/PadmaNPR"), FPaths::Combine(Plugin->GetBaseDir(), TEXT("Shaders")));
    }
    virtual bool SupportsDynamicReloading() override { return false; }
};
IMPLEMENT_MODULE(FPadmaNPRRuntimeModule, PadmaNPRRuntime)
