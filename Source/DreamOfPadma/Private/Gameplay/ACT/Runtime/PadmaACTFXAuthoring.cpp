#if WITH_EDITOR
#include "HAL/IConsoleManager.h"
#include "HAL/FileManager.h"
#include "NiagaraSystem.h"
#include "ViewModels/NiagaraSystemViewModel.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

// Authoring-only command. It resets one explicitly named source system in memory;
// the Python authoring recipe populates and saves it afterwards. No package deletion.
static FAutoConsoleCommand PadmaChenResetFX(
    TEXT("Padma.ChenFX.ResetSourceSystem"), TEXT("Reset a TASK-055 source Niagara system before deterministic authoring."),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
    {
        const TSet<FString> Allowed={TEXT("NS_chen_attack_01_start"),TEXT("NS_chen_attack_01_start2"),TEXT("NS_chen_attack_01_star_right"),TEXT("NS_fxbat_chen_common_hit_01"),TEXT("NS_fxbat_chen_common_hit_02")};
        const bool bEdgesOnly = Args.Num()==2 && Args[0]==TEXT("NS_chen_attack_01_start2") && Args[1]==TEXT("EdgeParticles");
        if ((!bEdgesOnly && Args.Num()!=1) || !Allowed.Contains(Args[0])) return;
        const FString Path=TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Art/Niagara/Systems/BladeContact/")+Args[0];
        if (auto* System=LoadObject<UNiagaraSystem>(nullptr,*Path))
        {
            TSet<FGuid> Ids;
            for (const auto& Handle:System->GetEmitterHandles())
            {
                if (!bEdgesOnly || Handle.GetName()==TEXT("SourceP00") || Handle.GetName()==TEXT("SourceP01")) Ids.Add(Handle.GetId());
            }
            TSharedPtr<FNiagaraSystemViewModel> ViewModel = MakeShared<FNiagaraSystemViewModel>();
            FNiagaraSystemViewModelOptions Options;
            Options.bCanAutoCompile = false;
            Options.bCanSimulate = false;
            Options.EditMode = ENiagaraSystemViewModelEditMode::SystemAsset;
            Options.MessageLogGuid = System->GetAssetGuid();
            ViewModel->Initialize(*System,Options);
            ViewModel->DeleteEmitters(Ids);
            ViewModel.Reset();
            const FString Report=FString::Printf(TEXT("%s\t%d\t%d\n"),*Path,Ids.Num(),System->GetNumEmitters());
            FFileHelper::SaveStringToFile(Report,*(FPaths::ProjectDir()/TEXT("Artifacts/ChenQianyu/Attack01/reset-systems.tsv")),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM,&IFileManager::Get(),FILEWRITE_Append);
            UE_LOG(LogTemp,Display,TEXT("Chen FX reset %s: %d -> %d"),*Path,Ids.Num(),System->GetNumEmitters());
        }
    }));

// A converter compile is asynchronous. Finish before Python releases its editor
// view model or saves the package, including commandlet/offscreen authoring.
static FAutoConsoleCommand PadmaChenFinishEdgeFX(
    TEXT("Padma.ChenFX.FinishEdgeParticles"), TEXT("Wait for the TASK-055 edge system compile before saving."),
    FConsoleCommandDelegate::CreateLambda([]()
    {
        if (auto* System=LoadObject<UNiagaraSystem>(nullptr,TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Art/Niagara/Systems/BladeContact/NS_chen_attack_01_start2")))
        {
            System->WaitForCompilationComplete(true,false);
            const FString Result=FString::Printf(TEXT("%d\t%d"),System->GetNumEmitters(),System->IsReadyToRun());
            FFileHelper::SaveStringToFile(Result,*(FPaths::ProjectDir()/TEXT("Artifacts/ChenQianyu/Attack01/edge-compile.tsv")));
            UE_LOG(LogTemp,Display,TEXT("Chen edge FX compiled: %d emitters; ready=%d"),System->GetNumEmitters(),System->IsReadyToRun());
        }
    }));
// TASK-055: edit existing Initialize inputs without rebuilding emitters/materials.
#include "ViewModels/NiagaraEmitterHandleViewModel.h"
#include "ViewModels/Stack/NiagaraStackViewModel.h"
#include "ViewModels/Stack/NiagaraStackFunctionInput.h"
#include "NiagaraScript.h"
#include "UObject/StructOnScope.h"

static void GatherChenOffsetInputs(UNiagaraStackEntry* Entry, TArray<UNiagaraStackFunctionInput*>& Out)
{
    if (auto* Input = Cast<UNiagaraStackFunctionInput>(Entry))
    {
        const auto* Script = Input->GetInputFunctionCallInitialScript();
        if (Script && Script->GetPathName() == TEXT("/Niagara/Modules/Spawn/Initialization/V2/InitializeParticle.InitializeParticle")
            && Input->GetInputParameterHandle().GetName() == TEXT("Position Offset")) Out.Add(Input);
    }
    TArray<UNiagaraStackEntry*> Children;
    Entry->GetUnfilteredChildren(Children);
    for (auto* Child : Children) GatherChenOffsetInputs(Child, Out);
}

static FAutoConsoleCommand PadmaChenSourceOffsets(
    TEXT("Padma.ChenFX.SourceOffsets"), TEXT("Audit or Apply the 11 verified TASK-055 source prefab offsets; caller saves assets."),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
    {
        if (Args.Num() != 1 || (Args[0] != TEXT("Audit") && Args[0] != TEXT("Apply"))) return;
        const bool bApply = Args[0] == TEXT("Apply");
        const FString Output = FPaths::ProjectDir()/TEXT("Artifacts/ChenQianyu/Attack01/SourceOffsets0919/offsets.tsv");
        IFileManager::Get().MakeDirectory(*FPaths::GetPath(Output), true);
        // Recovered source prefab translations, C=(-X,Z,Y), meters -> centimeters.
        // Rotation and scale are already baked into the meshes. P00/P01 in start2
        // have their own corrected procedural shape and must never be overwritten.
        const TArray<FString> Names = {TEXT("NS_chen_attack_01_start"),TEXT("NS_chen_attack_01_star_right"),TEXT("NS_chen_attack_01_start2")};
        const TArray<TArray<FVector3f>> Positions = {
            {{1,48.1f,93},{1,48.1f,93},{1,48.1f,93}},
            {{-4.80994218f,32.88545948f,90.87207449f},{-4.932428f,31.000996f,91.11505f},{-4.932428f,31.000996f,91.11505f},{-4.932428f,31.000996f,91.11505f},{-4.932428f,31.000996f,91.11505f},{-4.932428f,31.000996f,91.11505f}},
            {{-4.5702647f,98.043644f,114.07993f},{53,208,132}}
        };
        struct FTarget { UNiagaraSystem* System; UNiagaraStackFunctionInput* Input; FString Emitter; FVector3f Position; };
        TArray<FTarget> Targets;
        TArray<TSharedPtr<FNiagaraSystemViewModel>> Views;
        TArray<UNiagaraSystem*> Systems;
        FString Report = TEXT("system\temitter\tbefore_enabled\tbefore_xyz\tafter_enabled\tafter_xyz\texpected_xyz\n");
        auto Fail = [&](const FString& Reason)
        {
            // One-shot authoring: failure is never saved. Drain any setter-triggered compile before releasing Views.
            for (auto* System : Systems) System->WaitForCompilationComplete(true,false);
            FFileHelper::SaveStringToFile(TEXT("ERROR\t")+Reason, *Output);
            UE_LOG(LogTemp, Error, TEXT("Chen source offset audit failed: %s"), *Reason);
        };
        // Preflight every target before modifying any graph.
        for (int32 s=0; s<Names.Num(); ++s)
        {
            auto* System = LoadObject<UNiagaraSystem>(nullptr, *(TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Art/Niagara/Systems/BladeContact/")+Names[s]));
            const int32 ExpectedCount = s==0 ? 3 : s==1 ? 6 : 4;
            if (!System || System->GetNumEmitters()!=ExpectedCount) { Fail(Names[s]+TEXT(" count/load")); return; }
            auto View = MakeShared<FNiagaraSystemViewModel>();
            FNiagaraSystemViewModelOptions Options;
            Options.bCanAutoCompile=false; Options.bCanSimulate=false;
            Options.EditMode=ENiagaraSystemViewModelEditMode::SystemAsset;
            Options.MessageLogGuid=System->GetAssetGuid();
            View->Initialize(*System, Options);
            Views.Add(View); Systems.Add(System);
            for (int32 p=0; p<Positions[s].Num(); ++p)
            {
                const FString Emitter = FString::Printf(TEXT("SourceP%02d"), p+(s==2 ? 2 : 0));
                TArray<UNiagaraStackFunctionInput*> Inputs;
                for (const auto& Handle : View->GetEmitterHandleViewModels())
                    if (Handle->GetName()==FName(*Emitter)) GatherChenOffsetInputs(Handle->GetEmitterStackViewModel()->GetRootEntry(), Inputs);
                if (Inputs.Num()!=1 || Inputs[0]->GetInputType()!=FNiagaraTypeDefinition::GetVec3Def()
                    || !Inputs[0]->GetHasEditCondition() || Inputs[0]->GetValueMode()!=UNiagaraStackFunctionInput::EValueMode::Local)
                { Fail(Names[s]+TEXT("/")+Emitter+FString::Printf(TEXT(" unexpected Initialize input (%d)"),Inputs.Num())); return; }
                Targets.Add({System, Inputs[0], Emitter, Positions[s][p]});
            }
        }
        for (const auto& Target : Targets)
        {
            auto* Input=Target.Input;
            const bool bBefore=Input->GetEditConditionEnabled();
            const FVector3f Before=*reinterpret_cast<const FVector3f*>(Input->GetLocalValueStruct()->GetStructMemory());
            if (bApply)
            {
                if (!Before.Equals(Target.Position,0.0001f))
                {
                    auto Value=MakeShared<FStructOnScope>(FNiagaraTypeDefinition::GetVec3Def().GetScriptStruct());
                    *reinterpret_cast<FVector3f*>(Value->GetStructMemory())=Target.Position;
                    Input->SetLocalValue(Value);
                }
                Input->SetEditConditionEnabled(true);
            }
            const FVector3f After=*reinterpret_cast<const FVector3f*>(Input->GetLocalValueStruct()->GetStructMemory());
            const bool bAfter=Input->GetEditConditionEnabled();
            Report+=FString::Printf(TEXT("%s\t%s\t%d\t%s\t%d\t%s\t%s\n"),*Target.System->GetName(),*Target.Emitter,bBefore,*Before.ToString(),bAfter,*After.ToString(),*Target.Position.ToString());
            if (bApply && (!bAfter || !After.Equals(Target.Position,0.0001f))) { Fail(TEXT("Post-edit mismatch")); return; }
        }
        for (auto* System : Systems)
        {
            if (bApply) { System->RequestCompile(false); System->MarkPackageDirty(); }
            System->WaitForCompilationComplete(true,false);
            Report+=FString::Printf(TEXT("COMPILE\t%s\t%d\t%d\n"),*System->GetName(),System->GetNumEmitters(),System->IsReadyToRun());
        }
        FFileHelper::SaveStringToFile(Report,*Output);
        UE_LOG(LogTemp,Display,TEXT("Chen source offsets %s: %d existing inputs checked"),*Args[0],Targets.Num());
    }));

#endif
