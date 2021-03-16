/* The contents of this file are subject to the terms of the
   Common Development and Distribution License, Version 1.0 only. */

#include "Stream.h"


namespace base
{

FASTCALL Stream::~Stream ()
{
}



void FASTCALL Stream::close ()
{
pos = 0;
len = 0;
}



void FASTCALL Stream::flush ()
{
}

} // base
