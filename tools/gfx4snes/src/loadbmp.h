#ifndef _GFX4SNES_LOADBMP_H
#define _GFX4SNES_LOADBMP_H

 typedef  enum
 {
   BI_RGB = 0x0000,
   BI_RLE8 = 0x0001,
   BI_RLE4 = 0x0002,
   BI_BITFIELDS = 0x0003,
   BI_JPEG = 0x0004,
   BI_PNG = 0x0005,
   BI_CMYK = 0x000B,
   BI_CMYKRLE8 = 0x000C,
   BI_CMYKRLE4 = 0x000D
 } Compression;

typedef struct bmp_header_typ /**** BMP file header structure ****/
{
    unsigned short bfType;      /* Magic number for file */
    unsigned int bfSize;        /* Size of file */
    unsigned short bfReserved1; /* Reserved */
    unsigned short bfReserved2; /* ... */
    unsigned int bfOffBits;     /* Offset to bitmap data */
} bmp_header;

#define BF_TYPE 0x4D42 /* "MB" */

typedef struct bmp_info_header_typ
{
    unsigned int biSize;         /* Size of info header */
    int biWidth;                 /* Width of image */
    int biHeight;                /* Height of image */
    unsigned short biPlanes;     /* Number of color planes */
    unsigned short biBitCount;   /* Number of bits per pixel */
    unsigned int biCompression;  /* Type of compression to use */
    unsigned int biSizeImage;    /* Size of image data */
    int biXPelsPerMeter;         /* X pixels per meter */
    int biYPelsPerMeter;         /* Y pixels per meter */
    unsigned int biClrUsed;      /* Number of colors used */
    unsigned int biClrImportant; /* Number of important colors */
} bmp_info_header;

typedef struct BMPcolor_typ
{
    unsigned char red;                                              // Red component if color 0-63
    unsigned char green;                                            // Green component of color 0-63
    unsigned char blue;                                             // Blue component of color 0-63
} BMPcolor;

typedef struct BMPInfo_typ
{
    unsigned compression_method;    // compression method of the original file. Always 0 or BI_RLE8.
    BMPcolor palette[256];          // a maximum of 256 colors
} BMPInfo;

/*The settings, state and information for extended encoding and decoding.*/
typedef struct BMPState_typ
{
  BMPInfo info_png;                         // info of the PNG image obtained after decoding
} BMPState;

extern const char* bmp_error_text(unsigned code);
extern unsigned bmp_load_file(unsigned char** out, size_t* outsize, const char* filename);
extern unsigned bmp_decode(unsigned char** out, BMPState* state, unsigned* w, unsigned* h, const unsigned char* in, size_t insize);

#endif
