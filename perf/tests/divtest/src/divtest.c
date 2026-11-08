/* divtest.c - verifie / et % (u16, s16, u8) contre une division de reference
   par decalages, sans operateur / ni %. */
#include <snes.h>

static void ref_udiv(u16 a, u16 b, u16 *q, u16 *r)
{
    u16 quo = 0, rem = 0, i;
    for (i = 0; i < 16; i++) {
        rem = (rem << 1) | (a >> 15);
        a <<= 1;
        quo <<= 1;
        if (rem >= b) { rem -= b; quo |= 1; }
    }
    *q = quo; *r = rem;
}

static volatile u16 va, vb;   /* empeche le calcul a la compilation */
static volatile u8 v8a, v8b;

int main(void)
{
    u16 a, b, q, r, i, j, errs = 0, errs_s = 0, errs_8 = 0, n = 0;
    static const u16 divs[] = {1, 2, 3, 7, 10, 13, 100, 255, 256, 257, 300, 1000,
                               4095, 12345, 32767, 32768, 40000, 65535};
    consoleInitDefaultText(0);
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);
    setMode(BG_MODE1, 0);
    setScreenOn();

    for (j = 0; j < sizeof(divs) / sizeof(divs[0]); j++) {
        for (i = 0; i < 400; i++) {
            a = (u16)(i * 40503u + j * 977u);
            b = divs[j];
            va = a; vb = b;
            ref_udiv(a, b, &q, &r);
            if (va / vb != q || va % vb != r) errs++;
            {   /* signe : C tronque vers zero, le reste a le signe du dividende */
                s16 sa = (s16)a, sb = (s16)b, sq, sr;
                u16 ua = sa < 0 ? (u16)-sa : (u16)sa, ub = sb < 0 ? (u16)-sb : (u16)sb, uq, ur;
                if (sb == 0 || (sa == -32768 && sb == -1)) continue;
                ref_udiv(ua, ub, &uq, &ur);
                sq = ((sa < 0) != (sb < 0)) ? -(s16)uq : (s16)uq;
                sr = sa < 0 ? -(s16)ur : (s16)ur;
                va = (u16)sa; vb = (u16)sb;
                if ((s16)va / (s16)vb != sq || (s16)va % (s16)vb != sr) errs_s++;
            }
            n++;
        }
    }
    for (i = 0; i < 256; i++)
        for (j = 1; j < 256; j += 7) {
            v8a = (u8)i; v8b = (u8)j;
            ref_udiv(i, j, &q, &r);
            if ((u8)(v8a / v8b) != q || (u8)(v8a % v8b) != r) errs_8++;
        }
    consoleDrawText(1, 2, "DIV TESTS %u", n);
    consoleDrawText(1, 4, "U16 ERR %u", errs);
    consoleDrawText(1, 5, "S16 ERR %u", errs_s);
    consoleDrawText(1, 6, "U8  ERR %u", errs_8);
    consoleDrawText(1, 8, (errs | errs_s | errs_8) ? "ECHEC" : "OK");
    while (1) WaitForVBlank();
    return 0;
}
