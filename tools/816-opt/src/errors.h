
#ifndef _F816OPT_ERRORS_H
#define _F816OPT_ERRORS_H

//-------------------------------------------------------------------------------------------------
extern void info (const char *format, ...);
extern void warning (const char *format, ...);
extern void fatal (const char *, ...);
extern void errorcontinue (const char *format, ...);

#endif

