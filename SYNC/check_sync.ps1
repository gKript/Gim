# Confronta SYNC/ di PicGIM e Gim e verifica che le costanti SSP del codice PicGIM
# corrispondano al contratto. Uso: powershell -File SYNC/check_sync.ps1
$here = Split-Path -Parent $MyInvocation.MyCommand.Path
# PicGim e Gim sono cartelle sorelle: li trova a partire da questo script, su qualsiasi unita'.
$root = Split-Path -Parent $here
$base = Split-Path -Parent $root
$pic = Join-Path $base 'PicGim'
$gim = Join-Path $base 'Gim'
$rc = 0
foreach ($f in 'COMM_CONTRACT.md','SYNC_LOG.md','check_sync.ps1') {
  $a = Get-FileHash "$pic\SYNC\$f" -ErrorAction SilentlyContinue
  $b = Get-FileHash "$gim\SYNC\$f" -ErrorAction SilentlyContinue
  if (-not $a -or -not $b) { Write-Host "MANCA   $f"; $rc = 1 }
  elseif ($a.Hash -ne $b.Hash) { Write-Host "DIVERSO $f"; $rc = 1 }
  else { Write-Host "ok      $f" }
}
$h = Get-Content "$pic\PicGIM_Sources\PicGIM\include\pgim_ssp.h" -Raw
$gcp = Get-Content "$pic\PicGIM_Sources\PicGIM\include\pgim_gcp.h" -Raw
$expect = @{
  "PG_SSP_CONTROL_HEADER\s+'K'"='SSP header'; "PG_SSP_CONTROL_HEADER_REPLY\s+'k'"='SSP header reply';
  "PG_SSP_CONTROL_FOOTER\s+'X'"='SSP footer'; "PG_SSP_CONTROL_FOOTER_REPLY\s+'x'"='SSP footer reply';
  "PG_SSP_NO_ERROR\s*=\s*48"='SSP error base' }
foreach ($k in $expect.Keys) { if ($h -notmatch $k) { Write-Host "CODICE DIVERSO DAL CONTRATTO: $($expect[$k])"; $rc = 1 } }
if ($gcp -notmatch "PG_GCP_REPLY_OFFSET_ADD\s+32") { Write-Host "CODICE DIVERSO DAL CONTRATTO: GCP reply offset"; $rc = 1 }
if ($gcp -notmatch "PG_GCP_VERSION_MINOR\s+1") { Write-Host "VERSIONE GCP cambiata: aggiorna il contratto"; $rc = 1 }
exit $rc
