
```
  __       _     ___                     
 / _|     | |   /   |                    
| |_ _ __ | |_ / /| |___ _ __   ___  ___ 
|  _| '_ \| __/ /_| / __| '_ \ / _ \/ __|
| | | | | | |_\___  \__ \ | | |  __/\__ \
|_| |_| |_|\__|   |_/___/_| |_|\___||___/
```
A variable font width converter for Super Nintendo development  
Developed by Alekmaul and distributed under the terms of the [MIT license](./LICENSE).

## Usage
```
fnt4snes [options] png/bmp filename ...  
```
where filename is a 256 color PNG file  

## Options
### Palette options
- `-o (4..16)` The number of colors to output (4..16) to filename.pal   [4]
- `-p` Include palette for output  
- `-u (4|16)` The number of colors to use [4]  
  
### File options
- `-i <filename>` the png file to convert   

### Misc options 
- `-q` Quiet mode  
- `-v` Display version information  
 
## acknowledgments
fnt4snes uses the following libraries:
* [AsciiArt](https://patorjk.com/software/taag/#p=display&f=Doom&t=gfnt4snes%0A) fnt4snes logo with Doom font
* [LodePNG](http://lodev.org/lodepng) by Lode Vandevenne
* [cmdparser](https://github.com/XUJINKAI/cmdparser/tree/master) by XUJINKAI
