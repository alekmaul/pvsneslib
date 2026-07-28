#include <snes/dma.h>
#include <snes/console.h>

u16 spritemap32x32_len = 0;
u16* spritemap32x32;

void setSpriteMap32x32(u16* spritemap, u16 spritemap_len)
{
    spritemap32x32 = spritemap;
    spritemap32x32_len = spritemap_len;
}

u16* getSpriteMap32x32(u16* spritemap)
{
    return spritemap32x32;
}

u16 getSpriteMap32x32_length()
{
    return spritemap32x32_len;
}

void initSpriteMap32x32()
{
    u16 i;
    for (i = 0; i < spritemap32x32_len; i++)
    {
        spritemap32x32[i] = 0;
    }
}

/**
 * Draw a sprite on a 32 x 32 map. The map consists of four 16 x 16 maps, which are ordered this way:
 * Top left, top right, bottom left, bottom right.
 * The top left 16 x 16 map begins with y = 0.
 * The top right 16 x 16 map begins with y = 16.
 * The bottom left 16 x 16 map begins with y = 32.
 * The bottom right 16 x 16 map begins with y = 48.
 *
 * @param x 0 - 15
 * @param y 0 - 63
 */
void drawSpriteRaw32x32(u8 x, u8 y, u16 sprite)
{
    // i = colsMax * row + col; // for 1 tile, but we have 4 tiles per sprite
    u16 i = 64 * y + x * 2;

    // four tiles
    spritemap32x32[i]      = sprite;
    spritemap32x32[i + 1]  = sprite + 1;
    spritemap32x32[i + 32] = sprite + 2;
    spritemap32x32[i + 33] = sprite + 3;
}

getSpriteRaw32x32(u8 x, u8 y)
{
    // i = colsMax * row + col; // for 1 tile, but we have 4 tiles per sprite
    u16 i = 64 * y + x * 2;
    return spritemap32x32[i];
}

/**
 * Draw a sprite on a 32 x 32 map.
 *
 * @param x column 0 - 31
 * @param y row 0 - 31
 * @param sprite sprite
 */
void drawSprite32x32(u8 x, u8 y, u16 sprite)
{
    if (x < 16 && y < 16)
        drawSpriteRaw32x32(x, y, sprite); // top left
    else if (x < 32 && y < 16)
        drawSpriteRaw32x32(x - 16, y + 16, sprite); // top right
    else if (x < 16 && y < 32)
        drawSpriteRaw32x32(x, y + 16, sprite); // bottom left
    else if (x < 32 && y < 32)
        drawSpriteRaw32x32(x - 16, y + 32, sprite); // bottom right
    else
        consoleNocashMessage("drawSprite32x32: out of bounds: x=%u,y=%u\n", (int) x, (int) y);
}

u16 getSprite32x32(u8 x, u8 y)
{
    if (x < 16 && y < 16)
        return getSpriteRaw32x32(x, y); // top left
    else if (x < 32 && y < 16)
        return getSpriteRaw32x32(x - 16, y + 16); // top right
    else if (x < 16 && y < 32)
        return getSpriteRaw32x32(x, y + 16); // bottom left
    else if (x < 32 && y < 32)
        return getSpriteRaw32x32(x - 16, y + 32); // bottom right
    else
    {
        consoleNocashMessage("drawSprite32x32: out of bounds: x=%u,y=%u\n", (int) x, (int) y);
        return 0;
    }
}

/**
 * Convert element to its sprite index.
 *
 * @param elem gfx tile no 0 - (number_of_sprites - 1)
 * @return sprite index
 */
u16 element2sprite32x32(u8 elem)
{
    return elem * 4; // BG1_TSIZE8x8
}

u16 calculateSpriteIndex32x32(u8 elem)
{
    return elem * 256; // BG1_TSIZE8x8
}

u16 calculateSpritesLength32x32(u16 number_of_sprites)
{
    return 256 * number_of_sprites;
}

/**
 * Refresh one sprite position. Might require to call WaitForVBlank() previously.
 * @param x column
 * @param y row
 * @param address base vram address to copy to
 */
void screenRefreshPos32x32(u8 x, u8 y, u16 address)
{
    u16 offset = 0;
    if (x < 16 && y < 16)
        offset =            x     + y*64;
    else if (x < 32 && y < 16)
        offset = 32*32   + (x-16) + y*64;
    else if (x < 16 && y < 32)
        offset = 32*32*2 +  x     + (y-16)*64;
    else if (x < 32 && y < 32)
        offset = 32*32*3 + (x-16) + (y-16)*64;
    else
        consoleNocashMessage("screenRefreshPos32x32: out of bounds: x=%u,y=%u\n", (int) x, (int) y);

    dmaCopyVram((u8 *) (spritemap32x32 + offset), address + offset, 256);
}

/**
 * Update sprite after updating its graphics. Might require to call WaitForVBlank() previously.
 * Intended for a 16x16 sprite on a 32x32 map.
 *
 * @param source the source base to copy from
 * @param address base vram address to copy to
 * @param elem gfx tile no 0 - (number_of_sprites - 1)
 */
void updateSprite32x32(u8 *source, u16 address, u16 elem)
{
    //dmaCopyVram((u8*) source, address, 256 * number_of_sprites); // update all sprites
    dmaCopyVram((u8*) (source + calculateSpriteIndex32x32(elem)), address + element2sprite32x32(elem)*32, 256); // update one sprite
}
