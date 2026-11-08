/* ptrtest.c - acces par pointeur + indice (motif de test_rick_col_map dans
   rick1) : pointeur global indexe par un octet, pointeur local post-incremente,
   indice negatif. Chaque resultat est compare a l'acces direct au tableau. */
#include <snes.h>

u8 tiles[256];              /* table de drapeaux, indexee par un octet */
u8 map[64];                 /* carte : octets qui servent d'indices */
u8 *pt_tiles;               /* pointeur global vers la table */
u16 words[40];
u16 *pt_words;

static u16 err, n;

static void check(u16 got, u16 want) { if (got != want) err++; n++; }

/* flag |= pt_tiles[*p++] : le motif de rick1 */
static u8 scan(u8 *p, u16 count)
{
    u8 flag = 0;
    while (count--) flag |= pt_tiles[*p++];
    return flag;
}

static u8 scan_ref(u16 start, u16 count)
{
    u8 flag = 0;
    while (count--) flag |= tiles[map[start++]];
    return flag;
}

int main(void)
{
    u16 i, k;
    s16 j;
    u8 *p;
    u16 *q;

    err = 0; n = 0;
    consoleInitDefaultText(0);
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

    for (i = 0; i < 256; i++) tiles[i] = (u8)(1 << (i & 7)) ^ (u8)(i >> 3);
    for (i = 0; i < 64; i++) map[i] = (u8)(i * 37 + 11);
    for (i = 0; i < 40; i++) words[i] = i * 1000 + 7;
    pt_tiles = tiles;
    pt_words = words;

    /* pointeur global indexe par un octet */
    for (i = 0; i < 64; i++) check(pt_tiles[map[i]], tiles[map[i]]);
    /* pointeur local post-incremente + pointeur global indexe */
    for (i = 0; i < 60; i += 3) check(scan(&map[i], 4), scan_ref(i, 4));
    /* plusieurs lectures a la suite, comme dans rick1 */
    p = &map[5];
    k = pt_tiles[*p++]; k |= pt_tiles[*p++]; k |= pt_tiles[*p]; p += 30;
    k |= pt_tiles[*p++]; k |= pt_tiles[*p];
    check(k, tiles[map[5]] | tiles[map[6]] | tiles[map[7]] | tiles[map[37]] | tiles[map[38]]);

    /* indice negatif : p[j] avec j < 0 doit relire avant p */
    q = &pt_words[20];
    for (j = -20; j < 20; j++) check(q[j], words[20 + j]);
    p = &pt_tiles[128];
    for (j = -100; j < 100; j += 7) check(p[j], tiles[128 + j]);

    consoleDrawText(1, 1, "PTR TESTS %u", n);
    consoleDrawText(1, 2, "PTR ERR %u", err);
    consoleDrawText(1, 4, (err || n != 154) ? "ECHEC" : "OK");
    while (1) WaitForVBlank();
    return 0;
}
