-= The Last Super (source) =-

A brand new SNES game to celebrate the SNES 30th birthday!
by Dr. Ludos - 11/21/2020

Get all my other games: 
	http://drludos.itch.io/
Support my work and get access to betas and prototypes:
	http://www.patreon.com/drludos

Gameplay music: xracecar - 7049 Bytes of Memories
Used under a Creative Commons BY-NC-SA license:
	http://battleofthebits.org/arena/Entry/7049+Bytes+of+Memories/25393/
		
Title screen music: xracecar - ayyy you know the man
Used under a Creative Commons BY-NC-SA license:
	http://battleofthebits.org/arena/Entry/ayyy+you+know+the+man.xm/23747/


-= ABOUT =-

This game is a kind of a sequel / expanded version of a game I originally created in a Jam:
	Keeping SNES Alive: https://drludos.itch.io/keeping-snes-alive

The game is simple, but even with modern and easy tools, programming retro hardware is quite a challenge
(and the SNES is not a simple platform to understand!).

I don't pretend to provide you with "state of the art" code, but this is my 3rd SNES game created in C, so I start to get more confortable working with the console.
However, I still have plenty of things to learn, so please don't judge this game on its technical quality, but rather on the fun that I may provide you when you play! :)

I spent quite a lot of time commenting and cleaning the code for better readibility / learning.

I hope it'll help you make your own SNES games too!


-= HOW TO BUILD =-

The game is written in C. To compile it for the SNES, you'll have to use the PVSNESLib SDK,
created by legendary homebrew developer AlekMaul.

You can find the latest version here:
https://github.com/alekmaul/pvsneslib

When you have the toolchain all set up, please edit the "Makefile" first to set the PATH to your SNES SDK! 
Then you can call:
make all

It'll generate you a "TheLastSuper.sfc" file. It's a fully functional Super Nintendo program that you can load into
an emulator.I personnaly recommend BSNES (or higan) as emulator, as it's the most accurate available:
https://byuu.org/bsnes


-= GRAPHICS =-

Graphics are automatically converted to SNES format by a PVSNESLib tool named gfx2snes.

This tool expects graphic images in .BMP format as entry, and there are a lot of parameters that allow you
to specify how to generate the palette, tilemap (for background), sprite size (for sprites), etc.
That's also how you can prepare graphics for a specific SNES display mode.

The parameters for each .BMP file are defined inside the Makefile. 

For more details, please refer to the PVSNESLib documentation :) 
(or gfx2snes built-in commande line help message)


-= AUDIO =-

Audio is a tricky part on SNES. 
Each SNES game had to create a "sound driver" that will be run by the SNES SOUND Processor (S-SMP). 

This game use the "SNESMod" driver, that comes packed with SNESLib. It's a great driver, created by Mukunda
Johnson, that is very well suited to play "tracker music", and more specifically "Impulse Tracker" (.it)
files.

So you can create music with your favorite tracker (8 channels max!) and PVSNESLib will convert it for
SNES playback (with many limitations detailed in the docs).

For SFX, the driver has actually two ways to play them:
- It can load some SFX in the S-SMP RAM to play them back. You can have up to 2 SFX playing with this method, 
but they have to fit inside the RAM alongside the music track!
- It can also stream a single SFX from the ROM data. There is no size limit here, but be warned that the 
streaming code will only work flawlessly on NTSC consoles (on PAL consoles, sound can be glitchy unless you 
manually patch the driver to set PAL timing in the streaming functions)

For this game, I chose to use only the "load SFX into RAM" function.
To do this, I had to embed each .wav file into a separate ".it" file, where the sound effect file is a sample
These are the "audio_xxxx.it" file in the source dir.

These files are then converted by PVSNESlib and can be played as SFX back through the driver
I left many comments in the code related to this part to try to explain it as best as I could, 
as I spent an insane amount on time on getting audio to work when developing Yo-Yo Shuriken!


-= FINAL WORDS =-

Well, I hope you'll find it useful, and that it'll help you get into SNES homebrew development!

If you actually happen to read or want to use this source code and have any question about it, 
feel free to get in touch: drludos@ludoscience.com

I hope you'll enjoy the game!

All the best,
Dr. Ludos