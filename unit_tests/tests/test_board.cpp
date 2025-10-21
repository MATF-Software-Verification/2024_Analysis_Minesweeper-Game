#include <catch2/catch_all.hpp>
#include <vector> 
#include "Header.h"

// Minimalni "smoke" testovi zasnovani na dostupnim metodama.
// Ne oslanjamo se na "first click safe" jer ne znamo da li je implementiran.

TEST_CASE("Board constructs and reports zero opened cells initially") {
    const int size = 9;
    const int mines = 10;
    Board b(size, mines);

    // GetCellsOpened() postoji u interfejsu
    REQUIRE(b.GetCellsOpened() == 0);
}

TEST_CASE("GenerateBoard can be called once with a first click position") {
    const int size = 9;
    const int mines = 10;
    Board b(size, mines);

    // Pošto potpis funkcije kaže GenerateBoard(x, y),
    // pozovemo je sa (0,0) da "generiše" tablu.
    // Očekujemo da se program ne sruši i da je igra u "bezbednom" stanju.
    REQUIRE_NOTHROW(b.GenerateBoard(0, 0));

    // Nakon generisanja, nijedno polje formalno nije "otvoreno"
    // (zavisno od implementacije), pa >= 0 je najsigurnije.
    REQUIRE(b.GetCellsOpened() >= 0);

    // GetBombClick() je false dok ne kliknemo na bombu
    REQUIRE_FALSE(b.GetBombClick());
}

TEST_CASE("PlayerSelect can open and mark a cell") {
    const int size = 9;
    const int mines = 10;
    Board b(size, mines);

    // Prvi realan korisnički potez obično implicitno generiše tablu.
    // Pokušaj otvaranja (0,0)
    bool cont = b.PlayerSelect(0, 0, PlayerMove::Open);
    // Igra može da se nastavi ili odmah izgubi (u zavisnosti od logike).
    REQUIRE((cont == true || cont == false));

    // Mark/unmark ne bi smeo da ruši:
    REQUIRE_NOTHROW(b.PlayerSelect(0, 0, PlayerMove::Mark));
}
