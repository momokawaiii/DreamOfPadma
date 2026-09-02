[CmdletBinding()]
param(
    [string]$ProjectRoot = (Split-Path -Parent $PSScriptRoot)
)

$ErrorActionPreference = 'Stop'
$resolvedRoot = (Resolve-Path -LiteralPath $ProjectRoot).Path
$contentRoot = Join-Path $resolvedRoot 'Content'
if (-not (Test-Path -LiteralPath $contentRoot)) { throw "Missing Content directory: $contentRoot" }

$assets = Get-ChildItem -LiteralPath $contentRoot -Recurse -File
$extensionGroups = $assets | Group-Object Extension | Sort-Object Name

Write-Host "Content root: $contentRoot"
Write-Host "Files: $($assets.Count)"
foreach ($group in $extensionGroups) {
    Write-Host ("  {0,-10} {1,6}" -f $group.Name, $group.Count)
}

$allowedTopLevel = @('Padma', 'ThirdParty', 'Sandbox', 'Collections', 'Developers', '__ExternalActors__', '__ExternalObjects__')
$topLevel = Get-ChildItem -LiteralPath $contentRoot -Directory | Where-Object { $_.Name -notin @('__ExternalActors__', '__ExternalObjects__') }
foreach ($directory in $topLevel) {
    if ($directory.Name -notin $allowedTopLevel) {
        Write-Warning "Unclassified Content top-level directory: $($directory.Name)"
    }
}

$opaqueNamePattern = '^(ALL|DTA|DTB|DTC|DTD|NewDataTable|amsdj|yndj)$'
$opaqueNames = $assets | Where-Object { $_.BaseName -match $opaqueNamePattern }
if ($opaqueNames) {
    Write-Warning "Found $($opaqueNames.Count) potentially opaque asset names; review them before production content grows."
}

Write-Host 'Asset audit completed.' -ForegroundColor Green
