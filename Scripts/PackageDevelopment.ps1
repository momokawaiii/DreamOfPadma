[CmdletBinding()]
param(
    [string]$ProjectRoot = (Split-Path -Parent $PSScriptRoot),
    [string]$EngineRoot = $env:UNREAL_ENGINE_ROOT,
    [string]$OutputDirectory = ''
)

$ErrorActionPreference = 'Stop'
$resolvedRoot = (Resolve-Path -LiteralPath $ProjectRoot).Path
if ([string]::IsNullOrWhiteSpace($EngineRoot)) {
    throw 'Set -EngineRoot or the UNREAL_ENGINE_ROOT environment variable.'
}

$uat = Join-Path $EngineRoot 'Engine\Build\BatchFiles\RunUAT.bat'
$uproject = Get-ChildItem -LiteralPath $resolvedRoot -Filter '*.uproject' -File | Select-Object -First 1
if (-not (Test-Path -LiteralPath $uat)) { throw "RunUAT.bat not found: $uat" }
if (-not $uproject) { throw 'No .uproject file found.' }

if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
    $OutputDirectory = Join-Path $resolvedRoot 'Artifacts\WindowsDevelopment'
}
New-Item -ItemType Directory -Force -Path $OutputDirectory | Out-Null

$arguments = @(
    'BuildCookRun',
    "-project=$($uproject.FullName)",
    '-noP4', '-platform=Win64', '-clientconfig=Development',
    '-build', '-cook', '-stage', '-pak', '-archive',
    "-archivedirectory=$OutputDirectory", '-utf8output'
)

& $uat @arguments
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Write-Host "Development package completed: $OutputDirectory" -ForegroundColor Green
