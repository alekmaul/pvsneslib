PVSneslib uses Impulse Tracker (IT) files to play musics, it also uses a specific IT file for sound effects.  

## Tools to create IT files

You can use a tool like [openMPT](https://openmpt.org/) to create your song.  

## Composition rules for your IT files

Impulse tracker format must be used. 8 Channels MAX!  
Notes can not exceed 128Khz playback rate!  
The module must be in instrument mode. It must have instruments!  

DO NOT USE UNDEFINED INSTRUMENTS, they will not silence the channel, THEY WILL CAUSE MALFUNCTION!  

The 64 or less samples must fit within 58K of memory. This is AFTER "BRR" compression. 8-bit samples will be reduced to 9/16 size. 16-bit samples will be reduced to 9/32 size.  

## Tips from KungFuFurby about samples in IT files  

When something going wrong for the songs you attempted to convert:  

- Sample loop points must be divisible by 16. Loop points not divisible by 16 may not convert correctly. 
If you don't use loop points divisible by 16, at least make sure the loop length is divisible by 16.  

I use Schism Tracker to perform the job of loop point analysis and loop point adjustments since I can just simply type in the numbers.

I use a calculator to take care of loop point problems simply related to the sample being at the wrong sample rate to have loop point lengths divisible by 16 (the length of the looping portion of the sample should at the very least be divisible by 16).
You usually perform this on samples with shorter loop point lengths. 

I don't think it works so well on ones with longer loop point lengths, mainly because by then you're probably not dealing with simple waveforms as loops.

Using the Bass & Lead sample as an example...

Loop point is currently defined as...
Start: 3213
End: 3382

That's a loop length of 169.

I like using powers of 2 for my loop points so that if I have to decrease the quality of the sample, then I can do so as painlessly as possible to the sample (unless I find the degraded quality to be a bad idea), so that means 128 is the value I use here.

Divide 169 by 128 gets you an unusual decimal number... copy that number.  

Now get the full length of the sample (that's 3383 for this sample) and divide by that decimal number you acquired earlier (169/128).  
You'll most likely get another unusual decimal number. Round that off and there's your new length that you will resize the sample to.  

I use Alt-E in Schism Tracker to perform the resize sample command.  
The program will ask for the new size of the sample.  

Now you should have a loop length that is divisible by 16. You can perfect the loop point by adjusting them so that the loop point themselves are divisible by 16.  

- Only one sample can be defined per instrument...  

You'd have to duplicate the instruments and then enter the sample ID for all of those notes... and then you have to redefine the instrument IDs depending on the pitch from the old note table. Yeah...  

- I thought in one case, I saw the pitch go too high (it went over 128khz). That's because I noticed a hiccup in the notation.  

For the one song that has this problem, I usually resize the sample and make it half of its length...  
however, I may have to make additional adjustments depending on how the loop points are holding up (length may or may not be involved, although usually I'm checking the loop points themselves to make sure that they are divisible by 32 or some higher power of 16... this indicates how many times you can cut the sample in half).  

- Note Cut is the only New Note Action supported for SNESMod.  

One of these songs is the most visibly affected by this problem, and that's because SNESMod doesn't virtually allocate channels.  
You have to modify the patterns so that the note off commands go where the note would originally play, and the new note is put on another channel.  

## Adding music to your game  

To add music capabilty to your game, you must begin your main file with the boot initialization of the SPC700 processor.  
This **spcBoot** function will copy the Impulse Tracker emulator to SPC700.  
You will find all audio examples in **snes-examples/audio**, shipped with PVSneslib release.  

```bash
#include <snes.h>

//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize sound engine (take some time)
    spcBoot();
```

### Compiling musics and initialize the banks used by musics  

To add musics to your game, you must compile them to allow the driver to play them in the game.  
**smconv** is the tool shipped with PVSneslib to do the job.  

```bash
Put your IT files in a res subdirectory of your game, and add them to your makefile.
# list in AUDIOFILES all your .it files in the right order. It will build to generate soundbank file
AUDIOFILES := res/whatislove.it
# then define the path to generate soundbank data. The name can be different but do not forget to update your include in .c file !
export SOUNDBANK := res/soundbank
```

Add the correct parameters to your makefile conditions.

```bash
# to build musics, define SMCONVFLAGS with parameters you want
SMCONVFLAGS	:= -s -o $(SOUNDBANK) -V -b 5
musics: $(SOUNDBANK).obj

all: musics  $(ROMNAME).sfc
```

We must initialize the sound banks containing the musics, in reverse order if you have more that one 32K bank.  

```bash
// soundbank that are declared in soundbank.asm
extern char SOUNDBANK__0, SOUNDBANK__1;
```

Later in your code, after the sound engine initialization, you must declare the sounbanks, in **reverse order**!

```bash
    // Set soundbank available in soundbank.asm. Yes, in reverse order !
    spcSetBank(&SOUNDBANK__1);
    spcSetBank(&SOUNDBANK__0);
```

Check soundbank.asm file to know exactly how many banks you have.

```bash
.BANK 5
.SECTION "SOUNDBANK0" ; need dedicated bank(s)

SOUNDBANK__0:
.incbin "res/soundbank.bnk" read $8000
.ENDS

.BANK 6
.SECTION "SOUNDBANK1" ; need dedicated bank(s)

SOUNDBANK__1:
.incbin "res/soundbank.bnk" skip $8000
.ENDS
```

### Playing musics  

To play a  music file, you must first load the music file and then play it. You must pay attention that it will take some time to load it.  
It is because the SNES cpu will send the music to the audio CPU.

```bash
    // Load music
    spcLoad(MOD_POLLEN8);
```

You play the file with a parameter, allowing you to begin the music when you want. 0 must be use to play it from the beginning.  

```bash
    // Play file from the beginning
    spcPlay(0);
```
Use **spcStop()** to stop music.  

In your main loop, you must add a function named **spcProcess** just before waiting VBlank to allow the driver to process the music, without this instruction, it will not play music!

```bash
        // Update music / sfx stream and wait vbl
        spcProcess();
        WaitForVBlank();
```

## Adding sound effects to your game with some brr files  


## Adding sound effects to your game with one IT file  

