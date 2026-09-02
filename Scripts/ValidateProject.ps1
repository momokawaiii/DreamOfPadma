[CmdletBinding()]
param(
    [string]$ProjectRoot = (Split-Path -Parent $PSScriptRoot),
    [switch]$Strict
)

$ErrorActionPreference = 'Stop'
$resolvedRoot = (Resolve-Path -LiteralPath $ProjectRoot).Path
$errors = [System.Collections.Generic.List[string]]::new()
$warnings = [System.Collections.Generic.List[string]]::new()

$uproject = Get-ChildItem -LiteralPath $resolvedRoot -Filter '*.uproject' -File | Select-Object -First 1
if (-not $uproject) {
    $errors.Add('No .uproject file found in the project root.')
}

$requiredPaths = @(
    'AGENTS.md', 'README.md', '.gitignore', '.gitattributes',
    'Docs', 'Scripts', 'Build', 'Config', 'Content', 'Source'
)
foreach ($relativePath in $requiredPaths) {
    if (-not (Test-Path -LiteralPath (Join-Path $resolvedRoot $relativePath))) {
        $errors.Add("Missing required path: $relativePath")
    }
}

$gitIgnorePath = Join-Path $resolvedRoot '.gitignore'
if (Test-Path -LiteralPath $gitIgnorePath) {
    $gitIgnoreText = Get-Content -LiteralPath $gitIgnorePath -Raw
    if ($gitIgnoreText -match '(?m)^Content/?$') {
        $errors.Add('.gitignore must not ignore the whole Content directory.')
    }
    foreach ($generatedPath in @('Binaries/', 'DerivedDataCache/', 'Intermediate/', 'Saved/')) {
        if ($gitIgnoreText -notmatch [regex]::Escape($generatedPath)) {
            $warnings.Add(".gitignore does not visibly contain $generatedPath")
        }
    }
}

if ($uproject) {
    try {
        $projectJson = Get-Content -LiteralPath $uproject.FullName -Raw | ConvertFrom-Json
        if (-not $projectJson.Modules -or $projectJson.Modules.Count -lt 1) {
            $errors.Add('The .uproject does not declare a runtime module.')
        }
    }
    catch {
        $errors.Add("Unable to parse $($uproject.Name): $($_.Exception.Message)")
    }
}

Write-Host "Project: $resolvedRoot"
if ($uproject) { Write-Host "UProject: $($uproject.Name)" }
if ($errors.Count -gt 0) {
    Write-Host "Errors: $($errors.Count)" -ForegroundColor Red
    $errors | ForEach-Object { Write-Host "  ERROR: $_" -ForegroundColor Red }
}
if ($warnings.Count -gt 0) {
    Write-Host "Warnings: $($warnings.Count)" -ForegroundColor Yellow
    $warnings | ForEach-Object { Write-Host "  WARN: $_" -ForegroundColor Yellow }
}

if ($errors.Count -gt 0 -or ($Strict -and $warnings.Count -gt 0)) { exit 1 }
Write-Host 'Project validation passed.' -ForegroundColor Green
