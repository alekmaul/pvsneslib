No$sns (pronounce it: No cash snes) is an emulator and debugger for Windows.

Like PVSnesLib full environment is currently for Windows system only, this one seems to be a good choice for its performances and capabilities.
Other emulators nearest hardware of Super Nintendo exist and you will have to try it but No$sns will be very useful during development of your project.

If you want more information on it, you can consult [web site of developer](https://problemkaputt.de/sns.htm)

# Configuration

If it is the first time you launch this software, the main window is the debugger : it contains hexadecimal information on your rom.
Go to "Options" tab, then "Emulation Setup" and configure keyboard or joypad buttons as you want. When you will finished it, do not forget to do "Options" then "Save Options".

PVSneslib has a function to help debug of your code : it use console to print information. Depending the version of the software you use, you may have to enable it or adapt the value to your needs.

Go to "Options", "Debugger Setup" then update the hereunder value and save it :

For old versions :

![configure debug messages](https://user-images.githubusercontent.com/48180545/56092236-b202cf00-5eb9-11e9-93af-5773d029de38.png)

For recent versions :

![configure debug messages](https://user-images.githubusercontent.com/48180545/214948543-f6ece2c0-db83-4c86-bab2-2315c05abe64.png)

To open this window, go to "Window" tab then "Debug Message Window". In your project, you can now print information using `void consoleNocashMessage (const char *message)` defined in console.h file.
This function works like [standard printf function](https://en.wikipedia.org/wiki/Printf_format_string).

If you need a sample, take a look in "debug" example provided with PVSneslib.


# VRAM Viewer

This is part you will use more while developing. Go to "Window" tab then "VRAM Viewer".

The interface can be separate in 3 parts :

![Vram viewer interface](https://user-images.githubusercontent.com/48180545/56092240-bc24cd80-5eb9-11e9-9b31-8ef8675ea79f.png)

- Background tabs : each one which contain picture loaded. Their contents depends of background mode you used in your project because some of them doesn't exists if specific mode.
To get more information on backgrounds, consult [the wiki page](https://github.com/alekmaul/pvsneslib/wiki/Backgrounds)

- Tiles tab contains pictures you loaded for your projects. Memory is shared between each tab so if you load a 4bpp Picture, you will see chunk of it in other tabs.
But, do you know what is means bpp ?
You know that Snes can manage pictures of 2, 16 and 256 colors.
bpp are Bits Per Pixels:
	- 2 bpp means 2² = 4 colors
	- 4 bpp means 2⁴ = 16 colors
	- 8 bpp means 2⁸ = 256 colors

- OAM and palettes tabs : OAM is Object Attribute Memory where we store sprites information and Palettes is the tab where you see color you use.