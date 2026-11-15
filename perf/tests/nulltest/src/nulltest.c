/* nulltest.c - test de verite d'un pointeur sur ses 4 octets (correction A de
   816-tcc : gtst teste aussi le mot de banque) et conversion d'un entier non
   constant en pointeur (correction D : banque 0).
   Un pointeur $7F:0000 n'est pas NULL : !p, if (p), p ? : et sprintf("%s")
   doivent le voir non nul, et p == 0 / p != NULL aussi (correction B limitee
   a la comparaison avec 0). p == q compare encore 16 bits : non teste. */
#include <snes.h>
#include <string.h>

typedef union { char *p; u16 w[2]; } ptr_u;

static u16 err, n;
static char buf[32];

static void check(u16 got, u16 want)
{
    if (got != want) err++;
    n++;
}

/* fonctions non static, appelees avec des valeurs non constantes */
u16 is_null(char *s) { if (!s) return 1; return 0; }
u16 is_set(char *s) { if (s) return 1; return 0; }
u16 tern(char *s) { return s ? 1 : 0; }
u16 loop_count(char *s) { u16 c = 0; while (s && c < 3) c++; return c; }
u16 and_and(char *s, u16 x) { return (s && x) ? 1 : 0; }
u16 eq_zero(char *s) { if (s == 0) return 1; return 0; }
u16 ne_null(char *s) { return s != NULL; }
u16 zero_eq(char *s) { return 0 == s; }
u16 sum_eq(char *s, u16 x) { return x + (s == NULL); }
u16 bank_of(char *s) { ptr_u u; u.p = s; return u.w[1]; }
char *from_u16(u16 i) { return (char *) i; }
char *from_u8(u8 i) { return (char *) i; }

int main(void)
{
    ptr_u u;
    char save[4];
    char *q;
    u16 zero, one;

    err = 0; n = 0;
    zero = 0; one = 1;
    consoleInitDefaultText(0);
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

    /* $7F:0000 : adresse basse nulle, banque non nulle */
    u.w[0] = 0; u.w[1] = 0x7F;
    check(is_null(u.p), 0);
    check(is_set(u.p), 1);
    check(tern(u.p), 1);
    check(loop_count(u.p), 3);
    check(and_and(u.p, one), 1);
    check(!u.p, 0);
    check(eq_zero(u.p), 0);
    check(ne_null(u.p), 1);
    check(zero_eq(u.p), 0);
    check(sum_eq(u.p, 5), 5);

    /* sprintf("%s") sur $7F:0000 : la chaine, pas "<NULL>" */
    memcpy(save, u.p, 4);
    memcpy(u.p, "AB", 3);
    sprintf(buf, "%s", u.p);
    check(strcmp(buf, "AB"), 0);
    memcpy(u.p, save, 4);

    /* le vrai NULL reste NULL */
    u.w[0] = 0; u.w[1] = 0;
    check(is_null(u.p), 1);
    check(is_set(u.p), 0);
    check(tern(u.p), 0);
    check(loop_count(u.p), 0);
    check(and_and(u.p, one), 0);
    check(eq_zero(u.p), 1);
    check(ne_null(u.p), 0);
    check(zero_eq(u.p), 1);
    check(sum_eq(u.p, 5), 6);
    sprintf(buf, "%s", u.p);
    check(strcmp(buf, "<NULL>"), 0);

    /* adresse basse non nulle, banque nulle : non nul */
    u.w[0] = 0x1234; u.w[1] = 0;
    check(is_null(u.p), 0);

    /* entier non constant vers pointeur : banque 0 */
    q = from_u16(zero);
    check(is_null(q), 1);
    check(bank_of(q), 0);
    q = from_u16(0x1234);
    check(is_null(q), 0);
    check(bank_of(q), 0);
    q = from_u8((u8) zero);
    check(is_null(q), 1);
    check(bank_of(q), 0);

    consoleDrawText(1, 1, "NULL TESTS %u", n);
    consoleDrawText(1, 2, "NULL ERR %u", err);
    consoleDrawText(1, 4, (err || n != 28) ? "ECHEC" : "OK");
    while (1) WaitForVBlank();
    return 0;
}
