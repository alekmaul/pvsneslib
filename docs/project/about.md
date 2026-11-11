# About PVSnesLib

PVSnesLib is an open-source development library and toolchain for creating homebrew games and applications for the Super Nintendo Entertainment System (SNES) **using the C programming language**.

The project brings together the C-facing library API, development tools, examples and documentation needed to build software for the console. A developer writes game and application logic in C and uses PVSnesLib functions to access SNES features such as backgrounds, sprites, controller input and audio. Low-level parts of the library and the target platform also involve 65816 assembly; PVSnesLib does not require every game developer to write assembly for ordinary projects.

## What you can build

With PVSnesLib, you can:

- Write game logic and application code in C.
- Create graphics using tiles, maps, palettes and sprites.
- Read controller input and supported peripherals.
- Add music and sound effects using the SNES audio facilities.
- Build ROMs and test them in a SNES emulator or on suitable hardware.

The supplied `snes-examples/hello_world/src/hello_world.c` is a concrete starting point: it includes `<snes.h>`, defines `main()` and calls PVSnesLib functions to initialize the display and draw text.

## Explore the project

- [Source code and releases](https://github.com/alekmaul/pvsneslib)
- [Community Wiki](https://github.com/alekmaul/pvsneslib/wiki)
- [Generated API reference](../api/index.md)

## Start developing

If you are new to PVSnesLib, begin with the [Getting started](../getting-started/index.md) section. It explains how to prepare the toolchain and build your first C project.

Then continue with the [tutorials](../tutorials/index.md), browse the [examples](../examples/index.md), and consult the [API reference](../api/index.md) when you need details about library functions.
