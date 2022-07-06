 DevkitSnes 3.40
 
 Programming Compiler and Tools for Snes

INTRODUCTION
----------------------
DevkitSnes is a set of tools built to compile C sources files and
graphics files to produce homebrews for SNES.
Users are encouraged to download the source code of libsnes and 
submit bug fixes and patches as well as new features for inclusion 
in the DevkitSnes & PVSnesLib distribution.

SPECIAL THANKS
----------------------
  Byuu for bass - assembler with SPC700 support: http://byuu.org/programming/
  Mic_ for 816-tcc, sixpack, optimore, constify: http://jiggawatt.org/badc0de/sixpack/ and 
    http://jiggawatt.org/optimore-816r2.zip
  Neviksti for pcx2snes: <no URL>
  Shiru for snesbmp idea & sound tools: http://shiru.untergrund.net/
  Ulrich Hecht for SNES C SDK: http://code.google.com/p/snes-sdk/
  Mukunda for smconv tool: http://snes.mukunda.com/
  RetroAntho (https://github.com/RetroAntho) for his great help to move wla-dx to last version
    and also for all compilation optimizations in Makefile, snes_rules
    and, at least, for keeping me motivated to use the last version of wla-dx ;)
  RedBug for constify tcc bug fix and tips for Linux and Docker
  mills32 (https://github.com/mills32) for his mode7 3D example
  n_Arno for his help on Linux version (https://github.com/nArnoSNES/)
               
CHANGE LOG
----------------------
VERSION 3.50 (29, May, 2022)
- Add romsize update in snestools
- Fix country value update in snestools
- Add palette entry for map engine in gfx2snes
- Correct extra qualification on member error in smconv (thanks KungFuFurby)
- Add ARM compatibility (thanks RetroAntho)
- Improvements to have linux version (thanks RetroAntho)
- Clean up repository with useless files (thanks RetroAntho)
- Upgrade wla-dx to last version (thanks RetroAntho)
- Symplify build system (thanks RetroAntho)

VERSION 3.40 (19, June, 2021)
- Add map management in gfx2snes
- Remove dummy data section in 816-tcc
- No more sm_spc (SPC700 driver) compilation
  For reminding purpose, here is how to compile it :
    sm_spc.asm : ../snesmod/sm_spc.as7
        $(TA) -07 -b -l $< sm_spc.obj sm_spc.lst
        $(TXCONV) -ca sm_spc.obj
        @rm sm_spc.obj
        
VERSION 3.30 (28, May, 2021)
- Nothing new

VERSION 3.20 (24, May, 2021)
- Update wla-dx toolchain to last version (9.13a)
- Update constify to use uppercase characters for section keywords
- Update constify for more flexible options and less display on screen
- Remove bin2h, useless in devkitnes bin directory
- Update tcc to use uppercase characters for section keywords
- Update tcc to use different name for ram.data and .data sections in each C file
- Update tcc to append ram.data and .data sections to a global one
- Update bin2txt for more flexible options and less display on screen
- Update smconv for more flexible options and less display on screen
- Update snestools for more flexible options and less display on screen
- Fix gfx2snes 4bpp png error and update it for more flexible options and less display on screen

VERSION 3.10 (13, December, 2020)
- Remove PVSNESLIB variables in each makefile and add a system variable (PVSNESLIB_HOME)
- Fix 816-tcc rodata variable names for multiple files projects
- Change wla-dx for last version for wla-dx github repository
- Add source folder management in snes_rules (the name is src, 2 levels, see hello_world example)
- Remove stripcom.exe and optimore-816.exe, they are useless
- Update snes_rules with lots of information to clean Makefiles (thanks RetroAntho for that)
- Update constify to update static variable name (tccs)
- Fix 816-tcc temp filename on linux system (/ in the name)
- Add wla-spc700 for futur use

VERSION 3.00 (21, november, 2020)
- Change wla-dx & wlalink to the newset one (9.12a instead of 9.8)
- Fix some minor bug in 816-tcc
- Add mic_ optimizations in 816-tcc
- Fix snes_rule for last wla-dx compliance

VERSION 2.10 (24, may, 2020)
- Fix tilemap generation for image producing more than 4 palettes in 4 color mode
    (thanks 1r3n33)
VERSION 2.00 (13, may, 2017)
- Add png and bmp rle encoding in gfx2snes file format

VERSION 1.30 (xx, yyyy, 2014)
- Add sram and country management in snestool
- Add palette rounded option from Artemio Urbina in gfx2snes

VERSION 1.20 (xx, yyyy, 2014)
- Added sed path to sym file to be ok with no$sns symbols reading

VERSION 1.10 (xx, yyyy, 2012)
- Fix mode0 palette management in gfx2snes
- Add option for map priority in gfx2snes
- Add bin2txt tool
- Add tasm assembler for spc700 compilation
- Fix no more blank tile in map making for gfx2snes

VERSION 1.00 (06, June, 2012)
- Add tools directory for each executable which are not for c / asm source compilation
- Add constify tool in compilation , after python script
- Add smconv tool to convert it files to be compatible with it sampling
- Add snesbrr to convert wav file to brr format

VERSION 0.30 (23, April, 2012)
- Change all graphic tools for pvsneslib gfx2snes tool (based on pcx2snes by Neviksti)

VERSION 0.20 (14, April, 2012)
- Add bass v0.6

VERSION 0.10 (April, 2012)
- Initial release 

If you have questions, please email Alekmaul at alekmaul@portabledev.com.

EOF