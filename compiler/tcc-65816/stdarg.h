#ifndef _STDARG_H
#define _STDARG_H

typedef char *va_list;

#ifdef __65816__
#define va_start(ap, last) ap = ((char*)&(last)) + sizeof(last)
#define va_arg(ap, type) (ap += sizeof(type), *(type*)(ap - sizeof(type)))
#define va_end(ap)
#define va_copy(aq, ap) ((aq) = (ap))
#else
/* only correct for i386 */
#define va_start(ap, last) ap = ((char *)&(last)) + ((sizeof(last) + 3) & ~3)
#define va_arg(ap, type) (ap += (sizeof(type) + 3) & ~3, *(type *)(ap - ((sizeof(type) + 3) & ~3)))
#define va_copy(dest, src) (dest) = (src)
#define va_end(ap)

#endif
/* fix a buggy dependency on GCC in libio.h */
typedef va_list __gnuc_va_list;
#define _VA_LIST_DEFINED

#endif
