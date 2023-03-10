/*---------------------------------------------------------------------------------

    Copyright (C) 2012-2017
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

    Image converter for snes.
    Parts from pcx2snes from Neviksti
    palette rounded option from Artemio Urbina
  BMP BI_RLE8 compression support by Andrey Beletsky

***************************************************************************/
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

#include "loadimg.h"

#include "lodepng.h"

int PCX_Load(char *filename, pcx_picture_ptr image)
{
    // this function loads a pcx file into a picture structure, the actual image
    // data for the pcx file is decompressed and expanded into a secondary buffer
    // within the picture structure, the separate images can be grabbed from this
    // buffer later.  also the header and palette are loaded

    long num_bytes, index;
    long count;
    long x, y;
    unsigned char data;
    pcx_header *header;
    FILE *fp;

    // open the file
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("\ngfx2snes: error 'Can't open file [%s]'", filename);
        return 0;
    }

    // load the header
    header = &image->header;
    fread(header, 1, 128, fp);

    header->width++;
    header->height++;

    // check to make sure this is a 256 color PCX
    if ((header->manufacturer != 10) ||
        (header->encoding != 1) ||
        (header->bits_per_pixel != 8) ||
        (header->num_color_planes != 1) ||
        (header->palette_type != 1))
    {
        printf("\ngfx2snes: error 'File [%s] is not recognized as a 256 color PCX'", filename);
        fclose(fp);
        return 0;
    }

    // allocate memory for the picture + 64 empty lines
    image->buffer = malloc((size_t)(header->height + 64) * header->width);
    if (image->buffer == NULL)
    {
        printf("\ngfx2snes: error 'Can't allocate enough memory for the picture'");
        fclose(fp);
        return 0;
    }

    // initially clear the memory (to make those extra lines be blank)
    memset(image->buffer, 0, (size_t)(header->height + 64) * header->width);

    // load the data and decompress into buffer
    count = 0;
    for (y = 0; y < header->height; y++)
    {
        for (x = 0; x < header->width; x++)
        {
            // get the first piece of data
            data = getc(fp);

            // is this a rle?
            // if ( (data>=192) && (data<=255))
            if (data >= 192)
            {
                // how many bytes in run?
                num_bytes = data - 192;
                x += (num_bytes - 1);

                // get the actual data for the run
                data = getc(fp);

                // replicate data in buffer num_bytes times
                while (num_bytes-- > 0)
                    image->buffer[count++] = data;

            } // end if rle
            else
            {
                // actual data, just copy it into buffer at next location
                image->buffer[count++] = data;
            } // end else not rle

        } // end of x loop

        // get rid of the padding byte if there is one
        if (x < header->bytes_per_line)
            data = getc(fp);

    } // end of y loop

    // Get the Palette header (one byte, should equal 12)
    data = getc(fp);
    if (data != 12)
    {
        printf("\ngfx2snes: error 'Couldn't find palette header [%s]'", filename);
        free(image->buffer);
        fclose(fp);

        return 0;
    }

    // get the palette data
    for (index = 0; index < 256; index++)
    {
        image->palette[index].red = (getc(fp) >> 2);
        image->palette[index].green = (getc(fp) >> 2);
        image->palette[index].blue = (getc(fp) >> 2);
    }

    // check to make sure there weren't errors while reading the file
    if (ferror(fp))
    {
        printf("\ngfx2snes: error 'Error reading file [%s]'", filename);
        free(image->buffer);
        fclose(fp);
        return 0;
    }

    fclose(fp);

    return -1;

} // end PCX_Load

void BMP_BI_RLE8_Load(pcx_picture_ptr image,
                      const bmp_header *const bmphead, const bmp_info_header *const bmpinfohead,
                      FILE *fp)
{
    // BI_RLE8 decompress according to:
    // https://technet.microsoft.com/ru-ru/dd183383

    unsigned long line, i, count;
    // offset in image buffer where current line starts
    unsigned int pos;
    unsigned char ch, ch2;

    // start from bottom line
    line = bmpinfohead->biHeight;
    pos = (line - 1) * bmpinfohead->biWidth;

    count = 0;
    // read all image bytes
    while (count < bmpinfohead->biSizeImage)
    {
        ch = getc(fp);
        ++count;
        if (ch)
        {
            // repeat byte
            ch2 = getc(fp);
            ++count;
            for (i = 0; i < ch; ++i)
                image->buffer[pos++] = ch2;
            continue;
        }

        // escape char
        ch = getc(fp);
        ++count;
        if (ch == 0)
        {
            // End of line.

            // go one line up
            --line;
            // start of this line.
            pos = (line - 1) * bmpinfohead->biWidth;
        }
        else if (ch == 1)
        {
            // End of bitmap.
            break;
        }
        else if (ch == 2)
        {
            // Delta.
            // The two bytes following the escape contain unsigned values
            // indicating the horizontal and vertical offsets of the next pixel
            // from the current position.

            ch = getc(fp);
            ++count;
            // go right in the buffer
            pos += ch;

            ch = getc(fp);
            ++count;
            // go given lines up
            line -= ch;
            pos -= bmpinfohead->biWidth * ch;
        }
        else
        {
            // Absolute mode.
            // The second byte represents the number of bytes that follow,
            // each of which contains the color index of a single pixel.
            ch = getc(fp);
            ++count;
            for (i = 0; i < ch; ++i)
            {
                image->buffer[pos++] = getc(fp);
                ++count;
            }
            if (i % 2)
            {
                // Each run must be aligned on a word boundary.
                // Read and throw away the placeholder.
                ch2 = getc(fp);
                ++count;
            }
        }
    }
} // end BMP_BI_RLE8_Load

int BMP_Load(char *filename, pcx_picture_ptr image)
{
    // this function loads a bmp file into a picture structure, the actual image
    // data for the bmp file is decompressed and expanded into a secondary buffer
    // within the picture structure, the separate images can be grabbed from this
    // buffer later.  also the header and palette are loaded
    FILE *fp;
    int index, i;
    pcx_header *header;
    bmp_header bmphead;
    bmp_info_header bmpinfohead;

    // open the file
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("\ngfx2snes: error 'Can't open file [%s]'", filename);
        return 0;
    }

    // check to see if it is a valid bitmap file
    if (fread(&bmphead, sizeof(bmp_header), 1, fp) < 1)
    {
        printf("\ngfx2snes: error 'File [%s] has no correct BMP header'", filename);
        fclose(fp);
        return 0;
    }

    if (bmphead.bfType != BF_TYPE)
    {
        printf("\ngfx2snes: error 'File [%s] is not recognized as a BMP file'", filename);
        fclose(fp);
        return 0;
    }

    // check to see if it is a valid bitmap file
    if (fread(&bmpinfohead, sizeof(bmpinfohead), 1, fp) < 1)
    {
        printf("\ngfx2snes: error 'File [%s] has no correct BMP info header'", filename);
        fclose(fp);
        return 0;
    }

    if (bmpinfohead.biBitCount != 8 || (bmpinfohead.biCompression != 0 && bmpinfohead.biCompression != 1 /*BI_RLE8*/))
    {
        printf("\ngfx2snes: error 'File [%s] is not a valid BMP file: 256 colors, non-compressed or BI_RLE8 supported'", filename);
        fclose(fp);
        return 0;
    }

    // seek to palette
    fseek(fp, sizeof(bmp_header) + bmpinfohead.biSize, 0);

    // initially clear the palette if there are less then 256 colors in the file
    memset(image->palette, 0, (size_t)(256 * sizeof(RGB_color)));

    // read the palette information
    for (index = 0; index < 256; index++)
    {
        image->palette[index].blue = getc(fp) >> 2;
        image->palette[index].green = getc(fp) >> 2;
        image->palette[index].red = getc(fp) >> 2;
        // data=getc(fp);
        getc(fp);
    }

    header = &image->header;
    header->width = bmpinfohead.biWidth;
    header->height = bmpinfohead.biHeight;

    // allocate memory for the picture + 64 empty lines
    image->buffer = malloc((size_t)(header->height + 64) * header->width);
    if (image->buffer == NULL)
    {
        printf("\ngfx2snes: error 'Can't allocate enough memory for the picture'");
        fclose(fp);
        return 0;
    }

    // initially clear the memory (to make those extra lines be blank)
    memset(image->buffer, 0, (size_t)(header->height + 64) * header->width);

    // seek to image data
    fseek(fp, bmphead.bfOffBits, 0);

    // read the bitmap
    if (bmpinfohead.biCompression == 0)
    {
        for (index = (header->height - 1) * header->width; index >= 0; index -= header->width)
            for (i = 0; i < header->width; i++)
                image->buffer[index + i] = getc(fp);
    }
    else if (bmpinfohead.biCompression == 1)
    {
        // BI_RLE8
        BMP_BI_RLE8_Load(image, &bmphead, &bmpinfohead, fp);
    }

    // check to make sure there weren't errors while reading the file
    if (ferror(fp))
    {
        printf("\ngfx2snes: error 'Error reading file [%s]'", filename);
        free(image->buffer);
        fclose(fp);
        return 0;
    }

    fclose(fp);

    return -1;
} // end BMP_Load

int TGA_Load(char *filename, pcx_picture_ptr image)
{
    // this function loads a tga file into a picture structure, the actual image
    // data for the bmp file is decompressed and expanded into a secondary buffer
    // within the picture structure, the separate images can be grabbed from this
    // buffer later.  also the header and palette are loaded
    FILE *fp;
    unsigned long index, i;
    tga_header tgahead;
    pcx_header *header;

    // open the file
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("\ngfx2snes: error 'Can't open file [%s]'", filename);
        return 0;
    }

    // check to see if it is a valid bitmap file
    if (fread(&tgahead, sizeof(tga_header), 1, fp) < 1)
    {
        printf("\ngfx2snes: error 'File [%s] has no correct TGA header'", filename);
        fclose(fp);
        return 0;
    }

    // check to make sure there weren't errors while reading the file
    if (ferror(fp))
    {
        printf("\ngfx2snes: error 'Error reading file [%s]'", filename);
        free(image->buffer);
        fclose(fp);
        return 0;
    }

    if (tgahead.BPP != 8 || tgahead.ImageType != 1)
    {
        printf("\ngfx2snes: error 'File [%s] is not a valid indexed 256 colors TGA file'", filename);
        fclose(fp);
        return 0;
    }

    header = &image->header;
    header->width = tgahead.Width;
    header->height = tgahead.Height;

    // allocate memory for the picture + 64 empty lines
    image->buffer = malloc((size_t)(header->height + 64) * header->width);
    if (image->buffer == NULL)
    {
        printf("\ngfx2snes: error 'Can't allocate enough memory for the picture'");
        fclose(fp);
        return 0;
    }

    // initially clear the memory (to make those extra lines be blank)
    memset(image->buffer, 0, (size_t)(header->height + 64) * header->width);

    // read the palette information
    for (index = 0; index < 256; index++)
    {
        image->palette[index].blue = getc(fp) >> 2;
        image->palette[index].green = getc(fp) >> 2;
        image->palette[index].red = getc(fp) >> 2;
    }

    // read the bitmap
    for (index = (header->height - 1) * header->width; index >= 0; index -= header->width)
        for (i = 0; i < header->width; i++)
            image->buffer[index + i] = getc(fp);

    fclose(fp);

    return -1;
} // end TGA_Load

int PNG_Load(char *filename, pcx_picture_ptr image)
{
    unsigned error, index, i, sz, bpp;
    unsigned char *pngimage;
    unsigned char *png = 0;
    size_t pngsize;
    LodePNGState state;
    size_t width, height; // , wal,hal;
    pcx_header *header;

    /*optionally customize the state*/
    lodepng_state_init(&state);

    // no conversion of color (to keep palette mode)
    state.decoder.color_convert = 0;

    error = lodepng_load_file(&png, &pngsize, filename);
    if (!error)
    {
        error = lodepng_decode(&pngimage, &width, &height, &state, png, pngsize);
    }
    if (error)
    {
        printf("\ngfx2snes: error 'Decoder error %u: %s'\n", error, lodepng_error_text(error));
        free(png);
        lodepng_state_cleanup(&state);
        free(pngimage);
        return 0;
    }

    bpp = state.info_raw.bitdepth;
    // 130521 TOTO if ( (bpp  != 4) && (bpp != 8)) {
    if (bpp != 8)
    {
        printf("\ngfx2snes: error 'File [%s] is not a valid bbp value (%d bpp)'", filename, bpp);
        free(png);
        lodepng_state_cleanup(&state);
        free(pngimage);
        return 0;
    }

    if (state.info_raw.colortype != LCT_PALETTE)
    {
        printf("\ngfx2snes: error 'File [%s] is not a valid indexed palette mode (mode %d)'", filename, state.info_raw.colortype);
        free(png);
        lodepng_state_cleanup(&state);
        free(pngimage);
        return 0;
    }

    // read the palette information
    sz = state.info_png.color.palettesize;
    for (index = 0; index < sz; index++)
    {
        image->palette[index].red = state.info_png.color.palette[(index * 4) + 0] >> 2;
        image->palette[index].green = state.info_png.color.palette[(index * 4) + 1] >> 2;
        image->palette[index].blue = state.info_png.color.palette[(index * 4) + 2] >> 2;
    }

    // get png information
    header = &image->header;
    header->width = width;
    header->height = height;

    // allocate memory for the picture + 64 empty lines
    image->buffer = malloc((size_t)(header->height + 64) * header->width);
    if (image->buffer == NULL)
    {
        printf("\ngfx2snes: error 'Can't allocate enough memory for the picture'");
        return 0;
    }

    // initially clear the memory (to make those extra lines be blank)
    memset(image->buffer, 0, (size_t)(header->height + 64) * header->width);

    // 4 bpps conversion
    /*130521 TOTO
        if (bpp==4) {

            for (index = 0; index < header->height; index++) {
                for(i=0;i<header->width;i++)
                    image->buffer[index+i] = pngimage[i +index*header->height];
            }
          // get buffer size
                *size = (wAligned / 2) * hAligned;
                // and alloc
                result = malloc(*size);

                srcPix = 0;
                for (i = 0; i < h; i++)
                {
                    unsigned char *dst = &result[i * (wAligned / 2)];

                    memset(dst, 0, wAligned / 2);

                    for (j = 0; j < w; j++)
                    {
                            unsigned char v;

                            if (srcPix & 1) v = (out[srcPix / 2] >> 4) & 0xF;
                            else v =  (out[srcPix / 2] >> 0) & 0xF;
                            srcPix++;

                            if (j & 1) dst[j / 2] = (dst[j / 2] & 0x0F) | (v << 4);
                            else dst[j / 2] = (dst[j / 2] & 0xF0) | (v << 0);
                    }
                }
                for(;i < hAligned; i++)
                    memset(&result[i * (wAligned / 2)], 0, wAligned / 2);
        }
        // 8 bpps conversion
        else {
    */
    for (index = 0; index < header->height; index++)
    {
        for (i = 0; i < header->width; i++)
        {
            image->buffer[i + (header->width * index)] = pngimage[i + (header->width * index)];
        }
    }
    //}

    free(png);

    lodepng_state_cleanup(&state);

    free(pngimage);

    return -1;
} // end PNG_Load
