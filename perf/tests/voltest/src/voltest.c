/* voltest.c - lecture d'octets volatile : 816-opt lit les octets non volatile
   sur 16 bits (lda X / and.w #$00FF), ce qui lit aussi l'octet suivant. Un
   registre materiel ne doit pas etre lu ainsi : lire $2180 (port de donnees
   de la WRAM) avance l'adresse $2181-$2183. On lit $217F par un pointeur
   volatile : si $2180 etait lu en plus, la lecture suivante de $2180 rendrait
   buf[1] au lieu de buf[0]. Meme chose par une globale volatile placee a
   l'adresse du registre et par une adresse constante. */
#include <snes.h>

u8 buf[8];
volatile u8 *preg;
static u16 err, n;

static void check(u16 got, u16 want)
{
    if (got != want) err++;
    n++;
}

static void wram_addr(u8 *p)
{
    u16 a = (u16) p;
    *(vuint8 *) 0x2181 = a & 0xff;
    *(vuint8 *) 0x2182 = a >> 8;
    *(vuint8 *) 0x2183 = 0; /* buf est en $7E:xxxx, soit WRAM $0xxxx */
}

int main(void)
{
    u16 i, x;
    u8 *p;

    err = 0; n = 0;
    consoleInitDefaultText(0);
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

    for (i = 0; i < 8; i++) buf[i] = 0x10 + i;

    /* pointeur volatile vers $217F */
    preg = (volatile u8 *) 0x217F;
    wram_addr(buf);
    x = *preg;                          /* ne doit pas lire $2180 */
    check(*(vuint8 *) 0x2180, 0x10);    /* buf[0] */
    check(*(vuint8 *) 0x2180, 0x11);    /* buf[1] */

    /* adresse constante, octet volatile */
    wram_addr(buf + 2);
    x += *(vuint8 *) 0x217F;
    check(*(vuint8 *) 0x2180, 0x12);

    /* lecture non volatile ordinaire, par pointeur et en tableau */
    p = buf;
    x = 0;
    for (i = 0; i < 8; i++) x += p[i];
    check(x, 8 * 0x10 + 28);
    x = 0;
    for (i = 0; i < 8; i++) x += *p++;
    check(x, 8 * 0x10 + 28);

    consoleDrawText(1, 1, "VOL TESTS %u", n);
    consoleDrawText(1, 2, "VOL ERR %u", err);
    consoleDrawText(1, 4, (err || n != 5) ? "ECHEC" : "OK");
    while (1) WaitForVBlank();
    return 0;
}
