/*---------------------------------------------------------------------------------

	Copyright (C) 2012-2022
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
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#include <string.h>

#include "gfx2snes.h"

#include "imgtools.h"
#include "lodepng.h"
#include "loadimg.h"

#define RED_ERROR "\e[0;31merror\e[0m"	// ANSI color codes for more noticeable errors

int	border=1;						// options and their defaults
int	packed=0;						//
int size=0;							//
int screen=0;						//
int colors=0;						//
int output_palette=-1;	            //
int rearrange=0;				    //
int palette_entry=0;		        //
int file_type=1;			    	// 1 = bmp, 2 = pcx, 3 = tga, 4 = png
int quietmode=0;			    	// 0 = not quiet, 1 = i can't say anything :P
int collision=0;			    	// 1 = generated only collision map
int collisionsp=0;			        // n = 1st sprite entry regarding the map (so remove it from collision map)
int tile_reduction=1;	        	// 0 = no tile reduction (warning !)
int savepalette=1;		           	// 1 = save the palette
int savemap=1;				    	// 1 = save the map
int colortabinc=16;                 // 16 for 16 color mode, 4 for 4 color mode
int lzpacked=0;                     // 1 = compress file with LZSS algorithm
int highpriority=0;                 // 1 = high priority for map
int blanktile=0;                    // 1 = blank tile generated
int palette_rnd=0;                  // 1 = round palette up & down
int offset_tile=0;		            // n = offset in tile number
int pagemap32 = 0;                  // 1 = create tile maps organized in 32x32 pages
int hi512 = 0;      		        // 1 = create a 512 width map for mode 5 & 6

pcx_picture image;                  // file with image to convert

char filebase[256];                 // name of output file
char filename[256+4];               // use to manage filenames

//////////////////////////////////////////////////////////////////////////////
void PutWord(int data, FILE *fp)
{
	putc(LOW_BYTE(data),fp);
	putc(HI_BYTE(data),fp);
} //end of PutWord

//////////////////////////////////////////////////////////////////////////////
int LoadPicture(void)
{
	if (filebase[strlen(filebase)-4] == '.')
    {
		filebase[strlen(filebase)-4] = '\0';
	}
	switch (file_type) {
		case 2 : // PCX
			sprintf(filename,"%s.pcx",filebase);
			if (quietmode == 0)
				printf("\ngfx2snes: 'Opening graphics file: [%s]'",filename);
			if(!PCX_Load(filename,(pcx_picture_ptr) &image))
				return 1;
			break;
		case 3 : // TGA
			sprintf(filename,"%s.tga",filebase);
			if (quietmode == 0)
				printf("\ngfx2snes: 'Opening graphics file: [%s]'",filename);
			if(!TGA_Load(filename,(pcx_picture_ptr) &image))
				return 1;
			break;
		case 4 : // PNG
			sprintf(filename,"%s.png",filebase);
			if (quietmode == 0)
				printf("\ngfx2snes: 'Opening graphics file: [%s]'",filename);
			if(!PNG_Load(filename,(pcx_picture_ptr) &image))
				return 1;
			break;
		default : // BMP for everything else
			sprintf(filename,"%s.bmp",filebase);
			if (quietmode == 0)
				printf("\ngfx2snes: 'Opening graphics file: [%s]'",filename);
			if(!BMP_Load(filename,(pcx_picture_ptr) &image))
				return 1;
			break;
	}
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
void PrintOptions(char *str)
{
	printf("\n\nUsage : gfx2snes [options] bmp/pcx/tga filename ...");
	printf("\n  where filename is a 256 color PNG, BMP, PCX or TGA file");

	if(str[0]!=0)
		printf("\ngfx2snes: %s 'The [%s] parameter is not recognized'", RED_ERROR, str);

	printf("\n\nOptions are:");
	printf("\n\n--- Graphics options ---");
	printf("\n-gb               add blank tile management (for multiple bgs)");
	printf("\n-gp               Output in packed pixel format");
	printf("\n-glz              Output in lz77 compressed pixel format");
	printf("\n-gs(8|16|32|64)   Size of image blocks in pixels [8]");
	printf("\n\n--- Map options ---");
	printf("\n-m!               Exclude map from output");
	printf("\n-m                Convert the whole picture");
	printf("\n-mp               Convert the whole picture with high priority");
	printf("\n-m7               Convert the whole picture for mode 7 format");
	printf("\n-m5               Convert the whole picture for mode 5 & 6 512 width hires");
	printf("\n-mc               Generate collision map only");
	printf("\n-ms#              Generate collision map only with sprites table");
	printf("\n                   where # is the 1st tile corresponding to a sprite (0 to 255)");
	printf("\n-mn#              Generate the whole picture with an offset for tile number");
	printf("\n                   where # is the offset in decimal (0 to 2047)");
	printf("\n-mR!              No tile reduction (not advised)");
	printf("\n-m32p             Generate tile map organized in pages of 32x32 (good for scrolling)");
	printf("\n-me               Convert the map for PVSneslib map engine");
	printf("\n\n--- Palette options ---");
	printf("\n-p!               Exclude palette from output.");
	printf("\n-pc(4|16|128|256) The number of colors to use [256]");
	printf("\n-po#              The number of colors to output (0 to 256) to the filename.pal");
	printf("\n-pe#              The palette entry to add to map tiles (0 to 16)");
	printf("\n-pr               Rearrange palette, and preserve palette numbers in the tilemap");
	printf("\n-pR               Palette rounding");
	printf("\n\n--- File options ---");
	printf("\n-f[bmp|pcx|tga|png]   convert a bmp or pcx or tga or png file [bmp]");
	printf("\n\n--- Misc options ---");
	printf("\n-n                no border");
	printf("\n-q                quiet mode");
    printf("\n-v                Display gfx2snes version information");
	printf("\n");

} //end of PrintOptions()

//////////////////////////////////////////////////////////////////////////////
void PrintVersion(void)
{
	printf("\ngfx2snes.exe ("GFX2SNESVERSION") version "GFX2SNESDATE"");
    printf("\nCopyright (c) 2013-2021 Alekmaul");
    printf("\nBased on pcx2snes by Neviksti\n");
}

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

	unsigned char *buffer;
	unsigned char *temp;
	int *tilemap;
    int tileobj[MAXTILES];
	FILE *fp;

	int i, j;

    // init all filenames
    strcpy(filebase,"");
    strcpy(filename,"");

	// init all buffers
	buffer=NULL;

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
				else if( strcmp(&arg[i][1],"m5") == 0)
				{
					screen=1;
					border=0;
					hi512=1;
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
				else if( strcmp(&arg[i][1],"m32p") == 0)
				{
					screen=1;
					border=0;
					pagemap32 = 1;
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
			else if (arg[i][1]=='f') //file type specification
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
			else if (arg[i][1]=='v') //version information
            {
                PrintVersion();
				return 0;
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
		printf("\ngfx2snes: %s 'You must specify a base filename'", RED_ERROR);
		PrintOptions("");
		return 1;
	}

	if( colors == 0 )
	{
		printf("\ngfx2snes: %s 'The Number of Colors parameter must be specified'", RED_ERROR);
		PrintOptions("");
		return 1;
	}

	if((size == 0) && (border == 0) && (screen == 0))
	{
		printf("\ngfx2snes: %s 'The Size parameter must be specified when the border is turned off'", RED_ERROR);
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
			printf("\n***gfx2snes: WARNING '-c128 and -c256 override the -r option. The palette will not be rearranged'");
			rearrange=0;
		}
		if(screen == 0)
		{
			printf("\n***gfx2snes: warning 'the -r option means nothing in image block mode. The palette will not be rearranged'");
			rearrange=0;
		}
		else if((output_palette != i) && (output_palette != -1))
		{
			printf("\n***gfx2snes: warning '-o# is not over-riden, but because -r was selected, anything other than the 8 palettes won't mean much	after all the colors are re-arranged'");
		}
		else
			output_palette=i;
	}

	if(output_palette==-1)
		output_palette=256;

	//Load picture
    if (LoadPicture())
    {
        return 1;
    }
    else
    {
        //convert the palette
        ConvertPalette(image.palette, palette);

        height = image.header.height;
        width = image.header.width;
    }

	// go out if size if not multiple of 8
	if ( ((width%8) != 0) || ((height%8) !=0) )
	{
		printf("\ngfx2snes: error 'size %d x %d is not a multiple of 8 pixels for width or height.'\n",width,height);
		return 1;
	}
	
	//autodetect size if necessary
	if((size == 0) && (screen == 0))
	{
		if (quietmode == 0)
			printf("\ngfx2snes: %s 'Auto-detecting size of image blocks...'", RED_ERROR);

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
			printf("\ngfx2snes: %s 'Border format is incorrect... autodetect size failed.'\n", RED_ERROR);
			return 1;
		}

		xsize = xsize/(size+1); //the number of blocks across
		ysize = ysize/(size+1); //the number of blocks down
	}
	else
	{
		//determine the constants if in screen mode
		//or image block mode with no borders
		if (screen) {
			size=8;

			// Get out if hires and not 512 width
			if( (hi512) &&  (width != 512) )
			{
				printf("\ngfx2snes: %s 'EHiRes mode 5 format is not 512 pixels width.'\n", RED_ERROR);
				return 1;
			}
		}
		xsize = width/size;
		if (width%size)
			xsize++;

		ysize = height/size;
		if (height%size)
			ysize++;
	}

	//if its a full screen, determine the number of tiles
    tile_x=tile_y=32;  // default value
	if(screen)
	{
        //ALEK 09/08 if(width>256)
        //tile_x=64;
        //ALEK 01/11/18 tile_x=width/8;
        tile_x=width/size;
        if (hi512)
            tile_x>>=1;
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
        printf("\ngfx2snes: ("GFX2SNESVERSION") version "GFX2SNESDATE"");
		printf("\ngfx2snes: ****** O P T I O N S ***************");
		printf(border ? "\ngfx2snes: border=ON" : "'\ngfx2snes: border=OFF");
		printf(highpriority ? "\ngfx2snes: highpriority=ON" : "\ngfx2snes: highpriority=OFF");
		printf(blanktile ? "\ngfx2snes: blanktile=ON" : "\ngfx2snes: blanktile=OFF");
		printf(collision ? "\ngfx2snes: collisionmap=ON" : "\ngfx2snes: collisionmap=OFF");
		printf(pagemap32 ? "\ngfx2snes: tile map pages of 32x32=ON" : "\ngfx2snes: tile map pages of 32x32=OFF");
		printf(hi512 ? "\ngfx2snes: tile map mode 512=ON" : "\ngfx2snes: tile map mode 512=OFF");
		printf(tile_reduction ? "\ngfx2snes: Optimize tilemap=ON" : "\ngfx2snes: Optimize tilemap=OFF");
		printf(lzpacked ? "\ngfx2snes: LZSS compression=ON" : "\ngfx2snes: LZSS compression=OFF");
		printf(packed ? "\ngfx2snes: pixel format=packed-bit" : "\ngfx2snes: pixel format=bit-plane");

		if (file_type == 2)
			printf("\ngfx2snes: 'PCX file: %dx%d pixels'",width,height);
		else if (file_type == 3)
			printf("\ngfx2snes: TGA file: %dx%d pixels",width,height);
		else if (file_type == 4)
			printf("\ngfx2snes: PNG file: %dx%d pixels",width,height);
		else
			printf("\ngfx2snes: BMP file: %dx%d pixels",width,height);

		if(screen)
		{
			printf("\ngfx2snes: Screen mode selected: %dx%d tile map",tile_x,tile_y);
			printf(rearrange ? "\ngfx2snes: Palette rearrange: ON" : "\ngfx2snes: Palette rearrange: OFF");
		}
		else
		{
			printf("\ngfx2snes: Image mode selected: %dx%d pixel blocks",size,size);
			printf("\ngfx2snes:           image has: %dx%d blocks",xsize,ysize);
		}
		printf("\ngfx2snes: Total colors = %d", colors);

		if(output_palette)
			printf("\ngfx2snes: Palette section to convert: Color #0 to %d", output_palette-1);

		if(palette_entry)
			printf("\ngfx2snes: Palette entry to be for map tiles: Entry#%d", palette_entry);

		printf("\ngfx2snes: ************************************");
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
	tilemap=NULL;
    memset(tileobj,0x00,sizeof(tileobj));
    if(screen)
	{
		j=xsize;
		num_tiles=ysize;

		//first arrange into a list of 8x8 blocks
        buffer=ArrangeBlocks(image.buffer, width, height, size, &j, &num_tiles, (hi512 ? 16 : 8), 0);
        free(image.buffer);

		if(buffer==NULL)
		{
			printf("\ngfx2snes: %s 'Not enough memory to do image operations...'\n", RED_ERROR);
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
		tilemap=MakeMap(buffer, &num_tiles, (int *) &tileobj, xsize, ysize, tile_x, tile_y, colors, rearrange, palette_entry);
		if(tilemap==NULL)
		{
			free(buffer);
			printf("\ngfx2snes: %s 'Not enough memory to do tile map optimizations...'\n", RED_ERROR);
			return 1;
		}

		if(num_tiles<0)
		{
			num_tiles = -num_tiles;
			blank_absent = 1;
		}

		if((screen == 7) && (num_tiles+blank_absent)>256)
		{
			printf("\ngfx2snes: %s 'Need %d tiles to represent the screen in mode7. SNES only has room for 256 tiles'", RED_ERROR, num_tiles+blank_absent);
			free(tilemap);
			return 1;
		}

		if (quietmode == 0) {
			if (tile_reduction)
				printf("\ngfx2snes: 'Reduced screen to %d tiles'",num_tiles+blank_absent);
			else
				printf("\ngfx2snes: 'Computed screen with %d tiles'",num_tiles+blank_absent);
		}
	}
	else //image block mode
	{
		//first arrange in SNES image block format
		buffer=ArrangeBlocks(image.buffer, width, height, size, &xsize, &ysize, 16*8, border);
		free(image.buffer);

		if(buffer==NULL)
		{
			printf("\ngfx2snes: %s 'Not enough memory to do image operations...'\n", RED_ERROR);
			return 1;
		}

		//now re-arrange into a list of 8x8 blocks for easy conversion
		xsize *= size/8;
		ysize *= size/8;

		temp=ArrangeBlocks(buffer, xsize*8, ysize*8, 8, &xsize, &ysize, 8, 0);
		free(buffer);

		if(temp==NULL)
		{
			printf("\ngfx2snes: %s 'Not enough memory to do image operations...'\n", RED_ERROR);
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
			{
				free(tilemap);
			}
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
			printf("\ngfx2snes: 'Saving tile map file: [%s]'",filename);
		fp = fopen(filename,"wb");
		if(fp==NULL)
		{
			printf("\ngfx2snes: %s 'Can't open file [%s] for writing'\n", RED_ERROR, filename);
			free(tilemap);
			return 1;
		}

        // Little warning for more than one bank
        if ( (tile_x*tile_y*2) >=32768)
        {
            printf("\ngfx2snes: WARNING 'Map is too big for 1 32KB bank (%d)'\n",tile_x*tile_y);
        }

        // add all the tiles
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
				printf("\ngfx2snes: 'Saving sprite table for map file: [%s]'",filename);
			fp = fopen(filename,"wb");
			if(fp==NULL)
			{
				printf("\ngfx2snes: %s Can't open file [%s] for writing'\n", RED_ERROR, filename);
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
			printf("\ngfx2snes: 'Saving palette file: [%s]'",filename);
		fp = fopen(filename,"wb");

		if(fp==NULL)
		{
			printf("\ngfx2snes: %s 'Can't open file [%s] for writing'\n", RED_ERROR, filename);
			return 0;
		}

		for(i=0;i<output_palette;i++)
			PutWord(palette[i],fp);

		fclose(fp);
	}

	if (quietmode == 0)
		printf("\ngfx2snes: 'Done !'\n\n");

	return 0;
}

