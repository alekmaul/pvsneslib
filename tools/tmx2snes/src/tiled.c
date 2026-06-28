/*---------------------------------------------------------------------------------

    Copyright (C) 2022-2026
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
#include "tiled.h"

#define CUTE_TILED_IMPLEMENTATION
#include "cute_tiled.h"

int *data;                          // data from Tiled layer
cute_tiled_layer_t *layer;          // layers from Tiled  map
cute_tiled_tileset_t *tset;         // tileset from Tiled
cute_tiled_object_t *objm;          // objects from Tiled layer objects
cute_tiled_map_t *map;              // map from Tiled
cute_tiled_tile_descriptor_t *tile; // tiles from Tiled tiles attributes
cute_tiled_property_t *propm;       // properties from Tiled tiles properties
unsigned short tileprop[N_METATILES][3];    // to store tiles properties in correct order with index 0:attribute, 1:priority and 2:palette
pvsneslib_object_t objsnes[N_OBJECTS];      // to store objects in correct order
unsigned short tilesetmap[N_METATILES];     // to have map for each tile (optimization purpose)

//// F U N C T I O N S //////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void PutWord(int data, FILE *fp)
{
    putc(LOW_BYTE(data), fp);
    putc(HI_BYTE(data), fp);
} // end of PutWord

// Structure representing a software version number.
typedef struct
{
    int major; // Major version number.
    int minor; // Minor version number.
} Version;

// Convert a floating point version number to its major and minor components.
void floatToVersion(float version, Version *v)
{

    // Convert the floating point version to an integer representation by multiplying by 100.
    // This allows us to handle two decimal places. Additionally, add 0.5 to round to the nearest integer
    // to account for potential floating point inaccuracies.
    int fullVersion = (int)(version * 100 + 0.5);

    // The major version is obtained by dividing the integer representation by 100.
    v->major = fullVersion / 100;

    // The minor version is obtained by taking the remainder when divided by 100.
    v->minor = fullVersion % 100;
}

// Compare two Version structures to determine their order.
int compareVersions(Version v1, Version v2)
{
    // Compare the major version numbers first.
    if (v1.major < v2.major)
        return -1;
    if (v1.major > v2.major)
        return 1;

    // If major version numbers are equal, compare the minor version numbers.
    if (v1.minor < v2.minor)
        return -1;
    if (v1.minor > v2.minor)
        return 1;

    // Both major and minor version numbers are equal.
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
void WriteMap(const char *filename, bool isquiet)
{
    int tileattr, tilesnes, i;
    char *lastpostslash;
   	char *outputname;
    FILE *fpo;

    // prepare file extension
	outputname=(char *) malloc(FILENAME_MAX); 
	if(outputname==NULL)
	{
		fatal("can't allocate memory for bmp filename");
	}
	//snprintf(outputname, FILENAME_MAX, "%s.m16", layer->name.ptr);

    // We use directory and replace file name with layer name
    strncpy(outputname, filename, FILENAME_MAX - 1);
    outputname[sizeof(outputname) - 1] = '\0'; 
    lastpostslash = strrchr(outputname, '/');
    if (lastpostslash != NULL) {
        size_t prefix_len = (lastpostslash + 1) - outputname;
        snprintf(lastpostslash + 1, FILENAME_MAX - prefix_len, "%s.m16", layer->name.ptr);
    }
    else
        snprintf(outputname, FILENAME_MAX, "%s.m16", layer->name.ptr);

    if (!isquiet) info("Writing tiles map file...");
    fpo = fopen(outputname, "wb");
    if (fpo == NULL)
    {
        fatal("Can't open layer map file [%s] for writing", outputname);
    }
    // Put width & height
    PutWord(map->width * map->tilewidth, fpo);
    PutWord(map->height * map->tileheight, fpo);

    // Put number of tiles
    PutWord(layer->data_count * 2, fpo);

    // Write tile data to file
    // Tiled data format for flipping : flip x ->0x8000 0000 flip y -> 0x4000 0000
    // SNES format :
    // 		High     Low          Legend->  c: Starting character (tile) number
    // 		vhopppcc cccccccc               h: horizontal flip  v: vertical flip
    //				                        p: palette number   o: priority bit
    data = layer->data;
    for (i = 0; i < layer->data_count; i++)
    {
        // is data not "empty" ?
        tileattr = data[i];
        if (tileattr)
        {
            tilesnes = (tileattr - 1) & 0x03FF; // keep on the low 16bits of tile number

            if (tileattr & CUTE_TILED_FLIPPED_HORIZONTALLY_FLAG) // Flipx attribute
                tilesnes |= (1 << 14);
            if (tileattr & CUTE_TILED_FLIPPED_VERTICALLY_FLAG) // Flipy attribute
                tilesnes |= (1 << 15);

            PutWord(tilesnes, fpo);
        }
        // no (certainly an error in the map with no tile assignment), write 0
        else
            PutWord(0x0000, fpo);
    }
    // close current layer map file and cleanup memory
    fclose(fpo);
    free(outputname);
}

//-------------------------------------------------------------------------------------------------
void WriteTileset(const char *filename, bool isquiet)
{
    int i, blkprop;
    char *pend;
	char *outputname;
    FILE *fpo;

    // prepare file extension
	outputname=(char *) malloc(FILENAME_MAX); 
	if(outputname==NULL)
	{
		fatal("can't allocate memory for bmp filename");
	}
	snprintf(outputname, FILENAME_MAX, "%s.b16", filename);

    if (!isquiet) info("Writing tiles attribute file...");
    fpo = fopen(outputname, "wb");
    if (fpo == NULL)
    {
        fatal("Can't open tiles attribute file [%s] for writing", outputname);
    }

    // Write tile properties to file
    // 2 data are currently managed : priority & block attribute
    // tiles with library is in reverse order (why ?)
    tset = map->tilesets;
    tile = tset->tiles;

    if (tset->tilecount > N_METATILES)
    {
        fclose(fpo);
        free(outputname);
        fatal("too much tiles in tileset (%d tiles, %d max expected)", tset->tilecount, N_METATILES);
    }

    // browse and store in table
    memset(tileprop, 0x00, sizeof(tileprop));
    while (tile)
    {
        // browse through all properties of current tile
        for (i = 0; i < tile->property_count; i++)
        {
            propm = tile->properties + i;
            // bounds check to prevent buffer overflow
            if (tile->tile_index >= N_METATILES)
            {
                fclose(fpo);
                free(outputname);
                fatal("tile index %d exceeds maximum %d'", tile->tile_index, N_METATILES - 1);
            }
            // write attribute (blocker, etc..) property (which is a string)
            if (strcmp(propm->name.ptr, "attribute") == 0)
            {
                blkprop = (unsigned short)strtol(propm->data.string.ptr, &pend, 16);
                tileprop[tile->tile_index][0] = blkprop;
            }
            // write priority property (which is a string)
            if (strcmp(propm->name.ptr, "priority") == 0)
            {
                blkprop = (unsigned short)strtol(propm->data.string.ptr, &pend, 16);
                tileprop[tile->tile_index][1] = blkprop;
            }
            // write palette property (which is a string)
            if (strcmp(propm->name.ptr, "palette") == 0)
            {
                blkprop = (unsigned short)strtol(propm->data.string.ptr, &pend, 16);
                tileprop[tile->tile_index][2] = blkprop;
            }
        }
        // switch to next tile
        tile = tile->next;
    }

    // now write to file
    fflush(stdout);
    if (!isquiet) info("Writing %d tiles attributes to file...", tset->tilecount);

    for (i = 0; i < tset->tilecount; i++)
    {
        PutWord(tileprop[i][0], fpo);
    }

    // close current layer attribute file and cleanup memory
    fclose(fpo);
   	free(outputname);
}

//-------------------------------------------------------------------------------------------------
void WriteMapTileset(const char *filename, bool isquiet)
{
    int i, blkprop;
	char *outputname;
    FILE *fpo;

    // prepare file extension
	outputname=(char *) malloc(FILENAME_MAX); 
	if(outputname==NULL)
	{
		fatal("can't allocate memory for bmp filename");
	}
	snprintf(outputname, FILENAME_MAX, "%s.t16", filename);

    // now write tileset file with properties (only priority & palette for the moment)
    fpo = fopen(outputname, "wb");
    if (fpo == NULL)
    {
        fatal("Can't open tiles properties file [%s] for writing", outputname);
    }

    // now write to file
    fflush(stdout);
    if (!isquiet) info("Writing %d tiles properties to file...", tset->tilecount);

    for (i = 0; i < tset->tilecount; i++)
    {
        // compute attribute to match with vhopppcc cccccccc
        blkprop = tilesetmap[i] & 0x03FF;            // get tile number
        blkprop |= tileprop[i][1] ? 0x2000 : 0x0000; // check priority
        blkprop |= (tileprop[i][2] << 10);           // check palette
        PutWord(blkprop, fpo);
    }

    // close current layer attribute file and cleanup memory
    fclose(fpo);
   	free(outputname);
}

//-------------------------------------------------------------------------------------------------
void WriteEntities(const char *filename, bool isquiet)
{
    int i, blkprop, objidx;
    char *pend;
	char *outputname;
    FILE *fpo;

    // prepare file extension
	outputname=(char *) malloc(FILENAME_MAX); 
	if(outputname==NULL)
	{
		fatal("can't allocate memory for bmp filename");
	}
	snprintf(outputname, FILENAME_MAX, "%s.o16", filename);


    if (!isquiet) info("Writing entities object file...");
    fpo = fopen(outputname, "wb");
    if (fpo == NULL)
    {
        fatal("Can't open layer object file [%s] for writing", outputname);
    }

    // write objects to file
    objm = layer->objects;
    if (layer->object_count > N_OBJECTS)
    {
        fclose(fpo);
       	free(outputname);
        fatal("too much entities in map (%d entities, %d max expected)",layer->object_count,N_OBJECTS);
    }

    // browse and store in table
    fflush(stdout);
    memset(objsnes, 0x00, sizeof(objsnes));
    objidx = layer->object_count - 1;

    // if we have some objects to store
    if (layer->object_count)
    {
        while (objm)
        {
            // put object in reverse order
            objsnes[objidx].type = atoi(
                objm->type.ptr); //(unsigned short) strtol(objm->type.ptr,&pend,10);
            objsnes[objidx].x = (int)(objm->x);
            objsnes[objidx].y = (int)(objm->y);
            for (i = 0; i < objm->property_count; i++)
            {
                propm = objm->properties + i;
                // write blocker property (which is a string)
                if (strcmp(propm->name.ptr, "minx") == 0)
                {
                    blkprop = (unsigned short)strtol(propm->data.string.ptr, &pend, 10);
                    objsnes[objidx].minx = blkprop;
                }
                // write prio property (which is a string)
                if (strcmp(propm->name.ptr, "maxx") == 0)
                {
                    blkprop = (unsigned short)strtol(propm->data.string.ptr, &pend, 10);
                    objsnes[objidx].maxx = blkprop;
                }
            }

            // switch to next object
            objm = objm->next;
            objidx--;
        }
    }

    // now write to file
    if (!isquiet) printf("Writing %d objects to file...", layer->object_count);
    for (i = 0; i < layer->object_count; i++)
    {
        PutWord(objsnes[i].x, fpo);
        PutWord(objsnes[i].y, fpo);
        PutWord(objsnes[i].type, fpo);
        PutWord(objsnes[i].minx, fpo);
        PutWord(objsnes[i].maxx, fpo);
    }
    PutWord(0xFFFF, fpo);

    // close current layer map file and cleanup memory
    fclose(fpo);
   	free(outputname);
}

//-------------------------------------------------------------------------------------------------
void tmx_load(const char *tmxname, const char *tilemapname, bool isquiet) 
{
    FILE *fpi;                                                          // input file handlers
    unsigned int filesize;                                              // input file size
	char *outputname;

  	// prepare file extension
	outputname=(char *) malloc(FILENAME_MAX); //malloc(strlen(filename)+4);							// 4 to be sure to have enough for extension
	if(outputname==NULL)
	{
		fatal("can't allocate memory for tmj filename");
	}
	snprintf(outputname, FILENAME_MAX, "%s.tmj", tmxname);

    // open the tmx file
    fpi = fopen(outputname, "rb");
    if (fpi == NULL)
    {
        fatal("Can't open tmx file [%s]", outputname);
    }

    // get filesize
    fseek(fpi, 0, SEEK_END);
    filesize = ftell(fpi);
    fseek(fpi, 0, SEEK_SET);

    // load the map in memory
    if (!isquiet) info("Loading map: [%s]",outputname);
    map = cute_tiled_load_map_from_file(outputname, 0);
    if (map == NULL)
    {
        fclose(fpi);
        free(outputname);
        fatal("Cannot load map");
    }

    // close the input file
    fclose(fpi);

    // open the tileset map file
   	snprintf(outputname, FILENAME_MAX, "%s.map", tilemapname);
    fpi = fopen(outputname, "rb");
    if (fpi == NULL)
    {
        fatal("Can't open map file [%s]", outputname);
    }

    // get filesize
    fseek(fpi, 0, SEEK_END);
    filesize = ftell(fpi);
    fseek(fpi, 0, SEEK_SET);

    if (filesize > N_METATILES * 2) // no more than nb metatiles in words
    {
        fclose(fpi);
        free(outputname);
        fatal("tileset map file is too big [%d bytes]", filesize);
    }

    // read the map
    if (fread(tilesetmap, filesize, 1, fpi) != 1)
    {
        fclose(fpi);
        free(outputname);
        fatal("failed to read tileset map file");
    }

    // close the input file
    fclose(fpi);

    if ((map->width * map->height) > 16384)
    {
        free(outputname);
        fatal("map is too big (max 32K)! (%dK)", (map->width * map->height * 2) / 1024);
    }
    if (map->height > 256)
    {
        free(outputname);
        fatal("map height is too big! (max 256) (%d)", map->height);
    }
    if ((map->tilewidth != 8) || (map->tileheight != 8))
    {
        free(outputname);
        fatal("tile width or height are not 8px! (%d %d)",map->tilewidth,map->tileheight);
    }

    // Print what the user has selected
    if (!isquiet) info("<layername>.m16 file for map, used by pvsneslib 'mapLoad' function as 1st argument (only 1 layer)");
    if (!isquiet) info("%s.b16 file for tile attributes, used by pvsneslib 'mapLoad' function  as 3rd argument", tmxname);
    if (!isquiet) info("%s.o16 file for objects, used by pvsneslib 'objLoadObjects' as argument", tmxname);

    // loop over the map's layers and write them to disk
    if (!isquiet) info("Writing layers map & object files...");
    layer = map->layers;

    // write .b16 file first (to have priority flag of each tile for map...
    WriteTileset(tmxname, isquiet);

    while (layer)
    {
        if (!isquiet) info("Found layer %s...", layer->name.ptr);

        // if it is an entity layer
        if (strcmp(layer->name.ptr, "Entities") == 0)
        {
            // Write .o16 file ...
            WriteEntities(tmxname, isquiet);
        }
        // No it is a map layer
        else
        {
            // write .m16 and .t16 files ...
            WriteMap(tmxname, isquiet);
            WriteMapTileset(tmxname, isquiet);
        }

        layer = layer->next;
    }

    // free the Tiled map object
    cute_tiled_free_map(map);
   	free(outputname);
}
