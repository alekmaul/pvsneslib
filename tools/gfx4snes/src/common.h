#ifndef _GFX4SNES_COMMON_H
#define _GFX4SNES_COMMON_H

#define HI_BYTE(n) (((int) (n) >> 8) & 0x00ff)                 // extracts the hi-byte of a word
#define LOW_BYTE(n) ((int) (n) & 0x00ff)                       // extracts the low-byte of a word

#define WRITEFILEBYTE(data, fileptr) {	putc(data,fileptr);  }
#define WRITEFILEWORD(data, fileptr) {	putc(LOW_BYTE(data),fileptr); putc(HI_BYTE(data),fileptr); }

#endif

