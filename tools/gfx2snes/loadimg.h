#ifndef GFX2SNESIMG_H
#define GFX2SNESIMG_H
#pragma pack(push)
#pragma pack(1) // for bmp header to avoid data alignment

typedef struct RGB_color_typ
{
    unsigned char red;   // Red component if color 0-63
    unsigned char green; // Green component of color 0-63
    unsigned char blue;  // Blue component of color 0-63
} RGB_color, *RGB_color_ptr;

typedef struct pcx_header_typ
{
    char manufacturer;
    // Always 10.
    char version;
    // 0-Ver 2.5 Paintbrush, 2-Ver 2.8 with
    // palette, 3-Ver 2.8 use the default palette,
    // 5-Ver 3.0 or higher of Paintbrush
    char encoding;
    // Always 1, meaning RLE encoding.
    char bits_per_pixel;
    // Bits per pixel; in our case, eight
    short x, y;
    // Upper-left corner of the image
    short width, height;
    // Size of the image
    short horv_res;
    // Pixels in the x direction
    short vert_res;
    // Pixels in the y direction
    char ega_palette[48];
    // The EGA palette; we can ignore it
    char reserved;
    // Nothing
    char num_color_planes;
    // The number of planes in the image
    short bytes_per_line;
    // Bytes per one horizontal line
    short palette_type;
    // 1 = Color or B&W
    // 2 = Grayscale
    char padding[58];
    // Extra bytes for a rainy day
} pcx_header, *pcx_header_ptr;

typedef struct pcx_picture_typ
{
    pcx_header header;
    // The header
    RGB_color palette[256];
    // The VGA palette
    unsigned char *buffer;
    // The buffer to hold the image
} pcx_picture, *pcx_picture_ptr;

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

typedef struct tga_hearder_type
{
    unsigned char Offset;        // Usually 0,  add 18 to this value to find the start of the palette/image data.
    unsigned char ColorType;     // Image type. 0 = RGB, 1 = Indexed.
    unsigned char ImageType;     // 0 = None, 1 = Indexed, 2 = RGB, 3 = Greyscale, +8 = RLE encoded.
    unsigned short PaletteStart; // Start of palette.
    unsigned short PaletteLen;   // Number of palette entries.
    unsigned char PalBits;       // Bits per colour entry.
    unsigned short XOrigin;      // Image X Origin
    unsigned short YOrigin;      // Image Y Origin
    unsigned short Width;        // Image width (Pixels).
    unsigned short Height;       // Image height (Pixels)
    unsigned char BPP;           // Bits per pixel (8,16,24 or 32)
    unsigned char Orientation;   // If Bit 5 is set, the image will be upside down (like BMP)
} tga_header;

extern int PCX_Load(char *filename, pcx_picture_ptr image);
extern int BMP_Load(char *filename, pcx_picture_ptr image);
extern int TGA_Load(char *filename, pcx_picture_ptr image);
extern int PNG_Load(char *filename, pcx_picture_ptr image);
#pragma pack(pop)
#endif
