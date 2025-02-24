/*
 * Mouse data test.
 *
 * Used to verify the mouse reading code with a Hyperkin mouse.
 */

#include <snes.h>

#define JOYPAD_PORT 0
#define MOUSE_PORT 1

#define MAX_XPOS 255
#define MAX_YPOS 224


#define COL_1_XPOS    18
#define COL_2_XPOS    26

#define LABEL_YPOS     8

#define BUTTON_YPOS    8
#define SPEED_YPOS    10
#define DELTA_YPOS    12
#define POS_YPOS      14
#define LAG_YPOS      18

#define CONTOLS_YPOS  22


#define N_LABELS 6
const char* const LABELS[N_LABELS] = {
    "Button:",
    "Sensitivity:",
    "Displacement:",
    "Position:",
    "",
    "Lag counter:"
};

const char* const BUTTON_STRINGS[4] = {
    "     ",
    "LEFT ",
    "RIGHT",
    "BOTH ",
};

const u8 CURSOR_TILE[32] = {
   0b11000000, 0b00000000,
   0b11100000, 0b01000000,
   0b11110000, 0b01100000,
   0b11111000, 0b01110000,
   0b11111100, 0b01111000,
   0b11111000, 0b01100000,
   0b11100000, 0b00000000,
   0b00000000, 0b00000000,
};

const u16 CURSOR_PALETTE[4] = {
    0, 0, 0x17e5, 0x17e5
};

#define VRAM_BG3_MAP_WADDR  0x0000
#define VRAM_BG3_TILE_WADDR 0x1000
#define VRAM_OBJ_TILE_WADDR 0x6000

void printU8(u16 xPos, u16 yPos, u16 value);
void drawMouseDelta(u16 xPos, u16 value);

extern char Font_Tiles, Font_Palette;

u16 cursor_xPos = 128;
u16 cursor_yPos = 112;

int main(void)
{
    u16 i;

    consoleInit();

    setMode(BG_MODE1, 0);
    bgSetDisable(0);
    bgSetDisable(1);

    bgSetGfxPtr(2, VRAM_BG3_TILE_WADDR);
    bgSetMapPtr(2, VRAM_BG3_MAP_WADDR, SC_32x32);

    oamInitGfxSet((void*)&CURSOR_TILE, sizeof(CURSOR_TILE), (void*)&CURSOR_PALETTE, sizeof(CURSOR_PALETTE), 0, VRAM_OBJ_TILE_WADDR, OBJ_SIZE8_L16);

    consoleSetTextMapPtr(VRAM_BG3_MAP_WADDR);
    consoleSetTextGfxPtr(VRAM_BG3_TILE_WADDR);
    consoleSetTextOffset(0x0000);
    consoleInitText(0, 4 * 2, &Font_Tiles, &Font_Palette);

    consoleDrawText(2, 2, "MOUSE DATA TEST:");


    consoleDrawText(2, CONTOLS_YPOS, "Controls (Joypad %d):", JOYPAD_PORT + 1);
    consoleDrawText(5, CONTOLS_YPOS + 1, "Y/X/A - Set mouse speed");
    consoleDrawText(5, CONTOLS_YPOS + 2, " L/R  - Cycle mouse speed");
    consoleDrawText(5, CONTOLS_YPOS + 3, "  B   - Add lag");


    for (i = 0; i < N_LABELS; i++) {
        consoleDrawText(2, LABEL_YPOS + i * 2, "%s", LABELS[i]);
    }

    // Setup the cursor sprite (hard coded)
    oamMemory[0] = cursor_xPos;
    oamMemory[1] = cursor_yPos;
    oamMemory[2] = 0;
    oamMemory[3] = 0x30;
    oamMemory[512] = 0; // Clear hi table

    // Enable mouse reading and set the initial mouse sensitivity to medium
    initMouse(1);

    consoleDrawText(2, 4, "READING MOUSE ON PORT %d", MOUSE_PORT + 1);

    setScreenOn();


    while (true) {
        WaitForVBlank();

        if (mouseConnect[MOUSE_PORT] == false) {
            while (mouseConnect[MOUSE_PORT] == false) {
                consoleDrawText(2, 4, "NO MOUSE ON PORT %d     ", MOUSE_PORT + 1);

                WaitForVBlank();
            }

            consoleDrawText(2, 4, "READING MOUSE ON PORT %d", MOUSE_PORT + 1);
            WaitForVBlank();
        }

        // Update cursor position
        if (mouse_x[MOUSE_PORT] & 0x80) {
            cursor_xPos -= mouse_x[MOUSE_PORT] & 0x7e;
            if (cursor_xPos >= 0x8000) {
                cursor_xPos = 0;
            }
        }
        else {
            cursor_xPos += mouse_x[MOUSE_PORT];
            if (cursor_xPos >= MAX_XPOS + 1) {
                cursor_xPos = MAX_XPOS;
            }
        }

        if (mouse_y[MOUSE_PORT] & 0x80) {
            cursor_yPos -= mouse_y[MOUSE_PORT] & 0x7e;
            if (cursor_yPos >= 0x8000) {
                cursor_yPos = 0;
            }
        }
        else {
            cursor_yPos += mouse_y[MOUSE_PORT];
            if (cursor_yPos >= MAX_YPOS + 1) {
                cursor_yPos = MAX_YPOS;
            }
        }

        if (padsDown(JOYPAD_PORT) & (KEY_Y | KEY_X | KEY_A | KEY_L | KEY_R)) {
            if (padsDown(JOYPAD_PORT) & KEY_Y) {
                mouseSetSensitivity(MOUSE_PORT, MOUSE_SLOW);
            }
            if (padsDown(JOYPAD_PORT) & KEY_X) {
                mouseSetSensitivity(MOUSE_PORT, MOUSE_MEDIUM);
            }
            if (padsDown(JOYPAD_PORT) & KEY_A) {
                mouseSetSensitivity(MOUSE_PORT, MOUSE_FAST);
            }
            if (padsDown(JOYPAD_PORT) & KEY_R) {
                mouseCycleSensitivity(MOUSE_PORT);
            }
            if (padsDown(JOYPAD_PORT) & KEY_L) {
                mouseCycleSensitivityTwice(MOUSE_PORT);
            }
        }

        if (padsCurrent(JOYPAD_PORT) & KEY_B) {
            // Add lag
            for(i = 0; i < 1500; i++) {}
        }

        // Update mouse cursor sprite
        oamMemory[0] = cursor_xPos;
        oamMemory[1] = cursor_yPos;

        consoleDrawText(COL_1_XPOS, BUTTON_YPOS, BUTTON_STRINGS[mousePressed[MOUSE_PORT] & 3]);
        consoleDrawText(COL_2_XPOS, BUTTON_YPOS, BUTTON_STRINGS[mouseButton[MOUSE_PORT] & 3]);

        printU8(COL_1_XPOS, SPEED_YPOS, mouseSensitivity[MOUSE_PORT]);

        drawMouseDelta(COL_1_XPOS, mouse_x[MOUSE_PORT]);
        drawMouseDelta(COL_2_XPOS, mouse_y[MOUSE_PORT]);

        printU8(COL_1_XPOS, POS_YPOS, cursor_xPos);
        printU8(COL_2_XPOS, POS_YPOS, cursor_yPos);

        consoleDrawText(COL_1_XPOS, LAG_YPOS, "%d", lag_frame_counter);
    }
    return 0;
}

// Hack to quickly print a u8 without lag
const char* const U8_STR_TABLE[256] = {
    "  0", "  1", "  2", "  3", "  4", "  5", "  6", "  7", "  8", "  9", " 10",
    " 11", " 12", " 13", " 14", " 15", " 16", " 17", " 18", " 19", " 20", " 21",
    " 22", " 23", " 24", " 25", " 26", " 27", " 28", " 29", " 30", " 31", " 32",
    " 33", " 34", " 35", " 36", " 37", " 38", " 39", " 40", " 41", " 42", " 43",
    " 44", " 45", " 46", " 47", " 48", " 49", " 50", " 51", " 52", " 53", " 54",
    " 55", " 56", " 57", " 58", " 59", " 60", " 61", " 62", " 63", " 64", " 65",
    " 66", " 67", " 68", " 69", " 70", " 71", " 72", " 73", " 74", " 75", " 76",
    " 77", " 78", " 79", " 80", " 81", " 82", " 83", " 84", " 85", " 86", " 87",
    " 88", " 89", " 90", " 91", " 92", " 93", " 94", " 95", " 96", " 97", " 98",
    " 99", "100", "101", "102", "103", "104", "105", "106", "107", "108", "109",
    "110", "111", "112", "113", "114", "115", "116", "117", "118", "119", "120",
    "121", "122", "123", "124", "125", "126", "127", "128", "129", "130", "131",
    "132", "133", "134", "135", "136", "137", "138", "139", "140", "141", "142",
    "143", "144", "145", "146", "147", "148", "149", "150", "151", "152", "153",
    "154", "155", "156", "157", "158", "159", "160", "161", "162", "163", "164",
    "165", "166", "167", "168", "169", "170", "171", "172", "173", "174", "175",
    "176", "177", "178", "179", "180", "181", "182", "183", "184", "185", "186",
    "187", "188", "189", "190", "191", "192", "193", "194", "195", "196", "197",
    "198", "199", "200", "201", "202", "203", "204", "205", "206", "207", "208",
    "209", "210", "211", "212", "213", "214", "215", "216", "217", "218", "219",
    "220", "221", "222", "223", "224", "225", "226", "227", "228", "229", "230",
    "231", "232", "233", "234", "235", "236", "237", "238", "239", "240", "241",
    "242", "243", "244", "245", "246", "247", "248", "249", "250", "251", "252",
    "253", "254", "255"
};


void drawMouseDelta(u16 xPos, u16 value) {
    if (value & 0x80) {
        consoleDrawText(xPos - 1, DELTA_YPOS, "-");
    }
    else {
        consoleDrawText(xPos - 1, DELTA_YPOS, "+");
    }

    consoleDrawText(xPos, DELTA_YPOS, U8_STR_TABLE[value & 0x7f]);
}

void printU8(u16 xPos, u16 yPos, u16 value) {
    consoleDrawText(xPos, yPos, U8_STR_TABLE[value & 0xff]);
}

