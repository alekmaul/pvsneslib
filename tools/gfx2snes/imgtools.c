/*---------------------------------------------------------------------------------

	Copyright (C) 2012-2021
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
#include <malloc.h>

#include "imgtools.h"
#include "gfx2snes.h"

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
		printf("\ngfx2snes: 'ArrangeBlocks width=%d, height=%d, size=%d, *xsize=%d, *ysize=%d, new_width=%d, border=%d'",
			width, height, size, *xsize, *ysize, new_width, border);

	//get number of full image block rows in the new buffer
	rows = (*xsize)*(*ysize)/(new_width/size);   // rows = num_blocks / new_xsize

	//if it doesn't divide evenly, add another full row
	if ( ((*xsize)*(*ysize))%(new_width/size) != 0 )
		rows++;

	if (quietmode == 0)
		printf("\ngfx2snes: 'rows=%d'",rows);

	//get memory for the new buffer
	buffer = malloc( (size_t) rows*size*new_width );
	if(buffer == NULL) {
		printf("\ngfx2snes: error 'Can't allocate enough memory for the buffer in ArrangeBlocks'");
		return 0;
	}

	//initially clear the buffer, so if there are empty image blocks
	//or incomplete blocks, the empty parts will be blank
	memset(buffer,0,(size_t) rows*size*new_width );

	//position in new buffer (x,y) where x and y are in pixel co-ordinates
	x=0;
	y=0;

	if (hi512)
		*xsize >>=1;
		

	//go through each image block(i,j) where i and j are in block co-ordinates
	for(j=0; j < *ysize; j++) {
		for(i=0; i < *xsize; i++)	{
			// find out how much to copy
			// this is needed because the screen mode files may not be
			// a multiple of 8 pixels wide
			// or no-border files may have the wrong width
			num = width - (i*(size+border) + border);
			if(num>size)
				num=size;
			if (hi512) {
				// move each line of the block into the new buffer
				// don't worry about reading past the end of the image here
				// there is an extra 64 lines to read in.
                for(line=0;line<8;line++) 
                {
					memcpy( &buffer[ (y+line)*8  ], &img[ (j*8 + line)*width + i*16],16);
				}
			}
			else {
				// move each line of the block into the new buffer
				// don't worry about reading past the end of the image here
				// there is an extra 64 lines to read in.
				for(line=0;line<size;line++) 
                {
					memcpy( &buffer[ (y+line)*new_width + x ],
						&img[ (j*(size+border) + line + border)*width + i*(size+border) + border ],num);
				}
			}

			// move to the next location in the new buffer
			x+=size;
			if(x >= new_width) {
				x=0;
				y+=size;
			}
		}
  }
	
	*xsize = new_width/size;
	*ysize = rows;

	return buffer;
} //end of ArrangeBlocks()

//////////////////////////////////////////////////////////////////////////////
int *MakeMap(unsigned char *img, int *num_tiles,int *tiletab,
			 int xsize, int ysize, int tile_x, int tile_y, int colors, int rearrange, int pal_entry)
{
	int *map;
	unsigned char blank[128];
	int newtiles;
	int blank_absent;
	int current;	//the current tile we're looking at
	int i,t, palette, j, newadd, tileno;
	int x,y;
	int sizetile;

	//allocate map
	map=malloc((size_t)tile_x*tile_y*sizeof(int));
	if(map==NULL) {
		printf("\ngfx2snes: error 'Can't allocate enough memory for the map in MakeMap'");
		return 0;
	}

	if (quietmode == 0)
		printf("\ngfx2snes: 'MakeMap *num_tiles=%d, xsize=%d, ysize=%d, tile_x=%d, tile_y=%d, colors=%d, rearrange=%d'",
			*num_tiles, xsize, ysize, tile_x, tile_y, colors, rearrange);
			
	//clear map
	memset(map,0,tile_x*tile_y*sizeof(int));

	sizetile = 64;
	if (hi512) {
		xsize>>=1;
		//sizetile = 128;
	}
		
	//if the palette has been rearranged... save the palette number
    j=0;
	//if(rearrange)
	{
		current=0;
		for(y=0;y<ysize;y++)
		for(x=0;x<xsize;x++)
		{
			//get the palette number (0-7 for both 4 & 16 color mode)
			if (colortabinc == 16)
				palette = (img[current*sizetile] >> 4) & 0x07;
			else
				palette = (img[current*sizetile] >> 2) & 0x07;
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
			else if (pagemap32 == 1) {
				//create pages of 32x32
                int x_mult = (x)/32;
                int new_x = x - x_mult * 32;
                int idx = x_mult*1024 + y*32+new_x;
                map[idx]=t;
			}
			else {
        //32x32 or 128x128 screen
			  map[y*tile_x+x]=t;
			}


			//goto the next tile
			current++;
		}

	} //end of if(rearrange)


	//truncate the colors if necessary
	if(colors != 256)
	{
		t = colors - 1;	//color truncation mask

		for(i=0;i<xsize*ysize*sizetile;i++)
        {
			img[i] = img[i] & t;
        }
	}

	//make a blank tile
	memset(blank,0,sizeof(blank));

	//I want tile #0 to be blank..
	//is it?
	if (blanktile==1 )
	{
		if( memcmp(blank,img,sizetile) == 0 )
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
    tiletab[j++]=map[0];

	for(y=0;y<ysize;y++)
    {
        for(x=0;x<xsize;x++)
        {
            newadd=0;           // currerntly we add not the tile to tile table
            
            //if we already processed this, move on
            if(x==0 && y==0)
                continue;

            // if tile reduction
            if (tile_reduction) {
                //is the current tile blank?
                if (( memcmp(blank,&img[current*sizetile],sizetile) == 0 ) && (blanktile==1 ) )
                {
                    t=0;
                }
                else
                {
                    //check for matches with previous tiles if tile_reduction on
                    for(i=0;i<newtiles;i++)
                        if( memcmp(&img[i*sizetile],&img[current*sizetile],sizetile) == 0 )
                            break;

                    //is it a new tile?
                    if(i==newtiles)
                    {
                        // yes -> add it
                        memcpy(&img[newtiles*sizetile],&img[current*sizetile],sizetile);
                        t=newtiles+blank_absent;
                        newtiles++;
                        newadd=1;
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
                memcpy(&img[newtiles*sizetile],&img[current*sizetile],sizetile);
                t=newtiles+blank_absent;
                newtiles++;
            }

            //put tile number in map
            if(tile_x==64 && tile_y==32) // 64x32 screen
            {
                if(x<32) 
                {
                    map[y*32 + x] += t;
                    tileno=map[y*32 + x];
                }
                else 
                {
                    map[(y+32)*32+x-32] += t;
                    tileno=map[(y+32)*32+x-32];
                }
            }
            else if(tile_x==32 && tile_y==64) // 32x64 screen
            {
                map[y*32+x] += t;
                tileno=map[y*32+x];
            }
            else if(tile_x==64 && tile_y==64) // 64x64 screen
            {
                if(y<32) 
                {
                    if(x<32) 
                    {
                        map[y*32+x] += t;
                        tileno=map[y*32+x];
                    }
                    else 
                    {
                        map[(y+32)*32+x-32] += t;
                        tileno=map[(y+32)*32+x-32];
                    }
                }
                else
                {
                    if(x<32) 
                    {
                        map[(y+64-32)*32+x] += t;
                        tileno=map[(y+64-32)*32+x];
                    }
                    else
                    {
                        map[(y+96-32)*32+x-32] += t;
                        tileno=map[(y+96-32)*32+x-32];
                    }
                }
            }
            else if (pagemap32 == 1) 
            {
                //create pages of 32x32
                int x_mult = (x)/32;
                int new_x = x - x_mult * 32;
                int idx = x_mult*1024 + y*32+new_x;
                map[idx]+=t;
                tileno=map[idx];
            }
            else //32x32 or 128x128 screen
            {
                map[y*tile_x+x] += t;
                tileno=map[y*tile_x+x];
            }

            // add also to tile table
            if (newadd) 
            {
                if (j<MAXTILES)
                    tiletab[j]=tileno;
                j++;
                if (j>=MAXTILES) 
                {
                    printf("\ngfx2snes: WARNING 'Too much tiles for map engine (%d)'\n",j);
                }
            }

            //goto the next tile
            current++;
        }
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
int *MakeMapWithTileSet(unsigned char *img, unsigned char *imgtileset,int *num_tiles,int *tiletab,
			 int xsize, int ysize, int tile_x, int tile_y, int colors, int rearrange, int pal_entry)
{
	int *map;
	unsigned char blank[128];
	int blank_absent;
	int current;	//the current tile we're looking at
	int i,t, palette, tilenum, tileno;
	int x,y;
	int sizetile;

	//allocate map
	map=malloc((size_t)tile_x*tile_y*sizeof(int));
	if(map==NULL) {
		printf("\ngfx2snes: error 'Can't allocate enough memory for the map in MakeMap'");
		return 0;
	}

	if (quietmode == 0)
		printf("\ngfx2snes: 'MakeMapWithTileSet *num_tiles=%d, xsize=%d, ysize=%d, tile_x=%d, tile_y=%d, colors=%d, rearrange=%d'",
			*num_tiles, xsize, ysize, tile_x, tile_y, colors, rearrange);
			
	//clear map
	memset(map,0,tile_x*tile_y*sizeof(int));

	sizetile = 64;
	if (hi512) {
		xsize>>=1;
	}
		
	//if the palette has been rearranged... save the palette number
    current=0;
    for(y=0;y<ysize;y++)
    {
        for(x=0;x<xsize;x++)
        {
            //get the palette number (0-7 for both 4 & 16 color mode)
            if (colortabinc == 16)
                palette = (img[current*sizetile] >> 4) & 0x07;
            else
                palette = (img[current*sizetile] >> 2) & 0x07;
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
            else if (pagemap32 == 1) {
                //create pages of 32x32
                int x_mult = (x)/32;
                int new_x = x - x_mult * 32;
                int idx = x_mult*1024 + y*32+new_x;
                map[idx]=t;
            }
            else {
              //32x32 or 128x128 screen
              map[y*tile_x+x]=t;
            }


            //goto the next tile
            current++;
        }
    }

	//truncate the colors if necessary
	if(colors != 256)
	{
		t = colors - 1;	//color truncation mask

		for(i=0;i<xsize*ysize*sizetile;i++)
        {
			img[i] = img[i] & t;
        }
	}

	//make a blank tile
	memset(blank,0,sizeof(blank));

	//I want tile #0 to be blank..
	//is it?
	if (blanktile==1 )
	{
		if( memcmp(blank,img,sizetile) == 0 )
		{
			blank_absent=0;
			current=1;
			t=0;
		}
		else
		{
			blank_absent=1;
			current=1;
			t=1;
		}
	}
	else
	{
		blank_absent=0;
		current=1;
		t=0;
	}

	for(y=0;y<ysize;y++)
    {
        for(x=0;x<xsize;x++)
        {
            tilenum=0;           // currerntly we add not the tile to tile table
            
            //is the current tile blank?
            if (( memcmp(blank,&img[current*sizetile],sizetile) == 0 ) && (blanktile==1 ) )
            {
                t=0;
            }
            else
            {
                //check for matches with previous tiles if tile_reduction on
                for(i=0;i<xsize*ysize;i++)
                {
                    if( memcmp(&imgtileset[i*sizetile],&img[current*sizetile],sizetile) == 0 ) 
                    {
                        tilenum=i;
                        break;
                    }
                }

                //is it a new tile?
                if(i==xsize*ysize)
                {
                    // yes -> error, can't be in this state
                    printf("\ngfx2snes: warning 'Can't find tile %d in tileset'",i);
                    t=0;
                }
                else
                {	// no -> find what tile number it is
                    t=i+blank_absent;
                }
            }

            //put tile number in map
            if(tile_x==64 && tile_y==32) // 64x32 screen
            {
                if(x<32) 
                {
                    map[y*32 + x] += t;
                    tileno=map[y*32 + x];
                }
                else 
                {
                    map[(y+32)*32+x-32] += t;
                    tileno=map[(y+32)*32+x-32];
                }
            }
            else if(tile_x==32 && tile_y==64) // 32x64 screen
            {
                map[y*32+x] += t;
                tileno=map[y*32+x];
            }
            else if(tile_x==64 && tile_y==64) // 64x64 screen
            {
                if(y<32) 
                {
                    if(x<32) 
                    {
                        map[y*32+x] += t;
                        tileno=map[y*32+x];
                    }
                    else 
                    {
                        map[(y+32)*32+x-32] += t;
                        tileno=map[(y+32)*32+x-32];
                    }
                }
                else
                {
                    if(x<32) 
                    {
                        map[(y+64-32)*32+x] += t;
                        tileno=map[(y+64-32)*32+x];
                    }
                    else
                    {
                        map[(y+96-32)*32+x-32] += t;
                        tileno=map[(y+96-32)*32+x-32];
                    }
                }
            }
            else if (pagemap32 == 1) 
            {
                //create pages of 32x32
                int x_mult = (x)/32;
                int new_x = x - x_mult * 32;
                int idx = x_mult*1024 + y*32+new_x;
                map[idx]+=t;
                tileno=map[idx];
            }
            else //32x32 or 128x128 screen
            {
                map[y*tile_x+x] += t;
                tileno=map[y*tile_x+x];
            }

            // add also to tile table
            tiletab[tilenum]=tileno;

            //goto the next tile
            current++;
        }
    }
    
	//also return the number of new tiles
	//make it negative if we need to add the blank tile
	if (blank_absent)
		*num_tiles = -xsize*ysize;
	else
		*num_tiles = xsize*ysize;

	return map;

}//end of MakeMapWithTileSet

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
		printf("\ngfx2snes: error 'Not enough memory to do rearrangement calculations'\n");
		return 0;
	}

	combos=malloc(num_tiles*16*sizeof(int));
	if(combos==NULL)
	{
		printf("\ngfx2snes: error 'Not enough memory to do rearrangement calculations'\n");
		free(num);
		return 0;
	}

	list=malloc(num_tiles*sizeof(int));
	if(list==NULL)
	{
		printf("\ngfx2snes: error 'Not enough memory to do rearrangement calculations'\n");
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
					printf("\ngfx2snes: error 'Detected more colors in one 8x8 tile than is allowed'\n");
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
			printf("\ngfx2snes: error 'not enough colors/palettes to represent the picture'\n");
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
		printf("\ngfx2snes: 'Rearrangement possible!! Accomplished in %d palettes...'", num_final);

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
		printf("\ngfx2snes: 'Saving graphics file: [%s]'",filename);
	fp = fopen(filename,"wb");

	if(fp==NULL)
	{
		printf("\ngfx2snes: error 'Can't open file [%s] for writing'\n",filename);
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

	//find the number of bitplanes (default is 8 for 128 & 256 colors)
	bitplanes=8;
	if(colors==4)
		bitplanes=2;
	else if(colors==16)
		bitplanes=4;
	else if(colors<=128)
		bitplanes=4;

	// if lzss encoding begin with transferring data
	if (lzsspacked) {
		bufsize=8*bitplanes+num_tiles*8*bitplanes;
		buftolzin = malloc (bufsize);
		if (buftolzin == NULL) {
			printf("\ngfx2snes: error 'Can't allocate enough memory for the buffer comrpession 1'");
			fclose(fp);
			return 0;
		}
		buftolzout = malloc (bufsize);
		if (buftolzout == NULL) {
			printf("\ngfx2snes: error 'Can't allocate enough memory for the buffer comrpession 2'");
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
		if (blank_absent)
			for(i=0;i<8*bitplanes;i++)
				fputc(0,fp);

        if (quietmode == 0)
            printf("\ngfx2snes: 'decode for %d tiles and %d bitplanes'\n",num_tiles,bitplanes);

		for(t=0;t<num_tiles;t++) //loop through tiles
		for(b=0;b<bitplanes;b+=2) //loop through bitplane pairs
		for(y=0;y<8;y++)
		{
			//get bit-mask
			mask = 1 << b;
			data = 0;

			//get row of bit-plane and save row
			for(x=0;x<8;x++)
			{
				data = data << 1;
				if(buffer[t*64 + y*8 + x] & mask)
					data = data+1;
			}
			fputc(data,fp);

			//adjust bit-mask
			mask = mask << 1;
			data = 0;

			//get row of next bit-plane and save row
			for(x=0;x<8;x++)
			{
				data = data << 1;
				if(buffer[t*64  + y*8 + x] & mask)
					data = data+1;
			}
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