/* The contents of this file are subject to the terms of the
   Common Development and Distribution License, Version 1.0 only. */

#ifndef libbase_basetypes_h
#define libbase_basetypes_h

#include <stdint.h>



namespace base
{

namespace types
{
typedef   int8_t    int8;
typedef  uint8_t   uint8;
typedef  int16_t   int16;
typedef uint16_t  uint16;
typedef  int32_t   int32;
typedef uint32_t  uint32;
typedef  int64_t   int64;
typedef uint64_t  uint64;

typedef unsigned int   uint;
typedef unsigned long  ulong;
} // types


using namespace types;

} // base


#endif
