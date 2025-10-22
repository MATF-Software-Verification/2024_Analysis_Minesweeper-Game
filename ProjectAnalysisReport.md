# Izveštaj o analizi projekta 'Minesweeper-Game'

Ovaj dokument sadrži detaljan izveštaj o analizi otvorenog C++ projekta Minesweeper-Game.  
## Korišćeni alati

- **Testovi:** Catch2 (unit & smoke testovi) ** + OpenCppCoverage** (HTML izveštaj pokrivenosti na Windowsu)  
  *Repo/fajlovi:* `unit_tests/tests/*`, `unit_tests/run_coverage.ps1`, izlaz u `unit_tests/coverage/`
- **Statička analiza:** Cppcheck  
  *Repo/fajlovi:* `cppcheck/run_cppcheck.ps1`, izveštaj u `cppcheck/`
- **Profajliranje:** Valgrind — Callgrind (u WSL)  
  *Repo/fajlovi:* `valgrind/callgrind/run_callgrind.sh`, izveštaji u `valgrind/callgrind/`
- **Fuzzing:** LLVM libFuzzer + AddressSanitizer (u WSL), pronađen realan bug  
  *Repo/fajlovi:* `fuzz/run_fuzz.sh`, corpus/crashes/logs u `fuzz/`, patch u `patches/changes.patch`

## 1) Testiranje jedinica koda (Catch2) + pokrivenost (OpenCppCoverage)

### Šta testiramo
- **Board**: inicijalno stanje (`GetCellsOpened()==0`), bezbedno generisanje table (`GenerateBoard`), potezi (`PlayerSelect(Open/Mark)`), signalizacija bombe (`GetBombClick()`).
- **Minesweeper**: bezbedni pozivi koji ne blokiraju (`Instructions()`, `CheckWin()`).

Testovi su mali “smoke” scenariji da potvrde stabilnost bez interakcije sa konzolnim I/O ciklusom.

**Lokacije:**
- Test fajlovi: `unit_tests/tests/test_board.cpp`, `unit_tests/tests/test_gameplay.cpp`
- Skripta za pokrivenost: `unit_tests/run_coverage.ps1` (poziva test izvršni fajl kroz OpenCppCoverage i pravi HTML)

### Reprodukcija (Windows / PowerShell)

1. **Configure & build (VS 2022 CMake):**
   ```powershell
   & "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" `
     -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug

   & "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" `
     --build build --config Debug

2. **Pokretanje testova + HTML pokrivenost (OpenCppCoverage):**
   ```powershell
   Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
    .\unit_tests\run_coverage.ps1 -Config Debug

-Test izvršni: .\build\bin\Debug\ms_tests.exe

-HTML izveštaj: unit_tests\coverage\index.html
(per-fajl: unit_tests\coverage\Modules\ms-tests\*.html)

### Rezultat

-Očekivani ispis testova: All tests passed (8 assertions in 5 test cases)

-HTML pokrivenost dostupna lokalno (otvoriti [out/index.html](unit_tests\coverage\index.html)).
