#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Gameplay/ACT/Runtime/PadmaACTTrainingDummyPresentation.h"
#include "Gameplay/ACT/Runtime/PadmaACTImpactLocation.h"
#include "Gameplay/ACT/Runtime/PadmaACTMeleeLab.h"
#include "Gameplay/ACT/Runtime/PadmaACTTrainingAnalytics.h"
#include "Gameplay/ACT/Authoring/PadmaACTAuthoring.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/WorldSettings.h"
#include "Gameplay/ACT/Runtime/PadmaACTActions.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "UObject/UObjectIterator.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaACTTrainingDummyTest, "DreamOfPadma.ACT.WoodenTrainingDummy",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPadmaACTTrainingDummyTest::RunTest(const FString&)
{
    auto* Mesh=LoadObject<USkeletalMesh>(nullptr,TEXT("/Game/Sandbox/ACT/Training/WoodenDummy/Art/Meshes/SK_WoodenDummy"));
    auto* Clip=LoadObject<UAnimSequence>(nullptr,TEXT("/Game/Sandbox/ACT/Training/WoodenDummy/Animation/Sequences/AS_WoodenDummy_Hit"));
    if (!TestNotNull(TEXT("Wood mesh loads"),Mesh) || !TestNotNull(TEXT("Source hit animation loads"),Clip)) return false;
    UWorld::InitializationValues Values;
    Values.AllowAudioPlayback(false).CreatePhysicsScene(true).ShouldSimulatePhysics(false).CreateNavigation(false).CreateAISystem(false);
    auto* World=UWorld::CreateWorld(EWorldType::Game,false,NAME_None,nullptr,true,ERHIFeatureLevel::Num,&Values);
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT { World->EndPlay(EEndPlayReason::Quit); World->DestroyWorld(false); GEngine->DestroyWorldContext(World); };
    World->InitializeActorsForPlay(FURL());
    World->BeginPlay();
    World->GetWorldSettings()->NotifyBeginPlay();
    auto* Host=World->SpawnActor<AActor>();
    auto* Battle=NewObject<UPadmaCombatComponent>(Host);Host->AddInstanceComponent(Battle);Battle->RegisterComponent();
    Battle->PayCost=[](float,float,FName,FString&){return true;};
    auto* Analytics=NewObject<UPadmaACTTrainingAnalyticsComponent>(Host);Host->AddInstanceComponent(Analytics);Analytics->RegisterComponent();Analytics->BindBattle(Battle);
    FPadmaCombatSetup Setup;Setup.Mode=EPadmaCombatMode::ACT;
    FPadmaCombatUnitSpec Player;Player.Id=TEXT("player");Player.bPlayer=true;Player.Attack=20;Player.AttackRange=500;Player.Health=Player.MaxHealth=1000;
    Player.Location=FVector(0,0,90);Setup.Units.Add(Player);
    for(int32 I=0;I<3;++I)
    {
        auto D=Player;D.Id=FName(*FString::Printf(TEXT("dummy-%d"),I));D.bPlayer=false;D.Attack=0;D.bCanPursueInACT=false;
        D.Location=FVector(120+I*100,0,90);D.Presentation.Model=Mesh;Setup.Units.Add(D);
    }
    FString Failure;
    if(!TestTrue(TEXT("Battle starts"),Battle->StartBattle(Setup,Failure))) return false;
    TArray<UPadmaACTTrainingDummyPresentation*> Reactions;
    for(int32 I=0;I<3;++I)
    {
        auto* Unit=Battle->GetUnit(Setup.Units[I+1].Id);
        auto* R=NewObject<UPadmaACTTrainingDummyPresentation>(Unit);Unit->AddInstanceComponent(R);R->RegisterComponent();
        TestTrue(TEXT("Matching source skeleton configures"),R->Configure(Battle,Clip));Reactions.Add(R);
    }
    auto* Target=Battle->GetUnit(TEXT("dummy-0"));auto* Single=Target->GetMesh()->GetSingleNodeInstance();
    if(!TestNotNull(TEXT("Single node animation player"),Single)) return false;
    const FTransform Before=Target->GetActorTransform();
    FHitResult Contact;
    Contact.Location=FVector(0,0,400);Contact.ImpactPoint=Contact.Location;Contact.bStartPenetrating=true;
    TestEqual(TEXT("Overlapping blade impact stays on dummy, not raised sweep center"),
        PadmaACT::ResolveImpactLocation(Contact,*Target,NAME_None,false),Target->GetActorLocation());
    Contact.bStartPenetrating=false;
    TestEqual(TEXT("Volume query uses target instead of query center"),
        PadmaACT::ResolveImpactLocation(Contact,*Target,NAME_None,true),Target->GetActorLocation());
    Contact.ImpactPoint=Target->GetActorLocation()+FVector(-22,0,15);
    TestEqual(TEXT("Real blade surface contact is preserved"),
        PadmaACT::ResolveImpactLocation(Contact,*Target,NAME_None,false),Contact.ImpactPoint);
    for(const auto& Unit:Battle->GetUnits()) Unit->GetCharacterMovement()->DisableMovement();
    auto Advance=[&](float Seconds)
    {
        for(float Left=Seconds;Left>UE_SMALL_NUMBER;Left-=1.f/60)
        {
            ++GFrameCounter;
            World->Tick(LEVELTICK_All,FMath::Min(Left,1.f/60));
        }
    };
    TestFalse(TEXT("Starts at rest"),Single->IsPlaying());
    Analytics->Reset();
    TestTrue(TEXT("Real attack settles"),Battle->Attack({TEXT("dummy-0")},Failure));
    TestTrue(TEXT("Hit starts its reaction"),Reactions[0]->IsReacting() && Single->IsPlaying());
    TestFalse(TEXT("Other target remains still"),Reactions[1]->IsReacting());
    TestEqual(TEXT("One authoritative receipt"),Analytics->GetHitCount(NAME_None),1);
    TestTrue(TEXT("Real damage and GE recorded"),Target->Health()<1000 && !Analytics->GetGameplayEffectRecords().IsEmpty());
    Advance(.2f);
    TestTrue(TEXT("Animation time progresses"),Single->GetCurrentTime()>.1f);
    Battle->GetPlayerUnit()->AttackCooldown=0;
    TestTrue(TEXT("Second hit settles"),Battle->Attack({TEXT("dummy-0")},Failure));
    TestTrue(TEXT("Repeated hit restarts animation"),Single->GetCurrentTime()<.01f);
    Advance(Clip->GetPlayLength()+.1f);
    TestFalse(TEXT("One shot completes"),Reactions[0]->IsReacting());
    TestEqual(TEXT("Returns to source rest pose"),Single->GetCurrentTime(),0.f);
    TestTrue(TEXT("Visual reaction never moves gameplay actor"),Target->GetActorTransform().Equals(Before));
    Target->SetActorLocation(FVector(5000,0,90));Battle->GetPlayerUnit()->AttackCooldown=0;
    Battle->Attack({TEXT("dummy-0")},Failure);
    TestFalse(TEXT("Out of range swing cannot animate target"),Reactions[0]->IsReacting());
    FPadmaCombatReceipt Buff;Buff.Effect=TEXT("shield");Buff.TargetId=Target->Spec.Id;Battle->OnReceipt.Broadcast(Buff);
    TestFalse(TEXT("Non-damage receipt cannot animate target"),Reactions[0]->IsReacting());
    Reactions[0]->DestroyComponent();
    Buff.Effect=TEXT("damage");Battle->OnReceipt.Broadcast(Buff);
    TestFalse(TEXT("Destroyed component unsubscribes"),Single->IsPlaying());
    Battle->ExitBattle();

    auto* Lab=World->SpawnActor<APadmaACTMeleeLab>();
    if(!TestNotNull(TEXT("Lab spawns"),Lab)) return false;
    Lab->CharacterDefinition=LoadObject<UPadmaACTCharacterDefinition>(nullptr,TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/AbilitySystem/DA_ACTCharacter_CHEN"));
    if(!TestNotNull(TEXT("Lab character loads"),Lab->CharacterDefinition.Get())) return false;
    Lab->TrainingDummyMesh=Mesh;Lab->TrainingDummyHitAnimation=Clip;Lab->ResetBattle();
    auto* Old=Lab->Battle->GetUnit(TEXT("blade-dummy"));
    if(!TestNotNull(TEXT("Lab target spawns"),Old)) return false;
    TestNotNull(TEXT("Actual lab target has reaction component"),Old->FindComponentByClass<UPadmaACTTrainingDummyPresentation>());
    Lab->ResetBattle();
    TestEqual(TEXT("F8 restores all three targets plus player"),Lab->Battle->GetUnits().Num(),4);
    auto* Fresh=Lab->Battle->GetUnit(TEXT("blade-dummy"));
    if(!TestNotNull(TEXT("Reset target spawns"),Fresh)) return false;
    TestTrue(TEXT("Reset replaces old target"),Fresh!=Old);
    auto* FreshSingle=Fresh->GetMesh()->GetSingleNodeInstance();
    if(!TestNotNull(TEXT("Reset animation instance exists"),FreshSingle)) return false;
    TestFalse(TEXT("Reset target is at rest"),FreshSingle->IsPlaying());
    auto* Chen=Lab->Battle->GetPlayerUnit();
    if(!TestNotNull(TEXT("Chen exists for fourth attack"),Chen)) return false;
    for(const auto& Unit:Lab->Battle->GetUnits())
    {
        Unit->GetCharacterMovement()->DisableMovement();
        Unit->SetActorLocation(FVector(5000,0,90));
    }
    Chen->SetActorLocation(FVector(0,0,90));Chen->SetActorRotation(FRotator::ZeroRotator);
    Fresh->SetActorLocation(FVector(120,0,90));
    Lab->Battle->SetComponentTickEnabled(false);
    TestTrue(TEXT("Fourth normal attack starts"),Chen->GetActions()->RequestInput(TEXT("Combo4")));
    TSet<UNiagaraComponent*> ObservedImpacts;
    for(int32 Frame=0;Frame<60;++Frame)
    {
        ++GFrameCounter;World->Tick(LEVELTICK_All,1.f/60);
        for(TObjectIterator<UNiagaraComponent> It;It;++It)
        {
            if(It->GetWorld()!=World || !It->GetAsset() || !It->GetAsset()->GetName().Contains(TEXT("common_hit")) || ObservedImpacts.Contains(*It)) continue;
            ObservedImpacts.Add(*It);
            TestTrue(TEXT("Fourth attack impact spawns on wooden target"),
                FVector::Distance(It->GetComponentLocation(),Fresh->GetActorLocation())<100.f);
        }
    }
    TestTrue(TEXT("Fourth attack produces actual contact effects"),ObservedImpacts.Num()>0);
    return true;
}
