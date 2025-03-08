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

Add the correct parameters to your makefile conditions (here, we specified that we will use bank 5 as 1st bank for music and sfx).  

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

Check soundbank.asm file to know exactly how many banks you have and if the correct bank is choosen for them.

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

## Adding sound effects to your game with one IT file  

Sound effects in IT file is the best weay to play sounds during the game.  

**IT sound effects you want to have at the current state of your game need to be in the SAME IT file.**  

*Remember that all effects you want to have at a current state of your game **plus** the current music need to be lower than 64K, which is the size of the SPC RAM. This is beacuase we are going to load them in RAM with music and the sound engine.*  

When you use **spcconv** to convert your IT files, it will give you the sum of each IT file and also the difference between the 1st IT file you gave in the command line (which will be your IT sound effects file) and all the others.

If you need another IT sound effects file, you will have to do another smconv command with the sound effect plus the musics your have to have with it.  

In the next part, we will use **effectsandmusic** from the **snes-examples/audio** folder.  

### Prepare your wav sfx files  

All sfx files should be with a low frequency to save space. 8Mhz is a good frequency but you can use a higher one if your want.

With **openMTP** or another IT tracker, put all the sfx wav files in the sample part of your empty IT file.

<img src="https://github.com/user-attachments/assets/6e5ec9f1-236f-440c-bd02-86b040f19283">

Remember that you must also have **ONE** instrument mapping **ONE** sample or the sound engine will not work.  

The pattern part is not really important must must be present in the IT file.

<img src="https://github.com/user-attachments/assets/20fdd4b4-0932-4baf-84e8-0e348820b193">

### Compile the IT file with your music  

In this example, **effectssfx.it** contains all the sfxs and **pollen8.it** is a music file.  

```bash
# BEFORE including snes_rules :
# list in AUDIOFILES all your .it files in the right order. It will build to generate soundbank file
AUDIOFILES := res/effectssfx.it res/pollen8.it
```

And don't forget the parameters for smconv if you want something specific (in our example, it is bank 5 with b parameter).  

```bash
# to build musics, define SMCONVFLAGS with parameters you want
SMCONVFLAGS	:= -s -o $(SOUNDBANK) -V -b 5
musics: $(SOUNDBANK).obj
```

As explained in previous chapter, inside res directory of the example, you have the asm file generated with the correct includes for your sfx and musics.  

```bash
...

.BANK 5
.SECTION "SOUNDBANK0" ; need dedicated bank(s)

..

.incbin "res/soundbank.bnk" skip $8000
.ENDS
```

When compiling, smconv will indicate the footprint of sfxs and also the amount of memory available with the music file.  

```bash
smconv: Loading modules...
smconv: Starting conversion...
-----------------------------------------------------------------------
Adding module, Filename: <res/effectssfx.it>
             Title: <>
             IT Size: [76683 bytes]
Conversion report:

...
           Total: [20803 bytes]   *37154 bytes free*
-----------------------------------------------------------------------
Adding module, Filename: <res/pollen8.it>
             Title: <pollen resurrection>
             IT Size: [29369 bytes]
Conversion report:

...
           Total: [44137 bytes]   *13820 bytes free*
-----------------------------------------------------------------------
  Total Modules Size: [ 64940 bytes]
       Total IT Size: [106052 bytes]
```

### Loading and playing sfx IT effects in your program

We already explained that we will 1st need to load the banks in memory.   

```bash
    // Set soundbanks in reverse order
    spcSetBank(&SOUNDBANK__1);
	spcSetBank(&SOUNDBANK__0);
```

Then, you need to load sfx that are stored in the 1st entry. As we declared 5 samples, we will load them.

```bash
	// Load all effects
	spcStop(); spcLoad(0); 
	for (j=0;j<5;j++) { 
        spcLoadEffect(j); 
    }
```

It is also a good practice to wait a VBL after that with a  **WaitVBLFlag** call.

During your game, you play the sfx with the **spcEffect** function (i is the variable containing the effect index, 0 is the 1st one).  

```bash
    // Play effect
    spcEffect(8,i,15*16+8); // (u16 pitch,u16 sfxIndex, u8 volpan); pitch=1=4Khz , 2=8khz, 8=32Khz
```

## Adding sound effects to your game with some brr files  

You can also play brr sound files when playing music. In these case, they are not include inside a IT file. AS in previous chapter, all brr files should be with a low frequency to save space. 8Mhz is a good frequency but you can use a higher one if your want.


### convert your wav files  

TO convert your wave files and allow them to be played, you must declare them in your makefile.  

```bash
tada.brr: tada.wav
	@echo convert wav file ... $(notdir $<)
	$(BRCONV) -e $< $@
```

And add the **brr converted file** in your **data.asm** file.

```bash
soundbrr:
.incbin "tada.brr"
soundbrrend:
```

### Loading and playing brr effects in your program  

You must declare a variable with a specific structure in your c file

```bash
extern char soundbrr, soundbrrend;
brrsamples tadasound;
```

Also, you must reserved a specific amount of **SPC RAM** to allow your brr file to play.  

**BE CAREFULL**, you have only around 60K of RAM availabe for effects and musics !

```bash
    // allocate around 10K of sound ram (39 256-byte blocks)
    spcAllocateSoundRegion(39);
```  
 
During your game, you play your brr with the **spcEffect** function after loading it with the **spcSetSoundEntry** function.

```bash
    // Load effect
    spcSetSoundEntry(15, 15, 4, &soundbrrend - &soundbrr, &soundbrr, &tadasound);

    // Play effect
    spcPlaySound(0);
```
