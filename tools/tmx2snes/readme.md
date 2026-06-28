
```
 _                   _____                     
| |                 / __  \                    
| |_ _ __ ___ __  __`' / /'___ _ __   ___  ___ 
| __| '_ ` _ \\ \/ /  / / / __| '_ \ / _ \/ __|
| |_| | | | | |>  < ./ /__\__ \ | | |  __/\__ \
 \__|_| |_| |_/_/\_\\_____/___/_| |_|\___||___/
```
A Tiled (https://www.mapeditor.org/) json file converter for Super Nintendo development  
Developed by Alekmaul and distributed under the terms of the [MIT license](./LICENSE).

## Usage
```
tmx2snes [options] -i tmxfilename -m mapfilename  
```
where tmxfilename is a Tiled tmx file (in json format)  
  and mapfilename is the map file of tileset for tileset optimization

## Options
### File options
- `-i <tmx filename>` file to use for tiled tmx file in json format   
- `-m <tile filename>` file to use for map file of tileset for optimization

### Misc options 
- `-q` Quiet mode  
- `-v` Display version information  
  
## Example 
```
tmx2snes -i mytilemap.tmj -m mytilepic.map
```
 This will convert a myimage png file to a map/pal/pic files with 16 colors,palette entry #0,  8x8 tiles, a blank tile, a map, no border, 16 colors output.  

In your makefile, youneed to prepare the tiles before the map conversion

```
tiles.pic: tiles.png
	@echo convert map tileset... $(notdir $@)
	$(GFXCONV) -s 8 -o 16 -u 16 -p -m -i $<

map_1_1.m16: map_1_1.tmj tiles.pic
	@echo convert map tiled ... $(notdir $@)
	$(TMXCONV) -i $< -m tiles.map
```

## acknowledgments
tmx2snes uses the following libraries:
* [AsciiArt](https://patorjk.com/software/taag/#p=display&f=Doom&t=tmx2snes%0A) tmx2snes logo with Doom font  
* [cut_tiled](https://github.com/RandyGaul/cute_headers) tiled json conversion  
