/*---------------------------------------------------------------------------------
    SNES Integer Calculator
    Uses s16 (16-bit signed) - the only reliable integer type on 816-tcc
    Range: -32768 to 32767
---------------------------------------------------------------------------------*/
#include <snes.h>

extern char tilfont, palfont;

// s16 is the only reliable type - s32 (long) is also 16-bit on 816-tcc!
s16 display_value;
s16 stored_value;
u16 pending_op;
u16 new_input;
u16 cursor_x, cursor_y;

#define BTN_X 6
#define BTN_Y 9
#define BTN_W 5
#define BTN_H 2
#define DSP_X 8
#define DSP_Y 5

//---------------------------------------------------------------------------------
// Integer power - uses s16 only
s16 intPow(s16 base, s16 exp) {
    s16 r, limit;
    u16 i;

    if (exp == 0) return 1;
    if (exp < 0) return 0;
    if (base == 0) return 0;
    if (base == 1) return 1;
    if (base == -1) return (exp & 1) ? -1 : 1;

    // Calculate overflow limit: 32767 / |base|
    if (base > 0) {
        limit = 32767 / base;
    } else {
        limit = 32767 / (-base);
    }

    r = base;
    for (i = 1; i < exp && i < 15; i++) {
        // Check for overflow before multiplying
        if (r > limit || r < -limit) break;
        r = r * base;
    }
    return r;
}

//---------------------------------------------------------------------------------
// Integer square root using Newton-Raphson
s16 mySqrt(s16 x) {
    s16 guess, lastGuess;
    u16 i;

    if (x <= 0) return 0;
    if (x == 1) return 1;

    guess = x / 2;
    if (guess == 0) guess = 1;

    for (i = 0; i < 10; i++) {
        lastGuess = guess;
        guess = (guess + x / guess) / 2;
        if (guess == lastGuess || guess == lastGuess + 1) break;
        if (guess <= 0) { guess = 1; break; }
    }

    return guess;
}

//---------------------------------------------------------------------------------
void displayNumber(void) {
    char buf[12];
    s16 n;
    u16 neg, i;

    n = display_value;
    neg = 0;
    if (n < 0) {
        neg = 1;
        n = -n;
    }

    for (i = 0; i < 11; i++) buf[i] = ' ';
    buf[11] = 0;

    // Build integer from right
    i = 10;
    if (n == 0) {
        buf[i] = '0';
        i--;
    } else {
        while (n > 0 && i > 0) {
            buf[i] = '0' + (n % 10);
            n = n / 10;
            i--;
        }
    }

    if (neg && i > 0) {
        buf[i] = '-';
    }

    consoleDrawText(DSP_X, DSP_Y, buf);
}

//---------------------------------------------------------------------------------
void drawOneButton(u16 r, u16 c, u16 sel) {
    u16 x, y;
    x = BTN_X + (c * BTN_W);
    y = BTN_Y + (r * BTN_H);

    if (sel) consoleDrawText(x, y, "[");
    else consoleDrawText(x, y, " ");

    if (r == 0) {
        if (c == 0) consoleDrawText(x+1, y, "7");
        else if (c == 1) consoleDrawText(x+1, y, "8");
        else if (c == 2) consoleDrawText(x+1, y, "9");
        else if (c == 3) consoleDrawText(x+1, y, "/");
        else consoleDrawText(x+1, y, "^");
    }
    else if (r == 1) {
        if (c == 0) consoleDrawText(x+1, y, "4");
        else if (c == 1) consoleDrawText(x+1, y, "5");
        else if (c == 2) consoleDrawText(x+1, y, "6");
        else if (c == 3) consoleDrawText(x+1, y, "*");
        else consoleDrawText(x+1, y, "R");
    }
    else if (r == 2) {
        if (c == 0) consoleDrawText(x+1, y, "1");
        else if (c == 1) consoleDrawText(x+1, y, "2");
        else if (c == 2) consoleDrawText(x+1, y, "3");
        else if (c == 3) consoleDrawText(x+1, y, "-");
        else consoleDrawText(x+1, y, "~");
    }
    else {
        if (c == 0) consoleDrawText(x+1, y, "0");
        else if (c == 1) consoleDrawText(x+1, y, ".");
        else if (c == 2) consoleDrawText(x+1, y, "=");
        else if (c == 3) consoleDrawText(x+1, y, "+");
        else consoleDrawText(x+1, y, "C");
    }

    if (sel) consoleDrawText(x+2, y, "]");
    else consoleDrawText(x+2, y, " ");
}

//---------------------------------------------------------------------------------
void drawButtons(void) {
    u16 r, c;
    for (r = 0; r < 4; r++) {
        for (c = 0; c < 5; c++) {
            drawOneButton(r, c, (cursor_x == c && cursor_y == r) ? 1 : 0);
        }
    }
}

//---------------------------------------------------------------------------------
void showOp(void) {
    consoleDrawText(DSP_X + 11, DSP_Y, " ");
    if (pending_op == 1) consoleDrawText(DSP_X + 11, DSP_Y, "+");
    else if (pending_op == 2) consoleDrawText(DSP_X + 11, DSP_Y, "-");
    else if (pending_op == 3) consoleDrawText(DSP_X + 11, DSP_Y, "*");
    else if (pending_op == 4) consoleDrawText(DSP_X + 11, DSP_Y, "/");
    else if (pending_op == 5) consoleDrawText(DSP_X + 11, DSP_Y, "^");
}

//---------------------------------------------------------------------------------
void doOp(void) {
    if (pending_op == 1) {
        display_value = stored_value + display_value;
    }
    else if (pending_op == 2) {
        display_value = stored_value - display_value;
    }
    else if (pending_op == 3) {
        display_value = stored_value * display_value;
    }
    else if (pending_op == 4 && display_value != 0) {
        display_value = stored_value / display_value;
    }
    else if (pending_op == 5) {
        display_value = intPow(stored_value, display_value);
    }
    pending_op = 0;
}

//---------------------------------------------------------------------------------
void pressBtn(void) {
    u16 digit;

    // Number buttons (rows 0-2, cols 0-2)
    if (cursor_x < 3 && cursor_y < 3) {
        if (cursor_y == 0) digit = 7 + cursor_x;
        else if (cursor_y == 1) digit = 4 + cursor_x;
        else digit = 1 + cursor_x;

        if (new_input) {
            display_value = digit;
            new_input = 0;
        } else if (display_value < 3276) {
            display_value = display_value * 10 + digit;
        }
        displayNumber();
        return;
    }

    // 0 button
    if (cursor_y == 3 && cursor_x == 0) {
        if (new_input) {
            display_value = 0;
            new_input = 0;
        } else if (display_value < 3276) {
            display_value = display_value * 10;
        }
        displayNumber();
        return;
    }

    // Decimal point (unused in integer mode)
    if (cursor_y == 3 && cursor_x == 1) {
        displayNumber();
        return;
    }

    // = button
    if (cursor_y == 3 && cursor_x == 2) {
        if (pending_op) doOp();
        new_input = 1;
        displayNumber();
        showOp();
        return;
    }

    // Clear
    if (cursor_y == 3 && cursor_x == 4) {
        display_value = 0;
        stored_value = 0;
        pending_op = 0;
        new_input = 1;
        displayNumber();
        showOp();
        return;
    }

    // Negate
    if (cursor_y == 2 && cursor_x == 4) {
        display_value = -display_value;
        displayNumber();
        return;
    }

    // Square root
    if (cursor_y == 1 && cursor_x == 4) {
        if (display_value >= 0) {
            display_value = mySqrt(display_value);
        }
        new_input = 1;
        displayNumber();
        return;
    }

    // Power
    if (cursor_y == 0 && cursor_x == 4) {
        if (pending_op) doOp();
        stored_value = display_value;
        pending_op = 5;
        new_input = 1;
        displayNumber();
        showOp();
        return;
    }

    // Basic operators
    if (cursor_x == 3) {
        if (pending_op) doOp();
        stored_value = display_value;
        if (cursor_y == 0) pending_op = 4;
        else if (cursor_y == 1) pending_op = 3;
        else if (cursor_y == 2) pending_op = 2;
        else pending_op = 1;
        new_input = 1;
        displayNumber();
        showOp();
    }
}

//---------------------------------------------------------------------------------
int main(void) {
    u16 pad0, oldpad;

    consoleSetTextMapPtr(0x6800);
    consoleSetTextGfxPtr(0x3000);
    consoleSetTextOffset(0x0100);
    consoleInitText(0, 16 * 2, &tilfont, &palfont);

    bgSetGfxPtr(0, 0x2000);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    display_value = 0;
    stored_value = 0;
    pending_op = 0;
    new_input = 1;
    cursor_x = 0;
    cursor_y = 0;
    oldpad = 0;

    consoleDrawText(4, 2, "SNES CALCULATOR");
    consoleDrawText(DSP_X - 2, DSP_Y, "[           ]");
    consoleDrawText(5, 19, "^=POW R=SQRT ~=NEG");

    displayNumber();
    drawButtons();

    setScreenOn();

    while (1) {
        pad0 = padsCurrent(0);

        if ((pad0 & KEY_UP) && !(oldpad & KEY_UP) && cursor_y > 0) {
            cursor_y--;
            drawButtons();
        }
        if ((pad0 & KEY_DOWN) && !(oldpad & KEY_DOWN) && cursor_y < 3) {
            cursor_y++;
            drawButtons();
        }
        if ((pad0 & KEY_LEFT) && !(oldpad & KEY_LEFT) && cursor_x > 0) {
            cursor_x--;
            drawButtons();
        }
        if ((pad0 & KEY_RIGHT) && !(oldpad & KEY_RIGHT) && cursor_x < 4) {
            cursor_x++;
            drawButtons();
        }

        if ((pad0 & KEY_A) && !(oldpad & KEY_A)) {
            pressBtn();
        }

        if ((pad0 & KEY_B) && !(oldpad & KEY_B)) {
            display_value = 0;
            stored_value = 0;
            pending_op = 0;
            new_input = 1;
            displayNumber();
            showOp();
        }

        oldpad = pad0;
        WaitForVBlank();
    }
    return 0;
}
