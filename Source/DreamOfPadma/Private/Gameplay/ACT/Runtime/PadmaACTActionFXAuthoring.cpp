#if WITH_EDITOR
#include "HAL/IConsoleManager.h"
#include "HAL/FileManager.h"
#include "NiagaraSystem.h"
#include "NiagaraEmitter.h"
#include "NiagaraScript.h"
#include "NiagaraScriptSource.h"
#include "NiagaraGraph.h"
#include "NiagaraNodeAssignment.h"
#include "NiagaraNodeFunctionCall.h"
#include "NiagaraNodeOutput.h"
#include "EdGraph/EdGraphPin.h"
#include "ViewModels/NiagaraSystemViewModel.h"
#include "Misc/FileHelper.h"
#include "Containers/UnrealString.h"
#include "Misc/Paths.h"

// Scoped to this delivery's new derived systems. Never touches the Attack01 library.
static FAutoConsoleCommand PadmaChenActionFX(
    TEXT("Padma.ChenActions.FX"), TEXT("Reset|Finish|Order|Duration <NS_name> in Actions/FX; caller owns saving."),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
    {
        if (Args.Num()!=2 || (Args[0]!=TEXT("Reset") && Args[0]!=TEXT("Finish") && Args[0]!=TEXT("Order") && Args[0]!=TEXT("Duration")) || !Args[1].StartsWith(TEXT("NS_"))) return;
        for (TCHAR C:Args[1]) if (!FChar::IsAlnum(C) && C!=TEXT('_')) return;
        const FString Path=TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Art/Niagara/Systems/")+Args[1];
        auto* System=LoadObject<UNiagaraSystem>(nullptr,*Path);
        if (!System)
        {
            UE_LOG(LogTemp,Error,TEXT("Missing required Content Niagara system: %s"),*Path);
            return;
        }
        const FString ReportDir=FPaths::ProjectSavedDir()/TEXT("ChenAuthoring/FX");
        IFileManager::Get().MakeDirectory(*ReportDir,true);
        if (Args[0]==TEXT("Duration"))
        {
            // Immutable authoring values verified against all 375 existing applied-duration
            // report rows on 2026-09-22. Raw extraction remains an explicit offline step.
            const FString Config=FPaths::ProjectDir()/TEXT("Scripts/Editor/ChenActionFXEmitterDurations.tsv");
            TArray<FString> Lines; FString Audit;
            if (!FFileHelper::LoadFileToStringArray(Lines,*Config))
            {
                UE_LOG(LogTemp,Error,TEXT("Missing Chen FX duration config: %s"),*Config);
                return;
            }
            TMap<FString,float> Durations;
            TSet<FString> Keys;
            for (const auto& Line:Lines)
            {
                if (Line.TrimStartAndEnd().IsEmpty()) continue;
                TArray<FString> Fields; Line.ParseIntoArray(Fields,TEXT("\t"),false);
                float Duration=0;
                const FString Key=Fields.Num()==3 ? Fields[0]+TEXT("\t")+Fields[1] : FString();
                if (Fields.Num()!=3 || Fields[0].IsEmpty() || Fields[1].IsEmpty()
                    || !LexTryParseString(Duration,*Fields[2]) || !FMath::IsFinite(Duration) || Duration<=0 || Keys.Contains(Key))
                {
                    UE_LOG(LogTemp,Error,TEXT("Invalid or duplicate Chen FX duration config row: %s"),*Line);
                    return;
                }
                Keys.Add(Key);
                if (Fields[0]==Args[1]) Durations.Add(Fields[1],Duration);
            }
            // Validate complete coverage before touching any emitter; never silently skip missing values.
            if (Durations.IsEmpty())
            {
                UE_LOG(LogTemp,Error,TEXT("No duration config for %s"),*Args[1]);
                return;
            }
            for (const auto& Handle:System->GetEmitterHandles())
            {
                if (!Durations.Contains(Handle.GetName().ToString()))
                {
                    UE_LOG(LogTemp,Error,TEXT("Missing duration config for %s / %s"),*Args[1],*Handle.GetName().ToString());
                    return;
                }
            }
            for (const auto& Handle:System->GetEmitterHandles())
            {
                const float Duration=Durations.FindChecked(Handle.GetName().ToString());
                auto* Data=Handle.GetInstance().GetEmitterData();
                auto* Script=Data ? Data->EmitterUpdateScriptProps.Script.Get() : nullptr;
                if (!Script) continue;
                TArray<FNiagaraVariable> Variables; Script->RapidIterationParameters.GetParameters(Variables);
                bool bUpdated=false;
                for (const auto& Variable:Variables)
                {
                    if (Variable.GetType()==FNiagaraTypeDefinition::GetFloatDef() && Variable.GetName().ToString().EndsWith(TEXT(".EmitterState.Loop Duration")))
                    {
                        Script->RapidIterationParameters.SetParameterValue<float>(Duration,Variable);
                        bUpdated=true;
                        Audit+=FString::Printf(TEXT("%s\t%s\t%f\n"),*Handle.GetName().ToString(),*Variable.GetName().ToString(),Duration);
                    }
                }
                if (auto* Source=Cast<UNiagaraScriptSource>(Script->GetLatestSource()))
                {
                    // Converter clipboard inputs can be literal override pins rather
                    // than rapid-iteration parameters, depending on system settings.
                    for (const auto& Node:Source->NodeGraph->Nodes)
                        if (Node->GetClass()->GetFName()==TEXT("NiagaraNodeParameterMapSet"))
                            for (auto* Pin:Node->Pins)
                                if (Pin->Direction==EGPD_Input && Pin->LinkedTo.IsEmpty() && Pin->PinName.ToString().EndsWith(TEXT(".Loop Duration")))
                                {
                                    Node->Modify(); Pin->DefaultValue=FString::SanitizeFloat(Duration);
                                    if (!bUpdated) Audit+=FString::Printf(TEXT("%s\t%s\t%f\n"),*Handle.GetName().ToString(),*Pin->PinName.ToString(),Duration);
                                    bUpdated=true;
                                }
                    Source->NodeGraph->NotifyGraphChanged();
                    Script->GetLatestSource()->MarkNotSynchronized(TEXT("Burst duration safety margin"));
                    Script->GetLatestSource()->ForceGraphToRecompileOnNextCheck();
                }
                Script->MarkPackageDirty();
            }
            System->RequestCompile(true);
            FFileHelper::SaveStringToFile(Audit,*(ReportDir/(Args[1]+TEXT("-duration.tsv"))));
        }
        if (Args[0]==TEXT("Reset"))
        {
            System->WaitForCompilationComplete(true,false);
            TSet<FGuid> Ids;
            for (const auto& H:System->GetEmitterHandles()) Ids.Add(H.GetId());
            TSharedPtr<FNiagaraSystemViewModel> VM=MakeShared<FNiagaraSystemViewModel>();
            FNiagaraSystemViewModelOptions Options;
            Options.bCanAutoCompile=false; Options.bCanSimulate=false;
            Options.EditMode=ENiagaraSystemViewModelEditMode::SystemAsset;
            Options.MessageLogGuid=System->GetAssetGuid();
            VM->Initialize(*System,Options); VM->DeleteEmitters(Ids); VM.Reset();
        }
        if (Args[0]==TEXT("Order"))
        {
            // The converter groups direct assignments ahead of modules, regardless of
            // authoring call order. In particular, caching Velocity there caches zero.
            FString Audit;
            for (const auto& Handle : System->GetEmitterHandles())
            {
                const auto* Data=Handle.GetInstance().GetEmitterData();
                if (!Data) continue;
                for (auto* Script : {Data->SpawnScriptProps.Script.Get(),Data->UpdateScriptProps.Script.Get()})
                {
                    auto* Source=Script ? Cast<UNiagaraScriptSource>(Script->GetLatestSource()) : nullptr;
                    UNiagaraNodeOutput* Output=nullptr;
                    if (Source) for (const auto& Node : Source->NodeGraph->Nodes)
                        if (auto* Candidate=Cast<UNiagaraNodeOutput>(Node); Candidate && UNiagaraScript::IsEquivalentUsage(Candidate->GetUsage(),Script->GetUsage()) && Candidate->GetUsageId()==Script->GetUsageId()) Output=Candidate;
                    if (!Output) continue;
                    auto MapPin=[](UEdGraphNode* Node,EEdGraphPinDirection Direction)->UEdGraphPin*
                    {
                        for (auto* Pin : Node->Pins)
                            if (Pin->Direction==Direction && Pin->PinType.PinSubCategoryObject==FNiagaraTypeDefinition::GetParameterMapDef().GetStruct()) return Pin;
                        return nullptr;
                    };
                    TArray<UNiagaraNodeFunctionCall*> Nodes;
                    UEdGraphNode* Previous=Output;
                    while (auto* Pin=MapPin(Previous,EGPD_Input))
                    {
                        if (Pin->LinkedTo.Num()!=1) break;
                        Previous=Pin->LinkedTo[0]->GetOwningNode();
                        if (auto* Module=Cast<UNiagaraNodeFunctionCall>(Previous)) Nodes.Insert(Module,0);
                    }
                    if (Nodes.IsEmpty()) continue;
                    // Keep each module's override map and dynamic-input readers together,
                    // as the editor's stack-node groups do. Only boundary links change.
                    TMap<UNiagaraNodeFunctionCall*,TArray<UEdGraphPin*>> Starts;
                    UEdGraphPin* RootOutput=MapPin(Previous,EGPD_Output);
                    if (!RootOutput) continue;
                    auto* LastOutput=RootOutput;
                    for (auto* Node : Nodes)
                    {
                        Starts.Add(Node,LastOutput->LinkedTo);
                        LastOutput=MapPin(Node,EGPD_Output);
                    }
                    const auto EndInputs=LastOutput->LinkedTo;
                    Source->NodeGraph->Modify();
                    RootOutput->BreakAllPinLinks();
                    for (auto* Node : Nodes) MapPin(Node,EGPD_Output)->BreakAllPinLinks();
                    auto Rank=[](const UNiagaraNodeFunctionCall* Node)
                    {
                        const FString Asset=Node->FunctionScript ? Node->FunctionScript->GetName() : FString();
                        if (Asset.Contains(TEXT("InitializeParticle")) || Asset==TEXT("ParticleState")) return 0;
                        if (const auto* Assignment=Cast<UNiagaraNodeAssignment>(Node))
                        {
                            for (const auto& Target : Assignment->GetAssignmentTargets())
                                if (Target.GetName().ToString().StartsWith(TEXT("Particles.SourceShapeRandom"))) return -10;
                            for (const auto& Target : Assignment->GetAssignmentTargets())
                                if (Target.GetName()==TEXT("Particles.SourceInitialVelocity")) return 80;
                            return 20;
                        }
                        if (Asset==TEXT("SolveForcesAndVelocity")) return 100;
                        if (Asset==TEXT("GenerateLocationEvent")) return 110;
                        return 50;
                    };
                    Nodes.StableSort([&](const auto& A,const auto& B){return Rank(&A)<Rank(&B);});
                    for (int32 Index=0;Index<Nodes.Num();++Index)
                    {
                        auto* Before=Index==0 ? RootOutput : MapPin(Nodes[Index-1],EGPD_Output);
                        for (auto* Start : Starts[Nodes[Index]]) Before->MakeLinkTo(Start);
                        Audit+=FString::Printf(TEXT("%s\t%d\t%d\t%s\n"),*Handle.GetName().ToString(),int32(Script->GetUsage()),Index,*Nodes[Index]->GetFunctionName());
                    }
                    for (auto* End : EndInputs) MapPin(Nodes.Last(),EGPD_Output)->MakeLinkTo(End);
                    Source->NodeGraph->NotifyGraphChanged();
                    Script->GetLatestSource()->MarkNotSynchronized(TEXT("Source particle stack order corrected"));
                    Script->GetLatestSource()->ForceGraphToRecompileOnNextCheck();
                    Script->MarkPackageDirty();
                }
            }
            System->RequestCompile(true);
            FFileHelper::SaveStringToFile(Audit,*(ReportDir/(Args[1]+TEXT("-stack.tsv"))));
        }
        System->WaitForCompilationComplete(true,false);
        const FString Result=FString::Printf(TEXT("%s\t%d\t%d"),*Path,System->GetNumEmitters(),System->IsReadyToRun());
        FFileHelper::SaveStringToFile(Result,*(ReportDir/TEXT("compile.tsv")));
        // Compatibility output for the explicit offline AuthorChenActionFX.py importer.
        // No authoring command reads from this extraction/report directory.
        const FString LegacyReportDir=FPaths::ProjectDir()/TEXT("Artifacts/ChenQianyu/Actions/FX");
        IFileManager::Get().MakeDirectory(*LegacyReportDir,true);
        FFileHelper::SaveStringToFile(Result,*(LegacyReportDir/TEXT("compile.tsv")));
        UE_LOG(LogTemp,Display,TEXT("Chen action FX %s: %s"),*Args[0],*Result);
    }));
#endif
