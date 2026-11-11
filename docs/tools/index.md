# Development tools

PVSnesLib includes the compiler, assembler, linker, and asset-conversion tools needed to build SNES software. Most projects use these tools through the Makefile and the shared `snes_rules`; you do not need to launch every utility manually.

This page explains the main tools and the role each one plays in the build process. Exact executable names and available options can vary between PVSnesLib releases, so use the tools bundled with your installed version as the reference.

## The build toolchain

| Tool | Role |
| --- | --- |
| `816-tcc` | Compiles C source code for the 65C816 processor. |
| `816-opt` | Optimizes the generated code. |
| `wla-65816` | Assembles 65C816 assembly source into object files. |
| `wlalink` | Links object files to produce the program image. |
| `constify` | Detects suitable constant data and moves it from RAM-oriented output to ROM-oriented output. |

A normal project Makefile includes the shared build rules supplied by PVSnesLib:

```make
include ${PVSNESLIB_HOME}/devkitsnes/snes_rules
```

Those rules configure the compiler and linker, process the project's source files, and generate the ROM. Prefer the supplied rules over manually invoking the compiler or linker.

## Graphics and map conversion

### `gfx4snes`

The PVSnesLib Community FAQ describes `gfx4snes` as the central graphics converter. It transforms source artwork into SNES-ready resources such as tile graphics, palettes, and tile maps.

The options are grouped by output type:

- **Graphics options** configure the tile graphics output.
- **Map options** configure tile-map generation and related map data.
- **Palette options** configure palette colors and palette entries.

The converter must be configured for the target SNES background mode, color depth, and palette layout. Those constraints are important: an image that looks correct in an editor is not necessarily valid as an SNES resource.

See [Graphics and backgrounds](../tutorials/graphics.md) and [Sprites](../tutorials/sprites.md) for the concepts behind the generated files.

> **Version note:** documentation and older project files may use different converter names (for example, `gfx2snes`). Check the executable and help output in your installed release before copying a command from an older tutorial. Do not rename a tool or change a Makefile variable simply to match a page written for another release.

### `tmx2snes`

Some examples use `tmx2snes` to convert maps created with the Tiled map editor into data that can be used by a PVSnesLib project. The exact layers and outputs are project-specific, so inspect the example's Makefile and conversion command before reusing it.

## Audio conversion

### `smconv`

`smconv` converts Impulse Tracker (`.it`) modules into sound banks that the SNES audio system can use. It is normally called by the project's Makefile when music files are included.

Music authoring tools such as OpenMPT can be used to create or edit tracker modules. The PVSnesLib Wiki notes an important compatibility setting: OpenMPT's IT sample compression is not supported by `smconv`. If conversion reports `error 5`, check the `ITCompressionMono` and `ITCompressionStereo` settings described in the [Community FAQ](https://github.com/alekmaul/pvsneslib/wiki/Community-FAQ).

### `snesbrr`

`snesbrr` converts WAV audio into BRR samples, the compressed sample format used by the SNES audio hardware. The resulting sample can then be included in a project using the audio functions described in [Audio](../tutorials/audio.md).

## Other utilities

| Tool | Purpose |
| --- | --- |
| `bin2txt` | Converts a binary file into text so it can be included in source code. The Wiki mentions its use for SPC700-related generated data. |
| `snestools` | Retained utility for inspecting a ROM header. The Community FAQ says it is no longer part of the normal post-build ROM-patching workflow. |

## Use the tools through the Makefile

For everyday development, the recommended workflow is:

1. Keep source artwork, music, and project source files in the project directory.
2. Read the example's Makefile to see which conversion steps it uses.
3. Let `snes_rules` and the project's explicit Makefile rules run the converters.
4. Run `make` to build the ROM.
5. If you change source assets or build settings and suspect stale generated files, run `make clean` before rebuilding.

The variables exported by the shared rules connect Makefiles to the tools installed under `PVSNESLIB_HOME`. Their names are implementation details of the supplied build system; use the Makefile from a matching example as the template for your own project.

## Find the right reference

- [Community FAQ — What is the goal of each tool?](https://github.com/alekmaul/pvsneslib/wiki/Community-FAQ#what-is-the-goal-of-each-tool)
- [Community FAQ — audio conversion notes](https://github.com/alekmaul/pvsneslib/wiki/Community-FAQ#what-are-it-files)
- [Graphics and backgrounds](../tutorials/graphics.md)
- [Sprites](../tutorials/sprites.md)
- [Audio](../tutorials/audio.md)
- [First project](../getting-started/first-project.md)

The command-line help shipped with your installed tool is the authoritative reference for its accepted options. When documenting a specific command in a project, record the PVSnesLib release used to validate it.
