TO CHECK https://sneslab.net/wiki/SNES_ROM_Header#:~:text=Although%20it's%20not%20required%20to,and%20ends%20at%20%2400%3AFFDF.


All SNES rom have an internal header that is used to identifying the game, producer, region and technical aspects of the ROM. It's often referred as Internal ROM Header or SNES Software Specification.

Although it's not required to run a game on real hardware, the SNES ROM Header was used during the Nintendo approval process for validation and it's also used by the SNES emulators to identify and determine the memory layout and ROM type.

The data starts at SNES address $00:FFB0 and ends at $00:FFDF. $00:FFE0 though $00:FFFF contains the SNES vector information and it's actually used by the SNES CPU to determine where to execute when an interrupt occurs.

The header is located at the CPU address range $00FFC0-$00FFDF, right before the interrupt vectors, with an optional second header at $00FFB0-$00FFBF.  

This means that the location of the header within the actual ROM file will change based on the cartridge's memory map mode - with LoROM games placing it at $007Fxx, HiROM games placing it at $00FFxx, and ExHiROM games placing it at $40FFxx.  

Therefore, if it's correctly filled out, an emulator will have a higher chance of being able to figure out where the header is. See: Header Verification below.


## ROM Registration Data

### Cartridge header  

|ADDRESS|LENGTH|DATA NAME|  
|-------|------|---------|  
|$00:FFC0|21 bytes|Game Title Registration|  
|$00:FFD5|1 byte|Map Mode|  
|$00:FFD6|1 byte|Cartridge Type|  
|$00:FFD7|1 byte|ROM Size|  
|$00:FFD8|1 byte|RAM Size|  
|$00:FFD9|1 byte|Destination Code|  
|$00:FFDA|1 byte|Fixed Value|  
|$00:FFDB|1 byte|Mask ROM Version|  
|$00:FFDC|2 bytes|Complement Check|  
|$00:FFDE|2 bytes|Check Sum|  

#### Game Title Specification  
The game title is 21 bytes long, encoded with the JIS X 0201 character set (which consists of standard ASCII plus katakana). If the title is shorter than 21 bytes, then the remainder should be padded with spaces (0x20).  

#### Map Mode
Common values: #$20 - 2.68MHz LoROM, #$21 - 2.68MHz HiROM, #$23 - SA-1; #$25 - 2.68MHz ExHiROM; #$30 - 3.58MHz LoROM, #$31 - 3.58MHz HiROM; #$35 - 3.58MHz ExHiROM  

#### Cartridge Type
Common values: #$00 - ROM only; #$01 - ROM and RAM; #$02 - ROM, RAM and battery; #$33 - ROM and SA-1; #$34 - ROM, SA-1 and RAM; #$35 - ROM, SA-1, RAM and battery  

#### ROM Size
2^(this value) would be the size of the ROM in kilobytes. For example, for 512KB, this should be #$09.  

#### RAM Size
2^(this value) would be the size of the SRAM (if present) in kilobytes. Maximum supported value is #$07.  
Exception: If you're using Super FX aka GSU-1, move this value to the Expansion RAM Size field, and put #$00 in this byte.  

#### Destination Code
Where the game is intended to be sold. Common values: #$00 - Japan; #$01 - USA; #$02 - Europe (enables 50fps PAL mode)  
Store the code, from the table below, which best describes where the product will be sold.

|Value|Destination|ROM ROM Recognition Code (Fourth digit of Game Code)|  
|-------|------|---------|  
|00H|Japan|J|
|01H|North America (USA and Canada)|E|
|02H|All of Europe|P|
|03H|Scandinavia|W|
|06H|Europe (French only)|F|
|07H|Dutch|H|
|08H|Spanish|S|
|09H|German|D|
|OAH|Italian|I|
|OBH|Chinese|C|
|ODH|Korean|K|
|OEH|Common|A|
|OFH|Canada|N|
|10H|Brazil|B|
||Nintendo Gateway System|G|
|11H|Australia|U|
|12H|Other Variation|X|
|13H|Other Variation|Y|
|14H|Other Variation|Z|


### Expanded cartridge header    

|ADDRESS|LENGTH|DATA NAME|  
|-------|------|---------|  
|$00:FFB0|2 bytes|Maker Code|  
|$00:FFB2|4 bytes|Game Code|  
|$00:FFB6|7 bytes|Reserved, should be zero|  
|$00:FFBC|1 byte|Expansion flash size: 1 << N kilobytes|  
|$00:FFBD|1 byte|Expansion RAM size: 1 << N kilobytes - for GSU?|  
|$00:FFBE|1 byte|Special Version (usually 0)|  
|$00:FFBF|1 byte|Chipset subtype, used if chipset is $F0-$FF|  

#### Maker Code  
Two alphanumeric ASCII bytes identifying your company. The 2-digit ASCII code are assigned by Nintendo. Refer to the Nintendo/ Licensee contract, if in doubt. All letters must be in upper case.  

#### Game Code  
Four alphanumeric ASCII bytes identifying your game. The 4-digit Game Code assigned by Nintendo in ASCII. All letters must be in upper case.  
Exception: If the game code starts with Z and ends with J, it's a BS-X flash cartridge.  

