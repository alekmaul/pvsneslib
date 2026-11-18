#ifndef __INPUTDATA_H
#define __INPUTDATA_H

// param parser

#include <stdlib.h>
#include <stdio.h>
#include "basetypes.h"

typedef struct
{
    char *output; // NULL if not specified
    bool hirom;
    const char **files;
    int file_count;
    bool spc_mode;
    bool verbose_mode;
    bool show_help;
    bool show_version;
    bool check_effect_size;
    int banknumber;
} OperationData;

void operation_data_init(OperationData *od, int argc, char *argv[]);
void operation_data_free(OperationData *od);

#endif
