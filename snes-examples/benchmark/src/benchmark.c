/*---------------------------------------------------------------------------------
	PVSnesLib benchmark test
	-- alekmaul
---------------------------------------------------------------------------------*/

#include <snes.h>

#include "../pvsneslibfont.inc"

u8 line;
extern u32 micro_result;
//---------------------------------------------------------------------------------
void tstPostResult(char* txttest) {
    consoleDrawText(0, line, txttest);
    consoleDrawText(16+4, line, "%04X",(u16) (micro_result & 0xFFFF));
    consoleDrawText(16, line, "%04X",(u16) (micro_result / 0xFFFF));
    line++;
    if (line>28) line=1;
}

//---------------------------------------------------------------------------------
int main(void) {
    // Init text engine
    consoleSetTextMapPtr(0x6800);
    consoleSetTextGfxPtr(0x3000);
    consoleSetTextOffset(0x0100);
    consoleInitText(0, 16 * 2, &pvsneslibfont_til, &pvsneslibfont_pal);
    bgSetGfxPtr(0, 0x2000);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    consoleDrawText(7, 0, "BENCHMARK TEST V1.0");

    // Prepare for mode 0
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();
    
    // init global var for benchs
    line=2;

    while (1) {
        runMathTest();

        // stop here
        while(1);
    }
    return 0;
}
