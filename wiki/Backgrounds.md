**update in progress **

Background is a term in 2D graphics programming which refers to the image displayed on background of screen, in opposite of sprites. The Snes has dedicated hardware for dealing with multiple types of backgrounds.

## SNES Background Specs

SNES backgrounds are composed of tiled background. A tiled background consists of a collection of tiles and a map. Each tile has a number , so the map is the collection of numbers that are represented with the tiles.

A tile is a collection of 64 pixels forming a rectangle 8 pixels wide and 8 pixels tall. It is also possible to have 16x16 tiles but it is not implemented in PVSneslib for now.
A tile’s colors are defined by the tile’s color palette.

At least, a palette is a collection of either 4, 16 or 256 colors.

In order to render background, you has to set few things for each background, remember to set this for every background you want to render.

## Background Mode (affects number of planes, bitdepth)

The SNES has 8 background modes.
- Named Mode 0-7
- Mode 1 has a variation mode
- Mode 7 has a submode called Mode 7 Ext. BG
- Each mode has a different amount of background planes available
- Each mode has a different amount of colors for the available background planes for that mode
- Each mode has slightly different rendering properties
- For all modes and for all graphics planes, color index 0 for any subpalette is the transparency color.

```
Mode    # Colors for BG
         1   2   3   4
======---=---=---=---=
0        4   4   4   4
1       16  16   4   -
2       16  16   -   -
3      256  16   -   -
4      256   4   -   -
5       16   4   -   -
6       16   -   -   -
7      256   -   -   -
7EXTBG 256 128   -   -
```
In all modes and for all BGs, color 0 in any palette is considered transparent.

## Background sizes

In VRAM there is a two-dimensional array that is a map of the tiles on the screen. Depending on the setting of registers $2107 to $210A, this map may be 32x32, 32x64, 64x32 or 64x64 tiles in size.

All tilemaps are 32x32, bits 0-1 simply select the number of 32x32 tilemaps and how they're laid out in memory.

```
00  32x32   AA
            AA
01  64x32   AB
            AB
10  32x64   AA
            BB
11  64x64   AB
            CD
```

## Background in VRAM
## Backgrounds with PVSnesLib
## Converting with gfx4snes
## Init the background
## Displaying the background

Backgrounds begin at x = 0 and y = 1. It is not a bug and it is linked to a technical constraint :

> Note that many games will set their vertical scroll values to -1 rather than 0.
> This is because the SNES loads OBJ data for each scanline during the previous scanline. The very first line, though, wouldn't have any OBJ data loaded! So the SNES doesn't actually output scanline 0, although it does everything to render it.

If you want more information on it, you can consult [this page](https://wiki.superfamicom.org/backgrounds#toc-3)
