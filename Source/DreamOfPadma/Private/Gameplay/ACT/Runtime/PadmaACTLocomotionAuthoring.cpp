#if WITH_EDITOR
#include "HAL/IConsoleManager.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "AnimGraphNode_StateMachine.h"
#include "AnimationStateMachineGraph.h"
#include "AnimStateNode.h"
#include "AnimStateTransitionNode.h"
#include "AnimationTransitionGraph.h"
#include "AnimGraphNode_TransitionResult.h"
#include "AnimGraphNode_SequencePlayer.h"
#include "K2Node_VariableGet.h"
#include "K2Node_CallFunction.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "EdGraphSchema_K2.h"

namespace PadmaChenLocomotion
{
template<class T> T* Add(UEdGraph* Graph)
{
    FGraphNodeCreator<T> Creator(*Graph); T* Node=Creator.CreateNode(); Creator.Finalize(); return Node;
}
static UEdGraphPin* Variable(UEdGraph* G,FName Name)
{
    FGraphNodeCreator<UK2Node_VariableGet> Creator(*G); auto* N=Creator.CreateNode();
    N->VariableReference.SetSelfMember(Name); Creator.Finalize(); return N->GetValuePin();
}
static UK2Node_CallFunction* Function(UEdGraph* G,FName Name)
{
    FGraphNodeCreator<UK2Node_CallFunction> Creator(*G); auto* N=Creator.CreateNode();
    N->SetFromFunction(UKismetMathLibrary::StaticClass()->FindFunctionByName(Name)); Creator.Finalize(); return N;
}
static void Link(UEdGraph* G,UEdGraphPin* A,UEdGraphPin* B)
{ check(A && B); check(G->GetSchema()->TryCreateConnection(A,B)); }
static void Transition(UEdGraph* G,UAnimStateNode* From,UAnimStateNode* To,bool Moving,bool Running)
{
    UAnimStateTransitionNode* T=nullptr;
    for (UEdGraphNode* N:G->Nodes) if (auto* Candidate=Cast<UAnimStateTransitionNode>(N);
        Candidate && Candidate->GetPreviousState()==From && Candidate->GetNextState()==To) T=Candidate;
    if (!T) { T=Add<UAnimStateTransitionNode>(G); T->CreateConnections(From,To); }
    T->CrossfadeDuration=.15f;
    auto* Rule=CastChecked<UAnimationTransitionGraph>(T->BoundGraph);
    auto* Result=Rule->GetResultNode();
    auto Old=Rule->Nodes;
    for (UEdGraphNode* N:Old) if (N!=Result) N->DestroyNode();
    Result->BreakAllNodeLinks();
    auto* Compare=Function(Rule,Moving ? TEXT("Greater_DoubleDouble") : TEXT("LessEqual_DoubleDouble"));
    Link(Rule,Variable(Rule,TEXT("GroundSpeed")),Compare->FindPinChecked(TEXT("A")));
    Compare->FindPinChecked(TEXT("B"))->DefaultValue=TEXT("5.0");
    auto* Value=Compare->GetReturnValuePin();
    if (Moving)
    {
        auto* Run=Variable(Rule,TEXT("bWantsToRun"));
        if (!Running) { auto* Not=Function(Rule,TEXT("Not_PreBool")); Link(Rule,Run,Not->FindPinChecked(TEXT("A"))); Run=Not->GetReturnValuePin(); }
        auto* And=Function(Rule,TEXT("BooleanAND"));
        Link(Rule,Value,And->FindPinChecked(TEXT("A"))); Link(Rule,Run,And->FindPinChecked(TEXT("B"))); Value=And->GetReturnValuePin();
    }
    Link(Rule,Value,Result->FindPinChecked(TEXT("bCanEnterTransition")));
}
static void Configure()
{
    auto* BP=LoadObject<UAnimBlueprint>(nullptr,TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Animation/AnimBlueprints/ABP_ACT_CHEN"));
    auto* RunAsset=LoadObject<UAnimSequence>(nullptr,TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Sequences/Runtime/AS_Chen_RunBase_IP"));
    if (!BP || !RunAsset) return;
    BP->Modify();
    FEdGraphPinType BoolType; BoolType.PinCategory=UEdGraphSchema_K2::PC_Boolean;
    if (FBlueprintEditorUtils::FindNewVariableIndex(BP,TEXT("bWantsToRun"))==INDEX_NONE)
        FBlueprintEditorUtils::AddMemberVariable(BP,TEXT("bWantsToRun"),BoolType);
    TArray<UEdGraph*> Graphs; BP->GetAllGraphs(Graphs);
    for (auto* G:Graphs)
    {
        if (!G->IsA<UAnimationStateMachineGraph>() || G->GetName()!=TEXT("SM_Locomotion")) continue;
        UAnimStateNode *Idle=nullptr,*Walk=nullptr,*Run=nullptr;
        for (UEdGraphNode* N:G->Nodes) if (auto* State=Cast<UAnimStateNode>(N))
        {
            if (State->GetStateName()==TEXT("Run")) Run=State;
            for (UEdGraphNode* Child:State->BoundGraph->Nodes) if (auto* Player=Cast<UAnimGraphNode_SequencePlayer>(Child))
            {
                auto* Asset=Player->GetAnimationAsset(); if (!Asset) continue;
                if (Asset->GetName().Contains(TEXT("IdleBase"))) Idle=State;
                if (Asset->GetName().Contains(TEXT("WalkBase")))
                {
                    Walk=State;
                    Player->Node.SetSequence(LoadObject<UAnimSequence>(nullptr,TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Sequences/Runtime/AS_Chen_WalkBase_IP")));
                    Player->Node.SetLoopAnimation(true);
                }
            }
        }
        check(Idle && Walk);
        if (!Run) { Run=Add<UAnimStateNode>(G); FBlueprintEditorUtils::RenameGraph(Run->BoundGraph,TEXT("Run")); Run->NodePosX=Walk->NodePosX+300; Run->NodePosY=Walk->NodePosY; }
        UAnimGraphNode_SequencePlayer* Player=nullptr;
        for (UEdGraphNode* N:Run->BoundGraph->Nodes) if (auto* P=Cast<UAnimGraphNode_SequencePlayer>(N)) Player=P;
        if (!Player) Player=Add<UAnimGraphNode_SequencePlayer>(Run->BoundGraph);
        Player->Node.SetSequence(RunAsset); Player->Node.SetLoopAnimation(true);
        Link(Run->BoundGraph,Player->FindPinChecked(TEXT("Pose")),Run->GetPoseSinkPinInsideState());
        Transition(G,Idle,Walk,true,false); Transition(G,Idle,Run,true,true);
        Transition(G,Walk,Run,true,true); Transition(G,Run,Walk,true,false);
        Transition(G,Walk,Idle,false,false); Transition(G,Run,Idle,false,false);
    }
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
    FKismetEditorUtilities::CompileBlueprint(BP);
    check(BP->Status!=BS_Error);
    CastChecked<UAnimInstance>(BP->GeneratedClass->GetDefaultObject())->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
    BP->MarkPackageDirty();
    // Swap only the dedicated Actions montages. Source imports and legacy blade fixture stay intact.
    const TCHAR* Names[]={TEXT("Attack01"),TEXT("Attack02"),TEXT("Attack03"),TEXT("Attack04"),TEXT("Attack05"),TEXT("GuiQiongYu"),TEXT("JianTianHe"),TEXT("LieFengShuang"),TEXT("Execution"),TEXT("Dodge"),TEXT("PerfectDodge"),TEXT("Plunge"),TEXT("Jump")};
    for (auto* Name:Names)
    {
        auto* M=LoadObject<UAnimMontage>(nullptr,*FString::Printf(TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Montages/AM_Chen_%s"),Name)); check(M);
        M->Modify();
        for (auto& Slot:M->SlotAnimTracks) for (auto& Segment:Slot.AnimTrack.AnimSegments)
        {
            UAnimSequenceBase* Source=Segment.GetAnimReference(); check(Source);
            FString Base=Source->GetName(); Base.RemoveFromEnd(TEXT("_CM")); Base.RemoveFromEnd(TEXT("_Runtime"));
            auto* A=LoadObject<UAnimSequence>(nullptr,*(TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Sequences/Runtime/")+Base+TEXT("_Runtime"))); check(A);
            Segment.SetAnimReference(A);
        }
        M->RefreshCacheData(); M->MarkPackageDirty();
    }
    UE_LOG(LogTemp,Display,TEXT("Chen locomotion graph and montage roots configured"));
}
static FAutoConsoleCommand Command(TEXT("Padma.ChenActions.ConfigureLocomotion"),TEXT("Update Chen locomotion and runtime root tracks; caller saves."),FConsoleCommandDelegate::CreateStatic(&Configure));
}
#endif
