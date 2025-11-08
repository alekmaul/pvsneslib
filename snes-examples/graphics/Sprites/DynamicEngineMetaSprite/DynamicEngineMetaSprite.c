/*---------------------------------------------------------------------------------


    Simple Dynamic Sprite Engine demo
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

#include "spritehero32_meta.inc"
#include "spritehero32.inc"
#include "spritehero16_meta.inc"
#include "spritehero16.inc"
#include "spritehero8_meta.inc"
#include "spritehero8.inc"
#include "pvsneslibfont.inc"

u16 selectedItem;
bool keyPressed;
unsigned short pad0;

#define ADRBG1 0x2000

//---------------------------------------------------------------------------------
// Put menu for each sprite size test
void drawText()
{
    consoleDrawText(3, 2, "Object size :");
    consoleDrawText(3, 3, "%s %s", selectedItem == 0 ? ">" : " ", "Small:  8 - Large: 16");
    consoleDrawText(3, 4, "%s %s", selectedItem == 1 ? ">" : " ", "Small:  8 - Large: 32");
    consoleDrawText(3, 5, "%s %s", selectedItem == 2 ? ">" : " ", "Small: 16 - Large: 32");
}

//---------------------------------------------------------------------------------
// Load current sprites regarding size for small & large one
// 1st one will be at index 1 and second one at index 10 (as metasprites burn some intermediate indexes too)
void changeObjSize()
{

    // Force wait VBlank to avoid black frame
    WaitForVBlank();

    // check regarding the current selection
    if (selectedItem == 0) {
        // Init sprite engine (0x0000 for large, 0x1000 for small)
        oamInitDynamicSprite(0x0000, 0x1000, 0, 0, OBJ_SIZE8_L16);
        
        // Put default refresh flag for 1st sprite (will refresh all metasprites)
        //  other flags are set by metasprite function
        oambuffer[1].oamrefresh=1; 
        oambuffer[10].oamrefresh=1;
    }
    else if (selectedItem == 1) {
        // Init sprite engine (0x0000 for large, 0x1000 for small)
        oamInitDynamicSprite(0x0000, 0x1000, 0, 0, OBJ_SIZE8_L32);
        // Put default refresh flag for 1st sprite (will refresh all metasprites)
        //  other flags are set by metasprite function
        oambuffer[1].oamrefresh=1; 
        oambuffer[10].oamrefresh=1;
    }
    else if (selectedItem == 2) {
        // Init sprite engine (0x0000 for large, 0x1000 for small)
        oamInitDynamicSprite(0x0000, 0x1000, 0, 0, OBJ_SIZE16_L32);
        // Put default refresh flag for 1st sprite (will refresh all metasprites)
        //  other flags are set by metasprite function
        oambuffer[1].oamrefresh=1; 
        oambuffer[10].oamrefresh=1;
    }
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
    bgSetMapPtr(0, 0x6803, SC_32x32);

    // Now Put in 16 color mode and disable all backgrounds
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Init Sprites pal (same palette for the 3 sprites, but we also can manage differente palette)
    setPalette(&spritehero32_pal, 128 + 0 * 16, 16 * 2);
    
    dmaCopyVram((u8 *) &spritehero8_til,0x4000, (&spritehero8_tilend - &spritehero8_til));

    // Now Put in 16 color mode and disable other BGs except 1st one
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    selectedItem = 0;
    keyPressed = false;
    drawText();
    changeObjSize();

    setScreenOn();

	
    // Wait for nothing :P
    while (1)
    {
        pad0 = padsCurrent(0);

        if (pad0) {
            if (pad0 & KEY_UP) {
                if (selectedItem > 0 && !keyPressed) {
                    selectedItem--;
                    drawText();
                    changeObjSize();
                    keyPressed = true;
                }
            }
            if (pad0 & KEY_DOWN) {
                if (selectedItem < 3 && !keyPressed) {
                    selectedItem++;
                    drawText();
                    changeObjSize();
                    keyPressed = true;
                }
            }
        }
        else
            keyPressed = false;

        // redraw dynamic sprites regarding current onfiguraiton
        if (selectedItem == 0) {
            // draw the sprites (8 & 16), no need of size at 8x8 always small 
            oamMetaDrawDyn16(1, 64,140, (u8 *) spritehero16_metasprites[0],(u8 *) &spritehero16_til, OBJ_LARGE);
            oamMetaDrawDyn8(10, 128,140, (u8 *) spritehero8_metasprites[0],(u8 *) &spritehero8_til);
        }
        else if (selectedItem == 1) {
            // draw the sprites (8 & 32), no need of size at 32x32 is always large and 8x8 always small 
            oamMetaDrawDyn32(1, 64,140, (u8 *) spritehero32_metasprites[0],(u8 *) &spritehero32_til);
            oamMetaDrawDyn8(10, 192,140, (u8 *) spritehero8_metasprites[0],(u8 *) &spritehero8_til);
        }
        else if (selectedItem == 2) {
            // draw the sprites (16 & 32), no need of size at 32x32 large
            oamMetaDrawDyn32(1, 64,140, (u8 *) spritehero32_metasprites[0],(u8 *) &spritehero32_til);
            oamMetaDrawDyn16(10, 192,140, (u8 *) spritehero16_metasprites[0],(u8 *) &spritehero16_til, OBJ_SMALL);
        }

        // prepare next frame and wait vblank
        oamInitDynamicSpriteEndFrame();
        WaitForVBlank();
        oamVramQueueUpdate();
    }
    return 0;
}