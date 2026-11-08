/* arrtest.c - acces aux tableaux globaux (lectures, ecritures, echanges,
   table const en ROM, 2D, structures). Les sommes attendues sont calculees
   par perf/tests/arrtest/expected.py. */
#include <snes.h>
#include "expected.h"

u8 b8[300];
u16 w16[200];
const u16 tbl[16] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9, 3};
u16 grid[8][10];
typedef struct { s16 x, y; u8 f; } obj_t;
obj_t objs[20];

static u16 seed;
static u16 rnd(void) { seed = seed * 25173 + 13849; return seed; }

int main(void)
{
    u16 i, j, t, r, c, s_w = 0, s_b = 0, s_g = 0, s_o = 0, s_x = 0;
    u8 tb;

    consoleInitDefaultText(0);
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

    seed = 1234;
    for (i = 0; i < 300; i++) b8[i] = (u8)(i * 7);
    for (i = 0; i < 200; i++) w16[i] = rnd();

    /* tri a bulles (echanges entre deux cases) */
    for (i = 0; i < 199; i++)
        for (j = 0; j < 199 - i; j++)
            if (w16[j] > w16[j + 1]) { t = w16[j]; w16[j] = w16[j + 1]; w16[j + 1] = t; }

    /* lectures croisees et octets */
    for (i = 0; i < 150; i++) { tb = b8[i]; b8[i] = b8[299 - i] + tb; b8[299 - i] = tb ^ 0x5a; }
    for (i = 1; i < 200; i++) w16[i] = w16[i] + w16[i - 1] / 2;

    /* table const en ROM, tableau 2D */
    for (r = 0; r < 8; r++)
        for (c = 0; c < 10; c++)
            grid[r][c] = r * c + tbl[(r + c) & 15];

    /* structures */
    for (i = 0; i < 20; i++) { objs[i].x = i * 3; objs[i].y = -(s16)i; objs[i].f = i & 1; }
    for (i = 0; i < 20; i++) if (objs[i].f) objs[i].x += objs[i].y;

    for (i = 0; i < 200; i++) s_w += w16[i] ^ i;
    for (i = 0; i < 300; i++) s_b += b8[i];
    for (r = 0; r < 8; r++) for (c = 0; c < 10; c++) s_g += grid[r][c] * (c + 1);
    for (i = 0; i < 20; i++) s_o += objs[i].x * 3 + objs[i].y + objs[i].f;
    for (i = 0; i < 16; i++) s_x += tbl[i] * tbl[15 - i];

    consoleDrawText(1, 1, "W %u B %u", s_w, s_b);
    consoleDrawText(1, 2, "G %u O %u X %u", s_g, s_o, s_x);
    consoleDrawText(1, 4, (s_w == EXP_W && s_b == EXP_B && s_g == EXP_G && s_o == EXP_O && s_x == EXP_X)
                              ? "OK" : "ECHEC");
    while (1) WaitForVBlank();
    return 0;
}
