/*---------------------------------------------------------------------------------


     snes sramoffset demo
    -- nub1604


---------------------------------------------------------------------------------*/
#include <snes.h>

#define SLOT1 0
#define SLOT2 1

extern char snesfont, snespal;
char sz[16];

unsigned short pad0;

typedef struct
{
    s16 posX, posY;
    u16 camX, camY;
} SaveState;
//---------------------------------------------------------------------------------
SaveState vts, vtl;
//---------------------------------------------------------------------------------
void SaveSlot1()
{
    vts.camX = 0x1234;
    vts.camY = 0x5678;
    vts.posX = 0x0009;
    vts.posY = 0x000A;
    consoleCopySramWithOffset((u8 *)&vts, sizeof(SaveState), sizeof(SaveState) * SLOT1);
}
void SaveSlot2()
{
    vts.camX = 0xA987;
    vts.camY = 0x6543;
    vts.posX = 0x0002;
    vts.posY = 0x0001;
    consoleCopySramWithOffset((u8 *)&vts, sizeof(SaveState), sizeof(SaveState) * SLOT2);
}

void PrintLoadedSlot()
{

    sprintf(sz, "%04X camX ", vtl.camX);
    consoleDrawText(2, 14, sz);
    sprintf(sz, "%04X camY ", vtl.camY);
    consoleDrawText(7, 14, sz);
    sprintf(sz, "%04X posX ", vtl.posX);
    consoleDrawText(2, 15, sz);
    sprintf(sz, "%04X posY ", vtl.posY);
    consoleDrawText(7, 15, sz);
}
void LoadSlot1()
{
    consoleLoadSramWithOffset((u8 *)&vtl, sizeof(SaveState), sizeof(SaveState) * SLOT1);
    consoleDrawText(2, 13, "LOAD SLOT1");
    PrintLoadedSlot();
}
void LoadSlot2()
{
    consoleLoadSramWithOffset((u8 *)&vtl, sizeof(SaveState), sizeof(SaveState) * SLOT2);
    consoleDrawText(2, 13, "LOAD SLOT2");
    PrintLoadedSlot();
}

//---------------------------------------------------------------------------------

int main(void)
{
    // Initialize text console with our font
    consoleSetTextMapPtr(0x6800);
    consoleSetTextGfxPtr(0x3000);
    consoleSetTextOffset(0x0100);
    consoleInitText(0, 16 * 2, &snesfont, &snespal);

    // Draw a wonderful text :P
    consoleDrawText(12, 1, "SRAM TEST");
    consoleDrawText(3, 5, "USE A TO WRITE Slot1");
    consoleDrawText(3, 7, "USE B TO READ Slot1");
    consoleDrawText(3, 9, "USE X TO WRITE Slot2");
    consoleDrawText(3, 11, "USE Y TO READ Slot2");
    // Init background
    bgSetGfxPtr(0, 0x2000);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    // Now Put in 16 color mode
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Wait for nothing :P
    setScreenOn();

    while (1)
    {
        // Get current #0 pad
        pad0 = padsCurrent(0);

        // Update display with current pad
        switch (pad0)
        {
        case KEY_A:
            SaveSlot1();
            consoleDrawText(2, 13, "SAVE SLOT1");
            consoleDrawText(2, 14, "               ");
            consoleDrawText(2, 15, "               ");
            break;

        case KEY_B:
            LoadSlot1();
            break;

        case KEY_X:
            SaveSlot2();
            consoleDrawText(2, 13, "SAVE SLOT2");
            consoleDrawText(2, 14, "               ");
            consoleDrawText(2, 15, "               ");
            break;

        case KEY_Y:
            LoadSlot2();
            break;
        }
        WaitForVBlank();
    }
    return 0;
}