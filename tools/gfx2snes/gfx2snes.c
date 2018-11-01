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

// 18/05/10 add offset per tile for map generation 

//INCLUDES
#include "gfx2snes.h"

//// M A I N   V A R I A B L E S ////////////////////////////////////////////////
int	border=1;								//options and their defaults
int	packed=0;								//
int size=0;									//
int screen=0;								//
int colors=0;								//
int output_palette=-1;  		//
int rearrange=0;						//
int palette_entry=0;				//
int file_type=1;						// 1 = bmp, 2 = pcx, 3 = tga, 4 = png
int quietmode=0;						// 0 = not quiet, 1 = i can't say anything :P
int collision=0;						// 1 = generated only collision map
int collisionsp=0;					// n = 1st sprite entry regarding the map (so remove it from colision map)
int tile_reduction=1;				// 0 = no tile reduction (warning !)
int savepalette=1;					// 1 = save the palette
int savemap=1;							// 1 = save the map
int colortabinc=16;    			// 16 for 16 color mode, 4 for 4 color mode
int lzpacked=0;         		// 1 = compress file with LZSS algorithm
int highpriority=0;     		// 1 = high priority for map
int blanktile=0;        		// 1 = blank tile generated
int palette_rnd=0;      		// 1 = round palette up & down
int offset_tile=0;					// n = offset in tile number

//// F U N C T I O N S //////////////////////////////////////////////////////////

int PCX_Load(char *filename, pcx_picture_ptr image)
{
	// this function loads a pcx file into a picture structure, the actual image
	// data for the pcx file is decompressed and expanded into a secondary buffer
	// within the picture structure, the separate images can be grabbed from this
	// buffer later.  also the header and palette are loaded

	long num_bytes,index;
	long count;
	long x,y;
	unsigned char data;
	pcx_header *header;
	FILE *fp;

	// open the file
	fp = fopen(filename,"rb");
	if(fp==NULL)
	{
		printf("\nERROR: Can't open file [%s]",filename);
		return 0;
	}

	// load the header
	header = &image->header;
	fread(header, 1, 128, fp);

	header->width++;
	header->height++;

	// check to make sure this is a 256 color PCX
	if( (header->manufacturer != 10) ||
		(header->encoding != 1) ||
		(header->bits_per_pixel != 8) ||
		(header->num_color_planes != 1) ||
		(header->palette_type != 1) )
	{
		printf("\nERROR: File [%s] is not recognized as a 256 color PCX.",filename);
		fclose(fp);
		return 0;
	}

	//allocate memory for the picture + 64 empty lines
	image->buffer = malloc( (size_t)(header->height+64)*header->width );
	if(image->buffer == NULL)
	{
		printf("\nERROR: Can't allocate enough memory for the picture.");
		fclose(fp);
		return 0;
	}

	//initally clear the memory (to make those extra lines be blank)
	memset(image->buffer,0,(size_t)(header->height+64)*header->width);

	// load the data and decompress into buffer
	count=0;
	for(y=0; y < header->height; y++)
	{
		for(x=0; x < header->width; x++)
		{
			// get the first piece of data
			data = getc(fp);

			// is this a rle?
			//if ( (data>=192) && (data<=255))
			if (data>=192)
			{
				// how many bytes in run?
				num_bytes = data-192;
				x += (num_bytes-1);

				// get the actual data for the run
				data = getc(fp);

				// replicate data in buffer num_bytes times
				while( num_bytes-- > 0)
					image->buffer[count++] = data;

			} // end if rle
			else
		    {
				// actual data, just copy it into buffer at next location
				image->buffer[count++] = data;
			} // end else not rle

		} //end of x loop

		//get rid of the padding byte if there is one
		if( x < header->bytes_per_line)
			data = getc(fp);

	} //end of y loop

	//Get the Palette header (one byte, should equal 12)
	data = getc(fp);
	if(data != 12)
	{
		printf("\nERROR: Couldn't find palette header [%s]",filename);
		free(image->buffer);
		fclose(fp);

		return 0;
	}

	//get the pallete data
	for (index=0; index<256; index++)
    {
	    image->palette[index].red   = (getc(fp) >> 2);
	    image->palette[index].green = (getc(fp) >> 2);
	    image->palette[index].blue  = (getc(fp) >> 2);
    }

	//check to make sure there weren't errors while reading the file
	if(ferror(fp))
	{
		printf("\nERROR: Error reading file [%s]",filename);
		free(image->buffer);
		fclose(fp);
		return 0;
	}

	fclose(fp);

	return -1;

} // end PCX_Load

void BMP_BI_RLE8_Load(pcx_picture_ptr image,
	const bmp_header* const bmphead, const bmp_info_header* const bmpinfohead,
	FILE* fp)
{
	// BI_RLE8 decompress according to:
	// https://technet.microsoft.com/ru-ru/dd183383

	unsigned long line, i, count;
	// offset in image buffer where current line starts
	unsigned int pos;
	unsigned char ch, ch2;

	// start from bottom line
	line = bmpinfohead->biHeight;
	pos = (line-1) * bmpinfohead->biWidth;

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
			for (i = 0; i < ch; ++i) image->buffer[pos++] = ch2;
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
			pos = (line-1) * bmpinfohead->biWidth;
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
			if (i%2)
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
	int index,i;
	pcx_header *header;
	bmp_header bmphead;
	bmp_info_header bmpinfohead;

	// open the file
	fp = fopen(filename,"rb");
	if(fp==NULL)
	{
		printf("\nERROR: Can't open file [%s]",filename);
		return 0;
	}

	// check to see if it is a valid bitmap file
	if (fread(&bmphead, sizeof(bmp_header), 1, fp)  < 1)
	{
		printf("\nERROR: File [%s] has no correct BMP header.",filename);
		fclose(fp);
		return 0;
	}

	if (bmphead.bfType != BF_TYPE)
	{
		printf("\nERROR: File [%s] is not recognized as a BMP file.",filename);
		fclose(fp);
		return 0;
	}

	// check to see if it is a valid bitmap file
	if (fread(&bmpinfohead, sizeof(bmpinfohead), 1, fp)  < 1)
	{
		printf("\nERROR: File [%s] has no correct BMP info header.",filename);
		fclose(fp);
		return 0;
	}

	if (bmpinfohead.biBitCount != 8 || (bmpinfohead.biCompression != 0 && bmpinfohead.biCompression != 1 /*BI_RLE8*/))
	{
		printf("\nERROR: File [%s] is not a valid BMP file: 256 colors, non-compressed or BI_RLE8 supported.",filename);
		fclose(fp);
		return 0;
	}

	// seek to palette
	fseek(fp, sizeof(bmp_header) + bmpinfohead.biSize, 0);

	// initally clear the palette if there are less then 256 colors in the file
	memset(image->palette, 0, (size_t)(256 * sizeof(RGB_color)));

	// read the palette information
	for (index = 0; index<256; index++)
	{
		image->palette[index].blue = getc(fp) >> 2;
		image->palette[index].green = getc(fp) >> 2;
		image->palette[index].red = getc(fp) >> 2;
		//data=getc(fp);
		getc(fp);
	}

	header = &image->header;
	header->width = bmpinfohead.biWidth;
	header->height = bmpinfohead.biHeight;

	// allocate memory for the picture + 64 empty lines
	image->buffer = malloc( (size_t)(header->height+64) * header->width );
	if(image->buffer == NULL)
	{
		printf("\nERROR: Can't allocate enough memory for the picture.");
		fclose(fp);
		return 0;
	}

	// initally clear the memory (to make those extra lines be blank)
	memset(image->buffer,0,(size_t)(header->height+64) * header->width);

	// seek to image data
	fseek(fp, bmphead.bfOffBits, 0);

	// read the bitmap
	if (bmpinfohead.biCompression == 0)
	{
		for(index=(header->height-1) * header->width;index>=0;index-=header->width)
			for(i=0;i<header->width;i++) 
				image->buffer[index+i] = getc(fp);
	}
	else if (bmpinfohead.biCompression == 1)
	{
		// BI_RLE8
		BMP_BI_RLE8_Load(image, &bmphead, &bmpinfohead, fp);
	}

	// check to make sure there weren't errors while reading the file
	if(ferror(fp))
	{
		printf("\nERROR: Error reading file [%s]",filename);
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
	unsigned long index,i;
	tga_header tgahead;
	pcx_header *header;

	// open the file
	fp = fopen(filename,"rb");
	if(fp==NULL)
	{
		printf("\nERROR: Can't open file [%s]",filename);
		return 0;
	}

	// check to see if it is a valid bitmap file
	if (fread(&tgahead, sizeof(tga_header), 1, fp)  < 1)
	{
		printf("\nERROR: File [%s] has no correct TGA header.",filename);
		fclose(fp);
		return 0;
	}


		//check to make sure there weren't errors while reading the file
	if(ferror(fp))
	{
		printf("\nERROR: Error reading file [%s]",filename);
		free(image->buffer);
		fclose(fp);
		return 0;
	}


	if (tgahead.BPP != 8 || tgahead.ImageType != 1)
	{
		printf("\nERROR: File [%s] is not a valid indexed 256 colors TGA file.",filename);
		fclose(fp);
		return 0;
	}

	header = &image->header;
	header->width = tgahead.Width;
	header->height = tgahead.Height;

  //allocate memory for the picture + 64 empty lines
	image->buffer = malloc( (size_t)(header->height+64) * header->width );
	if(image->buffer == NULL)
	{
		printf("\nERROR: Can't allocate enough memory for the picture.");
		fclose(fp);
		return 0;
	}

	//initally clear the memory (to make those extra lines be blank)
	memset(image->buffer,0,(size_t)(header->height+64) * header->width);

	// read the palette information
	for(index=0;index<256;index++)
	{
		image->palette[index].blue = getc(fp) >> 2;
		image->palette[index].green = getc(fp) >> 2;
		image->palette[index].red = getc(fp) >> 2;
	}

	// read the bitmap
	for(index=(header->height-1) * header->width;index>=0;index-=header->width)
		for(i=0;i<header->width;i++)
			image->buffer[index+i] = getc(fp);

	fclose(fp);

	return -1;
} // end TGA_Load

int PNG_Load(char *filename, pcx_picture_ptr image)
{
  unsigned error, index, i,sz,bpp;
  unsigned char *pngimage;
  unsigned char* png = 0;
  size_t pngsize;
  LodePNGState state;
  size_t width, height;// , wal,hal;
	pcx_header *header;

  /*optionally customize the state*/
  lodepng_state_init(&state);

  // no conversion of color (to keep palette mode)
	state.decoder.color_convert = 0;

	error = lodepng_load_file(&png, &pngsize, filename);
  if (!error)  {
		error = lodepng_decode(&pngimage, &width, &height, &state, png, pngsize);
	}
	if(error) {
		printf("\nERROR: Decoder error %u: %s\n", error, lodepng_error_text(error));
		free(png);
		lodepng_state_cleanup(&state);
		free(pngimage);
		return 0;
	}

	bpp = state.info_raw.bitdepth;
	if ( (bpp  != 4) && (bpp != 8)) {
		printf("\nERROR: File [%s] is not a valid bbp value (%d bpp).",filename,bpp);
		free(png);
		lodepng_state_cleanup(&state);
		free(pngimage);
		return 0;
	}
	
	if (state.info_raw.colortype != LCT_PALETTE)
  {
		printf("\nERROR: File [%s] is not a valid indexed palette mode (mode %d).",filename,state.info_raw.colortype);
		free(png);
		lodepng_state_cleanup(&state);
		free(pngimage);
		return 0;
	}

	// read the palette information
	sz=state.info_png.color.palettesize;
	for(index=0;index<sz;index++) {
		image->palette[index].red = state.info_png.color.palette[(index*4) + 0]>>2;
		image->palette[index].green = state.info_png.color.palette[(index*4) + 1]>>2;
		image->palette[index].blue = state.info_png.color.palette[(index*4) + 2]>>2;
	}
	
	// get png information
	header = &image->header;
	header->width = width;
	header->height = height;

	printf("\nWidth %d Height %d",header->width,header->height);

  //allocate memory for the picture + 64 empty lines
	image->buffer = malloc( (size_t)(header->height+64) * header->width );
	if(image->buffer == NULL)	{
		printf("\nERROR: Can't allocate enough memory for the picture.");
		return 0;
	}

	//initally clear the memory (to make those extra lines be blank)
	memset(image->buffer,0,(size_t)(header->height+64) * header->width);

	// 4 bpps conversion
	if (bpp==4) {
	
		for (index = 0; index < header->height; index++) {
			for(i=0;i<header->width;i++)
				image->buffer[index+i] = pngimage[i +index*header->height];
		}
		/*
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
			*/
	}
	// 8 bpps conversion
	else {

		for (index = 0; index < header->height; index++) {
			for(i=0;i<header->width;i++) {
				image->buffer[i+(header->width*index)] = pngimage[i+(header->width*index)];
			}
		}
	}
			
	free(png);

	lodepng_state_cleanup(&state);

	free(pngimage);
	
	return -1;
} // end PNG_Load

//////////////////////////////////////////////////////////////////////////////

void PutWord(int data, FILE *fp)
{
	putc(LOW_BYTE(data),fp);
	putc(HI_BYTE(data),fp);
} //end of PutWord

//////////////////////////////////////////////////////////////////////////////

unsigned char *ArrangeBlocks( unsigned char *img, int width, int height,
							 int size, int *xsize, int *ysize, int new_width, int border)
{
	/*
	** img = image buffer
	** width = width (in pixels) of image buffer
	** height = height (in pixels) of image buffer
	**
	** size = size (in pixels) of image blocks in the image
	** *xsize = number of image block horizontally in image block grid
	** *ysize = number of image block vertically in image block grid
	**
	** border = how wide of a border surrounds the image blocks
	** new_width = how wide (in pixels) you want the new buffer to be
	**             must be a multiple of size
	**
	**
	** returns:
	**      pointer to new buffer, and updates xsize and ysize
	**
	*/
	unsigned char *buffer;
	int rows, num;
	int i,j, line;
	int x,y;

	if (quietmode == 0)
		printf("\nwidth=%d, height=%d, size=%d, *xsize=%d, *ysize=%d, new_width=%d, border=%d",
			width, height, size, *xsize, *ysize, new_width, border);

	//get number of full image block rows in the new buffer
	rows = (*xsize)*(*ysize)/(new_width/size);   // rows = num_blocks / new_xsize

	//if it doesn't divide evenly, add another full row
	if ( ((*xsize)*(*ysize))%(new_width/size) != 0 )
		rows++;

	if (quietmode == 0)
		printf("\nrows=%d",rows);

	//get memory for the new buffer
	buffer = malloc( (size_t) rows*size*new_width );
	if(buffer == NULL) {
		printf("\nERROR: Can't allocate enough memory for the buffer in ArrangeBlocks.");
		return 0;
	}

	//initially clear the buffer, so if there are empty image blocks
	//or incomplete blocks, the empty parts will be blank
	memset(buffer,0,(size_t) rows*size*new_width );

	//position in new buffer (x,y) where x and y are in pixel co-ordinates
	x=0;
	y=0;

	//go through each image block(i,j) where i and j are in block co-ordinates
	for(j=0; j < *ysize; j++)
	for(i=0; i < *xsize; i++)
	{
		//move each line of the block into the new buffer
		//don't worry about reading past the end of the image here
		//there is an extra 64 lines to read in.
		for(line=0;line<size;line++)
		{
			//find out how much to copy
			//this is needed because the screen mode files may not be
			//a multiple of 8 pixels wide
			//or no-border files may have the wrong width
			num = width - (i*(size+border) + border);
			if(num>size)
				num=size;

			memcpy( &buffer[ (y+line)*new_width + x ],
		     &img[ (j*(size+border) + line + border)*width + i*(size+border) + border ],
			 num);
		}

		//move to the next location in the new buffer
		x+=size;
		if(x >= new_width)
		{
			x=0;
			y+=size;
		}
	}

	*xsize = new_width/size;
	*ysize = rows;

	return buffer;
} //end of ArrangeBlocks()

//////////////////////////////////////////////////////////////////////////////

int *MakeMap(unsigned char *img, int *num_tiles,
			 int xsize, int ysize, int tile_x, int tile_y, int colors, int rearrange, int pal_entry)
{
	int *map;
	unsigned char blank[64];
	//int tiles = *num_tiles;
	int newtiles;
	int blank_absent;
	int current;	//the current tile we're looking at
	int i,t, palette;
	int x,y;
	//int x_offset, y_offset;

	//find x_offset,y_offset
	//don't center
	//x_offset=0;
	//y_offset=0;

	//allocate map
	map=malloc((size_t)tile_x*tile_y*sizeof(int));
	if(map==NULL) {
		printf("\nERROR: Can't allocate enough memory for the map in MakeMap.");
		return 0;
	}

	//clear map
	memset(map,0,tile_x*tile_y*sizeof(int));

	//if the palette has been rearranged... save the palette number
	//if(rearrange)
	{
		current=0;
		for(y=0;y<ysize;y++)
		for(x=0;x<xsize;x++)
		{
			//get the palette number
			if (colortabinc == 16)
				palette = (img[current*64] >> 4) & 0x07;
			else
				palette = (img[current*64] >> 2) & 0x03;
			t = ((palette+pal_entry) << 10);

			//put tile number in map
			if(tile_x==64 && tile_y==32) // 64x32 screen
			{
				if(x<32)
					map[y*32 + x]=t;
				else
					map[(y+32)*32+x-32]=t;
			}
			else if(tile_x==32 && tile_y==64) // 32x64 screen
				map[y*32+x]=t;
			else if(tile_x==64 && tile_y==64) // 64x64 screen
			{
				if(y<32)
					if(x<32)
						map[y*32+x]=t;
					else
						map[(y+32)*32+x-32]=t;
				else
					if(x<32)
						map[(y+64-32)*32+x]=t;
					else
						map[(y+96-32)*32+x-32]=t;
			}
			else //32x32 or 128x128 screen
				map[y*tile_x+x]=t;

			//goto the next tile
			current++;
		}

	} //end of if(rearrange)


	//truncate the colors if necessary
	if(colors != 256)
	{
		t = colors - 1;	//color truncation mask

		for(i=0;i<xsize*ysize*64;i++)
			img[i] = img[i] & t;
	}

	//make a blank tile
	memset(blank,0,64);

	//I want tile #0 to be blank..
	//is it?
	if (blanktile==1 )
	{
		if( memcmp(blank,img,64) == 0 )
		{
			blank_absent=0;
			current=1;
			t=0;
			newtiles=1;
		}
		else
		{
			blank_absent=1;
			current=1;
			t=1;
			newtiles=1;
		}
	}
	else
	{
		blank_absent=0;
		current=1;
		t=0;
		newtiles=1;
	}

	//save the first tilemap piece
	map[0] += t;

	for(y=0;y<ysize;y++)
	for(x=0;x<xsize;x++)
	{
		//if we already processed this, move on
		if(x==0 && y==0)
			continue;

		// if no reduction
		if (tile_reduction) {
			//is the current tile blank?
			if( memcmp(blank,&img[current*64],64) == 0 )
				t=0;
			else
			{
				//check for matches with previous tiles if tile_reduction on
				for(i=0;i<newtiles;i++)
					if( memcmp(&img[i*64],&img[current*64],64) == 0 )
						break;

				//is it a new tile?
				if(i==newtiles)
				{
					// yes -> add it
					memcpy(&img[newtiles*64],&img[current*64],64);
					t=newtiles+blank_absent;
					newtiles++;
				}
				else
				{	// no -> find what tile number it is
					t=i+blank_absent;
				}
			}
		}
		// else, always a new tile
		else {
			i = newtiles;

			// yes -> add it
			memcpy(&img[newtiles*64],&img[current*64],64);
			t=newtiles+blank_absent;
			newtiles++;
		}



		//put tile number in map
		if(tile_x==64 && tile_y==32) // 64x32 screen
		{
			if(x<32)
				map[y*32 + x] += t;
			else
				map[(y+32)*32+x-32] += t;
		}
		else if(tile_x==32 && tile_y==64) // 32x64 screen
			map[y*32+x] += t;
		else if(tile_x==64 && tile_y==64) // 64x64 screen
		{
			if(y<32)
				if(x<32)
					map[y*32+x] += t;
				else
					map[(y+32)*32+x-32] += t;
			else
				if(x<32)
					map[(y+64-32)*32+x] += t;
				else
					map[(y+96-32)*32+x-32] += t;
		}
		else //32x32 or 128x128 screen
			map[y*tile_x+x] += t;

		//goto the next tile
		current++;
	}

	//also return the number of new tiles
	//make it negative if we need to add the blank tile
	if (blank_absent)
		*num_tiles = -newtiles;
	else
		*num_tiles = newtiles;

	return map;

}//end of MakeMap

//////////////////////////////////////////////////////////////////////////////

int RearrangePalette(unsigned char *buffer, int *palette,
					 int num_tiles, int colors)
{
	int final[8];
	int num_final;
	int *combos;//holds sorted list of colors in combo of each tile
	int *num;	//holds number of colors in each combo
	int *list;	//for sorting combos
	int n;

	int new_palette[256];
	int color_table[256];

	int index, last_index;
	int test, test2;
	int num_miss;
	int data;
	int i,ii;

	//get memory
	num=malloc(num_tiles*sizeof(int));
	if(num==NULL)
	{
		printf("\nERROR: Not enough memory to do rearrangement calculations.\n");
		return 0;
	}

	combos=malloc(num_tiles*16*sizeof(int));
	if(combos==NULL)
	{
		printf("\nERROR: Not enough memory to do rearrangement calculations.\n");
		free(num);
		return 0;
	}

	list=malloc(num_tiles*sizeof(int));
	if(list==NULL)
	{
		printf("\nERROR: Not enough memory to do rearrangement calculations.\n");
		free(combos);
		free(num);
		return 0;
	}

	//clear 'color combo' lists
	memset(combos,0,num_tiles*16*sizeof(int));

	//start each list having one color... color zero
	for(i=0;i<num_tiles;i++)
		num[i]=1;

	//if two colors have the same RGB values...
	//replace all instances of the redundant color with the first color
	for(i=0;i<256;i++)
		for(ii=i+1;ii<256;ii++)
			if(palette[i]==palette[ii]) {
				for(index=0; index < num_tiles*8*8; index++)
					if(buffer[index]==ii)
						buffer[index]=i;
			}

	//now, build up the 'color combo' list...
	for(index=0;index<num_tiles;index++)
		for(i=0;i<64;i++)
		{
			data=buffer[index*64+i];

			//is this color already in the list?
			for(ii=0;ii<num[index];ii++)
				if(combos[index*colortabinc+ii]==data)
					break;

			//if not add it to the list
			if(ii==num[index])
			{
				if(num[index]==colors) //combo is full
				{
					printf("\nERROR: Detected more colors in one 8x8 tile than is allowed.\n");
					free(list);
					free(combos);
					free(num);
					return 0;
				}

				combos[index*colortabinc+ii]=data;
				num[index]++;
			}
		}

	//now sort combos in order of number of colors (greatest to least)
	//here's some more horrid code... I know this is all messy and
	//slow, but hey... I just don't care right now.
	n=0;
	for(ii=colors;ii>0;ii--)
		for(i=0;i<num_tiles;i++)
			if(num[i]==ii)
				list[n++]=i;

	//ok, now try to combine the combos
	last_index=-1;
	for(num_final=0;num_final<9;num_final++)
	{
		//start looking for next 'non-combined' combo in the list
		for(index=last_index+1; index<num_tiles; index++)
			if(num[list[index]]>0)
				break;

		//if none... we're done
		if(index==num_tiles)
			break;

		// test = combo # of new 'final combo'
		test=list[index];
		last_index=index;

		//check if we've failed
		if(num_final==8)
		{
			//we already have 8 palettes... can't add more
			printf("\nERROR: not enough colors/palettes to represent the picture.\n");
			free(list);
			free(combos);
			free(num);
			return 0;
		}

		//if one exists, then add to final and start combining
		final[num_final]=test;
		for(n=index+1; n<num_tiles; n++)
		{
			//n = index into sorted list of combos

			//test  = combo # of new 'final combo'
			//test2 = combo we're going to try to combine with the 'final combo'
			test2 = list[n];

			//if already combined to someone... move on
			if(num[test2]<0)
				continue;

			//can it be combined?
			num_miss = 0;
			for(ii=test2*colortabinc;ii<test2*colortabinc+num[test2];ii++)
			{
				//ii = index into the 'attempting to combine' combo
				// i = index into the 'final combo'

				//check for non-matched colors
				for(i=test*colortabinc;i<test*colortabinc+num[test]+num_miss;i++)
					if(combos[ii]==combos[i])
						break;

				//is there a miss?
				if(i == test*colortabinc+num[test]+num_miss)
				{
					if(num[test]+num_miss == colors)
					{
						//we can't add anymore colors
						//this combine has failed
						num_miss=-1;
						break;
					}

					//temporarily add the missed color to the 'final combo'
					combos[test*colortabinc + num[test] + num_miss] = combos[ii];
					num_miss++;
				}
			}//loop - try to combine an individual combo

			//did we succeed?
			if(num_miss>=0)
			{
				//permanently add in the new colors;
				num[test] = num[test] + num_miss;

				//save the final_num here, and make this negative to show it
				//has been combined
				num[test2] = num_final - 100;
			}

		}//loop - sweep through combos, finding potential ones to combine

	}//build up 8 palettes...


	//Yeah! ... if we made it here it worked!
	//(assuming my code is right)
	if (quietmode == 0)
		printf("\nRearrangement possible!! Accomplished in %d palettes...", num_final);

	//convert the image
	for(i=0;i<num_tiles;i++)
	{
		//reset conversion table
		memset(color_table, 0, 256*sizeof(int));

		//find which 'final combo' this block uses
		if(num[i]>0)
		{
			//this block's combo became a final, find which final it is
			for(n=0;n<num_final;n++)
				if(final[n]==i)
					break;
		}
		else
			n = num[i] + 100;

		//make the conversion table
		for(ii=0;ii<num[final[n]];ii++)
		{
			index = combos[ final[n]*colortabinc + ii ];
			color_table[index] = n*colortabinc + ii;
		}

		//convert the block
		for(ii=64*i; ii<64*(i+1); ii++)
			buffer[ii] = (unsigned char) color_table[ buffer[ii] ];
	}


	//clear conversion table, and default palette entries to the original palette
	memcpy(new_palette, palette, 256*sizeof(int));

	//make the palette conversion
	for(i=0;i<num_final;i++) {
		for(ii=0;ii<num[final[i]];ii++)
		{
			index = combos[ final[i]*colortabinc + ii ];
			new_palette[ i*colortabinc + ii ] = palette[index];
		}
	}

	//save back the palette
	memcpy(palette, new_palette, 256*sizeof(int));


	//free up mem from the combo lists
	free(list);
	free(combos);
	free(num);

	return -1;
}//end of RearrangePalette()

//////////////////////////////////////////////////////////////////////////////
extern int Convert2PicLZSS(int quietmode, unsigned char *bufin, int buflen, unsigned char *bufout);
int Convert2Pic(char *filebase, unsigned char *buffer,
				int num_tiles, int blank_absent, int colors, int packed, int lzsspacked)
{
	char filename[80];
	int x,y,t,b;
	int i,j;
	int bitplanes;
	int mask;
	int bufsize;
	unsigned char data;
	unsigned char *buftolzin, *buftolzout;
	FILE *fp;

	if(packed)
		sprintf(filename,"%s.pc7",filebase);
	else
		sprintf(filename,"%s.pic",filebase);

	if (quietmode == 0)
		printf("\nSaving graphics file: [%s]",filename);
	fp = fopen(filename,"wb");

	if(fp==NULL)
	{
		printf("\nERROR: Can't open file [%s] for writing\n",filename);
		return 0;
	}

	if(packed)
	{
		//remember to add the blank if its needed....
		if(blank_absent)
			for(i=0;i<64;i++)
				fputc(0,fp);

		fwrite(buffer,64*num_tiles,1,fp);

		fclose(fp);
		return -1;
	}

	//find the number of bitplanes
	bitplanes=8;
	if(colors==4)
		bitplanes=2;
	else if(colors==16)
		bitplanes=4;
	else if(colors<=128)
		bitplanes=4;
	//else if(colors==128)
	//	bitplanes=8;
	//else if (colors==256)
	//	bitplanes=8;

	// if lzss encoding begin with transferring data
	if (lzsspacked) {
		bufsize=8*bitplanes+num_tiles*8*bitplanes;
		buftolzin = malloc (bufsize);
		if (buftolzin == NULL) {
			printf("\nERROR: Can't allocate enough memory for the buffer comrpession 1.");
			fclose(fp);
			return 0;
		}
		buftolzout = malloc (bufsize);
		if (buftolzout == NULL) {
			printf("\nERROR: Can't allocate enough memory for the buffer comrpession 2.");
			free(buftolzin);
			fclose(fp);
			return 0;
		}
		//remember to add the blank if its needed....
		j=0;
		if(blank_absent)
			for(i=0;i<8*bitplanes;i++, j++)
				*(buftolzin+i) = 0;

		for(t=0;t<num_tiles;t++) { //loop through tiles
			for(b=0;b<bitplanes;b+=2) { //loop through bitplane pairs
				for(y=0;y<8;y++) {
					//get bit-mask
					mask = 1 << b;
					data = 0;

					//get row of bit-plane
					for(x=0;x<8;x++) {
						data = data << 1;
						if(buffer[t*64 + y*8 + x] & mask)
							data = data+1;
					}
					*(buftolzin+j) = data; j++;

					//adjust bit-mask
					mask = mask << 1;
					data = 0;

					//get row of next bit-plane
					for(x=0;x<8;x++) {
						data = data << 1;
						if(buffer[t*64 + y*8 + x] & mask)
							data = data+1;
					}
					*(buftolzin+j) = data; j++;
				}
			}
		}
		// Compress data and save to disc
		bufsize = Convert2PicLZSS(quietmode, buftolzin, j, buftolzout);
		for (i=0; i<bufsize;i++)
			fputc(*(buftolzout+i),fp);
		free(buftolzout);
		free(buftolzin);
	}
	else {
		//remember to add the blank if its needed....
		if(blank_absent)
			for(i=0;i<8*bitplanes;i++)
				fputc(0,fp);

		printf("\ndecode for %d tiles and %d bitplanes\n",num_tiles,bitplanes);

		for(t=0;t<num_tiles;t++) //loop through tiles
		for(b=0;b<bitplanes;b+=2) //loop through bitplane pairs
		for(y=0;y<8;y++)
		{
			//get bit-mask
			mask = 1 << b;
			data = 0;

			//get row of bit-plane
			for(x=0;x<8;x++)
			{
				data = data << 1;
				if(buffer[t*64 + y*8 + x] & mask)
					data = data+1;
			}

			//save row
			fputc(data,fp);

			//adjust bit-mask
			mask = mask << 1;
			data = 0;

			//get row of next bit-plane
			for(x=0;x<8;x++)
			{
				data = data << 1;
				if(buffer[t*64 + y*8 + x] & mask)
					data = data+1;
			}

			//save row
			fputc(data,fp);
		}
	}

	fclose(fp);

	return -1;
} //end of Convert2Pic

//////////////////////////////////////////////////////////////////////////////

void ConvertPalette(RGB_color *palette, int *new_palette)
{
	int i,data;
	int rounded;
	int temp;

	//Convert the colors and put them in the new array
	//
	//alternate rounding down and rounding up
	//this is an attempt to preserve the brightness of a color
	//which the human eye notices easier than a change in color
	rounded=0;
	for(i=0;i<256;i++)
	{
		if(palette_rnd)
		{
			data=0;

			//get blue portion and round it off
			temp = (palette[i].blue & 0x01);	//see if this needs rounding
			if(palette[i].blue == 63)			//if value == 63, then we can't round up
			{
				temp = 0;
				rounded = 1;
			}
			data = (data<<5) + (palette[i].blue >> 1)
				+ (temp & rounded);				//round up if necessary
			rounded = (temp ^ rounded);			//reset rounded down flag after rounding up

			//get green portion and round it
			temp = (palette[i].green & 0x01);	//see if this needs rounding
			if(palette[i].green == 63)			//if value == 63, then we can't round up
			{
				temp = 0;
				rounded = 1;
			}
			data = (data<<5) + (palette[i].green >> 1)
				+ (temp & rounded);				//round up if necessary
			rounded = (temp ^ rounded);			//reset rounded down flag after rounding up

			//get red portion and round it
			temp = (palette[i].red & 0x01);	//see if this needs rounding
			if(palette[i].red == 63)			//if value == 63, then we can't round up
			{
				temp = 0;
				rounded = 1;
			}
			data = (data<<5) + (palette[i].red >> 1)
				+ (temp & rounded);				//round up if necessary
			rounded = (temp ^ rounded);			//reset rounded down flag after rounding up

			//store converted color
			new_palette[i] = data;
		}
		else
		{
			data=0;

			data = (data<<5) + (palette[i].blue >> 1);
			data = (data<<5) + (palette[i].green >> 1);
			data = (data<<5) + (palette[i].red >> 1);

			//store converted color
			new_palette[i] = data;
		}
	}//loop through all colors

} //end of ConvertPalette

//////////////////////////////////////////////////////////////////////////////

void PrintOptions(char *str)
{
	printf("\n\nUsage : gfx2snes [options] bmp/pcx/tga filename ...");
	printf("\n  where filename is a 256 color BMP, PCX or TGA file");

	if(str[0]!=0)
		printf("\nThe [%s] parameter is not recognized.",str);

	printf("\n\nOptions are:");
	printf("\n\n--- Graphics options ---");
	printf("\n-gb               add blank tile management (for multiple bgs)");
	printf("\n-gp               Output in packed pixel format");
	printf("\n-glz              Output in lzss compressed pixel format");
	printf("\n-gs(8|16|32|64)   Size of image blocks in pixels [8]");
	printf("\n\n--- Map options ---");
	printf("\n-m!               Exclude map from output");
	printf("\n-m                Convert the whole picture");
	printf("\n-mp               Convert the whole picture with high priority");
	printf("\n-m7               Convert the whole picture for mode 7 format");
	printf("\n-mc               Generate collision map only");
	printf("\n-ms#              Generate collision map only with sprites table");
	printf("\n                   where # is the 1st tile corresponding to a sprite (0 to 255)");
	printf("\n-mn#              Generate the whole picture with an offset for tile number");
	printf("\n                   where # is the offset in decimal (0 to 2047)");
	printf("\n-mR!              No tile reduction (not advised)");
	printf("\n\n--- Palette options ---");
	printf("\n-p!               Exclude palette from output.");
	printf("\n-pc(4|16|128|256) The number of colors to use [256]");
	printf("\n-po#              The number of colors to output (0 to 256) to the filename.pal");
	printf("\n-pe#              The palette entry to add to map tiles (0 to 16)");
	printf("\n-pr               Rearrange palette, and preserve palette numbers in the tilemap");
	printf("\n-pR               Palette rounding");
	printf("\n\n--- File options ---");
	printf("\n-f[bmp|pcx|tga|png]   convert a bmp or pcx or gta or png file [bmp]");
	printf("\n\n--- Misc options ---");
	printf("\n-n                no border");
	printf("\n-q                quiet mode");
	printf("\n");

} //end of PrintOptions()

/// M A I N ////////////////////////////////////////////////////////////

int main(int argc, char **arg)
{
	int palette[256];
	int tile_x, tile_y;

	unsigned char clr;
	int height, width;
	int xsize, ysize;
	int num_tiles;
	int blank_absent=0;
	pcx_picture image;

	unsigned char *buffer;
	unsigned char *temp;
	int *tilemap;
	FILE *fp;

	char filebase[256]="";
	char filename[256];
	int i, j;

	// Show something to begin :)
	if (quietmode == 0) {
		printf("\n==============================");
		printf("\n---gfx2snes v"GFX2SNESVERSION" "GFX2SNESDATE"---");
		printf("\n------------------------------");
		printf("\n(c) 2013-2018 Alekmaul ");
		printf("\nBased on pcx2snes by Neviksti");
		printf("\n==============================\n");
	}

	//parse the arguments
	for(i=1;i<argc;i++)
	{
		if(arg[i][0]=='-')
		{
			if(arg[i][1]=='g') // graphics options
			{
				if( strcmp(&arg[i][1],"gb") == 0) // blank tile
				{
					blanktile=1;
				}
				else if( strcmp(&arg[i][1],"gp") == 0)	//packed pixels
				{
					packed=1;
				}
				else if( strcmp(&arg[i][1],"glz") == 0)	// lzss compressed pixels
				{
					lzpacked=1;
				}
				else if(arg[i][2]=='s')  //size specification
				{
					size=atoi(&arg[i][3]);
					if( (size != 8) && (size !=16) &&  (size != 32) && (size != 64) )
					{
						PrintOptions(arg[i]);
						return 1;
					}
				}
			}
			else if(arg[i][1]=='n') //no border
			{
				border=0;
			}
			else if(arg[i][1]=='q') //quiet mode
			{
				quietmode=1;
			}
			else if(arg[i][1]=='m') //map options
			{
				if( strcmp(&arg[i][1],"m") == 0)
				{
					screen=1;
					border=0;
				}
				else if( strcmp(&arg[i][1],"mp") == 0)
				{
					screen=1;
					border=0;
					highpriority=1;
				}
				else if( strcmp(&arg[i][1],"m!") == 0)
				{
					screen=1;
					border=0;
					savemap=0;
				}
				else if( strcmp(&arg[i][1],"m7") == 0)
				{
					screen=7;
					border=0;
					packed=1;
				}
				else if( strcmp(&arg[i][1],"mc") == 0)
				{
					screen=1;
					border=0;
					collision=1;
				}
				else if(arg[i][2]=='s') //sprite entry location
				{
					screen=1;
					border=0;
					collision=2;
					collisionsp = atoi(&arg[i][3]);
				}
				else if(arg[i][2]=='n') //offset for tiles
				{
					offset_tile = atoi(&arg[i][3]);
				}
				else if( strcmp(&arg[i][1],"mR!") == 0)
				{
					tile_reduction=0;
				}
				else
				{
					PrintOptions(arg[i]);
					return 1;
				}
			}
			else if(arg[i][1]=='p') //palette options
			{
				if(arg[i][2]=='c') //color number specification
				{
					colors=atoi(&arg[i][3]);
					if( (colors != 4) && (colors !=16) && (colors !=32) &&  (colors != 128) && (colors != 256) )
					{
						PrintOptions(arg[i]);
						return 1;
					}
					if (colors == 4)
						colortabinc = 4;
				}
				else if(arg[i][2]=='o') //color number specification
				{
					output_palette=atoi(&arg[i][3]);
					if( (output_palette < 0) || (output_palette > 256) )
					{
						PrintOptions(arg[i]);
						return 1;
					}
				}
				else if( strcmp(&arg[i][1],"pr") == 0) //re-arrange palette option
				{
					rearrange=1;
				}
				else if( strcmp(&arg[i][1],"p!") == 0) //re-arrange palette option
				{
					savepalette=0;
				}
				else if( strcmp(&arg[i][1],"pR") == 0)
				{
					palette_rnd=1;
				}
				else if(arg[i][2]=='e') //palette entry specification
				{
					palette_entry=atoi(&arg[i][3]);
					if( (palette_entry < 0) || (palette_entry > 16 ) )
					{
						PrintOptions(arg[i]);
						return 1;
					}
				}
			}
			else if(arg[i][1]=='f') //file type specification
			{
				if( strcmp(&arg[i][1],"fpcx") == 0)
				{
					file_type = 2; // PCX,
				}
				else if( strcmp(&arg[i][1],"ftga") == 0)
				{
					file_type = 3; // TGA
				}
				else if( strcmp(&arg[i][1],"fpng") == 0)
				{
					file_type = 4; // PNG, evething else is bmp
				}
			}
			else //invalid option
			{
				PrintOptions(arg[i]);
				return 1;
			}
		}
		else
		{
			//its not an option flag, so it must be the filebase
			if(filebase[0]!=0) // if already defined... there's a problem
			{
				PrintOptions(arg[i]);
				return 1;
			}
			else
				strcpy(filebase,arg[i]);
		}
	}

	//make sure options are valid
	if((colors==0) && screen)
		colors=256;

	if( filebase[0] == 0 )
	{
		printf("\nERROR: You must specify a base filename.");
		PrintOptions("");
		return 1;
	}

	if( colors == 0 )
	{
		printf("\nERROR: The Number of Colors parameter must be specified.");
		PrintOptions("");
		return 1;
	}

	if((size == 0) && (border == 0) && (screen == 0))
	{
		printf("\nERROR: The Size parameter must be specified when the border is turned off.");
		PrintOptions("");
		return 1;
	}

	if(rearrange)
	{
		//let i = the number of colors in all 8 palettes
		i = colors*8;
		if(i>256)
			i=256;

		if((colors == 256) || (colors == 128))
		{
			printf("\n***WARNING: -c128 and -c256 override the -r option. The palette will not be rearranged.");
			rearrange=0;
		}
		if(screen == 0)
		{
			printf("\n***WARNING: the -r option means nothing in image block mode. The palette will not be rearranged.");
			rearrange=0;
		}
		else if((output_palette != i) && (output_palette != -1))
		{
			printf("\n***WARNING: -o# is not over-riden, but because -r was selected, anything other than the 8 palettes won't mean much	after all the colors are re-arranged.");
		}
		else
			output_palette=i;
	}

	if(output_palette==-1)
		output_palette=256;

	//Load picture
	if (filebase[strlen(filebase)-4] == '.') {
		filebase[strlen(filebase)-4] = '\0';
	}
	switch (file_type) {
		case 2 : // PCX
			sprintf(filename,"%s.pcx",filebase);
			if (quietmode == 0)
				printf("\nOpening graphics file: [%s]",filename);
			if(!PCX_Load(filename,(pcx_picture_ptr) &image))
				return 1;
			break;
		case 3 : // TGA
			sprintf(filename,"%s.tga",filebase);
			if (quietmode == 0)
				printf("\nOpening graphics file: [%s]",filename);
			if(!TGA_Load(filename,(pcx_picture_ptr) &image))
				return 1;
			break;
		case 4 : // PNG
			sprintf(filename,"%s.png",filebase);
			if (quietmode == 0)
				printf("\nOpening graphics file: [%s]",filename);
			if(!PNG_Load(filename,(pcx_picture_ptr) &image))
				return 1;
			break;
		default : // BMP for everithing else
			sprintf(filename,"%s.bmp",filebase);
			if (quietmode == 0)
				printf("\nOpening graphics file: [%s]",filename);
			if(!BMP_Load(filename,(pcx_picture_ptr) &image))
				return 1;
			break;
	}

	//convert the palette
	ConvertPalette(image.palette, palette);

	height = image.header.height;
	width = image.header.width;

	//autodetect size if necessary
	if((size == 0) && (screen == 0))
	{
		if (quietmode == 0)
			printf("\nAuto-detecting size of image blocks...");

		clr = image.buffer[0]; //get the border color
		for(i=1; i<width; i++)
			if(image.buffer[i]!=clr)
				break;

		xsize = i-1; //if size = n, then xsize = multiple of (n+1)

		for(i=1; i<height; i++)
			if(image.buffer[i*width]!=clr)
				break;

		ysize = i-1; //if size = n, then xsize = multiple of (n+1)

		//look along line 1 until we hit the border color
		//this should be the end of the first image block
		for(i=1; i<width; i++)
			if(image.buffer[i + width]==clr)
				break;

		size = i-1;

		if( (xsize%(size+1) != 0 ) || (ysize%(size+1) != 0 ) )
		{
			printf("\nERROR : Border format is incorrect... autodetect size failed.\n");
			return 1;
		}

		xsize = xsize/(size+1); //the number of blocks across
		ysize = ysize/(size+1); //the number of blocks down
	}
	else
	{
		//determine the constants if in screen mode
		//or image block mode with no borders

		//printf("size=%d screen=%d\n",size,screen);
		if(screen)
			size=8;
		xsize = width/size;
		if(width%size)
			xsize++;

		ysize = height/size;
		if(height%size)
			ysize++;
	}

	//if its a full screen, determine the number of tiles
	if(screen)
	{
		//ALEK 09/08 if(width>256)
			//tile_x=64;
			tile_x=width/8;
		//ALEK 09/08 else
		//ALEK 09/08 	tile_x=32;

		//ALEK 09/08 if(height>256)
			//tile_y=64;
			tile_y=height/8;
		//ALEK 09/08 else
		//ALEK 09/08 	tile_y=32;

		if(screen==7)
		{
			tile_x=128;
			tile_y=128;
		}
	}

	//Print what the user has selected
	if (quietmode == 0) {
		printf("\n****** O P T I O N S ***************");
		if(border)
			printf("\nborder=ON");
		else
			printf("\nborder=OFF");

		if (highpriority)
			printf("\nhighpriority=ON");
		else
			printf("\nhighpriority=OFF");

		if (blanktile)
			printf("\nblanktile=ON");
		else
			printf("\nblanktile=OFF");

		if (collision)
			printf("\ncollisionmap=ON");
		else
			printf("\ncollisionmap=OFF");

		if (tile_reduction)
			printf("\nOptimize tilemap=ON");
		else
			printf("\nOptimize tilemap=OFF");
		
		if (lzpacked)
			printf("\nLZSS compression=ON");
		else
			printf("\nLZSS compression=OFF");
			
		if(packed)
			printf("\npixel format=packed-bit");
		else
			printf("\npixel format=bit-plane");

		if (file_type == 2)
			printf("\nPCX file: %dx%d pixels",width,height);
		else if (file_type == 3)
			printf("\nTGA file: %dx%d pixels",width,height);
		else if (file_type == 4)
			printf("\nPNG file: %dx%d pixels",width,height);
		else
			printf("\nBMP file: %dx%d pixels",width,height);

		if(screen)
		{
			printf("\nScreen mode selected: %dx%d tile map",tile_x,tile_y);
			if(rearrange)
				printf("\nPalette rearrange: ON");
			else
				printf("\nPalette rearrange: OFF");
		}
		else
		{
			printf("\nImage mode selected: %dx%d pixel blocks",size,size);
			printf("\n          image has: %dx%d blocks",xsize,ysize);
		}
		printf("\nTotal colors = %d", colors);

		if(output_palette)
			printf("\nPalette section to convert: Color #0 to %d", output_palette-1);

		if(palette_entry)
			printf("\nPalette entry to be for map tiles: Entry#%d", palette_entry);

		printf("\n************************************");
	}

	//truncate the colors if necessary
	if(colors == 128)
	{
		j = colors - 1;	//color truncation mask
		temp = image.buffer;

		for(i=0;i<width*height;i++)
			temp[i] = temp[i] & j;
	}

	//arrange the blocks according to how we would like them represented.
	if(screen)
	{
		j=xsize;
		num_tiles=ysize;

		//first arrange into a list of 8x8 blocks
		buffer=ArrangeBlocks(image.buffer, width, height,
							 size, &j, &num_tiles, 8, 0);
		free(image.buffer);

		if(buffer==NULL)
		{
			printf("\nERROR:Not enough memory to do image operations...\n");
			return 1;
		}

		//if re-arranging is wanted, attempt it now
		if(rearrange)
		{
			if(!RearrangePalette(buffer, palette, num_tiles, colors))
			{
				free(buffer);
				return 1;
			}
		}

		//make the tile map now
		tilemap=MakeMap(buffer, &num_tiles, xsize, ysize,
					tile_x, tile_y, colors, rearrange, palette_entry);
		if(tilemap==NULL)
		{
			free(buffer);
			printf("\nERROR:Not enough memory to do tile map optimizations..\n");
			return 1;
		}

		if(num_tiles<0)
		{
			num_tiles = -num_tiles;
			blank_absent = 1;
		}

		if((screen == 7) && (num_tiles+blank_absent)>256)
		{
			printf("\nERROR: Need %d tiles to represent the screen in mode7\n       the SNES only has room for 256 tiles" ,num_tiles+blank_absent);
			free(buffer);
			free(tilemap);
			return 1;
		}

		if (quietmode == 0) {
			if (tile_reduction)
				printf("\nReduced screen to %d tiles.",num_tiles+blank_absent);
			else
				printf("\nComputed screen with %d tiles.",num_tiles+blank_absent);
		}
	}
	else //image block mode
	{
		//first arrange in SNES image block format
		buffer=ArrangeBlocks(image.buffer, width, height,
							 size, &xsize, &ysize, 16*8, border);
		free(image.buffer);

		if(buffer==NULL)
		{
			printf("\nERROR:Not enough memory to do image operations...\n");
			return 1;
		}

		//now re-arrange into a list of 8x8 blocks for easy conversion
		xsize *= size/8;
		ysize *= size/8;

		temp=ArrangeBlocks(buffer, xsize*8, ysize*8,
							 8, &xsize, &ysize, 8, 0);
		free(buffer);

		if(temp==NULL)
		{
			printf("\nERROR:Not enough memory to do image operations...\n");
			return 1;
		}

		buffer=temp;
		num_tiles=xsize*ysize;
	}

	//convert pictures and save to file
	if (collision == 0)
	{
		if(!Convert2Pic(filebase, buffer, num_tiles, blank_absent, colors, packed, lzpacked))
		{
			if(screen)
				free(tilemap);
			free(buffer);
			return 1;
		}
	}

	//free up image memory
	free(buffer);

	//save the map
	if ( (screen) && (savemap))
	{
		if(screen==7)
			sprintf(filename,"%s.mp7",filebase);
		else {
			if (collision >= 1)
				sprintf(filename,"%s.clm",filebase);
			else
				sprintf(filename,"%s.map",filebase);
		}

		if (quietmode == 0)
			printf("\nSaving tile map file: [%s]",filename);
		fp = fopen(filename,"wb");
		if(fp==NULL)
		{
			printf("\nERROR: Can't open file [%s] for writing\n",filename);
			free(tilemap);
			return 1;
		}

		for(i=0;i<tile_x*tile_y;i++)
		{
			if(screen==7)
				fputc(tilemap[i]+offset_tile,fp);
			else {
				if (collision == 2) {
					if (tilemap[i]<collisionsp) PutWord(tilemap[i]+offset_tile,fp); else PutWord(0+offset_tile,fp);
				}
				else PutWord((tilemap[i]+offset_tile) | (highpriority<<13),fp);
			}
		}
		fclose(fp);

		// save the sprite table if needed
		if (collision == 2) {
			sprintf(filename,"%s.spr",filebase);

			if (quietmode == 0)
				printf("\nSaving sprite table for map file: [%s]",filename);
			fp = fopen(filename,"wb");
			if(fp==NULL)
			{
				printf("\nERROR: Can't open file [%s] for writing\n",filename);
				free(tilemap);
				return 1;
			}
			for(i=0;i<tile_x*tile_y;i++)
			{
				if (tilemap[i]>=collisionsp) {
					//printf("\ni=%d tile_x=%d collision=%d  %d %d\n",i,tile_x,i % tile_x, i / tile_x,tilemap[i]);
					PutWord(tilemap[i],fp);  // Entry
					PutWord(i % tile_x,fp);  // x coordinate
					PutWord(i / tile_x,fp);  // y coordinate
				}
			}
			fclose(fp);
		}

		//free up tilemap memory
		free(tilemap);
	}

	//convert and save the palette if necessary
	if ((output_palette) && (collision == 0) && (savepalette))
	{
		sprintf(filename,"%s.pal",filebase);
		if (quietmode == 0)
			printf("\nSaving palette file: [%s]",filename);
		fp = fopen(filename,"wb");

		if(fp==NULL)
		{
			printf("\nERROR: Can't open file [%s] for writing\n",filename);
			return 0;
		}

		for(i=0;i<output_palette;i++)
			PutWord(palette[i],fp);

		fclose(fp);
	}

	if (quietmode == 0)
		printf("\nDone !\n\n");

	return 0;
}

