/*---------------------------------------------------------------------------------


    Transparent window effect in mode 1 
    -- digifox


---------------------------------------------------------------------------------*/
#include <snes.h>
#include "window.h"

// ROM

#define REG_DMA0 (*(vuint8 *)0x4300)
#define REG_DMA1 (*(vuint8 *)0x4310)
#define REG_DMA2 (*(vuint8 *)0x4320)
#define REG_DMA3 (*(vuint8 *)0x4330)
#define REG_DMA4 (*(vuint8 *)0x4340)
#define REG_DMA5 (*(vuint8 *)0x4350)
#define REG_DMA6 (*(vuint8 *)0x4360)
#define REG_DMA7 (*(vuint8 *)0x4370)

//RAM

u8 hdmaTableLeft[1024] = {
    1, 255,        // Disable window for 1 scanline
    0x80 | 1,      // 1 line of single entries

    0,

    0x01, 0xff, 
    
    0
};

u8 hdmaTableRight[1024] = {
    1, 0,          // Disable window for 1 scanline
    0x80 | 1,		// 1 line of single entries

    0,

    0x00, 0xff, 
    
    0
};

u8 windowIndex;

/*!\brief Create an HDMA transparent Window.
    \param x the window's x position.
    \param y the window's y position.
    \param width the window's width.
    \param height the window's height.
    \param bgNumber the background number where the transparent window is displayed.
    \param color the window color (BGR format, see REG_COLDATA).
    \param colorIntensity the window color intensity (max value is 0b00011111).
    \param colorMath the color math to use (Add/Subtract, Half, Backdrop - see REG_CGADSUB)
    \warning Only works on BG1 and BG2.
    \note This function must be improved to work on BG3 and BG4.
*/
void createTransparentWindow(u8 x, u8 y, u8 width, u8 height, u8 bgNumber, u8 color, u8 colorIntensity, u8 colorMath) {
    hdmaTableLeft[0] = y;
    hdmaTableRight[0] = y;

    hdmaTableLeft[1] = 255;
    hdmaTableRight[1] = 0;

    hdmaTableLeft[2] = 0x80 | height;
    hdmaTableRight[2] = 0x80 | height;

    windowIndex = 3;

    while (windowIndex < height + 3) {
        hdmaTableLeft[windowIndex] = x;
        hdmaTableRight[windowIndex] = x + width;
        windowIndex++;
    }

    hdmaTableLeft[windowIndex] = 0x01;
    hdmaTableLeft[windowIndex+1] = 0xff;
    hdmaTableLeft[windowIndex+2] = 0x00;

    hdmaTableRight[windowIndex] = 0x00;
    hdmaTableRight[windowIndex+1] = 0xff;
    hdmaTableRight[windowIndex+2] = 0x00;

    // Enable Color Math and Activate MathWindow (on REG_CGWSEL)
    // Activate the passed colorMath when Main Screen = bgNumber (on REG_CGADSUB)
    setColorEffect(0b00010000, colorMath | (bgNumber + 1));

    // Set color with an intensity
    REG_COLDATA = color | colorIntensity;

    // Create an HDMA Window using HDMA Channels 4 and 5
    // Activate Window1 on bgNumber (see: REG_W12SEL)
    switch(bgNumber) {
        case 0:
            setModeHdmaWindow(MSWIN_BG1, 
                MSWIN1_BG1MSKENABLE, 
                (u8 *) &hdmaTableLeft, 
                (u8 *) &hdmaTableRight);
            break;

        case 1:
            setModeHdmaWindow(MSWIN_BG2, 
                MSWIN1_BG2MSKENABLE, 
                (u8 *) &hdmaTableLeft, 
                (u8 *) &hdmaTableRight);
            break;
    }

    // Disable main screen windows
    REG_TMW = 0;

    // Activate Color Math on Window1 Area
    REG_WOBJSEL = 0b00100000;

    // Reset HDMA channels 4 and 5
    REG_DMA4 = 0;
    REG_DMA5 = 0;
}
