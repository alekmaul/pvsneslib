/* The contents of this file are subject to the terms of the
   Common Development and Distribution License, Version 1.0 only. */

#ifndef libbase_FileStream_h
#define libbase_FileStream_h

#include <cstdio>
#include "Stream.h"


namespace base
{

class FileStream : public Stream
{
public:
 enum OpenMode
 {
 in = 1,
 out = 2,
 in_out = in | out
 };

protected:
 class FilePtr
 {
 private:
  std::FILE* file;
  uint mode;

 public:
  FilePtr (std::FILE* f = 0, uint m = 0) : file(f), mode(m)
  { }

  ~FilePtr ()
  { reset(); }

  void reset (std::FILE* f = 0, uint m = 0)
  {
  std::FILE* tmp = file;

  file = f;
  mode = m;

  if (tmp)
   std::fclose(tmp);
  }

  std::FILE* operator* () const
  { return file; }

  operator std::FILE* () const
  { return file; }

  bool is_open () const
  { return file != 0; }

  bool can_read () const
  { return mode & in; }

  bool can_write () const
  { return mode & out; }
 };

protected:
 FilePtr file;

public:
 FileStream ()
 { }

 FileStream (const char* fn, uint mode)
 { open(fn, mode); }

 void  open (const char* fn, uint mode);
 bool  try_open (const char* fn, uint mode);

 virtual  ~FileStream ();
 virtual void  close ();
 virtual void  flush ();

 virtual void  read (void* buf, size_type n);
 virtual uint  read ();
 virtual void  write (const void* buf, size_type n);
 virtual void  write (uint x);

 bool is_open () const
 { return file.is_open(); }

 bool can_read () const
 { return file.can_read(); }

 bool can_write () const
 { return file.can_write(); }

protected:
 virtual void  pseek ();
 virtual void  ptruncate ();
};

} // base


#endif
