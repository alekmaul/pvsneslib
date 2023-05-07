/* The contents of this file are subject to the terms of the
   Common Development and Distribution License, Version 1.0 only. */

#include <cstdio>
#include <cstring>
#include "OptionParser.h"

namespace base
{

    void OptionParser::print_help(const char *usage) const
    {
        std::printf("Usage: %s\n", usage);
        std::printf("Options:\n");

        for (const Option *o = options; o->id != 0; ++o)
        {
            std::printf("%4s, %-24s %.45s\n", o->sname, o->lname, o->desc);
        }
    }

    void OptionParser::print_version(const char *version, const char *date) const
    {
        std::printf("snesbrr ( %s ) version %s\n", date, version);
        std::printf("Copyright (c) 2006-2021 DMV27 & Alekmaul\n");
        std::printf("Based on SNESBRR (C) 2006 DMV27\n");
    }

    bool OptionParser::next()
    {
        if (index >= argc)
        {
            cmd = 0;
            return false;
        }

        cmd = argv[index++];
        return true;
    }

    uint OptionParser::parse() const
    {
        if (!cmd || !options)
            return 0;

        for (const Option *o = options; o->id != 0; ++o)
        {
            if ((std::strcmp(cmd, o->sname) == 0) || (std::strcmp(cmd, o->lname) == 0))
                return o->id;
        }

        return 0;
    }

} // base
