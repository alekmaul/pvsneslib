# A library to code in C or ASM for the Super Nintendo Entertainment System #

[PVSneslib V4.0.1 (November,20,2022) ](http://www.portabledev.com/jeux/supernes/pvsneslib/)

**PVSnesLib** (**P**rogrammer **V**aluable **Snes** **Lib**rary) is an open and free library to develop programs for the Super Nintendo using the C programming language.

Created initially for the 20th birthday of the Super Nintendo (2012) and based on Ulrich Hecht [SDK](http://code.google.com/p/snes-sdk/), it has been evolved a lot and is now mainly coded in assembler for performances reasons. It is also directly usable in asm for those who do not want to use C language but without rewriting everything from scratch.

It contains a snes-sdk compiler / linker and a library (sources included) which offer facilities to use backgrounds / sprites / pads / music & sound on the Super Nintendo system. 
It also contains examples which demonstrate how to use the functions in the library.  

# Installation #
<a href="https://github.com/alekmaul/pvsneslib/releases/download/4.0.0/Windows-x32-400-release.zip"><img src="https://img.shields.io/badge/release-windows32-green?style=flat&logo=github" alt="PVSNESLIB Windows32 Release"></a> 
<a href="https://github.com/alekmaul/pvsneslib/releases/download/4.0.0/Linux-x32-400-release.zip"><img src="https://img.shields.io/badge/release-linux-orange?style=flat&logo=github" alt="PVSNESLIB Linux32 Release"></a>

To install the library, please download the latest [release](https://github.com/alekmaul/pvsneslib/releases/latest) or with the link below and follow instructions on the [Wiki pages](https://github.com/alekmaul/pvsneslib/wiki).

You can also see the [documentation](https://alekmaul.github.io/pvsneslib/) generated from sources.

# Visual Studio Code #

You can find a Visual Studio Code template in the [vscode-template](https://github.com/alekmaul/pvsneslib/tree/master/vscode-template) folder to help PVSnesLib integration with it.

# Build status #

[![PVSNESLIB Build and Package](https://github.com/alekmaul/pvsneslib/actions/workflows/pvsneslib_build_package.yml/badge.svg?branch=develop)](https://github.com/alekmaul/pvsneslib/actions/workflows/pvsneslib_build_package.yml)

You can find with the link below the last stable builds of PVSnesLib. They are development builds, you can have bugs not yet resolved with them.

# Dependencies #

PVSnesLib works on Windows and Linux systems.
Due to technical constraints, it is available for now on 32 bits system only.

To use it, you will need at least :

- a unix-like environment like msys if you work on Windows
- python 3 installed and available in your path environment variable (accessible with the **python** command)

If you want to compile the whole project, please see [this wiki page](https://github.com/alekmaul/pvsneslib/wiki/Compiling-from-sources) to get all required dependencies.

# Contribution #

[GitHub project](https://github.com/alekmaul/pvsneslib)

To discuss about the library, your project or to request help, join us on [Discord](https://discord.gg/DzEFnhB).

PVSneslib and affiliated tools are distributed under the MIT license (see pvsneslib_license.txt file).

# Support PVSneslib #

PVSneslib is free but you can donate to support its development:<br>
[![Paypal](https://www.paypalobjects.com/fr_FR/FR/i/btn/x-click-but04.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=Y5USKF23DQVLC)

# Powered by PVSneslib #

These projects are based on PVSnesLib _(non-exhaustive list)_:

![alt text](pvsneslib/docs/images/yoyoshuriken.gif)
 
Yo-Yo Shuriken by [Dr. Ludos](https://drludos.itch.io/yo-yo-shuriken)

![alt text](pvsneslib/docs/images/eyra.gif)
 
Eyra by [Second Dimension](https://www.second-dimension.com/store/eyra-the-crow-maiden-snes)

![alt text](pvsneslib/docs/images/sydneycod.gif)
 
Sydney Hunter by [CollectorVision](https://collectorvision.com/store/shop/snes/snes-sydney-hunter-the-caverns-of-death-pal/)


# Special thanks #

-RetroAntho (https://github.com/RetroAntho) for his great help to move wla-dx to last version and also for all compilation optimizations in Makefile, snes_rules and, at least, for keeping me motivated to update PVSnesLib ;)  
-Ulrich Hecht for SNES C SDK: http://code.google.com/p/snes-sdk/  
-Byuu for bass - assembler with SPC700 support: http://byuu.org/programming/  
-Mic_ for 816-tcc, sixpack, optimore, constify: http://jiggawatt.org/badc0de/sixpack/ and http://jiggawatt.org/optimore-816r2.zip  
-Neviksti for pcx2snes  
-Shiru for snesbmp idea & sound tools: http://shiru.untergrund.net/  
-Mukunda for smconv tool: http://snes.mukunda.com/  
-RedBug for constify tcc bug fix and tips for Linux and Docker  
-mills32 (https://github.com/mills32) for his mode7 3D example  
-n_Arno for his help on Linux version (https://github.com/nArnoSNES/)  
-kobenairb for python & docker optimizations
  
Thanks !
