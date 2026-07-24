
```
 _                   _____                     
| |                 / __  \                    
| |_ _ __ ___ __  __`' / /'___ _ __   ___  ___ 
| __| '_ ` _ \\ \/ /  / / / __| '_ \ / _ \/ __|
| |_| | | | | |>  < ./ /__\__ \ | | |  __/\__ \
 \__|_| |_| |_/_/\_\\_____/___/_| |_|\___||___/
```
A Tiled (https://www.mapeditor.org/) file converter for Super Nintendo development  
Developed by Alekmaul and distributed under the terms of the [MIT license](./LICENSE).

## Usage
```
tmx2snes [options] -i tmxfilename -m mapfilename  
```
where tmxfilename is a Tiled map, either:
- a **native `.tmx` file** (Tiled's own XML format, or
- a **`.tmj`/`.json` file** (Tiled's "Export As... JSON")

You can pass the extension explicitly (`-i level1.tmx`), or omit it and tmx2snes will look for `<name>.tmx` first, then fall back to `<name>.tmj`.  
mapfilename is the map file of tileset for tileset optimization

>[!IMPORTANT]
> **Native `.tmx` files: CSV layer format required.** In Tiled, set `Map > Map Properties... > Tile Layer Format` to **CSV** before saving. 
>
> Native `.tmx` tilesets may reference an external `.tsx` file (`firstgid="1" source="tiles.tsx"`) or be embedded directly -- both are supported and resolved relative to the `.tmx` file's location.

## Options
### File options
- `-i <tmx filename>` tiled map file: native `.tmx`, or `.tmj`/`.json` export
- `-m <tile filename>` file to use for map file of tileset for optimization

### Misc options 
- `-q` Quiet mode  
- `-v` Display version information  
  
## Example 
```bash
tmx2snes -i mytilemap.tmj -m mytilepic.map
```
or, using a native Tiled map instead of a JSON export:
```bash
tmx2snes -i mytilemap.tmx -m mytilepic.map
```
 
In your makefile, you need to prepare the tiles before the map conversion

```makefile
tiles.pic: tiles.png
	@echo convert map tileset... $(notdir $@)
	$(GFXCONV) -s 8 -o 16 -u 16 -p -m -i $<

map_1_1.m16: map_1_1.tmj tiles.pic
	@echo convert map tiled ... $(notdir $@)
	$(TMXCONV) -i $< -m tiles.map
```
or, if you're working directly from a native `.tmx` map instead of a JSON export:
```makefile
map_1_1.m16: map_1_1.tmx tiles.pic
	@echo convert map tiled ... $(notdir $@)
	$(TMXCONV) -i $< -m tiles.map
```

## acknowledgments
tmx2snes uses the following libraries:
* [AsciiArt](https://patorjk.com/software/taag/#p=display&f=Doom&t=tmx2snes%0A) tmx2snes logo with Doom font  
* [cut_tiled](https://github.com/RandyGaul/cute_headers) tiled json conversion  
