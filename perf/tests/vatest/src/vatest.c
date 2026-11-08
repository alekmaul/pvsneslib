/* vatest.c - arguments variadiques : un u8, un s8 ou un bool passe dans "..."
   doit etre promu en int (2 octets), ce que lit va_arg(ap, int). Avant la
   correction de 816-tcc, un octet de dechet suivait la valeur (6161 au lieu
   de 17) et decalait les arguments suivants. */
#include <snes.h>
#include <string.h>
#include <stdarg.h>

static u16 err, n;
static char buf[64];

static void check(const char *got, const char *want)
{
    if (strcmp(got, want)) err++;
    n++;
}

/* fonction variadique ecrite en C : somme de "count" entiers */
static u16 sum(u16 count, ...)
{
    va_list ap;
    u16 s = 0;
    va_start(ap, count);
    while (count--) s += va_arg(ap, int);
    va_end(ap);
    return s;
}

u8 gu8;
s8 gs8;
u16 gu16;

int main(void)
{
    u8 a = 17, b = 200;
    s8 c = -5;
    bool t = 1;
    char ch = 'Z';
    char *p = "ok";

    err = 0; n = 0;
    gu8 = 255; gs8 = -128; gu16 = 1234;
    consoleInitDefaultText(0);
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

    sprintf(buf, "%u", a);                  check(buf, "17");
    sprintf(buf, "%u %u", a, b);            check(buf, "17 200");
    sprintf(buf, "%d", c);                  check(buf, "-5");
    sprintf(buf, "%d %u %d", c, a, c);      check(buf, "-5 17 -5");
    sprintf(buf, "%u", t);                  check(buf, "1");
    sprintf(buf, "%c%c", ch, 'a');          check(buf, "Za");
    sprintf(buf, "%u %s %u", a, p, gu16);   check(buf, "17 ok 1234");
    sprintf(buf, "%u %d", gu8, gs8);        check(buf, "255 -128");
    sprintf(buf, "%u", (u8) (a + b));       check(buf, "217");
    sprintf(buf, "%u", (u8) (b + b));       check(buf, "144");
    sprintf(buf, "%x", (u8) 0xAB);          check(buf, "ab");
    if (sum(4, a, b, c, gu16) != (u16) (17 + 200 - 5 + 1234)) err++;
    n++;
    if (sum(3, gu8, t, ch) != 255 + 1 + 'Z') err++;
    n++;

    consoleDrawText(1, 1, "VA TESTS %u", n);
    consoleDrawText(1, 2, "VA ERR %u", err);
    consoleDrawText(1, 3, "U8 %u S8 %d", a, c);
    consoleDrawText(1, 4, (err || n != 13) ? "ECHEC" : "OK");
    while (1) WaitForVBlank();
    return 0;
}
