#ifndef _FNT4SNES_COMMON_H
#define _FNT4SNES_COMMON_H

#define HI_BYTE(n) (((int) (n) >> 8) & 0x00ff)                 // extracts the hi-byte of a word
#define LOW_BYTE(n) ((int) (n) & 0x00ff)                       // extracts the low-byte of a word

#define WRITEFILEBYTE(data, fileptr) {	putc(data,fileptr);  }
#define WRITEFILEWORD(data, fileptr) {	putc(LOW_BYTE(data),fileptr); putc(HI_BYTE(data),fileptr); }

/*
#define PALETTE_OFS                  10                                  // palette bit shifting
#define PRIORITY_OFS                13                                  // priority bit shifting

#define PALETTE_MASK            0x1C00                              // mask for palette on map
#define TILEIDX_MASK            0x01FF                              // mask for tile index on map

#define OBJ_PAL(pal) (pal << 1)                                      // sprite palette attribute 
#define OBJ_PRIO(prio) (prio << 4)                                  // sprite priority attribute
#define OBJ_FLIPX 0x40                                              // sprite flip x attribute 
#define OBJ_FLIPY 0x80                                              // sprite flip y attribute 
*/
#define IMG_WIDTH                   768
#define IMG_HEIGHT                  8

#define GLYPH_COUNT                 96
#define GLYPH_W                     8
#define GLYPH_H                     8
#define FIRST_ASCII                 0x20

//-------------------------------------------------------------------------------------------------
/*typedef struct {
    unsigned char data[MAX_TILE_BYTES];
    int     len;
} Tile;*/




#endif
