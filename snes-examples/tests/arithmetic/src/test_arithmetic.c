/*---------------------------------------------------------------------------------
    PVSNESLIB Arithmetic Test Suite

    Tests s16 arithmetic operations to validate 816-tcc code generation.
    Results are stored in global variables that are read by the Lua test runner.

    Memory layout for Lua script (addresses in .sym file):
    - test_status: 0x00 = running, 0x01 = passed, 0xFF = failed
    - total_tests: Number of tests run
    - passed_tests: Number of tests passed
    - failed_tests: Number of tests failed
    - test_results[]: Array of individual test results (1=pass, 0=fail)
    - test_expected[]: Expected values for failed tests
    - test_actual[]: Actual values for failed tests
---------------------------------------------------------------------------------*/
#include <snes.h>

extern char tilfont, palfont;

// Test status values
#define STATUS_RUNNING  0x0000
#define STATUS_PASSED   0x0001
#define STATUS_FAILED   0x00FF

// Maximum number of tests
#define MAX_TESTS 128

// Test result variables - Lua script reads these from WRAM
u16 test_status;
u16 total_tests;
u16 passed_tests;
u16 failed_tests;
u16 current_test;
u16 display_row;

// Individual test results: 1 = pass, 0 = fail
u8 test_results[MAX_TESTS];

// Store expected/actual for failed tests (for debugging)
s16 test_expected[MAX_TESTS];
s16 test_actual[MAX_TESTS];

//---------------------------------------------------------------------------------
void initTest(void) {
    u16 i;

    test_status = STATUS_RUNNING;
    total_tests = 0;
    passed_tests = 0;
    failed_tests = 0;
    current_test = 0;
    display_row = 4;

    // Clear results arrays
    for (i = 0; i < MAX_TESTS; i++) {
        test_results[i] = 0;
        test_expected[i] = 0;
        test_actual[i] = 0;
    }
}

//---------------------------------------------------------------------------------
void check(s16 actual, s16 expected, const char* name) {
    char buf[32];
    u16 i;
    u16 testIndex;

    testIndex = total_tests;  // Use current count as index (0-based)
    current_test++;
    total_tests++;

    // Store expected and actual values
    if (testIndex < MAX_TESTS) {
        test_expected[testIndex] = expected;
        test_actual[testIndex] = actual;
    }

    if (actual == expected) {
        passed_tests++;
        if (testIndex < MAX_TESTS) {
            test_results[testIndex] = 1;  // Pass
        }

        // Display pass
        for (i = 0; i < 28; i++) buf[i] = ' ';
        buf[28] = 0;
        buf[0] = 'O'; buf[1] = 'K'; buf[2] = ':'; buf[3] = ' ';
        for (i = 0; name[i] && i < 20; i++) {
            buf[4 + i] = name[i];
        }
        consoleDrawText(1, display_row, buf);
    } else {
        failed_tests++;
        if (testIndex < MAX_TESTS) {
            test_results[testIndex] = 0;  // Fail (already 0, but explicit)
        }

        // Display fail
        for (i = 0; i < 28; i++) buf[i] = ' ';
        buf[28] = 0;
        buf[0] = 'F'; buf[1] = 'A'; buf[2] = 'I'; buf[3] = 'L'; buf[4] = ':'; buf[5] = ' ';
        for (i = 0; name[i] && i < 18; i++) {
            buf[6 + i] = name[i];
        }
        consoleDrawText(1, display_row, buf);
    }

    display_row++;
    if (display_row > 25) display_row = 4;  // Wrap around
}

//---------------------------------------------------------------------------------
void displaySummary(void) {
    char buf[32];
    u16 i, n;

    for (i = 0; i < 30; i++) buf[i] = ' ';
    buf[30] = 0;

    // "TOTAL: XX  PASS: XX  FAIL: XX"
    buf[0] = 'T'; buf[1] = 'O'; buf[2] = 'T'; buf[3] = ':';
    n = total_tests;
    buf[5] = '0' + (n / 10);
    buf[6] = '0' + (n % 10);

    buf[8] = 'P'; buf[9] = 'A'; buf[10] = 'S'; buf[11] = 'S'; buf[12] = ':';
    n = passed_tests;
    buf[13] = '0' + (n / 10);
    buf[14] = '0' + (n % 10);

    buf[16] = 'F'; buf[17] = 'A'; buf[18] = 'I'; buf[19] = 'L'; buf[20] = ':';
    n = failed_tests;
    buf[21] = '0' + (n / 10);
    buf[22] = '0' + (n % 10);

    consoleDrawText(1, 2, buf);

    // Update final status
    if (failed_tests == 0) {
        test_status = STATUS_PASSED;
        consoleDrawText(1, 27, "*** ALL TESTS PASSED ***");
    } else {
        test_status = STATUS_FAILED;
        consoleDrawText(1, 27, "*** TESTS FAILED ***");
    }
}

//---------------------------------------------------------------------------------
// Test functions
//---------------------------------------------------------------------------------

void test_addition(void) {
    s16 a, b, r;

    // Basic addition
    a = 5; b = 3;
    r = a + b;
    check(r, 8, "5+3=8");

    // Addition with zero
    a = 100; b = 0;
    r = a + b;
    check(r, 100, "100+0=100");

    // Negative addition
    a = -10; b = 3;
    r = a + b;
    check(r, -7, "-10+3=-7");

    // Large numbers
    a = 10000; b = 20000;
    r = a + b;
    check(r, 30000, "10000+20000");
}

void test_subtraction(void) {
    s16 a, b, r;

    // Basic subtraction
    a = 10; b = 3;
    r = a - b;
    check(r, 7, "10-3=7");

    // Result negative
    a = 5; b = 10;
    r = a - b;
    check(r, -5, "5-10=-5");

    // Both negative
    a = -10; b = -3;
    r = a - b;
    check(r, -7, "-10-(-3)=-7");
}

void test_multiplication(void) {
    s16 a, b, r;

    // Basic multiplication
    a = 6; b = 7;
    r = a * b;
    check(r, 42, "6*7=42");

    // Multiply by zero
    a = 100; b = 0;
    r = a * b;
    check(r, 0, "100*0=0");

    // Multiply by one
    a = 42; b = 1;
    r = a * b;
    check(r, 42, "42*1=42");

    // Negative multiply
    a = -5; b = 7;
    r = a * b;
    check(r, -35, "-5*7=-35");

    // Both negative
    a = -4; b = -8;
    r = a * b;
    check(r, 32, "-4*-8=32");

    // Larger values
    a = 100; b = 100;
    r = a * b;
    check(r, 10000, "100*100=10000");

    // Max safe multiply (181*181 = 32761)
    a = 181; b = 181;
    r = a * b;
    check(r, 32761, "181*181=32761");
}

void test_division(void) {
    s16 a, b, r;

    // Basic division
    a = 42; b = 6;
    r = a / b;
    check(r, 7, "42/6=7");

    // Division with remainder (truncates)
    a = 10; b = 3;
    r = a / b;
    check(r, 3, "10/3=3");

    // Negative division
    a = -42; b = 6;
    r = a / b;
    check(r, -7, "-42/6=-7");

    // Divide by one
    a = 123; b = 1;
    r = a / b;
    check(r, 123, "123/1=123");
}

void test_modulo(void) {
    s16 a, b, r;

    // Basic modulo
    a = 10; b = 3;
    r = a % b;
    check(r, 1, "10%3=1");

    // No remainder
    a = 12; b = 4;
    r = a % b;
    check(r, 0, "12%4=0");

    // Modulo of smaller
    a = 3; b = 10;
    r = a % b;
    check(r, 3, "3%10=3");
}

void test_shifts(void) {
    s16 a;
    u16 ua;

    // Left shift
    a = 1;
    a = a << 3;
    check(a, 8, "1<<3=8");

    // Right shift (signed)
    a = 16;
    a = a >> 2;
    check(a, 4, "16>>2=4");

    // Left shift larger
    a = 5;
    a = a << 10;
    check(a, 5120, "5<<10=5120");

    // Unsigned right shift
    ua = 0x8000;
    ua = ua >> 1;
    check((s16)ua, 0x4000, "0x8000>>1");
}

void test_comparisons(void) {
    s16 a, b, r;

    // Less than
    a = 5; b = 10;
    r = (a < b) ? 1 : 0;
    check(r, 1, "5<10 true");

    // Greater than
    r = (a > b) ? 1 : 0;
    check(r, 0, "5>10 false");

    // Equal
    a = 42; b = 42;
    r = (a == b) ? 1 : 0;
    check(r, 1, "42==42 true");

    // Negative comparison
    a = -10; b = -5;
    r = (a < b) ? 1 : 0;
    check(r, 1, "-10<-5 true");
}

void test_power(void) {
    s16 base, exp, r, i;

    // 2^10 = 1024 (manual calculation)
    base = 2; exp = 10;
    r = 1;
    for (i = 0; i < exp; i++) {
        r = r * base;
    }
    check(r, 1024, "2^10=1024");

    // 3^5 = 243
    base = 3; exp = 5;
    r = 1;
    for (i = 0; i < exp; i++) {
        r = r * base;
    }
    check(r, 243, "3^5=243");
}

void test_bitwise(void) {
    u16 a, b, r;

    // AND
    a = 0xFF00; b = 0x0FF0;
    r = a & b;
    check((s16)r, (s16)0x0F00, "AND: FF00&0FF0");

    // OR
    a = 0xFF00; b = 0x00FF;
    r = a | b;
    check((s16)r, (s16)0xFFFF, "OR: FF00|00FF");

    // XOR
    a = 0xAAAA; b = 0x5555;
    r = a ^ b;
    check((s16)r, (s16)0xFFFF, "XOR: AAAA^5555");

    // NOT (complement)
    a = 0x00FF;
    r = ~a;
    check((s16)r, (s16)0xFF00, "NOT: ~00FF");

    // Combined: extract high byte and OR with low byte shifted up
    // 0x1234 & 0xFF00 = 0x1200, (0x1234 & 0x00FF) << 8 = 0x3400
    // Result: 0x1200 | 0x3400 = 0x3600
    a = 0x1234;
    r = (a & 0xFF00) | ((a & 0x00FF) << 8);
    check((s16)r, (s16)0x3600, "Combined mask ops");
}

void test_increment(void) {
    s16 a;

    // Pre-increment
    a = 5;
    ++a;
    check(a, 6, "++a (5->6)");

    // Post-increment
    a = 10;
    a++;
    check(a, 11, "a++ (10->11)");

    // Pre-decrement
    a = 8;
    --a;
    check(a, 7, "--a (8->7)");

    // Post-decrement
    a = 3;
    a--;
    check(a, 2, "a-- (3->2)");

    // Increment in expression
    a = 5;
    check(a++, 5, "a++ returns 5");
    check(a, 6, "a is now 6");

    // Decrement in expression
    a = 10;
    check(--a, 9, "--a returns 9");
}

void test_unary(void) {
    s16 a, r;

    // Negation
    a = 42;
    r = -a;
    check(r, -42, "-(42)=-42");

    // Double negation
    a = -100;
    r = -a;
    check(r, 100, "-(-100)=100");

    // Negation of zero
    a = 0;
    r = -a;
    check(r, 0, "-(0)=0");

    // Logical NOT
    a = 0;
    r = !a;
    check(r, 1, "!0=1");

    a = 42;
    r = !a;
    check(r, 0, "!42=0");

    a = -1;
    r = !a;
    check(r, 0, "!(-1)=0");
}

void test_compound(void) {
    s16 a;

    // Compound assignment operators
    a = 10;
    a += 5;
    check(a, 15, "a+=5 (10->15)");

    a = 20;
    a -= 8;
    check(a, 12, "a-=8 (20->12)");

    a = 6;
    a *= 7;
    check(a, 42, "a*=7 (6->42)");

    a = 100;
    a /= 4;
    check(a, 25, "a/=4 (100->25)");

    a = 17;
    a %= 5;
    check(a, 2, "a%=5 (17->2)");

    a = 0x00F0;
    a &= 0x0F0F;
    check(a, 0x0000, "a&=0F0F");

    a = 0x00F0;
    a |= 0x0F00;
    check(a, 0x0FF0, "a|=0F00");

    a = 4;
    a <<= 3;
    check(a, 32, "a<<=3 (4->32)");

    a = 64;
    a >>= 2;
    check(a, 16, "a>>=2 (64->16)");
}

void test_array(void) {
    s16 arr[5];
    s16 i, sum;

    // Array write and read
    arr[0] = 10;
    arr[1] = 20;
    arr[2] = 30;
    arr[3] = 40;
    arr[4] = 50;

    check(arr[0], 10, "arr[0]=10");
    check(arr[2], 30, "arr[2]=30");
    check(arr[4], 50, "arr[4]=50");

    // Array sum
    sum = 0;
    for (i = 0; i < 5; i++) {
        sum += arr[i];
    }
    check(sum, 150, "sum(arr)=150");

    // Array with expressions
    arr[1+1] = 99;
    check(arr[2], 99, "arr[1+1]=99");
}

// s32/u32 tests - now enabled with typedef fix (long long = 32-bit)
// Note: s32 is now defined as 'signed long long' which is 4 bytes on 816-tcc

// 32-bit expected/actual values (store as two s16 values: low and high words)
s16 test_expected32_lo[MAX_TESTS];
s16 test_expected32_hi[MAX_TESTS];
s16 test_actual32_lo[MAX_TESTS];
s16 test_actual32_hi[MAX_TESTS];

// Helper: check 32-bit values by comparing low and high words
void check32(s32 actual, s32 expected, const char* name) {
    char buf[32];
    u16 i;
    u16 testIndex;
    s16 act_lo, act_hi, exp_lo, exp_hi;
    u16 pass;

    // Extract low and high words
    act_lo = (s16)(actual & 0xFFFF);
    act_hi = (s16)((actual >> 16) & 0xFFFF);
    exp_lo = (s16)(expected & 0xFFFF);
    exp_hi = (s16)((expected >> 16) & 0xFFFF);

    testIndex = total_tests;
    current_test++;
    total_tests++;

    // Store for debugging
    if (testIndex < MAX_TESTS) {
        test_expected32_lo[testIndex] = exp_lo;
        test_expected32_hi[testIndex] = exp_hi;
        test_actual32_lo[testIndex] = act_lo;
        test_actual32_hi[testIndex] = act_hi;
        // Also store in regular arrays for compatibility
        test_expected[testIndex] = exp_lo;
        test_actual[testIndex] = act_lo;
    }

    // Compare both words
    pass = (act_lo == exp_lo && act_hi == exp_hi) ? 1 : 0;

    if (pass) {
        passed_tests++;
        if (testIndex < MAX_TESTS) {
            test_results[testIndex] = 1;
        }
        for (i = 0; i < 28; i++) buf[i] = ' ';
        buf[28] = 0;
        buf[0] = 'O'; buf[1] = 'K'; buf[2] = ':'; buf[3] = ' ';
        for (i = 0; name[i] && i < 20; i++) {
            buf[4 + i] = name[i];
        }
        consoleDrawText(1, display_row, buf);
    } else {
        failed_tests++;
        if (testIndex < MAX_TESTS) {
            test_results[testIndex] = 0;
        }
        for (i = 0; i < 28; i++) buf[i] = ' ';
        buf[28] = 0;
        buf[0] = 'F'; buf[1] = 'A'; buf[2] = 'I'; buf[3] = 'L'; buf[4] = ':'; buf[5] = ' ';
        for (i = 0; name[i] && i < 18; i++) {
            buf[6 + i] = name[i];
        }
        consoleDrawText(1, display_row, buf);
    }

    display_row++;
    if (display_row > 25) display_row = 4;
}

void test_s32_basic(void) {
    s32 a, b, r;

    // Test that s32 is actually 32-bit by using values > 65535
    a = 100000;
    check32(a, 100000, "s32 assign 100k");

    // 32-bit addition
    a = 50000;
    b = 50000;
    r = a + b;
    check32(r, 100000, "50k+50k=100k");

    // Larger addition
    a = 500000;
    b = 500000;
    r = a + b;
    check32(r, 1000000, "500k+500k=1M");

    // 32-bit subtraction
    a = 100000;
    b = 30000;
    r = a - b;
    check32(r, 70000, "100k-30k=70k");

    // 32-bit multiplication
    a = 1000;
    b = 1000;
    r = a * b;
    check32(r, 1000000, "1k*1k=1M");

    // Another multiplication
    a = 500;
    b = 500;
    r = a * b;
    check32(r, 250000, "500*500=250k");

    // 32-bit division
    a = 1000000;
    b = 1000;
    r = a / b;
    check32(r, 1000, "1M/1k=1k");

    // Another division
    a = 100000;
    b = 100;
    r = a / b;
    check32(r, 1000, "100k/100=1k");
}

//---------------------------------------------------------------------------------
int main(void) {
    consoleSetTextMapPtr(0x6800);
    consoleSetTextGfxPtr(0x3000);
    consoleSetTextOffset(0x0100);
    consoleInitText(0, 16 * 2, &tilfont, &palfont);

    bgSetGfxPtr(0, 0x2000);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    consoleDrawText(1, 1, "PVSNESLIB ARITHMETIC TESTS");

    initTest();
    setScreenOn();

    // Run all tests
    test_addition();
    test_subtraction();
    test_multiplication();
    test_division();
    test_modulo();
    test_shifts();
    test_comparisons();
    test_power();
    test_bitwise();
    test_increment();
    test_unary();
    test_compound();
    test_array();
    test_s32_basic();  // Now enabled with long long typedef fix

    displaySummary();

    // Infinite loop - Lua script will read results and exit
    while (1) {
        WaitForVBlank();
    }
    return 0;
}
