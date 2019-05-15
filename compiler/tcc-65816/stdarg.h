#ifndef _STDARG_H
#define _STDARG_H

typedef char *va_list;
typedef va_list __builtin_va_list;

#ifdef __65816__
#define va_start(ap, last) ap = ((char*)&(last)) + sizeof(last)
#define va_arg(ap, type) (ap += sizeof(type), *(type*)(ap - sizeof(type)))
#define va_end(ap)
#define va_copy(aq, ap) ((aq) = (ap))
#else
/* only correct for i386 */
#define va_start(ap,last) ap = ((char *)&(last)) + ((sizeof(last)+3)&~3)
#define va_arg(ap,type) (ap += (sizeof(type)+3)&~3, *(type *)(ap - ((sizeof(type)+3)&~3)))
#define va_end(ap)

/* fix a buggy dependency on GCC in libio.h */
typedef va_list __gnuc_va_list;
#endif

#define __builtin_va_start va_start
#define __builtin_va_arg va_arg
#define __builtin_va_end va_end

#define _VA_LIST_DEFINED

#endif
