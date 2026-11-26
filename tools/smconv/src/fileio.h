// read and write a binary file with stdio

#ifndef __FILEIO_H
#define __FILEIO_H

#include <stdio.h>
#include "basetypes.h"

typedef enum
{
    MODE_READ,
    MODE_WRITE
} FileAccessMode;

typedef struct
{
    FILE *file;
    bool isOpen;
    FileAccessMode mode;
} File;

// open a file (the structure does not need to be initialized before)
bool file_open(File *f, const char *filename, FileAccessMode m);
void file_close(File *f);

u8 file_read8(File *f);
u16 file_read16(File *f);
u32 file_read32(File *f);

void file_write8(File *f, u8 data);
void file_write16(File *f, u16 data);
void file_write32(File *f, u32 data);

// write non-terminated ascii
void file_write_ascii(File *f, const char *str);

// write fixed length string (zero padded)
void file_write_asciif(File *f, const char *str, int length);

void file_zero_fill(File *f, int amount);

void file_write_align(File *f, u32 boundary);

void file_skip(File *f, s32 amount);
u32 file_tell(File *f);
void file_seek(File *f, u32 offset);

bool file_is_open(File *f);

u32 file_size(const char *filename);

#endif
