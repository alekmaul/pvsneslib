# Graphics API

The generated Doxygen reference documents the functions and definitions used to configure video output, background layers, tile graphics, maps, and related effects.

**Open:** [PVSnesLib generated API reference](https://alekmaul.github.io/pvsneslib/)

**Source header:** [`background.h`](https://github.com/alekmaul/pvsneslib/blob/master/pvsneslib/include/snes/background.h)

## Start with these topics

- General video initialization and display configuration.
- Background layer setup and enable/disable functions.
- Tile graphics and tile-map loading.
- Background scrolling.
- LZ-compressed graphics loading.
- Pixel-mode and video-mode management.

For example, the public background header declares helpers such as `bgSetGfxPtr()`, `bgSetMapPtr()`, `bgInitTileSet()`, `bgInitMapSet()`, and `bgSetScroll()`. Consult the header and generated reference for the exact signatures and parameter requirements.

## Related guide

[Graphics and backgrounds](../tutorials/graphics.md) explains the concepts and a practical workflow. Use Doxygen for the complete function-level reference.
