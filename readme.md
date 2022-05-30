# PVSneslib V3.5 #

[PVSneslib V3.5.0 (29, June, 2022) ](http://www.portabledev.com/jeux/supernes/pvsneslib/)

**PVSnesLib** (**P**rogrammer **V**aluable **Snes** **Lib**rary) is an open and free library to develop programs for the Nintendo SNES using the C programming language.

The first release was done in April 2012, for the 20th anniversary of the SNES. It is based on Ulrich Hecht works (<a href="http://code.google.com/p/snes-sdk/">)SNES Sdk.

It contains a snes-sdk compiler / linker and a library (sources included) which offer facilities to use backgrounds / sprites / pads / music & sound on the Nintendo SNES system. 
It also contains examples which demonstrate how to use the functions in the library.  

# Installation & documentation #

To install the library, please download the latest [release](https://github.com/alekmaul/pvsneslib/releases/latest) and follow instructions on the [Wiki pages](https://github.com/alekmaul/pvsneslib/wiki).

You can also see the [documentation](http://www.portabledev.com/pvsneslib/doc) generated from sources.

# Visual Studio #

You can find a Visual Studio template in the [vscode-template](https://github.com/alekmaul/pvsneslib/tree/master/vscode-template) folder to help PVSnesLib integration with Visual Studio.

# Contribution #

To discuss about the library, your project or to request help, join us on:
[Discord](https://discord.gg/DzEFnhB)

# Dependencies #

PVSnesLib works on Windows and Linux systems.
Due to technical constraints, it is available for now on 32 bits system only.

To use it, you will need at least :

- a unix-like environment like msys if you work on Windows
- python 3 installed and available in your path environment variable (accessible with the **python** command)

If you want to compile the whole project, please see [this wiki page](https://github.com/alekmaul/pvsneslib/wiki/Compiling-from-sources) to get all required dependencies.

# Support PVSneslib #

PVSneslib is free but you can donate to support PVSneslib development:<br>
[![Paypal](https://www.paypalobjects.com/fr_FR/FR/i/btn/x-click-but04.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=Y5USKF23DQVLC)


# Contribution #

[GitHub project](https://github.com/alekmaul/pvsneslib)

PVSneslib and affiliated tools are distributed under the MIT license (see pvsneslib_license.txt file).

# Special Thanks #

-RetroAntho (https://github.com/RetroAntho) for his great help to move wla-dx to last version  
   and also for all compilation optimizations in Makefile, snes_rules  
   and, at least, for keeping me motivated to update PVSnesLib ;)  
-Ulrich Hecht for SNES C SDK: http://code.google.com/p/snes-sdk/  
-Byuu for bass - assembler with SPC700 support: http://byuu.org/programming/  
-Mic_ for 816-tcc, sixpack, optimore, constify: http://jiggawatt.org/badc0de/sixpack/ and   
    http://jiggawatt.org/optimore-816r2.zip  
-Neviksti for pcx2snes: <no URL>  
  Shiru for snesbmp idea & sound tools: http://shiru.untergrund.net/  
-Mukunda for smconv tool: http://snes.mukunda.com/  
-RedBug for constify tcc bug fix and tips for Linux and Docker  
-mills32 (https://github.com/mills32) for his mode7 3D example  
-n_Arno for his help on Linux version (https://github.com/nArnoSNES/)  
  
Thanks !
