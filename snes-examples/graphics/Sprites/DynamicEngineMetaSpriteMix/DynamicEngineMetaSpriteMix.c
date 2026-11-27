/*---------------------------------------------------------------------------------


    Simple Dynamic Mixed size Sprite Engine demo
    -- alekmaul



---------------------------------------------------------------------------------*/
#include <snes.h>

#include "spritehero832.inc"
#include "spritehero832_meta.inc"
#include "spritehero1632.inc"
#include "spritehero1632_meta.inc"
#include "pvsneslibfont.inc"

#define ADRBG1 0x2000

#define HERO_X 96
#define HERO_Y 112
#define HERO_FRAMES 3
#define HERO_DELAY 12 // number of screens an animation frame stays

u16 selectedItem;
u16 heroFrame, heroDelay;
u8 heroRefresh;
bool keyPressed;
unsigned short pad0;

//---------------------------------------------------------------------------------
// Put menu for each sprite size test
void drawText()
{
    consoleDrawText(3, 2, "Object size :");
    consoleDrawText(3, 3, "%s %s", selectedItem == 0 ? ">" : " ", "Small:  8 - Large: 32");
    consoleDrawText(3, 4, "%s %s", selectedItem == 1 ? ">" : " ", "Small: 16 - Large: 32");
}

//---------------------------------------------------------------------------------
// Init the sprite engine with the object size of the current selection
void changeObjSize()
{
    // Force wait VBlank to avoid black frame
    WaitForVBlank();

    // Init sprite engine (0x0000 for large, 0x1000 for small)
    if (selectedItem == 0)
        oamInitDynamicSprite(0x0000, 0x1000, 0, 0, spritehero832_objsize);
    else
        oamInitDynamicSprite(0x0000, 0x1000, 0, 0, spritehero1632_objsize);

    // All the graphics have to be sent again to VRAM
    heroRefresh = 1;
}

//---------------------------------------------------------------------------------
// Draw a metasprite mixing two sprite sizes with the dynamic engine: each of its
// sprites goes to the function of its size, with the graphics of this size.
// refresh = 1 sends the graphics to VRAM (new animation frame).
// Large sprites are 32x32 here. Returns the next free entry of oambuffer.
u16 drawMetaDynMix(u16 id, s16 x, s16 y, const t_metasprite *meta, u8 *gfxlarge, u8 *gfxsmall, u8 refresh)
{
    while (meta->dx != (u16)metasprite_end)
    {
        oambuffer[id].oamx = x + meta->dx;
        oambuffer[id].oamy = y + meta->dy;
        oambuffer[id].oamframeid = meta->dtile;
        oambuffer[id].oamrefresh = refresh;

        if (meta->unused == OBJ_SPRITE32)
        {
            oambuffer[id].oamattribute = meta->props;
            oambuffer[id].oamgraphics = gfxlarge;
            oamDynamic32Draw(id);
        }
        else
        {
            // a small sprite takes its tiles in the second name table
            oambuffer[id].oamattribute = meta->props | 1;
            oambuffer[id].oamgraphics = gfxsmall;
            if (meta->unused == OBJ_SPRITE8)
                oamDynamic8Draw(id);
            else
                oamDynamic16Draw(id);
        }

        id++;
        meta++;
    }

    return id;
}

//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize text console with our font
    consoleSetTextMapPtr(0x6800);
    consoleSetTextGfxPtr(0x3000);
    consoleSetTextOffset(0x0100);
    consoleInitText(0, 16 * 2, &pvsneslibfont_til, &pvsneslibfont_pal);

    // Init background
    bgSetGfxPtr(0, ADRBG1);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    // Init Sprites palette (same palette for both sprites)
    setPalette(&spritehero832_pal, 128 + 0 * 16, 16 * 2);

    // Now Put in 16 color mode and disable other BGs except 1st one
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    selectedItem = 0;
    heroFrame = 0;
    heroDelay = 0;
    keyPressed = false;
    drawText();
    changeObjSize();

    setScreenOn();

    // Wait for nothing :P
    while (1)
    {
        pad0 = padsCurrent(0);

        if (pad0)
        {
            if ((pad0 & KEY_UP) && selectedItem > 0 && !keyPressed)
            {
                selectedItem--;
                drawText();
                changeObjSize();
                keyPressed = true;
            }
            if ((pad0 & KEY_DOWN) && selectedItem < 1 && !keyPressed)
            {
                selectedItem++;
                drawText();
                changeObjSize();
                keyPressed = true;
            }
        }
        else
            keyPressed = false;

        // next animation frame
        heroDelay++;
        if (heroDelay >= HERO_DELAY)
        {
            heroDelay = 0;
            heroFrame++;
            if (heroFrame >= HERO_FRAMES)
                heroFrame = 0;
            heroRefresh = 1;
        }

        // redraw the dynamic sprites regarding current configuration
        if (selectedItem == 0)
        {
            drawMetaDynMix(0, HERO_X, HERO_Y, spritehero832_metasprites[heroFrame],
                           (u8 *)&spritehero832_l_til, (u8 *)&spritehero832_s_til, heroRefresh);
        }
        else
        {
            drawMetaDynMix(0, HERO_X, HERO_Y, spritehero1632_metasprites[heroFrame],
                           (u8 *)&spritehero1632_l_til, (u8 *)&spritehero1632_s_til, heroRefresh);
        }
        heroRefresh = 0;

        // prepare next frame and wait vblank
        oamInitDynamicSpriteEndFrame();
        WaitForVBlank();
        oamVramQueueUpdate();
    }
    return 0;
}
