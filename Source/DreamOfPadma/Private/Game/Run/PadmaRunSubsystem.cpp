#include "Game/Run/PadmaRunSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UPadmaRunSubsystem::Initialize(const FPadmaContentSnapshot& Content, const FPadmaMapLayout* AuthoredReference)
{
    if (!RunRules.HasRun()) RunRules.Initialize(Content, AuthoredReference);
}

FPadmaRunCommandResult UPadmaRunSubsystem::StartNew(const TMap<FName, double>& Overrides)
{
    return RunRules.Start(Overrides);
}

FPadmaRunCommandResult UPadmaRunSubsystem::StartNewMap(const FPadmaContentSnapshot& Content, const FPadmaMapLayout* AuthoredReference)
{
    if (RunRules.IsBattle() || RunRules.IsDialogue()) return {false, TEXT("请先结束战斗或对话"), EPadmaRunCommandEffect::None};
    FPadmaRunRules Candidate;
    Candidate.Initialize(Content, AuthoredReference);
    auto Result = Candidate.Start();
    if (Result.bOk) RunRules = MoveTemp(Candidate);
    return Result;
}

FPadmaRunCommandResult UPadmaRunSubsystem::SaveRun(const FString& Slot)
{
    auto Check = RunRules.CheckSaveBoundary();
    if (!Check.bOk) return Check;
    Check = RunRules.ValidateState(RunRules.Run());
    if (!Check.bOk) return Check;
    if (Slot.IsEmpty()) return {false, TEXT("存档槽位无效"), EPadmaRunCommandEffect::None};
    UPadmaRunSaveGame* Save = Cast<UPadmaRunSaveGame>(UGameplayStatics::CreateSaveGameObject(UPadmaRunSaveGame::StaticClass()));
    if (!Save) return {false, TEXT("无法创建存档"), EPadmaRunCommandEffect::None};
    Save->Run = RunRules.Run();
    const bool bSaved = UGameplayStatics::SaveGameToSlot(Save, Slot, 0);
    return {bSaved, bSaved ? TEXT("已在沙盘安全边界保存本局") : TEXT("存档写入失败"), EPadmaRunCommandEffect::None};
}

FPadmaRunCommandResult UPadmaRunSubsystem::LoadRun(const FString& Slot)
{
    if (RunRules.IsBattle() || RunRules.IsDialogue()) return {false, TEXT("请先结束战斗或对话，再读取存档"), EPadmaRunCommandEffect::None};
    if (Slot.IsEmpty()) return {false, TEXT("存档槽位无效"), EPadmaRunCommandEffect::None};
    TArray<uint8> Data;
    if (!UGameplayStatics::LoadDataFromSlot(Data, Slot, 0)) return LoadValue(nullptr);
    return LoadValue(UPadmaRunSaveGame::LoadFromMemory(Data));
}

FPadmaRunCommandResult UPadmaRunSubsystem::LoadValue(const UPadmaRunSaveGame* Save)
{
    if (!Save || Save->AppId != TEXT("padma-native-demo") || Save->FormatVersion != 1)
        return {false, TEXT("没有有效存档，或存档类型／版本不兼容"), EPadmaRunCommandEffect::None};
    return RunRules.Restore(Save->Run);
}
