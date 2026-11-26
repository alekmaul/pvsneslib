/* logictest.c - && et || dont l'operande droit est lui-meme une liste de
   sauts de meme sens : a && (b && c), a || (b || c), a || !b, a || p == NULL,
   a && p != NULL, a && (comparaison 32 bits)... tcc resolvait la liste de
   l'operande gauche au debut du bloc "vrai" (816-gen.c, gtst) : branche
   fausse, ou boucle infinie dans un while. Le resultat de chaque forme est
   compare a la meme formule calculee avec & et | (sans saut), pour a, b, c,
   d dans {0, 1}. */
#include <snes.h>

static u16 err, n;
u8 ga, gb, gc;                 /* globales, comme dans le rapport de bug */

static void check(u16 got, u16 want)
{
    if (got != want) err++;
    n++;
}

/* if / else, comme dans le rapport */
u16 if_and_and(u8 a, u8 b, u8 c) { if (a && (b && c)) return 1; else return 0; }
u16 if_or_or(u8 a, u8 b, u8 c)   { if (a || (b || c)) return 1; else return 0; }
u16 if_and_or(u8 a, u8 b, u8 c)  { if (a && (b || c)) return 1; else return 0; }
u16 if_or_and(u8 a, u8 b, u8 c)  { if (a || (b && c)) return 1; else return 0; }
u16 if_left(u8 a, u8 b, u8 c)    { if ((a && b) && c) return 1; else return 0; }
u16 if_not(u8 a, u8 b, u8 c)     { if (!(a && (b && c))) return 1; else return 0; }
/* valeur, ternaire, boucle */
u16 val_and_and(u8 a, u8 b, u8 c) { return a && (b && c); }
u16 val_or_or(u8 a, u8 b, u8 c)   { return a || (b || c); }
u16 tern_and_and(u8 a, u8 b, u8 c) { return (a && (b && c)) ? 1 : 0; }
u16 while_and_and(u8 a, u8 b, u8 c) { u16 k = 0; while (k < 3 && (a && (b && c))) k++; return k; }
/* quatre niveaux */
u16 if_and4(u8 a, u8 b, u8 c, u8 d) { if (a && (b && (c && d))) return 1; return 0; }
u16 if_or4(u8 a, u8 b, u8 c, u8 d)  { if (a || (b || (c || d))) return 1; return 0; }
/* cas du rapport */
u16 if_paren2(u8 a, u8 b, u8 c)   { if (a && ((b && c))) return 1; return 0; }
u16 if_cmp(u8 a, u8 b, u8 c)      { if (a && (b == 1 && c == 1)) return 1; return 0; }
u16 if_and_mid(u8 a, u8 b, u8 c, u8 d) { if (a && (b && c) && d) return 1; return 0; }
u16 if_or_mid(u8 a, u8 b, u8 c, u8 d)  { if (a || (b || c) || d) return 1; return 0; }
u16 if_and_tail3(u8 a, u8 b, u8 c, u8 d) { if (a && (b && c && d)) return 1; return 0; }
u16 if_and_pair(u8 a, u8 b, u8 c, u8 d)  { if (a && b && (c && d)) return 1; return 0; }
u16 if_and_2x2(u8 a, u8 b, u8 c, u8 d)   { if ((a && b) && (c && d)) return 1; return 0; }
u16 if_or_2x2(u8 a, u8 b, u8 c, u8 d)    { if ((a || b) || (c || d)) return 1; return 0; }
u16 if_and_k1(u8 a, u8 b) { if (a && (b && 1)) return 1; return 0; }
u16 if_and_11(u8 a)       { if (a && (1 && 1)) return 1; return 0; }
u16 if_or_00(u8 a)        { if (a || (0 || 0)) return 1; return 0; }
/* autres formes qui donnent une liste de sauts a droite */
u16 if_or_not(u8 a, u8 b)    { if (a || !b) return 1; return 0; }
u16 if_and_not(u8 a, u8 b)   { if (a && !b) return 1; return 0; }
u16 if_or_null(u8 a, u8 b)   { char *p = b ? (char *) &err : 0; if (a || p == NULL) return 1; return 0; }
u16 if_and_ptr(u8 a, u8 b)   { char *p = b ? (char *) &err : 0; if (a && p != NULL) return 1; return 0; }
u16 if_and_l32(u8 a, u8 b)   { u32 x; if (b) x = 0x10000; else x = 5; if (a && (x > 0xFFFF)) return 1; return 0; }
u16 if_or_l32(u8 a, u8 b)    { u32 x; if (b) x = 0x10000; else x = 5; if (a || (x > 0xFFFF)) return 1; return 0; }
/* globales u8 */
u16 glob_and_and(void) { if (ga && (gb && gc)) return 1; else return 0; }

int main(void)
{
    u8 a, b, c, d;

    err = 0; n = 0;
    consoleInitDefaultText(0);
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

    for (a = 0; a < 2; a++)
        for (b = 0; b < 2; b++)
            for (c = 0; c < 2; c++)
            {
                check(if_and_and(a, b, c), a & b & c);
                check(if_paren2(a, b, c), a & b & c);
                check(if_or_not(a, b), a | (b ^ 1));
                check(if_and_not(a, b), a & (b ^ 1));
                check(if_or_null(a, b), a | (b ^ 1));
                check(if_and_ptr(a, b), a & b);
                check(if_and_l32(a, b), a & b);
                check(if_or_l32(a, b), a | b);
                check(if_cmp(a, b, c), a & b & c);
                check(if_and_k1(a, b), a & b);
                check(if_and_11(a), a);
                check(if_or_00(a), a);
                check(if_or_or(a, b, c), a | b | c);
                check(if_and_or(a, b, c), a & (b | c));
                check(if_or_and(a, b, c), a | (b & c));
                check(if_left(a, b, c), a & b & c);
                check(if_not(a, b, c), (a & b & c) ^ 1);
                check(val_and_and(a, b, c), a & b & c);
                check(val_or_or(a, b, c), a | b | c);
                check(tern_and_and(a, b, c), a & b & c);
                check(while_and_and(a, b, c), (a & b & c) * 3);
                ga = a; gb = b; gc = c;
                check(glob_and_and(), a & b & c);
                for (d = 0; d < 2; d++)
                {
                    check(if_and4(a, b, c, d), a & b & c & d);
                    check(if_or4(a, b, c, d), a | b | c | d);
                    check(if_and_mid(a, b, c, d), a & b & c & d);
                    check(if_or_mid(a, b, c, d), a | b | c | d);
                    check(if_and_tail3(a, b, c, d), a & b & c & d);
                    check(if_and_pair(a, b, c, d), a & b & c & d);
                    check(if_and_2x2(a, b, c, d), a & b & c & d);
                    check(if_or_2x2(a, b, c, d), a | b | c | d);
                }
            }

    consoleDrawText(1, 1, "LOGIC TESTS %u", n);
    consoleDrawText(1, 2, "LOGIC ERR %u", err);
    consoleDrawText(1, 4, (err || n != 304) ? "ECHEC" : "OK");
    while (1) WaitForVBlank();
    return 0;
}
