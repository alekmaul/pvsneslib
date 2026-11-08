/*
 * torture.c - meme source compile par 816-tcc et par llvm-mos ;
 * les deux ecrans doivent etre identiques au pixel pres.
 */
#include <snes.h>
#include <string.h>

#ifndef FAR   /* defini par l'overlay llvm-mos ; vide pour tcc */
#define FAR
#endif

/* .data near, .bss near, .rodata near */
u16 gcounter = 1234;
s16 gsigned = -321;
u16 gbss[8];
const u8 gtable[8] = {3, 1, 4, 1, 5, 9, 2, 6};
const char *gnames[3] = {"ZERO", "ONE", "TWO"};

/* tableaux far : RAM $7E (auto-far) et ROM */
u8 FAR bigram[3000];
const u16 FAR sinlut[8] = {0, 49, 90, 117, 127, 117, 90, 49};

typedef struct { s16 x, y; u8 id; } pt_t;
pt_t pts[3] = {{10, -20, 1}, {300, 400, 2}, {-5, 7, 3}};

static u16 fib(u16 n) { return n < 2 ? n : fib(n - 1) + fib(n - 2); }

static u16 op_add(u16 a, u16 b) { return a + b; }
static u16 op_mul(u16 a, u16 b) { return a * b; }
static u16 op_div(u16 a, u16 b) { return a / b; }
typedef u16 (*op_fn)(u16, u16);
op_fn ops[3] = {op_add, op_mul, op_div};

static const char *classify(u16 v)
{
    switch (v % 7) {
    case 0: return "ZERO";
    case 1: return "ONE";
    case 2: return "TWO";
    case 3: return "THREE";
    case 4: return "FOUR";
    case 5: return "FIVE";
    default: return "SIX";
    }
}

static u16 counter(void) { static u16 n = 40; return ++n; }

int main(void)
{
    u16 i, sum, y = 1;
    s16 s;
    u32 big;
    char buf[24];

    consoleInitDefaultText(0);
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    consoleDrawText(1, y++, "TORTURE TEST");

    /* donnees initialisees / zero */
    sum = 0;
    for (i = 0; i < 8; i++) sum += gbss[i] + gtable[i];
    gcounter += 66;
    consoleDrawText(1, y++, "DATA %u %d BSS+RO %u", gcounter, gsigned, sum);

    /* arithmetique 16 bits signee / non signee */
    s = -1000;
    consoleDrawText(1, y++, "MUL %u DIV %u MOD %u", (u16)(123 * 45), (u16)(60000 / 7), (u16)(60000 % 7));
    consoleDrawText(1, y++, "SDIV %d SMOD %d SHR %d", s / 7, s % 7, s >> 3);

    /* 32 bits */
    big = 100000;
    big = big * 3 + 12345;
    consoleDrawText(1, y++, "U32 %lld DIV %lld", big, big / 1000);

    /* recursion, pointeurs de fonction */
    consoleDrawText(1, y++, "FIB15 %u", fib(15));
    consoleDrawText(1, y++, "OPS %u %u %u", ops[0](7, 8), ops[1](7, 8), ops[2](700, 8));

    /* switch, tableau de pointeurs, statique locale */
    consoleDrawText(1, y++, "SW %s %s %s", classify(10), classify(13), classify(20));
    consoleDrawText(1, y++, "NAMES %s %s", gnames[2], gnames[0]);
    counter();
    consoleDrawText(1, y++, "STATIC %u", counter());

    /* structures */
    sum = 0;
    for (i = 0; i < 3; i++) sum += pts[i].x * 2 + pts[i].y + pts[i].id;
    consoleDrawText(1, y++, "STRUCT %d", (s16)sum);

    /* far RAM et far ROM */
    for (i = 0; i < 3000; i++) bigram[i] = (u8)(i * 7);
    sum = 0;
    for (i = 0; i < 3000; i += 3) sum += bigram[i];
    consoleDrawText(1, y++, "FARRAM %u", sum);
    sum = 0;
    for (i = 0; i < 8; i++) sum += sinlut[i];
    consoleDrawText(1, y++, "FARROM %u", sum);

    /* libc */
    strcpy(buf, "SNES");
    strcpy(buf + 4, "-LLVM");
    memset(buf + 9, 0, 4);
    consoleDrawText(1, y++, "LIBC %s LEN %u", buf, (u16)strlen(buf));

    /* formats */
    consoleDrawText(1, y++, "FMT %04x %X %c %5d|", 0xBEEF & 0xFFF, 0xCAFE, 'Q', 42);
    consoleDrawText(1, y++, "FMT %-5d| %s%%", -7, "PCT");

    consoleDrawText(1, y++, "END OK");
    setScreenOn();
    while (1) {
        WaitForVBlank();
    }
    return 0;
}
