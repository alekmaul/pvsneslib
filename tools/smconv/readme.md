```
 ___ _ __ ___   ___ ___  _ ____   __
/ __| '_ ` _ \ / __/ _ \| '_ \ \ / /
\__ \ | | | | | (_| (_) | | | \ V / 
|___/_| |_| |_|\___\___/|_| |_|\_/  
                                
```
A mod file converter for Super Nintendo development with SNESMOD audio player  
Adapt by Alekmaul and distributed under the terms of the [MIT license](./LICENSE).

## Usage
```
smconv [options] [input]...
```
where input is an Impulse Tracker (.it) file  

Two modes:
- **soundbank** (`-s`): converts one or more .it files to a soundbank for a PVSnesLib project. It creates `output.bnk` (binary data), `output.asm` (includes the .bnk in the ROM banks) and `output.h` (module definitions for the C code).
- **SPC** (default): converts one .it file to a `.spc` file, to listen to it in a SPC player.

## Options
### Soundbank options
- `-s` Soundbank creation mode (can specify multiple files). Default is SPC creation mode (only one file)
- `-b <bank>` Bank number of the soundbank [5]

### File options
- `-o <file>` Output file (SPC mode) or output file base (soundbank mode, required). In SPC mode, the default is the input file with a .spc extension

### Memory options
- `-i` Use HiROM mapping mode for the soundbank (64 KB banks instead of 32 KB)
- `-f` Check the size of the IT files with the 1st IT file (useful for effects)

### Misc options
- `-V` Verbose mode, shows how much SPC RAM the modules will use
- `-h` Display help information
- `-v` Display version information

Long forms are also accepted: `--soundbank`, `--bank`, `--output`, `--hirom`, `--effectsize`, `--verbose`, `--help`, `--version`. Options must be placed before the IT files.

## Example
```
smconv -s -o soundbank -V -b 5 music1.it music2.it
```
This will convert music1.it and music2.it to soundbank.bnk, soundbank.asm and soundbank.h, in bank 5, and show the memory report of each module.  
soundbank.h then contains `MOD_MUSIC1` and `MOD_MUSIC2` (module numbers for the C code), and their sizes (`MOD_MUSIC1_SIZE`...).

In a PVSnesLib project, set `SMCONVFLAGS` in the Makefile, `snes_rules` calls smconv on the .it files of the project:
```
SMCONVFLAGS := -s -o $(SOUNDBANK) -V -b 5
```

And for a SPC file:
```
smconv music.it
```

## IT file constraints
- 8 channels maximum
- 64 patterns, 64 instruments and 64 samples maximum, 200 orders maximum
- Samples must not be compressed (8 or 16 bits), otherwise smconv stops with a fatal error
- The module (patterns, samples, instruments and echo) must fit in the SPC RAM: use `-V` to see the free bytes

## Echo settings
The echo is set in the message of the IT file: write `[[SNESMOD]]` on a line, then one command per line:
- `edl <0..15>` Echo delay
- `efb <-128..127>` Echo feedback
- `evol <left> [right]` Echo volume (-128..127)
- `efir <c0> ... <c7>` Echo FIR filter coefficients (-128..127)
- `eon <channel> ...` Channels (1..8) with echo enabled

```
[[SNESMOD]]
edl 4
efb 64
evol 32
efir 127 0 0 0 0 0 0 0
eon 1 2 3
```

## acknowledgments
smconv uses the following libraries:
* [AsciiArt](https://patorjk.com/software/taag/#p=display&f=Doom&t=smconv%0A) smconv logo with Doom font  
* [SNESMOD](https://github.com/mukunda-/snesmod) Based on SNESMOD (C) 2009 Mukunda Johnson 
