/* cmultest.c - multiplications par une constante (code en ligne de 816-tcc :
   decalages et additions pour les constantes a 2 ou 3 bits), comparees au meme
   produit par une variable (tcc__mul), plus un tableau de structures de 72 octets. */
#include <snes.h>

static const u16 vals[] = {0, 1, 2, 3, 7, 100, 127, 128, 255, 256, 257, 1000, 4095, 12345,
                           32767, 32768, 40000, 65535};
#define NV (sizeof(vals) / sizeof(vals[0]))

typedef struct { u16 a, b; u8 pad[68]; } big_t;   /* 72 octets, comme dans rick1 */
big_t bigs[12];

static volatile u16 vc;   /* multiplicateur variable : force tcc__mul */
static u16 err, n;

#define T(c)                                           \
    do {                                               \
        vc = c;                                        \
        if ((u16)(x * c) != (u16)(x * vc)) err++;      \
        n++;                                           \
    } while (0)

int main(void)
{
    u16 i, x, s;
    big_t *p;

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
        T(6); T(10); T(12); T(20); T(24); T(40); T(48); T(72); T(96); T(100);
        T(136); T(160); T(192); T(200); T(320); T(1000); T(0x8001); T(0x4040);
        T(0xC000); T(0xFFFF);
    }
    /* tableau de structures de 72 octets : &bigs[i] = bigs + i*72 */
    for (i = 0; i < 12; i++) { bigs[i].a = i * 3; bigs[i].b = i + 100; }
    s = 0;
    for (i = 0; i < 12; i++) { p = &bigs[i]; s += p->a + p->b; }
    if (s != 3 * 66 + 12 * 100 + 66) err++;
    n++;

    consoleDrawText(1, 1, "CMUL TESTS %u", n);
    consoleDrawText(1, 2, "CMUL ERR %u", err);
    consoleDrawText(1, 4, (err || n != 361) ? "ECHEC" : "OK");
    while (1) WaitForVBlank();
    return 0;
}
