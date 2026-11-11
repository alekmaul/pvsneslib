# Your first PVSnesLib project

This guide takes the smallest reliable route from an installed PVSnesLib toolchain to a first Super Nintendo ROM: build the `hello_world` example shipped with PVSnesLib, inspect how it works, then use it as the starting point for your own project.

> **Before you begin:** PVSnesLib must already be installed and `PVSNESLIB_HOME` must point to its installation directory. On Windows, use the MSYS2 UCRT64 terminal for the build commands.

## 1. Start from the supplied example

PVSnesLib includes its examples in the `snes-examples` directory. Open:

```text
<PVSNESLIB_HOME>/snes-examples/hello_world
```

The example is a useful starting point because it already contains a working Makefile and the source code needed to build a ROM. Keep the original example intact; make a copy of the whole `hello_world` directory when you want to experiment.

## 2. Understand the project files

The example's main C source is:

```text
src/hello_world.c
```

The Makefile describes how the project is built and supplies the ROM configuration. In particular, it sets values such as `ROMNAME` and `ROMTITLE`, then includes the common build rules:

```make
include ${PVSNESLIB_HOME}/devkitsnes/snes_rules
```

Since PVSnesLib 4.6, `snes_rules` generates the ROM header automatically from the Makefile settings. You do not need to create or edit `hdr.asm` for a normal project. Manual header control is possible with `AUTOHDR := 0`, but it is not needed for this first build.

## 3. Read the Hello World program

The example uses the PVSnesLib API from `snes.h`:

```c
#include <snes.h>

int main(void)
{
    // Initialize the text console with the default font.
    consoleInitDefaultText(0);

    // Configure the background graphics and tile map locations.
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    // Use Mode 1 and disable the unused backgrounds.
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Draw text on the screen.
    consoleDrawText(10, 10, "Hello World !");
    consoleDrawText(6, 14, "WELCOME TO PVSNESLIB");
    consoleDrawText(3, 18, "HTTPS://WWW.PORTABLEDEV.COM");

    // Enable the display and wait for each vertical blank.
    setScreenOn();

    while (1)
    {
        WaitForVBlank();
    }

    return 0;
}
```

The example illustrates a few essential ideas:

- `main()` is the program entry point.
- `consoleInitDefaultText(0)` initializes the text console and its default font.
- `bgSetGfxPtr()` and `bgSetMapPtr()` configure the VRAM locations used by the background.
- `setMode(BG_MODE1, 0)` selects SNES background Mode 1.
- `consoleDrawText()` writes text to the console.
- `setScreenOn()` enables the display.
- `WaitForVBlank()` keeps the main loop synchronized with the SNES video frame.

The SNES runtime initialization is handled automatically. The example therefore starts directly with the application logic.

## 4. Build the ROM

Open the MSYS2 UCRT64 terminal, change to the `hello_world` directory, and run:

```sh
make clean
make
```

The build produces:

```text
hello_world.sfc
```

The example's Makefile uses a concise build summary. Full build output is also written to:

```text
hello_world.log
```

If the build fails, inspect the terminal output and the log file before changing the project configuration.

## 5. Run the ROM

Open `hello_world.sfc` in a SNES emulator. The expected result is a screen displaying the three text lines from the C program.

For a more reliable test, try the ROM in more than one emulator when possible. Emulator behavior can differ, and testing on real hardware is the best final check when suitable hardware is available.

## 6. Make a copy and start experimenting

Once the original example builds, copy the complete `hello_world` directory to a new location outside the installed examples directory. Keep the Makefile and its build-rule include in place.

Start with small changes:

1. Change one of the strings passed to `consoleDrawText()`.
2. Build again with `make`.
3. Open the newly generated `.sfc` file and check the result.
4. Add one feature at a time, using the dedicated tutorials for backgrounds, sprites, controller input, and audio.

If you move your C source files to another directory, the Makefile supports a `SRC` variable. Define it before including `snes_rules`, for example:

```make
SRC=./source
include ${PVSNESLIB_HOME}/devkitsnes/snes_rules
```

Use the supplied example as the reference for the rest of the Makefile settings rather than starting with a blank Makefile.

## Next steps

- [Installation](installation.md) — verify the toolchain and environment.
- [Graphics and backgrounds](../tutorials/graphics.md) — learn how background tiles, maps, and palettes work.
- [Sprites](../tutorials/sprites.md) — display and animate objects.
- [Controller input](../tutorials/input.md) — read joypad input.
- [Audio](../tutorials/audio.md) — add music and sound effects.
- [Community Wiki: compiling the Hello World example](https://github.com/alekmaul/pvsneslib/wiki/Compiling-helloworld-example) — additional details about the supplied example and its Makefile.
