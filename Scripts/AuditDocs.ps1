[CmdletBinding()]
param(
    [string]$ProjectRoot = (Split-Path -Parent $PSScriptRoot)
)

$ErrorActionPreference = 'Stop'
$resolvedRoot = (Resolve-Path -LiteralPath $ProjectRoot).Path
# Project-maintained documents only; generated output and external packages do not own this policy.
$excludedPattern = '\\(\.git|\.vs|\.vscode|Binaries|DerivedDataCache|Intermediate|Saved|Artifacts|ThirdParty|node_modules)\\'
$files = @(Get-ChildItem -LiteralPath $resolvedRoot -Recurse -File -Filter '*.md' -Force |
    Where-Object { $_.FullName -notmatch $excludedPattern })
$missing = [System.Collections.Generic.List[string]]::new()
$optionalCount = 0

foreach ($file in $files) {
    $path = $file.FullName
    if ($path -match '\\Docs\\Design\\ZH\\') {
        $companion = $path.Replace('\Docs\Design\ZH\', '\Docs\Design\EN\')
    }
    elseif ($file.Name.EndsWith('.zh-CN.md', [System.StringComparison]::OrdinalIgnoreCase)) {
        $companion = Join-Path $file.DirectoryName ($file.Name.Substring(0, $file.Name.Length - 9) + '.md')
    }
    elseif ($path -match '\\Docs\\Design\\EN\\') {
        $companion = $path.Replace('\Docs\Design\EN\', '\Docs\Design\ZH\')
    }
    else {
        $companion = Join-Path $file.DirectoryName ($file.BaseName + '.zh-CN.md')
        # Internal task/history/evidence records may be English-only. Existing Chinese files
        # are still checked in the branches above and must never be orphaned.
        if ($path -match '\\Docs\\Production\\(Tasks|History|Evidence)\\') {
            if (-not (Test-Path -LiteralPath $companion -PathType Leaf)) { $optionalCount++ }
            continue
        }
    }
    if (-not (Test-Path -LiteralPath $companion -PathType Leaf)) { $missing.Add($companion) }
}

Write-Host "Project Markdown files checked: $($files.Count)"
Write-Host "Allowed English-only internal records: $optionalCount"
if ($missing.Count -gt 0) {
    Write-Host "Missing required sources/summaries: $($missing.Count)" -ForegroundColor Red
    $missing | Sort-Object -Unique | ForEach-Object { Write-Host "  MISSING: $_" }
    exit 1
}
Write-Host 'Documentation summary/source audit passed.' -ForegroundColor Green
