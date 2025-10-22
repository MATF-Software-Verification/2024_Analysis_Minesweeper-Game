# Izveštaj o analizi projekta 'Minesweeper-Game'

Ovaj dokument sadrži detaljan izveštaj o analizi otvorenog C++ projekta Minesweeper-Game.  
## Korišćeni alati

- **Testovi:** Catch2 (unit & smoke testovi) ** + OpenCppCoverage** (HTML izveštaj pokrivenosti na Windowsu)  
  *Repo/fajlovi:* `unit_tests/tests/*`, `unit_tests/run_coverage.ps1`, izlaz u `unit_tests/coverage/`
- **Statička analiza:** Cppcheck  
  *Repo/fajlovi:* `cppcheck/run_cppcheck.ps1`, izveštaj u `cppcheck/`
- **Profajliranje:** Valgrind — Callgrind (u WSL)  
  *Repo/fajlovi:* `valgrind/callgrind/run_callgrind.sh`, izveštaji u `valgrind/callgrind/`
- **Fuzzing:** LLVM libFuzzer + AddressSanitizer (u WSL) 
  *Repo/fajlovi:* `fuzz/run_fuzz.sh`, corpus/crashes/logs u `fuzz/`, patch u `patches/changes.patch`

## 1) Testiranje jedinica koda (Catch2) + pokrivenost (OpenCppCoverage)

### Kratak opis alata

- **Catch2** – C++ framework za jedinično testiranje. Testovi se pišu kao obični `.cpp` fajlovi (bez posebnog runner-a), a izvršni fajl testova generiše Catch2 sam (main funkcija je obezbeđena preko zaglavlja/biblioteke).
- **OpenCppCoverage (Windows)** – alat za merenje pokrivenosti C/C++ koda na Windows-u. Pokreće zadati test izvršni fajl i generiše čitljiv **HTML izveštaj** sa procentima pokrivenosti po fajlovima/linijama.

---

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
   ```
   
2. **Pokretanje testova + HTML pokrivenost (OpenCppCoverage):**
   ```powershell
   Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
    .\unit_tests\run_coverage.ps1 -Config Debug
   ```

- Test izvršni: `.\build\bin\Debug\ms_tests.exe`

- HTML izveštaj: `unit_tests\coverage\index.html`
 (per-fajl: `unit_tests\coverage\Modules\ms-tests\*.html`)

### Rezultat

- Očekivani ispis testova: All tests passed (8 assertions in 5 test cases)

- HTML pokrivenost dostupna lokalno (otvoriti [out/index.html](unit_tests\coverage\index.html)).

---

## 2) Statička analiza (Cppcheck)

### Kratak opis alata

- **Cppcheck** – statički analizator za C/C++ koji otkriva probleme koje kompajler često ne prijavi (npr. neinicijalizovane promenljive, potencijalne greške sa pokazivačima, pogrešne idiome upravljanja resursima, stil/perf upozorenja).  

---

### Reprodukcija
Skripta `cppcheck/run_cppcheck.ps1` izvodi:
1. **Baseline** prolaz sa `--enable=all --inconclusive --std=c++17`, uz TXT i XML izveštaj.
2. (Opciono) **HTML** izveštaj pomoću `cppcheck-htmlreport` ako je dostupan.
3. **Improved** prolaz sa `--check-level=exhaustive` i `--platform=win64` u `output_improved.xml` (+ opcioni HTML).

Konfiguracija u skripti:
```powershell
$CppcheckEXE = "C:\Program Files\Cppcheck\cppcheck.exe"
$Src         = "$PWD\external\Minesweeper-Game"
$BuildDir    = "$PWD\build"
$OutDir      = "$PWD\cppcheck"
```

### Šta je analizirano

- **Ulaz**: `external/Minesweeper-Game/`  
- **Isključeno**: `build/`, `build-wsl/` i ostali generisani folderi (da izbegnemo lažne nalaze i ubrzamo analizu).  
- **Podešavanja**:
  - `--enable=all` – uključi sve provere
  - `--inconclusive` – uključi „neodređena“ upozorenja (ponekad lažno-pozitivna, ali daju koristan uvid)
  - `--std=c++17` – standard jezika
  - `-i build` – ignoriši folder `build` (ekvivalentno `--exclude` u novijim verzijama)
  - `-I external/Minesweeper-Game` – dodela include putanje ka izvorima


### 1) Neinicijalizovan član
- **Problem:** `Board::boardGenerated` nije inicijalizovan u konstruktoru (`Board.cpp`, konstruktor `Board::Board(int,int)`).
- **Rizik:** grananje na neodređenoj vrednosti (UB) ako se polje koristi pre postavljanja.
- **Cppcheck tag:** `uninitMemberVar`.
- **Predlog:** U konstruktoru eksplicitno postaviti `boardGenerated = false;`.

---

### 2) Rule of 3/5 upozorenje (upravljanje resursima)
- **Problem:** Klasa `Board` upravlja dinamičkom memorijom (`Cell** arr`), a nema definisan **copy constructor** ni **operator=**.
- **Rizik:** plitko kopiranje ⇒ moguće dvostruko oslobađanje resursa; čak i bez trenutnog kopiranja, ovo je **code smell**.
- **Cppcheck tagovi:** `noCopyConstructor`, `noOperatorEq`.
- **Predlog:** Eksplicitno zabraniti kopiranje (`delete`) ili implementirati semantiku kopiranja/pomeranja (Rule of 3/5).

---

### 3) Sakrivanje promenljive (shadowing)
- **Mesto:** `Board::GenerateBoard(...)`
- **Opis:** Unutrašnja petlja `for (int i = 0; i < xCord.size(); i++)` zasenjuje (shadow) spoljni `i`.
- **Efekat:** pogoršana čitljivost, rizik od skrivenih grešaka pri izmenama.
- **Cppcheck tag:** `shadowVariable`.
- **Predlog:** Preimenovati unutrašnji brojač (`j`, `k`…) ili koristiti poseban opseg.

---

### 4) Kandidati za `const` / `static` (stil/perf; *inconclusive*)
- **`const` kandidati (član-funkcije ne menjaju stanje):**  
  `Cell::DisplayCell`, `Cell::IsUnlocked`, `Cell::IsFlagged`, `Cell::CheckForZero`, `Cell::CheckIfBomb`,  
  `Board::GetCellsOpened`, `Board::GetBombClick`.
- **`static` kandidat:** `Minesweeper::Instructions`.
- **Napomena:** primeniti samo ako ne narušavaju postojeći dizajn / I/O.
- **Cppcheck tagovi:** `functionConst`, `functionStatic` (*inconclusive*).

---

### 5) Informativno
- Poruka: **“Limiting analysis of branches… Use `--check-level=exhaustive` …”**  
  Podseća da je moguća preciznija analiza grananja uz veću cenu vremena (što je urađeno u improved koraku).

---

### Prioriteti (preporučeni redosled sanacije)
1. **Inicijalizacija člana** – u konstruktoru eksplicitno postaviti `boardGenerated = false;`.
2. **Rule of 3/5** – eksplicitno zabraniti kopiranje (`delete`) ili pravilno implementirati kopirne/kretačke operacije.
3. **Shadowing** – preimenovati unutrašnji indeks/varijablu radi jasnoće.
4. **`const`/`static`** – primeniti gde je bezbedno; poboljšava čitljivost i nameru koda.

---

### Rezultat
- Analiza nije pronašla kritične greške koje bi sprečile rad, ali je uočila neinicijalizovan član, potencijalne probleme u upravljanju resursima (Rule of 3/5), shadowing i niz stil/performance sugestija (const, static). Predložene ispravke poboljšavaju stabilnost i održivost koda bez narušavanja funkcionalnosti.

---

## 3) Valgrind **Callgrind**

### Kratak opis alata
**Callgrind** je alat koji generiše listu poziva funkcija korisničkog programa u vidu grafa. Sakupljeni podaci sastoje se od broja izvršenih instrukcija, njihovog odnosa sa linijom u izvršnom kodu, odnosa pozivalac/pozvan između funkcija, kao i broja takvih poziva. Ove informacije mogu biti korisne u procesu optimizacije programa jer je moguće uočiti delove koda koji se najviše izvršavaju i troše najviše resursa.

Dobijeni izlaz (fajl `callgrind.out.<pid>`) najlakše se analizira u **KCachegrind**-u: nudi listu funkcija sa *inclusive/exclusive* troškovima, graf poziva i mapiranje na linije koda.

---

### Reprodukcija i pokretanje (WSL/Ubuntu)

> Pretpostavka: projekat je već izgrađen u WSL.

1. **Pokretanje**
   ```bash
   sudo apt-get update
   sudo apt-get install -y valgrind kcachegrind
   ```
   - Re-build u WSL-u
     ```bash
     mkdir -p build-wsl && cd build-wsl
     cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ..
     cmake --build . -j
     ```
   - Skripta za pokretanje
     ```bash
     # učini skriptu izvršnom (prvi put)
     chmod +x valgrind/callgrind/run_callgrind.sh
     
     # pokreni kolekciju profila (pravi callgrind.out.N fajlove, .txt izveštaje)
     ./valgrind/callgrind/run_callgrind.sh
     ```
   - Otvaranje u KCachegrind-u (vizuelna analiza)
     ```bash
     kcachegrind valgrind/callgrind/callgrind.out.test &
     ```

 ---

 ### Rezultat

- **Dominantan *inclusive* trošak** potiče iz logike **otvaranja susednih polja (flood-fill)**, koja se rekurzivno/kaskadno aktivira pri otvaranju ćelija sa nulom.  
  ↳ U **KCachegrind**-u se to vidi kroz visoke **Incl.** procente za funkcije poput `Board::OpenAdjacentCells` (ili ekvivalentne) i za lanac poziva koje one generišu.

- **Generisanje table** (`Board::GenerateBoard` + postavljanje mina/inkrement suseda) je **instrukcijski intenzivno**, ali se izvršava ređe (tipično **1× po rundi/testu**), pa u dužim sesijama doprinosi manjem udelu **Ir**.

- **Trošak test-harness-a (Catch2 & stdlib)** postoji, ali je **smanjen**:
  - korišćenjem test filtera `~"Minesweeper"` za fokusiranje kritičnih testova,
  - *(opciono)* uz `--collect-atstart=no` i `--toggle-collect=...` dodatno se filtrira inicijalizacija Catch2 i fokusira samo „run“ deo.

- **Keš/grana metrike** iz `callgrind_cache_report.txt` su u skladu sa očekivanjima za ovakav kod:
  - **Ir** dominira kao glavna mera, **I1/D1/LL** promašaji i **branch** promašaji ostaju **niski** (uz datu bazu testova i kraće scenarije).

- **Potencijalne optimizacije (po potrebi):**
  - preispitati implementaciju **flood-fill** (rekurzija vs. queue/BFS; izbegavanje duplih poseta),
  - smanjiti **alokacije/re-alokacije** u vrućim stazama (pre-alokacija pomoćnih struktura),
  - ukloniti suvišna **grananja** i **kopiranja** u „vrućim“ petljama.

---

## 4) Fuzz testiranje (libFuzzer + AddressSanitizer)

### Kratak opis alata
*Fuzzing* je tehnika dinamičkog testiranja koja automatski generiše veliki broj polu-nasumičnih ulaza i poziva ciljnu funkciju, tražeći rušenja, UB i greške u upravljanju memorijom.

### Pristup u projektu
- Napisan je mali **fuzz target** (`fuzz/fuzz_target.cpp`) koji **mapira bajtove ulaza** u poteze nad klasom `Board` (otvaranje/markiranje ćelija).  
- Fuzzer pokreće `Board` u petlji sa različitim ulazima, dok **ASan** nadgleda (de)alokacije, out-of-bounds i UB.  
- Korišćen je „bez-UI“ scenario (nema interakcije sa konzolom), fokus na *core* logiku `Board`.

### Rezultat

- **Otkrivena greška (ASan):** neusaglašeno oslobađanje memorije (*alloc–dealloc mismatch*) u `Board::~Board()`.
  - **Uzrok:** nizovi alocirani sa `new[]` oslobađani su `delete` umesto `delete[]`.
  - **Evidencija:** AddressSanitizer stack-trace u `fuzz/logs/run_*.log` i `fuzz/logs/repro_*.log` ukazuje na alokaciju u `Board::Board(int,int)` i dealokaciju u `Board::~Board()`.

- **Reprodukovanje problema:**  
  Fajl sa ulazom koji izaziva pad čuva se u `fuzz/crashes/…`. Reprodukcija je deterministička — pokretanje fuzz targeta sa tim ulazom uvek generiše istu grešku.

- **Minimizacija ulaza:**  
  Problematičan ulaz je minimizovan, tako da sadrži samo mali broj bajtova neophodnih da se aktivira greška. Ovo olakšava analizu i služi kao regresioni input.

- **Predložene korekcije (u `patches/changes.patch`):**
  - Ispraviti dealokaciju: koristiti `delete[]` za elemente i za `arr` (usaglašeno sa `new[]`).
  - Deterministički inicijalizovati član `boardGenerated = false;` u konstruktoru (sprečava potencijalno nedefinisano ponašanje).
  - Onemogućiti kopiranje `Board` (Rule of 3/5): deklarisati `Board(const Board&) = delete;` i `Board& operator=(const Board&) = delete;` zbog ručnog upravljanja resursima (`Cell** arr`).

- **Uticaj i rizik:**  
  Neusaglašeno oslobađanje memorije može dovesti do rušenja, korupcije memorije i nepredvidivog ponašanja, naročito pod opterećenjem ili u budućim izmenama koda.

- **Nakon primene popravke (očekivano):**
  - Eliminisanje pada na istom ulazu (pozitivna regresiona provera).
  - Poboljšana pouzdanost i održivost koda pri nasumičnim sekvencama poteza.

Fuzzing je **otkrio realnan problem** u upravljanju memorijom (neusaglašeno oslobađanje niza), koju tipično nije lako uhvatiti klasičnim testovima. Nalaz je reproducibilan i minimizovan. Priložen je patch male površine promene i nakon primene predložene popravke, ponovno pokretanje fuzz testa očekivano uklanja ASan upozorenje.






