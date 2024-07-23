#include <snes/dma.h>
#include <snes/console.h>

u16 spritemap64x64_len = 0;
u16* spritemap64x64;

void setSpriteMap64x64(u16* spritemap, u16 spritemap_len)
{
    spritemap64x64 = spritemap;
    spritemap64x64_len = spritemap_len;
}

u16* getSpriteMap64x64(u16* spritemap)
{
    return spritemap64x64;
}

u16 getSpriteMap64x64_length()
{
    return spritemap64x64_len;
}

void initSpriteMap64x64()
{
    u16 i;
    for (i = 0; i < spritemap64x64_len; i++)
    {
        spritemap64x64[i] = 0;
    }
}

/**
 * Draw a sprite on a 64 x 64 map. The map consists of four 32 x 32 maps, which are ordered this way:
 * Top left, top right, bottom left, bottom right.
 * The top left 32 x 32 map begins with y = 0.
 * The top right 32 x 32 map begins with y = 32.
 * The bottom left 32 x 32 map begins with y = 64.
 * The bottom right 32 x 32 map begins with y = 96.
 *
 * @param x 0 - 32
 * @param y 0 - 96
 */
void drawSpriteRaw64x64(u8 x, u8 y, u16 sprite)
{
    // i = colsMax * row + col;
    u16 i = 32 * y + x;
    spritemap64x64[i] = sprite;
}

u16 getSpriteRaw64x64(u8 x, u8 y)
{
    // i = colsMax * row + col;
    u16 i = 32 * y + x;
    return spritemap64x64[i];
}

/**
 * Draw a sprite on a 64 x 64 map.
 *
 * @param x column 0 - 63
 * @param y row 0 - 63
 * @param sprite sprite
 */
void drawSprite64x64(u8 x, u8 y, u16 sprite)
{
    if (x < 32 && y < 32)
        drawSpriteRaw64x64(x, y, sprite); // top left
    else if (x < 64 && y < 32)
        drawSpriteRaw64x64(x - 32, y + 32, sprite); // top right
    else if (x < 32 && y < 64)
        drawSpriteRaw64x64(x, y + 32, sprite); // bottom left
    else if (x < 64 && y < 64)
        drawSpriteRaw64x64(x - 32, y + 64, sprite); // bottom right
    else
        consoleNocashMessage("drawSprite64x64: out of bounds: x=%u,y=%u\n", (int) x, (int) y);
}

u16 getSprite64x64(u8 x, u8 y)
{
    if (x < 32 && y < 32)
        return getSpriteRaw64x64(x, y); // top left
    else if (x < 64 && y < 32)
        return getSpriteRaw64x64(x - 32, y + 32); // top right
    else if (x < 32 && y < 64)
        return getSpriteRaw64x64(x, y + 32); // bottom left
    else if (x < 64 && y < 64)
        return getSpriteRaw64x64(x - 32, y + 64); // bottom right
    else
        consoleNocashMessage("getSprite64x64: out of bounds: x=%u,y=%u\n", (int) x, (int) y);
}

/**
 * Convert element to its sprite index.
 *
 * @param elem gfx tile no 0 - (number_of_sprites - 1)
 * @return sprite index
 */
u16 element2sprite64x64(u8 elem)
{
    // for BG1_TSIZE16x16
    u8 mult = elem / 8;
    return (elem * 2) + (mult * 16);

    // alternative: maybe easier to understand
    //return calculateSpriteIndex64x64(elem) * 2 / 128;
}

u16 calculateSpriteIndex64x64(u8 elem)
{
    // BG1_TSIZE16x16: Sprite top and bottom have a distance of 1024 bytes.
    //     To prevent that a sprite top overwrites an already existing sprite bottom,
    //     this correction is required:
    u16 index = elem * 128;
    u8 mult = index / 1024;
    index += 1024 * mult;

    return index;
}

u16 calculateSpritesLength64x64(u16 number_of_sprites)
{
    // sprite bottom is shifted by 1024, sprite bottom length is 128
    return calculateSpriteIndex64x64(number_of_sprites - 1) + 1024 + 128;

    // NOTE: Sometimes identical with (256 * number_of_sprites), but not always!
}

/**
 * Refresh one sprite position. Might require to call WaitForVBlank() previously.
 * @param x column
 * @param y row
 * @param address vram base address to copy to
 */
void screenRefreshPos64x64(u8 x, u8 y, u16 address)
{
    u16 offset = 0;
    if (x < 32 && y < 32)
        offset =            x     + y*32;
    else if (x < 64 && y < 32)
        offset = 32*32   + (x-32) + y*32;
    else if (x < 32 && y < 64)
        offset = 32*32*2 +  x     + (y-32)*32;
    else if (x < 64 && y < 64)
        offset = 32*32*3 + (x-32) + (y-32)*32;
    else
        consoleNocashMessage("screenRefreshPos64x64: out of bounds: x=%u,y=%u\n", (int) x, (int) y);

    dmaCopyVram((u8 *) (spritemap64x64 + offset), address + offset, 256);
}

/**
 * Update sprite after updating its graphics. Might require to call WaitForVBlank() previously.
 * Intended for a 16x16 sprite on a 64x64 map.
 *
 * @param source the source base to copy from
 * @param address base vram address to copy to
 * @param elem gfx tile no 0 - (number_of_sprites - 1)
 */
void updateSprite64x64(u8 *source, u16 address, u16 elem)
{
    //dmaCopyVram((u8*) source, address, 256 * number_of_sprites); // update all sprites
    dmaCopyVram((u8*) (source + calculateSpriteIndex64x64(elem)), address + element2sprite64x64(elem)*32, 128); // update sprite top
    dmaCopyVram((u8*) (source + calculateSpriteIndex64x64(elem) + 1024), address + element2sprite64x64(elem)*32 + 1024/2, 128); // update sprite bottom
}
