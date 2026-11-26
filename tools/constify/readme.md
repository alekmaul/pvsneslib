```
                          _    _   __
                         | |  (_) / _|
  ___   ___   _ __   ___ | |_  _ | |_  _   _
 / __| / _ \ | '_ \ / __|| __|| ||  _|| | | |
| (__ | (_) || | | |\__ \| |_ | || |  | |_| |
 \___| \___/ |_| |_||___/ \__||_||_|   \__, |
                                        __/ |
                                       |___/
```
A tool that moves C `const` variables from RAM to ROM for Super Nintendo development  
Developed by Alekmaul and distributed under the terms of the [MIT license](./LICENSE).

## Why
816-tcc places some initialized `const` variables in the `.data` section, so they are copied to RAM at startup and use WRAM for nothing.
tcc already places one-dimension `const` arrays in `.rodata`; the others (multi-dimension arrays, arrays of structures...) go through constify.

constify reads the C source to find the `const` variables declared at global level, then moves their data from the RAM sections of the assembly file to the `.rodata` section (ROM).

It is called automatically by `snes_rules`, after 816-tcc and 816-opt, before wla-65816:
```
816-tcc -> 816-opt -> constify -> wla-65816
```

## Usage
```
constify [options] cfile asmfile output
```
where cfile is the C source file, asmfile the assembly file generated from it, and output the assembly file to create

## Options
### Section options
- `-s <name>` Use the `.<name>` section (SUPERFREE) instead of the default `.rodata` section

### Misc options
- `-h` Display help information
- `-q` Quiet mode
- `-v` Display version information

Long forms are also accepted: `--section`, `--quiet`, `--version`, `--help`. Options must be placed before the file names.

## Example
```
constify main.c main.asp main.asm
```
This will read main.c, move the data of its global `const` variables from the RAM sections of main.asp to `.rodata`, and write the result to main.asm.

All these declarations are recognized:
```c
const u16 map[][2] = {{1, 2}, {3, 4}};
const u16 map[][2]={
    {1, 2}, {3, 4}
};
const u16 map [2][2] =
{
    {1, 2}, {3, 4}
};
static const u8 table[4][4] = { ... };
```

## Notes
- Only the variables declared at global level are moved, local `const` variables are not.
- Comments (`//` and `/* */` on several lines) and the contents of strings and characters are ignored: a brace or a `const` word inside them does not change the result.
- A `static` variable is searched with the `tccs_` prefix used by 816-tcc.

## acknowledgments
constify uses the following:
* [AsciiArt](https://patorjk.com/software/taag/#p=display&f=Doom&t=constify%0A) constify logo with Doom font
* [cmdparser](https://github.com/XUJINKAI/cmdparser/tree/master) by XUJINKAI
* Based on constify by Mic
