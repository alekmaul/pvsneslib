/* multest.c - verifie * / % 16 bits (tcc__mul, tcc__udiv, tcc__div) contre des
   references sans ces operateurs, puis les utilise en meme temps dans le code
   principal et dans une callback VBlank (verrou du multiplicateur materiel). */
#include <snes.h>

static u16 ref_mul(u16 a, u16 b)
{
    u16 r = 0;
    while (b) {
        if (b & 1) r += a;
        a <<= 1;
        b >>= 1;
    }
    return r;
}

static u32 ref_mul32(u32 a, u32 b)
{
    u32 r = 0;
    while (b) {
        if (b & 1) r += a;
        a <<= 1;
        b >>= 1;
    }
    return r;
}

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
static volatile u32 wa, wb;
static volatile u32 nwa, nwb;   /* propres a la callback VBlank */

/* valeurs couvrant les chemins : < 256, >= 256, octet bas nul, extremes */
static const u16 vals[] = {0, 1, 2, 3, 7, 13, 100, 127, 128, 255, 256, 257, 300,
                           511, 1000, 4095, 4096, 12345, 32767, 32768, 40000,
                           0xff00, 0xfffe, 0xffff};
#define NV (sizeof(vals) / sizeof(vals[0]))

volatile u16 nmi_count, nmi_err, nmi_busy, nmi_overrun;
static u16 na = 1;

static void my_vblank(void)
{
    u16 q, r, p, h, l, lo, hi;
    u32 w;

    /* en premier : la VRAM n'est modifiable que pendant le VBlank (Mesen et
       la console ignorent les ecritures faites ensuite) */
    consoleVblank();
    if (nmi_busy) {         /* la callback precedente n'etait pas finie */
        nmi_overrun++;
        return;
    }
    nmi_busy = 1;
    nmi_count++;
    /* meme travail que le code principal, depuis l'interruption, en restant
       bien en dessous d'une frame */
    na = na * 25173 + 13849;
    p = (na & 0x7ff) * (na >> 11);
    if (p != ref_mul(na & 0x7ff, na >> 11)) nmi_err++;
    ref_udiv(na, (na & 0x7f) + 1, &q, &r);
    if (na / ((na & 0x7f) + 1) != q || na % ((na & 0x7f) + 1) != r) nmi_err++;
    /* 32 bits : na * 97 compare a un calcul par octets, sans depasser 16 bits */
    nwa = na; nwb = 97;
    w = nwa * nwb;
    h = na >> 8; l = na & 0xff;
    lo = (u16)(na * 97u);
    hi = (h * 97 + ((l * 97) >> 8)) >> 8;
    if ((u16)w != lo || (u16)(w >> 16) != hi) nmi_err++;
    nmi_busy = 0;
}

int main(void)
{
    u16 i, j, a, b, q, r, k;
    u16 err_mul = 0, err_div = 0, err_sdiv = 0, err_stress = 0, err_mul32 = 0, n = 0;
    u32 la, lb;
    u16 seed = 0xace1;

    consoleInitDefaultText(0);
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

    /* 1. exactitude : toutes les paires de vals, plus des paires aleatoires */
    for (k = 0; k < NV * NV + 3000; k++) {
        if (k < NV * NV) {
            a = vals[k / NV];
            b = vals[k % NV];
        } else {
            seed = seed * 25173 + 13849; a = seed;
            seed = seed * 25173 + 13849; b = seed;
            if (k & 1) b &= 0xff;           /* diviseurs 8 bits : voie materielle */
            if (k & 2) a &= 0xff;
        }
        va = a; vb = b;
        if (va * vb != ref_mul(a, b)) err_mul++;
        wa = a; wb = b;                     /* u16 x u16 -> u32 */
        if (wa * wb != ref_mul32(a, b)) err_mul32++;
        la = ((u32)a << 16) | b; lb = ((u32)b << 16) | (u16)(a ^ 0x5a5a);
        wa = la; wb = lb;                   /* u32 x u32 -> 32 bits de poids faible */
        if (wa * wb != ref_mul32(la, lb)) err_mul32++;
        if (b) {
            ref_udiv(a, b, &q, &r);
            if (va / vb != q || va % vb != r) err_div++;
            {   /* signe : troncature vers zero, reste du signe du dividende */
                s16 sa = (s16)a, sb = (s16)b, sq, sr;
                u16 ua = sa < 0 ? (u16)-sa : (u16)sa, ub = sb < 0 ? (u16)-sb : (u16)sb, uq, ur;
                if (!(sa == -32768 && sb == -1)) {
                    ref_udiv(ua, ub, &uq, &ur);
                    sq = ((sa < 0) != (sb < 0)) ? -(s16)uq : (s16)uq;
                    sr = sa < 0 ? -(s16)ur : (s16)ur;
                    if ((s16)va / (s16)vb != sq || (s16)va % (s16)vb != sr) err_sdiv++;
                }
            }
        }
        n++;
    }
    consoleDrawText(1, 1, "MULDIV TESTS %u", n);
    consoleDrawText(1, 2, "MUL ERR %u", err_mul);
    consoleDrawText(1, 3, "DIV ERR %u", err_div);
    consoleDrawText(1, 4, "SDIV ERR %u", err_sdiv);
    consoleDrawText(1, 5, "MUL32 ERR %u", err_mul32);

    /* 2. concurrence avec une callback VBlank qui multiplie et divise aussi */
    nmi_count = 0; nmi_err = 0; nmi_busy = 0; nmi_overrun = 0;
    nmiSet(my_vblank);
    for (i = 0; i < 300; i++) {
        for (j = 0; j < 40; j++) {
            seed = seed * 25173 + 13849;
            a = seed; b = (seed >> 9) + 1;
            va = a; vb = b;
            if (va * vb != ref_mul(a, b)) err_stress++;
            ref_udiv(a, b, &q, &r);
            if (va / vb != q || va % vb != r) err_stress++;
            wa = a; wb = b;
            if (wa * wb != ref_mul32(a, b)) err_stress++;
        }
        consoleDrawText(1, 6, "STRESS %u ERR %u", i, err_stress);
    }
    consoleDrawText(1, 7, "NMI %u ERR %u OVR %u", nmi_count, nmi_err, nmi_overrun);
    consoleDrawText(1, 9, (err_mul | err_div | err_sdiv | err_mul32 | err_stress | nmi_err | nmi_overrun) ? "ECHEC" : "OK");
    while (1) WaitForVBlank();
    return 0;
}
