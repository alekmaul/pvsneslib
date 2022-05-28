/* The contents of this file are subject to the terms of the
   Common Development and Distribution License, Version 1.0 only. */

#include "FileStream.h"
#include "StreamException.h"

#ifdef WIN32
# include <io.h>
#else
# include <unistd.h>
#endif


namespace base
{

 FileStream::~FileStream ()
{
}



void  FileStream::open (const char* fn, uint mode)
{
if (!try_open(fn, mode))
 throw StreamClosedException();
}



bool  FileStream::try_open (const char* fn, uint mode)
{
close();

std::FILE* f = 0;

if (mode & out)
 {
 if (mode & in)
  {
  f = std::fopen(fn, "r+b");
  if (!f)
   f = std::fopen(fn, "w+b");
  }
 else
  f = std::fopen(fn, "wb");
 }
else if (mode & in)
 f = std::fopen(fn, "rb");

if (!f)
 goto open_fail;

file.reset(f, mode);

off_type x;

if (std::fseek(*file, 0, SEEK_END) != 0)
 goto open_fail;

x = std::ftell(*file);
if (x == -1)
 goto open_fail;

if (std::fseek(*file, 0, SEEK_SET) != 0)
 goto open_fail;

pos = 0;
len = pos_type(x);

return true;

open_fail:

file.reset();
return false;
}



void  FileStream::close ()
{
Stream::close();
file.reset();
}



void  FileStream::flush ()
{
if (!is_open())
 throw StreamClosedException();

if (std::fflush(*file) != 0)
 throw StreamException();
}



void  FileStream::read (void* buf, size_type n)
{
if (!can_read())
 throw StreamReadException();

if ((pos + n) > len)
 throw StreamReadException();

pos += n;

if (std::fread(buf, n, 1, *file) != 1)
 throw StreamReadException();
}



uint  FileStream::read ()
{
if (!can_read())
 throw StreamReadException();

if (pos == len)
 throw StreamReadException();

++pos;

int x = std::fgetc(*file);

if (x == EOF)
 throw StreamReadException();

return uint(x);
}



void  FileStream::write (const void* buf, size_type n)
{
if (!can_write())
 throw StreamWriteException();

pos += n;
if (pos > len)
 len = pos;

if (std::fwrite(buf, n, 1, *file) != 1)
 throw StreamWriteException();
}



void  FileStream::write (uint x)
{
if (!can_write())
 throw StreamWriteException();

if (pos == len)
 ++len;
++pos;

if (std::fputc(x, *file) == EOF)
 throw StreamWriteException();
}



void  FileStream::pseek ()
{
if (!is_open())
 throw StreamClosedException();

if (pos > len)
 {
 if (!can_write())
  throw StreamWriteException();

 if (std::fseek(*file, len, SEEK_SET) != 0)
  throw StreamException();

 do
  {
  if (std::fputc(0, *file) == EOF)
   throw StreamWriteException();
  }
 while (++len != pos);
 }
else
 {
 if (std::fseek(*file, pos, SEEK_SET) != 0)
  throw StreamException();
 }
}



void  FileStream::ptruncate ()
{
if (!is_open())
 throw StreamClosedException();

#ifdef WIN32
if (_chsize(_fileno(*file), len) != 0)
 throw StreamException();
#else
if (ftruncate(fileno(*file), len) != 0)
 throw StreamException();
#endif
}

} // base
