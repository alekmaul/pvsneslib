/*---------------------------------------------------------------------------------

	Copyright (C) 2012-2023
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

	Image converter for Super Nintendo.
	Parts from pcx2snes from Neviksti
	palette rounded option from Artemio Urbina
  BMP BI_RLE8 compression support by Andrey Beletsky
	
***************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "loadbmp.h"

// This returns the description of a numerical error code in English. This is also
//  the documentation of all the error codes.
const char* bmp_error_text(unsigned code)
{
    switch(code)
    {
    		case 0: return "no error, everything went ok";
    		case 1: return "nothing done yet"; 						// the Encoder/Decoder has done nothing yet, error checking makes no sense yet
    		case 25: return "only BI_RLE8 compression method is supported";
            case 28: return "incorrect PBMP signature, it's no BMP or corrupted";
    		case 38: return "the palette is not 16 or 256 colors"; 
			case 78: return "failed to open file for reading"; /*file doesn't exist or couldn't be opened for reading*/
    		case 83: return "memory allocation failed";
#if 0

    case 10: return "end of input memory reached without huffman end code"; /*while huffman decoding*/
    case 11: return "error in code tree made it jump outside of huffman tree"; /*while huffman decoding*/
    case 13: return "problem while processing dynamic deflate block";
    case 14: return "problem while processing dynamic deflate block";
    case 15: return "problem while processing dynamic deflate block";
    case 16: return "unexisting code while processing dynamic deflate block";
    case 17: return "end of out buffer memory reached while inflating";
    case 18: return "invalid distance code while inflating";
    case 19: return "end of out buffer memory reached while inflating";
    case 20: return "invalid deflate block BTYPE encountered while decoding";
    case 21: return "NLEN is not ones complement of LEN in a deflate block";
     /*end of out buffer memory reached while inflating:
     This can happen if the inflated deflate data is longer than the amount of bytes required to fill up
     all the pixels of the image, given the color depth and image dimensions. Something that doesn't
     happen in a normal, well encoded, PNG image.*/
    case 22: return "end of out buffer memory reached while inflating";
    case 23: return "end of in buffer memory reached while inflating";
    case 24: return "invalid FCHECK in zlib header";
    case 26: return "FDICT encountered in zlib header while it's not used for PNG";
    case 27: return "PNG file is smaller than a PNG header";
    /*Checks the magic file header, the first 8 bytes of the PNG file*/
    case 28: return "incorrect PNG signature, it's no PNG or corrupted";
    case 29: return "first chunk is not the header chunk";
    case 30: return "chunk length too large, chunk broken off at end of file";
    case 31: return "illegal PNG color type or bpp";
    case 32: return "illegal PNG compression method";
    case 33: return "illegal PNG filter method";
    case 34: return "illegal PNG interlace method";
    case 35: return "chunk length of a chunk is too large or the chunk too small";
    case 36: return "illegal PNG filter type encountered";
    case 37: return "illegal bit depth for this color type given";
    case 39: return "more palette alpha values given in tRNS chunk than there are colors in the palette";
    case 40: return "tRNS chunk has wrong size for greyscale image";
    case 41: return "tRNS chunk has wrong size for RGB image";
    case 42: return "tRNS chunk appeared while it was not allowed for this color type";
    case 43: return "bKGD chunk has wrong size for palette image";
    case 44: return "bKGD chunk has wrong size for greyscale image";
    case 45: return "bKGD chunk has wrong size for RGB image";
    case 48: return "empty input buffer given to decoder. Maybe caused by non-existing file?";
    case 49: return "jumped past memory while generating dynamic huffman tree";
    case 50: return "jumped past memory while generating dynamic huffman tree";
    case 51: return "jumped past memory while inflating huffman block";
    case 52: return "jumped past memory while inflating";
    case 53: return "size of zlib data too small";
    case 54: return "repeat symbol in tree while there was no value symbol yet";
    /*jumped past tree while generating huffman tree, this could be when the
    tree will have more leaves than symbols after generating it out of the
    given lenghts. They call this an oversubscribed dynamic bit lengths tree in zlib.*/
    case 55: return "jumped past tree while generating huffman tree";
    case 56: return "given output image colortype or bitdepth not supported for color conversion";
    case 57: return "invalid CRC encountered (checking CRC can be disabled)";
    case 58: return "invalid ADLER32 encountered (checking ADLER32 can be disabled)";
    case 59: return "requested color conversion not supported";
    case 60: return "invalid window size given in the settings of the encoder (must be 0-32768)";
    case 61: return "invalid BTYPE given in the settings of the encoder (only 0, 1 and 2 are allowed)";
    /*LodePNG leaves the choice of RGB to greyscale conversion formula to the user.*/
    case 62: return "conversion from color to greyscale not supported";
    case 63: return "length of a chunk too long, max allowed for PNG is 2147483647 bytes per chunk"; /*(2^31-1)*/
    /*this would result in the inability of a deflated block to ever contain an end code. It must be at least 1.*/
    case 64: return "the length of the END symbol 256 in the Huffman tree is 0";
    case 66: return "the length of a text chunk keyword given to the encoder is longer than the maximum of 79 bytes";
    case 67: return "the length of a text chunk keyword given to the encoder is smaller than the minimum of 1 byte";
    case 68: return "tried to encode a PLTE chunk with a palette that has less than 1 or more than 256 colors";
    case 69: return "unknown chunk type with 'critical' flag encountered by the decoder";
    case 71: return "unexisting interlace mode given to encoder (must be 0 or 1)";
    case 72: return "while decoding, unexisting compression method encountering in zTXt or iTXt chunk (it must be 0)";
    case 73: return "invalid tIME chunk size";
    case 74: return "invalid pHYs chunk size";
    /*length could be wrong, or data chopped off*/
    case 75: return "no null termination char found while decoding text chunk";
    case 76: return "iTXt chunk too short to contain required bytes";
    case 77: return "integer overflow in buffer size";
	case 79: return "failed to open file for writing";
    case 80: return "tried creating a tree of 0 symbols";
    case 81: return "lazy matching at pos 0 is impossible";
    case 82: return "color conversion to palette requested while a color isn't in palette";
    case 84: return "given image too small to contain all pixels to be encoded";
    case 86: return "impossible offset in lz77 encoding (internal bug)";
    case 87: return "must provide custom zlib function pointer if LODEPNG_COMPILE_ZLIB is not defined";
    case 88: return "invalid filter strategy given for LodePNGEncoderSettings.filter_strategy";
    case 89: return "text chunk keyword too short or long: must have size 1-79";
    /*the windowsize in the LodePNGCompressSettings. Requiring POT(==> & instead of %) makes encoding 12% faster.*/
    case 90: return "windowsize must be a power of two";
    case 91: return "invalid decompressed idat size";
    case 92: return "too many pixels, not supported";
    case 93: return "zero width or height is invalid";
    case 94: return "header chunk must have a size of 13 bytes";

#endif
    }
    return "unknown error code";
}

// returns negative value on error. This should be pure C compatible, so no fstat. 
static long bmp_filesize(const char* filename)
{
    FILE* file;
    long size;
    file = fopen(filename, "rb");
    if(!file) return -1;

    if(fseek(file, 0, SEEK_END) != 0)
    {
      fclose(file);
      return -1;
    } 

    size = ftell(file);
  
    // It may give LONG_MAX as directory size, this is invalid for us. 
    if(size == LONG_MAX) size = -1;

    fclose(file);
    return size;
}

// load file into buffer that already has the correct allocated size. Returns error code.
static unsigned bmp_buffer_file(unsigned char* out, size_t size, const char* filename)
{
    FILE* file;
    size_t readsize;
    file = fopen(filename, "rb");
    if(!file) return 78;

    readsize = fread(out, 1, size, file);
    fclose(file);

    if (readsize != size) return 78;
    return 0;
}

// load file into buffer and allocate the correct size. Returns error code.
unsigned bmp_load_file(unsigned char** out, size_t* outsize, const char* filename)
{
    long size = bmp_filesize(filename);

    if (size < 0) return 78;
    *outsize = (size_t)size;

    *out = (unsigned char*) malloc((size_t)size);
    if(!(*out) && size > 0) return 83; 						// the above malloc failed

    return bmp_buffer_file(*out, (size_t)size, filename);
}

void BMP_BI_RLE8_Load(unsigned char** image,
                      const bmp_header *const bmphead, const bmp_info_header *const bmpinfohead,
                      const unsigned char *inbuf)
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
        ch = *inbuf++;
        ++count;
        if (ch)
        {
            // repeat byte
            ch2 = *inbuf++;
            ++count;
            for (i = 0; i < ch; ++i)
                *image[pos++] = ch2;
            continue;
        }

        // escape char
        ch = *inbuf++;
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

            ch = *inbuf++;
            ++count;
            // go right in the buffer
            pos += ch;

            ch = *inbuf++;
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
            ch = *inbuf++;
            ++count;
            for (i = 0; i < ch; ++i)
            {
                *image[pos++] = *inbuf++;
                ++count;
            }
            if (i % 2)
            {
                // Each run must be aligned on a word boundary.
                // Read and throw away the placeholder.
                ch2 = *inbuf++;
                ++count;
            }
        }
    }
} 

//-------------------------------------------------------------------------------------------------
// Converts BMP data in memory to raw pixel data.
unsigned bmp_decode(unsigned char** out, BMPState* state, unsigned int *w, unsigned int *h, const unsigned char* in, size_t insize)
{
    bmp_header bmphead;
    bmp_info_header bmpinfohead;
    unsigned int state_error=0;
    unsigned int currentseek;
    int i,index;
    unsigned int iwidth,iheight;
    unsigned char pix4bits;

//char *pp;

    // prepare output
    *out = 0;

    // get header and info header
    memcpy(&bmphead,in, sizeof(bmphead));
    currentseek = sizeof(bmphead);
    memcpy(&bmpinfohead,in+currentseek, sizeof(bmpinfohead));

    // not a valid BMP file: only 16, 256 colors
    if (bmphead.bfType != BF_TYPE )
    {
		    state_error=28;
    }
    // not a valid BMP file: only 16, 256 colors
    else if (bmpinfohead.biBitCount != 4 && bmpinfohead.biBitCount != 8 )
    {
		    state_error=38;
    }
    // not a valid BMP file: compressed and not BI_RLE8 supported
    else if (bmpinfohead.biCompression != 0 && bmpinfohead.biCompression != BI_RLE8)
    {
		    state_error=25;
    }
	  // hearder seems valid
	  else {
        // store the compression information
        state->info_bmp.compression_method=(bmpinfohead.biCompression == BI_RLE8);
		  
        // goto palette
	    currentseek=sizeof(bmp_header) + bmpinfohead.biSize;

        // read the palette information
        state->info_bmp.palettesize=1<<(bmpinfohead.biBitCount);
        for (index = 0; index < state->info_bmp.palettesize; index++)
        {
            state->info_bmp.palette[index].blue=*(in+(index<<2)+currentseek);
            state->info_bmp.palette[index].green=*(in+(index<<2)+currentseek+1);
            state->info_bmp.palette[index].red=*(in+(index<<2)+currentseek+2);
            // 4th byte is not used and always 0
        }

        // seek to image data
        currentseek=bmphead.bfOffBits;

        // read the bitmaps size and try to allocate memory
        *w = iwidth=bmpinfohead.biWidth;
        *h = iheight=bmpinfohead.biHeight;
        *out=(unsigned char *) malloc(iwidth*iheight);
        if (*out == NULL)
        {
            state_error=83;
        }
        else 
        {
            // read the uncompressed or compressed bitmap
            if (bmpinfohead.biCompression == 0)
            {
                for (index = (iheight - 1) * iwidth; index >= 0; index -= iwidth)
                {
                    if (bmpinfohead.biBitCount==4) // 16 colors mode, 1 byte = 2 pixels
                    {
                        for (i = 0; i < iwidth; )
                        {
                            pix4bits=*(in+currentseek+(i>>1));
                            *(*out+index + i) = (pix4bits>>4) & 0x0F;
                            i++;
                            *(*out+index + i) = pix4bits & 0x0F;
                            i++;
                        }
                        currentseek+=iwidth>>1;
                    }
                    else                            // 26 colors mode, 1 byte ) 1 pixel
                    {
                        for (i = 0; i < iwidth; i++)
                        {
                            *(*out+index + i) = *(in+currentseek+i);
                        }
                        currentseek+=iwidth;
                    }
                }
            }
            else if (bmpinfohead.biCompression == 1)
            {
                // BI_RLE8
                BMP_BI_RLE8_Load(out, &bmphead, &bmpinfohead, in+currentseek);
            }
        }
    }

    // go back with error code or not
    return state_error;
}

