All SNES rom have an internal header that is used to identifying the game, producer, region and technical aspects of the ROM. It's often referred as Internal ROM Header or SNES Software Specification.

Although it's not required to run a game on real hardware, the SNES ROM Header was used during the Nintendo approval process for validation and it's also used by the SNES emulators to identify and determine the memory layout and ROM type.

The data starts at SNES address $00:FFB0 and ends at $00:FFDF. $00:FFE0 though $00:FFFF contains the SNES vector information and it's actually used by the SNES CPU to determine where to execute when an interrupt occurs.

## ROM Registration Data

ADDRESS	LENGTH	DATA NAME TYPE
$00:FFB0	2 bytes	Maker Code
$00:FFB2	4 bytes	Game Code
$00:FFB6	7 bytes	Fixed Value
$00:FFBD	1 byte	Expansion RAM Size
$00:FFBE	1 byte	Special Version
$00:FFBF	1 byte	Cartridge Type (Sub-number)
$00:FFC0	21 bytes	Game Title Registration
$00:FFD5	1 byte	Map Mode
$00:FFD6	1 byte	Cartridge Type
$00:FFD7	1 byte	ROM Size
$00:FFD8	1 byte	RAM Size
$00:FFD9	1 byte	Destination Code
$00:FFDA	1 byte	Fixed Value
$00:FFDB	1 byte	Mask ROM Version
$00:FFDC	2 bytes	Complement Check
$00:FFDE	2 bytes	Check Sum