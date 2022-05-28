#ifndef __65816__
#include <stdio.h>
#include <stdint.h>
typedef int32_t __longlong;
typedef uint32_t __ulonglong;
#define int short
typedef int16_t __int;
#else
typedef long long __longlong;
typedef unsigned long long __ulonglong;
typedef int __int;
#endif

typedef unsigned char __uchar;
typedef unsigned int __uint;
