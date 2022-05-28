/* The contents of this file are subject to the terms of the
   Common Development and Distribution License, Version 1.0 only. */

#ifndef libbase_noncopyable_h
#define libbase_noncopyable_h


namespace base
{

class noncopyable
{
private:
 noncopyable (const noncopyable&)
 {}

 noncopyable& operator= (const noncopyable&)
 { return *this; }

protected:
 noncopyable ()
 {}

 ~noncopyable ()
 {}
};

} // base


#endif
