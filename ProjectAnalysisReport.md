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

## 1. Testiranje jedinica koda (Catch2) + pokrivenost (OpenCppCoverage)

### Kratak opis alata

- **Catch2** – C++ framework za jedinično testiranje. Testovi se pišu kao obični `.cpp` fajlovi (bez posebnog runner-a), a izvršni fajl testova generiše Catch2 sam (main funkcija je obezbeđena preko zaglavlja/biblioteke).
- **OpenCppCoverage (Windows)** – alat za merenje pokrivenosti C/C++ koda na Windows-u. Pokreće zadati test izvršni fajl i generiše čitljiv **HTML izveštaj** sa procentima pokrivenosti po fajlovima/linijama.

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

## 2. Statička analiza (Cppcheck)

### Kratak opis alata

- **Cppcheck** – alat za statičku analizu C/C++ kodova, koji otkriva probleme koje kompajler često ne prijavi (npr. neinicijalizovane promenljive, potencijalne greške sa pokazivačima, pogrešne idiome upravljanja resursima, stil/perf upozorenja).  

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

### 2) Rule of 3/5 upozorenje (upravljanje resursima)
- **Problem:** Klasa `Board` upravlja dinamičkom memorijom (`Cell** arr`), a nema definisan **copy constructor** ni **operator=**.
- **Rizik:** plitko kopiranje može dovesti do dvostrukog oslobađanja resursa ili drugih problema s upravljanjem memorijom. Čak i u odsustvu trenutnih posledica, ovakva implementacija predstavlja indikator lošeg dizajna (code smell).
- **Cppcheck tagovi:** `noCopyConstructor`, `noOperatorEq`.
- **Predlog:** Eksplicitno zabraniti kopiranje (`delete`) ili implementirati semantiku kopiranja/pomeranja (Rule of 3/5).

### 3) Sakrivanje promenljive (shadowing)
- **Mesto:** `Board::GenerateBoard(...)`
- **Opis:** Unutrašnja petlja `for (int i = 0; i < xCord.size(); i++)` zasenjuje (shadow) spoljni `i`.
- **Efekat:** pogoršana čitljivost, rizik od skrivenih grešaka pri izmenama.
- **Cppcheck tag:** `shadowVariable`.
- **Predlog:** Preimenovati unutrašnji brojač (`j`, `k`…) ili koristiti poseban opseg.

### 4) Kandidati za `const` / `static` (stil/perf; *inconclusive*)
- **`const` kandidati (član-funkcije ne menjaju stanje):**  
  `Cell::DisplayCell`, `Cell::IsUnlocked`, `Cell::IsFlagged`, `Cell::CheckForZero`, `Cell::CheckIfBomb`,  
  `Board::GetCellsOpened`, `Board::GetBombClick`.
- **`static` kandidat:** `Minesweeper::Instructions`.
- **Napomena:** primeniti samo ako ne narušavaju postojeći dizajn / I/O.
- **Cppcheck tagovi:** `functionConst`, `functionStatic` (*inconclusive*).

### 5) Informativno
- Poruka: **“Limiting analysis of branches… Use `--check-level=exhaustive` …”**  
  Podseća da je moguća preciznija analiza grananja uz veću cenu vremena (što je urađeno u improved koraku).

### Moguća poboljšanja
1. **Inicijalizacija člana** – u konstruktoru eksplicitno postaviti `boardGenerated = false;`.
2. **Rule of 3/5** – eksplicitno zabraniti kopiranje (`delete`) ili pravilno implementirati operacije za kopiranje/pomeranje.
3. **Shadowing** – preimenovati unutrašnji indeks/varijablu radi jasnoće.
4. **`const`/`static`** – primeniti gde je bezbedno; poboljšava čitljivost i nameru koda.


### Rezultat
- Analiza nije pronašla kritične greške koje bi sprečile rad, ali je uočila neinicijalizovani član, potencijalne probleme u upravljanju resursima (Rule of 3/5), shadowing i niz stil/performance sugestija (const, static). Predložene ispravke poboljšavaju stabilnost i održivost koda bez narušavanja funkcionalnosti.

---

## 3. Valgrind **Callgrind**

### Kratak opis alata
**Valgrind** platforma sadrži alate koji se koriste za profajliranje.<br/>
**Callgrind** je alat koji generiše listu poziva funkcija korisničkog programa u vidu grafa. Sakupljeni podaci sastoje se od broja izvršenih instrukcija, njihovog odnosa sa linijom u izvršnom kodu, odnosa pozivalac/pozvan između funkcija, kao i broja takvih poziva. Ove informacije mogu biti korisne u procesu optimizacije programa jer je moguće uočiti delove koda koji se najčešće izvršavaju i troše najviše resursa.

Dobijeni izlaz najlakše se analizira u **KCachegrind**-u: nudi listu funkcija sa *inclusive/exclusive* troškovima, graf poziva i mapiranje na linije koda.


### Pokretanje i reprodukcija (WSL/Ubuntu)

> Pretpostavka: **WSL/Ubuntu** je instaliran na Windows-u

   - Instalacija
     ```bash
     sudo apt-get update
     sudo apt-get install -y valgrind kcachegrind
     ```
   - build u WSL-u
     ```bash
     mkdir -p build-wsl && cd build-wsl
     cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ..
     cmake --build . -j
     ```
   - Pokretanje
     ```bash
     valgrind --tool=callgrind \
     --cache-sim=yes --branch-sim=yes \
     --callgrind-out-file="valgrind/callgrind/callgrind.out.test" \
     "./build-wsl/bin/ms_tests" '~"Minesweeper"'
     ```
   - Otvaranje u KCachegrind-u (vizuelna analiza)
     ```bash
     kcachegrind valgrind/callgrind/callgrind.out.test &
     ```

 ### Rezultat

Na sledećoj slici možemo videti deo podataka iz alata KCachegrind. 
Sa leve strane se nalazi lista funkcija koja se može sortirati na različite načine tako dobivši različite informacije. Kolona ```Incl.``` prikazuje procentualno ukupno vreme provedeno u funkciji i svim funkcijama koje ona poziva.

 <img width="1519" height="964" alt="kcachegrind_screenshot" src="https://github.com/user-attachments/assets/32c89abd-4d78-49e4-b4dc-9c24bd7cfd6f" />

- **Dominantan *inclusive* trošak** potiče iz logike **otvaranja susednih polja (flood-fill)**, koja se rekurzivno/kaskadno aktivira pri otvaranju ćelija sa nulom.  
  ↳ U **KCachegrind**-u se to vidi kroz visoke **Incl.** procente za funkcije poput `Board::OpenAdjacentCells` (ili ekvivalentne) i za lanac poziva koje one generišu.
  
  <img width="1911" height="915" alt="adjacent" src="https://github.com/user-attachments/assets/625c2f9c-32ae-4d43-bac9-1e10fa211018" />
  <img width="1499" height="835" alt="adjacent2" src="https://github.com/user-attachments/assets/d0b0ef35-3e06-40bf-9076-cae4183a97c6" />

- **Generisanje table** (`Board::GenerateBoard` + postavljanje mina/inkrement suseda) je **instrukcijski intenzivno**, ali se izvršava ređe (tipično **1× po rundi/testu**), pa u dužim sesijama doprinosi manjem udelu **Ir**.

  <img width="1510" height="720" alt="generateBoard" src="https://github.com/user-attachments/assets/f2851a0b-b748-4619-a121-a1ef311bf1aa" />


- **Trošak test-harness-a (Catch2 & stdlib)** postoji, ali je **smanjen**:
  - korišćenjem test filtera `~"Minesweeper"` za fokusiranje kritičnih testova,
  - *(opciono)* uz `--collect-atstart=no` i `--toggle-collect=...` dodatno se filtrira inicijalizacija Catch2 i fokusira samo „run“ deo.

- **Keš/grana metrike** iz `callgrind_cache_report.txt` su u skladu sa očekivanjima za ovakav kod:
  - **Ir** dominira kao glavna mera, **I1/D1/LL** promašaji i **branch** promašaji ostaju **niski**, što ukazuje da izračunavanje nije memorijski ograničeno. Stopa pogrešnih predikcija grana (~5%) je očekivana za ovakav kontrolni tok (flood-fill, grananja oko suseda).
  
   <img width="992" height="559" alt="callgrind-cache" src="https://github.com/user-attachments/assets/d414caa5-ebc4-4fcf-8c9e-2047132c6912" />

- **Moguća poboljšanja:**
  - Razmotriti drugačiju implementaciju otvaranja suseda (**flood-fill**): umesto rekurzije koristiti red/čekanje (queue/BFS), i voditi računa da se ista ćelija ne obilazi više puta.
  - Smanjiti broj **alokacija i realokacija** u delovima koda koji se često izvršavaju: npr. unapred alocirati pomoćne strukture (vektore/redove) i ponovo ih koristiti.
  - Pojednostaviti logiku **grananja** i izbegavati nepotrebna **kopiranja** podataka u petljama koje se često pokreću.

---

## 4) Fuzz testiranje (libFuzzer + AddressSanitizer)

### Kratak opis alata
*Fuzzing* je tehnika dinamičkog testiranja koja automatski generiše veliki broj polu-nasumičnih ulaza, sa ciljem da otkrije greške, padove, curenje memorije i ranjivosti.

### Pristup u projektu
- Napisan je **fuzz target** (`fuzz/fuzz_target.cpp`) koji **mapira bajtove ulaza** u poteze nad klasom `Board` (otvaranje/markiranje ćelija).  
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

---

## Zaključak
Analizom ovog projekta dolazi se do sledećih zapažanja:
- Osnovni tokovi igre (inicijalizacija table, generisanje, otvaranje/obeležavanje polja, provera stanja) rade stabilno pod jediničnim testovima; testovi prolaze bez padova i pokrivaju ključne javne metode
- Statička analiza je otkrila par realnih i više sugestija za stil i performanse. Najznačajnije je bilo neinicijalizovano stanje i upozorenje zbog ručne alokacije - to je adresirano inicijalizacijom članova i zabranom kopiranja ili prilagođenim uništavanjem. Ostale preporuke (const/static, shadowing) su niskog rizika, ali poboljšavaju čitljivost i održavanje.
- Instrukcijski trošak očekivano dominira u logici “flood-fill” otvaranja suseda, dok se generisanje table javlja ređe i proporcionalno manje utiče na ukupni Ir. Metrike keša i grana su u granicama očekivanog za ovakav, jednostavan rad sa matricom; nema indikacija ozbiljnih uskih grla. Postoje jasne tačke za fino podešavanje (BFS umesto rekurzije, prealokacija struktura, uklanjanje suvišnih kopiranja/grananja).
- Fuzzer je brzo otkrio stvaran problem neusaglašenog oslobađanja (new[]/delete), koji je korigovan (new[]/delete[]). Nakon ispravke, reprodukcija istog ulaza više ne prijavljuje grešku, što sugeriše da je uzrok uklonjen.

Konačni zaključak je da je projekat kvalitetan, sa dobro implementiranim funkcionalnostima i čitljivim kodom. Uz manja poboljšanja može da se dobije na robusnosti, testabilnosti i efikasnosti.





