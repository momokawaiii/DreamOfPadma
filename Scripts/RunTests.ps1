[CmdletBinding()]
param(
    [string]$ProjectRoot = (Split-Path -Parent $PSScriptRoot),
    [string]$EngineRoot = $env:UNREAL_ENGINE_ROOT,
    [string]$TestFilter = 'Project'
)

$ErrorActionPreference = 'Stop'
$resolvedRoot = (Resolve-Path -LiteralPath $ProjectRoot).Path
if ([string]::IsNullOrWhiteSpace($EngineRoot)) {
    throw 'Set -EngineRoot or the UNREAL_ENGINE_ROOT environment variable.'
}

$editorCmd = Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
$uproject = Get-ChildItem -LiteralPath $resolvedRoot -Filter '*.uproject' -File | Select-Object -First 1
if (-not (Test-Path -LiteralPath $editorCmd)) { throw "UnrealEditor-Cmd.exe not found: $editorCmd" }
if (-not $uproject) { throw 'No .uproject file found.' }

$reportDirectory = Join-Path $resolvedRoot 'Saved\Automation'
New-Item -ItemType Directory -Force -Path $reportDirectory | Out-Null
$execCommands = "Automation RunTests $TestFilter;Quit"
$arguments = @(
    $uproject.FullName,
    '-unattended', '-nop4', '-nosplash', '-nullrhi', '-NoSound',
    "-ExecCmds=$execCommands",
    '-testexit=Automation Test Queue Empty',
    "-ReportOutputPath=$reportDirectory"
)

& $editorCmd @arguments
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Write-Host 'Automation test command completed.' -ForegroundColor Green
