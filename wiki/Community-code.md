This page has been done to group code we saw everywhere. It contains some complete projects or sometimes only sample code made by the community but all can can be interesting to study.

Please keep in mind that some of them are not maintained, it can have a gap with the latest version of PVsneslib and do not build correctly.

Feel free to contribute and add a link to your project !

### Sound test project :

[This code](https://github.com/alekmaul/pvsneslib/files/6565523/SoundTest_ByDiegoLeao_PvsneslibV3.zip) made by diegoleao with PVSneslib v3 will explain you how to manage the audio part with the library (the "Readme.txt" file also have useful information that is not present in the code itself). 
If you need the previous version of SoundTest (for PVSneslib v2) you can find it [here](https://github.com/alekmaul/pvsneslib/files/5396843/SoundTest_ByDiegoLeao.zip).


### Street of rage demo port

Odelot shared his port of SoR2 which is now cancelled, it has been made with pvsneslib v2 : https://github.com/odelot/sor2_snes

### Bomberworld

1r3n33 created a game which is available on his github: https://github.com/1r3n33/bomberworld

It is very useful to see how you can organize your code!

### The last super

Dr. Ludos created it for a game jam then continued to improve it, the code is well commented. The original project is available on his website but it was build for PVSneslib v2: https://drludos.itch.io/the-last-super

It has been ported on PVSneslib v3 and is available [here](https://github.com/alekmaul/pvsneslib/files/6556286/TheLastSuper_sourcecode.zip)

### Scrolling map with collisions :

[This code](https://github.com/alekmaul/pvsneslib/files/7149024/mapscroll_example_with_collisions_v3.zip) made by diegoleao changes the original "map scroll example" to add collision checking. Move Mario up, down, left, and right to check for tiles that contain collisions. Press and hold B to move 1 pixel at a time, for finer control.

### Update the priority of a tile map area

#### Description

This code made by malayli updates a tile map area priority in any backgrounds like BG1 or BG2.
It's very useful if you want to have tiles appearing over and behind your sprites for example.

#### Code

```
u16 bgMapTemp[4096];
u16 bgMapPageNumberSize;
u8 bgMapX;
u16 bgMapY;
u16 bgMapYMax;

/*!\brief Set the priority of a tile map area at {x, y, width, height, pageNumber}
    \param tileMap the tile map
    \param priority the priority to be set for the tile map area
    \param x the X position of the tile map area
    \param y the Y position of the tile map area
    \param width the width of the tile map area
    \param height the height of the tile map area
    \param pageNumber the page number of the tile map
    \return the updated tile map
*/
char * updateTileMapAreaWithPriority(char *tileMap, u8 priority, u16 x, u16 y, u16 width, u16 height, u8 pageNumber) {
    bgMapPageNumberSize = 1024 * pageNumber;
    bgMapX = 0;
    bgMapY = (y * 32) + bgMapPageNumberSize;
    bgMapYMax = (y * 32) + (height * 32) + bgMapPageNumberSize;

    memcpy((u8 *) &bgMapTemp, tileMap, 4096);

    while(bgMapY < bgMapYMax) {
        if (x <= bgMapX && bgMapX < width) {
            bgMapTemp[bgMapY] = ((u16 *)tileMap)[bgMapY] | (priority<<13);
        }

        if (bgMapX == 31) {
            bgMapX = 0;

        } else {
            bgMapX++;
        }

        bgMapY++;
    }

    return (char *)bgMapTemp;
}
```

#### Example

![Update the priority of a tile map area](https://user-images.githubusercontent.com/48180545/187201833-17357bb2-eed4-48b8-a3b1-64a6538ece08.png)