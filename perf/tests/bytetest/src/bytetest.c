/* bytetest.c - operations |= &= ^= sur des octets (locaux, globaux, parametres),
   operandes lus par tableau et par pointeur ; chaque resultat est compare a un
   calcul fait en 16 bits puis tronque. */
#include <snes.h>

u8 tab[64];
u8 *ptab;
u8 g8;
static u16 err, n;

static void check(u16 got, u16 want) { if (got != want) err++; n++; }

static u8 f_or(u8 *p, u16 cnt, u8 init)
{
    u8 v = init;
    while (cnt--) v |= ptab[*p++];
    return v;
}

static u8 f_mix(u8 a, u8 b)
{
    u8 v = a;
    v &= tab[b & 63];
    v ^= tab[(b + 7) & 63];
    v |= a;
    return v;
}

int main(void)
{
    u16 i, w, ref;
    u8 v, k;

    err = 0; n = 0;
    consoleInitDefaultText(0);
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

    for (i = 0; i < 64; i++) tab[i] = (u8)(i * 29 + 3);
    ptab = tab;

    /* local |= tableau[octet] via pointeur */
    for (i = 0; i < 50; i += 5) {
        ref = 0x11;
        for (w = 0; w < 4; w++) ref |= tab[tab[i + w] & 63];
        for (w = 0; w < 4; w++) tab[i + w] &= 63;       /* indices valides */
        ref = 0x11;
        for (w = 0; w < 4; w++) ref |= tab[tab[i + w]];
        check(f_or(&tab[i], 4, 0x11), ref & 0xff);
    }
    /* melange &= ^= |= */
    for (i = 0; i < 64; i++) {
        k = (u8)(i * 7);
        ref = (u16)i & tab[k & 63];
        ref ^= tab[(k + 7) & 63];
        ref |= i;
        check(f_mix((u8)i, k), ref & 0xff);
    }
    /* global octet, resultat reutilise en 16 bits juste apres */
    g8 = 0x0f;
    v = 0xf0;
    for (i = 0; i < 16; i++) {
        g8 ^= tab[i];
        v |= g8;
        w = v + 1;          /* A et l'octet haut ne doivent pas etre faux */
        check(w, (u16)v + 1);
    }

    consoleDrawText(1, 1, "BYTE TESTS %u", n);
    consoleDrawText(1, 2, "BYTE ERR %u", err);
    consoleDrawText(1, 4, (err || n != 90) ? "ECHEC" : "OK");
    while (1) WaitForVBlank();
    return 0;
}
