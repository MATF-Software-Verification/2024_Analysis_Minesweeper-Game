#include <cstdint>
#include <cstddef>
#include "Header.h"

// Fuzzer mapira bajtove u poteze nad Board-om.
// Cilj: nema rušenja/UB; ASan će prijaviti probleme.

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 2) return 0;

    const int S = EASY_SIZE;   // 9
    const int M = EASY_MINES;  // 10

    Board b(S, M);

    // prvi potez: najčešće generiše tablu
    int x0 = data[0] % S;
    int y0 = data[1] % S;
    b.PlayerSelect(x0, y0, PlayerMove::Open);

    // ostali bajtovi: (x,y,akcija)
    for (size_t i = 2; i + 2 < size; i += 3) {
        int x = data[i]   % S;
        int y = data[i+1] % S;
        PlayerMove mv = (data[i+2] & 1) ? PlayerMove::Mark : PlayerMove::Open;
        b.PlayerSelect(x, y, mv);
        if (b.GetBombClick()) break; // ako smo kliknuli bombu, prekini rundu
    }
    return 0;
}
