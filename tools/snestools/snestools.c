/*---------------------------------------------------------------------------------

    Copyright (C) 2012-2024
        Alekmaul

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any
    damages arising from the use of this software.

    Permission is granted to anyone to use this software for any
    purpose, including commercial applications, and to alter it and
    redistribute it freely, subject to the following restrictions:

    1.	The origin of this software must not be misrepresented; you
        must not claim that you wrote the original software. If you use
        this software in a product, an acknowledgment in the product
        documentation would be appreciated but is not required.
    2.	Altered source versions must be plainly marked as such, and
        must not be misrepresented as being the original software.
    3.	This notice may not be removed or altered from any source
        distribution.

    Header checker / modifier for snes.
    Some parts are based on Snes mess driver.

---------------------------------------------------------------------------------*/

#include <ctype.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"

// DEFINES
#define SNESTOOLSVERSION __BUILD_VERSION
#define SNESTOOLSDATE __BUILD_DATE

#define LOROM_HEADER 0x7fc0
#define HIROM_HEADER 0xffc0
#define OFFSET_CHECKSUM 0x001c
#define OFFSET_TITLE 0x0000
#define OFFSET_CARDTYPE 0x0016
#define OFFSET_ROMSIZE 0x0017
#define OFFSET_SRAM 0x0018
#define OFFSET_COUNTRY 0x0019

#define UNK "Unknown"

/* Some known type of cart */
static const char *const types[] = {
    "ROM",
    "ROM, DSP-1",
    "ROM, DSP-2",
    "ROM, DSP-3",
    "ROM, DSP-4",
    "ROM, Super FX / FX2",
    "ROM, SA-1",
    "ROM, S-DD1",
    "ROM, OBC-1",
    "ROM, S-RTC",
    "ROM, Z80GB (Super Game Boy)",
    "ROM, C4",
    "ROM, Seta ST-010",
    "ROM, Seta ST-011",
    "ROM, Seta ST-018",
    "ROM, SPC7110",
    "ROM, SPC7110, RTC",
    UNK, // to add: Satellaview BS-X detection
};

/* Special chips, checked at init and used in memory handlers */
enum
{
    HAS_NONE = 0,
    HAS_DSP1,
    HAS_DSP2,
    HAS_DSP3,
    HAS_DSP4,
    HAS_SUPERFX,
    HAS_SA1,
    HAS_SDD1,
    HAS_OBC1,
    HAS_RTC,
    HAS_Z80GB,
    HAS_CX4,
    HAS_ST010,
    HAS_ST011,
    HAS_ST018,
    HAS_SPC7110,
    HAS_SPC7110_RTC,
    HAS_UNK
};

// ram/rom coding memory amount
static const int memory_amount[] = {
    /*$00 => no RAM */ 0,
    /*$01 => $800 bytes == 2 kilobytes, amount of RAM in Super Mario World */ 2,
    /*$02 => $1000 bytes == 4 kilobytes */ 4,
    /*$03 => $2000 bytes == 8 kilobytes */ 8,
    /*$04 => $4000 bytes == 16 kilobytes */ 16,
    /*$05 => $8000 bytes == 32 kilobytes, amount of RAM in Mario Paint */ 32,
    /*$06 => $10000 bytes == 64 kilobytes */ 64,
    /*$07 => $20000 bytes == 128 kilobytes, amount of RAM in Dezaemon - Kaite Tsukutte Asoberu */ 128,
    /*$08 => $40000 bytes == 256 kilobytes, minimum for ROM */ 256,
    /*$09 => $80000 bytes == 512 kilobytes, amount of ROM in Super Mario World */ 512,
    /*$0a => $100000 bytes == 1 megabyte, amount of ROM in Mario Paint */ 1024,
    /*$0b => $200000 bytes == 2 megabytes */ 2048,
    /*$0c => $400000 bytes == 4 megabytes */ 4096
};

/* Some known countries */
static const char *const countries[] = {
    /* 0*/ "Japan ",
    "USA & Canada ",
    "Europe, Oceania & Asia ",
    "Sweden ",
    /* 4*/ "Finland ",
    "Denmark ",
    "France ",
    "Holland ",
    /* 8*/ "Spain ",
    "Germany, Austria & Switzerland ",
    "Italy ",
    "Hong Kong & China ",
    /* c*/ "Indonesia ",
    "South Korea ",
    UNK,
    UNK,
};

/* Some known companies (integrations to the list from Snes9x) */
static const char *const companies[] = {
    /* 0*/ "Invalid",
    "Nintendo",
    "Ajinomoto",
    "Imagineer-Zoom",
    "Chris Gray Enterprises Inc.",
    "Zamuse",
    "Falcom",
    UNK,
    /* 8*/ "Capcom",
    "HOT-B",
    "Jaleco",
    "Coconuts",
    "Rage Software",
    "Micronet",
    "Technos",
    "Mebio Software",
    /*10*/ "SHOUEi System",
    "Starfish",
    "Gremlin Graphics",
    "Electronic Arts",
    "NCS / Masaya",
    "COBRA Team",
    "Human/Field",
    "KOEI",
    /*18*/ "Hudson Soft",
    "Game Village",
    "Yanoman",
    UNK,
    "Tecmo",
    UNK,
    "Open System",
    "Virgin Games",
    /*20*/ "KSS",
    "Sunsoft",
    "POW",
    "Micro World",
    UNK,
    UNK,
    "Enix",
    "Loriciel/Electro Brain",
    /*28*/ "Kemco",
    "Seta Co.,Ltd.",
    "Culture Brain",
    "Irem Japan",
    "Pal Soft",
    "Visit Co.,Ltd.",
    "INTEC Inc.",
    "System Sacom Corp.",
    /*30*/ "Viacom New Media",
    "Carrozzeria",
    "Dynamic",
    "Nintendo",
    "Magifact",
    "Hect",
    UNK,
    UNK,
    /*38*/ "Capcom Europe",
    "Accolade Europe",
    UNK,
    "Arcade Zone",
    "Empire Software",
    "Loriciel",
    "Gremlin Graphics",
    UNK,
    /*40*/ "Seika Corp.",
    "UBI Soft",
    UNK,
    UNK,
    "LifeFitness Exertainment",
    UNK,
    "System 3",
    "Spectrum Holobyte",
    /*48*/ UNK,
    "Irem",
    UNK,
    "Raya Systems/Sculptured Software",
    "Renovation Products",
    "Malibu Games/Black Pearl",
    UNK,
    "U.S. Gold",
    /*50*/ "Absolute Entertainment",
    "Acclaim",
    "Activision",
    "American Sammy",
    "GameTek",
    "Hi Tech Expressions",
    "LJN Toys",
    UNK,
    /*58*/ UNK,
    UNK,
    "Mindscape",
    "Romstar, Inc.",
    UNK,
    "Tradewest",
    UNK,
    "American Softworks Corp.",
    /*60*/ "Titus",
    "Virgin Interactive Entertainment",
    "Maxis",
    "Origin/FCI/Pony Canyon",
    UNK,
    UNK,
    UNK,
    "Ocean",
    /*68*/ UNK,
    "Electronic Arts",
    UNK,
    "Laser Beam",
    UNK,
    UNK,
    "Elite",
    "Electro Brain",
    /*70*/ "Infogrames",
    "Interplay",
    "LucasArts",
    "Parker Brothers",
    "Konami",
    "STORM",
    UNK,
    UNK,
    /*78*/ "THQ Software",
    "Accolade Inc.",
    "Triffix Entertainment",
    UNK,
    "Microprose",
    UNK,
    UNK,
    "Kemco",
    /*80*/ "Misawa",
    "Teichio",
    "Namco Ltd.",
    "Lozc",
    "Koei",
    UNK,
    "Tokuma Shoten Intermedia",
    "Tsukuda Original",
    /*88*/ "DATAM-Polystar",
    UNK,
    UNK,
    "Bullet-Proof Software",
    "Vic Tokai",
    UNK,
    "Character Soft",
    "I\'\'Max",
    /*90*/ "Takara",
    "CHUN Soft",
    "Video System Co., Ltd.",
    "BEC",
    UNK,
    "Varie",
    "Yonezawa / S'Pal Corp.",
    "Kaneco",
    /*98*/ UNK,
    "Pack in Video",
    "Nichibutsu",
    "TECMO",
    "Imagineer Co.",
    UNK,
    UNK,
    UNK,
    /*a0*/ "Telenet",
    "Hori",
    UNK,
    UNK,
    "Konami",
    "K.Amusement Leasing Co.",
    UNK,
    "Takara",
    /*a8*/ UNK,
    "Technos Jap.",
    "JVC",
    UNK,
    "Toei Animation",
    "Toho",
    UNK,
    "Namco Ltd.",
    /*b0*/ "Media Rings Corp.",
    "ASCII Co. Activison",
    "Bandai",
    UNK,
    "Enix America",
    UNK,
    "Halken",
    UNK,
    /*b8*/ UNK,
    UNK,
    "Culture Brain",
    "Sunsoft",
    "Toshiba EMI",
    "Sony Imagesoft",
    UNK,
    "Sammy",
    /*c0*/ "Taito",
    UNK,
    "Kemco",
    "Square",
    "Tokuma Soft",
    "Data East",
    "Tonkin House",
    UNK,
    /*c8*/ "KOEI",
    UNK,
    "Konami USA",
    "NTVIC",
    UNK,
    "Meldac",
    "Pony Canyon",
    "Sotsu Agency/Sunrise",
    /*d0*/ "Disco/Taito",
    "Sofel",
    "Quest Corp.",
    "Sigma",
    "Ask Kodansha Co., Ltd.",
    UNK,
    "Naxat",
    UNK,
    /*d8*/ "Capcom Co., Ltd.",
    "Banpresto",
    "Tomy",
    "Acclaim",
    UNK,
    "NCS",
    "Human Entertainment",
    "Altron",
    /*e0*/ "Jaleco",
    UNK,
    "Yutaka",
    UNK,
    "T&ESoft",
    "EPOCH Co.,Ltd.",
    UNK,
    "Athena",
    /*e8*/ "Asmik",
    "Natsume",
    "King Records",
    "Atlus",
    "Sony Music Entertainment",
    UNK,
    "IGS",
    UNK,
    /*f0*/ UNK,
    "Motown Software",
    "Left Field Entertainment",
    "Beam Software",
    "Tec Magik",
    UNK,
    UNK,
    UNK,
    /*f8*/ UNK,
    "Cybersoft",
    UNK,
    "Psygnosis",
    UNK,
    UNK,
    "Davidson",
    UNK,
};

// MACROS
#define HI_BYTE(n) (((int)n >> 8) & 0x00ff) // extracts the hi-byte of a word
#define LOW_BYTE(n) ((int)n & 0x00ff)       // extracts the low-byte of a word

#define HIGH(n) ((int)n << 8) // turn the char into high part of int

#pragma pack(1) // for bmp header to avoid data alignment

// STRUCTURES
typedef struct
{
    char extended[16];                  // extra information for header
                                        /*
                                        1	$00:FFB0	0x0081B0	2 bytes	Maker code
                                        2	$00:FFB2	0x0081B2	4 bytes	Game code
                                        3	$00:FFB6	0x0081B6	7 bytes	Fixed Value ($00)
                                        4	$00:FFBD	0x0081BD	1 byte	Expansion RAM size
                                        5	$00:FFBE	0x0081BE	1 byte	Special version
                                        6	$00:FFBF	0x0081BF	1 byte	Cartridge type
                                    */
    /*FFC0*/ char title[21];            // Name of the ROM, typically in ASCII, using spaces to pad the name to 21 bytes.
    /*FFD5*/ unsigned char romlayout;   // ROM layout, typically $20 for LoROM, or $21 for HiROM. Add $10 for FastROM.
    /*FFD6*/ unsigned char cardtype;    // Cartridge type, typically $00 for ROM only, or $02 for ROM with save-RAM.
    /*FFD7*/ unsigned char ROMsize;     // ROM size byte.
    /*FFD8*/ unsigned char SRAMsize;    // RAM size byte.
    /*FFD9*/ unsigned char countrycode; // Country code, which selects the video in the emulator. Values $00, $01, $0d use NTSC. Values in range $02..$0c use PAL. Other values are invalid.
    /*FFDA*/ unsigned char license;     // Licensee code. If this value is $33, then the ROM has an extended header with ID at $ffb2..$ffb5.
    /*FFDB*/ unsigned char version;     // Version number, typically $00.
    /*FFDC*/ unsigned short checksum_c; // Checksum complement, which is the bitwise-xor of the checksum and $ffff.
    /*FFDE*/ unsigned short checksum;   // SNES checksum, an unsigned 16-bit checksum of bytes.
    unsigned int unknow;                // Unknown.
    unsigned short cop_vecs;            // Table of interrupt vectors for native mode
    unsigned short brk_vecs;
    unsigned short abort_vecs;
    unsigned short nmi_vecs;
    unsigned short unused_vecs;
    unsigned short irq_vecs;
    unsigned int unknow1;     // Unknown.
    unsigned short ecop_vecs; // Table of interrupt vectors for emulation mode.
    unsigned short eunused_vecs;
    unsigned short eabort_vecs;
    unsigned short enmi_vecs;
    unsigned short ereset_vecs;
    unsigned short eirq_vecs;
} snes_header;

//// M A I N   V A R I A B L E S ////////////////////////////////////////////////
int quietmode     = 0; // 0 = not quiet, 1 = i can't say anything :P
int showheader    = 1; // 0 = don't display current header , 1 = display it
int fixcrc        = 0; // 1 = fix crc, 0 = don't fix it
int changetitle   = 0; // 1 = change game title, 0 = don't change it
int changecountry = 0; // 1 = change game country, 0 = don't change it
int changeromsize = 0; // 1 = change game rom size, 0 = don't change it
int nosram        = 0; // 1 = change default pvsneslib with no sram support

int rom_has_header = 0; // 1 = rom with extra 512 bytes header
snes_header snesheader; // content of all header
int rom_size     = 0;   // size of rom
int rom_is_lorom = 1;   // 1 = lorom, 0 = hiRom

//// F U N C T I O N S //////////////////////////////////////////////////////////
int char_to_int_conv(char id)
{
    int value;

    if (id == '1')
        value = 0x01;
    else if (id == '2')
        value = 0x02;
    else if (id == '3')
        value = 0x03;
    else if (id == '4')
        value = 0x04;
    else if (id == '5')
        value = 0x05;
    else if (id == '6')
        value = 0x06;
    else if (id == '7')
        value = 0x07;
    else if (id == '8')
        value = 0x08;
    else if (id == '9')
        value = 0x09;
    else if (id == 'A')
        value = 0x0a;
    else if (id == 'B')
        value = 0x0b;
    else if (id == 'C')
        value = 0x0c;
    else if (id == 'D')
        value = 0x0d;
    else if (id == 'E')
        value = 0x0e;
    else if (id == 'F')
        value = 0x0f;
    else
        value = 0x00;

    return value;
}

int find_addon_chip(snes_header *head)
{
    int supported_type = HAS_NONE;

    // Info mostly taken from http://snesemu.black-ship.net/misc/-from%20nsrt.edgeemu.com-chipinfo.htm
    switch (head->cardtype)
    {
    case 0x00:
    case 0x01:
    case 0x02:
        supported_type = HAS_NONE;
        break;

    case 0x03:
        supported_type = head->romlayout == 0x30 ? HAS_DSP4 : HAS_DSP1;
        break;

    case 0x04:
        supported_type = HAS_DSP1;
        break;

    case 0x05:
        if (head->romlayout == 0x20)
        {
            supported_type = HAS_DSP2;
        }
        /* DSP-3 is hard to detect. We exploit the fact that the only game has been manufactured by Bandai */
        else if ((head->romlayout == 0x30) && (head->license == 0xb2))
        {
            supported_type = HAS_DSP3;
        }
        else
        {
            supported_type = HAS_DSP1;
        }
        break;

    case 0x13: // Mario Chip 1
    case 0x14: // GSU-x
    case 0x15: // GSU-x
    case 0x1a: // GSU-1 (21 MHz at start)
        if (head->romlayout == 0x20)
            supported_type = HAS_SUPERFX;
        break;

    case 0x25:
        supported_type = HAS_OBC1;
        break;

    case 0x32: // needed by a Sample game (according to ZSNES)
    case 0x34:
    case 0x35:
        if (head->romlayout == 0x23)
            supported_type = HAS_SA1;
        break;

    case 0x43:
    case 0x45:
        if (head->romlayout == 0x32)
            supported_type = HAS_SDD1;
        break;

    case 0x55:
        if (head->romlayout == 0x35)
            supported_type = HAS_RTC;
        break;

    case 0xe3:
        supported_type = HAS_Z80GB;
        break;

    case 0xf3:
        supported_type = HAS_CX4;
        break;

    case 0xf5:
        if (head->romlayout == 0x30)
        {
            supported_type = HAS_ST018;
        }
        else if (head->romlayout == 0x3a)
        {
            supported_type = HAS_SPC7110;
        }
        break;

    case 0xf6:
        /* These Seta ST-01X chips have both 0x30 at 0x00ffd5,
         they only differ for the 'size' at 0x00ffd7 */
        if (head->ROMsize < 0x0a)
            supported_type = HAS_ST011;
        else
            supported_type = HAS_ST010;
        break;

    case 0xf9:
        if (head->romlayout == 0x3a)
            supported_type = HAS_SPC7110_RTC;
        break;

    default:
        supported_type = HAS_UNK;
        break;
    }
    return supported_type;
}

int show_header(char *filename, FILE *fp)
{
    char title[22], rom_id[5], company_id[2];
    int addr, i, company, has_ram = 0, has_sram = 0, valid_check = 0, add_on_chip = 0;

    // calculate offset of header and display it
    addr = 0x7fb0 + rom_has_header * 512;
    fseek(fp, addr, SEEK_SET);

    // read the header
    if (fread(&snesheader, sizeof(snes_header), 1, fp) < 1)
    {
        printf("snestools: error 'Header (loRom) of file [%s] is not correct'\n", filename);
        exit(EXIT_FAILURE);
    }

    // a valid checksum is the biggest indicator of a valid header to check lo or Hi rom.
    if ((snesheader.checksum + snesheader.checksum_c) == 0xffff && (snesheader.checksum != 0) && (snesheader.checksum_c != 0))
        valid_check = 1;
    if (!valid_check)
    {
        // calculate offset of header for a possible hirom
        addr = 0xffb0 + rom_has_header * 512;
        fseek(fp, addr, SEEK_SET);

        // read the header
        if (fread(&snesheader, sizeof(snes_header), 1, fp) < 1)
        {
            printf("snestools: error 'Header (hiRom) of file [%s] is not correct'\n", filename);
            exit(EXIT_FAILURE);
        }
        if ((snesheader.checksum + snesheader.checksum_c) == 0xffff && (snesheader.checksum != 0) && (snesheader.checksum_c != 0))
            valid_check = 1;
        if (!valid_check)
        {
            printf("snestools: error 'No valid Header (hiRom or loRom) for file [%s]'\n", filename);
            exit(EXIT_FAILURE);
        }
    }

    // Company
    for (i = 0; i < 2; i++)
        company_id[i] = snesheader.extended[0 + i];
    company = (char_to_int_conv(company_id[0]) << 4) + char_to_int_conv(company_id[1]);
    if (company == 0)
        company = snesheader.license;

    // ROM ID
    for (i = 0; i < 4; i++)
        rom_id[i] = snesheader.extended[2 + i];
    rom_id[4] = '\0';

    /* Title */
    for (i = 0; i < 21; i++)
        title[i] = snesheader.title[0 + i];
    title[21] = '\0';

    // RAM
    if (((snesheader.cardtype & 0xf) == 1) || ((snesheader.cardtype & 0xf) == 2) || ((snesheader.cardtype & 0xf) == 4) || ((snesheader.cardtype & 0xf) == 5))
        has_ram = 1;

    // SRAM
    if (((snesheader.cardtype & 0xf) == 2) || ((snesheader.cardtype & 0xf) == 5) || ((snesheader.cardtype & 0xf) == 6))
        has_sram = 1;

    // chip
    add_on_chip = find_addon_chip(&snesheader);

    // lo/hi rom
    rom_is_lorom = snesheader.romlayout & 0x01 ? 0 : 1;

    // Display only if needed
    if (showheader)
    {
        printf("\nGeneric Information");
        printf("\n-------------------");
        printf("\nGame title    : %s", title);
        printf("\nROM ID        : %.4s", snesheader.license == 0x33 ? rom_id : "None");
        printf("\nCompany       : %s [%02X]", companies[company], company_id);
        printf("\nLicense       : %s [%02X]", companies[snesheader.license], snesheader.license);
        printf("\nCountry:      : %s [%02X]", countries[snesheader.countrycode], snesheader.countrycode);
        printf("\nVersion       : 1.%d", snesheader.version);
        if (snesheader.license == 0x33)
            printf("\nPromo Version : %d", snesheader.extended[14]);
        else
            printf("\nPromo Version : None");
        printf("\nHeader Type   : %s", snesheader.license == 0x33 ? "Newer Extended Header" : "Old Header (not extended)");
        printf("\nChecksum      : %04X", snesheader.checksum);
        printf("\nComp Checksum : %04X", snesheader.checksum_c);

        printf("\n\nHardware Information");
        printf("\n--------------------");
        printf("\nROM Size      : %dK [%dK file]", memory_amount[snesheader.ROMsize], rom_size / 1024);
        printf("\nROM Speed     : %s", snesheader.romlayout & 0x10 ? "Fast (3.58MHz)" : "Slow (2.68MHz)");
        printf("\nBank size:    : %s", rom_is_lorom ? "LoROM" : "HiROM");
        printf("\nChipset       : %s", types[add_on_chip]);
        if ((snesheader.countrycode == 0x00) || (snesheader.countrycode == 0x01) || (snesheader.countrycode == 0x0D))
            printf("\nVideo Type    : NTSC (60Hz)");
        else
            printf("\nVideo Type    : PAL (50Hz)");
        printf("\nSRAM Size     : %dK [%02X]", memory_amount[snesheader.SRAMsize], snesheader.SRAMsize);
        printf("\nExp.RAM Size  : %dK", snesheader.license == 0x33 ? snesheader.extended[13] : 0);
        printf("\nBattery       : %s", has_sram ? "Yes" : "No");
        printf("\nNMI Address   : %04Xh (native) %04Xh (emul.)", snesheader.nmi_vecs, snesheader.enmi_vecs);
        printf("\nStart Address : %04Xh (native)", snesheader.ereset_vecs);
        printf("\n");
    }

    return -1;
}

int skip_header(char *filename, FILE *fp)
{
    unsigned char header[16];

    // Get rom size
    fseek(fp, 0, SEEK_END);
    rom_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Check for a header (512 bytes)
    if (fread(&header, sizeof(header), 1, fp) < 1)
    {
        errorcontinue("File [%s] has no correct header", filename);
        return 0;
    }

    if ((header[8] == 0xaa) && (header[9] == 0xbb) && (header[10] == 0x04))
    {
        // Found an SWC identifier
        if (quietmode == 0)
            info("File [%s] has header (SWC)", filename);
        rom_has_header = 1;
    }
    else if ((header[0] | (header[1] << 8)) == (((rom_size - 512) / 1024) / 8))
    { // 2621440
        // Some headers have the rom size at the start, if this matches with the actual rom size, we probably have a header
        if (quietmode == 0)
            info("File [%s] has header (size)", filename);
        rom_has_header = 1;
    }
    else if ((rom_size % 0x8000) == 512)
    {
        // As a last check we'll see if there's exactly 512 bytes extra to this image.
        if (quietmode == 0)
            info("File [%s] has header (extra)", filename);
        rom_has_header = 1;
    }
    else
    {
        // No header found so go back to the start of the file
        if (quietmode == 0)
            info("File [%s] has no header", filename);
        rom_has_header = 0;
    }

    return -1;
}

int change_checksum(char *filename, FILE *fp)
{
    unsigned short sum_crc, sum_crc_comp;
    int i, addr, c;

    // Seek the header
    fseek(fp, 512 * rom_has_header, SEEK_SET);

    // Compute address of checksum entry.
    addr = rom_is_lorom ? LOROM_HEADER + OFFSET_CHECKSUM : HIROM_HEADER + OFFSET_CHECKSUM;

    // Checksum everything up to the checksum entry.
    sum_crc = 0;
    for (i = 0; i < addr; ++i)
    {
        if ((c = fgetc(fp)) == EOF)
        {
            fatal("Unexpected end of file [%s] af offset %x", filename, i);
        }
        sum_crc += (unsigned char)c;
    }

    // Seek past the checksum fields.
    fseek(fp, 4, SEEK_CUR);

    // 2*255 is for the checksum and its complement bytes that we skipped earlier
    sum_crc += 0x1fe;

    // Finish computing the checksum.
    while ((c = fgetc(fp)) != EOF)
    {
        sum_crc += (unsigned char)c;
    }

    if (quietmode == 0)
        info("Change Checksum to %04Xh and Checksum complement %04Xh...", sum_crc, sum_crc ^ 0xffff);

    // Seek back to the checksum entry.
    fseek(fp, addr, SEEK_SET);

    // Rewrite the checksum complement entry.
    sum_crc_comp = sum_crc ^ 0xffff;
    fputc(sum_crc_comp & 0xff, fp);
    fputc((sum_crc_comp >> 8) & 0xff, fp);

    // Rewrite the checksum entry.
    fputc(sum_crc & 0xff, fp);
    fputc((sum_crc >> 8) & 0xff, fp);

    return -1;
}

void strtoupper(char *str)
{
    for (; *str; str++)
        *str = toupper(*str);
}

int change_title(char *filename, FILE *fp, char *title)
{
    int i, addr;

    if (quietmode == 0)
        info("Change title to [%s]...", title);

    // Compute address of title entry.
    addr = 512 * rom_has_header + rom_is_lorom ? LOROM_HEADER + OFFSET_TITLE : HIROM_HEADER + OFFSET_TITLE;

    // Go to title and change it
    fseek(fp, addr, SEEK_SET);
    for (i = 0; i < 21; i++)
        fputc(title[i], fp);

    // compute crc again to avoid problems
    change_checksum(filename, fp);

    return -1;
}

int change_country(char *filename, FILE *fp, char *country)
{
    int cntry = 0, addr;

    if (quietmode == 0)
        info("Change country to [%s]...", country);

    // Compute address of country entry.
    addr = 512 * rom_has_header + rom_is_lorom ? LOROM_HEADER + OFFSET_COUNTRY : HIROM_HEADER + OFFSET_COUNTRY;

    // Go to country and change it
    if ((country[0] >= '0') && (country[0] <= '9'))
        cntry = (country[0] - '0') * 10;
    else if ((country[0] >= 'A') && (country[0] <= 'A'))
        cntry = (country[0] - 'A') * 10 + 10;
    if ((country[1] >= '0') && (country[1] <= '9'))
        cntry += (country[1] - '0');
    else if ((country[1] >= 'A') && (country[1] <= 'A'))
        cntry += (country[1] - 'A') + 10;
    fseek(fp, addr, SEEK_SET);
    fputc(cntry, fp);

    // compute crc again to avoid problems
    change_checksum(filename, fp);

    return -1;
}

int change_romsize(char *filename, FILE *fp, char *romsize)
{
    int romsiz = 0, addr;

    if (quietmode == 0)
        info("Change rom size to [%s]...", romsize);

    // Compute address of country entry.
    addr = 512 * rom_has_header + rom_is_lorom ? LOROM_HEADER + OFFSET_ROMSIZE : HIROM_HEADER + OFFSET_ROMSIZE;

    // Go to romsize and change it
    // possible values
    // $08	2 Megabits
    // $09	4 Megabits
    // $0A	8 Megabits
    // $0B	16 Megabits
    // $0C	32 Megabits
    if ((romsize[1] >= '0') && (romsize[1] <= '9'))
        romsiz += (romsize[1] - '0');
    else if ((romsize[1] >= 'A') && (romsize[1] <= 'C'))
        romsiz += (romsize[1] - 'A') + 10;
    fseek(fp, addr, SEEK_SET);
    fputc(romsiz, fp);

    // compute crc again to avoid problems
    change_checksum(filename, fp);

    return -1;
}

int change_sram(char *filename, FILE *fp, char nosram)
{
    int addr;

    if (quietmode == 0)
        info("Change sram to [%s]...", nosram ? "NOSRAM" : "SRAM");

    // Compute address of title entry and Go to sram and change it
    addr = 512 * rom_has_header + rom_is_lorom ? LOROM_HEADER + OFFSET_SRAM : HIROM_HEADER + OFFSET_SRAM;
    fseek(fp, addr, SEEK_SET);
    if (nosram == 1)
        fputc(0, fp);

    // Compute address of title entry.
    addr = 512 * rom_has_header + rom_is_lorom ? LOROM_HEADER + OFFSET_CARDTYPE : HIROM_HEADER + OFFSET_CARDTYPE;
    fseek(fp, addr, SEEK_SET);
    if (nosram == 1)
        fputc(0, fp);

    // compute crc again to avoid problems
    change_checksum(filename, fp);

    return -1;
}

//-------------------------------------------------------------------------------------------------
void PrintOptions(char *str)
{
    printf("Usage: snestools.exe [options] sfc/smc filename ...\n");
    printf("  where filename is a SNES rom file\n");

    if (str[0] != 0)
        printf("\nsnestools: error 'The [%s] parameter is not recognized'", str);

    printf("\n\nHeader options:");
    printf("\n-hi!              Don't show current filename header information.");
    printf("\n-hf               Fix hearder CRC.");
    printf("\n-ht[text]         Change game title.");
    printf("\n-hc[country]      Change country (00 for NTSC,01 for PAL).");
    printf("\n-hr[romsize]      Change romsize (08 for 256K, 0B for 2MB, ROM size = (1 << value) KiB).");
    printf("\n-hS!              No SRAM (default is on in pvsneslib).");
    printf("\n\nMisc options:");
    printf("\n-h                Display this information");
    printf("\n-q                Quiet mode");
    printf("\n-v                Display version information");
    printf("\n");
}

//-------------------------------------------------------------------------------------------------
void PrintVersion(void)
{
    printf("snestools (" SNESTOOLSDATE ") version " SNESTOOLSVERSION "");
    printf("\nCopyright (c) 2012-2024 Alekmaul");
}

//-------------------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
    FILE *fp;

    char filebase[256];
    char programtitle[256];
    char country[3]; // new country
    char romsz[3];   // new romsize

    int i, j;

    // Init vars
    for (i = 0; i < 21; i++)
        programtitle[i] = ' ';
    programtitle[21] = '\0';
    filebase[0]      = '\0';

    // parse the arguments
    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            if (argv[i][1] == 'v') // show version
            {
                PrintVersion();
                exit(0);
            }
            else if (argv[i][1] == 'q') // quiet mode
            {
                quietmode = 1;
            }
            else if (argv[i][1] == 'h') // header options
            {
                if (strcmp(&argv[i][1], "hi!") == 0)
                {
                    showheader = 0;
                }
                else if (strcmp(&argv[i][1], "hf") == 0)
                {
                    if (!changetitle)
                        fixcrc = 1; // because change_title does it too
                }
                else if (argv[i][2] == 'c') // country
                {
                    changecountry = 1;
                    fixcrc        = 0; // because we will do it
                    strncpy(country, &argv[i][3], sizeof(country) - 1);
                    country[sizeof(country) - 1] = '\0';
                    if (strlen(country) != 2)
                    {
                        PrintOptions(argv[i]);
                        return 1;
                    }
                }
                else if (argv[i][2] == 'r') // romsize
                {
                    changeromsize = 1;
                    fixcrc        = 0; // because we will do it
                    strncpy(romsz, &argv[i][3], sizeof(romsz) - 1);
                    romsz[sizeof(romsz) - 1] = '\0';
                    if (strlen(romsz) != 2)
                    {
                        PrintOptions(argv[i]);
                        return 1;
                    }
                }
                else if (argv[i][2] == 't') // program title
                {
                    strncpy(programtitle, &argv[i][3], sizeof(programtitle) - 1);
                    programtitle[sizeof(programtitle) - 1] = '\0';
                    strtoupper(programtitle);
                    if (strlen(programtitle))
                    {
                        for (j = strlen(programtitle); j < 21; j++)
                            programtitle[j] = ' ';
                        programtitle[21] = '\0'; // to avoid problems
                    }
                    changetitle = 1;
                    fixcrc      = 0; // because we will do it
                }
                else if (strcmp(&argv[i][1], "hS!") == 0)
                {
                    nosram = 1;
                    fixcrc = 0; // because we will do it
                }
                else
                {
                    PrintOptions(argv[i]);
                    exit(1);
                }
            }
            else // invalid option
            {
                PrintOptions(argv[i]);
                exit(1);
            }
        }
        else
        {
            // its not an option flag, so it must be the filebase
            if (filebase[0] != 0) // if already defined... there's a problem
            {
                PrintOptions(argv[i]);
                exit(1);
            }
            else
            {
                strncpy(filebase, argv[i], sizeof(filebase) - 1);
                filebase[sizeof(filebase) - 1] = '\0';
            }
        }
    }

    // make sure options are valid
    if (filebase[0] == 0)
    {
        fatal("You must specify a snes filename");
    }
    if ((changetitle) && (strlen(programtitle) == 0))
    {
        fatal("You must specify a game title to change it");
    }

    // open the file
    fp = fopen(filebase, "rb+");
    if (fp == NULL)
    {
        fatal("Can't open file [%s]", filebase);
    }

    // check for an header and skip it
    if (!skip_header(filebase, fp))
    {
        fclose(fp);
        exit(1);
    }

    // display header if needed in this function
    if (!show_header(filebase, fp))
    {
        fclose(fp);
        exit(1);
    }

    // if needed, change some game information
    if (changetitle)
    {
        if (!change_title(filebase, fp, programtitle))
        {
            fclose(fp);
            exit(1);
        }
    }
    if (changecountry)
    {
        if (!change_country(filebase, fp, country))
        {
            fclose(fp);
            exit(1);
        }
    }
    if (changeromsize)
    {
        if (!change_romsize(filebase, fp, romsz))
        {
            fclose(fp);
            exit(1);
        }
    }
    if (nosram)
    {
        if (!change_sram(filebase, fp, 1))
        {
            fclose(fp);
            exit(1);
        }
    }

    // if needed, recalc checksum
    if (fixcrc)
    {
        if (!change_checksum(filebase, fp))
        {
            fclose(fp);
            exit(1);
        }
    }

    // close the file
    fclose(fp);

    if (quietmode == 0)
        info("Done!");

    return (EXIT_SUCCESS);
}
