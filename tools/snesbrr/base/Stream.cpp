/* The contents of this file are subject to the terms of the
   Common Development and Distribution License, Version 1.0 only. */

#include "Stream.h"


namespace base
{

 Stream::~Stream ()
{
}



void  Stream::close ()
{
pos = 0;
len = 0;
}



void  Stream::flush ()
{
}

} // base
