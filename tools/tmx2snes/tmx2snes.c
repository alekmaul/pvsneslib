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

#define TMX2SNESVERSION __BUILD_VERSION
#define TMX2SNESDATE __BUILD_DATE

#define HI_BYTE(n)  (((int)n>>8) & 0x00ff)  // extracts the hi-byte of a word
#define LOW_BYTE(n) ((int)n & 0x00ff)       // extracts the low-byte of a word

//// M A I N   V A R I A B L E S ////////////////////////////////////////////////
int quietmode=0;			// 0 = not quiet, 1 = i can't say anything :P
FILE *fpi,*fpo;				// input and output file handlers
unsigned int filesize;		// input file size
char filebase[256];			// input filename
char filemapname[260];		// output filename for map & objects

int* data;					// data from Tiled layer
cute_tiled_layer_t* layer;	// layers from Tiled  map
cute_tiled_map_t* map;		// map from Tiled

//// F U N C T I O N S //////////////////////////////////////////////////////////

#ifndef __HAS_STRUPR
void strupr(char *str)
{
	while(*str)
	{
		*str=toupper(*str);
		str++;
	}
} 
#endif

//////////////////////////////////////////////////////////////////////////////
void PutWord(int data, FILE *fp)
{
	putc(LOW_BYTE(data),fp);
	putc(HI_BYTE(data),fp);
} //end of PutWord
//////////////////////////////////////////////////////////////////////////////

#if 0
int tab_count = 0;

#define print_tabs() \
    do { for (int j = 0; j < tab_count; ++j) printf("\t"); } while (0)

 

#define print_category(name) \
    do { print_tabs(); printf(name " : \n"); } while (0)

 

#define print(pointer, name, specifier) \
    do { print_tabs(); printf(#name " : " #specifier "\n", pointer -> name); } while (0)

void print_properties(cute_tiled_property_t* properties, int property_count)
{
    print_category("properties");
    ++tab_count;
    for (int i = 0; i < property_count; ++i)
    {
        cute_tiled_property_t* p = properties + i;
        print_tabs();
        printf("%s : ", p->name.ptr);
        switch (p->type)
        {
        case CUTE_TILED_PROPERTY_INT: printf("%d\n", p->data.integer); break;
        case CUTE_TILED_PROPERTY_BOOL: printf("%d\n", p->data.boolean); break;
        case CUTE_TILED_PROPERTY_FLOAT: printf("%f\n", p->data.floating); break;
        case CUTE_TILED_PROPERTY_STRING: printf("%s\n", p->data.string.ptr); break;
        case CUTE_TILED_PROPERTY_FILE: printf("%s\n", p->data.file.ptr); break;
        case CUTE_TILED_PROPERTY_COLOR: printf("%d\n", p->data.color); break;
        case CUTE_TILED_PROPERTY_NONE: printf("CUTE_TILED_PROPERTY_NONE\n"); break;
        }
    }
    --tab_count;
}



void print_objects(cute_tiled_object_t* o)
{
    while (o)
    {
        print_category("object");
        ++tab_count;
        print(o, ellipse, %d);
        print(o, gid, %d);
        print(o, height, %f);
        print(o, id, %d);
        print(o, name.ptr, %s);
        print(o, point, %d);
        print_category("vertices");
        ++tab_count;
        for (int i = 0; i < o->vert_count; i += 2)
        {
            print_tabs();
            printf("%f, %f\n", o->vertices[i], o->vertices[i + 1]);
        }
        --tab_count;

        print(o, vert_type, %d);
        print_properties(o->properties, o->property_count);
        print(o, rotation, %f);
        print(o, type.ptr, %s);
        print(o, visible, %d);
        print(o, width, %f);
        print(o, x, %f);
        print(o, y, %f);
        o = o->next;
        --tab_count;
    }
}

 

void print_layer(cute_tiled_layer_t* layer)
{
    while (layer)
    {
        print_category("layer"); ++tab_count;
        ++tab_count;
        for (int i = 0; i < layer->data_count; ++i) print(layer, data[i], %d);
        --tab_count;
        print(layer, draworder.ptr, %s);
        print(layer, height, %d);
        print(layer, name.ptr, %s);
        print_objects(layer->objects);
        print(layer, opacity, %f);
        print_properties(layer->properties, layer->property_count);
        print(layer, type.ptr, %s);
        print(layer, visible, %d);
        print(layer, width, %d);
        print(layer, x, %d);
        print(layer, y, %d);
        print_layer(layer->layers);
         layer = layer->next;
        --tab_count;
    }
}

void print_tilesets(cute_tiled_tileset_t* tileset)
{
    while (tileset)
    {
        print_category("tileset"); ++tab_count;
        print(tileset, columns, %d);
        print(tileset, firstgid, %d);
        print(tileset, image.ptr, %s);
        print(tileset, imagewidth, %d);
        print(tileset, imageheight, %d);
        print(tileset, margin, %d);
        print(tileset, name.ptr, %s);
        print_properties(tileset->properties, tileset->property_count);
        print(tileset, spacing, %d);
        print(tileset, tilecount, %d);
        print(tileset, tileheight, %d);
        print(tileset, tilewidth, %d);
        print(tileset, type.ptr, %s);
        print(tileset, source.ptr, %s);
        print(tileset, objectalignment.ptr, %s);
        cute_tiled_tile_descriptor_t* tile = tileset->tiles;
        print_category("tiles"); ++tab_count;
        while (tile) {
            print(tile, tile_index, %d);
            print(tile, frame_count, %d);
            print_category("frame"); ++tab_count;
            for (int i = 0; i < tile->frame_count; ++i) {
                cute_tiled_frame_t* frame = tile->animation + i;
                print(frame, duration, %d);
                print(frame, tileid, %d);
            }
            --tab_count;
            print_layer(tile->objectgroup);
            print_properties(tile->properties, tile->property_count);
            print(tile, probability, %f);
            tile = tile->next;
        }
        --tab_count;
        tileset = tileset->next;
        --tab_count;
    }
}

 

void print_map(cute_tiled_map_t* m)
{
    // print all map contents
    print_category("map"); ++tab_count;
    print(m, backgroundcolor, %d);
    print(m, height, %d);
    print(m, infinite, %d);
    print_layer(m->layers);
    print(m, nextobjectid, %d);
    print(m, orientation.ptr, %s);
    print_properties(m->properties, m->property_count);
    print(m, renderorder.ptr, %s);
    print(m, tiledversion.ptr, %s);
    print(m, tileheight, %d);
    print_tilesets(m->tilesets);
    print(m, tilewidth, %d);
    print(m, type.ptr, %s);
    print(m, version, %f);
    print(m, width, %d);
}
#endif

//////////////////////////////////////////////////////////////////////////////
void PrintOptions(char *str)
{
	printf("\n\nUsage : tmx2snes [options] filename ...");
	printf("\n  where filename is a Tiled tmx file");
	
	if(str[0]!=0)
		printf("\n\tmx2snes: error 'The [%s] parameter is not recognized'",str);
	
	printf("\n\nMisc options:");
    printf("\n-h                Display this information");
	printf("\n-q                Quiet mode");
    printf("\n-v                Display version information");
	printf("\n");
	
} //end of PrintOptions()

//////////////////////////////////////////////////////////////////////////////
void PrintVersion(void)
{
	printf("\n\tmx2snes.exe ("TMX2SNESDATE") version "TMX2SNESVERSION"");
    printf("\nCopyright (c) 2022 Alekmaul\n");
}

//////////////////////////////////////////////////////////////////////////////
void WriteMap(void)
{
	if (quietmode == 0)
		printf("tmx2snes: 'Writing tiles map file...'\n");
	sprintf(filemapname,"%s.m16",layer->name.ptr);
	fpo = fopen(filemapname,"wb");
	if(fpo==NULL)
	{	
		printf("tmx2snes: error 'Can't open layer map file [%s] for writing'\n",filemapname);
		return 1;
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
	for (i = 0; i < layer->data_count; i++) {
		// is data not "empty" ?
		if (data[i])
			PutWord(((data[i]-1) & 0xFFFF),fpo);
		// no, write 0
		else
			PutWord(0x0000,fpo);
	}
	// close current layer map file and go to next
	fclose(fpo);
}
void WriteTileset(void)
{

}

/// M A I N ////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	int i;
    
	CUTE_TILED_UNUSED(argc);
    CUTE_TILED_UNUSED(argv);    
	
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
			else
				strcpy(filebase,argv[i]);
		}
	}

	//make sure options are valid
	if( filebase[0] == 0 )
	{
		printf("\tmx2snes: error 'You must specify a base filename'");
		PrintOptions("");
		exit( 1 );
	}

	// open the file
	fpi = fopen(filebase,"rb");
	if(fpi==NULL)
	{
		printf("\tmx2snes: error 'Can't open file [%s]'",filebase);
		exit( 1 );
	}
	if (filebase[strlen(filebase)-4] == '.') {
		filebase[strlen(filebase)-4] = '\0';
	}

	//Print what the user has selected

	// get filesize
	fseek(fpi, 0, SEEK_END);
	filesize = ftell(fpi);
	fseek(fpi, 0, SEEK_SET);

    // load the map in memory
	if (quietmode == 0)
		printf("tmx2snes: 'Loading map: [%s]'\n",filebase);
    map = cute_tiled_load_map_from_file(filebase, 0);
	if (map == NULL) {
		printf("tmx2snes: 'Cannot load map'\n");
		fclose(fpi);
        return 1;
	}

	// close the input file
	fclose(fpi);

	// loop over the map's layers and write them to disk
	if (quietmode == 0)
		printf("tmx2snes: 'Writing layers map & object files...'\n");
	layer = map->layers;
	while (layer) 			{
		if (quietmode == 0)
			printf("tmx2snes: 'Found layer %s...'\n",layer->name.ptr);
		// if it is an entity layer
		if (strcmp(layer->name.ptr,"Entities")==0) {
			if (quietmode == 0)
				printf("tmx2snes: 'Writing entities object file...'\n");
			sprintf(filemapname,"%s.s16",filebase);
			fpo = fopen(filemapname,"wb");
			if(fpo==NULL) {	
				printf("tmx2snes: error 'Can't open layer object file [%s] for writing'\n",filemapname);
				return 1;
			}
/*
def process_entities(map, objlayer):
    ret = list()

    for o in objlayer.objects:
        assert o.type, "Entity must have a type"

        min_tile = int(o.y / TILE_SIZE) * map.width + int(o.x / TILE_SIZE)
        max_tile = int((o.y + o.height - 1) / TILE_SIZE) * map.width + int((o.x + o.width - 1) / TILE_SIZE)

        entity = {
            'x': int(o.x),
            'y': int(o.y),
            'type': int(o.type),
            'name': o.name,
            'minx' : int(o.properties[1].value),
            'maxx' : int(o.properties[0].value),
        }
        ret.append(entity)

    return ret

def write_entities(filename, map, entities):
    player = None
    with open(filename, "wb") as of:

        for e in entities:
            if e['name'].lower() == "player":
                player = e
                of.write(struct.pack('<H', player['x']))
                of.write(struct.pack('<H', player['y']))
                of.write(struct.pack('<H', player['type']))
                of.write(struct.pack('<H', player['minx']))
                of.write(struct.pack('<H', player['maxx']))

        npcs = entities[:]
        if player != None:
          npcs.remove(player)

        assert len(npcs) <= N_NPCS, "Too many NPCs ("+str(len(npcs))+")"

        for n in npcs:
            of.write(struct.pack('<H', n['x']))
            of.write(struct.pack('<H', n['y']))
            of.write(struct.pack('<H', n['type']))
            of.write(struct.pack('<H', n['minx']))
            of.write(struct.pack('<H', n['maxx']))
        of.write(struct.pack('<H', 0xffff))

*/			
			
			
			// close current layer map file and go to next
			fclose(fpo);
		}
		// No it is a map layer
		else {
			// write .m16 file ....
			WriteMap();

        cute_tiled_tile_descriptor_t* tile = tileset->tiles;

/*
def process_tiles(map, layer):
    gidOffset = map.tilesets[0].firstgid
    tiles = layer.tiles
    ret = list()

    for t in tiles:
        assert t.hflip == False, "Doesn't support tile flipping"
        assert t.vflip == False, "Doesn't support tile flipping"
        assert t.dflip == False, "Doesn't support tile flipping"

        a = (t.gid - gidOffset)
        assert a < N_METATILES, "Tile number must be below N_METATILES"
        if a < 0:
            a = 0
        ret.append(a * TILE_MULTIPLIER)
*/

/*
def process_tileset(map_fname,map):
    tile = map.tilesets[0].tiles
    ret = list()

    for t in tile:
      a = 0
      if t.properties[0].value:
        a = int(t.properties[0].value,16)
      ret.append(a)

    with open(map_fname, "wb") as of:
        print("Writing blockers ..." + map_fname )
        for t in ret:
          of.write(struct.pack('<H', t))

		}
*/		
		
		
		layer = layer->next;
	}
	/*
	    with open(map_fname, "wb") as of:
        print("Writing map ..." + map_fname )
        of.write(struct.pack('<H', map.width * map.tilewidth))
        of.write(struct.pack('<H', map.height * map.tileheight))
        of.write(struct.pack('<H', len(tiles) * 2))
        for t in tiles:
            of.write(struct.pack('<H', t))
			*/
	
	// write objects
/*
    with open(filename, "wb") as of:

        for e in entities:
            if e['name'].lower() == "player":
                player = e
                of.write(struct.pack('<H', player['x']))
                of.write(struct.pack('<H', player['y']))
                of.write(struct.pack('<H', player['type']))
                of.write(struct.pack('<H', player['minx']))
                of.write(struct.pack('<H', player['maxx']))

        npcs = entities[:]
        if player != None:
          npcs.remove(player)

        assert len(npcs) <= N_NPCS, "Too many NPCs ("+str(len(npcs))+")"

        for n in npcs:
            of.write(struct.pack('<H', n['x']))
            of.write(struct.pack('<H', n['y']))
            of.write(struct.pack('<H', n['type']))
            of.write(struct.pack('<H', n['minx']))
            of.write(struct.pack('<H', n['maxx']))
        of.write(struct.pack('<H', 0xffff))
*/		

    //print_map(map);
	
	// free the Tiled map object
    cute_tiled_free_map(map);

	if (quietmode == 0)
		printf("tmx2snes: Done 'File converted'\n");

	return 0;
}
