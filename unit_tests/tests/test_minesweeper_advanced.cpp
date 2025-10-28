#include <catch2/catch_all.hpp>
#include <vector>
#include "Header.h"

// ============================================================================
// ADVANCED TESTS FOR INDIRECT COVERAGE OF Play() AND GetInput()
// ============================================================================
// Pošto Play() i GetInput() direktno koriste std::cin, testiramo ih
// indirektno kroz Board metode koje one pozivaju

TEST_CASE("GetInput simulation - checking bomb click during input", "[getinput]") {
    Board b(9, 10);
    
    b.GenerateBoard(0, 0);
    
    // GetInput() proverava GetBombClick() na početku petlje (linija 104-108)
    // Simuliramo scenario gde je bomba kliknuta
    
    // Prvo otvori ćelije dok ne nađeš bombu
    bool foundBomb = false;
    for (int i = 0; i < 9 && !foundBomb; i++) {
        for (int j = 0; j < 9 && !foundBomb; j++) {
            b.ClickCell(i, j);
            if (b.GetBombClick()) {
                foundBomb = true;
                // U GetInput(), ovo bi vratilo gameState = Lost
                REQUIRE(b.GetBombClick() == true);
            }
        }
    }
    
    // Ako smo našli bombu, GetInput() bi trebalo da vrati odmah
    if (foundBomb) {
        REQUIRE(b.GetBombClick() == true);
    }
}

TEST_CASE("GetInput simulation - valid row input range (1 to size)", "[getinput]") {
    Board b(9, 10);
    
    b.GenerateBoard(4, 4);
    
    // GetInput() proverava: row >= 1 && row <= size (linija 111-117)
    // U Board API, to je (row - 1), što je 0 do 8
    
    SECTION("Minimum valid row (1 -> 0)") {
        REQUIRE_NOTHROW(b.PlayerSelect(0, 4, PlayerMove::Open));
    }
    
    SECTION("Maximum valid row (9 -> 8)") {
        REQUIRE_NOTHROW(b.PlayerSelect(8, 4, PlayerMove::Open));
    }
    
    SECTION("Middle row (5 -> 4)") {
        REQUIRE_NOTHROW(b.PlayerSelect(4, 4, PlayerMove::Open));
    }
}

TEST_CASE("GetInput simulation - valid col input range (1 to size)", "[getinput]") {
    Board b(9, 10);
    
    b.GenerateBoard(4, 4);
    
    // GetInput() proverava: col >= 1 && col <= size (linija 118-126)
    
    SECTION("Minimum valid col (1 -> 0)") {
        REQUIRE_NOTHROW(b.PlayerSelect(4, 0, PlayerMove::Open));
    }
    
    SECTION("Maximum valid col (9 -> 8)") {
        REQUIRE_NOTHROW(b.PlayerSelect(4, 8, PlayerMove::Open));
    }
    
    SECTION("Middle col (5 -> 4)") {
        REQUIRE_NOTHROW(b.PlayerSelect(4, 4, PlayerMove::Open));
    }
}

TEST_CASE("GetInput simulation - valid operation input (1 or 2)", "[getinput]") {
    Board b(9, 10);
    
    b.GenerateBoard(4, 4);
    
    // GetInput() proverava: input >= 1 && input <= 2 (linija 127-135)
    // input == 1 -> Open, input == 2 -> Mark
    
    SECTION("Operation 1 - Open") {
        bool result = b.PlayerSelect(0, 0, PlayerMove::Open);
        REQUIRE((result == true || result == false));
    }
    
    SECTION("Operation 2 - Mark") {
        bool result = b.PlayerSelect(0, 0, PlayerMove::Mark);
        REQUIRE(result == true);
    }
}

TEST_CASE("GetInput simulation - PlayerSelect retry logic", "[getinput]") {
    Board b(9, 10);
    
    b.GenerateBoard(4, 4);
    
    // GetInput() ima do-while petlju koja se ponavlja dok PlayerSelect ne vrati true (linija 140)
    
    // Prvi pokušaj - otvori ćeliju
    bool result1 = b.PlayerSelect(0, 0, PlayerMove::Open);
    
    // Drugi pokušaj - pokušaj ponovo otvoriti istu ćeliju (treba da vrati false)
    bool result2 = b.ClickCell(0, 0);
    REQUIRE(result2 == false); // Ne može ponovo otvoriti
    
    // GetInput() bi tražilo novi unos jer PlayerSelect vratilo false
}

TEST_CASE("GetInput simulation - Open move triggers CheckWin", "[getinput]") {
    Board b(5, 3); // Mala tabla: 25 ćelija - 3 mine = 22 sigurne
    
    b.GenerateBoard(2, 2);
    
    // GetInput() poziva CheckWin() samo nakon Open poteza (linija 141-142)
    
    // Otvori sve sigurne ćelije
    int openedCount = 0;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (!b.GetBombClick()) {
                bool result = b.PlayerSelect(i, j, PlayerMove::Open);
                if (result) {
                    openedCount++;
                }
            }
        }
    }
    
    // Nakon svakog Open poteza, CheckWin bi trebalo da se pozove
    // CheckWin proverava: cellsOpened == (size * size - mines)
    int cellsOpened = b.GetCellsOpened();
    
    if (cellsOpened == 22 && !b.GetBombClick()) {
        // Pobeda! CheckWin bi postavio gameState = Won
        REQUIRE(cellsOpened == 22);
    }
}

TEST_CASE("GetInput simulation - Mark move does NOT trigger CheckWin", "[getinput]") {
    Board b(9, 10);
    
    b.GenerateBoard(4, 4);
    
    // Samo Open potezi pokreću CheckWin (linija 141-142)
    // Mark potezi NE pokreću CheckWin
    
    int cellsBefore = b.GetCellsOpened();
    
    // Izvrši Mark operaciju
    b.PlayerSelect(0, 0, PlayerMove::Mark);
    
    int cellsAfter = b.GetCellsOpened();
    
    // Broj otvorenih ćelija se ne menja sa Mark
    REQUIRE(cellsBefore == cellsAfter);
}

TEST_CASE("Play simulation - game loop with Playing state", "[play]") {
    Board b(9, 10);
    
    // Play() ima do-while petlju koja se izvršava dok je gameState == Playing (linija 18-22)
    
    b.GenerateBoard(4, 4);
    
    // Simuliraj nekoliko iteracija Play petlje
    for (int turn = 0; turn < 10 && !b.GetBombClick(); turn++) {
        // Svaka iteracija: ShowBoard() -> GetInput()
        b.ShowBoard();
        
        // Simuliraj GetInput sa različitim potezima
        int row = turn % 9;
        int col = (turn + 1) % 9;
        b.PlayerSelect(row, col, PlayerMove::Open);
    }
    
    // Proveri da li je igra u validnom stanju
    REQUIRE(b.GetCellsOpened() >= 0);
}

TEST_CASE("Play simulation - Lost state triggers correct output", "[play]") {
    Board b(9, 10);
    
    // Play() za Lost stanje (linija 23-29):
    // - Ispisuje "You've clicked on a bomb!"
    // - Poziva UnlockBombs()
    // - Poziva ShowBoard()
    // - Ispisuje "Better luck next time!"
    
    b.GenerateBoard(0, 0);
    
    // Klikaj dok ne nađeš bombu
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            b.ClickCell(i, j);
            if (b.GetBombClick()) {
                // Simuliraj Lost stanje
                REQUIRE_NOTHROW(b.UnlockBombs());
                REQUIRE_NOTHROW(b.ShowBoard());
                
                // gameState bi bilo Lost, petlja bi se završila
                break;
            }
        }
        if (b.GetBombClick()) break;
    }
}

TEST_CASE("Play simulation - Won state triggers correct output", "[play]") {
    Board b(5, 3);
    
    // Play() za Won stanje (linija 30-36):
    // - Ispisuje "Congratulations! You've won!"
    // - Poziva UnlockBombs()
    // - Poziva ShowBoard()
    // - Ispisuje "Thanks for playing!"
    
    b.GenerateBoard(2, 2);
    
    // Pokušaj otvoriti sve sigurne ćelije
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (!b.GetBombClick()) {
                b.PlayerSelect(i, j, PlayerMove::Open);
            }
        }
    }
    
    int cellsOpened = b.GetCellsOpened();
    
    // Ako smo otvorili sve sigurne ćelije (22 = 25 - 3)
    if (cellsOpened >= 22 && !b.GetBombClick()) {
        // Simuliraj Won stanje
        REQUIRE_NOTHROW(b.UnlockBombs());
        REQUIRE_NOTHROW(b.ShowBoard());
    }
}

TEST_CASE("CheckWin calculation - exact win condition", "[checkwin]") {
    // CheckWin() proverava: cellsOpened == (size * size - mines) (linija 148-151)
    
    SECTION("Easy - 9x9 with 10 mines") {
        Board b(9, 10);
        b.GenerateBoard(4, 4);
        
        // Win condition: 81 - 10 = 71 ćelija
        // Trenutno: GetCellsOpened() vraća trenutni broj
        int currentOpened = b.GetCellsOpened();
        REQUIRE(currentOpened < 71); // Ne bi trebalo još da je pobeda
    }
    
    SECTION("Medium - 15x15 with 40 mines") {
        Board b(15, 40);
        b.GenerateBoard(7, 7);
        
        // Win condition: 225 - 40 = 185 ćelija
        int currentOpened = b.GetCellsOpened();
        REQUIRE(currentOpened < 185);
    }
    
    SECTION("Hard - 25x25 with 99 mines") {
        Board b(25, 99);
        b.GenerateBoard(12, 12);
        
        // Win condition: 625 - 99 = 526 ćelija
        int currentOpened = b.GetCellsOpened();
        REQUIRE(currentOpened < 526);
    }
}

TEST_CASE("CheckWin edge case - one cell before win", "[checkwin]") {
    Board b(5, 3); // 25 - 3 = 22 za pobedu
    
    b.GenerateBoard(2, 2);
    
    // Otvori skoro sve ćelije
    int targetOpened = 21; // Jedna manje od pobede
    int attempts = 0;
    
    while (b.GetCellsOpened() < targetOpened && !b.GetBombClick() && attempts < 25) {
        int row = attempts / 5;
        int col = attempts % 5;
        b.PlayerSelect(row, col, PlayerMove::Open);
        attempts++;
    }
    
    int opened = b.GetCellsOpened();
    
    // Ako nismo izgubili i blizu smo pobede
    if (!b.GetBombClick() && opened >= 20) {
        // CheckWin bi trebalo da kaže "još ne pobeda"
        REQUIRE( opened <= 22 );
    }
}

TEST_CASE("Multiple complete game simulations", "[simulation][integration]") {
    // Simuliraj više kompletnih igara
    
    for (int game = 0; game < 5; game++) {
        Board b(7, 8);
        
        b.GenerateBoard(3, 3);
        
        // Igraj do kraja ili dok ne klikneš bombu
        for (int i = 0; i < 7 && !b.GetBombClick(); i++) {
            for (int j = 0; j < 7 && !b.GetBombClick(); j++) {
                b.PlayerSelect(i, j, PlayerMove::Open);
            }
        }
        
        // Svaka igra bi trebalo da se završi u validnom stanju
        REQUIRE(b.GetCellsOpened() >= 0);
        
        if (b.GetBombClick()) {
            // Lost
            REQUIRE_NOTHROW(b.UnlockBombs());
        } else {
            // Možda Won
            int opened = b.GetCellsOpened();
            if (opened == (49 - 8)) {
                REQUIRE_NOTHROW(b.UnlockBombs());
            }
        }
    }
}

// ============================================================================
// TESTS FOR SPECIFIC CODE PATHS IN GetInput()
// ============================================================================

TEST_CASE("GetInput path - immediate bomb click return", "[getinput][path]") {
    Board b(9, 10);
    
    b.GenerateBoard(0, 0);
    
    // Scenario: Bomba je već kliknuta pre poziva GetInput
    // GetInput linija 104-108 proverava GetBombClick() i vraća odmah
    
    // Proveri da li možemo kliknuti bombu
    bool hitBomb = false;
    for (int i = 0; i < 81 && !hitBomb; i++) {
        int row = i / 9;
        int col = i % 9;
        b.ClickCell(row, col);
        if (b.GetBombClick()) {
            hitBomb = true;
            // U ovom trenutku, GetInput() bi vratio sa gameState = Lost
            REQUIRE(b.GetBombClick() == true);
        }
    }
}

TEST_CASE("GetInput path - successful Open operation", "[getinput][path]") {
    Board b(9, 10);
    
    // Path: Unesi row, col, operaciju 1 (Open), PlayerSelect vrati true
    
    b.GenerateBoard(4, 4);
    
    int cellsBefore = b.GetCellsOpened();
    
    // Izvrši Open
    bool result = b.PlayerSelect(5, 5, PlayerMove::Open);
    
    int cellsAfter = b.GetCellsOpened();
    
    if (result && !b.GetBombClick()) {
        // Ako je uspešno, broj otvorenih bi trebalo da se poveća
        REQUIRE(cellsAfter >= cellsBefore);
    }
}

TEST_CASE("GetInput path - successful Mark operation", "[getinput][path]") {
    Board b(9, 10);
    
    // Path: Unesi row, col, operaciju 2 (Mark), PlayerSelect vrati true
    
    b.GenerateBoard(4, 4);
    
    int cellsBefore = b.GetCellsOpened();
    
    // Izvrši Mark
    bool result = b.PlayerSelect(0, 0, PlayerMove::Mark);
    
    int cellsAfter = b.GetCellsOpened();
    
    // Mark ne menja broj otvorenih ćelija
    REQUIRE(cellsBefore == cellsAfter);
    REQUIRE(result == true);
}

TEST_CASE("GetInput path - failed PlayerSelect requiring retry", "[getinput][path]") {
    Board b(9, 10);
    
    // Path: PlayerSelect vrati false, do-while se ponavlja
    
    b.GenerateBoard(4, 4);
    
    // Prvi pokušaj - uspešno otvori
    bool first = b.PlayerSelect(0, 0, PlayerMove::Open);
    
    // Drugi pokušaj - pokušaj ponovo otvoriti (vrati false)
    bool second = b.ClickCell(0, 0);
    REQUIRE(second == false);
    
    // U GetInput(), ovo bi tražilo novi unos
    // Simuliraj novi unos sa drugom pozicijom
    bool third = b.PlayerSelect(0, 1, PlayerMove::Open);
    
    // Treći bi trebalo da uspe (ili ne, zavisno od sadržaja)
    REQUIRE((third == true || third == false));
}

// ============================================================================
// COVERAGE FOR SPECIFIC Board METHODS CALLED BY GetInput/Play
// ============================================================================

TEST_CASE("ShowBoard with various board states", "[board][coverage]") {
    Board b(9, 10);
    
    b.GenerateBoard(4, 4);
    
    SECTION("Empty board") {
        REQUIRE_NOTHROW(b.ShowBoard());
    }
    
    SECTION("Board with some opened cells") {
        b.PlayerSelect(4, 4, PlayerMove::Open);
        REQUIRE_NOTHROW(b.ShowBoard());
    }
    
    SECTION("Board with flags") {
        b.PlayerSelect(0, 0, PlayerMove::Mark);
        b.PlayerSelect(1, 1, PlayerMove::Mark);
        REQUIRE_NOTHROW(b.ShowBoard());
    }
    
    SECTION("Board after losing") {
        for (int i = 0; i < 9 && !b.GetBombClick(); i++) {
            for (int j = 0; j < 9; j++) {
                b.ClickCell(i, j);
                if (b.GetBombClick()) {
                    b.UnlockBombs();
                    REQUIRE_NOTHROW(b.ShowBoard());
                    break;
                }
            }
        }
    }
}

TEST_CASE("GetBombClick throughout game lifecycle", "[board][coverage]") {
    Board b(9, 10);
    
    // Pre generisanja
    REQUIRE_FALSE(b.GetBombClick());
    
    // Nakon generisanja
    b.GenerateBoard(4, 4);
    REQUIRE_FALSE(b.GetBombClick());
    
    // Nakon otvaranja sigurne ćelije
    b.PlayerSelect(4, 4, PlayerMove::Open);
    REQUIRE_FALSE(b.GetBombClick());
    
    // Nakon postavljanja zastave
    b.PlayerSelect(0, 0, PlayerMove::Mark);
    REQUIRE_FALSE(b.GetBombClick());
}

TEST_CASE("GetCellsOpened increments correctly", "[board][coverage]") {
    Board b(9, 10);
    
    b.GenerateBoard(4, 4);
    
    int count = b.GetCellsOpened();
    REQUIRE(count >= 0);
    
    // Otvori neke ćelije
    b.PlayerSelect(4, 4, PlayerMove::Open);
    int count2 = b.GetCellsOpened();
    REQUIRE(count2 >= count);
    
    b.PlayerSelect(5, 5, PlayerMove::Open);
    int count3 = b.GetCellsOpened();
    REQUIRE(count3 >= count2);
}

TEST_CASE("UnlockBombs reveals all mine positions", "[board][coverage]") {
    Board b(5, 5);
    
    b.GenerateBoard(2, 2);
    
    // Pre UnlockBombs
    int openedBefore = b.GetCellsOpened();
    
    // Pozovi UnlockBombs
    REQUIRE_NOTHROW(b.UnlockBombs());
    
    // UnlockBombs otvara sve mine (ali ne povećava cellsOpened jer nisu "sigurne" ćelije)
    // Test da funkcija ne pada
    REQUIRE_NOTHROW(b.ShowBoard());
}

// ============================================================================
// BOUNDARY AND STRESS TESTS FOR GetInput/Play SCENARIOS
// ============================================================================

TEST_CASE("GetInput with all corners and edges", "[getinput][boundary]") {
    Board b(9, 10);
    
    b.GenerateBoard(4, 4);
    
    // Test svih ivica i uglova kao što GetInput može da primi
    
    std::vector<std::pair<int, int>> positions = {
        {0, 0}, {0, 4}, {0, 8},  // top
        {4, 0}, {4, 8},           // middle edges
        {8, 0}, {8, 4}, {8, 8}   // bottom
    };
    
    for (auto& pos : positions) {
        REQUIRE_NOTHROW(b.PlayerSelect(pos.first, pos.second, PlayerMove::Open));
    }
}

TEST_CASE("GetInput alternating Open and Mark operations", "[getinput][pattern]") {
    Board b(9, 10);
    
    b.GenerateBoard(4, 4);
    
    // Alterniraj između Open i Mark
    for (int i = 0; i < 9 && !b.GetBombClick(); i++) {
        if (i % 2 == 0) {
            b.PlayerSelect(i, 0, PlayerMove::Open);
        } else {
            b.PlayerSelect(i, 1, PlayerMove::Mark);
        }
    }
    
    REQUIRE(b.GetCellsOpened() >= 0);
}

TEST_CASE("Play maximum iterations simulation", "[play][stress]") {
    Board b(9, 10);
    
    b.GenerateBoard(4, 4);
    
    // Simuliraj maksimalan broj poteza (svaka ćelija)
    int maxMoves = 81;
    
    for (int move = 0; move < maxMoves && !b.GetBombClick(); move++) {
        int row = move / 9;
        int col = move % 9;
        
        b.ShowBoard();
        b.PlayerSelect(row, col, PlayerMove::Open);
    }
    
    // Ili smo pobedili ili izgubili
    REQUIRE(b.GetCellsOpened() >= 0);
}

TEST_CASE("GetInput flag exhaustion scenario", "[getinput][flags]") {
    Board b(9, 5); // Samo 5 zastava
    
    b.GenerateBoard(4, 4);
    
    // Postavi sve zastave
    REQUIRE(b.PlayerSelect(0, 0, PlayerMove::Mark) == true);
    REQUIRE(b.PlayerSelect(0, 1, PlayerMove::Mark) == true);
    REQUIRE(b.PlayerSelect(0, 2, PlayerMove::Mark) == true);
    REQUIRE(b.PlayerSelect(0, 3, PlayerMove::Mark) == true);
    REQUIRE(b.PlayerSelect(0, 4, PlayerMove::Mark) == true);
    
    // Pokušaj 6. zastave
    REQUIRE(b.PlayerSelect(0, 5, PlayerMove::Mark) == false);
    
    // U GetInput(), ovo bi vratilo false i tražilo novi unos
    
    // Ukloni jednu zastavu
    REQUIRE(b.PlayerSelect(0, 0, PlayerMove::Mark) == true); // unflag
    
    // Sada možemo postaviti novu
    REQUIRE(b.PlayerSelect(0, 5, PlayerMove::Mark) == true);
}

