PVSneslib uses IT files to play musics, it also uses a specific IT file for sound effects.

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