#include "Misc/AutomationTest.h"
#include "Game/Content/PadmaContentCatalog.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include <type_traits>

#if WITH_DEV_AUTOMATION_TESTS
namespace
{
template<typename Row>
UDataTable* MakeDemoTable(FName Name, const Row& Value)
{
	UDataTable* Table = NewObject<UDataTable>();
	Table->RowStruct = Row::StaticStruct();
	Table->AddRow(Name, Value);
	return Table;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaDemoCatalogShardValidation,
	"Project.MVP.TASK046.Catalog.TypedShardsAndAtomicPublish",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPadmaDemoCatalogShardValidation::RunTest(const FString& Parameters)
{
	UPadmaContentCatalog* Catalog = NewObject<UPadmaContentCatalog>();
	FPadmaContentSnapshot Published;
	FPadmaCardDefinitionRow Sentinel;
	Sentinel.Id = TEXT("retained");
	Published.Cards.Add(Sentinel.Id, Sentinel);
	Published.CardOrder.Add(Sentinel.Id);
	FString Error;

	FPadmaCardDefinitionRow Card;
	Card.Id = TEXT("same");
	Catalog->CardTables = {
		MakeDemoTable(Card.Id, Card),
		MakeDemoTable(Card.Id, Card)
	};
	TestFalse(TEXT("Duplicate IDs across different shards fail"), Catalog->Load(Published, Error));
	TestTrue(TEXT("Duplicate error is actionable"), Error.Contains(TEXT("duplicate ID across shards")));
	TestTrue(TEXT("Failed import keeps existing snapshot"), Published.Cards.Contains(TEXT("retained")));
	TestEqual(TEXT("Failed import publishes no partial cards"), Published.Cards.Num(), 1);

	Card.Id = TEXT("stable");
	Catalog->CardTables = { MakeDemoTable(FName(TEXT("different-row-name")), Card) };
	TestFalse(TEXT("Row name must agree with stable ID"), Catalog->Load(Published, Error));
	TestTrue(TEXT("Row ID failure retains snapshot order"), Published.CardOrder[0] == FName(TEXT("retained")));

	Catalog->CardTables.Reset();
	FPadmaEncounterSkillEffectRow EncounterSkill;
	EncounterSkill.Id = TEXT("cut");
	Catalog->ACTSkillTables = { MakeDemoTable(EncounterSkill.Id, EncounterSkill) };
	TestFalse(TEXT("ACT rejects Encounter table despite common neutral fields"), Catalog->Load(Published, Error));
	TestTrue(TEXT("Wrong mode table reports type failure"), Error.Contains(TEXT("wrong-type")));

	Catalog->ACTSkillTables.Reset();
	FPadmaACTSkillEffectRow ACTSkill;
	ACTSkill.Id = TEXT("cut");
	Catalog->EncounterSkillTables = { MakeDemoTable(ACTSkill.Id, ACTSkill) };
	TestFalse(TEXT("Encounter rejects ACT table"), Catalog->Load(Published, Error));
	TestTrue(TEXT("All failed loads retain prior snapshot"), Published.Cards.Contains(TEXT("retained")));

	FPadmaModelBinding A;
	A.Id = TEXT("duplicate");
	Catalog->Models = {A, A};
	TestNull(TEXT("Ambiguous model art hook is not silently selected"), Catalog->FindModelBinding(A.Id));
	TestNull(TEXT("Missing model art hook has no first-entry fallback"), Catalog->FindModelBinding(TEXT("missing")));
	return true;
}

/** Shared by native rule tests: fixture JSON is exported from data.js, never a second hard-coded catalog. */
bool LoadPadmaDemoTestContent(FPadmaContentSnapshot& Out, FString& Error)
{
	FString Json;
	const FString Path = FPaths::Combine(FPaths::ProjectDir(), TEXT("Artifacts/TASK-046/PlayableData.json"));
	TSharedPtr<FJsonObject> Root;
	if (!FFileHelper::LoadFileToString(Json, *Path)
		|| !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json), Root) || !Root.IsValid())
	{
		Error = TEXT("Run node Scripts/Editor/ExportPlayableData.cjs before native fixture tests: ") + Path;
		return false;
	}
	const TSharedPtr<FJsonObject>* TableObject;
	const TSharedPtr<FJsonObject>* OrderObject;
	if (!Root->TryGetObjectField(TEXT("tables"), TableObject) || !Root->TryGetObjectField(TEXT("order"), OrderObject))
	{
		Error = TEXT("Malformed fixture export.");
		return false;
	}
	FPadmaContentSnapshot Candidate;
	auto Import = [&Error, TableObject](const TCHAR* Name, auto& Destination)
	{
		using RowType = std::remove_reference_t<decltype(Destination.FindChecked(FName()))>;
		const TSharedPtr<FJsonObject>* Spec;
		const TArray<TSharedPtr<FJsonValue>>* Rows;
		if (!(*TableObject)->TryGetObjectField(Name, Spec) || !(*Spec)->TryGetArrayField(TEXT("rows"), Rows))
		{
			Error = TEXT("Missing fixture table: ") + FString(Name);
			return false;
		}
		FString TableJson;
		FJsonSerializer::Serialize(*Rows, TJsonWriterFactory<>::Create(&TableJson));
		UDataTable* Table = NewObject<UDataTable>();
		Table->RowStruct = RowType::StaticStruct();
		const TArray<FString> Problems = Table->CreateTableFromJSONString(TableJson);
		if (!Problems.IsEmpty())
		{
			Error = FString(Name) + TEXT(": ") + FString::Join(Problems, TEXT("; "));
			return false;
		}
		for (FName Id : Table->GetRowNames())
		{
			const RowType* Row = Table->FindRow<RowType>(Id, TEXT("DemoTestContent"), false);
			if (!Row || Row->Id != Id || Destination.Contains(Id))
			{
				Error = TEXT("Malformed/duplicate fixture row: ") + Id.ToString();
				return false;
			}
			Destination.Add(Id, *Row);
		}
		return true;
	};
	if (!Import(TEXT("Cards_A"), Candidate.Cards) || !Import(TEXT("Cards_B"), Candidate.Cards)
		|| !Import(TEXT("Cards_C"), Candidate.Cards) || !Import(TEXT("Nodes"), Candidate.Nodes)
		|| !Import(TEXT("Profiles"), Candidate.Profiles) || !Import(TEXT("EncounterSkills"), Candidate.EncounterSkills)
		|| !Import(TEXT("ACTSkills"), Candidate.ACTSkills) || !Import(TEXT("ACTCharacters"), Candidate.ACTCharacters)
		|| !Import(TEXT("ACTWeapons"), Candidate.ACTWeapons) || !Import(TEXT("HomeCharacters"), Candidate.HomeCharacters)
		|| !Import(TEXT("HomeOutfits"), Candidate.HomeOutfits) || !Import(TEXT("HomeWeapons"), Candidate.HomeWeapons)
		|| !Import(TEXT("HomePoses"), Candidate.HomePoses) || !Import(TEXT("FPSCharacters"), Candidate.FPSCharacters)
		|| !Import(TEXT("FPSWeapons"), Candidate.FPSWeapons) || !Import(TEXT("Dialogues"), Candidate.Dialogues)) { return false; }
	TMap<FName, FPadmaWorldEdgeDefinitionRow> Edges;
	if (!Import(TEXT("Edges"), Edges)) { return false; }
	TArray<FName> EdgeOrder;
	Edges.GetKeys(EdgeOrder);
	EdgeOrder.Sort(FNameLexicalLess());
	for (FName Id : EdgeOrder) { Candidate.Edges.Add(Edges.FindChecked(Id)); }
	auto ReadOrder = [&Error, OrderObject](const TCHAR* Name, TArray<FName>& Values)
	{
		const TArray<TSharedPtr<FJsonValue>>* Items;
		if (!(*OrderObject)->TryGetArrayField(Name, Items)) { Error = TEXT("Missing fixture order."); return false; }
		for (const auto& Item : *Items) { Values.Add(FName(*Item->AsString())); }
		return true;
	};
	if (!ReadOrder(TEXT("cards"), Candidate.CardOrder) || !ReadOrder(TEXT("nodes"), Candidate.NodeOrder)
		|| !ReadOrder(TEXT("skills"), Candidate.SkillOrder) || !ReadOrder(TEXT("eras"), Candidate.Eras)
		|| !ReadOrder(TEXT("phases"), Candidate.Phases) || !Candidate.Validate(Error)) { return false; }
	Out = MoveTemp(Candidate);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaDemoExportedFixture,
	"Project.MVP.TASK046.Catalog.ExportedHTMLFixture",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FPadmaDemoExportedFixture::RunTest(const FString& Parameters)
{
	FPadmaContentSnapshot Content;
	FString Error;
	if (!TestTrue(TEXT("Exported HTML fixture loads"), LoadPadmaDemoTestContent(Content, Error)))
	{
		AddError(Error);
		return false;
	}
	TestEqual(TEXT("Source ABC row count"), Content.Cards.Num(), 9);
	TestEqual(TEXT("Source profile count"), Content.Profiles.Num(), 38);
	TestEqual(TEXT("Source dialogue line count"), Content.Dialogues.Num(), 6);
	TestTrue(TEXT("Explicit deterministic first card"), Content.CardOrder[0] == FName(TEXT("dawn")));
	TestTrue(TEXT("Door starts immovable"), Content.Cards.FindChecked(TEXT("door")).Mobility.Policy == EPadmaCardMobility::Immovable);
	TestTrue(TEXT("Reed starts conditionally movable"), Content.Cards.FindChecked(TEXT("reed")).Mobility.Policy == EPadmaCardMobility::Conditional);
	TestFalse(TEXT("ACT heal does not inherit Encounter faith condition"), Content.ACTSkills.FindChecked(TEXT("light")).bFaithIfUnacted);
	TestTrue(TEXT("Encounter heal keeps its faith condition"), Content.EncounterSkills.FindChecked(TEXT("light")).bFaithIfUnacted);
	return true;
}

#endif
