/*---------------------------------------------------------------------------------


    Simple breakout game
    Base on SNES Sdk game from Ulrich Hecht
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>
#include <string.h>

//---------------------------------------------------------------------------------
extern char tiles1; // external assets
extern char tiles2;
extern char bg1map;
extern char bg2map;
extern char palette;
extern char backpal;

typedef struct
{
    s16 x, y;
} Vec2;

//---------------------------------------------------------------------------------
const Vec2 dir[4] = {
    {-2, -1}, {-1, -2}, {1, -2}, {2, -1}};

const u8 map[0x64] = {
    // default wall of bricks
    7,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    7,
    8,
    7,
    8,
    7,
    8,
    8,
    7,
    8,
    7,
    8,
    8,
    8,
    7,
    8,
    7,
    7,
    8,
    7,
    8,
    8,
    8,
    8,
    8,
    1,
    3,
    3,
    1,
    8,
    8,
    8,
    8,
    0,
    4,
    8,
    8,
    8,
    8,
    4,
    0,
    8,
    8,
    0,
    8,
    8,
    5,
    5,
    8,
    8,
    0,
    8,
    8,
    0,
    4,
    8,
    8,
    8,
    8,
    4,
    0,
    8,
    8,
    8,
    8,
    1,
    3,
    3,
    1,
    8,
    8,
    8,
    8,
    8,
    6,
    8,
    6,
    6,
    8,
    6,
    8,
    8,
    7,
    7,
    7,
    7,
    8,
    8,
    7,
    7,
    7,
    7,
};

#define ST_READY "PLAYER 1\n\n READY"
#define ST_GAMEOVER "GAME OVER"
#define ST_PAUSED "PAUSE"
#define ST_BLANK "        "

//---------------------------------------------------------------------------------
u16 blockmap[0x400];
u16 backmap[0x400];
u16 pal[0x100];
u8 blocks[0x64];

u8 i, j, k;              // loop variables
u16 a, c, b;             // bricks for init table
u16 blockcount;          // number of bricks on screen
u16 bx, by;              // ball coordinates
u16 obx, oby;            // for brick test
u16 score, hiscore;      // score & hiscore
u16 level2;              // max level
u16 color, level, lives; // color of BG, current level & number of lives
u16 px;                  // paddle x coordinate
Vec2 vel;                // Velocity of ball
Vec2 pos;                // Coordinates of ball
u16 pad0;                // pad#0 status

//---------------------------------------------------------------------------------
u16 clamp(u16 val, u16 min, u16 max)
{
    if (val < min)
        val = min;

    if (val > max)
        val = max;

    return val;
}

//---------------------------------------------------------------------------------
void writestring(const char *st, u16 *map, u16 p, u16 offset)
{
    u16 sp = p; // start position
    u8 c;

    while ((c = *st))
    {
        if (c == '\n')
        {
            sp += 0x20;
            p = sp;
        }
        else
        {
            map[p] = c + offset;
            p++;
        }

        st++;
    }
}

//---------------------------------------------------------------------------------
void writenum(u16 num, u8 len, u16 *map, u16 p, u16 offset)
{
    u8 figure;
    p += len - 1;

    if (!num)
    {
        map[p] = offset;
    }
    else
    {
        while (len && num)
        {
            figure = num % 10;

            if (num || figure)
                map[p] = figure + offset;

            num /= 10;
            p--;
            len--;
        }
    }
}

//---------------------------------------------------------------------------------
void draw_screen(void)
{
    // main sprites (ball & paddle) (sprites are automaticaly update in VBlank function of PVSneslib)
    // id (multiple of 4),  xspr, yspr, priority, hflip, vflip, gfxoffset, paletteoffset
    oamSet(0, pos.x, pos.y, 3, 0, 0, 20 | (1 << 8), 0);
    oamSet(1 * 4, px + 0, 200, 3, 0, 0, 15 | (1 << 8), 0);
    oamSet(2 * 4, px + 8, 200, 3, 0, 0, 16 | (1 << 8), 0);
    oamSet(3 * 4, px + 16, 200, 3, 1, 0, 16 | (1 << 8), 0);
    oamSet(4 * 4, px + 24, 200, 3, 0, 0, 17 | (1 << 8), 0);

    // shadow sprites
    oamSet(5 * 4, pos.x + 3, pos.y + 3, 1, 0, 0, 21 | (1 << 8), 0);
    oamSet(6 * 4, px + 4, 204, 1, 0, 0, 18 | (1 << 8), 0);
    oamSet(7 * 4, px + 12, 204, 1, 0, 0, 19 | (1 << 8), 0);
    oamSet(8 * 4, px + 20, 204, 1, 1, 0, 19 | (1 << 8), 0);
    oamSet(9 * 4, px + 28, 204, 1, 0, 0, 18 | (1 << 8), 0);
}

//---------------------------------------------------------------------------------
void new_level(void)
{
    // Update all variables regarding levels
    level++;
    level2++;
    pos.x = 94;
    pos.y = 109;
    px = 80;
    writenum(level2, 8, blockmap, 0x2D6, 0x426);
    writestring(ST_READY, blockmap, 0x248, 0x3F6);

    // Change backgrounds
    memcpy((u8 *)&backmap, &bg2map + 0x800 * (level & 3), 0x800);
    memcpy((u8 *)&blocks, &map, 0x64);

    // Manage color of background
    if (color < 6)
    {
        color++;
    }
    else
        color = 0;

    // Change background color
    memcpy((u8 *)&pal + 16, &backpal + color * 16, 0x10);

    // Init the wall of bricks
    b = 0;
    for (j = 0; j < 10; j++)
    {
        for (i = 0; i < 20; i++, i++)
        {
            a = blocks[b];
            if (a < 8)
            {
                c = (j << 5) + i;
                blockcount++;
                blockmap[0x62 + c] = 13 + (a << 10);
                blockmap[0x63 + c] = 14 + (a << 10);
                backmap[0x83 + c] += 0x400;
                backmap[0x84 + c] += 0x400;
            }
            b++;
        }
    }

    // Reinit pall and backgrounds
    WaitForVBlank();
    dmaCopyCGram((u8 *)pal, 0, 256 * 2);
    dmaCopyVram((u8 *)blockmap, 0x0000, 0x800);
    dmaCopyVram((u8 *)backmap, 0x0400, 0x800);

    // Put again paddle and ball
    draw_screen();

    // Wait a key pressed
    while (padsCurrent(0) == 0)
        WaitForVBlank();

    // Remove message on screen
    writestring(ST_BLANK, blockmap, 0x248, 0x3F6);
    writestring(ST_BLANK, blockmap, 0x289, 0x3F6);
    WaitForVBlank();
    dmaCopyVram((u8 *)blockmap, 0x0000, 0x800);
}

//---------------------------------------------------------------------------------
void die(void)
{
    // hugly management when no more live (yeah, we do an infinite loop :P)
    if (lives == 0)
    {
        writestring(ST_GAMEOVER, blockmap, 0x267, 0x3F6);
        WaitForVBlank();
        dmaCopyVram((u8 *)blockmap, 0x0000, 0x800);
        while (1)
            ;
    }

    // Rinit all var for ball & paddle, also remove one live
    lives--;
    pos.x = 94;
    pos.y = 109;
    px = 80;

    // Put a message on screen to start again
    writenum(lives, 8, blockmap, 0x136, 0x426);
    writestring(ST_READY, blockmap, 0x248, 0x3F6);
    WaitForVBlank();
    dmaCopyVram((u8 *)blockmap, 0x0000, 0x800);

    // Put again paddle and ball
    draw_screen();

    // Wait a key pressed
    while (padsCurrent(0) == 0)
        WaitForVBlank();

    // Remove message
    writestring(ST_BLANK, blockmap, 0x248, 0x3F6);
    writestring(ST_BLANK, blockmap, 0x289, 0x3F6);
    WaitForVBlank();
    dmaCopyVram((u8 *)blockmap, 0x0000, 0x800);
}

//---------------------------------------------------------------------------------
void handle_pause(void)
{
    // If we push START button
    if ((pad0 & KEY_START) != 0)
    {
        // Put a message on screen
        writestring(ST_PAUSED, blockmap, 0x269, 0x3F6);
        WaitForVBlank();
        dmaCopyVram((u8 *)blockmap, 0x0000, 0x800);

        // Wait for START released
        while (padsCurrent(0) != 0)
        {
            WaitForVBlank();
        }

        // Wait for START pressed again
        while ((padsCurrent(0) & KEY_START) == 0)
            WaitForVBlank();

        // Wait for START released
        while (padsCurrent(0) != 0)
        {
            WaitForVBlank();
        }

        // Remove message on screen
        writestring(ST_BLANK, blockmap, 0x269, 0x3F6);
        WaitForVBlank();
        dmaCopyVram((u8 *)blockmap, 0x0000, 0x800);
    }
}

//---------------------------------------------------------------------------------
void run_frame(void)
{
    // Get current pad #0 status
    pad0 = padsCurrent(0);

    // Do we push START for pause ?
    handle_pause();

    // If A pressed, do a fast move
    if (pad0 & KEY_A)
    {
        if (pad0 & KEY_RIGHT)
            px += 4;

        if (pad0 & KEY_LEFT)
            px -= 4;
    }
    // Not A, so a normal move
    else
    {
        if (pad0 & KEY_RIGHT)
            px += 2;

        if (pad0 & KEY_LEFT)
            px -= 2;
    }

    // Limit x coordinate of paddle
    px = clamp(px, 16, 144);

    // Increase ball speed with velocity
    pos.x += vel.x;
    pos.y += vel.y;

    // Change ball move regarding the walls
    if (pos.x > 171)
    {
        vel.x = -vel.x;
        pos.x = 171;
    }
    else if (pos.x < 16)
    {
        vel.x = -vel.x;
        pos.x = 16;
    }

    // Now check ball regarding bottom of screen or bricks
    if (pos.y < 15)
    { // top of screen
        vel.y = -vel.y;
    }
    else if (pos.y > 195)
    {
        // are we on the padddle ?
        if (pos.y < 203)
        {
            if ((pos.x >= px) && (pos.x <= px + 27))
            {
                k = (pos.x - px) / 7;
                vel.x = dir[k].x;
                vel.y = dir[k].y;
            }
        }
        // are we on bottom of screen
        else if (pos.y > 224)
        {
            die();
        }
    }
    else if (pos.y < 112)
    {
        obx = bx;
        oby = by;
        bx = (pos.x - 14) >> 4;
        by = (pos.y - 14) >> 3;
        b = bx + (by << 3) + (by << 1) - 10;

        if ((b >= 0) && (b < 100))
        {
            // is the brick still here ?
            if (blocks[b] != 8)
            {
                blockcount--;
                for (i = 0; i <= level; i++)
                    score += blocks[b] + 1;

                if (oby != by)
                    vel.y = -vel.y;

                if (obx != bx)
                    vel.x = -vel.x;

                // Remove brick from screen
                blocks[b] = 8;
                b = (by << 5) + (bx << 1);
                blockmap[0x42 + b] = 0;
                blockmap[0x43 + b] = 0;
                backmap[0x63 + b] -= 0x400;
                backmap[0x64 + b] -= 0x400;
                writenum(score, 8, blockmap, 0xF5, 0x426);

                if (score > hiscore)
                {
                    hiscore = score;
                    writenum(score, 8, blockmap, 0x95, 0x426);
                }
                // Update now screen
                WaitForVBlank();
                dmaCopyVram((u8 *)blockmap, 0x0000, 0x800);
                dmaCopyVram((u8 *)backmap, 0x0400, 0x800);

                // if no more bricks, do a new level
                if (blockcount == 0)
                {
                    new_level();
                }
            }
        }
    }

    // Update again sprites
    draw_screen();

    // Wait for vectical refresh (one frame)
    WaitForVBlank();
}

//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize SNES
    consoleInit();

    // Put screen off and Wait VBlank to allow us to update VRAM
    setBrightness(0);
    WaitForVBlank();

    // load tiles into VRAM address 0x1000 and 0x2000
    dmaCopyVram(&tiles1, 0x1000, 0xF00);
    dmaCopyVram(&tiles2, 0x2000, 0x250);

    // Copy dat files to ram var to be able to modify them
    memcpy((u8 *)&blockmap, &bg1map, 0x800);
    memcpy((u8 *)&backmap, &bg2map, 0x800);
    memcpy((u8 *)&blocks, &map, 0x64);
    memcpy((u8 *)&pal, &palette, 0x200);

    // Init global variables of the game
    blockcount = 0;
    bx = 5;
    by = 11;
    score = 0;
    hiscore = 50000;
    level2 = 1;
    color = 0;
    level = 0;
    lives = 4;
    px = 80;
    vel.x = 2;
    vel.y = 1;
    pos.x = 94;
    pos.y = 109;

    // Init map with all bricks
    b = 0;
    for (j = 0; j < 10; j++)
    {
        for (i = 0; i < 20; i += 2)
        {
            a = blocks[b++];

            if (a < 8)
            {
                c = (j << 5) + i;
                blockcount++;
                blockmap[0x62 + c] = 13 + (a << 10);
                blockmap[0x63 + c] = 14 + (a << 10);
                backmap[0x83 + c] += 0x400;
                backmap[0x84 + c] += 0x400;
            }
        }
    }

    // Update board with number of lives and the PLAYER1 READY text
    writenum(lives, 8, blockmap, 0x136, 0x426);
    writestring(ST_READY, blockmap, 0x248, 0x3F6);

    // Wait again VBlank to avoid glitches
    WaitForVBlank();

    // Init map BG0 & BG2 address and put data inside them
    bgInitMapSet(0, (u8 *)blockmap, 0x800, SC_32x32, 0x0000);
    bgInitMapSet(2, (u8 *)backmap, 0x800, SC_32x32, 0x0400);

    // Same thing for palette
    dmaCopyCGram((u8 *)pal, 0, 256 * 2);

    // Init graphics pointeur for each BG
    bgSetGfxPtr(0, 0x1000);
    bgSetGfxPtr(2, 0x2000);

    // Now Put in 16 color mode and disable BG2
    setMode(BG_MODE1, 0);
    bgSetDisable(1);

    // Put screen on
    setScreenOn();

    // Update the sprites on screen
    draw_screen();

    // The 1st time, you need to show them
    for (i = 0; i < 10 * 4; i += 4)
        oamSetEx(i, OBJ_SMALL, OBJ_SHOW);

    // Wait a key pressed
    while (padsCurrent(0) == 0)
        WaitForVBlank();

    // Remove the PLAYER1 READY text (need to wait VBlank to be sure of no glitches)
    writestring(ST_BLANK, blockmap, 0x248, 0x3F6);
    writestring(ST_BLANK, blockmap, 0x289, 0x3F6);
    WaitForVBlank();
    dmaCopyVram((u8 *)blockmap, 0x0000, 0x800);

    // Wait for nothing :P
    while (1)
    {
        // do a frame
        run_frame();
    }
    return 0;
}
