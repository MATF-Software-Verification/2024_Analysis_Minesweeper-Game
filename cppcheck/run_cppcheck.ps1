# cppcheck/run_cppcheck.ps1
param(
  [string]$Src = "$PWD\external\Minesweeper-Game",
  [string]$BuildDir = "$PWD\build"
)

$ErrorActionPreference = "Stop"

# 1) Lokacije alata
$CppcheckEXE = "C:\Program Files\Cppcheck\cppcheck.exe"
if (-not (Test-Path $CppcheckEXE)) {
  throw "Cppcheck not found at: $CppcheckEXE"
}

# (opciono) htmlreport (često je Python skripta u istoj instalaciji)
$HtmlReportPy = "C:\Program Files\Cppcheck\htmlreport\cppcheck-htmlreport"
$HasHtml = (Test-Path $HtmlReportPy) -and (Get-Command python -ErrorAction SilentlyContinue)

# 2) Izlazni fajlovi/folderi
$OutDir = "$PWD\cppcheck"
$ReportDir = "$OutDir\report"
$ReportDirImproved = "$OutDir\report_improved"
$Txt = "$OutDir\cppcheck-report.txt"
$Xml = "$OutDir\output.xml"
$XmlImproved = "$OutDir\output_improved.xml"

New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

Write-Host ">> Baseline run (TXT + XML)..."

# 3) BASELINE – TXT (uhvati stderr i stdout u fajl)
& "$CppcheckEXE" `
  --enable=all --inconclusive --std=c++17 `
  -I "$Src" -i "$BuildDir" `
  --suppress=missingIncludeSystem `
  "$Src" 2>&1 | Out-File -FilePath "$Txt" -Encoding utf8

# 3b) BASELINE – XML (cppcheck piše XML na stderr) 
& "$CppcheckEXE" `
  --enable=all --inconclusive --std=c++17 `
  -I "$Src" -i "$BuildDir" `
  --suppress=missingIncludeSystem `
  --xml "$Src" 2> "$Xml"

if ($HasHtml) {
  Write-Host ">> Generating HTML (report/)..."
  New-Item -ItemType Directory -Force -Path $ReportDir | Out-Null
  python "$HtmlReportPy" --file "$Xml" --report-dir "$ReportDir" --source-dir "$Src" --title "Cppcheck Report"
} else {
  Write-Host ">> Skipping HTML (cppcheck-htmlreport or python not found)."
}

# 4) IMPROVED – pojačani checkovi (exhaustive, portability itd.)
Write-Host ">> Improved run (XML only)..."
& "$CppcheckEXE" `
  --enable=all --inconclusive --std=c++17 `
  --check-level=exhaustive `
  --platform=win64 `
  -I "$Src" -i "$BuildDir" `
  --suppress=missingIncludeSystem `
  --xml "$Src" 2> "$XmlImproved"

if ($HasHtml) {
  Write-Host ">> Generating HTML (report_improved/)..."
  New-Item -ItemType Directory -Force -Path $ReportDirImproved | Out-Null
  python "$HtmlReportPy" --file "$XmlImproved" --report-dir "$ReportDirImproved" --source-dir "$Src" --title "Cppcheck Report (Improved)"
}

Write-Host "== DONE =="
Write-Host "TXT:  $Txt"
Write-Host "XML:  $Xml"
Write-Host "XML+: $XmlImproved"
if ($HasHtml) {
  Write-Host "HTML: $ReportDir\index.html"
  Write-Host "HTML+: $ReportDirImproved\index.html"
} else {
  Write-Host "(HTML skipped)"
}
