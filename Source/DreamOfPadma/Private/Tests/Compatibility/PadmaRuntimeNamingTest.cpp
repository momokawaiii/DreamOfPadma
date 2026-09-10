#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS
#include "Game/Save/PadmaRunSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/Class.h"
#include "UObject/CoreRedirects.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

namespace
{
// Write old struct names while UE measures each property tag's payload size.
// This is a synthetic legacy payload, not a claim that the old binary wrote it.
class FLegacyNamesArchive : public FObjectAndNameAsStringProxyArchive
{
public:
	FLegacyNamesArchive(FArchive& Inner, const TMap<FName, FName>& InNames)
		: FObjectAndNameAsStringProxyArchive(Inner, false), Names(InNames) {}
	virtual FArchive& operator<<(FName& Value) override
	{
		FName Name = Names.Contains(Value) ? Names[Value] : Value;
		return FNameAsStringProxyArchive::operator<<(Name);
	}
private:
	const TMap<FName, FName>& Names;
};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaRuntimeNamingTest,
	"Project.Naming.TASK047.RedirectsAndLegacySave",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPadmaRuntimeNamingTest::RunTest(const FString& Parameters)
{
	FString Json;
	if (!TestTrue(TEXT("Versioned naming manifest exists"), FFileHelper::LoadFileToString(Json,
		*FPaths::Combine(FPaths::ProjectDir(), TEXT("Scripts/Migrations/RuntimeNaming047.json"))))) return false;
	TSharedPtr<FJsonObject> Manifest;
	if (!TestTrue(TEXT("Naming manifest parses"), FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json), Manifest))) return false;
	const auto& Redirects = Manifest->GetArrayField(TEXT("redirects"));
	TestEqual(TEXT("Exact reflected redirect coverage"), Redirects.Num(), 39);
	TMap<FName, FName> LegacyNames;
	for (const auto& Entry : Redirects)
	{
		const auto& Row = Entry->AsObject();
		const FString OldPath = Row->GetStringField(TEXT("old"));
		const FString NewPath = Row->GetStringField(TEXT("new"));
		UClass* Kind = Row->GetStringField(TEXT("kind")) == TEXT("Class") ? UClass::StaticClass() : UScriptStruct::StaticClass();
		UObject* Current = StaticLoadObject(Kind, nullptr, *NewPath);
		const auto Flag = Kind == UClass::StaticClass() ? ECoreRedirectFlags::Type_Class : ECoreRedirectFlags::Type_Struct;
		const auto Resolved = FCoreRedirects::GetRedirectedName(Flag, FCoreRedirectObjectName(OldPath));
		TestNotNull(*NewPath, Current);
		TestEqual(*FString::Printf(TEXT("%s redirect"), *OldPath), Resolved.ToString(), NewPath);
		LegacyNames.Add(FCoreRedirectObjectName(NewPath).ObjectName, FCoreRedirectObjectName(OldPath).ObjectName);
	}
	// Optional bytes written by the old binary. Never read or overwrite the user's ordinary save slot.
	if (FParse::Param(FCommandLine::Get(), TEXT("PadmaNamingLegacySave")))
	{
		TArray<uint8> OldBytes;
		if (!TestTrue(TEXT("Original pre-rename bytes exist"), UGameplayStatics::LoadDataFromSlot(OldBytes, TEXT("PadmaNamingCompatibility047"), 0))) return false;
		auto* Save = UPadmaRunSaveGame::LoadFromMemory(OldBytes);
		if (!TestNotNull(TEXT("Pre-rename save resolves to the new native class"), Save)) return false;
		TestEqual(TEXT("Save app marker preserved"), Save->AppId, FString(TEXT("padma-native-demo")));
		TestEqual(TEXT("Save format preserved"), Save->FormatVersion, 1);
		TestEqual(TEXT("Nested run RNG preserved"), Save->Run.RNG, uint32(731));
		TestEqual(TEXT("Nested resources preserved"), Save->Run.Resources.Flow, 0.0);
		TestEqual(TEXT("Nested ACT preparation preserved"), Save->Run.Preparation.ACTHP, 50.0);
		TestEqual(TEXT("Stable character identity preserved"), Save->Run.Preparation.ACTCharacter, FName(TEXT("traveller")));

		// Distinct values and nested containers prevent constructor defaults hiding lost state.
		Save->Run.RNG = 92417;
		Save->Run.Calendar.Day = 7;
		Save->Run.Resources.Flow = 123.5;
		Save->Run.Preparation.ACTHP = 19.25;
		Save->Run.Profile.Add(TEXT("opening_flow"), 80.0);
		auto& Card = Save->Run.Cards.AddDefaulted_GetRef();
		Card.Id = TEXT("legacy-card"); Card.DefId = TEXT("dancer"); Card.Node = TEXT("home"); Card.HP = 17.5;
		auto& Node = Save->Run.Nodes.Add(TEXT("home"));
		Node.Owner = TEXT("player"); Node.bVisited = true;
		Save->Run.SkillsDiscard.Add(TEXT("fish-rain"));
		Save->Run.Mobility.Add(TEXT("dancer")).MinFaith = 37;
		FMemoryReader Header = UGameplayStatics::StripSaveGameHeader(OldBytes);
		TArray<uint8> LegacyPayload;
		LegacyPayload.Append(OldBytes.GetData(), Header.Tell());
		FMemoryWriter Writer(LegacyPayload, true);
		Writer.SetUEVer(Header.UEVer());
		Writer.SetEngineVer(Header.EngineVer());
		Writer.SetCustomVersions(Header.GetCustomVersions());
		Writer.Seek(LegacyPayload.Num());
		FLegacyNamesArchive LegacyArchive(Writer, LegacyNames);
		Save->Serialize(LegacyArchive);
		auto* Loaded = UPadmaRunSaveGame::LoadFromMemory(LegacyPayload);
		if (!TestNotNull(TEXT("Synthetic non-default legacy payload loads"), Loaded)) return false;
		TestTrue(TEXT("All nested legacy run values survive"), FPadmaRunState::StaticStruct()->CompareScriptStruct(&Save->Run, &Loaded->Run, 0));
		TArray<uint8> CurrentBytes;
		UGameplayStatics::SaveGameToMemory(Loaded, CurrentBytes);
		auto* Reloaded = UPadmaRunSaveGame::LoadFromMemory(CurrentBytes);
		TestTrue(TEXT("New envelope roundtrip preserves migrated run"), Reloaded && FPadmaRunState::StaticStruct()->CompareScriptStruct(&Save->Run, &Reloaded->Run, 0));
	}
	return !HasAnyErrors();
}
#endif
