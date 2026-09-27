@echo off

set "UE=E:\Epic Games\UE_5.8"
set "PROJECT=%~dp0DreamOfPadma.uproject"

"%UE%\Engine\Binaries\Win64\UnrealEditor.exe" "%PROJECT%" -game -log

pause