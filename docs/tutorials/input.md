# Controller input and peripherals

PVSnesLib supports standard SNES joypads, the MultiPlayer 5 multitap, the SNES mouse, and the Super Scope.

This guide is based on the [PVSnesLib Wiki — Gamepad and Peripherals](https://github.com/alekmaul/pvsneslib/wiki/Gamepad-and-Peripherals), and checked against `include/snes/input.h`, `source/input.asm`, and the supplied examples under `snes-examples/input`.

## Reading a standard joypad

The standard controller ports are identified as pad `0` and pad `1`. The input header states that inputs are read automatically by the VBlank ISR on non-lag frames; game code does not need to call a scan function every frame.

Read the current state with `padsCurrent()`:

```c
unsigned short pad0;

pad0 = padsCurrent(0);

if (pad0 & KEY_A) {
    /* A is currently pressed. */
}

if (pad0 & (KEY_A | KEY_X)) {
    /* A or X is pressed. */
}
```

To test whether both buttons are pressed, compare the masked value with the full mask:

```c
if ((pad0 & (KEY_A | KEY_X)) == (KEY_A | KEY_X)) {
    /* Both A and X are pressed. */
}
```

The button constants defined in `include/snes/input.h` are `KEY_A`, `KEY_B`, `KEY_X`, `KEY_Y`, `KEY_SELECT`, `KEY_START`, `KEY_RIGHT`, `KEY_LEFT`, `KEY_DOWN`, `KEY_UP`, `KEY_R`, and `KEY_L`.

The header also provides `padsDown()` for newly pressed keys (the 0-to-1 transition) and `padsUp()` for released keys (the 1-to-0 transition).

## MultiPlayer 5 multitap

A MultiPlayer 5 adapter can expose up to five pads, numbered `0` through `4`. Call `detectMPlay5()` during startup; the `snes_mplay5` variable is set to 1 when the adapter is detected.

```c
detectMPlay5();

if (snes_mplay5) {
    u8 i;
    for (i = 0; i < 5; i++) {
        unsigned short keys = padsCurrent(i);
        if (keys & KEY_A) {
            /* Pad i has A pressed. */
        }
    }
}
```

The input header carries an important hardware warning: do not write to `REG_WRIO` while MultiPlayer 5 is active. See `snes-examples/input/multiplay5` for a complete example.

## SNES mouse

The mouse API has two distinct initialization/detection functions:

- `initMouse(sensitivity)` clears the mouse state variables, sets the initial sensitivity for both ports, and enables mouse reading in the VBlank ISR.
- `detectMouse()` checks the controller auto-read data and sets `snes_mouse` when a mouse signature is found. It does not perform the same initialization as `initMouse()`.

The supplied `snes-examples/input/mouse/mouse.c` example uses `initMouse(MOUSE_SLOW)` and then waits one VBlank before checking `mouseConnect[]`:

```c
initMouse(MOUSE_SLOW);
WaitForVBlank();

if (mouseConnect[0]) {
    /* A mouse is connected to port 0. */
}
```

Mouse state arrays use the controller port as their index (`0` or `1`):

- `mouseButton[port]`: button mask pressed on the current frame (click mode).
- `mousePressed[port]`: button mask currently held (turbo/held mode).
- `mouseConnect[port]`: whether a mouse is present.
- `mouse_x[port]` and `mouse_y[port]`: encoded displacement. Bit 7 gives direction; the lower seven bits give acceleration.

The button masks are `mouse_L` and `mouse_R`. Sensitivity values are `MOUSE_SLOW`, `MOUSE_MEDIUM` and `MOUSE_FAST`. `mouseSetSensitivity(port, sensitivity)` accepts sensitivity values 0–2; the header notes that the change is queued for the next VBlank and does not affect the Hyperkin clone mouse.

See `snes-examples/input/mouse` and `snes-examples/input/mouse-data-test` for complete examples.

## Super Scope

Call `detectSuperScope()` to detect a Super Scope on the console's second controller port. The result is stored in `snes_sscope`.

The input header defines the state variables `scope_down`, `scope_now`, `scope_held`, and `scope_last`, along with flags:

- `SSC_FIRE`
- `SSC_CURSOR`
- `SSC_PAUSE`
- `SSC_TURBO`
- `SSC_OFFSCREEN`
- `SSC_NOISE`

The supplied `snes-examples/input/superscope` example calls `detectSuperScope()` during startup and checks `scope_down` for button flags. It also demonstrates aim adjustment using the raw and adjusted shot-position variables declared in `input.h`, including `scope_shothraw`, `scope_shotvraw`, `scope_shoth`, and `scope_shotv`.

See `snes-examples/input/superscope` for the complete shooting-gallery example.

## Reference

- `include/snes/input.h` and `source/input.asm` in the supplied PVSnesLib source archive.
- `snes-examples/input` in the supplied examples archive.
- [Gamepad and Peripherals — PVSnesLib Wiki](https://github.com/alekmaul/pvsneslib/wiki/Gamepad-and-Peripherals)
