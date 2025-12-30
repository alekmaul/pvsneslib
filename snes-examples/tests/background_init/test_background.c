/*
 * Background Initialization Test
 *
 * Tests that bgSetEnable and bgInitTileSet properly initialize
 * the bkgrd_val1 variable (both low and high bytes).
 *
 * This catches regressions of the uninitialized memory bug fixed in
 * commit 47856dc2.
 */

#include <snes.h>

// Background layer definitions
#define BG0 0
#define BG1 1
#define BG2 2
#define BG3 3

// Test result structure
u16 test_status;
u16 test_total;
u16 test_passed;
u16 test_failed;

// External access to bkgrd_val1 for testing
extern u16 bkgrd_val1;

// Status codes
#define STATUS_RUNNING  0x0000
#define STATUS_PASSED   0x0001
#define STATUS_FAILED   0x00FF

// Simple tile data (just zeros)
const u8 tileset[32] = {0};
const u8 palette[32] = {0};

void check(u8 condition, const char* name) {
    test_total++;
    if (condition) {
        test_passed++;
    } else {
        test_failed++;
    }
}

int main(void) {
    test_status = STATUS_RUNNING;
    test_total = 0;
    test_passed = 0;
    test_failed = 0;

    // Test 1: bgSetEnable should clear high byte
    // Pre-poison bkgrd_val1 with garbage
    bkgrd_val1 = 0xDEAD;

    // Call bgSetEnable(BG0) - this should set bkgrd_val1 to a small value
    bgSetEnable(BG0);

    // After bgSetEnable, bkgrd_val1 should have high byte = 0
    check((bkgrd_val1 & 0xFF00) == 0, "bgSetEnable clears high byte");

    // Test 2: bgSetEnable with BG1
    bkgrd_val1 = 0xDEAD;
    bgSetEnable(BG1);
    check((bkgrd_val1 & 0xFF00) == 0, "bgSetEnable(BG1) clears high byte");

    // Test 3: bgSetEnable with BG2
    bkgrd_val1 = 0xDEAD;
    bgSetEnable(BG2);
    check((bkgrd_val1 & 0xFF00) == 0, "bgSetEnable(BG2) clears high byte");

    // Test 4: bgSetEnable with BG3
    bkgrd_val1 = 0xDEAD;
    bgSetEnable(BG3);
    check((bkgrd_val1 & 0xFF00) == 0, "bgSetEnable(BG3) clears high byte");

    // Test 5: bgInitTileSet with paletteEntry=0 (the buggy path)
    // This was the main bug - when paletteEntry=0, it branched without
    // initializing the high byte
    bkgrd_val1 = 0xDEAD;

    // Use minimal valid parameters
    // bgInitTileSet(bgNumber, tileSource, tilePalette, paletteEntry, tileSize, paletteSize, colorMode, address)
    // Note: This will fail silently if VRAM/palette not set up, but
    // we just need to verify bkgrd_val1 initialization
    bgInitTileSet(0, (u8*)tileset, (u8*)palette, 0, 32, 32, BG_16COLORS, 0x0000);

    check((bkgrd_val1 & 0xFF00) == 0, "bgInitTileSet(paletteEntry=0) clears high byte");

    // Test 6: bgInitTileSet with non-zero paletteEntry (uses different code path)
    bkgrd_val1 = 0xDEAD;
    bgInitTileSet(0, (u8*)tileset, (u8*)palette, 1, 32, 32, BG_16COLORS, 0x1000);
    check((bkgrd_val1 & 0xFF00) == 0, "bgInitTileSet(paletteEntry=1) clears high byte");

    // Set final status
    if (test_failed == 0) {
        test_status = STATUS_PASSED;
    } else {
        test_status = STATUS_FAILED;
    }

    // Infinite loop - Mesen will read results and exit
    while(1) {
        WaitForVBlank();
    }

    return 0;
}
