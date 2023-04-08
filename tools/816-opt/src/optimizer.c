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

#include "optimizer.h"

/**
 * @brief Checks if OPT816_QUIET is set.
 * This environment variable sets the output in a quiet mode.
 * Just set it if you don't want extra messages (export OPT816_QUIET=1).
 * @return 0 = verbose (by default), 1 = quiet.
 */
int verbosity()
{
    char *QUIET_MODE = getenv("OPT816_QUIET");

    if (!QUIET_MODE)
        return 1;
    return 0;
}

/**
 * @brief Checks if it touches the accumulator register.
 * @param a The asm instruction.
 * @return 1 (true) or 0 (false).
 */
int changeAccu(const char *a)
{
    if (strlen(a) > 2 && ((a[2] == 'a' && !startWith(a, "pha") && !startWith(a, "sta")) || (strlen(a) == 5 && endWith(a, " a"))))
        return 1;

    return 0;
}

/**
 * @brief Check if the line alters the control flow.
 * @param a The asm instruction.
 * @return 1 (true) or 0 (false).
 */
int isControl(const char *a)
{
    const char *p = a;

    while (*p != '\0' && isspace(*p))
    {
        p++;
    }

    if (*p == '\0')
    {
        return 0;
    }

    switch (*p)
    {
    case 'j':
    case 'b':
    case '-':
    case '+':
        return 1;
    }

    if (endWith(a, ":"))
    {
        return 1;
    }

    return 0;
}

/**
 * @brief Create an array of strings from a file
    without comment and leading/trailing white spaces.
    Accept an ASM file as argument or stdin.
 * @param argc The number of arguments provided.
 * @param argv The arguments provided.
 * @return A structure (dynArray).
 */
dynArray tidyFile(const int argc, char **argv)
{
    char buf[MAXLEN_LINE];
    size_t nptrs = 10;
    size_t len;
    dynArray file;
    file.used = 0;

    if (argc > 2)
    {
        fprintf(stderr, "usage:\n");
        fprintf(stderr, "  - %s <filename>\n", argv[0]);
        fprintf(stderr, "  - <stdin> | %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *fp = argc > 1 ? fopen(argv[1], "r") : stdin;

    if (!fp)
    {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    if ((file.arr = malloc(nptrs * sizeof(char *))) == NULL)
    {
        perror("malloc-lines");
        exit(EXIT_FAILURE);
    }

    while (fgets(buf, MAXLEN_LINE, fp))
    {
        buf[(len = strcspn(buf, "\n"))] = 0;

        if (!startWith(buf, ASM_COMMENT))
        {
            if (file.used == nptrs)
            {
                void *tmp = realloc(file.arr, (2 * nptrs) * sizeof(char *));
                if (!tmp)
                {
                    perror("realloc-lines");
                    break;
                }
                file.arr = tmp;
                nptrs *= 2;
            }
            if (!(file.arr[file.used] = malloc(len + 1)))
            {
                perror("malloc-lines[used]");
                break;
            }
            memcpy(file.arr[file.used], trimWhiteSpace(buf), len + 1);
            file.used += 1;
        }
    }
    if (fp != stdin)
        fclose(fp);

    // dynArray r = { lines, used };
    return file;
}

/**
 * @brief Create an array of string to store
    block bss instructions (first word only).
 * @param file The parsed asm file provided as a structure.
 * @return A structure (dynArray).
 */
dynArray storeBss(dynArray file)
{

    size_t bss_on = 0;
    size_t len;
    dynArray bss;
    bss.used = 0;

    if ((bss.arr = malloc(file.used * sizeof(char *))) == NULL)
    {
        perror("malloc-lines");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < file.used; i++)
    {
        if (matchStr(file.arr[i], BSS_SECTION_START))
        {
            bss_on = 1;
            continue;
        }
        if (matchStr(file.arr[i], SECTION_END) && bss_on)
        {
            bss_on = 0;
            continue;
        }
        if (!matchStr(file.arr[i], BSS_SECTION_START) && bss_on)
        {
            len = strlen(file.arr[i]);

            if ((bss.arr[bss.used] = malloc(len + 1)) == NULL)
            {
                perror("malloc-lines");
                exit(EXIT_FAILURE);
            }
            memcpy(bss.arr[bss.used], file.arr[i], len + 1);
            // Get the first word only.
            strtok(bss.arr[bss.used], " ");
            bss.used += 1;
        }
    }

    return bss;
}

/**
 * @brief Optimize ASM code.
 * @param file The asm file cleaned (see tidyFile function).
 * @param bss The bss section (only forst words).
 * @param verbose The level of verbosity (see verbosity function).
 */
dynArray optimizeAsm(dynArray file, const dynArray bss, const size_t verbose)
{

    size_t totalopt = 0; // Total number of optimizations performed
    int opted = -1;      // Have we Optimized in this pass
    size_t opass = 0;    // Optimization pass counter
    dynArray r, r1;      // Store regexMatchGroups structs
    char snp_buf1[MAXLEN_LINE],
        snp_buf2[MAXLEN_LINE]; // Store snprintf buffers
    dynArray text_opt;

    while (opted)
    {
        if ((text_opt.arr = malloc(file.used * sizeof(char *))) == NULL)
        {
            perror("malloc-lines");
            exit(EXIT_FAILURE);
        }

        text_opt.used = 0;
        opass += 1;
        opted = 0;
        size_t i = 0;

        if (verbose)
            fprintf(stderr, "optimization pass %lu: ", opass);

        while (i < file.used)
        {
            if (startWith(file.arr[i], "st"))
            {
                /* Eliminate redundant stores */
                r = regexMatchGroups(file.arr[i], STORE_AXYZ_TO_PSEUDO, 3);
                if (r.arr != NULL)
                {
                    size_t doopt = 0;
                    for (size_t j = (i + 1); j < (size_t)min(file.used, (i + 30)); j++)
                    {
                        snprintf(snp_buf1, sizeof(snp_buf1), "st([axyz]).b tcc__%s$", r.arr[2]);
                        r1 = regexMatchGroups(file.arr[j], snp_buf1, 2);
                        if (r1.arr != NULL)
                        {

                            freedynArray(r1);

                            doopt = 1;
                            break;
                        }
                        /* Before function call (will be clobbered anyway) */
                        if (startWith(file.arr[j], "jsr.l ") && !startWith(file.arr[j], "jsr.l tcc__"))
                        {

                            doopt = 1;
                            break;
                        }
                        /* Cases in which we don't pursue optimization further
                            #1 Branch or other use of the pseudo register */
                        snprintf(snp_buf1, sizeof(snp_buf1), "tcc__%s", r.arr[2]);
                        if (isControl(file.arr[j]) || isInText(file.arr[j], snp_buf1))
                        {

                            break;
                        }
                        /* #2 Use as a pointer */
                        snprintf(snp_buf1, sizeof(snp_buf1), "[tcc__%s", r.arr[2]);
                        char *ss_buffer = sliceStr(snp_buf1, 0, strlen(snp_buf1) - 1); // Remove the last char
                        if (endWith(r.arr[2], "h") && isInText(file.arr[j], ss_buffer))
                        {

                            free(ss_buffer);

                            break;
                        }
                        free(ss_buffer);
                    }
                    freedynArray(r);
                    if (doopt)
                    {
                        i += 1; // Skip redundant store
                        opted += 1;
                        continue;
                    }
                }
                /* Stores (x/y) to pseudo-registers */
                r = regexMatchGroups(file.arr[i], STORE_XY_TO_PSEUDO, 3);
                if (r.arr != NULL)
                {
                    /* Store hwreg to preg, push preg,
                        function call -> push hwreg, function call */
                    snprintf(snp_buf1, sizeof(snp_buf1), "pei (tcc__%s)",
                             r.arr[2]);
                    if (matchStr(file.arr[i + 1], snp_buf1) && startWith(file.arr[i + 2], "jsr.l "))
                    {

                        snprintf(snp_buf1, sizeof(snp_buf1), "ph%s", r.arr[1]);
                        text_opt = pushToArray(text_opt, snp_buf1);

                        freedynArray(r);

                        i += 2;
                        opted += 1;
                        continue;
                    }
                    /* Store hwreg to preg, push preg -> store hwreg to preg,
                        push hwreg (shorter) */
                    if (matchStr(file.arr[i + 1], snp_buf1))
                    {

                        text_opt = pushToArray(text_opt, file.arr[i]);

                        snprintf(snp_buf1, sizeof(snp_buf1), "ph%s", r.arr[1]);
                        text_opt = pushToArray(text_opt, snp_buf1);

                        freedynArray(r);

                        i += 2;
                        opted += 1;
                        continue;
                    }
                    /* Store hwreg to preg, load hwreg from preg -> store hwreg to
                       preg, transfer hwreg/hwreg (shorter) */
                    snprintf(snp_buf1, sizeof(snp_buf1), "lda.b tcc__%s",
                             r.arr[2]);
                    snprintf(snp_buf2, sizeof(snp_buf2),
                             "lda.b tcc__%s ; DON'T OPTIMIZE", r.arr[2]);
                    if (matchStr(file.arr[i + 1], snp_buf1) || matchStr(file.arr[i + 1], snp_buf2))
                    {

                        text_opt = pushToArray(text_opt, file.arr[i]);

                        snprintf(snp_buf1, sizeof(snp_buf1), "t%sa",
                                 r.arr[1]); // FIXME: shouldn't this be marked as
                                            // DON'T OPTIMIZE again?
                        text_opt = pushToArray(text_opt, snp_buf1);

                        freedynArray(r);

                        i += 2;
                        opted += 1;
                        continue;
                    }
                    freedynArray(r);
                }
                /* Stores (accu only) to pseudo-registers */
                r = regexMatchGroups(file.arr[i], STORE_A_TO_PSEUDO, 2);
                if (r.arr != NULL)
                {
                    /* Store preg followed by load preg */
                    snprintf(snp_buf1, sizeof(snp_buf1), "lda.b tcc__%s",
                             r.arr[1]);
                    if (matchStr(file.arr[i + 1], snp_buf1))
                    {

                        text_opt = pushToArray(text_opt, file.arr[i]);

                        freedynArray(r);

                        i += 2; // Omit load
                        opted += 1;
                        continue;
                    }
                    /* Store preg followed by load preg with ldx/ldy in between */
                    if ((startWith(file.arr[i + 1], "ldx") || startWith(file.arr[i + 1], "ldy")) && matchStr(file.arr[i + 2], snp_buf1))
                    {

                        text_opt = pushToArray(text_opt, file.arr[i]);
                        text_opt = pushToArray(text_opt, file.arr[i + 1]);

                        freedynArray(r);

                        i += 3; // Omit load
                        opted += 1;
                        continue;
                    }
                    /* Store accu to preg, push preg, function call -> push accu,
                        function call */
                    snprintf(snp_buf1, sizeof(snp_buf1), "pei (tcc__%s)",
                             r.arr[1]);
                    if (matchStr(file.arr[i + 1], snp_buf1) && startWith(file.arr[i + 2], "jsr.l "))
                    {

                        text_opt = pushToArray(text_opt, "pha");

                        freedynArray(r);

                        i += 2;
                        opted += 1;
                        continue;
                    }
                    /* Store accu to preg, push preg -> store accu to preg,
                        push accu (shorter) */
                    if (matchStr(file.arr[i + 1], snp_buf1))
                    {

                        text_opt = pushToArray(text_opt, file.arr[i]);
                        text_opt = pushToArray(text_opt, "pha");

                        freedynArray(r);

                        i += 2;
                        opted += 1;
                        continue;
                    }
                    /* Store accu to preg1, push preg2, push preg1 -> store accu to
                       preg1, push preg2, push accu */
                    else if (startWith(file.arr[i + 1], "pei ") && matchStr(file.arr[i + 2], snp_buf1))
                    {

                        text_opt = pushToArray(text_opt, file.arr[i + 1]);
                        text_opt = pushToArray(text_opt, file.arr[i]);
                        text_opt = pushToArray(text_opt, "pha");

                        freedynArray(r);

                        i += 3;
                        opted += 1;
                        continue;
                    }
                    /* Convert incs/decs on pregs incs/decs on hwregs */
                    size_t cont = 0;
                    const char *crem[] = {"inc", "dec"};
                    for (size_t k = 0; k < sizeof(crem) / sizeof(const char *); k++)
                    {
                        snprintf(snp_buf1, sizeof(snp_buf1), "%s.b tcc__%s",
                                 crem[k], r.arr[1]);
                        if (matchStr(file.arr[i + 1], snp_buf1))
                        {

                            /* Store to preg followed by crement on preg */
                            if (matchStr(file.arr[i + 2], snp_buf1) && startWith(file.arr[i + 3], "lda"))
                            {

                                /* Store to preg followed by two crements on preg
                                    increment the accu first, then store it to preg
                                 */
                                snprintf(snp_buf1, sizeof(snp_buf1), "%s a",
                                         crem[k]);
                                text_opt = pushToArray(text_opt, snp_buf1);
                                text_opt = pushToArray(text_opt, snp_buf1);
                                snprintf(snp_buf1, sizeof(snp_buf1),
                                         "sta.b tcc__%s", r.arr[1]);
                                text_opt = pushToArray(text_opt, snp_buf1);

                                /* A subsequent load can be omitted (the right value
                                 * is already in the accu) */
                                snprintf(snp_buf1, sizeof(snp_buf1),
                                         "lda.b tcc__%s", r.arr[1]);
                                if (matchStr(file.arr[i + 3], snp_buf1))
                                    i += 4;
                                else
                                    i += 3;

                                freedynArray(r);

                                opted += 1;
                                cont += 1;
                                break;
                            }
                            else if (startWith(file.arr[i + 2], "lda"))
                            {

                                snprintf(snp_buf1, sizeof(snp_buf1), "%s a",
                                         crem[k]);
                                text_opt = pushToArray(text_opt, snp_buf1);

                                snprintf(snp_buf1, sizeof(snp_buf1),
                                         "sta.b tcc__%s", r.arr[1]);
                                text_opt = pushToArray(text_opt, snp_buf1);

                                snprintf(snp_buf1, sizeof(snp_buf1),
                                         "lda.b tcc__%s", r.arr[1]);
                                if (matchStr(file.arr[i + 2], snp_buf1))
                                    i += 3;
                                else
                                    i += 2;

                                freedynArray(r);

                                opted += 1;
                                cont += 1;
                                break;
                            }
                        }
                    }
                    if (cont)
                        continue;

                    r1 = regexMatchGroups(file.arr[i + 1], "lda.b tcc__([rf][0-9]{0,})",
                                          2);
                    if (r1.arr != NULL)
                    {

                        char *ss_buffer = sliceStr(file.arr[i + 2], 0, 3);
                        if (matchStr(ss_buffer, "and") || matchStr(ss_buffer, "ora"))
                        {

                            /* Store to preg1, load from preg2, and/or preg1 ->
                             * store to preg1, and/or preg2 */
                            snprintf(snp_buf1, sizeof(snp_buf1), ".b tcc__%s",
                                     r.arr[1]);
                            if (endWith(file.arr[i + 2], snp_buf1))
                            {

                                text_opt = pushToArray(text_opt, file.arr[i]);

                                snprintf(snp_buf1, sizeof(snp_buf1), "%s.b tcc__%s",
                                         ss_buffer, r1.arr[1]);
                                text_opt = pushToArray(text_opt, snp_buf1);

                                free(ss_buffer);
                                freedynArray(r);
                                freedynArray(r1);

                                i += 3;
                                opted += 1;
                                continue;
                            }
                        }
                        free(ss_buffer);
                        freedynArray(r1);
                    }

                    /* Store to preg, switch to 8 bits, load from preg => skip the
                     * load */
                    snprintf(snp_buf1, sizeof(snp_buf1), "lda.b tcc__%s",
                             r.arr[1]);
                    if (matchStr(file.arr[i + 1], "sep #$20") && matchStr(file.arr[i + 2], snp_buf1))
                    {

                        text_opt = pushToArray(text_opt, file.arr[i]);
                        text_opt = pushToArray(text_opt, file.arr[i + 1]);

                        freedynArray(r);

                        i += 3; // Skip load
                        opted += 1;
                        continue;
                    }

                    /* Two stores to preg without control flow or other uses of preg
                     * => skip first store
                     */
                    snprintf(snp_buf1, sizeof(snp_buf1), "tcc__%s", r.arr[1]);
                    if (!isControl(file.arr[i + 1]) && !isInText(file.arr[i + 1], snp_buf1))
                    {

                        if (matchStr(file.arr[i + 2], file.arr[i]))
                        {

                            text_opt = pushToArray(text_opt, file.arr[i + 1]);
                            text_opt = pushToArray(text_opt, file.arr[i + 2]);

                            freedynArray(r);

                            i += 3; // Skip first store
                            opted += 1;
                            continue;
                        }
                    }

                    /* Store hwreg to preg, load hwreg from preg -> store hwreg to
                       preg, transfer hwreg/hwreg (shorter) */
                    snprintf(snp_buf1, sizeof(snp_buf1), "ld([xy]).b tcc__%s",
                             r.arr[1]);
                    r1 = regexMatchGroups(file.arr[i + 1], snp_buf1, 2);
                    if (r1.arr != NULL)
                    {

                        text_opt = pushToArray(text_opt, file.arr[i]);

                        snprintf(snp_buf1, sizeof(snp_buf1), "ta%s", r1.arr[1]);
                        text_opt = pushToArray(text_opt, snp_buf1);

                        freedynArray(r);
                        freedynArray(r1);

                        i += 2;
                        opted += 1;
                        continue;
                    }

                    /* Store accu to preg then load accu from preg,
                        with something in-between that does not alter */
                    snprintf(snp_buf1, sizeof(snp_buf1), "tcc__%s", r.arr[1]);

                    if (!(isControl(file.arr[i + 1]) || changeAccu(file.arr[i + 1]) || isInText(file.arr[i + 1], snp_buf1)))
                    {

                        snprintf(snp_buf1, sizeof(snp_buf1), "lda.b tcc__%s",
                                 r.arr[1]);
                        if (matchStr(file.arr[i + 2], snp_buf1))
                        {

                            text_opt = pushToArray(text_opt, file.arr[i]);
                            text_opt = pushToArray(text_opt, file.arr[i + 1]);

                            freedynArray(r);

                            i += 3; // Skip load
                            opted += 1;
                            continue;
                        }
                    }

                    /* Store preg1, clc, load preg2,
                        add preg1 -> store preg1, clc, add preg2 */
                    if (matchStr(file.arr[i + 1], "clc"))
                    {

                        r1 = regexMatchGroups(file.arr[i + 2], "lda.b tcc__(r[0-9]{0,})",
                                              2);
                        if (r1.arr != NULL)
                        {
                            snprintf(snp_buf1, sizeof(snp_buf1), "adc.b tcc__%s",
                                     r.arr[1]);
                            if (matchStr(file.arr[i + 3], snp_buf1))
                            {

                                text_opt = pushToArray(text_opt, file.arr[i]);
                                text_opt = pushToArray(text_opt, file.arr[i + 1]);

                                snprintf(snp_buf1, sizeof(snp_buf1),
                                         "adc.b tcc__%s", r1.arr[1]);
                                text_opt = pushToArray(text_opt, snp_buf1);

                                freedynArray(r);
                                freedynArray(r1);

                                i += 4; // Skip load
                                opted += 1;
                                continue;
                            }
                            freedynArray(r1);
                        }
                    }

                    /* Store accu to preg, asl preg => asl accu, store accu to preg
                        FIXME: is this safe? can we rely on code not making
                       assumptions about the contents of the accu after the shift?
                     */
                    snprintf(snp_buf1, sizeof(snp_buf1), "asl.b tcc__%s",
                             r.arr[1]);
                    if (matchStr(file.arr[i + 1], snp_buf1))
                    {

                        text_opt = pushToArray(text_opt, "asl a");
                        text_opt = pushToArray(text_opt, file.arr[i]);

                        freedynArray(r);

                        i += 2;
                        opted += 1;
                        continue;
                    }
                    freedynArray(r);
                }

                r = regexMatchGroups(file.arr[i], "sta (.{0,}),s$", 2);
                if (r.arr != NULL)
                {
                    snprintf(snp_buf1, sizeof(snp_buf1), "lda %s,s", r.arr[1]);
                    if (matchStr(file.arr[i + 1], snp_buf1))
                    {

                        text_opt = pushToArray(text_opt, file.arr[i]);

                        freedynArray(r);

                        i += 2; // Omit load
                        opted += 1;
                        continue;
                    }
                    freedynArray(r);
                }
            } // End of startWith(file.arr[i], "st")

            if (startWith(file.arr[i], "ld"))
            {

                r = regexMatchGroups(file.arr[i], "ldx #0", 1);
                if (r.arr != NULL)
                {

                    r1 = regexMatchGroups(file.arr[i], "lda.l (.{0,}),x$", 2);
                    if (r1.arr != NULL && !endWith(file.arr[i + 3], ",x"))
                    {

                        snprintf(snp_buf1, sizeof(snp_buf1), "lda.l %s",
                                 r1.arr[1]);
                        text_opt = pushToArray(text_opt, snp_buf1);

                        freedynArray(r1);
                        freedynArray(r);

                        i += 2;
                        opted += 1;
                        continue;
                    }
                    else if (r1.arr != NULL)
                    {

                        snprintf(snp_buf1, sizeof(snp_buf1), "lda.l %s", r1.arr[1]);
                        text_opt = pushToArray(text_opt, snp_buf1);

                        text_opt = pushToArray(text_opt, file.arr[i + 2]);

                        char *rs_buffer = replaceStr(file.arr[i + 3], ",x", "");
                        text_opt = pushToArray(text_opt, rs_buffer);

                        freedynArray(r1);
                        freedynArray(r);

                        i += 4;
                        opted += 1;
                        continue;
                    }
                    freedynArray(r);
                }

                if (startWith(file.arr[i], "lda.w #") && matchStr(file.arr[i + 1], "sta.b tcc__r9") && startWith(file.arr[i + 2], "lda.w #") && matchStr(file.arr[i + 3], "sta.b tcc__r9h") && matchStr(file.arr[i + 4], "sep #$20") && startWith(file.arr[i + 5], "lda.b ") && matchStr(file.arr[i + 6], "sta.b [tcc__r9]") && matchStr(file.arr[i + 7], "rep #$20"))
                {

                    text_opt = pushToArray(text_opt, "sep #$20");
                    text_opt = pushToArray(text_opt, file.arr[i + 5]);

                    char *ss_buffer = sliceStr(file.arr[i + 2], 7, strlen(file.arr[i + 2]));
                    char *ss_buffer2 = sliceStr(file.arr[i], 7, strlen(file.arr[i]));
                    snprintf(snp_buf1, sizeof(snp_buf1), "sta.l %lu",
                             atol(ss_buffer) * 65536 + atol(ss_buffer2));
                    text_opt = pushToArray(text_opt, snp_buf1);

                    text_opt = pushToArray(text_opt, "rep #$20");

                    free(ss_buffer);
                    free(ss_buffer2);

                    i += 8;
                    opted += 1;
                    continue;
                }

                if (matchStr(file.arr[i], "lda.w #0"))
                {

                    if (startWith(file.arr[i + 1], "sta.b ") && startWith(file.arr[i + 2], "lda"))
                    {

                        char *rs_buffer = replaceStr(file.arr[i + 1], "sta.", "stz.");
                        text_opt = pushToArray(text_opt, rs_buffer);

                        i += 2;
                        opted += 1;
                        continue;
                    }
                }
                else if (startWith(file.arr[i], "lda.w #"))
                {

                    if (matchStr(file.arr[i + 1], "sep #$20") && startWith(file.arr[i + 2], "sta ") && matchStr(file.arr[i + 3], "rep #$20") && startWith(file.arr[i + 4], "lda"))
                    {

                        text_opt = pushToArray(text_opt, "sep #$20");

                        char *rs_buffer = replaceStr(file.arr[i], "lda.w", "lda.b");
                        text_opt = pushToArray(text_opt, rs_buffer);

                        text_opt = pushToArray(text_opt, file.arr[i + 2]);
                        text_opt = pushToArray(text_opt, file.arr[i + 3]);

                        i += 4;
                        opted += 1;
                        continue;
                    }
                }

                if (startWith(file.arr[i], "lda.b") && !isControl(file.arr[i + 1]) && !isInText(file.arr[i + 1], "a") && startWith(file.arr[i + 2], "lda.b"))
                {

                    text_opt = pushToArray(text_opt, file.arr[i + 1]);
                    text_opt = pushToArray(text_opt, file.arr[i + 2]);

                    i += 3;
                    opted += 1;
                    continue;
                }

                /* Don't write preg high back to stack if
                    it hasn't been updated */
                if (endWith(file.arr[i + 1], "h") && startWith(file.arr[i + 1], "sta.b tcc__r") && startWith(file.arr[i], "lda ") && endWith(file.arr[i], ",s"))
                {

                    char *local = sliceStr(file.arr[i], 4, strlen(file.arr[i]));
                    char *reg = sliceStr(file.arr[i + 1], 6, strlen(file.arr[i + 1]));

                    /* lda stack ; store high preg ; ...
                        ; load high preg ; sta stack */
                    size_t j = i + 2;
                    while (j < (file.used - 2) && !isControl(file.arr[j]) && !isInText(file.arr[j], reg))
                    {

                        j += 1;
                    }
                    snprintf(snp_buf1, sizeof(snp_buf1), "lda.b %s", reg);
                    snprintf(snp_buf2, sizeof(snp_buf2), "sta %s", local);
                    if (matchStr(file.arr[j], snp_buf1) && matchStr(file.arr[j + 1], snp_buf2))
                    {
                        while (i < j)
                        {
                            text_opt = pushToArray(text_opt, file.arr[i]);

                            i += 1;
                        }

                        free(reg);
                        free(local);

                        i += 2; // Skip load high preg ; sta stack
                        opted += 1;
                        continue;
                    }
                    free(reg);
                    free(local);
                }

                /* Reorder copying of 32-bit value to preg if it looks as
                    if that could allow further optimization.
                    Looking for:
                        lda something
                        sta.b tcc_rX
                        lda something
                        sta.b tcc_rYh
                        ...tcc_rX...
                */
                if (startWith(file.arr[i], "lda") && startWith(file.arr[i + 1], "sta.b tcc__r"))
                {

                    char *reg = sliceStr(file.arr[i + 1], 6, strlen(file.arr[i + 1]));
                    if (!endWith(reg, "h") && startWith(file.arr[i + 2], "lda") && !endWith(file.arr[i + 2], reg) && startWith(file.arr[i + 3], "sta.b tcc__r") && endWith(file.arr[i + 3], "h") && endWith(file.arr[i + 4], reg))
                    {

                        text_opt = pushToArray(text_opt, file.arr[i + 2]);
                        text_opt = pushToArray(text_opt, file.arr[i + 3]);
                        text_opt = pushToArray(text_opt, file.arr[i]);
                        text_opt = pushToArray(text_opt, file.arr[i + 1]);

                        free(reg);

                        i += 4;
                        // this is not an optimization per se, so we don't count it
                        continue;
                    }
                    free(reg);
                }

                /* Compare optimizations inspired by optimore
                    These opts simplify compare operations, which are monstrous because
                    they have to take the long long case into account.
                    We try to detect those cases by checking if a tya follows the
                    comparison (not sure if this is reliable, but it passes the test suite)
                */
                if (matchStr(file.arr[i], "ldx #1") && startWith(file.arr[i + 1], "lda.b tcc__") && matchStr(file.arr[i + 2], "sec") && startWith(file.arr[i + 3], "sbc #") && matchStr(file.arr[i + 4], "tay") && matchStr(file.arr[i + 5], "beq +") && matchStr(file.arr[i + 6], "dex") && matchStr(file.arr[i + 7], "+") && startWith(file.arr[i + 8], "stx.b tcc__") && matchStr(file.arr[i + 9], "txa") && matchStr(file.arr[i + 10], "bne +") && startWith(file.arr[i + 11], "brl ") && matchStr(file.arr[i + 12], "+") && !matchStr(file.arr[i + 13], "tya"))
                {

                    text_opt = pushToArray(text_opt, file.arr[i + 1]);

                    char *ins = sliceStr(file.arr[i + 3], 5, strlen(file.arr[i + 1]));
                    snprintf(snp_buf1, sizeof(snp_buf1), "cmp #%s", ins);
                    text_opt = pushToArray(text_opt, snp_buf1);

                    text_opt = pushToArray(text_opt, file.arr[i + 5]);
                    text_opt = pushToArray(text_opt, file.arr[i + 11]); // brl
                    text_opt = pushToArray(text_opt, file.arr[i + 12]); // +

                    free(ins);

                    i += 13;
                    opted += 1;
                    continue;
                }

                if (matchStr(file.arr[i], "ldx #1") && matchStr(file.arr[i + 1], "sec") && startWith(file.arr[i + 2], "sbc #") && matchStr(file.arr[i + 3], "tay") && matchStr(file.arr[i + 4], "beq +") && matchStr(file.arr[i + 5], "dex") && matchStr(file.arr[i + 6], "+") && startWith(file.arr[i + 7], "stx.b tcc__") && matchStr(file.arr[i + 8], "txa") && matchStr(file.arr[i + 9], "bne +") && startWith(file.arr[i + 10], "brl ") && matchStr(file.arr[i + 11], "+") && !matchStr(file.arr[i + 12], "tya"))
                {

                    char *ins = sliceStr(file.arr[i + 2], 5, strlen(file.arr[i + 2]));
                    snprintf(snp_buf1, sizeof(snp_buf1), "cmp #%s", ins);
                    text_opt = pushToArray(text_opt, snp_buf1);

                    text_opt = pushToArray(text_opt, file.arr[i + 4]);
                    text_opt = pushToArray(text_opt, file.arr[i + 10]); // brl
                    text_opt = pushToArray(text_opt, file.arr[i + 11]); // +

                    free(ins);

                    i += 12;
                    opted += 1;
                    continue;
                }

                if (matchStr(file.arr[i], "ldx #1") && startWith(file.arr[i + 1], "lda.b tcc__r") && matchStr(file.arr[i + 2], "sec") && startWith(file.arr[i + 3], "sbc.b tcc__r") && matchStr(file.arr[i + 4], "tay") && matchStr(file.arr[i + 5], "beq +") && matchStr(file.arr[i + 6], "bcs ++") && matchStr(file.arr[i + 7], "+ dex") && matchStr(file.arr[i + 8], "++") && startWith(file.arr[i + 9], "stx.b tcc__r") && matchStr(file.arr[i + 10], "txa") && matchStr(file.arr[i + 11], "bne +") && startWith(file.arr[i + 12], "brl ") && matchStr(file.arr[i + 13], "+") && !matchStr(file.arr[i + 14], "tya"))
                {

                    text_opt = pushToArray(text_opt, file.arr[i + 1]);

                    char *ins = sliceStr(file.arr[i + 3], 6, strlen(file.arr[i + 3]));
                    snprintf(snp_buf1, sizeof(snp_buf1), "cmp.b %s", ins);
                    text_opt = pushToArray(text_opt, snp_buf1);

                    text_opt = pushToArray(text_opt, file.arr[i + 5]);
                    text_opt = pushToArray(text_opt, "bcc +");
                    text_opt = pushToArray(text_opt, "brl ++");
                    text_opt = pushToArray(text_opt, "+");
                    text_opt = pushToArray(text_opt, file.arr[i + 12]);
                    text_opt = pushToArray(text_opt, "++");

                    free(ins);

                    i += 14;
                    opted += 1;
                    continue;
                }

                if (matchStr(file.arr[i], "ldx #1") && matchStr(file.arr[i + 1], "sec") && startWith(file.arr[i + 2], "sbc.w #") && matchStr(file.arr[i + 3], "tay") && matchStr(file.arr[i + 4], "bvc +") && matchStr(file.arr[i + 5], "eor #$8000") && matchStr(file.arr[i + 6], "+") && matchStr(file.arr[i + 7], "bmi +++") && matchStr(file.arr[i + 8], "++") && matchStr(file.arr[i + 9], "dex") && matchStr(file.arr[i + 10], "+++") && startWith(file.arr[i + 11], "stx.b tcc__r") && matchStr(file.arr[i + 12], "txa") && matchStr(file.arr[i + 13], "bne +") && startWith(file.arr[i + 14], "brl ") && matchStr(file.arr[i + 15], "+") && !matchStr(file.arr[i + 16], "tya"))
                {

                    text_opt = pushToArray(text_opt, file.arr[i + 1]);
                    text_opt = pushToArray(text_opt, file.arr[i + 2]);
                    text_opt = pushToArray(text_opt, file.arr[i + 4]);
                    text_opt = pushToArray(text_opt, "eor #$8000");
                    text_opt = pushToArray(text_opt, "+");
                    text_opt = pushToArray(text_opt, "bmi +");
                    text_opt = pushToArray(text_opt, file.arr[i + 14]);
                    text_opt = pushToArray(text_opt, "+");

                    i += 16;
                    opted += 1;
                    continue;
                }

                if (matchStr(file.arr[i], "ldx #1") && startWith(file.arr[i + 1], "lda.b tcc__r") && matchStr(file.arr[i + 2], "sec") && startWith(file.arr[i + 3], "sbc.b tcc__r") && matchStr(file.arr[i + 4], "tay") && matchStr(file.arr[i + 5], "bvc +") && matchStr(file.arr[i + 6], "eor #$8000") && matchStr(file.arr[i + 7], "+") && matchStr(file.arr[i + 8], "bmi +++") && matchStr(file.arr[i + 9], "++") && matchStr(file.arr[i + 10], "dex") && matchStr(file.arr[i + 11], "+++") && startWith(file.arr[i + 12], "stx.b tcc__r") && matchStr(file.arr[i + 13], "txa") && matchStr(file.arr[i + 14], "bne +") && startWith(file.arr[i + 15], "brl ") && matchStr(file.arr[i + 16], "+") && !matchStr(file.arr[i + 17], "tya"))
                {

                    text_opt = pushToArray(text_opt, file.arr[i + 1]);
                    text_opt = pushToArray(text_opt, file.arr[i + 2]);
                    text_opt = pushToArray(text_opt, file.arr[i + 3]);
                    text_opt = pushToArray(text_opt, file.arr[i + 5]);
                    text_opt = pushToArray(text_opt, file.arr[i + 6]);
                    text_opt = pushToArray(text_opt, "+");
                    text_opt = pushToArray(text_opt, "bmi +");
                    text_opt = pushToArray(text_opt, file.arr[i + 15]);
                    text_opt = pushToArray(text_opt, "+");

                    i += 17;
                    opted += 1;
                    continue;
                }

                if (matchStr(file.arr[i], "ldx #1") && matchStr(file.arr[i + 1], "sec") && startWith(file.arr[i + 2], "sbc.b tcc__r") && matchStr(file.arr[i + 3], "tay") && matchStr(file.arr[i + 4], "bvc +") && matchStr(file.arr[i + 5], "eor #$8000") && matchStr(file.arr[i + 6], "+") && matchStr(file.arr[i + 7], "bmi +++") && matchStr(file.arr[i + 8], "++") && matchStr(file.arr[i + 9], "dex") && matchStr(file.arr[i + 10], "+++") && startWith(file.arr[i + 11], "stx.b tcc__r") && matchStr(file.arr[i + 12], "txa") && matchStr(file.arr[i + 13], "bne +") && startWith(file.arr[i + 14], "brl ") && matchStr(file.arr[i + 15], "+") && !matchStr(file.arr[i + 16], "tya"))
                {

                    text_opt = pushToArray(text_opt, file.arr[i + 1]);
                    text_opt = pushToArray(text_opt, file.arr[i + 2]);
                    text_opt = pushToArray(text_opt, file.arr[i + 4]);
                    text_opt = pushToArray(text_opt, file.arr[i + 5]);
                    text_opt = pushToArray(text_opt, "+");
                    text_opt = pushToArray(text_opt, "bmi +");
                    text_opt = pushToArray(text_opt, file.arr[i + 14]);
                    text_opt = pushToArray(text_opt, "+");

                    i += 16;
                    opted += 1;
                    continue;
                }
            } // End of startWith(file.arr[i], "ld")

            if (matchStr(file.arr[i], "rep #$20") && matchStr(file.arr[i + 1], "sep #$20"))
            {

                i += 2;
                opted += 1;
                continue;
            }

            if (matchStr(file.arr[i], "sep #$20") && startWith(file.arr[i + 1], "lda #") && matchStr(file.arr[i + 2], "pha") && startWith(file.arr[i + 3], "lda #") && matchStr(file.arr[i + 4], "pha"))
            {

                char *token1, *token2;
                token1 = splitStr(file.arr[i + 1], "#", 1);
                snprintf(snp_buf1, sizeof(snp_buf1), "pea.w (%s * 256", token1);
                token2 = splitStr(file.arr[i + 3], "#", 1);
                snprintf(snp_buf2, sizeof(snp_buf2), "%s + %s)", snp_buf1, token2);
                text_opt = pushToArray(text_opt, snp_buf2);
                text_opt = pushToArray(text_opt, file.arr[i]);

                i += 5;
                opted += 1;
                continue;
            }

            r = regexMatchGroups(file.arr[i], "adc #(.{0,})$", 2);
            if (r.arr != NULL)
            {

                r1 = regexMatchGroups(file.arr[i + 1], "sta.b (tcc__[fr][0-9]{0,})$", 2);
                if (r1.arr != NULL)
                {

                    snprintf(snp_buf1, sizeof(snp_buf1), "inc.b %s",
                             r1.arr[1]);
                    if (file.arr[i + 2] && file.arr[i + 3] && matchStr(file.arr[i + 2], snp_buf1) && matchStr(file.arr[i + 3], snp_buf1))
                    {

                        snprintf(snp_buf1, sizeof(snp_buf1), "adc #%s + 2",
                                 r.arr[1]);
                        text_opt = pushToArray(text_opt, snp_buf1);
                        text_opt = pushToArray(text_opt, file.arr[i + 1]);

                        freedynArray(r1);
                        freedynArray(r);

                        i += 4;
                        opted += 1;
                        continue;
                    }

                    freedynArray(r1);
                }

                freedynArray(r);
            }

            if (strlen(file.arr[i]) >= 6)
            {
                char *ss_buffer = sliceStr(file.arr[i], 0, 6);

                if (matchStr(ss_buffer, "lda.l ") || matchStr(ss_buffer, "sta.l "))
                {
                    size_t cont = 0;
                    char *ss_buffer2 = sliceStr(file.arr[i], 2, strlen(file.arr[i]));

                    for (size_t b = 0; b < bss.used; b++)
                    {
                        snprintf(snp_buf1, sizeof(snp_buf1), "a.l %s ",
                                 bss.arr[b]);
                        if (startWith(ss_buffer2, snp_buf1))
                        {

                            char *rs_buffer = replaceStr(file.arr[i], "a.l", "a.w");
                            text_opt = pushToArray(text_opt, rs_buffer);

                            i += 1;
                            opted += 1;
                            cont = 1;
                            break;
                        }
                    }
                    free(ss_buffer2);
                    if (cont)
                    {
                        free(ss_buffer);
                        continue;
                    }
                }
                free(ss_buffer);
            }

            if (startWith(file.arr[i], "jmp.w ") || startWith(file.arr[i], "bra __"))
            {
                size_t j = i + 1;
                size_t cont = 0;
                while (j < file.used && endWith(file.arr[j], ":"))
                {
                    char *ss_buffer = sliceStr(file.arr[j], 0, strlen(file.arr[j]) - 1);
                    if (endWith(file.arr[i], ss_buffer))
                    {

                        free(ss_buffer);
                        i += 1; // Redundant branch, discard it.
                        opted += 1;
                        cont = 1;
                        break;
                    }
                    j += 1;

                    free(ss_buffer);
                    if (cont)
                        continue;
                }
            }

            if (startWith(file.arr[i], "jmp.w "))
            {

                /* Worst case is a 4-byte instruction, so if the jump target is closer
                    than 32 instructions, we can safely substitute a branch */
                char *label = sliceStr(file.arr[i], 6, strlen(file.arr[i]));
                snprintf(snp_buf1, sizeof(snp_buf1), "%s:", label);
                size_t cont = 0;
                for (size_t l = max(0, (i - 32)); l < (size_t)min(file.used, (i + 32)); l++)
                {
                    if (matchStr(file.arr[l], snp_buf1))
                    {

                        char *rs_buffer = replaceStr(file.arr[i], "jmp.w", "bra");
                        text_opt = pushToArray(text_opt, rs_buffer);

                        i += 1;
                        opted += 1;
                        cont = 1;
                        break;
                    }
                }
                free(label);
                if (cont)
                {
                    continue;
                }
            }

            text_opt = pushToArray(text_opt, file.arr[i]);

            i++;

        } // End of while (i < file.used)

        /* Cleaning */
        freedynArray(file);
        if (opted > 0)
        {
            if ((file.arr = malloc(text_opt.used * sizeof(char *))) == NULL)
            {
                perror("malloc-lines");
                exit(EXIT_FAILURE);
            }

            file.used = text_opt.used;

            for (size_t i = 0; i < text_opt.used; i++)
            {

                size_t len = strlen(text_opt.arr[i]);

                if ((file.arr[i] = malloc(len + 1)) == NULL)
                {
                    perror("malloc-lines");
                    exit(EXIT_FAILURE);
                }

                memcpy(file.arr[i], text_opt.arr[i], len + 1);
            }
            freedynArray(text_opt);
        }

        if (verbose)
            fprintf(stderr, "%u optimizations performed\n", opted);

        totalopt += opted;
    }

    if (verbose)
        fprintf(stderr, "%lu optimizations performed in total\n", totalopt);

    return text_opt;
}
