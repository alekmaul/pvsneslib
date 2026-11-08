/* sdivtest.c - division et modulo signes par une puissance de 2 constante
   (code en ligne genere par 816-tcc), compares au meme calcul par une variable
   (tcc__div). Troncature vers zero, reste du signe du dividende. */
#include <snes.h>

static const s16 vals[] = {0, 1, -1, 2, -2, 3, -3, 7, -7, 8, -8, 9, -9, 100, -100, 255, -255,
                           256, -256, 1000, -1000, 12345, -12345, 16383, -16383, 16384, -16384,
                           32767, -32767, -32768};
#define NV (sizeof(vals) / sizeof(vals[0]))

static volatile s16 vd;   /* diviseur variable : force tcc__div */
static u16 err, n;

#define T(k)                                                  \
    do {                                                      \
        vd = k;                                               \
        if ((s16)(x / k) != (s16)(x / vd)) err++;             \
        if ((s16)(x % k) != (s16)(x % vd)) err++;             \
        n += 2;                                               \
    } while (0)

int main(void)
{
    u16 i;
    s16 x;

    err = 0; n = 0;
    consoleInitDefaultText(0);
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

    for (i = 0; i < NV; i++) {
        x = vals[i];
        T(1); T(2); T(4); T(8); T(16); T(32); T(64); T(128);
        T(256); T(512); T(1024); T(2048); T(4096); T(8192); T(16384);
    }
    consoleDrawText(1, 1, "SDIV TESTS %u", n);
    consoleDrawText(1, 2, "SDIV ERR %u", err);
    consoleDrawText(1, 4, (err || n != 900) ? "ECHEC" : "OK");
    while (1) WaitForVBlank();
    return 0;
}
