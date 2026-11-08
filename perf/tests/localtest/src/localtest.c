/* localtest.c - variables locales sur la pile : adresse prise et modification
   par pointeur, tableau local, octets et mots melanges, structure locale,
   compteurs de boucle avec appels. Cible : l'analyse des valeurs de 816-opt
   (une case de pile ne doit pas etre supposee inchangee a tort). */
#include <snes.h>

typedef struct { u16 a; u8 b; u16 c; } st_t;

static u16 err, n;
static volatile u16 sink;
u16 g;

static void check(u16 got, u16 want) { if (got != want) err++; n++; }
static void set7(u16 *p) { *p = 7; }
static void bump(st_t *s) { s->a += 1; s->b += 2; s->c += 3; }
static u16 twice(u16 v) { g += v; return v * 2; }
static u16 param(u16 v) { v += 5; v = v * 3; return v - 1; }

int main(void)
{
    u16 x, y, i, j, k, acc;
    u16 *p;
    u16 arr[6];
    u8 b1, b2;
    st_t s;

    err = 0; n = 0; g = 0;
    consoleInitDefaultText(0);
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

    /* adresse prise : ecriture par pointeur puis relecture */
    x = 1; p = &x; *p = 5; check(x, 5);
    x = 2; set7(&x); check(x, 7);
    y = x; *p = 9; check(x, 9); check(y, 7);

    /* tableau local */
    for (i = 0; i < 6; i++) arr[i] = i * 11;
    arr[2] = arr[5] + arr[1];
    check(arr[2], 66); check(arr[0] + arr[3], 33);
    p = &arr[4]; *p = 100; check(arr[4], 100);

    /* octets et mots melanges */
    b1 = 200; b2 = 100; x = b1 + b2; check(x, 300);
    b1 = b1 + b2; check(b1, 44); /* 300 & 255 */
    x = 0x1234; b1 = (u8)x; check(b1, 0x34); check(x, 0x1234);

    /* structure locale modifiee par une fonction */
    s.a = 10; s.b = 20; s.c = 30;
    bump(&s); bump(&s);
    check(s.a, 12); check(s.b, 24); check(s.c, 36);

    /* compteurs de boucle avec appels dans le corps */
    acc = 0;
    for (i = 0; i < 5; i++)
        for (j = 0; j < 4; j++) {
            k = twice(i + j);
            acc += k;
            sink = i;
        }
    check(acc, 140); check(g, 70); check(i, 5); check(j, 4);

    /* meme valeur rechargee apres modification */
    x = 3; y = x; x = x + 1; check(y, 3); check(x, 4);
    x = y; y = 8; check(x, 3);

    /* parametres modifies dans l'appele */
    check(param(4), 26); check(param(0), 14);

    consoleDrawText(1, 1, "LOCAL TESTS %u", n);
    consoleDrawText(1, 2, "LOCAL ERR %u", err);
    consoleDrawText(1, 4, (err || n != 23) ? "ECHEC" : "OK");
    while (1) WaitForVBlank();
    return 0;
}
