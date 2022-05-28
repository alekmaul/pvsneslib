/* The contents of this file are subject to the terms of the
   Common Development and Distribution License, Version 1.0 only. */

#include "MemoryStream.h"
#include "StreamException.h"
#include <string.h>

namespace base
{

 MemoryStream::~MemoryStream ()
{
}



void  MemoryStream::close ()
{
Stream::close();
mem.clear();
}



void  MemoryStream::read (void* buf, size_type n)
{
if ((pos + n) > len)
 throw StreamReadException();

memcpy(buf, &mem[pos], n);
pos += n;
}



uint  MemoryStream::read ()
{
if (pos == len)
 throw StreamReadException();

uint r = mem[pos];
++pos;
return r;
}



void  MemoryStream::write (const void* buf, size_type n)
{
pos_type nmax = len - pos;

if (nmax == 0)
 {
 len += n;
 mem.insert(mem.end(), (const uint8*)(buf), (const uint8*)(buf) + n);
 }
else if (n > nmax)
 {
 len += n - nmax;
 memcpy(&mem[pos], buf, nmax);
 mem.insert(mem.end(), (const uint8*)(buf) + nmax, (const uint8*)(buf) + n);
 }
else
 memcpy(&mem[pos], buf, n);

pos += n;
}



void  MemoryStream::write (uint x)
{
if (pos == len)
 {
 ++pos;
 ++len;
 mem.push_back(x);
 }
else
 {
 mem[pos] = x;
 ++pos;
 }
}



void  MemoryStream::pseek ()
{
if (pos > len)
 {
 len = pos;
 mem.resize(len, 0);
 }
}



void  MemoryStream::ptruncate ()
{
mem.resize(len, 0);
}

} // base
