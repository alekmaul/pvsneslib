#include <stdlib.h>
#include <string.h>

#include "io.h"

void *xmalloc(size_t size)
{
    void *p = malloc(size ? size : 1);
    if (!p)
    {
        printf("%s: " ERRORRED("fatal error") ": out of memory\n", ERRORBRIGHT("smconv"));
        exit(EXIT_FAILURE);
    }
    return p;
}

void *xcalloc(size_t count, size_t size)
{
    void *p = calloc(count ? count : 1, size ? size : 1);
    if (!p)
    {
        printf("%s: " ERRORRED("fatal error") ": out of memory\n", ERRORBRIGHT("smconv"));
        exit(EXIT_FAILURE);
    }
    return p;
}

void *xrealloc(void *ptr, size_t size)
{
    void *p = realloc(ptr, size ? size : 1);
    if (!p)
    {
        printf("%s: " ERRORRED("fatal error") ": out of memory\n", ERRORBRIGHT("smconv"));
        exit(EXIT_FAILURE);
    }
    return p;
}

char *xstrdup(const char *str)
{
    size_t len = strlen(str) + 1;
    char *p = (char *)xmalloc(len);
    memcpy(p, str, len);
    return p;
}

bool file_open(File *f, const char *filename, FileAccessMode m)
{
    f->mode = m;
    f->file = fopen(filename, m ? "wb" : "rb");
    f->isOpen = f->file ? true : false;
    return f->isOpen;
}

void file_close(File *f)
{
    if (f->isOpen)
    {
        fclose(f->file);
        f->file = 0;
        f->isOpen = false;
    }
}

u8 file_read8(File *f)
{
    u8 a[1] = {0};
    if (f->mode == MODE_WRITE || (!f->isOpen))
        return 0;
    if (fread(a, 1, 1, f->file) != 1)
        a[0] = 0;
    return a[0];
}

u16 file_read16(File *f)
{
    u16 a = file_read8(f);
    a |= file_read8(f) << 8;
    return a;
}

u32 file_read32(File *f)
{
    u32 a = file_read16(f);
    a |= (u32)file_read16(f) << 16;
    return a;
}

void file_skip(File *f, s32 amount)
{
    if (f->isOpen)
    {
        if (f->mode == MODE_WRITE)
        {
            for (u32 i = amount; i; i--)
            {
                file_write8(f, 0);
            }
        }
        else
        {
            fseek(f->file, amount, SEEK_CUR);
        }
    }
}

void file_write8(File *f, u8 data)
{
    if (f->isOpen && f->mode == MODE_WRITE)
    {
        fwrite(&data, 1, 1, f->file);
    }
}

void file_write16(File *f, u16 data)
{
    file_write8(f, data & 0xFF);
    file_write8(f, data >> 8);
}

void file_write32(File *f, u32 data)
{
    file_write16(f, data & 0xFFFF);
    file_write16(f, data >> 16);
}

void file_write_ascii(File *f, const char *str)
{
    for (int i = 0; str[i]; i++)
    {
        file_write8(f, str[i]);
    }
}

void file_write_asciif(File *f, const char *str, int length)
{
    int i;
    for (i = 0; str[i] && i < length; i++)
    {
        file_write8(f, str[i]);
    }
    for (; i < length; i++)
    {
        file_write8(f, 0);
    }
}

void file_zero_fill(File *f, int amount)
{
    for (int i = 0; i < amount; i++)
    {
        file_write8(f, 0);
    }
}

void file_write_align(File *f, u32 boundary)
{
    int skip = file_tell(f) % boundary;
    if (skip)
    {
        file_skip(f, boundary - skip);
    }
}

void file_seek(File *f, u32 offset)
{
    if (f->isOpen)
        fseek(f->file, offset, SEEK_SET);
}

u32 file_tell(File *f)
{
    if (f->isOpen)
        return ftell(f->file);
    else
        return 0;
}

bool file_is_open(File *f)
{
    return f->isOpen;
}

u32 file_size(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f)
    {
        return 0;
    }
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fclose(f);
    return size;
}
