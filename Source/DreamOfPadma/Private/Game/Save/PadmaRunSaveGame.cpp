#include "Game/Save/PadmaRunSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

UPadmaRunSaveGame* UPadmaRunSaveGame::LoadFromMemory(const TArray<uint8>& Data)
{
    // The project started with the version-3 GVAS envelope. Let UE parse its header;
    // LoadGameFromMemory itself does not redirect the class name stored as a string.
    if (Data.Num() < 8) return nullptr;
    FMemoryReader Prefix(Data, true);
    uint32 Magic = 0;
    int32 Version = 0;
    Prefix << Magic << Version;
    if (Magic != 0x53415647 || Version != 3) return nullptr;

    FMemoryReader Reader = UGameplayStatics::StripSaveGameHeader(Data);
    if (Reader.IsError() || Reader.Tell() >= Data.Num()) return nullptr;

    // The class path is the final FString in the engine header. Only accept our
    // two exact envelopes; never instantiate a class chosen by arbitrary bytes.
    const auto HasClass = [&Data, &Reader](FString Path)
    {
        TArray<uint8> Encoded;
        FMemoryWriter Writer(Encoded, true);
        Writer << Path;
        const int64 Start = Reader.Tell() - Encoded.Num();
        return Start >= 0 && FMemory::Memcmp(Data.GetData() + Start, Encoded.GetData(), Encoded.Num()) == 0;
    };
    if (!HasClass(StaticClass()->GetPathName()) && !HasClass(TEXT("/Script/DreamOfPadma.PadmaDemoSaveGame"))) return nullptr;

    auto* Save = NewObject<UPadmaRunSaveGame>();
    FObjectAndNameAsStringProxyArchive Archive(Reader, true);
    Save->Serialize(Archive);
    return Reader.IsError() ? nullptr : Save;
}
