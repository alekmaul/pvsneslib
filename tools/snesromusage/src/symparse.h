#ifndef _SNESROMUSAGE_SYMPARSE_H
#define _SNESROMUSAGE_SYMPARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>

#include "errors.h"

//-------------------------------------------------------------------------------------------------
#define MAX_LABEL_LEN 512
#define BAR_WIDTH 28

#define PFX_START "SECTIONSTART_"
#define PFX_END   "SECTIONEND_"


typedef enum { 
    MODE_AUTO, MODE_LOROM, MODE_HIROM, MODE_EXHIROM 
} RomMode;

typedef enum { 
    MEM_ROM, MEM_WRAM, MEM_SRAM, MEM_UNKNOWN 
} MemType;

//-------------------------------------------------------------------------------------------------
typedef struct {                                            // Reposting
    char     label[40];
    char     type[8];
    unsigned int range_lo, range_hi;                            //inclusive, informational only
    unsigned int size;
    unsigned int used;
} t_reportrow;

typedef struct {    
    unsigned int addr;                                          // 24-bit flat SNES address
    char     name[MAX_LABEL_LEN];
} t_synbol;

typedef struct {                                            // t_section instances (paired SECTIONSTART_/SECTIONEND_)
    char     key[MAX_LABEL_LEN];                            // name after the SECTIONSTART_/SECTIONEND_ prefix 
    unsigned int start_addr;
} t_opensection;

typedef struct {
    char     name[MAX_LABEL_LEN];                           // section name (without SECTIONSTART_/END_ prefix)
    unsigned int start_addr;
    unsigned int end_addr;                                      // exclusive 
    unsigned int size;
    MemType  type;
    int      bank;                                          // physical bank this was folded onto, or raw bank for WRAM/unknown
} t_section;

/*typedef struct { 
    t_section *s; 
} 
t_sortref;*/

extern size_t  g_symbol_count;
extern size_t   g_section_count;

//-------------------------------------------------------------------------------------------------
extern void load_sym_file(const char *path, bool isquiet);
extern void build_sections(bool isquiet);
extern void display_symbols(int forcemode, RomMode moderom, int topsection, bool showsection, bool isquiet);

#endif
