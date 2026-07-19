#ifndef _FNT4SNES_IMAGES_H
#define _FNT4SNES_IMAGES_H

#include "common.h"

#include "errors.h"

#include "lodepng.h"

//-------------------------------------------------------------------------------------------------
typedef struct RGB_color_typ
{
    unsigned char r;                                                // Red component if color 0-63
    unsigned char g;                                                // Green component of color 0-63
    unsigned char b;                                                // Blue component of color 0-63
} t_RGB_color;

typedef struct header_typ
{
    unsigned int width, height;                                     // width & height of image
} t_header;

typedef struct pngbmp_image_typ
{
    t_header header;                                                // filetype specific header
    t_RGB_color palette[256];                                       // palette (max 256 colors)
    unsigned char *buffer;                                          // The buffer to hold the image
} t_image;

extern t_image snesimage;											// current image converted

//-------------------------------------------------------------------------------------------------
extern void image_load(const char *filename, t_image *img, bool isquiet);

#endif
