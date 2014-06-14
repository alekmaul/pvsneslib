 DevkitSnes 0.40
 
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
  Byuu for bass - assembler with SPC700 support : http://byuu.org/programming/
  Mic_ for sixpack, optimore, constify : http://jiggawatt.org/badc0de/sixpack/ and 
    http://jiggawatt.org/optimore-816r2.zip
  Neviksti for pcx2snes : <no URL>
  Shiru for snesbmp idea & sound tools : http://shiru.untergrund.net/
  Ulrich Hecht for SNES C SDK : http://code.google.com/p/snes-sdk/
  Mukunda for smconv tool : http://snes.mukunda.com/
               
CHANGE LOG
----------------------
VERSION 1.30 (xx, yyyy, 2014)
- Added : sram and country management in snestool

VERSION 1.20 (xx, yyyy, 2014)
- Added : sed path to sym file to be ok with no$sns symbols reading

VERSION 1.10 (xx, yyyy, 2012)
- Fixed : mode0 palette management in gfx2snes
- Added : option for map priority in gfx2snes
- Added : bin2txt tool
- Added : tasm assembler for spc700 compilation
- Fixed : no more blank tile in map making for gfx2snes

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