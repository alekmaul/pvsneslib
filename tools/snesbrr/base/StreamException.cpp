/* The contents of this file are subject to the terms of the
   Common Development and Distribution License, Version 1.0 only. */

#include "StreamException.h"


namespace base
{

StreamException::~StreamException () throw()
{
}



StreamClosedException::~StreamClosedException () throw()
{
}



StreamReadException::~StreamReadException () throw()
{
}



StreamWriteException::~StreamWriteException () throw()
{
}

} // base
