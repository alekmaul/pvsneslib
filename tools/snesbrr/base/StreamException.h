/* The contents of this file are subject to the terms of the
   Common Development and Distribution License, Version 1.0 only. */

#ifndef libbase_StreamException_h
#define libbase_StreamException_h

#include <exception>


namespace base
{

class StreamException : public std::exception
{
public:
 virtual ~StreamException () throw();
};



class StreamClosedException : public StreamException
{
public:
 virtual ~StreamClosedException () throw();
};



class StreamReadException : public StreamException
{
public:
 virtual ~StreamReadException () throw();
};



class StreamWriteException : public StreamException
{
public:
 virtual ~StreamWriteException () throw();
};

} // base


#endif
