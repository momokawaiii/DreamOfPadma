#if WITH_EDITOR
#include "HAL/IConsoleManager.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimSequence.h"
#include "AnimationStateMachineGraph.h"
#include "AnimStateNode.h"
#include "AnimGraphNode_SequencePlayer.h"
#include "AnimGraphNode_BlendListByBool.h"
#include "AnimGraphNode_Slot.h"
#include "K2Node_VariableGet.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "EdGraphSchema_K2.h"

namespace PadmaChenWeaponIdle
{
template<class T> T* Add(UEdGraph* Graph)
{
    FGraphNodeCreator<T> Creator(*Graph); auto* Node=Creator.CreateNode(); Creator.Finalize(); return Node;
}
static void Link(UEdGraph* G,UEdGraphPin* A,UEdGraphPin* B)
{ check(A && B); check(G->GetSchema()->TryCreateConnection(A,B)); }
static void Configure()
{
    auto* BP=LoadObject<UAnimBlueprint>(nullptr,TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Animation/AnimBlueprints/ABP_ACT_CHEN"));
    auto* CombatIdle=LoadObject<UAnimSequence>(nullptr,TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Sequences/Imported/AS_Chen_CombatIdle_CM"));
    if (!BP || !CombatIdle) return;
    BP->Modify();
    FEdGraphPinType BoolType; BoolType.PinCategory=UEdGraphSchema_K2::PC_Boolean;
    if (FBlueprintEditorUtils::FindNewVariableIndex(BP,TEXT("bWeaponCombatIdle"))==INDEX_NONE)
        FBlueprintEditorUtils::AddMemberVariable(BP,TEXT("bWeaponCombatIdle"),BoolType);
    TArray<UEdGraph*> Graphs; BP->GetAllGraphs(Graphs);
    int32 Configured=0;
    for (auto* G:Graphs)
    {
        // Keep the armed idle evaluated beneath Montage blends, avoiding a neutral-pose flash on completion.
        for (UEdGraphNode* N:G->Nodes) if (auto* Slot=Cast<UAnimGraphNode_Slot>(N); Slot && Slot->Node.SlotName==TEXT("DefaultSlot"))
            Slot->Node.bAlwaysUpdateSourcePose=true;
        if (!G->IsA<UAnimationStateMachineGraph>() || G->GetName()!=TEXT("SM_Locomotion")) continue;
        for (UEdGraphNode* N:G->Nodes) if (auto* State=Cast<UAnimStateNode>(N))
        {
            UAnimGraphNode_SequencePlayer *Neutral=nullptr,*Armed=nullptr;
            UAnimGraphNode_BlendListByBool* Blend=nullptr;
            UEdGraph* PoseGraph=State->BoundGraph;
            for (UEdGraphNode* Child:PoseGraph->Nodes)
            {
                if (auto* P=Cast<UAnimGraphNode_SequencePlayer>(Child); P && P->GetAnimationAsset())
                {
                    if (P->GetAnimationAsset()->GetName()==TEXT("AS_Chen_IdleBase_CM")) Neutral=P;
                    if (P->GetAnimationAsset()==CombatIdle) Armed=P;
                }
                if (Child->NodeComment==TEXT("PadmaWeaponCombatIdle")) Blend=Cast<UAnimGraphNode_BlendListByBool>(Child);
            }
            if (!Neutral) continue;
            if (!Armed) Armed=Add<UAnimGraphNode_SequencePlayer>(PoseGraph);
            Armed->Node.SetSequence(CombatIdle); Armed->Node.SetLoopAnimation(true);
            Neutral->Node.SetLoopAnimation(true);
            if (!Blend)
            {
                // Refuse to bypass an unrelated user-authored idle graph.
                auto* Sink=State->GetPoseSinkPinInsideState();
                check(Sink->LinkedTo.Num()==1 && Sink->LinkedTo[0]->GetOwningNode()==Neutral);
                Blend=Add<UAnimGraphNode_BlendListByBool>(PoseGraph); Blend->NodeComment=TEXT("PadmaWeaponCombatIdle");
                FGraphNodeCreator<UK2Node_VariableGet> Creator(*PoseGraph); auto* Value=Creator.CreateNode();
                Value->VariableReference.SetSelfMember(TEXT("bWeaponCombatIdle")); Creator.Finalize();
                Link(PoseGraph,Value->GetValuePin(),Blend->FindPinChecked(TEXT("bActiveValue")));
                Sink->BreakAllPinLinks();
            }
            Blend->NodePosX=Neutral->NodePosX+350; Armed->NodePosY=Neutral->NodePosY-220;
            Blend->FindPinChecked(TEXT("BlendTime_0"))->DefaultValue=TEXT("0.10");
            Blend->FindPinChecked(TEXT("BlendTime_1"))->DefaultValue=TEXT("0.10");
            Link(PoseGraph,Armed->FindPinChecked(TEXT("Pose")),Blend->FindPinChecked(TEXT("BlendPose_0")));
            Link(PoseGraph,Neutral->FindPinChecked(TEXT("Pose")),Blend->FindPinChecked(TEXT("BlendPose_1")));
            Link(PoseGraph,Blend->FindPinChecked(TEXT("Pose")),State->GetPoseSinkPinInsideState());
            ++Configured;
        }
    }
    check(Configured==1);
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
    FKismetEditorUtilities::CompileBlueprint(BP); check(BP->Status!=BS_Error);
    BP->MarkPackageDirty();
    UE_LOG(LogTemp,Display,TEXT("Chen combat idle connected; walk/run transitions and action Slot preserved"));
}
static FAutoConsoleCommand Command(TEXT("Padma.ChenActions.ConfigureCombatIdle"),TEXT("Wire the owned armed/neutral idle selector; caller saves."),FConsoleCommandDelegate::CreateStatic(&Configure));
}
#endif
