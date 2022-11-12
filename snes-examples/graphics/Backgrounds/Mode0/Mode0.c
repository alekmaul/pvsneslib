/*---------------------------------------------------------------------------------


    Simple tile mode 1 demo
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char m0, m0_end, p0, p0_end, t0, t0_end;
extern char m1, m1_end, p1, p1_end, t1, t1_end;
extern char m2, m2_end, p2, p2_end, t2, t2_end;
extern char m3, m3_end, p3, p3_end, t3, t3_end;

unsigned char sxbg1 = 0, sxbg2 = 0, sxbg3 = 0, flip = 0;

//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize SNES
    consoleInit();

    // Copy tiles to VRAM, begin with adr=0x2000 for first BG
    // ALso, always use specific mode0 palette entry (ppp)
    // BG1 w/bits ppp=0 is entries 0-3
    // BG1 w/bits ppp=1 is entries 4-7
    // BG1 w/bits ppp=2 is entries 8-11
    // BG1 w/bits ppp=3 is entries 12-15
    // BG1 w/bits ppp=4 is entries 16-19
    // BG1 w/bits ppp=5 is entries 20-23
    // BG1 w/bits ppp=6 is entries 24-27
    // BG1 w/bits ppp=7 is entries 28-31
    // and then for BG2 ppp resets to 0 and resumes at 32...
    // BG2 w/bits ppp=0 is entries 32-35
    // BG2 w/bits ppp=1 is entries 36-39
    // BG2 w/bits ppp=2 is entries 40-43
    // etc etc
    bgInitTileSet(0, &t0, &p0, 0, (&t0_end - &t0), 8, BG_4COLORS0, 0x2000);
    bgInitTileSet(1, &t1, &p1, 0, (&t1_end - &t1), 8, BG_4COLORS0, 0x3000);
    bgInitTileSet(2, &t2, &p2, 0, (&t2_end - &t2), 8, BG_4COLORS0, 0x4000);
    bgInitTileSet(3, &t3, &p3, 0, (&t3_end - &t3), 16, BG_4COLORS0, 0x5000);

    // Copy Map to VRAM
    bgInitMapSet(0, &m0, (&m0_end - &m0), SC_32x32, 0x0000);
    bgInitMapSet(1, &m1, (&m1_end - &m1), SC_32x32, 0x0400);
    bgInitMapSet(2, &m2, (&m2_end - &m2), SC_32x32, 0x0800);
    bgInitMapSet(3, &m3, (&m3_end - &m3), SC_32x32, 0x0C00);

    // Now Put in 16 color mode and disable other BGs except 1st one
    setMode(BG_MODE0, 0);
    setScreenOn();

    // Wait for nothing :P
    while (1)
    {
        // Shcnage scrolling only each 3 frames
        flip++;
        if (flip == 3)
        {
            flip = 0;
            sxbg3++;
            sxbg2 += 2;
            sxbg1 += 3;
            bgSetScroll(1, sxbg1, 0);
            bgSetScroll(2, sxbg2, 0);
            bgSetScroll(3, sxbg3, 0);
        }
        WaitForVBlank();
    }
    return 0;
}