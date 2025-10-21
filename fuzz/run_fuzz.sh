#!/usr/bin/env bash
set -euo pipefail

ROOT="/mnt/c/Users/Milan Manojlovic/Minesweeper-Game"
SRC="$ROOT/external/Minesweeper-Game"
OUT="$ROOT/fuzz"
BIN="$OUT/ms_fuzz"

mkdir -p "$OUT/corpus" "$OUT/crashes" "$OUT/logs"

echo "== Build =="
clang++ -std=c++17 -g -O1 \
  -fsanitize=fuzzer,address,undefined \
  -I"$SRC" \
  "$OUT/fuzz_target.cpp" \
  "$SRC/Board.cpp" "$SRC/Cell.cpp" "$SRC/Utility.cpp" \
  -o "$BIN"

echo "== Seeds =="
printf '\x00\x00\x00\x00' > "$OUT/corpus/seed0" || true
printf '\x02\x02\x01\x03\x04\x00' > "$OUT/corpus/seed1" || true

echo "== Run (60s) =="
ASAN_OPTIONS=atexit=1:abort_on_error=0:allocator_may_return_null=1 \
UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=0 \
"$BIN" \
  -max_total_time=60 \
  -timeout=5 \
  -max_len=64 \
  -artifact_prefix="$OUT/crashes/" \
  "$OUT/corpus" 2>&1 | tee "$OUT/logs/run_$(date +%Y%m%d_%H%M%S).log"

echo "Done. BIN=$BIN"
echo "Corpus:  $OUT/corpus"
echo "Crashes: $OUT/crashes"
echo "Logs:    $OUT/logs"
