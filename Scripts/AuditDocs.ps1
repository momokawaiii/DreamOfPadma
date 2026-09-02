[CmdletBinding()]
param(
    [string]$ProjectRoot = (Split-Path -Parent $PSScriptRoot)
)

$ErrorActionPreference = 'Stop'
$resolvedRoot = (Resolve-Path -LiteralPath $ProjectRoot).Path
$excludedPattern = '\\(\.git|\.vs|\.vscode|Binaries|DerivedDataCache|Intermediate|Saved)\\'
$files = @(Get-ChildItem -LiteralPath $resolvedRoot -Recurse -File -Filter '*.md' -Force |
    Where-Object { $_.FullName -notmatch $excludedPattern })

function Get-CompanionPath {
    param([System.IO.FileInfo]$File)

    $fullPath = $File.FullName
    if ($fullPath -match '\\Docs\\Design\\EN\\') {
        return $fullPath.Replace('\Docs\Design\EN\', '\Docs\Design\ZH\')
    }
    if ($fullPath -match '\\Docs\\Design\\ZH\\') {
        return $fullPath.Replace('\Docs\Design\ZH\', '\Docs\Design\EN\')
    }

    $suffix = '.zh-CN.md'
    if ($File.Name.EndsWith($suffix, [System.StringComparison]::OrdinalIgnoreCase)) {
        $englishName = $File.Name.Substring(0, $File.Name.Length - $suffix.Length) + '.md'
        return (Join-Path $File.DirectoryName $englishName)
    }

    return (Join-Path $File.DirectoryName ($File.BaseName + '.zh-CN.md'))
}

$missing = [System.Collections.Generic.List[string]]::new()
$checkedPairs = @{}
foreach ($file in $files) {
    $companion = Get-CompanionPath -File $file
    $pairKey = @($file.FullName.ToLowerInvariant(), $companion.ToLowerInvariant()) | Sort-Object
    $pairKey = $pairKey -join '|'
    if ($checkedPairs.ContainsKey($pairKey)) { continue }
    $checkedPairs[$pairKey] = $true

    if (-not (Test-Path -LiteralPath $companion -PathType Leaf)) {
        $missing.Add($companion)
    }
}

Write-Host "Markdown files checked: $($files.Count)"
Write-Host "Language pairs checked: $($checkedPairs.Count)"
if ($missing.Count -gt 0) {
    Write-Host "Missing companions: $($missing.Count)" -ForegroundColor Red
    $missing | Sort-Object -Unique | ForEach-Object { Write-Host "  MISSING: $_" -ForegroundColor Red }
    exit 1
}

Write-Host 'Markdown language-pair audit passed.' -ForegroundColor Green
