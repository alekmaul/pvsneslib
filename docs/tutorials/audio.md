# Audio: music and sound effects

PVSnesLib provides an SPC700 sound driver. The public interface is declared in `include/snes/sound.h`; the supplied examples under `snes-examples/audio` show the music-module, IT sound-effect and standalone BRR workflows.

This guide follows the [PVSnesLib Wiki — Sounds and Musics](https://github.com/alekmaul/pvsneslib/wiki/Sounds-and-Musics), with API details checked against the supplied source and examples.

## Creating tracker modules

The Wiki recommends [OpenMPT](https://openmpt.org/) for creating Impulse Tracker (`.it`) modules. The documented workflow uses instrument mode and has constraints on channels, sample count and SPC RAM use. Follow the Wiki's current tracker-format guidance and check the size of the converted sound bank; the available SPC memory is shared by music, effects and the driver.

## Booting the sound engine

Call `spcBoot()` once during startup. The declaration in `include/snes/sound.h` notes that it boots the SPC700 with the SM-SPC driver and that interrupts should be disabled during this time.

```c
#include <snes.h>

int main(void)
{
    spcBoot();

    /* Continue with the rest of the game initialization. */
}
```

## Building a sound bank

The supplied `snes-examples/audio/music/Makefile` uses `AUDIOFILES` to list the `.it` modules, `SOUNDBANK` to name the generated bank, and `SMCONVFLAGS` to pass options to `smconv`:

```make
AUDIOFILES := res/pollen8.it
export SOUNDBANK := res/soundbank

SMCONVFLAGS := -s -o $(SOUNDBANK) -V -b 5

musics: $(SOUNDBANK).obj
all: musics bitmaps $(ROMNAME).sfc
```

The combined effects-and-music example lists the effects module first:

```make
AUDIOFILES := res/effectssfx.it res/pollen8.it
export SOUNDBANK := res/soundbank
SMCONVFLAGS := -s -o $(SOUNDBANK) -V -b 5 -f
```

Its source explicitly warns that the effects module must be the first IT file in the conversion list, with music modules after it. Match module order, bank settings and generated symbols to the example you are following.

## Registering banks and playing music

The basic music example registers one bank and loads the module:

```c
extern char SOUNDBANK__;

spcSetBank(&SOUNDBANK__);
spcLoad(MOD_POLLEN8);
spcPlay(0);
```

For a sound bank spanning multiple ROM banks, the supplied `musicGreaterThan32k` example registers the banks in reverse order:

```c
extern char SOUNDBANK__0, SOUNDBANK__1;

spcSetBank(&SOUNDBANK__1);
spcSetBank(&SOUNDBANK__0);
```

`spcLoad()` loads a module, `spcPlay(0)` queues playback from the beginning, and `spcStop()` stops the current module. The header notes that `spcPlay()` queues a message; use `spcFlush()` if the program needs to wait for the message to be processed.

When using streamed music or effects, call `spcProcess()` every frame. The supplied music example does this before `WaitForVBlank()`:

```c
while (1) {
    spcProcess();
    WaitForVBlank();
}
```

## Sound effects stored in an IT module

The `snes-examples/audio/effectsandmusic` example loads the first IT module, then loads the effect samples with `spcLoadEffect()`:

```c
spcStop();
spcLoad(MOD_EFFECTSSFX);

for (j = 0; j < 5; j++) {
    spcLoadEffect(j);
}
```

During gameplay, `spcEffect(pitch, sfxIndex, volpan)` plays a loaded effect. The header defines pitch values as a rate selector (for example, 4 corresponds to 16 kHz); `sfxIndex` is 0–15, and `volpan` combines volume and panning in two four-bit values.

```c
spcEffect(4, effectIndex, 15 * 16 + 8);
```

The effect indices and loaded samples must match the IT module. Consult the complete example for the module order and memory budget.

## Standalone BRR sound effects

The supplied `snes-examples/audio/tada/tada.c` demonstrates standalone BRR playback. It boots the sound engine, reserves SPC RAM, registers the sample, and plays it:

```c
extern char soundbrr, soundbrrend;
brrsamples tadasound;

spcBoot();
spcAllocateSoundRegion(39);
spcSetSoundEntry(
    15, 15, 4,
    &soundbrrend - &soundbrr,
    &soundbrr,
    &tadasound
);

/* Later, when the sound is needed: */
spcPlaySound(0);
```

`spcAllocateSoundRegion(size)` reserves `size * 256` bytes and, according to the header, also stops module playback. `spcSetSoundEntry()` registers the sample in the sound table. The header notes that `spcPlaySound(0)` plays the last sound loaded; index 1 refers to the previous one.

## Reference

- `include/snes/sound.h` in the supplied PVSnesLib source archive.
- `snes-examples/audio/music`, `effectsandmusic`, `musicGreaterThan32k` and `tada` in the supplied examples archive.
- [Sounds and Musics — PVSnesLib Wiki](https://github.com/alekmaul/pvsneslib/wiki/Sounds-and-Musics)
