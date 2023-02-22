Feel free to help us by implementing any of this functionalities !

# High priority:

- Include the python script directly in tcc tool (or convert it in a new tool in c which will regroup all optimisations parts)
- upgrade to the latest version of tcc
- Review of constify tools :
	- '\_\_tcc_' should become 'tcc_'
	- manage const declared like : const myArray[5*6] = ...;
	- add a function to remove comments in C source file (// ou /* */)
	- add a function to detect include in C files then parse them
	- manage implicit declaration of const array (const char a[] = {3,3}) on one or multiple lines
- solve the segmentation fault wich appears in tcc when using it without the -m32 flag (requirement to have 64b version). Try to apply these 2 updates manually to see if it works :
	- [cleanup: stop abuse of sym->c for #define tokenstreams](https://github.com/adorad/tcc/commit/bed17847bdfa872e219a12237e06df83e464bdba)
	- [cleanup: stop abuse of sym->r for inline functions](https://github.com/adorad/tcc/commit/d0b432ab38b09cd167d8a8065ffe1e14878d6e2d?diff=split)
- manage LZ compression for graphics

# Medium priority:

- have only one credit list in readme.md, automaticaly injected in snes.h. Remove credit from wiki page
- replace .obj files by .lib with wla dx
- Finish to implement mode 5, 6 by adapting gfx2snes to have compatible pictures (see [here](https://github.com/alekmaul/pvsneslib/issues/14))
- replace tasm.exe part (spc700) by the wla dx version to provide a unix alternative
- integrate docker image
- add some chipsets support (sa1, dsp, superfx, ...)
- clean some issues opened for a long time on github
- create new sample to show how to load sounds and musics at the same time (we need to respect specific order: sound at first, then music...)
- replace pixel.c file by asm version to improve performances and provide function to draw pixels
- improve the current text system to draw on any background. See the .c example created by [Digifox](https://github.com/malayli/snes-bg3-text)
- test to build the library with Cygwin and update it if necessary
- add fixed point support 

# Low priority:

- Fix cygwin and gitbash incompatibility
- Improve mode 7 camera rotation
- Add capabilities to create HIrom games
- Add a check (in tcc or in other tool?) to compare the signature of functions between .c and .h file and trigger an error if they are different (it can save a lot of time to developers because this difference is hard to see and produce strange result)
- Manage hblank
- develop new driver to get more files format with sound engine
- add in snes rules the check on PVSNESLIB_DEBUG variable. If it exists, it will create automatically the temporary tree to store all steps executed during the build (.ps file, .asp, .asm...). It will be easier and quick to see the content of each files to debug it.
- update in pvsneslib sources, samples code and tools which use extern variables the data type (probably char) by the real type (u16 for map/sprites...). It will avoid some cast in the code will be more clear
- add new check on variable PVSNESLIB_DEBUG to provide detailed informations during the build process to assist users when they begin to use the library. For example: if you use setmode with mode7, display an error saying to use setmode7 function instead. It is a kind of assert which could be managed in asm with .FAIL directive. By default, the library could be built without this variable and it will not decrease performances
- Add mouse driver compatibility
- Add superscope compatibility
- Add all needed extensions for C/C++ and vscode (like Intellisense, etc ...) in wiki part of vscode template
- create a setScreenOff function to be harmonised (the lib have setScreenOn) which do the same thing than setBrightness