/*---------------------------------------------------------------------------------
    Window circular demo in mode 1
    -- alekmaul
---------------------------------------------------------------------------------*/
#include <snes.h>

#include <string.h>
#include <stdlib.h>

#include "pvsneslibbg1.inc"

#define INCDIR               4                  // The more it is ,t he faster it is
#define MAXRADIUS          160                  // 160 is enough to cover the whole screen 256x224

extern u8 circle_buffer[113];
extern u8 hdma_table_LR[224 * 3 + 1]; 
u8 hdma_table_LRB[224 * 3 + 1]; 


extern void update_iris_bresenham(u16 r);

//---------------------------------------------------------------------------------
u8 radius;
s8 dir;

//---------------------------------------------------------------------------------
void WaitForKey() { 
    while (padsCurrent(0) == 0) {
        WaitForVBlank();
    }
    while (padsCurrent(0) != 0) {
        WaitForVBlank();
    }
}

//---------------------------------------------------------------------------------
// c source code here but too slow, did the same in assmbly labguage
#if 0
u8 *ptrLR;                                       // for faster access
u16 i;                                           // loop variable
#define cy 112
#define cx 128

void update_iris_bresenham(u16 r) {
    //int x = 0;
    //int y = r;
    //int d = 3 - 2 * r;

    // 1. Reset the buffer
    memset(circle_buffer,0x00,113);

    // 2. Standard Bresenham
    /*
    if (r > 0) {
        while (y >= x) {
            if (y <= 112) circle_buffer[y] = x;
            if (x <= 112) circle_buffer[x] = y;
            if (d < 0) {
                d = d + (4 * x) + 6;
            } else {
                d = d + 4 * (x - y) + 10;
                y--;
            }
            x++;
        }
    }
    */

    // 3. Transfer to HDMA
    ptrLR = hdma_table_LR;

    for (i = 0; i < 224; i++) {
        s16 dist_y = abs(i - cy); // ((i-cy) ^0xFFFF)+1;//
        u16 half_width = 0;

        if (dist_y < r && dist_y <= 112) {
            half_width = circle_buffer[dist_y];
        }

        s16 left = cx - half_width;
        s16 right = cx + half_width;

        // FIX: If half_width is 0, the circle doesn't exist on this line.
        // We set Left to 255 and Right to 0. 
        // Since Left > Right, the SNES Window logic will result in 0 pixels being masked.
        if (half_width == 0) {
            left = 255; 
            right = 0;
        } else {
            // Standard bounds clipping
            if (left < 0) left = 0;
            if (right > 255) right = 255;
            
            // Extra safety: if they ended up equal, nudge them to avoid the 1-pixel glitch
            if (left == right) {
                left = 255;
                right = 0;
            }
        }

        *ptrLR++ = 1; *ptrLR++ = (u8)left; *ptrLR++ = (u8)right;
    }
    *ptrLR = 0;
}
#endif

//---------------------------------------------------------------------------------
int main(void) {

    // Copy tiles & Map to VRAM
    bgInitTileSet(0, &pvsneslibbg1_til, &pvsneslibbg1_pal, 0, (&pvsneslibbg1_tilend - &pvsneslibbg1_til), (&pvsneslibbg1_palend - &pvsneslibbg1_pal), BG_16COLORS, 0x4000);
    bgInitMapSet(0, &pvsneslibbg1_map, (&pvsneslibbg1_mapend - &pvsneslibbg1_map)*2, SC_32x32, 0x0000);

    // Now Put in 16 color mode and disable other BGs except 1st one
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

    // Prepare circle effect witht the two tables
    setModeHdmaWindowEx(MSWIN_BG1, MSWIN1_BG1MSKENABLE | MSWIN1_BG1MSKOUT, hdma_table_LRB);
    radius = 0;
    dir = INCDIR; 

    // global loop of iris effect
    while (1) {
        // Calculate and Update HDMA tables
        update_iris_bresenham(radius);
        memcpy(hdma_table_LRB,hdma_table_LR,224*3+1); // to avoid glitch on screen when calculating new values

        radius += dir;
        if (radius >= MAXRADIUS) {
            radius = MAXRADIUS;
            dir = -INCDIR;
            WaitForKey();
        }
        if (radius <= 0) {
            radius = 0;
            dir = INCDIR;
            WaitForKey();
        }
        WaitForVBlank();
    }

    return 0;
}
