#include <catch2/catch_all.hpp>
#include <vector> 
#include "Header.h"

// Testira konstrukciju "Game Manager"-a i osnovne metode koje ne zahtevaju interakciju.

TEST_CASE("Minesweeper constructs and shows instructions", "[interactive]") {
    Minesweeper game;

    // Ove metode podležu I/O-u; proveravamo samo da ne pucaju.
    REQUIRE_NOTHROW(game.Instructions());
}

TEST_CASE("Minesweeper high-level flow calls do not crash", "[interactive]") {
    Minesweeper game;

    // SetDifficulty i CreateBoard često traže ulaz korisnika.
    // Ako u tvojoj verziji čitaju sa std::cin, ovaj test može biti NOP ili će čekati unos.
    // Zato ih ne pozivamo ovde — fokusiramo se na non-blocking delove.

    // CheckWin obično proverava stanje; trebalo bi da je bezbedno pozvati ga pre igre.
    REQUIRE_NOTHROW(game.CheckWin());
}
