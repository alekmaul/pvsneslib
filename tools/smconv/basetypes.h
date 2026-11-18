#ifndef __BASETYPES_H
#define __BASETYPES_H

#include <stdbool.h>
#include <stddef.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

#define ERRORRED(STRING) "\x1B[31m" STRING "\033[0m"
#define ERRORPINK(STRING) "\x1B[35m" STRING "\033[0m"
#define ERRORBRIGHT(STRING) "\x1B[97m" STRING "\033[0m"

// allocation helpers (exit with a message if memory is exhausted)
void *xmalloc(size_t size);
void *xcalloc(size_t count, size_t size);
void *xrealloc(void *ptr, size_t size);
char *xstrdup(const char *str);

#endif
