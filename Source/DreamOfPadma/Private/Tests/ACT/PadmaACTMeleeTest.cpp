#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Gameplay/ACT/Runtime/PadmaACTMelee.h"
#include "Gameplay/ACT/Runtime/PadmaACTAnimNotifies.h"
#include "Gameplay/ACT/Runtime/PadmaACTAbility.h"
#include "Gameplay/ACT/Authoring/PadmaACTAuthoring.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Gameplay/Combat/PadmaCombatAttributes.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "PadmaACTLegacyFixture.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaAnimatedBladeTest,"DreamOfPadma.ACT.AnimatedBladeContact",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaAnimatedBladeTest::RunTest(const FString& Parameters)
{
    PadmaACTTest::FLegacyMeleeFixture Fixture;
    if (!Fixture.Initialize(*this)) return false;
    auto* Definition=Fixture.Character.Get();
    TestFalse(TEXT("Fixture exercises generic legacy ability"),Definition->bUseCharacterActions);
    TestEqual(TEXT("Legacy slots are transient and empty"),Fixture.Slots->GetRowMap().Num(),0);
    // Retain the former saved-Montage contract checks as well as the engine-driven
    // regression below. The material test now checks the different live action rules.
    int32 Starts[3]={0,0,0},Stops[3]={0,0,0},Hits[2]={0,0},Combos=0;
    for (const auto& Event:Fixture.Montage->Notifies)
    {
        if (auto* FX=Cast<UAnimNotify_PadmaACTFX>(Event.Notify))
        {
            int32 Index=INDEX_NONE;
            for(int32 I=0;I<3;++I) if(FX->Effect.System.Get()==Fixture.Systems[I].Get()) Index=I;
            if(!TestTrue(TEXT("Legacy notify uses retained source FX"),Index!=INDEX_NONE)) return false;
            ++(FX->bStop ? Stops[Index] : Starts[Index]);
            TestFalse(TEXT("Legacy action FX remain stationary"),FX->Effect.bFollowMount);
            TestEqual(TEXT("Legacy action FX use character root"),FX->Effect.WeaponIndex,-1);
            TestTrue(TEXT("Legacy source FX have no extra mount offset"),FX->Effect.RelativeTransform.Equals(FTransform::Identity));
            TestTrue(TEXT("Legacy burst/stop time preserved"),FMath::IsNearlyEqual(Event.GetTime(),
                FX->bStop ? (Index==2 ? 53.f : 50.f)/30.f : (Index==2 ? 10.f : 7.f)/30.f,.0001f));
        }
        if (auto* Hit=Cast<UAnimNotifyState_PadmaACTHitWindow>(Event.NotifyStateClass))
        {
            const int32 Index=Hit->Window.WeaponIndex;
            if(!TestTrue(TEXT("Legacy contact uses main or offhand blade"),Index==0 || Index==1)) return false;
            ++Hits[Index];
            TestTrue(TEXT("Each legacy blade retains its exact impact system"),Hit->Window.HitEffect.Get()==Fixture.Systems[3+Index].Get());
            TestTrue(TEXT("Legacy contact time preserved"),FMath::IsNearlyEqual(Event.GetTime(),(Index==0 ? 19.f : 11.f)/60.f,.0001f));
            TestTrue(TEXT("Legacy contact duration preserved"),FMath::IsNearlyEqual(Event.GetDuration(),.1f,.0001f));
            TestEqual(TEXT("Legacy damage multiplier preserved"),Hit->Window.DamageMultiplier,.2f);
            TestEqual(TEXT("Legacy impact locator preserved"),Hit->Window.HitSocket,FName(TEXT("VB_Hit")));
            TestTrue(TEXT("Legacy impact faces target in plane"),Hit->Window.bFaceTargetPlanar);
            TestTrue(TEXT("Legacy contact uses blade sweep, not action volumes"),Hit->Window.Shapes.IsEmpty() && Hit->Window.AreaRadius==0 && !Hit->Window.bPrimaryTargetOnly);
        }
        if (Cast<UAnimNotifyState_PadmaACTComboWindow>(Event.NotifyStateClass))
        {
            ++Combos;
            TestTrue(TEXT("Legacy combo opens at thirty percent"),FMath::IsNearlyEqual(Event.GetTime(),Fixture.Montage->GetPlayLength()*.3f,.0001f));
            TestTrue(TEXT("Legacy combo closes at ninety percent"),FMath::IsNearlyEqual(Event.GetDuration(),Fixture.Montage->GetPlayLength()*.6f,.0001f));
        }
    }
    for(int32 Count:Starts) TestEqual(TEXT("Each legacy source burst occurs once"),Count,1);
    for(int32 Count:Stops) TestEqual(TEXT("Each legacy source stop occurs once"),Count,1);
    for(int32 Count:Hits) TestEqual(TEXT("Each legacy blade has one contact window"),Count,1);
    TestEqual(TEXT("One legacy combo window"),Combos,1);
    TestTrue(TEXT("Legacy Attack01 section retained"),Fixture.Montage->GetSectionIndex(TEXT("Attack01"))!=INDEX_NONE);
    UWorld::InitializationValues Values; Values.AllowAudioPlayback(false).CreatePhysicsScene(true)
        .ShouldSimulatePhysics(false).EnableTraceCollision(true).CreateNavigation(false).CreateAISystem(false);
    auto* World=UWorld::CreateWorld(EWorldType::Game,false,NAME_None,nullptr,true,ERHIFeatureLevel::Num,&Values);
    if (!TestNotNull(TEXT("Trace world"),World)) return false;
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT { World->EndPlay(EEndPlayReason::Quit); World->DestroyWorld(false); GEngine->DestroyWorldContext(World); };
    World->InitializeActorsForPlay(FURL()); World->BeginPlay(); World->GetWorldSettings()->NotifyBeginPlay();
    auto* Host=World->SpawnActor<AActor>(); auto* Battle=NewObject<UPadmaCombatComponent>(Host);
    Host->AddInstanceComponent(Battle);Battle->RegisterComponent();Battle->SetComponentTickEnabled(false);
    Battle->PayCost=[](float,float,FName,FString&){return true;};
    FPadmaCombatSetup Setup;Setup.Mode=EPadmaCombatMode::ACT;
    FPadmaCombatUnitSpec P;P.Id=TEXT("chen");P.Health=P.MaxHealth=100;P.Attack=20;P.Speed=100;
    P.Presentation.ACTDefinition=Definition;P.Location=FVector(0,0,90);Setup.Units.Add(P);
    auto E=P;E.Id=TEXT("dummy");E.bPlayer=false;E.Presentation={};E.Location=FVector(1000,0,90);Setup.Units.Add(E);
    FString Failure;
    if (!TestTrue(TEXT("ACT fixture starts"),Battle->StartBattle(Setup,Failure))) { AddError(Failure); return false; }
    auto* Player=Battle->GetPlayerUnit();auto* Enemy=Battle->GetUnit(TEXT("dummy"));auto* Melee=Player->GetMelee();
    Enemy->GetCapsuleComponent()->SetCapsuleSize(24,88);
    // Drive the real engine world/animation/task/notify pipeline; never pose or advance melee manually.
    auto Step=[&](float Delta)
    {
        ++GFrameCounter; // Separate synthetic engine frames inside this synchronous automation command.
        World->Tick(LEVELTICK_All,Delta);
    };
    auto Run=[&](float Seconds)
    {
        for (float Left=Seconds;Left>UE_SMALL_NUMBER;Left-=1.f/120.f) Step(FMath::Min(Left,1.f/120.f));
    };
    auto Start=[&](float Rate)
    {
        Player->GetAbilitySystemComponent()->CancelAllAbilities();Run(.2f);
        Enemy->SetActorLocation(FVector(1000,0,90));
        Player->AttackCooldown=0;Melee->SetAttackPlayRate(Rate);
        return TestTrue(TEXT("GAS starts montage"),Battle->Attack({},Failure));
    };
    Run(.05f);
    if (!TestTrue(TEXT("Profile configured"),Melee->IsConfigured())) return false;
    auto* Ability=Player->GetAbilitySystemComponent()->FindAbilitySpecFromClass(UPadmaACTAbility::StaticClass());
    const auto* Montage=Melee->GetAttackMontage();
    if (!TestNotNull(TEXT("Runtime montage"),Montage)) return false;
    for (float Rate : {.5f,1.f,2.f})
    {
        if (!Start(Rate)) return false;
        TestTrue(TEXT("GAS remains active"),Ability && Ability->IsActive());
        TestFalse(TEXT("No activation reentry"),Battle->Attack({},Failure));
        TestFalse(TEXT("Early combo rejected"),Melee->RequestCombo());
        const FTransform WeaponBefore=Melee->GetWeapon(0)->GetComponentTransform();
        Run(.15f/Rate);
        TestFalse(TEXT("Slot evaluates moving attack weapon pose"),Melee->GetWeapon(0)->GetComponentTransform().Equals(WeaponBefore,.01f));
        TestTrue(TEXT("Montage advances at requested rate"),FMath::IsNearlyEqual(Melee->GetAttackTime(),.15f,.025f));
        TestEqual(TEXT("No early burst"),Melee->GetSpawnedActionEffects(),0);
        Run(.35f/Rate);
        TestEqual(TEXT("All same-time and later AN bursts fired once"),Melee->GetSpawnedActionEffects(),3);
        TestEqual(TEXT("Whiff has no contacts"),Melee->GetConfirmedContacts(),0);
        TestEqual(TEXT("Whiff has no hit FX"),Melee->GetSpawnedHitEffects(),0);
        TestEqual(TEXT("ANS closed both windows"),Melee->GetActiveWindowCount(),0);
        TArray<UNiagaraComponent*> Components;World->GetWorldSettings()->GetComponents(Components);
        for (auto* C : Components)
        {
            if (!IsValid(C) || !C->GetAsset()->GetName().StartsWith(TEXT("NS_chen_attack_01"))) continue;
            TestNull(TEXT("Stationary source action FX are world-owned"),C->GetAttachParent());
            TestEqual(TEXT("Action particle age follows attack speed"),C->GetCustomTimeDilation(),Rate);
        }
        Run((Montage->GetPlayLength()-.5f)/Rate+.2f);
        TestFalse(TEXT("Montage completion ends GAS"),Ability->IsActive());
        TestFalse(TEXT("Montage completion clears attack"),Melee->IsAttacking());
        TestEqual(TEXT("Completion cleans action FX"),Melee->GetLiveActionEffectCount(),0);
    }
    Start(1.f);World->GetWorldSettings()->SetTimeDilation(.1f);Step(.1f);
    TestTrue(TEXT("World slow motion affects Montage once"),FMath::IsNearlyEqual(Melee->GetAttackTime(),.01f,.002f));
    World->GetWorldSettings()->SetTimeDilation(1.f);
    Start(.5f); Run(.2f);const float Before=Melee->GetAttackTime();Melee->SetAttackPlayRate(2.f);Run(.1f);
    TestTrue(TEXT("Mid-attack rate change changes Montage progression"),FMath::IsNearlyEqual(Melee->GetAttackTime()-Before,.2f,.025f));
    TestEqual(TEXT("Rate change does not duplicate same-time bursts"),Melee->GetSpawnedActionEffects(),2);
    Start(1.f);Run(.19f);
    Enemy->SetActorLocation(Melee->GetWeapon(1)->GetComponentTransform().TransformPosition(FVector(0,0,55)));
    Run(.01f);
    TestEqual(TEXT("Offhand ANS uses actual evaluated blade contact"),Melee->GetConfirmedContacts(),1);
    TestEqual(TEXT("Damage settled through GAS"),Enemy->Health(),96.f);
    TestEqual(TEXT("Contact creates one impact"),Melee->GetSpawnedHitEffects(),1);
    Run(.02f);TestEqual(TEXT("No repeated damage within one window"),Enemy->Health(),96.f);
    Enemy->SetActorLocation(FVector(1000,0,90));Run(.1f);
    Enemy->SetActorLocation(Melee->GetWeapon(0)->GetComponentTransform().TransformPosition(FVector(0,0,55)));
    Run(.01f);
    TestEqual(TEXT("Main-hand ANS confirms separate contact"),Melee->GetConfirmedContacts(),2);
    TestEqual(TEXT("Two impacts from two real contacts"),Melee->GetSpawnedHitEffects(),2);
    TestEqual(TEXT("Two configured settlements"),Enemy->Health(),92.f);
    Enemy->SetActorLocation(FVector(1000,0,90));Run(.85f);
    TestTrue(TEXT("ANS opens combo acceptance"),Melee->IsComboWindowOpen());
    TestTrue(TEXT("Combo input accepted"),Melee->RequestCombo());Step(1.f/120.f);
    TestEqual(TEXT("Section jump starts second cycle"),Melee->GetAttackCycle(),2);
    TestTrue(TEXT("Section jump resets Montage position"),Melee->GetAttackTime()<.02f);
    TestTrue(TEXT("Same GAS activation survives combo"),Ability->IsActive());
    Run(.19f);Enemy->SetActorLocation(Melee->GetWeapon(1)->GetComponentTransform().TransformPosition(FVector(0,0,55)));Run(.01f);
    TestEqual(TEXT("Combo resets per-window deduplication"),Melee->GetConfirmedContacts(),3);
    TestEqual(TEXT("Combo settles damage again"),Enemy->Health(),88.f);
    Player->GetAbilitySystemComponent()->CancelAllAbilities();
    TestFalse(TEXT("Cancel inside ANS ends GAS"),Ability->IsActive());
    TestEqual(TEXT("Cancel closes ANS"),Melee->GetActiveWindowCount(),0);
    TestEqual(TEXT("Cancel destroys action FX"),Melee->GetLiveActionEffectCount(),0);
    Run(.6f);TestEqual(TEXT("Cancelled notify tail cannot damage"),Enemy->Health(),88.f);
    Start(1.f);Player->GetAbilitySystemComponent()->CancelAllAbilities();Run(.5f);
    TestEqual(TEXT("Cancel before first notify spawns nothing"),Melee->GetSpawnedActionEffects(),0);
    Start(1.f);Run(.35f);Player->GetMesh()->GetAnimInstance()->Montage_Stop(.05f);
    TestFalse(TEXT("External montage interrupt ends GAS"),Ability->IsActive());
    TestEqual(TEXT("Interrupt removes slash instances"),Melee->GetLiveActionEffectCount(),0);
    // One engine frame crosses the entire offhand window: retain Begin+End until post-pose sweep.
    Start(4.f);Enemy->GetCapsuleComponent()->SetCapsuleSize(200,200);Enemy->SetActorLocation(Player->GetActorLocation());Step(.075f);
    TestTrue(TEXT("Cross-window frame performs real sweep and settlement"),Melee->GetConfirmedContacts()>0);
    Enemy->GetCapsuleComponent()->SetCapsuleSize(24,88);
    TestEqual(TEXT("Cross-window frame closes contact state"),Melee->GetActiveWindowCount(),0);
    TestEqual(TEXT("Cross-window frame still fires both coincident bursts"),Melee->GetSpawnedActionEffects(),2);
    Start(1.f);Run(Montage->GetPlayLength()*.92f);
    TestFalse(TEXT("Late combo rejected"),Melee->RequestCombo());
    Player->GetAbilitySystemComponent()->CancelAllAbilities();
    TestFalse(TEXT("Recovery cancel ends task"),Ability->IsActive());
    // Receipt callback may synchronously remove all units during a sweep.
    Start(1.f);Run(.19f);
    Enemy->SetActorLocation(Melee->GetWeapon(1)->GetComponentTransform().TransformPosition(FVector(0,0,55)));
    const auto Handle=Battle->OnReceipt.AddLambda([&](const FPadmaCombatReceipt&){Battle->ExitBattle();});
    Step(.01f);
    TestFalse(TEXT("Reentrant receipt exit is safe"),Battle->IsBattleActive());
    TestFalse(TEXT("Exit clears active montage attack"),Melee->IsAttacking());
    TestEqual(TEXT("No impact spawned after receipt teardown"),Melee->GetSpawnedHitEffects(),0);
    Battle->OnReceipt.Remove(Handle);
    // Killing impacts remain world-owned after ordinary battle settlement removes both units.
    TestTrue(TEXT("Restart after receipt exit"),Battle->StartBattle(Setup,Failure));
    Player=Battle->GetPlayerUnit();Enemy=Battle->GetUnit(TEXT("dummy"));Melee=Player->GetMelee();
    Enemy->GetCapsuleComponent()->SetCapsuleSize(24,88);Run(.05f);Start(1.f);
    Enemy->ApplyAttribute(Player,UPadmaCombatAttributes::GetHealthAttribute(),1.f,true);
    Run(.19f);Enemy->SetActorLocation(Melee->GetWeapon(1)->GetComponentTransform().TransformPosition(FVector(0,0,55)));Run(.01f);
    TestFalse(TEXT("Animated blade kills target"),Enemy->IsAlive());
    TestEqual(TEXT("Lethal contact spawns impact"),Melee->GetSpawnedHitEffects(),1);
    TArray<UNiagaraComponent*> ImpactComponents;World->GetWorldSettings()->GetComponents(ImpactComponents);
    TArray<TWeakObjectPtr<UNiagaraComponent>> Impacts;
    for (auto* C:ImpactComponents) if (IsValid(C) && C->GetAsset()->GetName().Contains(TEXT("common_hit_02"))) Impacts.Add(C);
    TestTrue(TEXT("Lethal impact exists"),!Impacts.IsEmpty());
    static_cast<UActorComponent*>(Battle)->TickComponent(.016f,LEVELTICK_All,nullptr);
    TestFalse(TEXT("Lethal settlement ends battle"),Battle->IsBattleActive());
    for (const auto& C:Impacts) TestTrue(TEXT("Impact survives unit teardown"),C.IsValid());
    // Transient adversarial fixture: shortened notify windows, unchanged animation/Slot/section.
    // Dynamic Montages must be constructed as playable transient Montages, not
    // duplicated as nested asset subobjects. Keep this independent fixture rooted.
    PadmaACTTest::FLegacyMeleeFixture EdgeFixture;
    if (!EdgeFixture.Initialize(*this)) return false;
    auto* EdgeDefinition=EdgeFixture.Character.Get();
    auto* EdgeProfile=EdgeFixture.Profile.Get();
    auto* EdgeMontage=EdgeFixture.Montage.Get();
    TStrongObjectPtr<UPadmaACTCharacterDefinition> KeepEdgeDefinition(EdgeDefinition);
    TStrongObjectPtr<UPadmaACTMeleeDefinition> KeepEdgeProfile(EdgeProfile);
    TStrongObjectPtr<UAnimMontage> KeepEdgeMontage(EdgeMontage);
    EdgeDefinition->MeleeProfile=EdgeProfile;EdgeProfile->AttackMontage=EdgeMontage;
    EdgeMontage->BlendIn.SetBlendTime(0);EdgeMontage->BlendOut.SetBlendTime(0);
    const float Length=EdgeMontage->GetPlayLength();
    bool KeptHit=false;
    for (int32 I=EdgeMontage->Notifies.Num()-1;I>=0;--I)
    {
        auto& Event=EdgeMontage->Notifies[I];
        if (Cast<UAnimNotifyState_PadmaACTHitWindow>(Event.NotifyStateClass) && !KeptHit)
        { Event.SetTime(Length-.06f);Event.SetDuration(.04f);KeptHit=true; }
        else if (Cast<UAnimNotifyState_PadmaACTComboWindow>(Event.NotifyStateClass))
        { Event.SetTime(.05f);Event.SetDuration(.03f); }
        else EdgeMontage->Notifies.RemoveAt(I);
    }
    EdgeMontage->RefreshCacheData();
    Setup.Units[0].Presentation.ACTDefinition=EdgeDefinition;
    TestTrue(TEXT("Edge fixture starts"),Battle->StartBattle(Setup,Failure));
    Player=Battle->GetPlayerUnit();Enemy=Battle->GetUnit(TEXT("dummy"));Melee=Player->GetMelee();
    Ability=Player->GetAbilitySystemComponent()->FindAbilitySpecFromClass(UPadmaACTAbility::StaticClass());
    Run(.05f);Start(4.f);Step(.03f);
    TestTrue(TEXT("Single frame crossed entire combo window"),Melee->GetAttackTime()>.08f);
    TestFalse(TEXT("Queued ANS cannot accept combo after its authored end"),Melee->RequestCombo());
    Start(4.f);Step(.015f);
    TestTrue(TEXT("Inside shortened combo window accepted"),Melee->RequestCombo());
    Start(4.f);Run((Length-.08f)/4.f);
    Enemy->GetCapsuleComponent()->SetCapsuleSize(200,200);Enemy->SetActorLocation(Player->GetActorLocation());
    Step(.04f);
    TestEqual(TEXT("Completion-crossing frame retains contact notification"),Melee->GetConfirmedContacts(),1);
    TestEqual(TEXT("Completion-crossing contact settles once"),Enemy->Health(),96.f);
    TestEqual(TEXT("Completion-crossing impact survives normal completion"),Melee->GetSpawnedHitEffects(),1);
    TestFalse(TEXT("Deferred normal completion ends GAS after contact"),Ability->IsActive());
    TestEqual(TEXT("Completion clears pending windows"),Melee->GetActiveWindowCount(),0);
    Battle->ExitBattle();
    return true;
}
