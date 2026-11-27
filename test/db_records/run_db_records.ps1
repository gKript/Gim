# Test of the record commands of gim_db (update_record, delete_record) through gimshell.
# Usage (from the project folder): powershell -ExecutionPolicy Bypass -File test\db_records\run_db_records.ps1
# Needs the shell built with gkmake (build_e\gimshell.exe).
# The permanent database "test_records_perm" lives in the Gim home (%USERPROFILE%\.gim\GimShell).
# If it already exists the test stops: remove that folder by hand, then run again.

$here = $PSScriptRoot
$root = Split-Path -Parent (Split-Path -Parent $here)
$shell = Join-Path $root 'build_e\gimshell.exe'
$out = Join-Path $here 'out'
New-Item -ItemType Directory -Force -Path $out | Out-Null

if (-not (Test-Path $shell)) { Write-Output "gimshell not found: $shell (build with gkmake first)"; exit 2 }

$permHome = Join-Path $env:USERPROFILE '.gim\GimShell\test_records_perm'
if (Test-Path $permHome) {
	Write-Output "The permanent test database already exists: $permHome"
	Write-Output "Remove that folder by hand, then run the test again."
	exit 2
}

$seedVol  = (Join-Path $here 'seed_volatile.gdbs')
$seedPerm = (Join-Path $here 'seed_permanent.gdbs')

function Invoke-Shell([string]$name, [string]$inputFile, [hashtable]$replace) {
	$text = Get-Content -Raw -Path (Join-Path $here $inputFile)
	foreach ($k in $replace.Keys) { $text = $text.Replace($k, $replace[$k]) }
	$runFile = Join-Path $out "$name.txt"
	# written without BOM: a pipe into a native program would add one to the first command
	[System.IO.File]::WriteAllText($runFile, $text, (New-Object System.Text.UTF8Encoding($false)))
	$log = Join-Path $out "$name.out"
	cmd /c "`"$shell`" < `"$runFile`" > `"$log`" 2>&1"
	return (Get-Content -Raw -Path $log)
}

$failures = 0
function Expect([string]$what, [string]$text, [string]$needle) {
	if ($text.Contains($needle)) { Write-Output "ok:   $what" }
	else { Write-Output "FAIL: $what  (missing: $needle)"; $script:failures++ }
}

# 1. volatile database: update and delete cases
$v = Invoke-Shell 'volatile' 'volatile_cases.txt' @{ '%SEED_VOL%' = "`"$seedVol`"" }
Expect 'update of KEY to an existing key is refused' $v 'UPDATE: KEY codice = 1 already exists'
Expect 'update of KEY on several records is refused' $v 'the KEY codice can be set only when one record matches'
Expect 'update of an unknown field is refused'      $v "has no field 'nope'"
Expect 'delete by condition removes 4 records'      $v '4 record(s) deleted from'
Expect 'after the deletes 16 records remain'         $v '16 record(s) of 16'

# 2. permanent database: write, reopen and empty, reopen empty
$p1 = Invoke-Shell 'perm_1' 'permanent_1_write.txt' @{ '%SEED_PERM%' = "`"$seedPerm`"" }
Expect 'permanent: 300 inserts and 1 update'        $p1 '1 record(s) updated in'
Expect 'permanent: delete of 250 records'           $p1 '250 record(s) deleted from'
$p2 = Invoke-Shell 'perm_2' 'permanent_2_delete_all.txt' @{}
Expect 'permanent: reopen shows the 50 saved records' $p2 '50 record(s) of 50'
Expect 'permanent: delete of all records'           $p2 '50 record(s) deleted from'
$p3 = Invoke-Shell 'perm_3' 'permanent_3_empty_reload.txt' @{}
Expect 'permanent: empty table reloads without error' $p3 '0 record(s) of 0'
Expect 'permanent: insert after an empty reload'     $p3 '1 record(s) of 1'

# 3. table drop on a permanent database: the other table keeps its records, the files of the dropped one are gone
$dropHome = Join-Path $env:USERPROFILE '.gim\GimShell\test_records_drop'
if (-not (Test-Path $dropHome)) {
	$seedDrop = (Join-Path $here 'seed_drop.gdbs')
	$d1 = Invoke-Shell 'drop_1' 'drop_1.txt' @{ '%SEED_DROP%' = "`"$seedDrop`"" }
	Expect 'drop: table alpha dropped'                 $d1 "table 'alpha' dropped"
	Expect 'drop: unknown table refused'               $d1 "table drop: 'nope' not dropped"
	$d2 = Invoke-Shell 'drop_2' 'drop_2.txt' @{}
	Expect 'drop: reopen lists only beta'              $d2 'beta                 PERMANENT         1        2'
	Expect 'drop: beta keeps its 2 records'            $d2 '2 record(s) of 2'
	if (Test-Path (Join-Path $dropHome 'alpha.conf')) { Write-Output 'FAIL: drop: alpha.conf still on disk'; $failures++ } else { Write-Output 'ok:   drop: files of alpha deleted' }
} else {
	Write-Output "FAIL: the drop test database already exists: $dropHome (remove it by hand)"
	$failures++
}

# 4. GDBS: UPDATE and DELETE (with AND) and DROP TABLE run through the API; bad syntax refuses the whole script
$gdbsCases = @(
	@{ name = 'gdbs_1'; input = 'gdbs_1_update_delete.txt'; script = 'gdbs_update_delete.gdbs' },
	@{ name = 'gdbs_2'; input = 'gdbs_2_bad_syntax.txt';    script = 'gdbs_bad_syntax.gdbs' },
	@{ name = 'gdbs_3'; input = 'gdbs_3_gt_refused.txt';    script = 'gdbs_gt_refused.gdbs' },
	@{ name = 'gdbs_4'; input = 'gdbs_4_drop_table.txt';    script = 'gdbs_drop_table.gdbs' }
)
$g = @{}
foreach ($c in $gdbsCases) {
	$g[$c.name] = Invoke-Shell $c.name $c.input @{ '%SCRIPT%' = "`"$(Join-Path $here $c.script)`"" }
}
Expect 'gdbs UPDATE sets codice 2 to tre'           $g['gdbs_1'] 'codice=2, nome=tre'
Expect 'gdbs DELETE with AND removes codice 3'      $g['gdbs_1'] '2 record(s) of 2 in'
Expect 'gdbs bad syntax refuses the script'         $g['gdbs_2'] 'is not valid'
Expect 'gdbs operator > is refused'                  $g['gdbs_3'] 'is not valid'
Expect 'gdbs DROP TABLE leaves only b'              $g['gdbs_4'] 'b                    VOLATILE          1        1'

# 5. CSV: export and import keep the same SHA-256 (round trip), the trainer reads the export, bad files add nothing
$csvOut = Join-Path $out 'csv'
New-Item -ItemType Directory -Force -Path $csvOut | Out-Null
$csvSeed = Join-Path $here 'seed_csv.gdbs'
$trn = Join-Path $csvOut 'trn.csv'
$nts = Join-Path $csvOut 'nts.csv'
$badInt = Join-Path $csvOut 'bad_int.csv'
$dupIn = Join-Path $csvOut 'dup_in_file.csv'
[IO.File]::WriteAllText($badInt, "label,ch0,ch0_ok,ch1,ch1_ok`n1,abc,1,331,0`n", (New-Object System.Text.UTF8Encoding($false)))
[IO.File]::WriteAllText($dupIn, "id,nota,temp,ok`n99,`"a`",1.5,YES`n99,`"b`",2.5,NO`n", (New-Object System.Text.UTF8Encoding($false)))
$csvLines = @("db create test_records_csv `"$csvSeed`"")
for ($i = 1; $i -le 60; $i++) { $csvLines += "insert trn label $($i % 3) ch0 $(($i * 997) % 65536) ch0_ok $($i % 2) ch1 $(($i * 331) % 65536) ch1_ok $(($i + 1) % 2)" }
for ($i = 1; $i -le 12; $i++) { $okv = if ($i % 2 -eq 0) { 'YES' } else { 'NO' }; $csvLines += "insert nts id $i nota `"nota, num $i`" temp $i.5 ok $okv" }
$csvLines += @(
	"table export trn `"$trn`"",
	"table export nts `"$nts`"",
	"table import trn2 `"$trn`"",
	"table export trn2 `"$(Join-Path $csvOut 'trn2.csv')`"",
	"table import nts2 `"$nts`"",
	"table export nts2 `"$(Join-Path $csvOut 'nts2.csv')`"",
	"table import nts `"$nts`"",
	"select trn2",
	"table import trn2 `"$badInt`"",
	"select trn2",
	"table import nts2 `"$dupIn`"",
	"select nts2",
	"exit"
)
$csvText = ($csvLines -join "`r`n") + "`r`n"
$csvInput = Join-Path $out 'csv_1.txt'
[IO.File]::WriteAllText($csvInput, $csvText, (New-Object System.Text.UTF8Encoding($false)))
$csvLog = Join-Path $out 'csv_1.out'
cmd /c "`"$shell`" < `"$csvInput`" > `"$csvLog`" 2>&1"
$c = Get-Content -Raw -Path $csvLog
$shas = @([regex]::Matches($c, '(?m)^sha256 ([0-9a-f]{64})') | ForEach-Object { $_.Groups[1].Value })
if ($shas.Count -ge 6 -and $shas[0] -eq $shas[2] -and $shas[0] -eq $shas[3]) { Write-Output 'ok:   csv: trn export, import and re-export have the same SHA-256' } else { Write-Output 'FAIL: csv: trn round trip changed the file'; $failures++ }
if ($shas.Count -ge 6 -and $shas[1] -eq $shas[4] -and $shas[1] -eq $shas[5]) { Write-Output 'ok:   csv: nts export (with commas, FLOAT, BOOL) round trip keeps the same SHA-256' } else { Write-Output 'FAIL: csv: nts round trip changed the file'; $failures++ }
$tr = cmd /c "`"$root\build_e\gimaitrain.exe`" --dataset `"$trn`" --out `"$(Join-Path $csvOut 'model.h')`" 2>&1"
Expect 'csv: the AI trainer accepts the exported file' ($tr -join "`n") 'ACCEPTED'
Expect 'csv: import of trn: 60 records'                 $c '60 record(s) imported into ''trn2'''
Expect 'csv: a value that is not an INT is refused'      $c "wants an INT, got 'abc'"
Expect 'csv: a KEY repeated in the file is refused'      $c 'KEY id = 99 appears twice in the file'
Expect 'csv: a KEY already in the table is refused'      $c 'KEY id = 1 already exists in [nts]'
Expect 'csv: after the refused files trn2 still has 60'  $c '60 record(s) of 60 in ''trn2'''
Expect 'csv: after the refused file nts2 still has 12'   $c '12 record(s) of 12 in ''nts2'''

# the permanent databases were created by this test: remove them, so the next run starts clean
if (Test-Path $permHome) { Remove-Item -Recurse -Force $permHome }
if (Test-Path $dropHome) { Remove-Item -Recurse -Force $dropHome }

Write-Output ""
if ($failures -eq 0) { Write-Output "ALL OK" } else { Write-Output "$failures FAILED" }
exit $failures
