/* The contents of this file are subject to the terms of the
   Common Development and Distribution License, Version 1.0 only. */

#include <cstdio>
#include <cstring>
#include "OptionParser.h"


namespace base
{

void FASTCALL OptionParser::print_help (const char* usage) const
{
std::printf("Usage: %s\n", usage);
std::printf("Options:\n");

for (const Option* o = options; o->id != 0; ++o)
 {
 std::printf("%4s, %-24s %.45s\n", o->sname, o->lname, o->desc);
 }
}



bool FASTCALL OptionParser::next ()
{
if (index >= argc)
 {
 cmd = 0;
 return false;
 }

cmd = argv[index++];
return true;
}



uint FASTCALL OptionParser::parse () const
{
if (!cmd || !options)
 return 0;

for (const Option* o = options; o->id != 0; ++o)
 {
 if ((std::strcmp(cmd, o->sname) == 0) || (std::strcmp(cmd, o->lname) == 0))
  return o->id;
 }

return 0;
}

} // base
