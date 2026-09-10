#include "Game/Content/PadmaContentCatalog.h"
#include "Game/WorldMap/PadmaWorldMapAsset.h"
#include "Game/Presentation/PadmaPresentationCatalog.h"
#include "Gameplay/ACT/Authoring/PadmaACTAuthoring.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimMontage.h"
#include "Engine/Texture2D.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "NiagaraSystem.h"
#include "LevelSequence.h"

namespace
{
template<typename Row>
bool ReadShards(const TArray<TSoftObjectPtr<UDataTable>>& Shards, TMap<FName, Row>& Out, FString& Error)
{
	TSet<const UDataTable*> Seen;
	for (const auto& Reference : Shards)
	{
		const UDataTable* Table = Reference.LoadSynchronous();
		if (!Table || Table->GetRowStruct() != Row::StaticStruct() || Seen.Contains(Table))
		{
			Error = TEXT("Missing, wrong-type or duplicate table shard: ") + Reference.ToSoftObjectPath().ToString();
			return false;
		}
		Seen.Add(Table);
		for (FName Name : Table->GetRowNames())
		{
			const Row* Value = Table->FindRow<Row>(Name, TEXT("PadmaContentCatalog"), false);
			if (!Value || Value->Id.IsNone() || Value->Id != Name || Out.Contains(Value->Id))
			{
				Error = TEXT("Empty/mismatched row ID or duplicate ID across shards: ") + Table->GetPathName() + TEXT(".") + Name.ToString();
				return false;
			}
			Out.Add(Value->Id, *Value);
		}
	}
	return true;
}

template<typename Binding>
const Binding* FindUnique(const TArray<Binding>& Values, FName Id)
{
	if (Id.IsNone()) { return nullptr; }
	const Binding* Found = nullptr;
	for (const Binding& Value : Values)
	{
		if (Value.Id == Id)
		{
			if (Found) { return nullptr; }
			Found = &Value;
		}
	}
	return Found;
}

template<typename Binding>
bool CheckDefinitions(const TArray<Binding>& Bindings, FString& Error)
{
	TSet<FName> Ids;
	for (const auto& Binding : Bindings)
	{
		const auto* Definition = Binding.Definition.LoadSynchronous();
		if (Binding.Id.IsNone() || Ids.Contains(Binding.Id) || !Definition || Definition->DefinitionId != Binding.Id)
		{
			Error = TEXT("Invalid/duplicate definition binding: ") + Binding.Id.ToString();
			return false;
		}
		Ids.Add(Binding.Id);
	}
	return true;
}

template<typename T>
bool Optional(const TSoftObjectPtr<T>& Reference)
{
	return Reference.IsNull() || Reference.LoadSynchronous() != nullptr;
}
}

bool UPadmaContentCatalog::Load(FPadmaContentSnapshot& Out, FString& Error) const
{
	Error.Reset();
	FPadmaContentSnapshot Candidate;
	if (ContentVersion != 1 || Provenance.IsEmpty())
	{
		Error = TEXT("Unsupported or unlabelled UE Demo content version.");
		return false;
	}
	if (!ReadShards(CardTables, Candidate.Cards, Error)) { return false; }
	if (!ReadShards(NodeTables, Candidate.Nodes, Error)) { return false; }
	if (!ReadShards(ProfileTables, Candidate.Profiles, Error)) { return false; }
	if (!ReadShards(EncounterSkillTables, Candidate.EncounterSkills, Error)) { return false; }
	if (!ReadShards(ACTSkillTables, Candidate.ACTSkills, Error)) { return false; }
	if (!ReadShards(ACTCharacterTables, Candidate.ACTCharacters, Error)) { return false; }
	if (!ReadShards(ACTWeaponTables, Candidate.ACTWeapons, Error)) { return false; }
	if (!ReadShards(HomeCharacterTables, Candidate.HomeCharacters, Error)) { return false; }
	if (!ReadShards(HomeOutfitTables, Candidate.HomeOutfits, Error)) { return false; }
	if (!ReadShards(HomeWeaponTables, Candidate.HomeWeapons, Error)) { return false; }
	if (!ReadShards(HomePoseTables, Candidate.HomePoses, Error)) { return false; }
	if (!ReadShards(FPSCharacterTables, Candidate.FPSCharacters, Error)) { return false; }
	if (!ReadShards(FPSWeaponTables, Candidate.FPSWeapons, Error)) { return false; }
	if (!ReadShards(DialogueTables, Candidate.Dialogues, Error)) { return false; }
	TMap<FName, FPadmaWorldEdgeDefinitionRow> EdgeRows;
	if (!ReadShards(EdgeTables, EdgeRows, Error)) { return false; }
	TArray<FName> EdgeOrder;
	EdgeRows.GetKeys(EdgeOrder);
	EdgeOrder.Sort(FNameLexicalLess());
	for (FName Id : EdgeOrder) { Candidate.Edges.Add(EdgeRows.FindChecked(Id)); }
	Candidate.CardOrder = CardOrder;
	Candidate.NodeOrder = NodeOrder;
	Candidate.SkillOrder = SkillOrder;
	Candidate.Eras = Eras;
	Candidate.Phases = Phases;
	// Retain the old authored graph solely to import saves that predate frozen map definitions.
	Candidate.LegacyMapLayout.MapId = TEXT("legacy-html-v1");
	Candidate.LegacyMapLayout.HomeNode = TEXT("home");
	Candidate.LegacyMapLayout.BossNode = TEXT("boss");
	for (FName Id : NodeOrder) if (const auto* Node = Candidate.Nodes.Find(Id)) Candidate.LegacyMapLayout.Nodes.Add(*Node);
	Candidate.LegacyMapLayout.Edges = Candidate.Edges;
	if (!WorldMap.IsNull())
	{
		const auto* Map = WorldMap.LoadSynchronous();
		if (!Map) { Error = TEXT("World map asset is missing."); return false; }
		if (Map->bUseGenerated && !FPadmaWorldMapGenerator::ValidateGenerated(Map->Template, Map->GeneratedLayout, Error)) return false;
		if (!FPadmaWorldMapGenerator::ApplyToContent(Map->GetActiveLayout(), Candidate, Error)) return false;
	}
	if (!Candidate.Validate(Error)) { return false; }

	TSet<FName> ModelIds;
	for (const auto& Binding : Models)
	{
		const UPadmaModelDefinition* Definition = Binding.Definition.LoadSynchronous();
		if (Binding.Id.IsNone() || ModelIds.Contains(Binding.Id) || !Definition
			|| Definition->VisualDefinitionId != Binding.Id || !Optional(Binding.Portrait) || !Optional(Binding.Pose))
		{
			Error = TEXT("Invalid/duplicate model binding or optional art reference: ") + Binding.Id.ToString();
			return false;
		}
		const FPadmaResolvedPresentation Resolved = ResolveModel(Binding.Id);
		if (!Resolved.bVisualReady)
		{
			Error = TEXT("Unresolved model binding: ") + Binding.Id.ToString();
			return false;
		}
		if (!Binding.Pose.IsNull())
		{
			const UAnimSequence* Pose = Binding.Pose.LoadSynchronous();
			if (!Resolved.SkeletalModel || Pose->GetSkeleton() != Resolved.SkeletalModel->GetSkeleton())
			{
				Error = TEXT("Pose requires a matching skeletal model: ") + Binding.Id.ToString();
				return false;
			}
		}
		ModelIds.Add(Binding.Id);
	}
	auto CheckModels = [&ModelIds, &Error](const auto& Rows)
	{
		for (const auto& Pair : Rows)
		{
			if (!Pair.Value.ModelId.IsNone() && !ModelIds.Contains(Pair.Value.ModelId))
			{
				Error = TEXT("Row references unknown ModelId: ") + Pair.Key.ToString();
				return false;
			}
		}
		return true;
	};
	if (!CheckModels(Candidate.Cards) || !CheckModels(Candidate.ACTCharacters) || !CheckModels(Candidate.ACTWeapons)
		|| !CheckModels(Candidate.HomeCharacters) || !CheckModels(Candidate.HomeOutfits)
		|| !CheckModels(Candidate.HomeWeapons) || !CheckModels(Candidate.HomePoses)
		|| !CheckModels(Candidate.FPSCharacters) || !CheckModels(Candidate.FPSWeapons)) { return false; }
	if (!CheckDefinitions(ACTCharacterDefinitions, Error) || !CheckDefinitions(ACTWeaponDefinitions, Error)
		|| !CheckDefinitions(ACTSkillDefinitions, Error)) { return false; }
	for (const auto& Pair : Candidate.ACTCharacters)
	{
		const auto* Definition = ResolveACTCharacter(Pair.Value.DefinitionId);
		if (!Definition)
		{
			Error = TEXT("ACT character references unknown definition: ") + Pair.Key.ToString();
			return false;
		}
		// Draft model/animation are allowed: the explicit ModelId supplies a visible placeholder.
		const UDataTable* Skills = Definition->SkillTable.LoadSynchronous();
		if (!Skills || Skills->GetRowStruct() != FPadmaACTSkillRow::StaticStruct() || Skills->GetRowMap().IsEmpty())
		{
			Error = TEXT("ACT character requires a typed, nonempty authoring skill table: ") + Pair.Key.ToString();
			return false;
		}
		TSet<FName> Bindings;
		for (FName RowName : Skills->GetRowNames())
		{
			const auto* Row = Skills->FindRow<FPadmaACTSkillRow>(RowName, TEXT("DemoACTBindings"), false);
			const auto* Skill = Row ? Row->Definition.LoadSynchronous() : nullptr;
			if (!Row || !Skill || Row->SkillId != Skill->DefinitionId || !Candidate.ACTSkills.Contains(Row->SkillId)
				|| Row->ActivationBindingId.IsNone() || Bindings.Contains(Row->ActivationBindingId))
			{
				Error = TEXT("Invalid ACT authoring skill/binding reference: ") + RowName.ToString();
				return false;
			}
			Bindings.Add(Row->ActivationBindingId);
		}
	}
	for (const auto& Pair : Candidate.ACTWeapons)
	{
		if (!ResolveACTWeapon(Pair.Value.DefinitionId))
		{
			Error = TEXT("ACT weapon references unknown definition: ") + Pair.Key.ToString();
			return false;
		}
	}
	for (const auto& Pair : Candidate.ACTSkills)
	{
		if (!ResolveACTSkill(Pair.Value.DefinitionId))
		{
			Error = TEXT("ACT skill references unknown definition: ") + Pair.Key.ToString();
			return false;
		}
	}
	TSet<FName> EffectIds;
	for (const auto& Binding : Effects)
	{
		if (Binding.Id.IsNone() || EffectIds.Contains(Binding.Id) || !Optional(Binding.Icon)
			|| !Optional(Binding.Niagara) || !Optional(Binding.LevelSequence) || !Optional(Binding.Montage))
		{
			Error = TEXT("Invalid/duplicate optional effect binding: ") + Binding.Id.ToString();
			return false;
		}
		EffectIds.Add(Binding.Id);
	}
	TSet<FName> BoundNodes;
	for (const auto& Binding : NodePresentation)
	{
		if (!Candidate.Nodes.Contains(Binding.NodeId) || BoundNodes.Contains(Binding.NodeId)
			|| (!Binding.GroundModelId.IsNone() && !ModelIds.Contains(Binding.GroundModelId))
			|| (!Binding.BuildingModelId.IsNone() && !ModelIds.Contains(Binding.BuildingModelId))
			|| (!Binding.EffectId.IsNone() && !EffectIds.Contains(Binding.EffectId)) || !Optional(Binding.Scene))
		{
			Error = TEXT("Invalid node presentation reference: ") + Binding.NodeId.ToString();
			return false;
		}
		BoundNodes.Add(Binding.NodeId);
	}
	Out = MoveTemp(Candidate);
	return true;
}

bool UPadmaContentCatalog::ValidateCatalog(FString& Error) const
{
	FPadmaContentSnapshot Content;
	return Load(Content, Error);
}

const FPadmaModelBinding* UPadmaContentCatalog::FindModelBinding(FName Id) const { return FindUnique(Models, Id); }
const FPadmaEffectBinding* UPadmaContentCatalog::FindEffectBinding(FName Id) const { return FindUnique(Effects, Id); }

const FPadmaNodePresentationBinding* UPadmaContentCatalog::FindNodePresentation(FName Id) const
{
	const FPadmaNodePresentationBinding* Found = nullptr;
	for (const auto& Value : NodePresentation)
	{
		if (Value.NodeId == Id)
		{
			if (Found) { return nullptr; }
			Found = &Value;
		}
	}
	return Found;
}

FPadmaResolvedPresentation UPadmaContentCatalog::ResolveModel(FName Id) const
{
	const auto* Binding = FindModelBinding(Id);
	if (!Binding)
	{
		FPadmaResolvedPresentation Result;
		Result.DefinitionId = Id;
		Result.VisualErrors.Add(FText::FromString(TEXT("Missing/duplicate playable model binding.")));
		return Result;
	}
	// Reuse the established asset/skeleton validation and presentation projection.
	UPadmaPresentationCatalog* Adapter = NewObject<UPadmaPresentationCatalog>();
	FPadmaPresentationCatalogEntry Entry;
	Entry.Key.Domain = EPadmaPresentationSource::Model;
	Entry.Key.DefinitionId = Id;
	Entry.Model = Binding->Definition;
	Adapter->Entries.Add(Entry);
	return Adapter->Resolve(Entry.Key, NAME_None);
}

UPadmaACTCharacterDefinition* UPadmaContentCatalog::ResolveACTCharacter(FName Id) const
{
	const auto* Binding = FindUnique(ACTCharacterDefinitions, Id);
	return Binding ? Binding->Definition.LoadSynchronous() : nullptr;
}
UPadmaACTWeaponDefinition* UPadmaContentCatalog::ResolveACTWeapon(FName Id) const
{
	const auto* Binding = FindUnique(ACTWeaponDefinitions, Id);
	return Binding ? Binding->Definition.LoadSynchronous() : nullptr;
}
UPadmaACTSkillDefinition* UPadmaContentCatalog::ResolveACTSkill(FName Id) const
{
	const auto* Binding = FindUnique(ACTSkillDefinitions, Id);
	return Binding ? Binding->Definition.LoadSynchronous() : nullptr;
}
