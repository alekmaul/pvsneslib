/*---------------------------------------------------------------------------------

	Copyright (C) 2022
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

    Convert Tiled tmx file to binary files compatible with pvsneslib
	
	Tiled is a tool to make graphic maps based on tiles
		https://www.mapeditor.org/

---------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CUTE_TILED_IMPLEMENTATION
#include "cute_tiled.h"

#define TMX2SNESVERSION             __BUILD_VERSION
#define TMX2SNESDATE                __BUILD_DATE

#define HI_BYTE(n)                  (((int)n>>8) & 0x00ff)  // extracts the hi-byte of a word
#define LOW_BYTE(n)                 ((int)n & 0x00ff)       // extracts the low-byte of a word

#define N_METATILES                 1024					// maximum tiles
#define N_OBJECTS                   64						// maximum objects

//// M A I N   V A R I A B L E S ////////////////////////////////////////////////
typedef struct 
{
	int x;                            		// x coordinate in pixels.
	int y;                           		// y coordinate in pixels.
	int class;         						// class of object (0=main character, 1..63 other classes)
	int minx;                          		// horizontal or vertical min x coordinate in pixels.
	int maxx;                         		// horizontal or vertical max x coordinate in pixels.
} pvsneslib_object_t;

int quietmode=0;			       			// 0 = not quiet, 1 = i can't say anything :P
FILE *fpi,*fpo;				        		// input and output file handlers
unsigned int filesize;		        		// input file size
char filebase[256];			        		// input filename
char filemapname[260];		        		// output filename for map & objects

int* data;					        		// data from Tiled layer
cute_tiled_layer_t *layer;	        		// layers from Tiled  map
cute_tiled_tileset_t *tset;					// tileset from Tiled
cute_tiled_object_t *objm;          		// objects from Tiled layer objects
cute_tiled_map_t *map;		        		// map from Tiled
cute_tiled_tile_descriptor_t *tile; 		// tiles from Tiled tiles attributes
cute_tiled_property_t *propm;       		// properties from Tiled tiles properties
unsigned short tileprop[N_METATILES][3];	// to store tiles properties in correct order with index 0:attribute, 1:priority and 2:palette
pvsneslib_object_t objsnes[N_OBJECTS];		// to store objects in correct order

//// F U N C T I O N S //////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void PutWord(int data, FILE *fp)
{
	putc(LOW_BYTE(data),fp);
	putc(HI_BYTE(data),fp);
} //end of PutWord

//////////////////////////////////////////////////////////////////////////////
void PrintOptions(char *str)
{
	printf("\n\nUsage : tmx2snes [options] filename ...");
	printf("\n  where filename is a Tiled tmx file");
	printf("\n\n  tmx2snes will do:");
	printf("\n  	.m16 file for map");
	printf("\n  	.b16 file for tile attributes");
	printf("\n  	.o16 file for objects");
	
	if(str[0]!=0)
		printf("\ntmx2snes: error 'The [%s] parameter is not recognized'",str);
	
	printf("\n\nMisc options:");
    printf("\n-h                Display this information");
	printf("\n-q                Quiet mode");
    printf("\n-v                Display version information");
	printf("\n");
	
} //end of PrintOptions()

//////////////////////////////////////////////////////////////////////////////
void PrintVersion(void)
{
	printf("\ntmx2snes.exe ("TMX2SNESDATE") version "TMX2SNESVERSION"");
    printf("\nCopyright (c) 2022 Alekmaul\n");
}

//////////////////////////////////////////////////////////////////////////////
void WriteMap(void)
{
    int tileattr, tilesnes, i;
	char *lastpostslash;
	
    // We use directory and replace file name with layer name
	strcpy(filemapname,filebase);
    lastpostslash = strrchr( filemapname, '/' );
	sprintf(lastpostslash+1,"%s.m16",layer->name.ptr);

	if (quietmode == 0)
		printf("tmx2snes:     Writing tiles map file...\n");
	//sprintf(filemapname,"%s.m16",layer->name.ptr);
	fpo = fopen(filemapname,"wb");
	if(fpo==NULL)
	{	
		printf("tmx2snes: error 'Can't open layer map file [%s] for writing'\n",filemapname);
		exit(1);
	}
	// Put width & heigh
	PutWord(map->width*map->tilewidth,fpo);
	PutWord(map->height*map->tileheight,fpo);

	// Put number of tiles
	PutWord(layer->data_count*2,fpo);

	// Write tile data to file 
	// Tiled data format for flipping : flip x ->0x8000 0000 flip y -> 0x4000 0000
	// SNES format :
	// 		High     Low          Legend->  c: Starting character (tile) number
	// 		vhopppcc cccccccc               h: horizontal flip  v: vertical flip
	//				                        p: palette number   o: priority bit
	data = layer->data;
	fflush(stdout);
	for (i = 0; i < layer->data_count; i++) {
		// is data not "empty" ?
        tileattr=data[i];
		if (tileattr) {
            tilesnes=(tileattr-1) & 0x03FF; // keep on the low 16bits of tile number

            if (tileattr & CUTE_TILED_FLIPPED_HORIZONTALLY_FLAG) // Flipx attribute
				printf("tmx2snes: warning ! 'flipx attribute is not supported'\n");
            if (tileattr & CUTE_TILED_FLIPPED_VERTICALLY_FLAG) // Flipy attribute
				printf("tmx2snes: warning ! 'flipy attribute is not supported'\n");
			
			PutWord(tilesnes,fpo);
        }
		// no (certainly an error in the map with no tile assignment), write 0
		else
			PutWord(0x0000,fpo);
	}
	// close current layer map file 
	fclose(fpo);
}

void WriteTileset(void)
{
	int i,blkprop;
	char *pend;
	
	if (quietmode == 0)
		printf("tmx2snes: Writing tiles attribute file...\n");
	sprintf(filemapname,"%s.b16",filebase);
	fpo = fopen(filemapname,"wb");
	if(fpo==NULL)
	{	
		printf("tmx2snes: error 'Can't open tiles attribute file [%s] for writing'\n",filemapname);
		exit(1);
	}

	// Write tile properties to file 
    // 2 datas are currently managed : priority & block attribute
	// tiles with library is in reverse order (why ?)
    tset=map->tilesets;
	tile = tset->tiles;
    
	if (tset->tilecount>N_METATILES)
	{	
		printf("tmx2snes: error 'too much tiles in tileset (%d tiles, %d max expected)'\n",tset->tilecount,N_METATILES);
		exit(1);
	}

	// browse and store in table
	fflush(stdout);
	memset(tileprop,0x00,sizeof(tileprop));
	while (tile) {
		// browse through all propperties of current tile
        for (i = 0; i < tile->property_count; i++) {
            propm = tile->properties + i;
			// write attribute (blocker, etc..) property (which is a string)
			if (strcmp(propm->name.ptr,"attribute")==0) {
				blkprop=(unsigned short) strtol(propm->data.string.ptr,&pend,16);
				tileprop[tile->tile_index][0]=blkprop;
			}
			// write priority property (which is a string)
			if (strcmp(propm->name.ptr,"priority")==0) {
				blkprop=(unsigned short) strtol(propm->data.string.ptr,&pend,16);
				tileprop[tile->tile_index][1]=blkprop;
			}
			// write palette property (which is a string)
			if (strcmp(propm->name.ptr,"palette")==0) {
				blkprop=(unsigned short) strtol(propm->data.string.ptr,&pend,16);
				tileprop[tile->tile_index][2]=blkprop;
			}
		}
        // switch to next tile
        tile = tile->next;
    }
	
	// now write to file
	if (quietmode == 0)
		printf("tmx2snes:     Writing %d tiles attributes to file...\n",tset->tilecount);
	for (i = 0; i < tset->tilecount; i++) {
		PutWord(tileprop[i][0],fpo);
	}
	
	// close current layer atribute file 
	fclose(fpo);

	// now write tileset file with properties (only priority for the moment)
	sprintf(filemapname,"%s.t16",filebase);
	fpo = fopen(filemapname,"wb");
	if(fpo==NULL)
	{	
		printf("tmx2snes: error 'Can't open tiles properties file [%s] for writing'\n",filemapname);
		exit(1);
	}
	if (quietmode == 0)
		printf("tmx2snes:     Writing %d tiles properties to file...\n",tset->tilecount);
	for (i = 0; i < tset->tilecount; i++) {
		// compute attribute to match with vhopppcccccccccc
		blkprop=tileprop[i][1] ? 0x2000 : 0x0000;		// check priority
		blkprop|=(tileprop[i][2] << 10); // checkpalette
		PutWord(blkprop+i,fpo);
	}

	// close current layer atribute file 
	fclose(fpo);

}

void WriteEntities(void)
{
	int i,blkprop, objidx;
	char *pend;

    if (quietmode == 0)
        printf("tmx2snes: Writing entities object file...\n");
    sprintf(filemapname,"%s.o16",filebase);
    fpo = fopen(filemapname,"wb");
    if(fpo==NULL) {	
        printf("tmx2snes: error 'Can't open layer object file [%s] for writing'\n",filemapname);
        exit(1);
    }

    // write objects to file
    objm=layer->objects;
	if (layer->object_count>N_OBJECTS)
	{	
		printf("tmx2snes: error 'too much entities in map (%d entities, %d max expected)'\n",layer->object_count,N_OBJECTS);
		exit(1);
	}

	// browse and store in table
	fflush(stdout);
	memset(objsnes,0x00,sizeof(objsnes));
	objidx=layer->object_count-1;
	
	// if we have some objects to store
	if (layer->object_count) {
		while (objm) {
			// put object in reverse order 
			objsnes[objidx].class=atoi(objm->type.ptr); //(unsigned short) strtol(objm->class.ptr,&pend,10);
			objsnes[objidx].x=(int) (objm->x);
			objsnes[objidx].y=(int) (objm->y);
			for (i = 0; i < objm->property_count; i++) {
				propm = objm->properties + i;
				// write blocker property (which is a string)
				if (strcmp(propm->name.ptr,"minx")==0) {
					blkprop=(unsigned short) strtol(propm->data.string.ptr,&pend,10);
					objsnes[objidx].minx=blkprop;
				}
				// write prio property (which is a string)
				if (strcmp(propm->name.ptr,"maxx")==0) {
					blkprop=(unsigned short) strtol(propm->data.string.ptr,&pend,10);
					objsnes[objidx].maxx=blkprop;
				}
			}

			// switch to next object
			objm = objm->next;
			objidx--;
		}
	}

	// now write to file
	if (quietmode == 0)
		printf("tmx2snes:     Writing %d objects to file...\n",layer->object_count);
	for (i = 0; i < layer->object_count; i++) {
		PutWord(objsnes[i].x,fpo);
		PutWord(objsnes[i].y,fpo);
		PutWord(objsnes[i].class,fpo);
		PutWord(objsnes[i].minx,fpo);
		PutWord(objsnes[i].maxx,fpo);
	}
	PutWord(0xFFFF,fpo);

	// close current layer map file 
	fclose(fpo);
}

/// M A I N ////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	int i;
    
	CUTE_TILED_UNUSED(argc);
    CUTE_TILED_UNUSED(argv);    
	
    // init all filenames
    strcpy(filebase,"");
    strcpy(filemapname,"");

	//parse the arguments
	for(i=1;i<argc;i++)
	{
		if(argv[i][0]=='-')
		{
			if(argv[i][1]=='v') // show version
            {
                PrintVersion();
                exit( 0 );
            }
			else if(argv[i][1]=='h') // show help
            {
                PrintOptions((char *) "");
                exit( 0 );
            }
			else if(argv[i][1]=='q') //quiet mode
			{
				quietmode=1;
			}
			else //invalid option
			{
				PrintOptions(argv[i]);
				exit( 1 );
			}
		}
		else
		{
			//its not an option flag, so it must be the filebase
			if(filebase[0]!=0) // if already defined... there's a problem
			{
				PrintOptions(argv[i]);
				exit( 1 );
			}
			else {
				strcpy(filebase,argv[i]);
			}
		}
	}

	//make sure options are valid
	if( filebase[0] == 0 )
	{
		printf("\ntmx2snes: error 'You must specify a base filename'");
		PrintOptions("");
		exit( 1 );
	}

	// open the file
	fpi = fopen(filebase,"rb");
	if(fpi==NULL)
	{
		printf("\ntmx2snes: error 'Can't open file [%s]'",filebase);
		exit( 1 );
	}

	// get filesize
	fseek(fpi, 0, SEEK_END);
	filesize = ftell(fpi);
	fseek(fpi, 0, SEEK_SET);


    // load the map in memory
	if (quietmode == 0)
		printf("tmx2snes: Loading map: [%s]\n",filebase);
    map = cute_tiled_load_map_from_file(filebase, 0);
	if (map == NULL) {
		printf("tmx2snes: error 'Cannot load map'\n");
		fclose(fpi);
        return 1;
	}

	// close the input file
	fclose(fpi);

	// test some attributes of Tiled map
	float currentExportSupportedVersion = 1.9;
	if(map->version != currentExportSupportedVersion) {
		printf("tmx2snes: error 'the export version you used (%.1f) is not yet supported. The tool supports only the %.1f version.'\n",map->version, currentExportSupportedVersion);
		return 1;
	}

	if ((map->width*map->height)>16384) {
		printf("tmx2snes: error 'map is too big (max 32K)! (%dK)'\n",(map->width*map->height*2)/1024);
        return 1;
	}
	if (map->height>256) {
		printf("tmx2snes: error 'map height is too big! (max 256) (%d)'\n",map->height);
        return 1;
	}
	if ((map->tilewidth!=8) || (map->tileheight!=8)) {
		printf("tmx2snes: error 'tile width or height are not 8px! (%d %d)\n",map->tilewidth, map->tileheight);
        return 1;
	}

	// remove filename extension (tmj or json)
	if (filebase[strlen(filebase)-5] == '.') {
		filebase[strlen(filebase)-5] = '\0';
	}
	else if (filebase[strlen(filebase)-4] == '.') {
		filebase[strlen(filebase)-4] = '\0';
	}

	// Print what the user has selected
	printf("\n<layername>.m16 file for map, used by pvsneslib 'mapLoad' function as 1st argument (only 1 layer)\n");
	printf("%s.b16 file for tile attributes, used by pvsneslib 'mapLoad' function  as 3rd argument\n",filebase);
	printf("%s.o16 file for objects, used by pvsneslib 'objLoadObjects' as argument\n\n",filebase);

	// loop over the map's layers and write them to disk
	if (quietmode == 0)
		printf("tmx2snes: Writing layers map & object files...\n");
	layer = map->layers;

	// write .b16 file first (to have priority flag of each tile for map...
	WriteTileset();

	while (layer) 			{
		if (quietmode == 0)
			printf("tmx2snes: Found layer %s...\n",layer->name.ptr);

		// if it is an entity layer
		if (strcmp(layer->name.ptr,"Entities")==0) {
            // Write .o16 file ...
		    WriteEntities();
		}
		// No it is a map layer
		else {
			// write .m16 file ...
			WriteMap();
        }

		layer = layer->next;
	}
	
	// free the Tiled map object
    cute_tiled_free_map(map);

	if (quietmode == 0)
		printf("tmx2snes: Done 'File converted'\n");

	return 0;
}
