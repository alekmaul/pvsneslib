/*
 * opt-65816 - Assembly code optimizer for the WDC 65816 processor.
 *
 * Description: Assembly code optimizer produced
 * by the 816 Tiny C Compiler (816-tcc).
 * This library is a C port of the 816-opt python tool.
 *
 * Author: kobenairb (kobenairb@gmail.com).
 *
 * Copyright (c) 2022.
 *
 * This project is released under the GNU Public License.
 *
 */

#include "helpers.h"
#include "optimizer.h"

/**
 * @brief The main function. Accept an ASM file
 as argument or stdin.
 * @param argc The number of arguments provided.
 * @param argv The arguments provided.
 * @return 0 or 1 if exit on error.
 */
int main(int argc, char **argv)
{
    /* -------------------------------- */
    /*      Parse the arguments         */
    /* -------------------------------- */
    for (size_t i = 1; i < (size_t)argc; i++)
    {
        if (argv[i][0] == '-')
        {
            if (argv[i][1] == 'v') // show version
            {
                PrintVersion();
                exit(0);
            }
        }
    }
    /* -------------------------------- */
    /*       Enable verbosity level     */
    /* -------------------------------- */
    size_t verbose = verbosity();

    /* -------------------------------- */
    /*       Store trimmed file         */
    /* -------------------------------- */
    dynArray file = tidyFile(argc, argv);

    /* -------------------------------- */
    /*      Store BSS instuctions       */
    /* -------------------------------- */
    dynArray bss = storeBss(file);

    /* -------------------------------- */
    /*       ASM Optimization           */
    /* -------------------------------- */
    dynArray optAsm = optimizeAsm(file, bss, verbose);

    for (size_t j = 0; j < optAsm.used; j++)
    {
        fprintf(stdout, "%s\n", optAsm.arr[j]);
    }

    /* -------------------------------- */
    /*       Free pointers              */
    /* -------------------------------- */
    freedynArray(bss);
    freedynArray(optAsm);
}
