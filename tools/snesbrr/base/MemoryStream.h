/* The contents of this file are subject to the terms of the
   Common Development and Distribution License, Version 1.0 only. */

#ifndef libbase_MemoryStream_h
#define libbase_MemoryStream_h

#include <vector>
#include "Stream.h"


namespace base
{

class MemoryStream : public Stream
{
protected:
 std::vector<uint8> mem;

public:
 MemoryStream ()
 { }

 virtual  ~MemoryStream ();
 virtual void  close ();

 virtual void  read (void* buf, size_type n);
 virtual uint  read ();
 virtual void  write (const void* buf, size_type n);
 virtual void  write (uint x);

protected:
 virtual void  pseek ();
 virtual void  ptruncate ();

public:
 void swap (std::vector<uint8>& x)
 {
 mem.swap(x);
 pos = 0;
 len = mem.size();
 }
};

} // base


#endif
