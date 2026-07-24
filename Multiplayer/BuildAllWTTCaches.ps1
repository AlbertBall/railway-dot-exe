param(
    [string]$SourceRoot = '',
    [string[]]$Book = @()
)

$ErrorActionPreference = 'Stop'
$multiplayerDirectory = $PSScriptRoot
$registryFile = Join-Path $multiplayerDirectory 'wtt_books.csv'
$sourceFile = Join-Path $multiplayerDirectory 'wtt_source.txt'

if ($SourceRoot -eq '' -and (Test-Path -LiteralPath $sourceFile)) {
    $SourceRoot = (Get-Content -LiteralPath $sourceFile -TotalCount 1).Trim()
}
if ($SourceRoot -eq '' -or !(Test-Path -LiteralPath $SourceRoot -PathType Container)) {
    throw 'Set SourceRoot, or put the local Working Timetable documents folder on the first line of Multiplayer\wtt_source.txt.'
}

$requested = @{}
foreach ($code in $Book) { $requested[$code.Trim().ToUpperInvariant()] = $true }
$definitions = @{}
foreach ($line in Get-Content -LiteralPath $registryFile) {
    $trimmed = $line.Trim()
    if ($trimmed -eq '' -or $trimmed.StartsWith('#')) { continue }
    $parts = $trimmed.Split(',')
    if ($parts.Count -lt 4) { continue }
    $bookCode = $parts[1].Trim().ToUpperInvariant()
    if ($requested.Count -gt 0 -and !$requested.ContainsKey($bookCode)) { continue }
    if (!$definitions.ContainsKey($bookCode)) {
        $definitions[$bookCode] = [pscustomobject]@{
            Code = $bookCode
            Cache = $parts[2].Trim()
            Source = $parts[3].Trim()
        }
    }
}

if ($definitions.Count -eq 0) { throw 'No matching WTT books were found in Multiplayer\wtt_books.csv.' }
foreach ($definition in ($definitions.Values | Sort-Object Code)) {
    $bookSource = Join-Path $SourceRoot $definition.Source
    if (!(Test-Path -LiteralPath $bookSource -PathType Container)) {
        throw "WTT source folder for $($definition.Code) was not found: $bookSource"
    }
    $output = Join-Path (Join-Path $multiplayerDirectory 'WTT') $definition.Cache
    Write-Output "Building $($definition.Code) from $bookSource"
    & (Join-Path $multiplayerDirectory 'BuildWTTCache.ps1') -SourceDirectory $bookSource -OutputFile $output
}
