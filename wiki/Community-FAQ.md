The remaining entries here on the unofficial FAQ are maintained by the community.

# Frequently asked questions (FAQ)

**[Miscellaneous](#miscellaneous)**

- [I want to contribute to this project](#i-want-to-contribute-to-this-project)
- [I want to ask something](#i-want-to-ask-something)
- [I found a bug. What can i do ?](#i-found-a-bug-what-can-i-do-)
- [Can we create Hirom games with the lib ?](#can-we-create-hirom-games-with-the-lib-)
- [Why is it called PVSnesLib ?](#why-is-it-called-pvsneslib-)
- [I would like to share my project with PVSneslib community](#i-would-like-to-share-my-project-with-pvsneslib-community)
- [Is it possible to use Docker ?](#is-it-possible-to-use-docker-)
- [How to update WLA submodule to the latest commit ?](#how-to-update-wla-submodule-to-the-latest-commit-)

**[About the lib](#about-the-lib)**

- [Is it possible to rotate a picture ?](#is-it-possible-to-rotate-a-picture-)
- [What is the function to clear text ?](#what-is-the-function-to-clear-text-)
- [How can i display special characters in text ?](#how-can-i-display-special-characters-in-text-)
- [How to create random number ?](#how-to-create-random-number-)
- [What is the goal of each tool ?](#what-is-the-goal-of-each-tool-)
- [What are .it files ?](#what-are-it-files-)
- [How to convert .mid to .it ?](#how-to-convert-mid-to-it-)

**[Common errors](#common-errors)**

- [Frequently asked questions (FAQ)](#frequently-asked-questions-faq)
  - [Miscellaneous](#miscellaneous)
    - [I want to contribute to this project](#i-want-to-contribute-to-this-project)
    - [I want to ask something](#i-want-to-ask-something)
    - [I found a bug. What can i do ?](#i-found-a-bug-what-can-i-do-)
    - [Can we create HiROM games with the lib ?](#can-we-create-hirom-games-with-the-lib-)
    - [Why is it called PVSnesLib ?](#why-is-it-called-pvsneslib-)
    - [I would like to share my project with PVSneslib community](#i-would-like-to-share-my-project-with-pvsneslib-community)
    - [Is it possible to use Docker ?](#is-it-possible-to-use-docker-)
    - [How to update WLA submodule to the latest commit ?](#how-to-update-wla-submodule-to-the-latest-commit-)
  - [About the lib](#about-the-lib)
    - [Is it possible to rotate a picture ?](#is-it-possible-to-rotate-a-picture-)
    - [What is the function to clear text ?](#what-is-the-function-to-clear-text-)
    - [How can I display special characters in text ?](#how-can-i-display-special-characters-in-text-)
    - [How to create random number ?](#how-to-create-random-number-)
    - [What is the goal of each tool ?](#what-is-the-goal-of-each-tool-)
    - [What are .it files ?](#what-are-it-files-)
    - [How to convert .mid to .it ?](#how-to-convert-mid-to-it-)
    - [Why i get "error 5" with smconv when compiling a .it file saved with OpenMPT ?](#how-to-convert-mid-to-it-)
  - [Common errors](#common-errors)
    - [What is CHECK\_HEADERS error ?](#what-is-check_headers-error-)
    - [Colors of my loaded picture are wrong](#colors-of-my-loaded-picture-are-wrong)
    - [I get FIX\_LABELS error when I build my project](#i-get-fix_labels-error-when-i-build-my-project)
    - [Why HDMA channel 0 doesn't work ?](#why-hdma-channel-0-doesnt-work-)
    - [Soundbank files are missing in music samples](#soundbank-files-are-missing-in-music-samples)
    - [My music has some glitches during playing](#my-music-has-some-glitches-during-playing)
    - [Programmer's Notepad adds other text when I compile](#programmers-notepad-adds-other-text-when-i-compile)
    - [Font system doesn't work with some background mode](#font-system-doesnt-work-with-some-background-mode)
    - [How to build tcc 816 provided with PVSnesLib sources ?](#how-to-build-tcc-816-provided-with-pvsneslib-sources-)
    - [I get the error "echo: command not found"](#i-get-the-error-echo-command-not-found)
    - [On Linux i get : "fatal error: bits/libc-header-start.h"](#on-linux-i-get--fatal-error-bitslibc-header-starth)
    - [On Mac OS i get : "sed: 1: "hello_world.sym": extra characters at the end of h command"](#on-Mac-OS-i-get--sed-1--hello-world-sym-extra-characters-at-the-end-of-h-command)
    - [Using malloc with PVSneslib](#using-malloc-with-pvsneslib)
  - [Maps](#maps)
    - [How to create maps with 16x16 tiles ?](#how-to-create-maps-with-16x16-tiles-)
    - [Backgrounds begin at x = 0 and y = 1](#backgrounds-begin-at-x--0-and-y--1)

**[Maps](#maps)**

- [How to create maps with 16x16 tiles ?](#how-to-create-maps-with-16x16-tiles-)
- [Backgrounds begin at x = 0 and y = 1](#backgrounds-begin-at-x--0-and-y--1)
---

## Miscellaneous

### I want to contribute to this project

PVSneslib's main developer and community will be happy! You can create a fork of this project and submit your code by pull request.

### I want to ask something

There is no more official forum for PVSneslib. The best thing to do is to ask on [Snesdev](https://forums.nesdev.com/viewforum.php?f=12) forum or on the PVSnesLib Discord channel.

### I found a bug. What can i do ?

Please create a minimal code to reproduce it then create an [issue](https://github.com/alekmaul/pvsneslib/issues) on Github explaining the bug.

### Can we create HiROM games with the lib ?

No, PVSnesLib currently works with LoROM games only but it may be implemented in the future !

### Why is it called PVSnesLib ?

The library could not be named LibSnes because another project with the same name already existed.
The "PV" letters comes from **P**ortablde**V**, the name of Alekmaul's [website](https://www.portabledev.com).

### I would like to share my project with PVSneslib community

That's great! The community will probably be happy to study it. However, we cannot include your code with the others examples in PVSnesLib because we need to keep it up to date for each improvement of PVSnesLib.

In this case, we recommend you to upload it on your GitHub page and create a wiki page here to present it.

### Is it possible to use Docker ?

Yes, some people worked on it and created a docker image that you can find [here](https://github.com/Crazy-Piri/pvsneslib-docker)

### How to update WLA submodule to the latest commit ?

You just need to execute `git submodule update --remote --merge` but please keep in mind that upgrading WLA may impact PVSnesLib. Be sure that you fully test it.

---

## About the lib

### Is it possible to rotate a picture ?

Only background in mode7 can be rotate.

### What is the function to clear text ?

There is no function to clear text directly. However, you can use spaces to replace text like this:
```
consoleDrawText(1,2,"some text on the screen");
// will be cleared with :
consoleDrawText(1,2,"                       ");
```

### How can I display special characters in text ?

PVSnesLib only lets you manage characters from ASCII code 32 to 127. If you need other characters, you have to implement it yourself (and share it with the community !).
For more information about text in PVSnesLib, you can consult [this page](https://github.com/alekmaul/pvsneslib/wiki/Input-and-Output)

### How to create random number ?

If you want to create random numbers, you can use the rand() function available in PVSnesLib. You do not need to initialise anything as it is managed by the library.
The rand() function will work only if you assign its result to a `u16` variable type.
To limit numbers between values like 0 and 50, you have to do `(rand() % 50)`. But keep in mind that modulo, multiplication and division operations are very very slow on the 65C816 processor used by SNES. So try to avoid using it when possible.

### What is the goal of each tool ?

- bin2txt : convert binary file to text file to include it directly in your project. It is used to include the file generated (with TASM tool) for spc700 CPU (audio) directly in PVSneslib source code.

- gfx4snes : transform all kinds of graphics to resources in Super Nintendo format. This is a central tool used by PVSnesLib to load sprites, maps or palettes.
Remember that we have a lot of constraints on SNES and each parameter of the tool is important ! Parameters are divided in 3 groups : Graphics, Map, Palette.

- smconv : to convert music or sounds to bank of sounds readable on Super Nintendo and its sound processor SPC700

- snesbrr : to convert .wav files to .brr. If you want more information on this format, you can read [this page](https://wiki.superfamicom.org/bit-rate-reduction-(brr))

- snestools : this tool is not used anymore by PVSnesLib (it was used to patch the ROM after its build) but it is still provided with it if you want to see the header of your ROM.

- 816-tcc and 816-opt : this is the tiny C compiler for 8/16 bits architecture, it translate your C code to ASM for 65c816. Due to some limitations and performances issues, the 816-opt is used after to optimize the generated code.

- constify : it looks through your .c source files to detect const variables, it moves variables from RAM to ROM in .asm files to improve performance.

- tasm : it is the assembler tool for SPC700 (the processor used to manage the sounds and music on SNES)

- wla-65816 and wlalink : these tools are the assembler to convert your .asm files to .obj files (code readable by 65c816). Wlalink is the linker that "merges" all files.

### What are .it files ?

.it files are impulse tracker files, a music format similar to the SNES one (.spc files). It is used because it manages many channels like the SNES and a tool is provided with PVSnesLib (smconv) to convert it to soundbanks.
[Schism Tracker](http://schismtracker.org/) can be used to open this files, it is a reimplementation of Impulse Tracker

### How to convert .mid to .it ?

[OpenMPT (Open ModPlug Tracker)](https://openmpt.org/features) is able to do it !

### Why i get "error 5" with smconv when compiling a .it file saved with OpenMPT ?

OpenMPT enables IT sample compression by default now, which is not supported.
To turn it off, go to OpenMPT > View > Setup :

![Setup](https://user-images.githubusercontent.com/48180545/251185140-ce2998d8-3687-4837-8fd6-dcce030a1e35.png)

Click on the Advanced tab and search for ITCompression.
You will find two values: ITCompressionMono and ITCompressionStereo:

![ITCompression](https://user-images.githubusercontent.com/48180545/251185152-cf38c2cb-eb29-4ba4-a764-ea7fe5338963.png)

By default, the former is set to 7 and the latter is set to 4.
Set them both to 0.

---

## Common errors

### What is CHECK_HEADERS error ?

Your game needs to be built with the same parameters that we have in PVSnesLib header. This is because .obj files from the library are embedded with .obj files of you project which contains its own header too. It is an improvement to do with PVSnesLib!

### Colors of my loaded picture are wrong

It is an issue with your color palette. We do not recommend using MS Paint which mixes color strangely. You can use [Graphics Gale](https://graphicsgale.com/us/) software if you want to edit your colors easily.
This is a sample when editing a sprite with MS Paint from DynamicSprite sample:

![Palette issue](https://user-images.githubusercontent.com/48180545/56434580-727f1d00-62d5-11e9-94a1-1feb1e9d591b.png)

but your initial sprite looks like that :

![initial sprite](https://user-images.githubusercontent.com/48180545/56434581-727f1d00-62d5-11e9-8762-095da6a992b6.png)

As you saw in the [sprite tutorial](https://github.com/alekmaul/pvsneslib/wiki/Sprites), your transparency color must be the first color in your palette. If this is not the case, open Graphics gale and remove unused colors from your palette by using this menu :

![Graphics gale remove unused colors](https://user-images.githubusercontent.com/48180545/56434583-7317b380-62d5-11e9-9670-497e43cb22b2.png)

In this case, the transparency color is Magenta (RGB : 255 0 255). In the same menu, use "save palette" and open it in a text editor (this .pal file is not the same that .pal file generated with gfx4snes tool).
Find the line which contains your transparency color and move it to the first line of colors. Be careful, the first lines are essential information for the tool:

![Move transparency color](https://user-images.githubusercontent.com/48180545/56434582-7317b380-62d5-11e9-80cb-c934547af546.png)

Now, import your new palette file in Graphics gale. In the "Load Palette" window, click "All" to replace colors and save your new file with palette correctly sorted.

Do not forget to clean your project (with make clean command) to try your new sprite.

### I get FIX_LABELS error when I build my project

It is recommended to split your code in multiple files but do not declare global variables in .h files.
Declare the variable within your .c source file:

**commonFile.c**

```
#include "commonFile.h"

bool myCommonVariable = false;

...
```

then use extern in other places (either .c files, or an .h file that .c files include) :

**main.c**

```
#include "commonFile.h"

extern bool myCommonVariable;

...
```

and do not forget to protect your header files from multiple inclusion:

**commonFile.h**

```
#ifndef COMMON_FILE_H
#define COMMON_FILE_H

...

#endif
```

### Why HDMA channel 0 doesn't work ?

HDMA channel 0 is used internally for all DMA functions.

### Soundbank files are missing in music samples

Soundbank files (sounbank.asm and soundbank.h) are automatically generated in music sample when your project is compiled. To manage the project, the files are added in project tree but not in the folder like other generated files (.pal, .map, ...).

### My music has some glitches during playing

This is because you didn't respect snesmod requirements! It should be very useful to implement a tool to manage it automatically if possible... Take a look of pvsneslib_snesmod.txt in pvsneslib and follow this advice from **KungFuFurby** :

> Sample loop points must be divisible by 16. Loop points not
> divisible by 16 may not convert correctly. If you don't use loop
> points divisible by 16, at least make sure the loop length is
> divisible by 16.
> I use Schism Tracker to perform the job of loop point analysis and
> loop point adjustments since I can just simply type in the numbers.

> I use a calculator to take care of loop point problems simply related
> to the sample being at the wrong sample rate to have loop point
> lengths divisible by 16 (the length of the looping portion of the
> sample should at the very least be divisible by 16). I usually perform
> this on samples with shorter loop point lengths. I don't think it
> works so well on ones with longer loop point lengths, mainly because
> by then you're probably not dealing with simple waveforms as loops.

> Using the Bass & Lead sample as an example...

> Loop point is currently defined as...
> Start: 3213
> End: 3382

> That's a loop length of 169.

> I like using powers of 2 for my loop points so that if I have to
> decrease the quality of the sample, then I can do so as painlessly as
> possible to the sample (unless I find the degraded quality to be a bad
> idea), so that means 128 is the value I use here.

> Divide 169 by 128 gets you an unusual decimal number... copy that number.

> Now get the full length of the sample (that's 3383 for this sample)
> and divide by that decimal number you acquired earlier (169/128).
> You'll most likely get another unusual decimal number. Round that off
> and there's your new length that you will resize the sample to.

> I use Alt-E in Schism Tracker to perform the resize sample command.
> The program will ask for the new size of the sample.

> Now you should have a loop length that is divisible by 16. You can
> perfect the loop point by adjusting them so that the loop point
> themselves are divisible by 16.

> Only one sample can be defined per instrument...
> You'd have to duplicate the instruments and then enter the sample ID
> for all of those notes... and then you have to redefine the instrument
> IDs depending on the pitch from the old note table. Yeah...

> I thought in one case, I saw the pitch go too high (it went over
> 128khz). That's because I noticed a hiccup in the notation.
> For the one song that has this problem, I usually resize the sample
> and make it half of its length... however, I may have to make
> additional adjustments depending on how the loop points are holding up
> (length may or may not be involved, although usually I'm checking the
> loop points themselves to make sure that they are divisible by 32 or
> some higher power of 16... this indicates how many times you can cut
> the sample in half).

> Note Cut is the only New Note Action supported for SNESMod.
> One of these songs is the most visibly affected by this problem, and
> that's because SNESMod doesn't virtually allocate channels. You have
> to modify the patterns so that the note off commands go where the note
> would originally play, and the new note is put on another channel.

### Programmer's Notepad adds other text when I compile

You will notice this when your project gives a strange error during compilation. This is because some pieces of header code are inserted into your file. In this case, save your project and close Programmer's Notepad. Open the file that doesn't compile **with another tool** (such as a simple Notepad) to remove bad lines and save it. Then you can continue with Programmer's Notepad software.

### Font system doesn't work with some background mode

The output system is only available for **BG_MODE1**. If you need it in other modes, you need to develop it.


### How to build tcc 816 provided with PVSnesLib sources ?

Go to the tcc-65816 directory, then execute **./configure** command to create the config.mak file.
If you are on windows and get an error like "'.' is not recognized as an internal or external command", you probably need to execute the **sh** command before.
After this command, you can build tcc by doing : **make 816-tcc.exe**


### I get the error "echo: command not found"

You probably have an issue with the format of your PVSNESLIB_HOME environment variable.
The value must be in unix style (**/c/snesdev** instead of **c:\\snesdev**) to avoid this issue. The variable can be created with this command line : `setx PVSNESLIB_HOME "/c/snesdev"`

### On Linux i get : "fatal error: bits/libc-header-start.h"

When building some tools on Linux like **snestools**, if you get the error _/usr/include/stdlib.h:25:10: fatal error: bits/libc-header-start.h: no such file or directory_, it is related to the -m32 CFLAG provided in the makefile, you probably forgot to install some libraries from gcc. For example on Ubuntu, you just have to install **gcc-multilib** by executing `sudo apt-get install gcc-multilib`

### On Mac OS i get : "sed: 1: "hello_world.sym": extra characters at the end of h command"

It is probably related to the version of `sed` command you use which is not the correct one.
Please read the [installation page](https://github.com/alekmaul/pvsneslib/wiki/Installation) which explain how to solve it.

### Using malloc with PVSneslib

I want to use malloc in my program, so I am trying something like this: u16 *myHudBuffer = (u16*)malloc(160*sizeof(u16)); but it is not working. malloc requires stdlib.h which I think does not work with PVSnesLib.

Instead, you need to declare an array with a fixed size, u16 myHudBuffer[160] in this case.


## Maps

### How to create maps with 16x16 tiles ?

It is not possible to load maps with 16x16 tiles with the existing version of gfx4snes. You need to use 8x8 tiles or update the tool !
The "-s" parameter (for "graphic Size") in gfx4snes is only applied to sprites.


### Backgrounds begin at x = 0 and y = 1

It is not a bug and it is linked to a technical constraint of the SNES:

> Note that many games will set their vertical scroll values to -1 rather than 0. This is because the SNES loads OBJ data for each scanline during the previous scanline. The very first line, though, wouldn't have any OBJ data loaded! So the SNES doesn't actually output scanline 0, although it does everything to render it.

If you want more information on it, you can consult [this page.](https://wiki.superfamicom.org/backgrounds#toc-3)
