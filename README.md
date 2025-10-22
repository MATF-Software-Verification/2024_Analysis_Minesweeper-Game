# 2024_Analysis_Minesweeper-Game

Seminarski rad iz predmeta **Verifikacija softvera**

  ## Analizirani projekat

- **Naziv:** Minesweeper-Game  
- **Upstream repo:** <https://github.com/RohithKKannan/Minesweeper-Game>  
- **Grana i commit koji su analizirani:**  
  - Grana: main  
  - Commit hash: fbe3cf7c1a30d12bd254d25375b346f93f7c32c9

Analiziran je konzolni **Minesweeper** napisan u C++.  
Projekat implementira klasičnu igru *Minesweeper* sa izborom težine (Beginner/Intermediate/Expert), generisanjem mine-field table, označavanjem polja (flag), otvaranjem ćelija i logikom pobede/poraza.
Kod je organizovan oko klasa **Cell**, **Board** i **Minesweeper**, uz pomoćne funkcije za konverzije i generisanje nasumičnih vrednosti.

---

## Autor

- Aleksandra Biočanin, mr241092
- Kontakt: aleksandrabiocanin6@gmail.com

---

## Struktura projekta
```
├── .github/
│ └── workflows/ 
├── external/
│ └── Minesweeper-Game/      # git submodule – analizirani projekat
├── unit_tests/              # Catch2 testovi + skripte i coverage HTML
├── cppcheck/                # Statička analiza (izveštaji, skripte)
├── valgrind/
│ └── callgrind/             # Callgrind izlazi + skripta i screenshot-ovi
├── fuzz/                    # libFuzzer target, corpus, crashes, logs
├── patches/                 # Opcione zakrpe (patch files) sa ispravkama
├── CMakeLists.txt           # Build definicija za testove/fuzz
├── ProjectAnalysisReport.md # Detaljan izveštaj
└── README.md
```

---

## Korišćeni alati i tehnike

1) **Catch2 testovi + OpenCppCoverage (C++ kôd, Windows)**
- **Svrha:** Validacija osnovne logike (Board/Minesweeper) i merenje pokrivenosti testovima.  
- **Tip:** Jedinični testovi.  
- **Izlaz:** Konsolni izlaz testova + HTML izveštaj pokrivenosti (`unit_tests/coverage/index.html`).  
- **Reprodukovanje:** Skripta `unit_tests/run_coverage.ps1` (poziva OpenCppCoverage nad `ms_tests.exe`).

2) **Cppcheck (statička analiza)**
- **Svrha:** Pronalaženje potencijalnih grešaka, upozorenja o stilu/perf., neinicijalizovane promenljive, “const-correctness”.  
- **Tip:** Statička analiza koda.  
- **Izlaz:** TXT (`cppcheck/cppcheck-report.txt`) + XML (`cppcheck/output.xml`) + HTML izveštaj (`cppcheck/report/index.html`).  
- **Reprodukovanje:** Skripta `cppcheck/run_cppcheck.ps1`.

3) **Valgrind / Callgrind (profajliranje u WSL-u)**
- **Svrha:** Profajliranje broja instrukcija i relacija poziva.  
- **Tip:** Dinamičko profajliranje (Callgrind).  
- **Izlaz:** `valgrind/callgrind/callgrind.out.*`, sažetak (`callgrind.ms_tests.txt`, `callgrind_cache_report.txt`), i **screenshot-ovi** iz KCachegrind.  
- **Reprodukovanje:** Skripta `valgrind/callgrind/run_callgrind.sh` (WSL).

4) **libFuzzer + AddressSanitizer (fuzzing u WSL-u)**
- **Svrha:** Automatizovano generisanje ulaza radi otkrivanja rušenja/UB (ASan hvata memory bug-ove).  
- **Tip:** Fuzz testiranje.  
- **Izlaz:** `fuzz/corpus/*`, `fuzz/crashes/*` (repro fajlovi), `fuzz/logs/*.log`.  
- **Reprodukovanje:** Skripta `fuzz/run_fuzz.sh` (gradi sa `-fsanitize=address,fuzzer` i pokreće fuzzing).


---

## Reprodukcija (kratko uputstvo)

### 1) Testovi + pokrivenost (Windows PowerShell)
```powershell
Set-Location "C:\Users\<USER>\Minesweeper-Game"
# Build (CMake iz Visual Studia)
& "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug
& "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build --config Debug

# Pokretanje testova + coverage
.\unit_tests\run_coverage.ps1
# Otvoriti: unit_tests\coverage\index.html
```

### 2) Cppcheck (Windows PowerShell)
```powershell
# Pretpostavlja se: Cppcheck instaliran u "C:\Program Files\Cppcheck\cppcheck.exe"
.\cppcheck\run_cppcheck.ps1
# Rezultati: cppcheck\cppcheck-report.txt, cppcheck\output.xml, cppcheck\report\index.html
```

### 3) Callgrind (WSL/Ubuntu)
```bash
cd /mnt/c/Users/<USER>/Minesweeper-Game
mkdir -p build-wsl && cd build-wsl
cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ..
cmake --build . -j

cd ..
bash valgrind/callgrind/run_callgrind.sh
# Rezultati: valgrind/callgrind/callgrind.out.*, callgrind.ms_tests.txt, callgrind_cache_report.txt
# KCachegrind za vizuelizaciju (ako je instaliran).
```

### 4) Fuzzing (WSL/Ubuntu)
```bash
cd /mnt/c/Users/<USER>/Minesweeper-Game
bash fuzz/run_fuzz.sh
# Rezultati: fuzz/corpus/*, fuzz/crashes/*, fuzz/logs/*.log

# Reprodukcija konkretnog crash-a:
./fuzz/ms_fuzz fuzz/crashes/<naziv_crasha> 2>&1 | tee fuzz/logs/repro_<timestamp>.log
```

---

## Sažetak zaključaka
- **Testovi + pokrivenost:** Osnovne funkcionalnosti prolaze, a izveštaj pokrivenosti potvrđuje da su ključni putevi izvršeni (videti `unit_tests/coverage/index.html`).

- **Cppcheck:** Pronađena upozorenja (npr. neinicijalizovan član `Board::boardGenerated`, predlozi za `const` metode, senčenje promenljivih, Rule-of-3/5 preporuke).

- **Callgrind:** Inclusive trošak dominira u flood-fill logici otvaranja susednih polja i u generisanju table; deo troška odlazi na Catch2/standardnu biblioteku (inicijalizacije i parsiranje argumenata).

- **Fuzzing (libFuzzer+ASan):** Otkriven deterministički **problem alloc-dealloc mismatch** u destruktoru `Board` (korišćenje `delete` umesto `delete[]` nad dinamičnim nizovima). Reprodukcija i logovi su u `fuzz/crashes/` i `fuzz/logs/`.

Predlog ispravke: dat u `patches/changes.patch` (ne menja submodul automatski; služi kao artefakt analize).


