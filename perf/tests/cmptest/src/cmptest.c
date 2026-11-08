/* cmptest.c - comparaisons 16 et 32 bits sous trois formes :
   - "if (a OP b)"     : saut si faux (fusion comparaison + branchement de 816-opt)
   - "if (!(a OP b))"  : saut si vrai
   - "x = a OP b"      : booleen utilise comme valeur (non fusionne), sert de reference.
   Les trois doivent donner le meme resultat pour toutes les paires. */
#include <snes.h>

static const u16 vals[] = {0, 1, 2, 3, 100, 127, 128, 255, 256, 0x7ffe, 0x7fff,
                           0x8000, 0x8001, 0xfffe, 0xffff};
#define NV (sizeof(vals) / sizeof(vals[0]))

static volatile u16 va, vb;
static volatile u32 wa, wb;
static u16 err, n;
static u16 cv, cf, cg;   /* globales : des locales par CHECK depasseraient
                            les 255 octets de pile adressables par tcc (d,s) */

/* compare les formes if / if-not / valeur pour une condition */
#define CHECK(cond)                                   \
    do {                                              \
        cv = (cond); cf = 0; cg = 1;                  \
        if (cond) cf = 1;                             \
        if (!(cond)) cg = 0;                          \
        if (cf != cv || cg != cv || cv > 1) err++;    \
        n++;                                          \
    } while (0)

static void check_u16(u16 a, u16 b)
{
    va = a; vb = b;
    CHECK(va == vb); CHECK(va != vb);
    CHECK(va < vb);  CHECK(va <= vb);
    CHECK(va > vb);  CHECK(va >= vb);
    CHECK((s16)va < (s16)vb);  CHECK((s16)va <= (s16)vb);
    CHECK((s16)va > (s16)vb);  CHECK((s16)va >= (s16)vb);
}

static void check_const(u16 a)
{
    s16 s;
    va = a; s = (s16)va;
    CHECK(va == 100); CHECK(va != 100);
    CHECK(va < 128);  CHECK(va <= 128);
    CHECK(va > 255);  CHECK(va >= 255);
    CHECK(va < 0x8000); CHECK(va > 0x7fff);
    CHECK(s < 0);   CHECK(s <= -1);
    CHECK(s > 127); CHECK(s >= -100);
    CHECK(s < 3);   CHECK(s > -32767);
}

static void check_u32(u32 a, u32 b)
{
    wa = a; wb = b;
    CHECK(wa == wb); CHECK(wa != wb);
    CHECK(wa < wb);  CHECK(wa <= wb);
    CHECK(wa > wb);  CHECK(wa >= wb);
    CHECK((s32)wa < (s32)wb); CHECK((s32)wa > (s32)wb);
}

/* resultat de reference calcule sans operateur de comparaison */
static u16 ult_ref(u16 a, u16 b)
{
    u32 d = (u32)a - (u32)b;    /* emprunt dans le bit 31 */
    return (u16)(d >> 31);
}

int main(void)
{
    u16 i, j, loops = 0, sum = 0, err_ref = 0;

    err = 0; n = 0;   /* en HiROM, les statiques non initialisees ne sont pas mises a zero */
    consoleInitDefaultText(0);
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

    for (i = 0; i < NV; i++) {
        check_const(vals[i]);
        for (j = 0; j < NV; j++) {
            check_u16(vals[i], vals[j]);
            check_u32(((u32)vals[i] << 16) | vals[j], ((u32)vals[j] << 16) | vals[i]);
            check_u32(vals[i], vals[j]);
            va = vals[i]; vb = vals[j];
            if ((va < vb) != ult_ref(vals[i], vals[j])) err_ref++;
            if (va < vb) sum += i * 16 + j;   /* signature des resultats */
        }
    }
    /* boucles : for / while / do-while avec differentes conditions */
    for (i = 0; i < 10; i++) loops++;
    for (i = 10; i > 0; i--) loops++;
    i = 0; while (i != 7) { i++; loops++; }
    i = 0; do { i += 3; loops++; } while (i <= 12);
    { s16 s; for (s = -5; s < 5; s++) loops++; for (s = 5; s >= -5; s--) loops++; }

    consoleDrawText(1, 1, "CMP TESTS %u", n);
    consoleDrawText(1, 2, "CMP ERR %u REF ERR %u", err, err_ref);
    consoleDrawText(1, 3, "SUM %u LOOPS %u", sum, loops);
    consoleDrawText(1, 5, (err | err_ref || sum != 8295 || loops != 53) ? "ECHEC" : "OK");
    while (1) WaitForVBlank();
    return 0;
}
