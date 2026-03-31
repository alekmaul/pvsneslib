/*---------------------------------------------------------------------------------
    Window circular demo
    -- alekmaul
---------------------------------------------------------------------------------*/
#include <snes.h>
#include <math.h>

#include "pvsneslibbg1.inc"

#define SCREEN_H   224
#define MAX_RADIUS 128

u8 hdmaCircleL[226];
u8 hdmaCircleR[226];

s16 rloop;                                   // loop for iris in & out
u8 y;                                       // circle y line computation
u8 i;                                       // loop variable
u8 hw,dy;                                   // for calculation
u16 r2, dy2;                                // for calculation

//---------------------------------------------------------------------------------
void buildCircleHdma(u8 cx, u8 cy, u8 radius) {
    r2 = (u16)radius * radius;   // max 16384 — fits u16
    hw = radius;
    
    i  = 0;
    hdmaCircleL[i] = hdmaCircleR[i] = 0xE0;   // repeat header: 224 lines
    i++;

    for (y = 0; y < SCREEN_H; y++, i++) {
        dy  = (y < cy) ? (cy - y) : (y - cy);
        dy2 = (u16)dy * dy;   // max 12544 — fits u16

        if (dy2 >= r2) {
            hdmaCircleL[i] = 0xFF;
            hdmaCircleR[i] = 0x00;
            hw = 0;
        }
        else {
            // hw*hw + dy2 max = 16384 + 12544 = 28928 — fits u16
            while (hw > 0 && (u16)(hw * hw) + dy2 > r2)
                hw--;

            hdmaCircleL[i] = (hw > cx)       ? 0   : (cx - hw);
            hdmaCircleR[i] = (cx + hw > 255) ? 255 : (cx + hw);
        }

        if (y == cy)
            hw = radius;   // reset for bottom half
    }
    hdmaCircleL[i] = hdmaCircleR[i] = 0x00;   // terminator
}

void irisIn(s16 cx, s16 cy) {
    s16 r;

    buildCircleHdma(cx, cy, MAX_RADIUS);
    setModeHdmaWindow(MSWIN_BG1 | MSWIN_BG2,
                      MSWIN1_BG1MSKENABLE | MSWIN1_BG2MSKENABLE,
                      hdmaCircleL, hdmaCircleR);
    for (rloop = MAX_RADIUS; r >= 0; rloop -= 3) {
        WaitForVBlank();
        buildCircleHdma(cx, cy, (u8) rloop);
    }
}

void irisOut(s16 cx, s16 cy) {
    buildCircleHdma(cx, cy, 0);
    setModeHdmaWindow(MSWIN_BG1 | MSWIN_BG2,
                      MSWIN1_BG1MSKENABLE | MSWIN1_BG2MSKENABLE,
                      hdmaCircleL, hdmaCircleR);

    for (rloop = 0; rloop <= 128; rloop += 3) {
        WaitForVBlank();
        buildCircleHdma(cx, cy, (u8) rloop);
    }

    setModeHdmaWindowReset(HDMA_CHANNEL4 | HDMA_CHANNEL5);
}

//---------------------------------------------------------------------------------
int main(void) {
    // Copy tiles to VRAM
    bgInitTileSet(0, &pvsneslibbg1_til, &pvsneslibbg1_pal, 0, (&pvsneslibbg1_tilend - &pvsneslibbg1_til), (&pvsneslibbg1_palend - &pvsneslibbg1_pal), BG_16COLORS, 0x4000);

    // Copy Map to VRAM
    bgInitMapSet(0, &pvsneslibbg1_map, (&pvsneslibbg1_mapend - &pvsneslibbg1_map)*2, SC_32x32, 0x0000);

    // Now Put in 16 color mode and disable other BGs except 1st and 2nd one
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

    irisIn(128, 112);    // close to screen center

    // ... swap level data here ...

    irisOut(128, 112);   // open from screen center

    while (1)
        WaitForVBlank();

    return 0;
}
