/*---------------------------------------------------------------------------------
    Simple mode 7 rotating demo with rotation and scaling
    -- alekmaul
---------------------------------------------------------------------------------*/
#include <snes.h>

extern char patterns, patterns_end;
extern char palette;
extern char map, map_end;

u16 pad0;
u8 angle;
u16 zscale;
extern u16 m7sx, m7sy;

//---------------------------------------------------------------------------------
int main(void)
{
    // Read tiles & map to VRAM  (interlace for mode 7)
    bgInitMapTileSet7(&patterns, &map, &palette, (&patterns_end - &patterns), 0x0000);

    // Now Put mode7 without anything else
    setMode7(0);

    // Display screen
    setScreenOn();

    // Init angle
    angle = 0; zscale=0x200; // default value stored in PVSnesLib for m7sx & m7sy
    setMode7Rot(angle);

    // Wait for nothing :P
    while (1)
    {
        // Get current #0 pad
        pad0 = padsCurrent(0);
        if (pad0)
        {
            switch (pad0)
            {
            // Update rotation with A & B
            case KEY_A:
                angle++;
                setMode7Rot(angle);
                break;
            case KEY_B:
                angle--;
                setMode7Rot(angle);
                break;
            case KEY_DOWN:
                if (zscale>16) zscale-=16; 
                m7sx=zscale; setMode7Rot(angle);// as we also modify angle, we use this trick instead of calling setMode7Scale(zscale,zscale);
                break;
            case KEY_UP:
                if (zscale<0xF00) zscale+=16; 
                m7sx=zscale; setMode7Rot(angle); // as we also modify angle, we use this trick instead of calling  setMode7Scale(zscale,zscale);
                break;
            }
        }
        WaitForVBlank();
    }
    return 0;
}