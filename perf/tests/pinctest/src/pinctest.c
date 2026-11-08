/* pinctest.c - pointeurs post/pre-incrementes (*p++, *p--, *++p) vers deux
   banques differentes (table const en ROM, tampon en RAM $7E), en alternance :
   tcc garde une copie de l'ancien pointeur, qu'il type en int ; elle doit
   pourtant garder l'octet de banque. Le banc de 816-tcc gardait par hasard la
   bonne banque quand tous les pointeurs visaient la meme. */
#include <snes.h>

static const u8 rom8[16] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9, 3};
static const u16 rom16[8] = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000};
u8 ram8[16];
u16 ram16[8];
static u16 err, n;

static void check(u16 got, u16 want)
{
    if (got != want) err++;
    n++;
}

int main(void)
{
    u16 i, s;
    const u8 *pc;
    u8 *pr;
    const u16 *qc;
    u16 *qr;

    err = 0; n = 0;
    consoleInitDefaultText(0);
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

    /* octets : lecture ROM et ecriture RAM post-incrementees, en alternance */
    pc = rom8; pr = ram8;
    for (i = 0; i < 16; i++) *pr++ = *pc++ + 1;
    for (i = 0; i < 16; i++) check(ram8[i], rom8[i] + 1);

    /* mots, post-decrement depuis la fin */
    qc = rom16 + 7; qr = ram16 + 7;
    for (i = 0; i < 8; i++) *qr-- = *qc-- / 10;
    for (i = 0; i < 8; i++) check(ram16[i], rom16[i] / 10);

    /* pre-increment, et ancien pointeur garde dans une expression */
    pc = rom8 - 1; s = 0;
    for (i = 0; i < 16; i++) s += *++pc;
    check(s, 80);
    pc = rom8; pr = ram8; s = 0;
    for (i = 0; i < 8; i++) { s += *pc++; *pr++ = (u8) s; s += *pc++; }
    check(s, 80);
    check(ram8[0], 3); check(ram8[1], 8); check(ram8[7], 77);

    /* pointeur recopie apres incrementation */
    qc = rom16;
    for (i = 0; i < 8; i++) { const u16 *old = qc++; check(*old, rom16[i]); check(qc - old, 1); }

    consoleDrawText(1, 1, "PINC TESTS %u", n);
    consoleDrawText(1, 2, "PINC ERR %u", err);
    consoleDrawText(1, 4, (err || n != 45) ? "ECHEC" : "OK");
    while (1) WaitForVBlank();
    return 0;
}
