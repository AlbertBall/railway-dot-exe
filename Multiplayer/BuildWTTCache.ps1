param(
    [Parameter(Mandatory = $true)][string]$SourceDirectory,
    [string]$OutputFile = "",
    [string]$LocationFile = ""
)

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.IO.Compression
Add-Type -AssemblyName System.IO.Compression.FileSystem

$multiplayerDirectory = $PSScriptRoot
if ($OutputFile -eq '') { $OutputFile = Join-Path $multiplayerDirectory 'WTT\wtt_cache.tsv' }
if ($LocationFile -eq '') { $LocationFile = Join-Path $multiplayerDirectory 'wtt_locations.csv' }
$outputDirectory = Split-Path -Parent $OutputFile
if (!(Test-Path -LiteralPath $outputDirectory)) { New-Item -ItemType Directory -Path $outputDirectory | Out-Null }

$locationMap = @{}
foreach ($line in Get-Content -LiteralPath $LocationFile) {
    $trimmed = $line.Trim()
    if ($trimmed -eq '' -or $trimmed.StartsWith('#')) { continue }
    $parts = $trimmed.Split(',')
    if ($parts.Count -lt 3) { continue }
    $wttLocation = $parts[0].Trim().ToUpperInvariant()
    $railLocation = $parts[1].Trim()
    if ($wttLocation -ne $railLocation.ToUpperInvariant()) {
        Write-Warning "Ignoring unsafe WTT alias '$wttLocation' -> '$railLocation'. Location ownership requires an exact name match."
        continue
    }
    $locationMap[$wttLocation] = @($railLocation, $parts[2].Trim().ToUpperInvariant())
}

function Get-CellColumnIndex([string]$Reference) {
    $index = 0
    foreach ($character in $Reference.ToCharArray()) {
        if ($character -lt 'A' -or $character -gt 'Z') { break }
        $index = ($index * 26) + ([int]$character - [int][char]'A') + 1
    }
    return $index - 1
}

function Clean-Field([object]$Value) {
    if ($null -eq $Value) { return '' }
    $text = [string]$Value
    $text = $text.Replace([string][char]0x00BD, 'H')
    return $text.Replace("`t", ' ').Replace("`r", ' ').Replace("`n", ' ').Replace('|', '/').Trim()
}

function Get-ZipText($Archive, [string]$EntryName) {
    $entry = $Archive.GetEntry($EntryName)
    if ($null -eq $entry) { return $null }
    $reader = New-Object IO.StreamReader($entry.Open())
    try { return $reader.ReadToEnd() } finally { $reader.Dispose() }
}

function Get-SharedStrings($Archive) {
    $text = Get-ZipText $Archive 'xl/sharedStrings.xml'
    if ($null -eq $text) { return @() }
    [xml]$xml = $text
    $values = New-Object Collections.Generic.List[string]
    foreach ($item in $xml.DocumentElement.ChildNodes) {
        $values.Add((Clean-Field $item.InnerText))
    }
    return $values.ToArray()
}

function Get-CellValue($Cell, [string[]]$SharedStrings) {
    $type = [string]$Cell.t
    if ($type -eq 'inlineStr') { return Clean-Field $Cell.InnerText }
    $valueNode = $Cell.SelectSingleNode("*[local-name()='v']")
    if ($null -eq $valueNode) { return '' }
    $value = [string]$valueNode.InnerText
    if ($type -eq 's') {
        $index = 0
        if ([int]::TryParse($value, [ref]$index) -and $index -ge 0 -and $index -lt $SharedStrings.Count) {
            return Clean-Field $SharedStrings[$index]
        }
    }
    return Clean-Field $value
}

function Read-SheetRows($Archive, [string]$EntryName, [string[]]$SharedStrings) {
    $text = Get-ZipText $Archive $EntryName
    if ($null -eq $text) { return @{} }
    [xml]$xml = $text
    $rows = @{}
    foreach ($row in $xml.SelectNodes("//*[local-name()='sheetData']/*[local-name()='row']")) {
        $rowNumber = [int]$row.r
        $cells = @{}
        foreach ($cell in $row.SelectNodes("*[local-name()='c']")) {
            $column = Get-CellColumnIndex ([string]$cell.r)
            $cells[$column] = Get-CellValue $cell $SharedStrings
        }
        $rows[$rowNumber] = $cells
    }
    return $rows
}

function Row-Value($Rows, [int]$Row, [int]$Column) {
    if (!$Rows.ContainsKey($Row) -or !$Rows[$Row].ContainsKey($Column)) { return '' }
    return [string]$Rows[$Row][$Column]
}

$writer = New-Object IO.StreamWriter($OutputFile, $false, (New-Object Text.UTF8Encoding($false)))
$workbookCount = 0
$serviceCount = 0
$callCount = 0
try {
    $writer.WriteLine("WTT-CACHE`t1`t$([DateTime]::Now.ToString('o'))`t$(Clean-Field $SourceDirectory)")
    $files = Get-ChildItem -LiteralPath $SourceDirectory -Recurse -File -Filter *.xlsx |
        Where-Object { $_.FullName -notlike '*\__MACOSX\*' -and $_.Name -notlike '._*' }
    foreach ($file in $files) {
        $archive = [IO.Compression.ZipFile]::OpenRead($file.FullName)
        try {
            $sharedStrings = Get-SharedStrings $archive
            [xml]$workbookXml = Get-ZipText $archive 'xl/workbook.xml'
            [xml]$relationsXml = Get-ZipText $archive 'xl/_rels/workbook.xml.rels'
            $relations = @{}
            foreach ($relationship in $relationsXml.DocumentElement.ChildNodes) {
                $relations[[string]$relationship.Id] = [string]$relationship.Target
            }
            foreach ($sheet in $workbookXml.SelectNodes("//*[local-name()='sheets']/*[local-name()='sheet']")) {
                $relationshipID = [string]$sheet.GetAttribute('id', 'http://schemas.openxmlformats.org/officeDocument/2006/relationships')
                if (!$relations.ContainsKey($relationshipID)) { continue }
                $target = $relations[$relationshipID].Replace('\', '/')
                if ($target.StartsWith('/')) { $target = $target.TrimStart('/') }
                elseif (!$target.StartsWith('xl/')) { $target = 'xl/' + $target.TrimStart('/') }
                $rows = Read-SheetRows $archive $target $sharedStrings
                if ($rows.Count -eq 0) { continue }

                $locationBlocks = New-Object Collections.Generic.List[object]
                $maximumRow = ($rows.Keys | Measure-Object -Maximum).Maximum
                for ($rowNumber = 10; $rowNumber -le $maximumRow; $rowNumber++) {
                    $wttLocation = (Row-Value $rows $rowNumber 0).Trim().ToUpperInvariant()
                    if ($wttLocation -eq '' -or $wttLocation -eq '0') { continue }
                    $block = [ordered]@{ Name=$wttLocation; Rows=@{} }
                    for ($detailRow = $rowNumber; $detailRow -le [Math]::Min($maximumRow, $rowNumber + 8); $detailRow++) {
                        if ($detailRow -gt $rowNumber) {
                            $nextLocation = (Row-Value $rows $detailRow 0).Trim()
                            if ($nextLocation -ne '' -and $nextLocation -ne '0') { break }
                        }
                        $label = (Row-Value $rows $detailRow 1).Trim().ToLowerInvariant()
                        if ($label -ne '') { $block.Rows[$label] = $detailRow }
                    }
                    $locationBlocks.Add([pscustomobject]$block)
                }

                $maximumColumn = 1
                foreach ($cellColumn in $rows[1].Keys) { if ($cellColumn -gt $maximumColumn) { $maximumColumn = $cellColumn } }
                for ($column = 2; $column -le $maximumColumn; $column++) {
                    $tid = Row-Value $rows 1 $column
                    $uid = Row-Value $rows 2 $column
                    if ($tid -eq '' -and $uid -eq '') { continue }
                    $calls = New-Object Collections.Generic.List[object]
                    $touchesRegisteredBox = $false
                    foreach ($block in $locationBlocks) {
                        $platform = if ($block.Rows.Contains('platform')) { Row-Value $rows $block.Rows['platform'] $column } else { '' }
                        $arrival = if ($block.Rows.Contains('arr')) { Row-Value $rows $block.Rows['arr'] $column } else { '' }
                        $departure = if ($block.Rows.Contains('dep')) { Row-Value $rows $block.Rows['dep'] $column } else { '' }
                        $pass = if ($block.Rows.Contains('pass')) { Row-Value $rows $block.Rows['pass'] $column } else { '' }
                        $line = if ($block.Rows.Contains('running line')) { Row-Value $rows $block.Rows['running line'] $column } else { '' }
                        if (($arrival -in @('', '..')) -and ($departure -in @('', '..')) -and ($pass -in @('', '..'))) { continue }
                        $railLocation = ''
                        $ownerBox = ''
                        if ($locationMap.ContainsKey($block.Name)) {
                            $railLocation = $locationMap[$block.Name][0]
                            $ownerBox = $locationMap[$block.Name][1]
                            $touchesRegisteredBox = $true
                        }
                        $calls.Add([pscustomobject]@{WTT=$block.Name;Rail=$railLocation;Box=$ownerBox;Arr=$arrival;Dep=$departure;Pass=$pass;Platform=$platform;Line=$line})
                    }
                    if (!$touchesRegisteredBox) { continue }

                    $serviceKey = "$(Clean-Field $uid)|$(Clean-Field (Row-Value $rows 7 $column))|$(Clean-Field (Row-Value $rows 4 $column))"
                    $writer.WriteLine("S`t$(Clean-Field $serviceKey)`t$(Clean-Field $file.Name)`t$(Clean-Field ([string]$sheet.name))`t$(Clean-Field $tid)`t$(Clean-Field $uid)`t$(Clean-Field (Row-Value $rows 3 $column))`t$(Clean-Field (Row-Value $rows 4 $column))`t$(Clean-Field (Row-Value $rows 5 $column))`t$(Clean-Field (Row-Value $rows 7 $column))`t$(Clean-Field (Row-Value $rows 8 $column))`t$(Clean-Field (Row-Value $rows 9 $column))")
                    foreach ($call in $calls) {
                        $writer.WriteLine("C`t$(Clean-Field $call.WTT)`t$(Clean-Field $call.Rail)`t$(Clean-Field $call.Box)`t$(Clean-Field $call.Arr)`t$(Clean-Field $call.Dep)`t$(Clean-Field $call.Pass)`t$(Clean-Field $call.Platform)`t$(Clean-Field $call.Line)")
                        $callCount++
                    }
                    $writer.WriteLine('E')
                    $serviceCount++
                }
            }
            $workbookCount++
        }
        finally { $archive.Dispose() }
    }
}
finally { $writer.Dispose() }

Write-Output "WTT cache written to $OutputFile"
Write-Output "Workbooks: $workbookCount; services touching registered boxes: $serviceCount; calls: $callCount"
