#if WITH_EDITOR
#include "Animation/AnimMontage.h"
#include "Gameplay/ACT/Runtime/PadmaACTAnimNotifies.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "NiagaraSystem.h"
#include "NiagaraEmitter.h"
#include "NiagaraRendererProperties.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionSceneColor.h"
#include "Materials/MaterialExpressionEyeAdaptationInverse.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Engine/Texture.h"
#include "Gameplay/ACT/Runtime/PadmaACTMelee.h"
#include "PadmaACTLegacyFixture.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaChenMaterialTest,"DreamOfPadma.ACT.ChenSavedMaterialBindings",EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaChenMaterialTest::RunTest(const FString& Parameters)
{
    const FString Root=PadmaACTTest::SystemsRoot;
    auto* Character=LoadObject<UPadmaACTCharacterDefinition>(nullptr,PadmaACTTest::CharacterPath);
    auto* Profile=LoadObject<UPadmaACTMeleeDefinition>(nullptr,PadmaACTTest::EquipmentPath);
    auto* Montage=LoadObject<UAnimMontage>(nullptr,PadmaACTTest::MontagePath);
    if (!TestNotNull(TEXT("Persisted live character"),Character)
        || !TestNotNull(TEXT("Persisted live equipment"),Profile)) return false;
    if (!TestNotNull(TEXT("Saved GAS montage"),Montage)) return false;
    TestTrue(TEXT("Live character references canonical equipment"),Character->MeleeProfile.LoadSynchronous()==Profile);
    TestTrue(TEXT("Live equipment references canonical attack montage"),Profile->AttackMontage.LoadSynchronous()==Montage);
    auto* Table=Character->SkillTable.LoadSynchronous();
    if (!TestNotNull(TEXT("Live action table"),Table)
        || !TestTrue(TEXT("Live action table schema"),Table->GetRowStruct()==FPadmaACTSkillRow::StaticStruct())) return false;
    int32 PrimaryBindings=0;
    for (const auto& Pair:Table->GetRowMap())
    {
        const auto* Row=reinterpret_cast<const FPadmaACTSkillRow*>(Pair.Value);
        if (Row->ActivationBindingId!=FName(TEXT("Primary"))) continue;
        ++PrimaryBindings;
        auto* Skill=Row->Definition.LoadSynchronous();
        if (TestNotNull(TEXT("Primary action definition"),Skill))
        {
            TestEqual(TEXT("Primary resolves Chen.Attack01"),Skill->DefinitionId,FName(TEXT("Chen.Attack01")));
            TestTrue(TEXT("Primary action uses the audited live montage"),Skill->Montage.LoadSynchronous()==Montage);
        }
    }
    TestEqual(TEXT("Exactly one live primary binding"),PrimaryBindings,1);
    const TArray<FString> Names={TEXT("NS_chen_attack_01_start"),TEXT("NS_chen_attack_01_start2"),TEXT("NS_chen_attack_01_star_right"),TEXT("NS_fxbat_chen_common_hit_01"),TEXT("NS_fxbat_chen_common_hit_02")};
    TArray<UNiagaraSystem*> Systems;
    for (const auto& Name:Names)
    {
        auto* System=LoadObject<UNiagaraSystem>(nullptr,*(Root+Name));
        if (!TestNotNull(*Name,System)) return false;
        TestEqual(TEXT("Blade-contact package identity, not a same-name action system"),System->GetOutermost()->GetName(),Root+Name);
        Systems.Add(System);
    }
    int32 Starts=0,Stops=0,Windows=0,Combo=0;
    int32 FXStarts[3]={0,0,0},FXStops[3]={0,0,0},HitTimes[2]={0,0};
    for (const auto& Event : Montage->Notifies)
    {
        if (auto* FX=Cast<UAnimNotify_PadmaACTFX>(Event.Notify))
        {
            FX->bStop ? ++Stops : ++Starts;
            TestFalse(TEXT("Source action FX preserve Stationary mounting"),FX->Effect.bFollowMount);
            auto* Bound=FX->Effect.System.LoadSynchronous();
            const int32 Index=Systems.IndexOfByKey(Bound);
            if (!TestTrue(TEXT("AN references an exact retained blade-contact action system"),Index>=0 && Index<3)) continue;
            ++(FX->bStop ? FXStops[Index] : FXStarts[Index]);
            const bool Star=Index==2;
            const float Expected=FX->bStop ? (Star?53.f:50.f)/30.f : (Star?10.f:7.f)/30.f;
            TestTrue(TEXT("Montage AN preserves source action time"),FMath::IsNearlyEqual(Event.GetTime(),Expected,.0001f));
        }
        if (auto* Hit=Cast<UAnimNotifyState_PadmaACTHitWindow>(Event.NotifyStateClass))
        {
            ++Windows;
            // Current source-rule volumes, not the retired legacy two-blade fixture.
            TestTrue(TEXT("Live contact keeps retained hit01 identity"),Hit->Window.HitEffect.LoadSynchronous()==Systems[3]);
            const int32 Index=FMath::IsNearlyEqual(Event.GetTime(),8.f/30.f,.0001f) ? 0 :
                FMath::IsNearlyEqual(Event.GetTime(),11.f/30.f,.0001f) ? 1 : INDEX_NONE;
            if (TestTrue(TEXT("Live source contact starts at frame 8 or 11"),Index!=INDEX_NONE)) ++HitTimes[Index];
            TestTrue(TEXT("Live contact lasts one source frame"),FMath::IsNearlyEqual(Event.GetDuration(),1.f/30.f,.0001f));
            TestEqual(TEXT("Live contact weapon index"),Hit->Window.WeaponIndex,0);
            TestEqual(TEXT("Live contact damage scale"),Hit->Window.DamageMultiplier,.2f);
            TestFalse(TEXT("Live contact is spatial, not primary-target-only"),Hit->Window.bPrimaryTargetOnly);
            if (TestEqual(TEXT("One authored contact volume"),Hit->Window.Shapes.Num(),1))
            {
                const auto& Shape=Hit->Window.Shapes[0];
                TestTrue(TEXT("Source volume size in centimeters"),Shape.Size.Equals(FVector(200,125,200),.001f));
                TestTrue(TEXT("Source volume center in centimeters"),Shape.Center.Equals(FVector(100,0,100),.001f));
                TestEqual(TEXT("Source contact uses box, not sphere"),Shape.Radius,0.f);
                // ApplyChenACTSourceRules maps Unity quaternion (x,y,z,w) to UE (z,x,y,w).
                if (Index!=INDEX_NONE) TestTrue(TEXT("Source contact box rotation"),Shape.Rotation.Quaternion().Equals(
                    FQuat(FVector::ForwardVector,FMath::DegreesToRadians(Index==0 ? 0.0 : 28.454247)),.0001));
            }
        }
        if (Cast<UAnimNotifyState_PadmaACTComboWindow>(Event.NotifyStateClass))
        {
            ++Combo;
            TestTrue(TEXT("Live combo opens at source frame 14"),FMath::IsNearlyEqual(Event.GetTime(),14.f/30.f,.0001f));
            TestTrue(TEXT("Live combo closes at source frame 30"),FMath::IsNearlyEqual(Event.GetDuration(),16.f/30.f,.0001f));
        }
    }
    TestEqual(TEXT("Three source burst ANs"),Starts,3);
    TestEqual(TEXT("Three explicit source stop ANs"),Stops,3);
    TestEqual(TEXT("Two live source contact ANS"),Windows,2);
    for (int32 I=0;I<3;++I)
    {
        TestEqual(*(Names[I]+TEXT(" starts exactly once")),FXStarts[I],1);
        TestEqual(*(Names[I]+TEXT(" stops exactly once")),FXStops[I],1);
    }
    for (int32 Count:HitTimes) TestEqual(TEXT("Each source contact time occurs exactly once"),Count,1);
    TestEqual(TEXT("One combo input ANS"),Combo,1);
    TestTrue(TEXT("Authored Attack01 section"),Montage->GetSectionIndex(TEXT("Attack01"))!=INDEX_NONE);
    const TMap<FString,int32> Expected={{Names[0],3},{Names[1],4},{Names[2],6},{Names[3],13},{Names[4],14}};
    FString Report=TEXT("System\tEmitter\tRenderer\tMaterial\tTextures\n");int32 Renderers=0;
    for (const auto& Name:Names)
    {
        auto* System=LoadObject<UNiagaraSystem>(nullptr,*(Root+Name));
        if (!TestNotNull(*Name,System)) continue;
        TestEqual(*(Name+TEXT(" exact emitter count, no accumulated generations")),System->GetNumEmitters(),Expected[Name]);
        for (const auto& Handle:System->GetEmitterHandles())
        {
            const auto* Data=Handle.GetEmitterData();if (!TestNotNull(TEXT("Persisted emitter"),Data)) continue;
            for (const auto* Renderer:Data->GetRenderers())
            {
                ++Renderers;TArray<UMaterialInterface*> Materials;Renderer->GetUsedMaterials(nullptr,Materials);
                TestTrue(TEXT("Renderer has a material"),!Materials.IsEmpty());
                for (auto* Material:Materials)
                {
                    if (!TestNotNull(TEXT("Bound material"),Material)) continue;
                    TestTrue(TEXT("Uses canonical blade-contact material package, not same-name action variant"),
                        Material->GetPathName().StartsWith(TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/Art/Niagara/Materials/BladeContact/")));
                    TestTrue(TEXT("Uses this system's authored Chen material, not engine fallback"),
                        Material->GetName().StartsWith(TEXT("M_")+Name.RightChop(3)+TEXT("_")));
                    auto* Base=Material->GetMaterial();TestTrue(TEXT("Niagara shader usage saved"),Base->GetUsageByFlag(MATUSAGE_NiagaraSprites) && Base->GetUsageByFlag(MATUSAGE_NiagaraMeshParticles) && Base->GetUsageByFlag(MATUSAGE_NiagaraRibbons));
                    TArray<UTexture*> Textures;Material->GetUsedTextures(Textures);FString TextureNames;
                    for (auto* Texture:Textures) TextureNames+=Texture->GetPathName()+TEXT(";");
                    if (Material->GetName()==TEXT("M_fxbat_chen_common_hit_01_0") || Material->GetName()==TEXT("M_fxbat_chen_common_hit_02_10"))
                    {
                        TestEqual(TEXT("Radial blur uses masked scene compositing"),Base->GetBlendMode(),BLEND_Translucent);
                        TestTrue(TEXT("Radial blur uses original spatial mask, not just an animation LUT"),TextureNames.Contains(TEXT("T_fx_glow_101_D_p622183D86F3FBEEF")));
                        bool HasSceneSample=false;
                        for (const UMaterialExpression* Expression:Base->GetExpressions())
                            if (Expression && Expression->IsA<UMaterialExpressionSceneColor>()) HasSceneSample=true;
                        TestTrue(TEXT("Radial blur samples the scene instead of emitting a white billboard"),HasSceneSample);
                    }
                    const TSet<FString> Halos={TEXT("M_chen_attack_01_start2_3"),TEXT("M_fxbat_chen_common_hit_01_2"),TEXT("M_fxbat_chen_common_hit_01_6"),TEXT("M_fxbat_chen_common_hit_02_3"),TEXT("M_fxbat_chen_common_hit_02_8")};
                    if (Halos.Contains(Material->GetName()))
                    {
                        bool Exposure=false,Gain=false;
                        for (const UMaterialExpression* Expression:Base->GetExpressions())
                        {
                            Exposure |= Expression && Expression->IsA<UMaterialExpressionEyeAdaptationInverse>();
                            if (const auto* Parameter=Cast<UMaterialExpressionScalarParameter>(Expression))
                                Gain |= Parameter->ParameterName==TEXT("SourceHaloGain") && FMath::IsNearlyEqual(Parameter->DefaultValue,.12f);
                        }
                        TestTrue(TEXT("Source IgnorePostExposure has a saved UE exposure bridge"),Exposure);
                        TestTrue(TEXT("Weak halo calibration survives material rebuild"),Gain);
                    }
                    // Untextured source spark materials intentionally have no texture.
                    if (Material->GetName()!=TEXT("M_fxbat_chen_common_hit_01_1") && Material->GetName()!=TEXT("M_fxbat_chen_common_hit_01_10")) TestTrue(TEXT("Original texture is reachable from material"),!Textures.IsEmpty());
                    Report+=Name+TEXT("\t")+Handle.GetName().ToString()+TEXT("\t")+Renderer->GetClass()->GetName()+TEXT("\t")+Material->GetPathName()+TEXT("\t")+TextureNames+TEXT("\n");
                }
            }
        }
    }
    TestEqual(TEXT("35 source renderers and five source particle trails"),Renderers,40);
    const FString Out=FPaths::ProjectDir()/TEXT("Artifacts/ChenQianyu/Attack01");IFileManager::Get().MakeDirectory(*Out,true);
    TestTrue(TEXT("Write persisted renderer binding evidence"),FFileHelper::SaveStringToFile(Report,*(Out/TEXT("material-bindings.tsv"))));
    return true;
}
#endif
