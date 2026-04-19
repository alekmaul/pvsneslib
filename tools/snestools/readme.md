
```
                     _              _ 
                    | |            | |
 ___ _ __   ___  ___| |_ ___   ___ | |
/ __| '_ \ / _ \/ __| __/ _ \ / _ \| |
\__ \ | | |  __/\__ \ || (_) | (_) | |
|___/_| |_|\___||___/\__\___/ \___/|_|
```
A header file information and modification of rom files for Super Nintendo development  
Developed by Alekmaul and distributed under the terms of the [MIT license](./LICENSE).

## Usage
```
snestools.exe [options] sfc/smc filename  
```
where filename is a SNES rom file  

## Options
### Header options  
- `-hi!` don't show current filename header information  
- `-hf` fix hearder CRC.  
- `-ht[text]` change game title wi `text`  
- `-hc[country]` change country (00 for NTSC,01 for PAL)  
- `-hr[romsize]` change romsize (08 for 256K, 0B for 2MB, ROM size = (1 << value) KiB)  
- `-hS!` No SRAM (default is on in pvsneslib)  

### Misc options  
- `-h` display help  
- `-q` quiet mode  
- `-v` display version information  
  
## Example 
```
snestools hello_world.sfc
```
 This will display all the information from the header of `hello_world.sfc` file.  

```
 $ ./snestools.exe hello_world.sfc
snestools: File [hello_world.sfc] has no header

Generic Information
-------------------
Game title    : LIBSNES HELLO WORLD
ROM ID        : None
Company       : Invalid [D6B5F886]
License       : Invalid [00]
Country:      : USA & Canada  [01]
Version       : 1.0
Promo Version : None
Header Type   : Old Header (not extended)
Checksum      : 5B2A
Comp Checksum : A4D5

Hardware Information
--------------------
ROM Size      : 256K [256K file]
ROM Speed     : Slow (2.68MHz)
Bank size:    : LoROM
Chipset       : ROM
Video Type    : NTSC (60Hz)
SRAM Size     : 0K [00]
Exp.RAM Size  : 0K
Battery       : No
NMI Address   : 81ADh (native) 841Ah (emul.)
Start Address : 838Dh (native)
snestools: Done!
```
 
## future work
[.] things I think to add ;)

## acknowledgments
snestool uses the following libraries:
* [AsciiArt](https://patorjk.com/software/taag/#p=display&f=Doom&t=snestool%0A) snestool logo with Doom font  
