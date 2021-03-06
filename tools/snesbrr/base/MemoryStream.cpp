/* The contents of this file are subject to the terms of the
   Common Development and Distribution License, Version 1.0 only. */

#include "MemoryStream.h"
#include "StreamException.h"


namespace base
{

FASTCALL MemoryStream::~MemoryStream ()
{
}



void FASTCALL MemoryStream::close ()
{
Stream::close();
mem.clear();
}



void FASTCALL MemoryStream::read (void* buf, size_type n)
{
if ((pos + n) > len)
 throw StreamReadException();

//std::memcpy(buf, &mem[pos], n);
memcpy(buf, &mem[pos], n);
pos += n;
}



uint FASTCALL MemoryStream::read ()
{
if (pos == len)
 throw StreamReadException();

uint r = mem[pos];
++pos;
return r;
}



void FASTCALL MemoryStream::write (const void* buf, size_type n)
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
 //std::memcpy(&mem[pos], buf, nmax);
 memcpy(&mem[pos], buf, nmax);
 mem.insert(mem.end(), (const uint8*)(buf) + nmax, (const uint8*)(buf) + n);
 }
else
 //std::memcpy(&mem[pos], buf, n);
 memcpy(&mem[pos], buf, n);

pos += n;
}



void FASTCALL MemoryStream::write (uint x)
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



void FASTCALL MemoryStream::pseek ()
{
if (pos > len)
 {
 len = pos;
 mem.resize(len, 0);
 }
}



void FASTCALL MemoryStream::ptruncate ()
{
mem.resize(len, 0);
}

} // base
