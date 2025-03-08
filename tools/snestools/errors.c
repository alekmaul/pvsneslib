/*---------------------------------------------------------------------------------

    Copyright (C) 2012-2024
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

    Header checker / modifier for snes.
    Some parts are based on Snes mess driver.

---------------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "errors.h"

#define ERRORRED(STRING) "\x1B[31m" STRING "\033[0m"
#define ERRORPINK(STRING) "\x1B[35m" STRING "\033[0m"
#define ERRORBRIGHT(STRING) "\x1B[97m" STRING "\033[0m"

//-------------------------------------------------------------------------------------------------
// Print an info message - output produced, that's all
void info (const char *format, ...)
{
  va_list ap;

  va_start (ap, format);
  fprintf (stdout, "%s: ", ERRORBRIGHT("snestools"));
  vfprintf (stdout, format, ap);
  va_end (ap);
  fputc ('\n', stdout);
  fflush(stdout);
}

//-------------------------------------------------------------------------------------------------
// Print a warning message - output produced, but there may be problems.
void warning (const char *format, ...)
{
  va_list ap;

  va_start (ap, format);
  fprintf (stderr, "%s: " ERRORPINK("warning") ": ", ERRORBRIGHT("snestools"));
  vfprintf (stderr, format, ap);
  va_end (ap);
  fputc ('\n', stderr);
  fflush(stderr);
}

//-------------------------------------------------------------------------------------------------
// Print an error message - output produced, but terminate execution is elsewhere.
void errorcontinue (const char *format, ...)
{
  va_list ap;

  va_start (ap, format);
  fprintf (stderr, "%s: " ERRORRED("error") ": ", ERRORBRIGHT("snestools"));
  vfprintf (stderr, format, ap);
  va_end (ap);
  fputc ('\n', stderr);
  fflush(stderr);
}

//-------------------------------------------------------------------------------------------------
// Fatal error - terminate execution immediately.  Does not return. 
void fatal (const char *format, ...)
{
  va_list ap;

  va_start (ap, format);
  fprintf (stderr, "%s: " ERRORRED("fatal error") ": ", ERRORBRIGHT("snestools"));
  vfprintf (stderr, format, ap);
  va_end (ap);
  fputc ('\n', stderr);
  exit (EXIT_FAILURE);
}