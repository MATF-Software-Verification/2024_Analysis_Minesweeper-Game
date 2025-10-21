param(
  [ValidateSet("Debug","Release")]
  [string]$Config = "Debug"
)

$OCC = "C:\Program Files\OpenCppCoverage\OpenCppCoverage.exe"
if (-not (Test-Path $OCC)) {
  throw "OpenCppCoverage not found at: $OCC"
}

$SRC = "$PWD\external\Minesweeper-Game"
$OUT = "$PWD\unit_tests\coverage"
$EXE = "$PWD\build\bin\$Config\ms_tests.exe"

if (-not (Test-Path $EXE)) {
  Write-Host "[$Config] ms_tests.exe not found at: $EXE" -ForegroundColor Yellow
  $cand = Get-ChildItem -Recurse -Filter ms_tests.exe | Select-Object -First 1 -ExpandProperty FullName
  if (-not $cand) { throw "Build tests first." }
  $EXE = $cand
  Write-Host "Using: $EXE" -ForegroundColor Yellow
}

mkdir $OUT -ErrorAction SilentlyContinue | Out-Null

& $OCC `
  --sources "$SRC" `
  --excluded_sources "$PWD\build" `
  --export_type=html:"$OUT" `
  -- "$EXE"

Write-Host "Coverage HTML: $OUT\index.html" -ForegroundColor Green
