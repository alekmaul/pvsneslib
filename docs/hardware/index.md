# SNES hardware overview

PVSnesLib targets the Super Nintendo Entertainment System (SNES). Understanding the console's main hardware resources helps explain why graphics, sprites, sound, and frame timing are handled in specific ways.

This page is an orientation to the hardware concepts used throughout the documentation. It is not a replacement for a full SNES register reference or a cycle-accurate hardware manual.

## Main components

The SNES combines a 65C816 main processor with dedicated video and audio hardware.

| Component | Main responsibility | Why it matters to a game |
| --- | --- | --- |
| 65C816 CPU | Runs the game logic and main program | Executes application code and coordinates hardware access. |
| PPU (Picture Processing Unit) | Generates the video output | Displays backgrounds, sprites, color effects, and windows. |
| SPC700 and audio DSP | Produce audio | Play music and sound effects using audio samples and sequencing data. |
| WRAM | Main system work memory | Holds variables, stacks, and runtime data. |
| VRAM | Video memory used by the PPU | Stores background tile graphics and tile maps. |
| CGRAM | Color memory used by the PPU | Stores the colors used by backgrounds and sprites. |
| OAM | Object attribute memory | Stores sprite attributes such as position, tile, palette, and size. |

## Video memory resources

The PPU uses several distinct memory areas. They are not interchangeable, and each has its own capacity and purpose.

### VRAM — tiles and tile maps

VRAM is 64 KiB of video memory. It stores background tile graphics and tile maps.

- A tile contains the pixel data for a small graphic element.
- A tile map describes which tiles appear on the background and how they are arranged.
- Background modes determine the available background layers and the supported tile formats.

When planning a screen, account for both the graphics and the maps they require. The VRAM layout configured by the program must not overlap incompatible resources.

See [Graphics and backgrounds](../tutorials/graphics.md) for the PVSnesLib background workflow.

### CGRAM — palettes

CGRAM stores 256 color entries, with each color represented in the SNES 15-bit color format.

Backgrounds and sprites use palette entries rather than storing a separate full RGB color with every pixel. The selected color depth and palette organization therefore affect how graphics assets are prepared.

See [Sprites](../tutorials/sprites.md) and [Graphics and backgrounds](../tutorials/graphics.md) for palette use in PVSnesLib.

### OAM — sprites

OAM contains the attributes for hardware objects (sprites), including their screen position, tile selection, palette selection, priority, and size.

The SNES supports up to 128 sprite entries. The PPU also has per-scanline limits, so a scene can exceed the visible sprite budget even when the OAM table itself is not full.

A metasprite in a game engine is a logical object made from multiple hardware sprites; it is not a separate SNES hardware feature.

See [Sprites](../tutorials/sprites.md) for OAM helpers, animation, and metasprites.

## CPU memory and ROM

The 65C816 has a 24-bit address bus and can address up to 16 MiB of address space. The SNES memory map places system resources, cartridge ROM, cartridge RAM, and hardware registers into different regions.

The exact ROM mapping depends on the cartridge mapping mode and build configuration. Do not assume that a CPU address is the same thing as a physical ROM file offset.

PVSnesLib's Makefile and shared `snes_rules` handle the normal ROM build process. Start with [First project](../getting-started/first-project.md) rather than manually constructing a ROM header.

## Video timing and VBlank

The PPU generates the video signal line by line. The vertical blanking interval (VBlank) occurs between frames and is commonly used for work that should be synchronized with the display.

PVSnesLib examples often use:

```c
while (1)
{
    WaitForVBlank();
}
```

This keeps the main loop synchronized with the video frame. It is also a natural point to schedule visual updates, but the amount of work performed during VBlank is limited by the available time.

Do not treat VBlank as unlimited time to upload arbitrary amounts of graphics data. Large transfers may need to be split across frames or planned with DMA and the library's transfer facilities.

## DMA and HDMA

The SNES provides direct memory access (DMA) channels so data can be transferred without the CPU copying every byte individually.

- **DMA** is commonly used for bulk transfers, including moving graphics or palette data to video hardware.
- **HDMA** can update selected hardware registers during the display, making effects such as per-scanline changes possible.

These mechanisms are hardware capabilities; the exact PVSnesLib helper functions and constraints should be checked in the matching API reference and examples before use.

## Audio hardware

The SNES audio subsystem is separate from the main 65C816 program. It uses an SPC700 processor and a dedicated audio DSP. Audio data and programs are loaded into the audio subsystem's memory before playback.

PVSnesLib provides routines for booting and communicating with the sound system, loading music banks, and triggering sound effects. See [Audio](../tutorials/audio.md) for the PVSnesLib workflow.

## Practical hardware-aware checklist

Before adding a feature, consider:

- **VRAM:** Do the tiles and maps fit in the planned layout?
- **CGRAM:** Are the palette entries and color depth compatible with the assets?
- **OAM:** Is the number of sprites reasonable, including per-scanline limits?
- **WRAM:** Is there enough work memory for maps, game state, and buffers?
- **VBlank:** Can the required visual updates fit in the available interval?
- **Audio memory:** Do music and effects fit the sound subsystem's resources?

These constraints are part of normal SNES development. Planning them early makes later debugging much easier.

## Further reading

- [PVSnesLib source repository](https://github.com/alekmaul/pvsneslib)
- [PVSnesLib Community Wiki](https://github.com/alekmaul/pvsneslib/wiki)
- [Graphics and backgrounds](../tutorials/graphics.md)
- [Sprites](../tutorials/sprites.md)
- [Audio](../tutorials/audio.md)
- [API reference](../api/index.md)

For register-level details, timing diagrams, and edge cases, consult a dedicated SNES hardware reference. This overview intentionally stays at the level needed to understand the PVSnesLib documentation.
