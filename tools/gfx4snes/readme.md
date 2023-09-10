
```
        __        ___                     
       / _|      /   |                    
  __ _| |___  __/ /| |___ _ __   ___  ___ 
 / _` |  _\ \/ / /_| / __| '_ \ / _ \/ __|
| (_| | |  >  <\___  \__ \ | | |  __/\__ \
 \__, |_| /_/\_\   |_/___/_| |_|\___||___/
  __/ |                                   
 |___/                                    
                                          
```
A graphic converter for Super Nintendo development  
Developed by ALekmaul and distributed under the terms of the [MIT license](./LICENSE).

## Usage
```
gfx4snes [options] png/bmp filename ...  
```
where filename is a 256 color PNG or BMP file  

## Options
### Tiles options 
- `-b` Add blank tile management (for multiple bgs)  
- `-s (8|16|32|64)` Size of image blocks in pixels [8]  
- `-k` Output in packed pixel format
- `-z` Output in lz77 compressed pixel format
  
### Map options
- `-c` Generate collision map
- `-f (0..2047)` Generate the whole picture with an offset for tile number {0..2047}
- `-m` Include map for output  
- `-R` No tile reduction (not advised)  
- `-M (1|5|6|7)` Convert the whole picture for mode 1, 5, 6 or 7 format [1]
  
### Palette options
- `-a` Rearrange palette, and preserve palette numbers in tilemap  
- `-d` Palette rounding  (to a maximum value of 63)
- `-e (0..15)` The palette entry to add to map tiles (0 to 15)  
- `-o (0..256)` The number of colors to output (0 to 256) to filename.pal  
- `-p` Include palette for output  
- `-u (4|16|128|256)` The number of colors to use [256]  
  
### File options
- `-i <filename>` the file to convert   
- `-t (bmp|png)` Convert a bmp or png file [png]  

### Misc options 
- `-q` Quiet mode  
- `-v` Display version information
  
## Example 
```
gfx4snes -o16 -s8 -c16 -e0 -fpng -p -m -b -i myimage.png
```
 This will convert a myimage png file to a map/pal/pic files with 16 colors,palette entry #0,  8x8 tiles, a blank tile, a map, no border, 16 colors output.  
 
## future work
[.] things I think to add ;)

## acknowledgments
gfx4snes uses the following libraries:
* [AsciiArt](https://patorjk.com/software/taag/#p=display&f=Doom&t=gfx4snes%0A) gfx4snes logo with Doom font
* [LodePNG](http://lodev.org/lodepng) by Lode Vandevenne
* [cmdparser](https://github.com/XUJINKAI/cmdparser/tree/master) by XUJINKAI
