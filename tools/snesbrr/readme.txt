snesbrr - An SNES BRR encoder / decoder.

Version: 2006-12-13
 Author: DMV27
License: Common Development and Distribution License, Version 1.0 only


Overview
--------

snesbrr encodes standard PCM wave files into BRR files that can be used
by the S-DSP of the SNES. It supports all valid combinations of BRR ranges
and filters as well as optional sample looping.



Usage
-----

snesbrr [options] input-file output-file



Command Line Options
--------------------

-?
--help
    Print a summary of the command line options and exit.

-e
--encode
    Encode WAV (input-file) to BRR (output-file).
    Valid options: '--loop-start'.

-d
--decode
    Decode BRR (input-file) to WAV (output-file).
    Valid options: '--pitch', '--enable-gauss'.

-t
--transcode
    Transcode WAV (input-file) to BRR (internal) to WAV (output-file).
    Valid options are the same as those specified by '--encode' and
    '--decode'.

-l START
--loop-start START
    Set the loop start sample to the decimal value START. If this value
    or the size of the loop is not a multiple of 16, the looped samples
    will be repeated until both the start and end points are multiples
    of 16. Looping is disabled by default.

-p PITCH
--pitch PITCH
    Set the pitch rate to the hexadecimal value PITCH. This value is
    only used during decoding. It must be between 0x0001 and 0x3FFF.
    The default value is 0x1000.

-g
--enable-gauss
    Enable gaussian filtering during decoding. This setting can be
    used to simulate the decoding process of an SNES.



Tips
----

* The loop start point and the loop size should both be multiples of 16
  in order to produce the smallest possible BRR files.

* Choosing a higher sampling rate than needed for the source sample will
  improve the quality of the encoded sound. It will also help to offset
  the effects of gaussian filtering which can reduce the volume of the
  high frequencies (relative to the sampling rate).

  For example, a 4000 Hz square wave needs a minimum sampling rate of
  8000 Hz. However, at this rate the decoded square wave will only reach
  about 27% of its original volume level due to the gaussian filtering.
  If the rate is increased to 16000 Hz, it will reach 89% volume. At
  32000 Hz, 99% volume.

* Simple waves can easily be manually encoded for improved quality and
  size. For example, at a pitch rate of 0x1000 the waves listed below
  can be generated using the provided BRR block data.

  1000 Hz Square Wave:
  B0 77 77 77 77 77 77 77 77
  B3 99 99 99 99 99 99 99 99

  1000 Hz Triangle Wave:
  B0 01 23 45 67 76 54 32 10
  B3 FE DC BA 98 89 AB CD EF

  1000 Hz Sawtooth Wave:
  B0 00 11 22 33 44 55 66 77
  B3 88 99 AA BB CC DD EE FF



Limitations
-----------

* Filtering is disabled for the first block and the loop start block.

* Stereo wave files are converted to mono before being encoded.


