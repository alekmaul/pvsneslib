## MapBuffer Extension

### Desciption:

- this addon extends the ability of PVSneslibs mapengine to change tiles during runtime. 


### Installation:

- copy the following files to your source folder
> "mapbufferextension.h","mapbufferextension.c", "mapbufferextensionA.asm"
- include with mapbufferextension.h to your c files, where you need the extension
> #include "mapbufferrextension.h"

### Load maps into mapbuffer[]

- first load the map into the buffer with the mapbuffersLoad 
- than call mapLoad as usual but now we point to the mapbuffer
```c
    #include "mapbufferextension.h"
    ...
    //load map into wram 
    mapbuffersLoad((u8 *)&mapmario, (&mapmario_end- &mapmario));
    //map engine gets the buffer instead
    mapLoad((u8 *)&mapbuffer, (u8 *)&tilesetdef, (u8 *)&tilesetatt);
```
congrats the easy part is done, now you can manipulate the map during runtime.
when you manipulate the maptiles in ram it doesn't mean the vram ram will be updated.
there are two ways to solve this problem

### Update vram 1 slow

one way to update the onscreen tiles is to set "mapdirty" to 1
```c
    mapbuffer[123] = 0x0001;
    mapdirty = 1;
```
- this updates the entire screen to the vram
- if frames doesn't matter, this the way to go

### Update vram 2 fast

the second approach uses additional buffers, the sequence is the following: 

1. select and check a tile
2. prebuffer the tile
3. manipulate the tile
4. push tiles that are currently on screen to a queue before v-blank
5. update vram from queue during v-blank

##### Detailed steps:

1. the first function __mapGetMetaTilesInfo__ is similar to __mapGetMetaTilesProp__ but it prestores some variables which are preparation for __dynamictilebuffer__ -> __dtb__.

2. the next function __GetDynamicTileID__ returns will setup or update the __dtb__ and it returns the index which can be used by on of the next function.

3. Manipulating variants

    1. the __mapChangeTileByID__ takes the index of the __dtb__, the new tile number and the tile attributes (tile attributes not fully integrated)

    2. __ManipulateDynamicTile__ extends the __mapChangeTileByID__, this is an example 
    of event driven tile manipulation (hit by head, stand on it.) feel free to extend it or write you own logic

    3. __DynamicTileAutoUpdate__ extends the __mapChangeTileByID__, this is an example how to manipulate a tile over time, feel free to extend it or write you own logic call it once before __maptileQueueUpdate__ 

4. __maptileQueueUpdate__ pushes tiles from __dtb__ into a __dynamictilequeue__ (__dtq__) when there on screen, call it once before WaitForVBlank 
    

5. the __maptileVRAMUpdate__ function update all queued tiles in the __dtq__ into vram __queuetilebuffer__ and flush the queue, call it once after WaitForVBlank because vram access is only allowed during v-blank  

### Important notes and limitations



- **!!!** only map widths based on __power of two__ are allowed
> tested map widths: 32, 62, 128
- **!!!** only testet with lowrom, the bank for wram is set to__$7F__ by default.
```assembly
;mapbufferextensionA.asm:
.DEFINE DTBUFFERBANK               $7F
```
- manipulated offscreen tiles will be updated by  mapUpdateCamera when the scree is scrolling

- the map size in byte must not exceed the size of the mapbuffer
> default 16.387 bytes (4003 hex), 3 bytes for the header, it is defined in asm file

```assembly
;mapbufferextensionA.asm:
.DEFINE MAPBUFFER_SIZE			 $4003
```
> the calculation of the map in bytes: (3+ 2*(mapheightblock * block width))
- the size of __dtb__ and the __dtq__ is set to 32 by default
- to change the size of __dynamictilebuffer__ has to be set in the header and the asm file as well


```assembly
;mapbufferextensionA.asm:
.DEFINE DTBUFFER_MAX                32
.DEFINE DTQUEUE_MAX                 32 
```

```c
//mapbufferextension.h:
#define DTBUFFER_MAX 32 // same value like in the asm file!
```
- to change the size of __dynamictilequeue__ has to be set asm file