#!/usr/bin/env bash
set -euo pipefail

ROOT="/mnt/c/Users/Milan Manojlovic/Minesweeper-Game"
BUILD="$ROOT/build-wsl"
BIN="$BUILD/bin/ms_tests"
OUT="$ROOT/valgrind/callgrind"

# Ako imaš testove označene [interactive], preskoči ih:
FILTER="~[interactive]"

mkdir -p "$OUT"

echo "== Callgrind: 5 pokretanja (fokus na test petlju) =="
  echo "-- Run $i"
valgrind --tool=callgrind \
  --collect-atstart=no \
  --toggle-collect="main" \
  --toggle-collect="Catch::Session::run" \
  --toggle-collect="Catch::Session::runInternal" \
  --toggle-collect="Catch::RunContext::runCurrentTest" \
  --toggle-collect="Catch::(anonymous namespace)::TestGroup::execute" \
  --cache-sim=yes --branch-sim=yes \
  --callgrind-out-file="$OUT/callgrind.out.$i" \
  "$BIN" $FILTER >/dev/null 2>&1 || true

done

echo "== Annotate (inclusive) =="
callgrind_annotate --auto=yes --inclusive=yes \
  "$OUT/callgrind.out.5" > "$OUT/callgrind.ms_tests.txt"

echo "== Cache report (sažetak) =="
callgrind_annotate --auto=yes --threshold=99 \
  "$OUT/callgrind.out.5" > "$OUT/callgrind_cache_report.txt"

echo "Gotovo. Artefakti su u: $OUT"
