/* shifttest.c - decalages par une constante 1..15 (<<, >> non signe, >> signe)
   compares a des decalages d'un bit repetes. */
#include <snes.h>

static const u16 vals[] = {0, 1, 2, 3, 0x7f, 0x80, 0xff, 0x100, 0x1234, 0x7fff, 0x8000, 0x8001,
                           0xa5a5, 0xfffe, 0xffff};
#define NV (sizeof(vals) / sizeof(vals[0]))

static volatile u16 vv;
static u16 err, n;

static u16 shl(u16 v, u16 k) { while (k--) v = v << 1; return v; }
static u16 shr(u16 v, u16 k) { while (k--) v = v >> 1; return v; }
static s16 sar(s16 v, u16 k) { while (k--) v = v >> 1; return v; }

#define T(k)                                                        \
    do {                                                            \
        u16 u = vv;                                                 \
        s16 s = (s16)vv;                                            \
        if ((u16)(u << k) != shl(u, k)) err++;                      \
        if ((u16)(u >> k) != shr(u, k)) err++;                      \
        if ((s16)(s >> k) != sar(s, k)) err++;                      \
        n += 3;                                                     \
    } while (0)

int main(void)
{
    u16 i;

    err = 0; n = 0;
    consoleInitDefaultText(0);
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

    for (i = 0; i < NV; i++) {
        vv = vals[i];
        T(1); T(2); T(3); T(4); T(5); T(6); T(7); T(8);
        T(9); T(10); T(11); T(12); T(13); T(14); T(15);
    }
    consoleDrawText(1, 1, "SHIFT TESTS %u", n);
    consoleDrawText(1, 2, "SHIFT ERR %u", err);
    consoleDrawText(1, 4, (err || n != 675) ? "ECHEC" : "OK");
    while (1) WaitForVBlank();
    return 0;
}
