#include <snes/dma.h>

/**
 * Refresh tile of a spritemap. Might require to call WaitForVBlank() previously.
 *
 * @param u16* spritemap 32x32 or 64x64 sprite map
 * @param tile 0-3
 * @param address vram base address to copy to
 */
void screenRefreshTile(u16* spritemap, u8 tile, u16 address)
{
    switch(tile)
    {
    case 0:
        dmaCopyVram((u8 *) (spritemap),          address,          2048); // top left map
        break;
    case 1:
        dmaCopyVram((u8 *) (spritemap + 1024),   address + 1024,   2048); // top right map
        break;
    case 2:
        dmaCopyVram((u8 *) (spritemap + 1024*2), address + 1024*2, 2048); // bottom left map
        break;
    case 3:
        dmaCopyVram((u8 *) (spritemap + 1024*3), address + 1024*3, 2048); // bottom right map
        break;
    default:
        break;
    }
}

/**
 * Refresh spritemap.
 *
 * @param u16* spritemap 32x32 or 64x64 sprite map
 * @param address vram base address to copy to
 */
void screenRefresh(u16* spritemap, u16 address)
{
    WaitForVBlank();
    screenRefreshTile(spritemap, 0, address);
    screenRefreshTile(spritemap, 1, address);
    WaitForVBlank(); // required, otherwise screen not updated completely
    screenRefreshTile(spritemap, 2, address);
    screenRefreshTile(spritemap, 3, address);
}
