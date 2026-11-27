# Riepilogo di Gim per una nuova sessione.
# Primo passo: riconosce il PC (nome del computer) e sceglie la modalita':
#   full     = PC di lavoro del progetto (Ancarlo): git fetch, stato, strumenti, memorie.
#   readonly = PC non riconosciuto (es. portatile di lavoro): solo lettura delle memorie
#              e della data di HANDOFF. Nessun git, nessuno strumento controllato, nessuna modifica.
# Un PC nuovo va aggiunto in $pcProfiles qui sotto. Non riconosciuto = readonly (il default sicuro).
# Uso: powershell -ExecutionPolicy Bypass -File scripts\inizio_sessione.ps1 [-Mode full|readonly] [-NoFetch] [-NoMemory]

param(
    [ValidateSet('full', 'readonly')][string]$Mode,
    [switch]$NoFetch,
    [switch]$NoMemory
)

# Nome del computer -> modalita'. Nomi da verificare con: $env:COMPUTERNAME
$pcProfiles = @{
    'ANCARLO' = 'full'
    'PROXIMA' = 'full'
}

Set-Location (Split-Path -Parent $PSScriptRoot)
$root = Split-Path -Parent (Get-Location)   # cartella "GitHub Asy" sull'unita' di questo PC

# ---------- 0. Riconoscimento del PC ----------
$pc = $env:COMPUTERNAME
if ($pcProfiles.ContainsKey($pc)) { $known = $true; $autoMode = $pcProfiles[$pc] }
else { $known = $false; $autoMode = 'readonly' }
if (-not $Mode) { $Mode = $autoMode }

Write-Output "== PC =="
Write-Output ("Computer: " + $pc + "   Unita' progetto: " + $root.Substring(0, 2))
if ($known) { Write-Output ("Riconosciuto. Modalita': " + $Mode) }
else { Write-Output ("NON in elenco. Modalita': " + $Mode + " (PC non riconosciuto: aggiungerlo in pcProfiles)") }
if ($Mode -eq 'readonly') { $NoFetch = $true }
Write-Output ""

# ---------- 1. Stato del progetto (solo modalita' full) ----------
if ($Mode -eq 'full') {
    if (-not $NoFetch) {
        Write-Output "== Aggiornamento da origin =="
        # git fetch in un job, con limite di 30 secondi: se la rete manca non blocca l'avvio.
        $job = Start-Job -ScriptBlock { param($d) Set-Location $d; git fetch --quiet origin 2>&1 } -ArgumentList (Get-Location).Path
        if (Wait-Job $job -Timeout 30) {
            Receive-Job $job | Out-Null
            Write-Output "fetch: completato"
        } else {
            Stop-Job $job
            Write-Output "fetch: NON completato (timeout 30 s o rete assente): stato locale"
        }
        Remove-Job $job -Force
        Write-Output ""
    }

    Write-Output "== Branch e confronto con origin =="
    $branch = git branch --show-current
    Write-Output ("Branch: " + $branch)
    $track = git rev-list --left-right --count ("origin/" + $branch + "..." + $branch) 2>$null
    if ($LASTEXITCODE -eq 0 -and $track) {
        $parts = $track -split '\s+'
        Write-Output ("Rispetto a origin/" + $branch + ": " + $parts[1] + " commit avanti, " + $parts[0] + " indietro")
    } else {
        Write-Output ("origin/" + $branch + ": non presente")
    }
    git log --oneline -5 2>$null
    Write-Output ""

    Write-Output "== Working tree =="
    $st = @(git status --short 2>$null)
    Write-Output ("File modificati o non tracciati: " + $st.Count)
    Write-Output ""

    Write-Output "== Voci aperte in SYNC\TODO_GIM.md =="
    $todo = Get-Content 'SYNC\TODO_GIM.md'
    Write-Output ("Voci non fatte: " + @($todo | Where-Object { $_ -match '^\s*- \[ \]' }).Count)
    Write-Output ""
}

Write-Output "== Stato in SYNC\HANDOFF.md =="
$handoff = Get-Content 'SYNC\HANDOFF.md' -ErrorAction SilentlyContinue
$stato = $handoff | Where-Object { $_ -match '^Stato al ' } | Select-Object -First 1
if ($stato) { Write-Output $stato } else { Write-Output "data non trovata" }
Write-Output ""

if ($Mode -eq 'full') {
    Write-Output "== Strumenti =="
    $gk = Get-Command gkmake -ErrorAction SilentlyContinue
    if ($gk) { Write-Output ("gkmake:  " + $gk.Source) } else { Write-Output "gkmake:  NON nel PATH" }
    $dx = Get-Command doxygen -ErrorAction SilentlyContinue
    if ($dx) { Write-Output ("doxygen: " + $dx.Source) } else { Write-Output "doxygen: NON nel PATH" }
    Write-Output ""
}

# ---------- 2. Memorie di Claude: tutte, non solo quella del progetto (in entrambe le modalita') ----------
if (-not $NoMemory) {
    Write-Output "== Memorie di Claude =="
    # Cartelle di memoria: tutti i progetti di questo PC, poi quelle sul disco condiviso.
    $dirs = @()
    $projects = Join-Path $env:USERPROFILE '.claude\projects'
    if (Test-Path $projects) {
        $dirs += Get-ChildItem -Path $projects -Directory | ForEach-Object { Join-Path $_.FullName 'memory' }
    }
    $dirs += Join-Path $root '.claude\memory'
    $dirs += Get-ChildItem -Path $root -Directory -ErrorAction SilentlyContinue | ForEach-Object { Join-Path $_.FullName '.claude\memory' }

    $found = 0
    foreach ($d in ($dirs | Where-Object { Test-Path $_ } | Select-Object -Unique)) {
        Write-Output ""
        Write-Output ("--- " + $d)
        Get-ChildItem -Path $d -Filter '*.md' -File | Sort-Object Name | ForEach-Object {
            $found++
            Write-Output ("### " + $_.Name + "  (modificato " + $_.LastWriteTime.ToString('yyyy-MM-dd') + ")")
            Get-Content $_.FullName -Encoding UTF8
            Write-Output ""
        }
    }
    if ($found -eq 0) { Write-Output "Nessuna memoria trovata." }
    Write-Output ""
}

if ($Mode -eq 'full') { Write-Output "Prima di lavorare leggere SYNC\HANDOFF.md e CLAUDE.md." }
else { Write-Output "Modalita' sola lettura: nessuna modifica, nessun git, nessuna installazione." }
