/*
 * bench.c - noyaux de calcul typiques d'un jeu, chronometres en frames (VBlank).
 * Meme source pour 816-tcc et llvm-mos.
 */
#include <snes.h>

#ifndef FAR
#define FAR
#endif

#define NOBJ 64
typedef struct { s16 x, y, vx, vy; u8 alive; } obj_t;
obj_t objs[NOBJ];
u16 arr[100];
u16 map[32 * 16];
u8 src[1024], dst[1024];

static u16 t0;
static void tic(void) { WaitForVBlank(); t0 = snes_vblank_count; }
static u16 toc(void) { return snes_vblank_count - t0; }

static void k_sort(void)
{
    u16 i, j, t, pass;
    for (pass = 0; pass < 3; pass++) {
        for (i = 0; i < 100; i++) arr[i] = (u16)(i * 7919u) ^ 0x5A5A;
        for (i = 0; i < 99; i++)
            for (j = 0; j < 99 - i; j++)
                if (arr[j] > arr[j + 1]) { t = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = t; }
    }
}

static void k_physics(void)
{
    u16 f, i;
    for (f = 0; f < 60; f++)
        for (i = 0; i < NOBJ; i++) {
            obj_t *o = &objs[i];
            if (!o->alive) continue;
            o->vy += 3;
            o->x += o->vx;
            o->y += o->vy;
            if (o->x < 0 || o->x > 255 * 16) { o->vx = -o->vx; o->x += o->vx; }
            if (o->y > 223 * 16) { o->vy = -(o->vy * 3) / 4; o->y = 223 * 16; }
        }
}

static void k_map(void)
{
    u16 r, x, y;
    for (r = 0; r < 20; r++)
        for (y = 0; y < 16; y++)
            for (x = 0; x < 32; x++)
                map[y * 32 + x] = (x + r) | ((y ^ r) << 5) | 0x2000;
}

static u16 fib(u16 n) { return n < 2 ? n : fib(n - 1) + fib(n - 2); }

static void k_copy(void)
{
    u16 r, i;
    for (r = 0; r < 30; r++)
        for (i = 0; i < 1024; i++) dst[i] = src[i] + (u8)r;
}

static u16 k_math(void)
{
    u16 i, acc = 0;
    for (i = 1; i < 2000; i++) acc += (u16)(i * 13) / (i & 15 | 1) + i % 10;
    return acc;
}

int main(void)
{
    u16 i, t, y = 2, chk;

    consoleInitDefaultText(0);
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

    for (i = 0; i < NOBJ; i++) {
        objs[i].x = i * 60; objs[i].y = i * 30;
        objs[i].vx = (i & 7) - 3; objs[i].vy = 0; objs[i].alive = (i % 5) != 0;
    }
    for (i = 0; i < 1024; i++) src[i] = (u8)i;

    consoleDrawText(1, 0, "BENCH (FRAMES)");
    tic(); k_sort();    t = toc(); consoleDrawText(1, y++, "SORT    %u  %u", t, arr[50]);
    tic(); k_physics(); t = toc(); consoleDrawText(1, y++, "PHYSICS %u  %d", t, objs[7].y);
    tic(); k_map();     t = toc(); consoleDrawText(1, y++, "MAP     %u  %u", t, map[300]);
    tic(); chk = fib(18); t = toc(); consoleDrawText(1, y++, "FIB18   %u  %u", t, chk);
    tic(); k_copy();    t = toc(); consoleDrawText(1, y++, "COPY    %u  %u", t, dst[500]);
    tic(); chk = k_math(); t = toc(); consoleDrawText(1, y++, "MATH    %u  %u", t, chk);
    consoleDrawText(1, y++, "DONE");
    while (1) WaitForVBlank();
    return 0;
}
