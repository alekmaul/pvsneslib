# API reference

The PVSnesLib API reference is generated from the library source with Doxygen. It is the authoritative place to look up function declarations, parameters, constants, structures, and low-level details.

<div class="home-hero-actions">
  <a class="md-button md-button--primary" href="https://alekmaul.github.io/pvsneslib/">Open the generated API reference</a>
  <a class="md-button" href="https://github.com/alekmaul/pvsneslib/tree/master/pvsneslib/include/snes">Browse public headers</a>
</div>

## Find an API by topic

Use this table to choose the most relevant part of the generated reference. The topic descriptions are orientation only; the Doxygen pages contain the actual declarations and documentation.

| Topic | What to look for | Related guide |
| --- | --- | --- |
| Graphics and video | Video initialization, background layers, tile and map setup, scrolling, display modes, and graphics helpers | [Graphics and backgrounds](../tutorials/graphics.md) |
| Sprites | OAM helpers, sprite attributes, positions, tile and palette selection | [Sprites](../tutorials/sprites.md) |
| Audio | Sound engine, music modules, sound effects, and SPC700 communication | [Audio](../tutorials/audio.md) |
| Input and peripherals | Joypad state, buttons, mouse, and Super Scope support | [Controller input](../tutorials/input.md) |
| System and memory | DMA, interrupts, memory helpers, and system-level functions | [SNES hardware](../hardware/index.md) |
| Console and debugging | Text console and debug output | [First project](../getting-started/first-project.md) |

The generated reference homepage groups its contents into 2D engine API, Audio API, Memory, System, User Input/output, Engine API functions, and Miscellaneous functions.

## How to use the reference

1. Open the [generated Doxygen documentation](https://alekmaul.github.io/pvsneslib/).
2. Browse the module or use Doxygen's search to find a function, macro, type, or constant.
3. Open the declaration to inspect its parameter types and documentation.
4. Follow the related tutorial or example to see how the API is used in a complete program.

When a declaration and an example appear to disagree, check the headers and source code from the same PVSnesLib revision that you are building. The online generated reference may describe a different release from a locally installed copy.

## API reference, tutorials, and Wiki

These resources serve different purposes:

- **Doxygen API reference:** function-level documentation generated from the library source.
- **MkDocs tutorials:** task-oriented explanations and workflows for common development activities.
- **GitHub Wiki:** community tips, troubleshooting, historical notes, and additional techniques.

Useful links:

- [Generated API reference](https://alekmaul.github.io/pvsneslib/)
- [Public SNES headers](https://github.com/alekmaul/pvsneslib/tree/master/pvsneslib/include/snes)
- [PVSnesLib source repository](https://github.com/alekmaul/pvsneslib)
- [Community Wiki](https://github.com/alekmaul/pvsneslib/wiki)
