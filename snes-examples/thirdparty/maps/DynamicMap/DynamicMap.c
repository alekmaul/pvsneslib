/*---------------------------------------------------------------------------------


    DynamicMap

    Create and update 16x16 sprites in RAM on a 32x32 or 64x64 map.

---------------------------------------------------------------------------------*/
#include <snes.h>
#include <snes/interrupt.h>

#include "maputil.h"
#include "map32x32.h"
#include "map64x64.h"

#define SPRITE_EMPTY 0
#define SPRITE_GARGOYLE 1
#define SPRITE_ROCKFORD 2

extern char snesfont, snespal;
extern char sprite16, sprite16_end, palsprite16, palsprite16_end;
extern char sprite16_64x64, sprite16_64x64_end, palsprite16_64x64, palsprite16_64x64_end;
extern char c64_sprite;

// map32x32 or map64x64 (visible: 16x14)
u16 spritemap_len = 0x2000;
u16 spritemap[0x2000];

int number_of_sprites = 0x40; // keep in sync with ram.asm

//char sprites_ram[0x40 * 256]; // this will collide with 0x7E8000 RAM, so using ramsection 0x7F (ram.asm)
extern char sprites_ram;

// no endless scrolling
bool scroll_lock = true;

// 32x32 or 64x64 map
bool map32x32 = true;

u16 max_scroll_width = MAX_SCROLL_WIDTH_32x32;
u16 max_scroll_height = MAX_SCROLL_HEIGHT_32x32;

void refresh()
{
    screenRefresh(spritemap, 0x1000);
}

void copySpriteToRAM_32x32(u8 *from, u16 index)
{
    // 16x16 sprite, 256 colors, 256 bytes
    u16 i;
    for (i = 0; i < 256; i++)
    {
        *(&sprites_ram + index + i) = *(from + i);
    }
}
void copySpriteToRAM64x64(u8 *from, u16 index)
{
    // 16x16 sprite, 256 colors, 256 bytes
    // top    = 128 bytes
    // bottom = 128 bytes
    // between top and bottom is a 1024 bytes gap!
    // => sprites can be interleaved!

    u16 i;

    // copy top
    for (i = 0; i < 128; i++)
    {
        *(&sprites_ram + index + i) = *(from + i);
    }

    // copy bottom
    for (i = 1024; i < 1024 + 128; i++)
    {
        *(&sprites_ram + index + i) = *(from + i);
    }
}

/**
 * Just to convert the 256 bytes sprite16 to RAM.
 * This makes the 2048 bytes sprite16_64x64 superfluous!
 */
void copySpriteToRAM64x64_alt(u8 *from, u16 index)
{
    // 16x16 sprite, 256 colors, 256 bytes
    // top    = 128 bytes
    // bottom = 128 bytes
    // between top and bottom is a 1024 bytes gap!
    // => sprites can be interleaved!

    u16 i;

    // copy top
    for (i = 0; i < 128; i++)
    {
        *(&sprites_ram + index + i) = *(from + i);
    }

    // copy bottom
    for (i = 1024; i < 1024 + 128; i++)
    {
        *(&sprites_ram + index + i) = *(from + i - 1024 + 128);
    }
}

void resetRAM32x32(u16 index)
{
    // 16x16 sprite, 256 colors, 256 bytes
    u16 i;
    for (i = 0; i < 256; i++)
    {
        *(&sprites_ram + index + i) = 0;
    }
}

void resetRAM64x64(u16 index)
{
    u16 i;

    // reset top
    for (i = 0; i < 128; i++)
    {
        *(&sprites_ram + index + i) = 0;
    }

    // reset bottom
    for (i = 1024; i < 1024 + 128; i++)
    {
        *(&sprites_ram + index + i) = 0;
    }
}

bool isBitSet(u8 b, u8 index)
{
    return ((b >> index) & 1) == 1;
}

/**
 * Get pixel from a C64 sprite.
 *
 * C64 sprite spec:
 * 8x16 pixels (32 bytes), NOTE: will be stretched to 16x16 on screen
 * quarter tile: 4x8 pixels (8 bytes)
 * quarter order: top left, top right, bottom left, bottom right
 * 16 colors (4 colors effectively)
 * colors:
 *     00 = black
 *     01 = orange
 *     10 = grey
 *     11 = white
 *
 * @param chr_no c64 sprite
 * @param tile 0 - 3
 * @param x 0 - 7
 * @param y 0 - 7
 * @return color
 */
u8 getPixel(u8 chr_no, u8 tile, u8 x, u8 y)
{
    vuint32 c64_sprite_base = (vuint32) &c64_sprite;

    // stretch by resetting to even number
    int x_even = x & 0b11111110;
    if (tile >= 2)
    {
        tile -= 2;
        chr_no += 0x10; // bottom half of C64 sprites are shifted
    }

    u8 row = *((u8*)(c64_sprite_base + chr_no*8 + 8*tile + y));

    // C64 sprite has 2-bit color depth
    bool bit0 = isBitSet(row, 7 - x_even);
    bool bit1 = isBitSet(row, 7 - x_even - 1);
    u8 color = (bit1 << 1) | bit0;

    return color;
}

/**
 * Convert C64 sprite to SNES format.
 *
 * @param chr_no c64 sprite to convert
 * @param index index of address area to copy sprite to
 * @param map32x32 32x32 or 64x64 map
 */
void convertC64Sprite(u8 chr_no, u16 index, bool map32x32)
{
    consoleNocashMessage("sprite: 0x%x, index=0x%x\n", (int) chr_no, index);

    // 2^8 = 256 colors, 8 bitplanes
    // 8x8 tile size
    // 8 rows * 8 bitplanes = 64 bytes * 4 tiles = 256 bytes

    // Convert2PicLZ77 from https://github.com/alekmaul/pvsneslib/blob/master/tools/gfx2snes/imgtools.c
    u8 num_tiles = 4;
    u8 bitplanes = 8;
    u8 mask;
    u8 t;
    u8 b;
    u16 x;
    u16 y;
    unsigned char data;

    for (t = 0; t < num_tiles; t++)        // loop through tiles
    {
        // added for map32x32: sprite top and bottom have a distance of 1024 bytes
        if (!map32x32 && t == 2)
            index += 1024 - 128; // sprite top length is 128 bytes

        for (b = 0; b < bitplanes; b += 2) // loop through bitplane pairs
        {
            for (y = 0; y < 8; y++)
            {
                // get bit-mask
                mask = 1 << b;
                data = 0;

                // get row of bit-plane and save row
                for (x = 0; x < 8; x++)
                {
                    data = data << 1;
                    if (getPixel(chr_no, t, x, y) & mask)
                        data = data + 1;
                }
                *(&sprites_ram + index++) = data;

                // adjust bit-mask
                mask = mask << 1;
                data = 0;

                // get row of next bit-plane and save row
                for (x = 0; x < 8; x++)
                {
                    data = data << 1;
                    if (getPixel(chr_no, t, x, y) & mask)
                        data = data + 1;
                }
                *(&sprites_ram + index++) = data;
            }
        }
    }
}

void initDemoMap32x32()
{
    map32x32 = true;
    max_scroll_width = MAX_SCROLL_WIDTH_32x32;
    max_scroll_height = MAX_SCROLL_HEIGHT_32x32;

    setMode(BG_MODE3, BG1_TSIZE8x8);

    setSpriteMap32x32(spritemap, spritemap_len);
    initSpriteMap32x32();

    u16 sprites_len = calculateSpritesLength32x32(number_of_sprites);
    consoleNocashMessage("sprites.addr: 0x%x\n", &sprites_ram);
    consoleNocashMessage("sprites_len=0x%x\n", sprites_len);

    // init empty sprite (required if emulator does not clear RAM)
    resetRAM32x32(calculateSpriteIndex32x32(0));

    u16 i;
    // copy sprites to RAM, for this example just use the same graphics for sprites 1-9
    for (i = 1; i < 10; i++)
    {
        u16 index = calculateSpriteIndex32x32(i);
        copySpriteToRAM_32x32(&sprite16, index); // Gargoyle sprite: 16x16, 256 colors, 256 bytes
        consoleNocashMessage("element: 0x%x, sprite: 0x%x, index: 0x%x\n", (int) i, element2sprite32x32(i), index);
        /* output:
           element: 0x1, sprite: 0x4, index: 0x100
           element: 0x2, sprite: 0x8, index: 0x200
           element: 0x3, sprite: 0xc, index: 0x300
           element: 0x4, sprite: 0x10, index: 0x400
           element: 0x5, sprite: 0x14, index: 0x500
           element: 0x6, sprite: 0x18, index: 0x600
           element: 0x7, sprite: 0x1c, index: 0x700
           element: 0x8, sprite: 0x20, index: 0x800
           element: 0x9, sprite: 0x24, index: 0x900
        */
    }

    //bgInitTileSet(0, &sprite16, &palsprite16, 0, (&sprite16_end - &sprite16), 16 * 2, BG_256COLORS, 0x4000); // Gargoyle from ROM
    bgInitTileSet(0, (u8*) &sprites_ram, &palsprite16, 0, sprites_len, 16 * 2, BG_256COLORS, 0x4000);
    bgInitMapSet(0, (u8*) spritemap, spritemap_len, SC_64x64, 0x1000);
}

void initDemoMap64x64()
{
    map32x32 = false;
    max_scroll_width = MAX_SCROLL_WIDTH_64x64;
    max_scroll_height = MAX_SCROLL_HEIGHT_64x64;

    setMode(BG_MODE3, BG1_TSIZE16x16);

    setSpriteMap64x64(spritemap, spritemap_len);
    initSpriteMap64x64();

    u16 sprites_len = calculateSpritesLength64x64(number_of_sprites); // keep in sync with ram.asm
    consoleNocashMessage("sprites.addr: 0x%x\n", &sprites_ram);
    consoleNocashMessage("sprites_len=0x%x\n", sprites_len);

    // init empty sprite (required if emulator does not clear RAM)
    resetRAM64x64(calculateSpriteIndex64x64(0));

    u16 i;
    // copy sprites to RAM, for this example just use the same graphics for sprites 1-9
    for (i = 1; i < 9; i++)
    {
        u16 index = calculateSpriteIndex64x64(i);
        copySpriteToRAM64x64(&sprite16_64x64, index); // Gargoyle sprite: 16x16, 256 colors, 2048 bytes
        //copySpriteToRAM64x64_alt(&sprite16, index); // alternative: reuse the 256 bytes sprite
        consoleNocashMessage("element: 0x%x, sprite: 0x%x, index: 0x%x\n", (int) i, element2sprite64x64(i), index);
        /* output:
        element: 0x1, sprite: 0x2, index: 0x80
        element: 0x2, sprite: 0x4, index: 0x100
        element: 0x3, sprite: 0x6, index: 0x180
        element: 0x4, sprite: 0x8, index: 0x200
        element: 0x5, sprite: 0xa, index: 0x280
        element: 0x6, sprite: 0xc, index: 0x300
        element: 0x7, sprite: 0xe, index: 0x380
        element: 0x8, sprite: 0x20, index: 0x800
        element: 0x9, sprite: 0x22, index: 0x880
        */
    }

    //bgInitTileSet(0, &sprite16_64x64, &palsprite16_64x64, 0, (&sprite16_64x64_end - &sprite16_64x64), 16 * 2, BG_256COLORS, 0x4000); // Gargoyle from ROM
    bgInitTileSet(0, (u8*) &sprites_ram, &palsprite16_64x64, 0, sprites_len, 16 * 2, BG_256COLORS, 0x4000);
    bgInitMapSet(0, (u8*) spritemap, spritemap_len, SC_64x64, 0x1000);
}

void drawSpriteFrame32x32(u16 sprite)
{
    u8 x;
    u8 y;
    for (x = 0; x < 32; x++)
        for (y = 0; y < 32; y++)
            if (x == 0 || y == 0 || x == 31 || y == 31)
                drawSprite32x32(x, y, element2sprite32x32(sprite));
}

void drawSpriteFrame64x64(u16 sprite)
{
    u8 x;
    u8 y;
    for (x = 0; x < 64; x++)
        for (y = 0; y < 64; y++)
            if (x == 0 || y == 0 || x == 63 || y == 63)
                drawSprite64x64(x, y, element2sprite64x64(sprite));
}

void testGetSprite32x32()
{
    u8 x = rand() % 31;
    u8 y = rand() % 31;
    //consoleNocashMessage("%u,%u\n", (int) x, (int) y);
    u16 sprite = element2sprite32x32(SPRITE_GARGOYLE);
    drawSprite32x32(x, y, sprite);
    WaitForVBlank();
    screenRefreshPos32x32(x, y, 0x1000);

    u16 sprite2 = getSprite32x32(x, y);
    if (sprite == sprite2)
        consoleNocashMessage("TEST OK: testGetSprite32x32(): 0x%x == 0x%x\n", sprite, sprite2);
    else
        consoleNocashMessage("TEST FAILED: testGetSprite32x32(): 0x%x != 0x%x\n", sprite, sprite2);
}

void testGetSprite64x64()
{
    u8 x = rand() % 63;
    u8 y = rand() % 63;
    //consoleNocashMessage("%u,%u\n", (int) x, (int) y);
    u16 sprite = element2sprite64x64(SPRITE_GARGOYLE);
    drawSprite64x64(x, y, sprite);
    WaitForVBlank();
    screenRefreshPos64x64(x, y, 0x1000);

    u16 sprite2 = getSprite64x64(x, y);
    if (sprite == sprite2)
        consoleNocashMessage("TEST OK: testGetSprite64x64(): 0x%x == 0x%x\n", sprite, sprite2);
    else
        consoleNocashMessage("TEST FAILED: testGetSprite64x64(): 0x%x != 0x%x\n", sprite, sprite2);
}

//---------------------------------------------------------------------------------
int main()
{
    // Initialize text console with our font
    consoleSetTextMapPtr(0x6800);
    consoleSetTextGfxPtr(0x3000);

    consoleSetTextOffset(0x0100);
    consoleInitText(0, 16 * 2, &snesfont, &snespal);

    // Init background
    bgSetGfxPtr(1, 0x2000);
    bgSetMapPtr(1, 0x6800, SC_32x32);

    consoleDrawText(6, 10, "DynamicMap");
    consoleDrawText(6, 12, "A = Map size 32x32");
    consoleDrawText(6, 14, "B = Scroll lock ON ");
    consoleDrawText(6, 16, "X = Random sprite");
    consoleDrawText(6, 18, "Y = Convert C64 sprite");
    consoleDrawText(6, 20, "DPAD = Scroll map");

    initDemoMap32x32();
    setScreenOn();
    drawSpriteFrame32x32(SPRITE_GARGOYLE);
    refresh();

    short sxbg0 = 0;
    short sybg0 = 0;

    u16 pad0;
    u16 pad0_up;

    consoleNocashMessage("c64_sprite.addr=0x%x\n", &c64_sprite);

    while (true)
    {
        //consoleNocashMessage("x=%d y=%d\n", sxbg0, sybg0);
        pad0 = padsCurrent(0);
        pad0_up = padsUp(0);
        if (pad0 & KEY_RIGHT)
        {
            if (!scroll_lock)
                sxbg0 += 4;
            else if (sxbg0 < max_scroll_width)
                sxbg0 += 4;
        }
        else if (pad0 & KEY_LEFT)
        {
            if (!scroll_lock)
                sxbg0 -= 4;
            else if (sxbg0 > 0)
                sxbg0 -= 4;
        }
        if (pad0 & KEY_UP)
        {
            if (!scroll_lock)
                sybg0 -= 4;
            else if (sybg0 > 0)
                sybg0 -= 4;
        }
        else if (pad0 & KEY_DOWN)
        {
            if (!scroll_lock)
                sybg0 += 4;
            else if (sybg0 < max_scroll_height)
                sybg0 += 4;
        }
        if (pad0_up & KEY_A)
        {
            map32x32 = !map32x32;
            if (map32x32)
            {
                consoleDrawText(6, 12, "A = Map size 32x32");
                setScreenOff();
                initDemoMap32x32();
                setScreenOn();
                drawSpriteFrame32x32(SPRITE_GARGOYLE);
                refresh();
            }
            else
            {
                consoleDrawText(6, 12, "A = Map size 64x64");
                setScreenOff();
                initDemoMap64x64();
                setScreenOn();
                drawSpriteFrame64x64(SPRITE_GARGOYLE);
                refresh();
            }
            sxbg0 = 0;
            sybg0 = 0;
        }
        if (pad0_up & KEY_B)
        {
            scroll_lock = !scroll_lock;
            if (scroll_lock)
                consoleDrawText(6, 14, "B = Scroll lock ON ");
            else
                consoleDrawText(6, 14, "B = Scroll lock OFF");
        }
        if (pad0 & KEY_X)
        {
            if (map32x32)
                testGetSprite32x32();
            else
                testGetSprite64x64();
        }
        if (pad0 & KEY_Y)
        {
            if (map32x32)
            {
                convertC64Sprite(0, calculateSpriteIndex32x32(SPRITE_ROCKFORD), true);
                WaitForVBlank();
                updateSprite32x32(&sprites_ram, 0x4000, SPRITE_ROCKFORD);
                drawSpriteFrame32x32(SPRITE_ROCKFORD);
                refresh();
            }
            else
            {
                convertC64Sprite(0, calculateSpriteIndex64x64(SPRITE_ROCKFORD), false);
                WaitForVBlank();
                updateSprite64x64(&sprites_ram, 0x4000, SPRITE_ROCKFORD);
                drawSpriteFrame64x64(SPRITE_ROCKFORD);
                refresh();
            }
        }

        bgSetScroll(0, sxbg0, sybg0);
        WaitForVBlank();
    }

    return 0;
}
