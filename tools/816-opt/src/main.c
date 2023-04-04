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

    for (size_t i = 0; i < optAsm.used; i++)
    {
        fprintf(stdout, "%s\n", optAsm.arr[i]);
    }
    // size_t labelon = 0;

    // for (size_t i = 0; i < optAsm.used; i++)
    // {
    //     // Skip empty lines.
    //     if (optAsm.arr[i][0] == '\0')
    //         continue;

    //     // Remove the inline comments.
    //     if (isInText(optAsm.arr[i], ASM_COMMENT))
    //         strtok(optAsm.arr[i], ASM_COMMENT);

    //     if (startWith(optAsm.arr[i], ".SECTION")
    //         || startWith(optAsm.arr[i], ".RAMSECTION"))
    //     {
    //         printf("\n%s\n", optAsm.arr[i]);
    //         continue;
    //     }

    //     if (startWith(optAsm.arr[i], ".if")
    //         || endWith(optAsm.arr[i], SECTION_END)
    //         || endWith(optAsm.arr[i], ".endif"))
    //     {
    //         printf("%s\n", optAsm.arr[i]);
    //         continue;
    //     }

    //     // Chek if we are inside label to add indentation
    //     // Don't need to reset labelon to 0 as
    //     // when a label is found, all other instructions
    //     // will also be inside a label.

    //     // Don't need a new line if the label is preceded by
    //     // a section (already performed above).
    //     if (endWith(optAsm.arr[i], ":")
    //         && (startWith(optAsm.arr[i - 1], ".SECTION")
    //             || optAsm.arr[i - 1][0] == '\0'))
    //     {
    //         labelon = 1;
    //         printf("%s\n", optAsm.arr[i]);
    //         continue;
    //     }

    //     if (endWith(optAsm.arr[i], ":")
    //         && (!startWith(optAsm.arr[i - 1], ".SECTION")
    //             || optAsm.arr[i - 1][0] == '\0'))
    //     {
    //         labelon = 1;
    //         printf("\n%s\n", optAsm.arr[i]);
    //         continue;
    //     }

    //     // Add indentation if instruction is
    //     // inside a label.
    //     if (labelon)
    //     {
    //         fprintf(stdout, "    %s\n", trimWhiteSpace(optAsm.arr[i]));
    //     }
    //     else
    //         fprintf(stdout, "%s\n", trimWhiteSpace(optAsm.arr[i]));
    // }

    /* -------------------------------- */
    /*       Free pointers              */
    /* -------------------------------- */
    freedynArray(bss);
    freedynArray(optAsm);
}