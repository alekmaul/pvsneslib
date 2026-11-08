/* nmitest.c - callback VBlank en C (nmiSet) pendant que le code principal
   calcule et appelle la lib en continu. Aucune erreur de calcul attendue. */
#include <snes.h>

volatile u16 nmi_count;
u16 scroll;

static void my_vblank(void)
{
    nmi_count++;
    scroll++;
    bgSetScroll(1, scroll, 0);      /* appels lib depuis l interruption */
    consoleVblank();                /* obligatoire avec nmiSet + consoleDrawText */
}

static u16 fib(u16 n) { return n < 2 ? n : fib(n - 1) + fib(n - 2); }

int main(void)
{
    u16 loops = 0, errs = 0, i;
    u16 buf[16];

    consoleInitDefaultText(0);
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);
    setMode(BG_MODE1, 0);
    bgSetDisable(2);
    nmiSet(my_vblank);
    setScreenOn();

    while (loops < 300) {
        if (fib(12) != 144) errs++;
        for (i = 0; i < 16; i++) buf[i] = i * 3 + loops;
        for (i = 0; i < 16; i++) if (buf[i] != (u16)(i * 3 + loops)) errs++;
        if ((u16)(loops * 7) / 7 != loops) errs++;
        consoleDrawText(1, 2, "LOOPS %u  ERR %u", loops, errs);   /* lib + glue */
        loops++;
    }
    consoleDrawText(1, 4, "NMI COUNT %u", nmi_count);
    consoleDrawText(1, 5, "SCROLL %u", scroll);
    consoleDrawText(1, 7, errs ? "ECHEC" : "OK");
    while (1) WaitForVBlank();
    return 0;
}
