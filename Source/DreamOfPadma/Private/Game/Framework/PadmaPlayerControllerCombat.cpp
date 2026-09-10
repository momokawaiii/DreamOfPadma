#include "Game/Framework/PadmaPlayerController.h"
#include "World/Combat/PadmaCombatFeedback.h"
#include "Game/Content/PadmaContentCatalog.h"
#include "Game/Run/PadmaRunSubsystem.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Gameplay/ACT/Authoring/PadmaACTAuthoring.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Camera/CameraActor.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "TimerManager.h"

void APadmaPlayerController::StartPendingBattle()
{
	const auto& D=Rules().Content();const auto& R=Rules().Run();const auto& Request=Rules().PendingBattle();
	const auto* Node=D.Nodes.Find(Request.Node);if(!Node){ApplyResult(Rules().FinishBattle(false));Travel(false);return;}
	FPadmaCombatSetup Setup;Setup.Mode=Request.Mode==TEXT("act")?EPadmaCombatMode::ACT:EPadmaCombatMode::Encounter;
	Setup.Era=R.Calendar.Era;Setup.EncounterSkills=D.EncounterSkills;Setup.ACTSkills=D.ACTSkills;
	Setup.EnemyAttackInterval=Rules().P(TEXT("enemyInterval"));
	if(Setup.Mode==EPadmaCombatMode::Encounter)
	{
		int32 i=0;
		for(FName Id:Request.Ids)
		{
			const auto* C=Rules().Card(Id);const auto* Def=C?D.Cards.Find(C->DefId):nullptr;
			if(!C||!Def||Def->Family!=TEXT("C"))continue;
			FPadmaCombatUnitSpec U;U.Id=Id;U.DefinitionId=C->DefId;U.DisplayName=Def->DisplayName;U.bPlayer=true;
			U.Family=Def->Family;U.Attribute=Def->Attribute;U.Health=C->HP;U.MaxHealth=Def->Health;U.Attack=C->Attack;U.Defense=Def->Defense;
			U.AttackCost=Def->AttackCost;U.MaxTargets=Def->MaxTargets;U.Speed=Rules().P(TEXT("playerSpeed"));U.Location=FVector(160,140+150*i++,90);
			if(const auto* B=Catalog->FindModelBinding(Def->ModelId))if(auto* M=B->Definition.LoadSynchronous()){U.Presentation.Model=M->SkeletalModel;U.Presentation.StaticModel=M->StaticModel;U.Presentation.AnimationClass=M->AnimationClass;}
			if(const auto* FX=Catalog->FindEffectBinding(Def->Id))U.Presentation.AttackMontage=FX->Montage;
			Setup.Units.Add(U);
		}
	}
	else
	{
		const auto* CharacterRow=D.ACTCharacters.Find(R.Preparation.ACTCharacter);const auto* Weapon=D.ACTWeapons.Find(R.Preparation.ACTWeapon);
		if(!CharacterRow||!Weapon){ApplyResult(Rules().FinishBattle(false));Travel(false);return;}
		FPadmaCombatUnitSpec U;U.Id=FName(*(TEXT("act-")+CharacterRow->Id.ToString()));U.DefinitionId=CharacterRow->Id;U.DisplayName=CharacterRow->DisplayName;U.Family=TEXT("ACT");U.bPlayer=true;
		U.Health=R.Preparation.ACTHP;U.MaxHealth=Rules().P(TEXT("actHP"));U.Attack=Rules().P(TEXT("actAtk"))*Weapon->DamageMultiplier;U.Defense=Rules().P(TEXT("actDef"));
		U.AttackRange=Rules().P(TEXT("actRange"))*Weapon->RangeMultiplier;U.AttackInterval=Rules().P(TEXT("actCooldown"))*Weapon->IntervalMultiplier;
		U.Speed=Rules().P(TEXT("actSpeed"));if(Node->Terrain==CharacterRow->SpeedBonusTerrain&&Node->Terrain!=CharacterRow->TraitDisabledTerrain)U.Speed*=CharacterRow->TerrainSpeedMultiplier;
		U.Location=FVector(160,245,90);
		if(auto* Definition=Catalog->ResolveACTCharacter(CharacterRow->Id)){U.Presentation.Model=Definition->Model;U.Presentation.AnimationClass=Definition->AnimationClass;U.Presentation.ACTDefinition=Definition;}
		if(U.Presentation.Model.IsNull())if(const auto* B=Catalog->FindModelBinding(CharacterRow->ModelId))if(auto* M=B->Definition.LoadSynchronous()){U.Presentation.Model=M->SkeletalModel;U.Presentation.StaticModel=M->StaticModel;U.Presentation.AnimationClass=M->AnimationClass;}
		Setup.Units.Add(U);
	}
	for(int32 i=0;i<Node->EnemyCount;++i)
	{
		FPadmaCombatUnitSpec U;U.Id=FName(*FString::Printf(TEXT("enemy-%d"),i));U.DefinitionId=TEXT("guard");U.DisplayName=FText::FromString(FString::Printf(TEXT("%s %d"),Request.Node==Rules().BossNode()?TEXT("王庭守卫"):TEXT("关隘守卫"),i+1));U.bPlayer=false;U.Attribute=TEXT("R");
		U.Health=U.MaxHealth=Rules().P(Request.Node==Rules().BossNode()?TEXT("bossHP"):TEXT("enemyHP"));U.Attack=Rules().P(TEXT("enemyAtk"));U.Defense=Rules().P(TEXT("enemyDef"));U.Speed=Rules().P(TEXT("enemySpeed"))+i;U.Location=FVector(570+i*110,170+i*140,90);U.InitialAttackDelay=.8+i*.5;
		if(const auto* B=Catalog->FindModelBinding(TEXT("guard")))if(auto* M=B->Definition.LoadSynchronous()){U.Presentation.Model=M->SkeletalModel;U.Presentation.StaticModel=M->StaticModel;U.Presentation.AnimationClass=M->AnimationClass;}
		Setup.Units.Add(U);
	}
	Combat=NewObject<UPadmaCombatComponent>(this);Combat->RegisterComponent();
	Combat->PayCost=[this](float Flow,float Calc,FName Skill,FString& Failure)
	{
		const auto Result=Skill.IsNone()?Rules().TryPay(Flow,Calc):Rules().ConsumeBasicSkill(Skill);
		if(!Result.bOk)Failure=Result.Text;return Result.bOk;
	};
	Combat->ModifyFaith=[this](float BattlePlayer,float Ruler){Rules().ChangeFaith(BattlePlayer,Ruler);};
	Combat->OnFinished=[this](bool Won,const TArray<FPadmaCombatUnitSnapshot>& Units){OnBattleFinished(Won,Units);};
	Combat->OnReceipt.AddUObject(this,&APadmaPlayerController::PlayReceipt);
	Combat->OnChanged.AddWeakLambda(this,[this]{bNeedsRefresh=true;});
	FString Error;
	if(!Combat->StartBattle(Setup,Error))
	{
		Notice=Error;ApplyResult(Rules().FinishBattle(false));bBattleFinished=true;ModalTitle=TEXT("战斗启动失败 · 已回滚");ModalText=Error;
	}
	SetCamera(FVector(400,245,95),1650,-42,-90);
	if(Setup.Mode==EPadmaCombatMode::ACT)
	{
		if(auto* BattlePlayer=Combat->GetPlayerUnit())
		if(auto* Weapon=Catalog->ResolveACTWeapon(R.Preparation.ACTWeapon))
		{
			FTransform WeaponTransform=FTransform::Identity;
			if(const auto* Row=D.ACTWeapons.Find(R.Preparation.ACTWeapon))
				if(const auto* Binding=Catalog->FindModelBinding(Row->ModelId))
					if(auto* Model=Binding->Definition.LoadSynchronous())WeaponTransform=Model->ModelTransform;
			if(auto* Mesh=Weapon->StaticModel.LoadSynchronous())
			{
				auto* C=NewObject<UStaticMeshComponent>(BattlePlayer);C->SetStaticMesh(Mesh);C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				C->SetupAttachment(BattlePlayer->GetMesh(),Weapon->AttachmentSocket);C->RegisterComponent();C->SetRelativeTransform(WeaponTransform);
			}
			if(auto* Mesh=Weapon->SkeletalModel.LoadSynchronous())
			{
				auto* C=NewObject<USkeletalMeshComponent>(BattlePlayer);C->SetSkeletalMesh(Mesh);C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				C->SetupAttachment(BattlePlayer->GetMesh(),Weapon->AttachmentSocket);C->RegisterComponent();C->SetRelativeTransform(WeaponTransform);
			}
		}
	}
}
void APadmaPlayerController::OnBattleFinished(bool Won,const TArray<FPadmaCombatUnitSnapshot>& Units)
{
	TMap<FName,double> Health;double ACTHP=-1;
	for(const auto& U:Units)if(U.bPlayer){if(Combat->GetMode()==EPadmaCombatMode::ACT)ACTHP=U.Health;else Health.Add(U.Id,U.Health);}
	const auto Settlement=Rules().FinishBattle(Won,Health,ACTHP,Combat->GetActionCount(),Combat->GetElapsed());
	ApplyResult(Settlement);
	if(!Settlement.bOk){Rules().FinishBattle(false);Won=false;}
	RestoreOverlay({});OverlayHistory.Reset();UpdateInspectionGate();
	FinishedUnits=Units;bBattleFinished=true;CancelTargeting();UnitLabels.Reset();
	ModalTitle=Won?TEXT("局部战斗胜利"):TEXT("战斗结束 · 已回滚");
	ModalText=Won?TEXT("战果已提交，移动组进入目标节点。"):TEXT("已恢复战前的资源、卡牌、位置、随机流和阵容，包含移动费用。");
	for(const auto& U:Units)ModalText+=FString::Printf(TEXT("\n%s：%.0f / %.0f"),*U.DisplayName.ToString(),U.Health,U.MaxHealth);
	bNeedsRefresh=true;
}
void APadmaPlayerController::SelectBattleTarget(FName Id)
{
	if(!Combat||!Combat->IsBattleActive())return;
	if(bAttackArmed)
	{
		auto* Target=Combat->GetUnit(Id);
		if(!Target||Target->Spec.bPlayer||!Target->IsAlive()){Notice=TEXT("请选择存活敌方目标。");bNeedsRefresh=true;return;}
		AttackTargets.AddUnique(Id);
		if(AttackTargets.Num()>=AttackTargetCount)
		{
			FString Error;if(!Combat->Attack(AttackTargets,Error))Notice=Error;else Notice=TEXT("普攻已释放");CancelTargeting();
		}
	}
	else if(!ArmedSkill.IsNone())
	{
		const FPadmaSkillEffectRowBase* Skill=Combat->GetMode()==EPadmaCombatMode::Encounter?static_cast<const FPadmaSkillEffectRowBase*>(Rules().Content().EncounterSkills.Find(ArmedSkill)):static_cast<const FPadmaSkillEffectRowBase*>(Rules().Content().ACTSkills.Find(ArmedSkill));
		auto* Target=Combat->GetUnit(Id);
		if(!Skill)return;
		if(Skill->Target==TEXT("all")){SkillTarget=TEXT("arena");Notice=TEXT("已选择全场，点击确认释放；尚未扣费。");}
		else if(Target&&Target->IsAlive()&&!Target->Spec.bCore&&(Skill->Target==TEXT("ally")?Target->Spec.bPlayer:!Target->Spec.bPlayer))
		{SkillTarget=Id;Notice=TEXT("目标已选，点击确认释放；尚未扣费。");}
		else{SkillTarget=NAME_None;Notice=TEXT("目标不合法，未扣费。");}
	}
	bNeedsRefresh=true;
}
void APadmaPlayerController::PlayReceipt(const FPadmaCombatReceipt& R)
{
	LastFeedbackTime=FPlatformTime::Seconds();
	Notice=FString::Printf(TEXT("%s → %s：%s%.1f%s"),R.SourceKind==EPadmaCombatSource::Unit?*R.SourceId.ToString():TEXT("卡牌／场景"),*R.After.DisplayName.ToString(),R.bTrueDamage?TEXT("真伤 "):TEXT(""),R.Amount,R.bBlocked?TEXT("（抗衡）"):TEXT(""));
	const FName EffectId=R.SourceKind==EPadmaCombatSource::Environment?FName(TEXT("rain")):!ArmedSkill.IsNone()?ArmedSkill:FName(TEXT("attack"));
	const auto* Binding=Catalog->FindEffectBinding(EffectId);
	if(Binding)
	{
		if(auto* FX=Binding->Niagara.LoadSynchronous())UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,FX,R.After.Location);
		if(auto* Sequence=Binding->LevelSequence.LoadSynchronous())
		{
			ALevelSequenceActor* Actor=nullptr;FMovieSceneSequencePlaybackSettings Settings;Settings.bAutoPlay=false;
			if(auto* BattlePlayer=ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(),Sequence,Settings,Actor)){BattlePlayer->Play();if(Actor)Actor->SetLifeSpan(10);}
		}
	}
	// Always-readable fallback trail/rain, independent of imported effects or montage callbacks.
	FVector Start=R.After.Location+FVector(0,0,250+R.Wave*35);
	if(R.SourceKind==EPadmaCombatSource::Unit)
		if(auto* Source=Combat->GetUnit(R.SourceId))Start=Source->GetActorLocation()+FVector(0,0,50);
	const FVector End=R.After.Location+FVector(0,0,50);
	if(auto* FX=GetWorld()->SpawnActor<APadmaCombatFeedback>())FX->Play(Start,End,
		FString::Printf(TEXT("%s%.1f"),R.bBlocked?TEXT("BLOCK "):R.Effect==TEXT("damage")?TEXT("-"):TEXT("+"),R.Amount),
		R.bTrueDamage?FLinearColor::Yellow:FLinearColor(.8f,1.f,.85f),R.Wave*.13f,ViewCamera->GetActorLocation());
	bNeedsRefresh=true;
}
