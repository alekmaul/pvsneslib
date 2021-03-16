/* The contents of this file are subject to the terms of the
   Common Development and Distribution License, Version 1.0 only. */

#ifndef libbase_Stream_h
#define libbase_Stream_h

#include "basetypes.h"
#include "noncopyable.h"


namespace base
{

class Stream : noncopyable
{
public:
 typedef ulong  size_type;
 typedef  long  difference_type;
 typedef ulong  pos_type;
 typedef  long  off_type;

protected:
 pos_type pos;
 pos_type len;

protected:
 Stream () : pos(0), len(0)
 { }

public:
 virtual FASTCALL ~Stream ();
 virtual void FASTCALL close ();
 virtual void FASTCALL flush ();

 virtual void FASTCALL read (void* buf, size_type n) = 0;
 virtual uint FASTCALL read () = 0;
 virtual void FASTCALL write (const void* buf, size_type n) = 0;
 virtual void FASTCALL write (uint x) = 0;

protected:
 virtual void FASTCALL pseek () = 0;
 virtual void FASTCALL ptruncate () = 0;

public:
 void resize (pos_type n)
 {
 pos = n;
 pseek();
 if (pos != len)
  {
  len = pos;
  ptruncate();
  }
 }

 void seekpos (pos_type n)
 {
 pos = n;
 pseek();
 }

 void rseekpos (pos_type n)
 {
 if (n >= len)
  seekpos(0);
 else
  seekpos(len - n);
 }

 void seekoff (off_type n)
 {
 if ((n < 0) && (pos_type(-n) >= pos))
  seekpos(0);
 else
  seekpos(pos + n);
 }

 void rseekoff (off_type n)
 { seekoff(-n); }

 pos_type tell () const
 { return pos; }

 pos_type size () const
 { return len; }

 bool eof () const
 { return pos == len; }
};

} // base


#endif
