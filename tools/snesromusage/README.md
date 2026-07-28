```
                                                                    
 ___ _ __   ___  ___ _ __ ___  _ __ ___  _   _ ___  __ _  __ _  ___ 
/ __| '_ \ / _ \/ __| '__/ _ \| '_ ` _ \| | | / __|/ _` |/ _` |/ _ \
\__ \ | | |  __/\__ \ | | (_) | | | | | | |_| \__ \ (_| | (_| |  __/
|___/_| |_|\___||___/_|  \___/|_| |_| |_|\__,_|___/\__,_|\__, |\___|
                                                         |___/      
```

A graphic converter for Super Nintendo development  
Developed by Alekmaul and distributed under the terms of the [MIT license](./LICENSE).

## Usage

```
snesromusage [options] -i file.symfull
```

where `file.symfull` is the symbol file wlalink writes out next to your `.sfc` ROM.

## Options
### Display options
- `-t` show the largest N largest sections (default is 15)
- `-s` dump every parsed section (bank/type/range/size)

### ROM options
- `-r` Force rom to lorom, hirom, exhirom

#### File options
- `-i <filename>` the file to convert   

### Misc options 
- `-q` Quiet mode  
- `-v` Display version information 

## Example 
```bash
snesromusage.exe -i hello_world.symfull
```
This parses `hello_world.symfull`, auto-detects the LoROM memory map from the bank/offset ranges actually used and reports usage per ROM/WRAM bank.  

```
snesromusage: (1.0.0) version 20260728
snesromusage: parse sym file [hello_world.symfull]...
snesromusage: bluid sections for 1615 symbols...
Memory map          : LoROM  (auto-detected)
Sections parsed     : 52

Bank           Type   Range            Size    Used  Used%    Free  Free%
-------------- ------ ------------- ------- ------- ------ ------- ------
ROM bank $00   ROM    0x8000-0xFFFF   32768   12274    37%   20494    63%  |##########..................|
ROM bank $01   ROM    0x8000-0xFFFF   32768    4218    13%   28550    87%  |###.........................|
WRAM bank $7E  WRAM   0x0000-0xFFFF   65536   10818    17%   54718    83%  |####........................|
WRAM bank $7F  WRAM   0x0000-0xFFFF   65536       0     0%   65536   100%  |............................|
-------------- ------ ------------- ------- ------- ------ ------- ------
TOTAL ROM      ROM    0x0000-0x0000   65536   16492    25%   49044    75%  |#######.....................|
TOTAL WRAM     WRAM   0x0000-0x0000  131072   10818     8%  120254    92%  |##..........................|

snesromusage: processed in 6ms
```

## acknowledgments

snesromusage was inspired by [bbbbbr/romusage](https://github.com/bbbbbr/romusage) for GBDK.  

snesromusage uses the following libraries:
* [AsciiArt](https://patorjk.com/software/taag/#p=display&f=Doom&t=gfx4snes%0A) gfx4snes logo with Doom font
* [cmdparser](https://github.com/XUJINKAI/cmdparser/tree/master) by XUJINKAI
