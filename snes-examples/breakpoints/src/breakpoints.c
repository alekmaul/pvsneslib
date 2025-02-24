/*---------------------------------------------------------------------------------


    Simple example of using special Mesen breakpoints.
    The 65816 CPU has an unused opcode called WDM which functions as a NOP.
    Mesen can be told to break on it.

    -- jeffythedragonslayer


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char tilfont, palfont;

//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize SNES
    consoleMesenBreakpoint();

    // Initialize text console with our font
    consoleMesenBreakpoint();
    consoleSetTextMapPtr(0x6800);
    consoleSetTextGfxPtr(0x3000);
    consoleSetTextOffset(0x0100);
    consoleInitText(0, 16 * 2, &tilfont, &palfont);

    // Init background
    consoleMesenBreakpoint();
    bgSetGfxPtr(0, 0x2000);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    // Now Put in 16 color mode and disable Bgs except current
    consoleMesenBreakpoint();
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Draw a wonderful text :P
    consoleMesenBreakpoint();
    consoleDrawText(10, 10, "Breakpoints!");
    consoleDrawText(6, 14, "Press Ctrl+D to open");
    consoleDrawText(7, 15, "the Mesen debugger.");
    consoleDrawText(5, 18, "Make sure \'Break on...\'");
    consoleDrawText(8, 19, "WDM is checked");

    // Wait for nothing :P
    setScreenOn();

    while (1)
    {
	consoleMesenBreakpoint();
        WaitForVBlank();
    }
    return 0;
}
