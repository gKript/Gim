# postprocess_cards.ps1  -  PicGIM documentation
#
# Wraps every documentation section (the \section blocks, rendered by Doxygen as h1.doxsection)
# in <section class="pg-card">...</section>, so that picgim_modern.css can draw each section as
# a card. Doxygen emits a heading and its text as flat siblings, so CSS alone cannot do it.
#
# Run by Make_documentation_MODERN.bat after Doxygen. PowerShell 5.1, no extra tools.
#   - only pages whose main text (div.textblock) contains at least one h1 are touched
#   - the text before the first section (logos, table of contents) stays outside the cards
#   - only <section> tags are added: the page structure and the Doxygen scripts are unaffected
#   - safe to run twice: pages that already contain pg-card are skipped
#
# Usage:  powershell -NoProfile -ExecutionPolicy Bypass -File postprocess_cards.ps1 -HtmlDir <folder>
#
# To go back to the plain look, do not run this script (remove its line from the .bat).

param([Parameter(Mandatory = $true)][string]$HtmlDir)

$ErrorActionPreference = 'Stop'
$enc = New-Object System.Text.UTF8Encoding($false)
$marker = '<div class="textblock">'
$pages = 0; $cards = 0; $skipped = 0

foreach ($f in Get-ChildItem -Path $HtmlDir -Filter '*.html' -File) {
    $s = [IO.File]::ReadAllText($f.FullName)
    if ($s.Contains('class="pg-card"')) { $skipped++; continue }
    $start = $s.IndexOf($marker)
    if ($start -lt 0) { continue }
    $innerStart = $start + $marker.Length

    # find the </div> that closes this textblock
    $tail = $s.Substring($innerStart)
    $depth = 1; $close = -1
    foreach ($m in [regex]::Matches($tail, '<div\b|</div>')) {
        if ($m.Value -eq '</div>') { $depth-- } else { $depth++ }
        if ($depth -eq 0) { $close = $m.Index; break }
    }
    if ($close -lt 0) { Write-Warning "$($f.Name): textblock not closed, skipped"; continue }

    $inner = $tail.Substring(0, $close)
    if ($inner -notmatch '<h1\b') { continue }

    $parts = [regex]::Split($inner, '(?=<h1\b)')
    $sb = New-Object System.Text.StringBuilder
    foreach ($p in $parts) {
        if ($p -match '^\s*<h1\b') { [void]$sb.Append('<section class="pg-card">').Append($p).Append('</section>'); $cards++ }
        else { [void]$sb.Append($p) }
    }
    $new = $s.Substring(0, $innerStart) + $sb.ToString() + $tail.Substring($close)
    [IO.File]::WriteAllText($f.FullName, $new, $enc)
    $pages++
}
"cards: $cards sections in $pages pages ($skipped already processed)"
exit 0
