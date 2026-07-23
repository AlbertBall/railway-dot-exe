$ErrorActionPreference = 'Stop'

$projectRoot = Split-Path -Parent $PSScriptRoot
$outputDirectory = Join-Path $projectRoot 'Program timetables'

$guildfordEntries = @(
    '08:00',
    '2G01;Guildford to Reigate multiplayer test;0;120;180;101;1200,08:00;Snt;45-20 46-20,08:01;Guildford,08:06;08:07;Shalford,08:11;08:12;Chilworth,08:18;08:19;Gomshall,08:21;Fer;125-21'
)

$reigateEntries = @(
    '08:00',
    '2G01;Guildford to Reigate multiplayer test;60;120;180;101;1200,08:21;Snt;33-19 34-19,08:29;08:30;Dorking West,08:33;08:34;Dorking Deepdene,08:40;08:41;Betchworth,08:47;Reigate,Frh'
)

function Write-RailwayTimetable {
    param(
        [Parameter(Mandatory = $true)][string]$Path,
        [Parameter(Mandatory = $true)][string[]]$Entries
    )

    $content = ($Entries -join [char]0) + [char]0
    [IO.File]::WriteAllBytes($Path, [Text.Encoding]::ASCII.GetBytes($content))
}

Write-RailwayTimetable -Path (Join-Path $outputDirectory 'MULTIPLAYER-GD 0800.ttb') -Entries $guildfordEntries
Write-RailwayTimetable -Path (Join-Path $outputDirectory 'MULTIPLAYER-RG 0800.ttb') -Entries $reigateEntries
