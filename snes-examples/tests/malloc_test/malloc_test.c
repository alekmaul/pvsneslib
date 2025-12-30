/*
 * Malloc Test - Validates malloc() works in small projects (Issue #311)
 *
 * This test specifically checks that malloc() returns valid memory
 * when globram.data section is minimal/empty, which was previously
 * broken due to a compiler bug with NULL pointer comparisons.
 *
 * Test results are written to fixed memory locations for Mesen2 validation:
 * - $7E2000: test_status (0=running, 1=complete)
 * - $7E2002: test_total (number of tests run)
 * - $7E2004: test_passed (number of tests passed)
 * - $7E2006: test_failed (number of tests failed)
 */

#include <snes.h>

// Test results - use global variables in BSS section
// These will be in WRAM bank $7E and the Lua script will find them via symbol table
volatile u16 test_status;
volatile u16 test_total;
volatile u16 test_passed;
volatile u16 test_failed;

void check(u8 condition, const char *name)
{
    test_total++;
    if (condition)
    {
        test_passed++;
        consoleNocashMessage("PASS: %s\n", name);
    }
    else
    {
        test_failed++;
        consoleNocashMessage("FAIL: %s\n", name);
    }
}

int main(void)
{
    char *ptr1, *ptr2, *ptr3;
    u16 *intptr;
    int i;

    // Initialize test counters
    test_status = 0;
    test_total = 0;
    test_passed = 0;
    test_failed = 0;

    consoleNocashMessage("=== Malloc Test Suite ===\n");

    // Test 1: Basic malloc returns non-NULL
    ptr1 = (char *)malloc(10);
    check(ptr1 != 0, "malloc(10) returns non-NULL");

    // Test 2: Can write to allocated memory
    if (ptr1)
    {
        ptr1[0] = 'H';
        ptr1[1] = 'i';
        ptr1[2] = '\0';
        check(ptr1[0] == 'H' && ptr1[1] == 'i', "Write to malloc'd memory");
    }
    else
    {
        check(0, "Write to malloc'd memory (skipped - ptr is NULL)");
    }

    // Test 3: Multiple allocations
    ptr2 = (char *)malloc(20);
    ptr3 = (char *)malloc(30);
    check(ptr2 != 0 && ptr3 != 0, "Multiple malloc calls return non-NULL");

    // Test 4: Allocations are at different addresses
    check(ptr1 != ptr2 && ptr2 != ptr3 && ptr1 != ptr3,
          "Multiple allocations return different addresses");

    // Test 5: Larger allocation (256 bytes)
    intptr = (u16 *)malloc(256);
    check(intptr != 0, "malloc(256) returns non-NULL");

    // Test 6: Can use larger allocation
    if (intptr)
    {
        for (i = 0; i < 128; i++)
        {
            intptr[i] = (u16)(i * 2);
        }
        // Verify a few values
        check(intptr[0] == 0 && intptr[64] == 128 && intptr[127] == 254,
              "Read/write to larger allocation");
    }
    else
    {
        check(0, "Read/write to larger allocation (skipped - ptr is NULL)");
    }

    // Test 7: Free and reallocate
    free(ptr1);
    ptr1 = (char *)malloc(10);
    check(ptr1 != 0, "malloc after free returns non-NULL");

    // Mark tests complete
    test_status = 1;

    consoleNocashMessage("=== Tests Complete: %d/%d passed ===\n",
                         (int)test_passed, (int)test_total);

    // Visual feedback
    consoleInit();
    setScreenOn();

    if (test_failed == 0)
    {
        // Green = all passed
        setPaletteColor(0, RGB5(0, 31, 0));
    }
    else
    {
        // Red = some failed
        setPaletteColor(0, RGB5(31, 0, 0));
    }

    while (1)
    {
        WaitForVBlank();
    }

    return 0;
}
