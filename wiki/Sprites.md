Sprites is a term in 2D graphics programming which refers to an image or animation. The SNES has dedicated hardware for dealing with sprites.  

This makes the system very useful for 2D. Most gaming systems do not have a 2D core, and all sprites and other 2D graphics have to be handled manually (with bitmaps, masked binary bitmaps, quads, and so on ...).  

## SNES Sprite Specs

Before starting using them, let’s see what exactly the SNES is capable of...   

* SNES can display 128 different sprites on screen. The sprite definitions are stored in **O**bject **A**ttribute **M**emory, or **OAM**.  
* Each sprite can be flipped horizontally and/or vertically, can be moved all around the screen, can be animated (by updating the image used).
* All SNES sprites are 16 colors (but we have 8 palettes we can use for sprites).  Also, we can have only two sizes of sprites on the screen at once. A sprite size is a multiple of 8 pixels. 
* The character data for sprites is stored in **V**ideo **RAM**, or **VRAM** in the same format as background tiles.
* Two or more sprites can share the same set of tiles. When the sprites are larger than 8x8 pixels, they are arranged in columns, followed by rows of 8x8 tiles.  

## Sprite sizes
 
You can have two different sizes each time of screen, the OAM attribute defines (see below) which size you are going to use for a specific size. We named the first size the "Small" size and when we toggle the bit in OAM memory the "Large" size. Be careful, it's not a zoom of the sprite, it's only a way to define two different sizes.  

Here is a table showing how we can handle that :  

|Val|Small|Large|Comment|  
|---|-----|-----|-------|  
| 0|8x8   |16x16| Caution: |  
| 1|8x8   |32x32| In 224-lines mode, OBJs with 64-pixel height |  
| 2|8x8   |64x64| may wrap from lower to upper screen border. |  
| 3|16x16 |32x32| In 239-lines mode, the same problem applies |  
| 4|16x16 |64x64| also for OBJs with 32-pixel height. |  
| 5|32x32 |64x64| |  
| 6|16x32 |32x64| (undocumented) |  
| 7|16x32 |32x32| (undocumented) |  

## Transparent color

Last thing to take into account is that you don’t want the background color of your sprite to be visible.  

If you have a round sprite, like a ball, you wouldn’t want the square border to show. In order to do such behavior, the first color of the sprite palette is used for the background to be ‘removed’, or rather set as the transparent color (only 1 transparent color per sprite).  

You have to choose your transparent color : the best is magenta, as you don’t use it very often (red : 255, green : 0, blue : 255), or some use black ...  
 
![color](http://www.portabledev.com/wp-content/uploads/2018/02/soprite_color.png)

Here are some common colors used for transparency:  

|Color name|R,G,B|  
|----------|-----|  
| Magenta | 255,0,255  |  
| “Blackread” | 1,0,0 |  
| Off Black | 1,1,1 |  
| Off white | 254,254,254 |  

## OAM structure

The Object Attribute Memory are two tables which control the position, size, mirroring, palette, and priority of sprites.  

The first table has room for 128 entries, an entry for each sprite :  

```
  Byte 1    xxxxxxxx    x: X coordinate  
  Byte 2    yyyyyyyy    y: Y coordinate  
  Byte 3    cccccccc    c: starting character (tile) number    p: palette number  
  Byte 4    vhoopppc    v: vertical flip   h: horizontal flip  o: priority bits  
            Note: the 'c' in byte 4 is the MOST significant bit in the 9-bit char #.  
```

The second table is 32 bytes and has 2 bits for each sprite (each byte contains information for 4 sprites.)  The lowest significant bits hold the information for the lower object numbers (for example, the least significant two bits of the first byte are for object #0.)  Bit 0 (and 2, 4, 6) is the size toggle bit and bit 1 (3, 5, 7) is the most significant bit of the X coordinate. 

```
  bit 0/2/4/6 of byte OBJ/4: x   x: X most significant bit coordinate
  bit 1/3/5/7 of byte OBJ/4: s   s: size toggle bit 
```

### Sprites in VRAM

Sprites are interleaved in Video RAM. In other words, when using 32x32 tiles, there would be 3 more sprites stored in that "unused" space. 

When the sprites are larger than 8x8, they are arranged in columns, followed by rows of 8x8 tiles.  For example, a 32x32 sprite is stored like this:

```
  Byte Offset  0     32    64     96     128   160   192    224    256    ... and so on.  
  Tile Coord   (0,0) (8,0) (16,0) (24,0) (0,8) (8,8) (16,8) (24,8) (0,16) ... and so on.  
```

Now that you've consumed the above information, realize that it is not completely correct.  The SNES has one more display quirk that is thrown into the mix: all of the rows must be stored 16 tiles apart (i.e. 512 bytes, or 256 words.)  This means that, if there was one 32x32 sprite in VRAM, it would have to be stored like this:  

```
  Offset  Y-coord. Tile Coord  
  0       0-7      (0, 0) (8, 0) (16, 0) (24, 0) <Unused--room for 12 more tiles>  
  512     8-15     (0, 8) (8, 8) (16, 8) (24, 8) <Unused--room for 12 more tiles>  
  1024    16-23    (0,16) (8,16) (16,16) (24,16) <Unused--room for 12 more tiles>  
  1536    24-31    (0,24) (8,24) (16,24) (24,24) <Unused>  
```

If the first sprite, shown in the above table, started at offset 0, the next sprite would start at offset 128 (the Character Number, in OAM, would be 4); the third sprite would start at offset 256 (Character #8), and the fourth sprite would start at offset 384 (Character #12).  

If a fifth sprite was desired, the pattern would repeat and it would be located at offset 2048 (Character #64).  

Similarly, when using 16x16 sprites, there will be 8 sprites interleaved, and the ninth sprite will have to start after them at offset 1024.  Finally, 64x64 sprites have only two sprites interleaved.  


## Converting with gfx2snes

You can use **gfx2snes**, shipped with **devkitsnes** to convert your bitmap files into a correct format for PVSnesLib. Remember that the size must be a multiple of 8 pixels 8x8, 16x16, 32x32, and so on ... The table above gave you the correct size you can use for your sprites at a specific moment of your homebrew.  

As you can see above, the sprite graphics are arranged vertically to avoid problem during conversion.  

Here is an example of a makefile instruction to convert a sprite of 16 pix width / height with **gfx2snes**.  

```
#---------------------------------------------------------------------------------
sprites.pic: sprites.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -gs16 -pc16 -po16 -n $<
```

  gs16 because we have 16 pix width   
  pc16 because we can only use 16 colors  
  po16 because we are going to use only one palette of 16 colors  
  n because we do not have a border around the sprites  
  
Then, create a **data.asm** file with the converted file include in it, like you can see in **PVSnesLib** examples. This file will be included with your project and linked with the graphics.  

```
.include "hdr.asm"

.section ".rodata1" superfree
gfxpsrite: .incbin "sprites.pic"
gfxpsrite_end:

palsprite: .incbin "sprites.pal"
.ends
```

During initialization process in your C files, you will have to declare some external variables to allow functions to know which graphic sprites you are going to use, regarding the name you entered in your **data.asm** file.  

```
extern char gfxpsrite, gfxpsrite_end;
extern char palsprite;
```

## Init the sprites

**PVSnesLib** used an internal table to store sprites OAM. This table is named **oamMemory** and it is used to address the two OAM tables of the SNES.  
You don't have to declare another table to handle sprite OAM, and the include files shipped with **PVSnesLib** allows your homebrew to know the oam memory table.  
```
unsigned char oamMemory[128*4+8*4]; // to address OAM table low and high
```

During **consoleInit()** process, this table is init, so don't care about that ! The **consoleInit** does this call :  

```
  // Init sprites
  oamInit();
```

This function also put all the sprites outside the screen.  

The only thing that you need to do is to init graphics, palette and sprite size. The **oamInitGfxSet()** function does that, here is a common process to do such thing :  

```
    // Initialize SNES 
    consoleInit();
	
    // Init Sprites gfx and palette with default size of 16x16
    oamInitGfxSet(&gfxpsrite, (&gfxpsrite_end-&gfxpsrite), &palsprite, 0, 0x4000, OBJ_SIZE16_L32);
```

The parameters are :  

**&gfxpsrite**  address of sprite graphics  

**(&gfxpsrite_end-&gfxpsrite)** length of sprites graphics (so we did subtraction of end address and beginning address, easy isn't it ;-)  

**&palsprite** address of palette  

**0** palette number we are going to use (0 through 8), and remember only 16 colors for each graphics file !  

**0x4000** address in SNES VRAM where we are going to put the graphics (it's a 8K word step, be careful !).  

**OBJ_SIZE16_L32** size of sprite, you can use **OBJ_SIZE8_L16**, **OBJ_SIZE8_L32**, **OBJ_SIZE8_L64**, **OBJ_SIZE16_L32**, **OBJ_SIZE16_L64** and **OBJ_SIZE32_L64**. It represent the size when using **small** or **large** attribute with **oamSetEx** function, so OBJ_SIZE16_L32 mean the size is 16 pixel for small size (OBJ_SMALL) and 32 for large size (OBJ_LARGE).


Also, if you put all graphics in VRAM during one process (with a DMA copy for example) and want to change only the size and address of sprites, you can use the function **oamInitGfxAttr()** with two parameters : the address and the size.  

## Drawing the sprites

Drawing a sprite on screen is the same thing that transferring **OAM** variables to **SNES OAM**. This transfer can only be done during Vblank. But don't care about that, PVSnesLib does that for you :-D. the only thing to do is the definition of each sprite attribute in **OAM** variables.  

The function **oamSet()** does that : oamSet(id,  xspr, yspr, priority, hflip, vflip, gfxoffset, paletteoffset) .  

```
	oamSet(0,  monster.x, monster.y, 0, 0, 0, 0, 0); 
```

| parameters|description|  
|-----------|-----------|
| id | sprite number : be careful, must be a multiply by 4. because each sprite has 4 parameters in OAM memory, so 1st one is 0, second is 4, third is 8, and so on ... |  
| xspr | x coordinate : 0 to 255 |  
| yspr | y coordinate : 0 to 255 |  
| priority | priority relative to BG : 0 for Low and 3 is the highest |  
| hflip | 0 or 1 to flip or not horizontally |  
| vflip | 0 or 1 to flip or not vertically |  
| gfxoffset | the graphic offset, see the explanation above to compute the correct value |  
| paletteoffset |0 certainly, except if you have another palette number to use |  

The another function to use is oamSetEx() to define if the sprite is shown or not, and of course its size.  

```
	oamSetEx(0, OBJ_SMALL, OBJ_SHOW);
```

The first parameter is the sprite id (don't forget , it is multiply by 4 !).
For the next parameters, you can use **OBJ_SMALL** or **OBJ_LARGE** for the size and **OBJ_SHOW** or **OBJ_HIDE** to see or not the sprite :-P  

## Moving the sprites

You have two ways to move a sprite. You can use the **oamSet()** function and changing the x and y coordinates and you can also use the **oamSetXY()** function with only 3 parameters : **id** (hum, remember how to handle it now, 0, 4 , 8 and so on ;-) ), x and y coordinates.  

Of course, if you need to change also the flipping of the sprite, use the **oamset** function, like we do in the animated sprite example.  

```c
	// Wait for nothing :P
	while(1) {
		// Get current #0 pad
		pad0 = padsCurrent(0);
		
		if (pad0) {
			// Update sprite with current pad
			if(pad0 & KEY_UP) {
				if(monster.y >= SCREEN_TOP) monster.y--;
				monster.state = W_UP;
				monster.flipx = 0;
			}
			if(pad0 & KEY_LEFT) {
				if(monster.x >= SCREEN_LEFT) monster.x--;
				monster.state = W_LEFT;
				monster.flipx = 1;
			}
			if(pad0 & KEY_RIGHT) {
				if(monster.x <= SCREEN_RIGHT) monster.x++;
				monster.state = W_LEFT;
				monster.flipx = 0;
			}
			if(pad0 & KEY_DOWN) {
				if(monster.y <= SCREEN_BOTTOM) monster.y++;
				monster.state = W_DOWN;
				monster.flipx = 0;
			}
			monster.anim_frame++;
			if(monster.anim_frame >= FRAMES_PER_ANIMATION) monster.anim_frame = 0;
		}
		
		// Now, get current sprite in current animation
		monster.gfx_frame = sprTiles[monster.anim_frame + monster.state*FRAMES_PER_ANIMATION ];
		oamSet(0,  monster.x, monster.y, 3, monster.flipx, 0, monster.gfx_frame, 0);
		
		// Wait VBL 'and update sprites too ;-) )
		WaitForVBlank();
	}
```

**That's all for sprite tutorial, you are now able to move objects on SNES screen !** LOL  

![Anim](http://www.portabledev.com/wp-content/uploads/2018/02/animatedsprite-20120501-101156.png)

----
_Some parts from Qwertie's "SNES Documentation" and Matthew Callis's "Sprites in SNES development"_