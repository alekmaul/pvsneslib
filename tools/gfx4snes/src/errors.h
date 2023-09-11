
#ifndef _GFX4SNES_ERRORS_H
#define _GFX4SNES_ERRORS_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-------------------------------------------------------------------------------------------------
extern void info (const char *format, ...);
extern void warning (const char *format, ...);
extern void fatal (const char *, ...);

#endif

