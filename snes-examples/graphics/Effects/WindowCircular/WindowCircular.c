/*---------------------------------------------------------------------------------
    Window circular demo in mode 1
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
u8 i,hw,dy;                                  // for calculation
u16 r2, dy2;                                // for calculation
u8 x1,x2;                                   // for calculation

// NOTE: Does not pause execution if a pad 0 key is currently pressed.
void WaitForKey() {
    while (padsCurrent(0) == 0) {
        WaitForVBlank();
    }
}

//---------------------------------------------------------------------------------
void buildCircleHdma(u8 cx, u8 cy, u8 radius) {
    r2 = radius * radius;   // max 16384 — fits u16
    hw = radius;
    
    // Set HDMA header and terminator
    hdmaCircleL[0] = hdmaCircleR[0] = 0xE0;
    hdmaCircleL[SCREEN_H + 1] = hdmaCircleR[SCREEN_H + 1] = 0x00;

    // Pre-fill all scanlines as "outside circle" (closed window)
    for (i = 1; i <= SCREEN_H; i++) {
        hdmaCircleL[i] = 0xFF;
        hdmaCircleR[i] = 0x00;
    }

    // Walk outward from center, fill both halves symmetrically
    for (dy = 0; (dy <= cy) && (dy < SCREEN_H); dy++) {
        // Decrease hw while (hw,dy) is outside circle
        while (hw > 0 && (u16)(hw * hw) + (u16)(dy * dy) > r2)
            hw--;

        // Once hw reaches 0 and we are outside, all further scanlines stay closed
        if (hw == 0 && (u16)(dy * dy) > r2)
            break;
        
        x1 = (hw >= cx)           ? 0   : (cx - hw);
        x2 = ((u16) (cx + hw) > 255) ? 255 : (cx + hw);

        // Top half (index = scanline + 1 because byte 0 is the header)
        hdmaCircleL[cy - dy + 1] = x1;
        hdmaCircleR[cy - dy + 1] = x2;

        // Bottom half (symmetric, skip if same scanline as top)
        if ((dy > 0) && ((u16) (cy + dy) < SCREEN_H) ) {
            hdmaCircleL[cy + dy + 1] = x1;
            hdmaCircleR[cy + dy + 1] = x2;
        }
    }
}

void irisIn(s16 cx, s16 cy) {
    s16 r;

    buildCircleHdma(cx, cy, MAX_RADIUS);
    setModeHdmaWindow(MSWIN_BG1 | MSWIN_BG2, MSWIN1_BG1MSKENABLE | MSWIN1_BG2MSKENABLE, hdmaCircleL, hdmaCircleR);
    for (rloop = MAX_RADIUS; r >= 0; rloop -= 3) {
        WaitForVBlank();
        buildCircleHdma(cx, cy, (u8) rloop);
        WaitForKey();
    }
}

void irisOut(s16 cx, s16 cy) {
    buildCircleHdma(cx, cy, 0);
    setModeHdmaWindow(MSWIN_BG1 | MSWIN_BG2, MSWIN1_BG1MSKENABLE | MSWIN1_BG2MSKENABLE, hdmaCircleL, hdmaCircleR);

    for (rloop = 0; rloop <= 128; rloop += 3) {
        WaitForVBlank();
        buildCircleHdma(cx, cy, (u8) rloop);
        WaitForKey();
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

    WaitForKey();

    while (1) {
        irisOut(128, 112);   // open from screen center
        WaitForVBlank();
        WaitForKey();

        irisIn(128, 112);    // close to screen center
        WaitForVBlank();
        WaitForKey();
    }

    return 0;
}
