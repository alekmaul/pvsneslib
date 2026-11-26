
#ifndef _SMCONV_ERRORS_H
#define _SMCONV_ERRORS_H

//-------------------------------------------------------------------------------------------------
extern void info (const char *format, ...);
extern void warning (const char *format, ...);
extern void fatal (const char *, ...);
extern void errorcontinue (const char *format, ...);

#endif

