.-----------------------------------------------------------------------.
| SNESMOD COMPOSITION RULES                                             |
|-----------------------------------------------------------------------|
| Impulse tracker format must be used. 8 Channels MAX!                  |
|                                                                       |
| Notes can not exceed 128Khz playback rate!!                           |
|                                                                       |
| The module must be in instrument mode. It must have instruments!      |
|                                                                       |
| DO NOT USE UNDEFINED INSTRUMENTS, they will not silence the channel,  |
| THEY WILL CAUSE MALFUNCTION!                                          |
|                                                                       |
| The 64 or less samples must fit within 58K of memory. This is AFTER   |
| "BRR" compression. 8-bit samples will be reduced to 9/16 size. 16-bit |
| samples will be reduced to 9/32 size.                                /
|=====================================================================|
| Cut Features                                                        |
|---------------------------------------------------------------------|
| There is not be a complete IT implementation, here are some         |
| IT features that you must not use:                                  |
|                                                                     |
| * New Note Actions are not supported.                               |
| * Complex sample mapping is not supported.                          |
|     Only the center entry in each instruments sample map will be    |
|     used for the entire map! (and the note values are ignored!!!)   |
| * Pitch envelope is not supported.                                  |
| * No stereo samples!!                                               |
| * Envelope sustain must remain on one node only.                    |
| * Envelope nodes must be only 1 to 256 ticks apart.                 |
| * Envelope carry is not supported.                                  |
| * Random volume/panning variations are not supported.               |
| * Pitch/Pan separation is not supported.                            |
| * "Old effects" must be set to 'off'.                               |
| * Only linear frequency mode is supported.                          |
| * Gxx must not share memory with Exx/Fxx.                           |
| * Auto-vibrato is not supported.                                    |
| * Filters are not supported. (doy)                                  |
| * These volume effects are not supported:                           |
|     Ex, Fx, Gx, Hx                                                  |
| * No memory for volume commands. (A0,B0,C0,D0 will do nothing)      |
| * These effects are partially/not supported:                        |
|     Cxx - xx must be zero.                                          |
|     S1x - not supported                                             |
|     S2x - not supported                                             |
|     S3x - not supported                                             |
|     S4x - not supported                                             |
|     S5x - not supported                                             |
|     S7x - not supported                                             |
|     S9x - not supported                                             |
|     S99 - toggle duck modulator (not supported)                     |
|     SAy - not supported                                             |
|     SBx - not supported - pattern loop, sorry :(                    |
|     Zxx - not supported                                             |
|                                                                     |
| And because mukunda is so damn lazy these are also not supported:   |
|  panning envelopes, tremor, tremolo, panbrello                      |
|                                                                     |
| * "+++" patterns ARE supported!!!                                   |
|=====================================================================|
| Echo commands                                                       |
|---------------------------------------------------------------------|
| S0x is used to toggle echo for a channel. Only the SPC channels can |
| have echo and use the commands.                                     |
|                                                                     |
| S01 - Turn off echo for channel.                                    |
| S02 - Turn on echo for channel.                                     |
| S03 - Turn off echo for all channels.                               |
| S04 - Turn on echo for all channels.                                |
|                                                                     |
| The other echo settings for the module can be configured during     |
| conversion. SEE "SPECIAL SONG MESSAGE COMMANDS"                     |
|                                                                     |
| NOTE: When setting the Echo Delay parameter, you lose "2K x D" of   |
| sample memory from the SPC.                                         |
|=====================================================================|
| Quality                                                             |
|---------------------------------------------------------------------|
| 16-bit quality is recommened for the SPC samples, since they        |
| have more data for compressing.                                     |
|                                                                     |
| If the loop width of an SPC sample is not a multiple of 16 then the |
| sample will be unrolled OR resampled with linear interpolation for  |
| alignment. (it will be resampled if unrolling uses too much memory) |
|                                                                     |
| Do not downsample your SPC samples too much or else they will sound |
| like butt.                                                          |
|                                                                     |
| Sample offset is supported for the SPC samples, but there is a      |
| small chance of creating unwanted NOISE because of the compression. |
|                                                                     |
| ALSO NOTE BIDI SAMPLES WILL BE UNROLLED AND USE UP LOTS OF SPACE.   |
|                                                                     |
| *ALSO* If you are making songs for a game remember that channel 8   |
| will be OVERWRITTEN with sound effects. Don't put important stuff   |
| there.                                                              |
|=====================================================================|
| SPECIAL SONG MESSAGE COMMANDS                                       |
|---------------------------------------------------------------------|
| To control the echo parameters for a module you must specify some   |
| things in the song message.                                         |
|                                                                     |
| To trigger this you must put an "[[SNESMOD]]" tag in your song      |
| message followed by special commands!                               |
|                                                                     |
| See the example IT module, it has song message commands!            |
|                                                                     |
| Command List:                                                       |
|                                                                     |
| EDL <delay value>                                                   |
|                                                                     |
|   This sets the echo delay value. Range is 0-15                     |
|   Delay time will be d*16 milliseconds.                             |
|   NOTE: each 16ms of delay will use 2K of SPC memory. Your song's   |
|   size limit will be more restricted with larger amounts of delay   |
|   used.                                                             |
|                                                                     |
|   Issuing this command will enable echo (master) for the song       |
|                                                                     |
|   Example:                                                          |
|                                                                     |
|     "EDL 5"                                                         |
|                                                                     |
|   The echo delay time will be 5*16 (80) milliseconds. 10 Kilobytes  |
|   will be taken away from the memory space, so it will have some-   |
|   thing like 48 KB left.                                            | 
|                                                                     |
| EFB <feedback level>                                                |
|                                                                     |
|   Set echo feedback level. Range is -128..127 (-100%..100%)         |
|   Default value (if no command present) is 0.                       |
|                                                                     |
|   Example:                                                          |
|                                                                     |
|     "EFB 127"                                                       |
|                                                                     |
|   The echo feedback level will be set to 127, or 100%.              |
|   This value will probably destroy the sound because the feedback   |
|   will cover the output.                                            |
|                                                                     |
| EVOL <volume> [volume right]                                        |
|                                                                     |
|   Set echo volume level. Range is -128..127.                        |
|   One or two values may be given. If only one is specified then it  |
|   will be used for both left and right outputs. If two values are   |
|   given then one will b eused for each output (left, right)         |
|                                                                     |
|   Examples:                                                         |
|                                                                     |
|     "EVOL 40"                                                       |
|                                                                     |
|   Both left and right echo outputs will have a volume of 40,        |
|   or 31%.                                                           |
|                                                                     |
|     "EVOL 31 -31"                                                   |
|                                                                     |
|   Left output will be 25%, right output will be -25%                |
|                                                                     |
|   Negative values will indeed inverse the phase of the applied      |
|   waveform.                                                         |
|                                                                     |
| EFIR <c0> [c1] [c2] [c3] [c4] [c5] [c6] [c7]                        |
|                                                                     |
|   Specifies the coefficients for the 8-tap FIR filter applied to    |
|   the echo output.                                                  |
|                                                                     |
|   Default values are 127, 0, 0, 0, 0, 0, 0, 0                       |
|                                                                     |
|   If less than 8 values are specified then the default values will  |
|   be used for the rest of the entries.                              |
|                                                                     |
|   Example:                                                          |
|                                                                     |
|     "EFIR 64 -32 16"                                                |
|                                                                     |
|   Set the first three coefficients to these values and the rest     |
|   will be zero.                                                     |
|                                                                     |
| EON <channels>                                                      |
|                                                                     |
|   Enable echo for the channels listed.                              |
|                                                                     |
|   Example:                                                          |
|                                                                     |
|     "EON 1 3 4 5"                                                   |
|                                                                     |
|   Enable echo for channels 1 (first), 3, 4, and 5.                  |
|                                                                     |
| Here is an example song message with commands in it:                |
|---------------------------------------------------------------------|
| Here is my song, blah blah blah blah                                |
| By Mr.X                                                             |
|                                                                     |
| [[SNESMOD]]                                                         |
| edl 6                                                               |
| efb 127                                                             |
| evol 31 -31                                                         |
| efir 127 0 0 0 0 0 0 0                                              |
| eon 1 2 3                                                           |
|=====================================================================|
| (PLEASE ENJOY)                                                      |
`---------------------------------------------------------------------'
  -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
