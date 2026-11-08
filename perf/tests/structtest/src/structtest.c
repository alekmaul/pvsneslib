/* structtest.c - motifs des boucles de jeux (816-opt, analyse de flot) :
   - ++ / -- sur des variables u8 globales et locales, avec passage 255 -> 0 ;
   - comparaisons d'un u8 a une constante (non signees si possible), et d'un
     s8, qui doivent rester signees ;
   - champs d'un tableau de structures (tab[i].champ) en lecture et ecriture ;
   - valeurs de retour de fonctions non static selon leur type (u8, u16,
     pointeur vers une autre banque, long long), dont tcc declare le type a
     816-opt (.define __f_ret). */
#include <snes.h>

typedef struct {
    u8 a;
    u16 b;
    u8 pad[59];
    u16 next;   /* decalage 62 */
} obj_t;        /* 64 octets */

obj_t objs[8];
u8 gc, gd;
static u16 err, n;
static const u8 tabrom[4] = {1, 2, 3, 4};
u8 bigbuf[4];

static void check(u16 got, u16 want)
{
    if (got != want) err++;
    n++;
}

/* fonctions non static : le type de retour borne les registres lus */
u8 ret_u8(u16 x) { return (u8) (x + 1); }
u16 ret_u16(u16 x) { return x * 3; }
const u8 *ret_ptr(u16 i) { return &tabrom[i]; }
u8 *ret_ram(u16 i) { return &bigbuf[i]; }
unsigned long long ret_ll(u16 x) { return ((unsigned long long) x << 16) | 0x1234; }
void ret_void(u16 x) { gd = (u8) x; }

/* boucle u8 typique : la variable de boucle est une globale */
u16 count_below(u8 limit)
{
    u16 c = 0;
    for (gc = 0; gc < limit; gc++) c++;
    return c;
}

int main(void)
{
    u16 i, s;
    u8 lc, k;
    s8 sc;
    const u8 *p;
    unsigned long long ll;

    err = 0; n = 0;
    consoleInitDefaultText(0);
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

    /* ++ / -- sur un octet, avec rebouclage */
    gc = 254; gc++; check(gc, 255); gc++; check(gc, 0); gc--; check(gc, 255);
    lc = 1; lc--; check(lc, 0); lc--; check(lc, 255); lc++; check(lc, 0);
    s = 0;
    for (lc = 250; lc != 4; lc++) s++;   /* 250..255 puis 0..3 */
    check(s, 10);

    /* comparaisons u8 a une constante, de part et d'autre des bornes */
    s = 0;
    for (k = 0; k < 255; k++) {
        if (k < 128) s++;
        if (k > 200) s += 2;
        if (k <= 0) s += 3;
        if (k >= 255) s += 100;   /* jamais vrai */
    }
    check(s, 128 + 2 * 54 + 3);
    check(count_below(0), 0);
    check(count_below(1), 1);
    check(count_below(200), 200);
    check(count_below(255), 255);

    /* s8 : la comparaison doit rester signee */
    s = 0;
    for (sc = -100; sc < 100; sc++) {
        if (sc < 10) s++;
        if (sc > -10) s += 100;
    }
    check(s, 110 + 109 * 100);

    /* champs d'un tableau de structures */
    for (i = 0; i < 8; i++) {
        objs[i].a = (u8) (i + 1);
        objs[i].b = i * 100;
        objs[i].next = i + 1000;
    }
    s = 0;
    for (k = 0; k < 8; k++) s += objs[k].a + objs[k].b + objs[k].next;
    check(s, 36 + 2800 + 8028);
    for (i = 0; i < 8; i++) objs[i].next += objs[i].a;
    check(objs[7].next, 1007 + 8);
    check(objs[0].next, 1001);

    /* valeurs de retour */
    check(ret_u8(255), 0);
    check(ret_u8(17), 18);
    check(ret_u16(1000), 3000);
    p = ret_ptr(2);
    check(*p, 3);                    /* banque de la ROM */
    *ret_ram(1) = 77;
    check(bigbuf[1], 77);            /* banque de la RAM */
    check(*ret_ptr(3) + *ret_ram(1), 81);
    ll = ret_ll(0x55);
    check((u16) ll, 0x1234);
    check((u16) (ll >> 16), 0x55);
    ret_void(42);
    check(gd, 42);

    consoleDrawText(1, 1, "STRUCT TESTS %u", n);
    consoleDrawText(1, 2, "STRUCT ERR %u", err);
    consoleDrawText(1, 4, (err || n != 25) ? "ECHEC" : "OK");
    while (1) WaitForVBlank();
    return 0;
}
