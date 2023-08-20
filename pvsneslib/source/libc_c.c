/*---------------------------------------------------------------------------------

    Copyright (C) 2012-2021
        Alekmaul

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any
    damages arising from the use of this software.

    Permission is granted to anyone to use this software for any
    purpose, including commercial applications, and to alter it and
    redistribute it freely, subject to the following restrictions:

    1.	The origin of this software must not be misrepresented; you
        must not claim that you wrote the original software. If you use
        this software in a product, an acknowledgment in the product
        documentation would be appreciated but is not required.
    2.	Altered source versions must be plainly marked as such, and
        must not be misrepresented as being the original software.
    3.	This notice may not be removed or altered from any source
        distribution.

---------------------------------------------------------------------------------*/
/*
 *  linux/lib/vsprintf.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

/* vsprintf.c -- Lars Wirzenius & Linus Torvalds. */
/*
 * Wirzenius wrote this portably, Torvalds fucked it up :-)
 */

/*
 * Fri Jul 13 2001 Crutcher Dunnavant <crutcher+kernel@datastacks.com>
 * - changed to provide snprintf and vsnprintf functions
 */

/* simple malloc()/free() implementation
 * adapted from here: http://www.flipcode.com/archives/Simple_Malloc_Free_Functions.shtml
 */

#include <stdarg.h>
#include <string.h>
#include <limits.h>

/**
 * @brief Check if a character is a decimal digit.
 *
 * @param c The character to check.
 * @return Nonzero if the character is a decimal digit, zero otherwise.
 */
int isdigit(int c)
{
    if (c >= '0' && c <= '9')
        return 1;
    else
        return 0;
}

/**
 * @brief Check if a character is a hexadecimal digit.
 *
 * @param c The character to check.
 * @return Nonzero if the character is a hexadecimal digit, zero otherwise.
 */
int isxdigit(int c)
{
    if (isdigit(c) ||
        (c >= 'a' && c <= 'f') ||
        (c >= 'A' && c <= 'F'))
        return 1;
    else
        return 0;
}

/**
 * @brief Check if a character is a lowercase letter.
 *
 * @param c The character to check.
 * @return Nonzero if the character is a lowercase letter, zero otherwise.
 */
int islower(int c)
{
    if (c >= 'a' && c <= 'z')
        return 1;
    else
        return 0;
}

/**
 * @brief Convert a lowercase letter to uppercase.
 *
 * @param c The character to convert.
 * @return The uppercase equivalent of the input character, or the input character if it is not a lowercase letter.
 */
int toupper(int c)
{
    if (islower(c))
        return c - ('a' - 'A');
    else
        return c;
}

/**
 * @brief Check if a character is a whitespace character.
 *
 * @param c The character to check.
 * @return Nonzero if the character is a whitespace character, zero otherwise.
 */
int isspace(int c)
{
    if (c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r')
        return 1;
    else
        return 0;
}

/**
 * @brief Determines whether the given character is printable or not.
 *
 * This function checks whether the given character is a printable ASCII character
in the range of 0x20 to 0x7E (inclusive).
 * @param c The character to be checked for printability.
 * @return Returns 1 if the character is printable, and 0 otherwise.
 */
int isprint(int c)
{
    if (c >= ' ' && c <= '~')
        return 1;
    else
        return 0;
}
/**
 * @def USED
 *
 * @brief A macro used to mark memory blocks as used in the memory allocation algorithm.
 */
#define USED 1

/**
 * @brief The memory block header used by the custom memory allocation implementation.
 *
 * This structure represents the header of each memory block that is allocated using the custom memory allocation implementation. It only contains the size of the block, which is used to manage the allocation and deallocation of memory.
 */
typedef struct
{
    unsigned int size; /**< The size of the memory block, including the header. */
} unit;

/**
 * @brief Memory system structure that holds the free and heap units.
 */
typedef struct
{
    unit *free; /**< Pointer to the beginning of the free unit */
    unit *heap; /**< Pointer to the beginning of the heap unit */
} msys_t;

/**
 * @brief The memory system instance.
 *
 * This global variable is an instance of the memory system structure used by the malloc() and free() functions to manage memory.
 */
static msys_t msys;

/**
 * @brief Compacts the free memory blocks in the heap to ensure contiguous space for future allocations.
 *
 * This function searches the heap for consecutive free blocks of memory and merges them together. If a block of
memory can fit the required size, it is returned to the caller. If the heap is not large enough to contain
such a block of memory, the function returns NULL.
 * @param p Pointer to the start of the heap.
 * @param nsize The size of the memory block to allocate.
 * @return A pointer to the allocated block of memory, or NULL if the heap is not large enough.
 */
static unit *__compact(unit *p, unsigned int nsize)
{
    unsigned int bsize, psize;
    unit *best;

    best = p;
    bsize = 0;

    while (psize = p->size, psize)
    {
        if (psize & USED)
        {
            if (bsize != 0)
            {
                best->size = bsize;
                if (bsize >= nsize)
                {
                    return best;
                }
            }
            bsize = 0;
            best = p = (unit *)((void *)p + (psize & ~USED));
        }
        else
        {
            bsize += psize;
            p = (unit *)((void *)p + psize);
        }
    }

    if (bsize != 0)
    {
        best->size = bsize;
        if (bsize >= nsize)
        {
            return best;
        }
    }

    return 0;
}

/**
 * @brief Frees a block of memory that was previously allocated using malloc().
 *
 * @param ptr A pointer to the beginning of the block to free.
 */
void free(void *ptr)
{
    if (ptr)
    {
        unit *p;

        p = (unit *)((void *)ptr - sizeof(unit));
        p->size &= ~USED;
    }
}

/**
 * @brief Allocates a block of memory of the specified size.
 *
 * @param size The size of the block to allocate.
 * @return void* Returns a pointer to the beginning of the allocated block, or
NULL if the allocation failed.
 */
void *malloc(unsigned int size)
{
    unsigned int fsize;
    unit *p;

    if (size == 0)
        return 0;

    size += 3 + sizeof(unit);
    size >>= 2;
    size <<= 2;

    if (msys.free == 0 || size > msys.free->size)
    {
        msys.free = __compact(msys.heap, size);
        if (msys.free == 0)
            return 0;
    }

    p = msys.free;
    fsize = msys.free->size;

    if (fsize >= size + sizeof(unit))
    {
        msys.free = (unit *)((void *)p + size);
        msys.free->size = fsize - size;
    }
    else
    {
        msys.free = 0;
        size = fsize;
    }

    p->size = size | USED;

    return (void *)((void *)p + sizeof(unit));
}

/**
 * @brief Initializes a dynamic memory allocation system.
 *
 * This function initializes a dynamic memory allocation system using the specified heap of the given length.
It sets the free pointer and the heap pointer to the start of the heap, and sets the size of both pointers to the length of the heap minus the size of a memory unit.
It also sets the last word of the heap to zero to indicate the end of the heap.
 * @param heap A pointer to the start of the heap.
 * @param len The length of the heap.
 */
void malloc_init(void *heap, unsigned int len)
{
    len >>= 2;
    len <<= 2;
    msys.free = msys.heap = (unit *)heap;
    msys.free->size = msys.heap->size = len - sizeof(unit);
    *(unsigned int *)((char *)heap + len - sizeof(unit)) = 0;
}

/**
 * @brief Compacts the memory heap by consolidating adjacent free blocks.
 *
 * This function compacts the memory heap by consolidating adjacent free blocks.
 * It sets the free block pointer to point to the start of the newly compacted heap.
 */
void compact(void)
{
    msys.free = __compact(msys.heap, 0xffff);
}

/**
 * @brief Converts the initial part of a string to an unsigned long integer.
 *
 * This function parses the string pointed to by @p cp, interpreting its content as an unsigned long integer in the specified @p base.
 * @param cp Pointer to the string to be converted to an unsigned long integer.
 * @param endp Pointer to a pointer that is set to the character past the last character used in the conversion.
 * @param base Optional integer argument specifying the conversion base. If 0, the base is determined by the prefix of the string.
 * @return The converted unsigned long integer value.
 */
unsigned long strtoul(const char *cp, char **endp, unsigned int base)
{
    unsigned long result = 0, value;

    if (!base)
    {
        base = 10;
        if (*cp == '0')
        {
            base = 8;
            cp++;
            if ((*cp == 'x') && isxdigit(cp[1]))
            {
                cp++;
                base = 16;
            }
        }
    }
    while (isxdigit(*cp) &&
           (value = isdigit(*cp) ? *cp - '0' : toupper(*cp) - 'A' + 10) < base)
    {
        result = result * base + value;
        cp++;
    }
    if (endp)
        *endp = (char *)cp;
    return result;
}

/**
 * @brief Converts a string to a signed long integer.
 *
 * This function converts the initial part of the string pointed to by @p cp to a
signed long integer value according to the given @p base, which must be between 2
 * @param cp A pointer to the string to be converted.
 * @param endp A pointer to a pointer to the first invalid character in the input string.
 * @param base The number base to use for the conversion (2-36 inclusive), or 0 to  automatically detect the base from the * input string.
 * @return The converted value as a signed long integer.
 */
long strtol(const char *cp, char **endp, unsigned int base)
{
    if (*cp == '-')
        return -strtoul(cp + 1, endp, base);
    return strtoul(cp, endp, base);
}

/**
 * @brief Converts a string to an unsigned long long integer value.
 *
 * The function parses the initial portion of the string pointed to by cp as an unsigned integer of the specified base.
The string must begin with a sequence of whitespace or digits followed by a valid digit or a letter representing a * digit in the given base.
 * @param cp A pointer to the string to be converted.
 * @param endp A pointer to a pointer to the character that stops the scan.
 * @param base The base to be used for the conversion.
 * @return The converted value of type unsigned long long.
 */
unsigned long long strtoull(const char *cp, char **endp, unsigned int base)
{
    unsigned long long result = 0, value;

    if (!base)
    {
        base = 10;
        if (*cp == '0')
        {
            base = 8;
            cp++;
            if ((*cp == 'x') && isxdigit(cp[1]))
            {
                cp++;
                base = 16;
            }
        }
    }
    while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp - '0' : (islower(*cp) ? toupper(*cp) : *cp) - 'A' + 10) < base)
    {
        result = result * base + value;
        cp++;
    }
    if (endp)
        *endp = (char *)cp;
    return result;
}

/**
 * @brief Converts a string to a long long integer with specified base.
 *
 * This function parses the string pointed to by cp interpreting its content as a long long integer value according to the specified base, which must be between 2 and 36 inclusive or be the special value 0.
If base is 0, the base used is determined by the format of the string: if it starts with "0x" or "0X", base 16 is used,if it starts with "0", base 8 is used, otherwise base 10 is used.
The function optionally returns the pointer to the character immediately following the last character used in the conversion through endp.
 * @param cp Pointer to the string to convert.
 * @param endp Pointer to a pointer to a character that will be set by the function to the first invalid character encountered in the input string. This argument can be NULL if unused.
 * @param base Numeric base to be used for the conversion. If base is zero or 16, the string may include a 0x prefix.
 * @return The converted long long integer value. If the converted value overflows, LLONG_MAX or LLONG_MIN is returned,depending on the sign of the value.
 */
long long strtoll(const char *cp, char **endp, unsigned int base)
{
    if (*cp == '-')
        return -strtoull(cp + 1, endp, base);
    return strtoull(cp, endp, base);
}

/**
 * @brief Skips over and converts any decimal digit string pointed to by the input pointer.
 *
 * This function takes a pointer to a pointer to a character array (string) and converts
any decimal digits found in the string to an integer value. The input pointer will be
updated to point at the character following the last digit in the string.
 * @param s A pointer to a pointer to a character array (string) containing the decimal digit string.
 * @return The integer value resulting from the conversion of the decimal digit string.
 */
static int skip_atoi(const char **s)
{
    int i = 0;

    while (isdigit(**s))
        i = i * 10 + *((*s)++) - '0';
    return i;
}

/**
 * @def do_div
 * @ref number "Used in *number function"
 * @brief Divides an unsigned long long integer by a given base.
 *
 * This macro divides an unsigned long long integer by a given base, and returns the remainder of the division.
 The original value of the integer is modified to store the result of the integer division.
 * @param n The unsigned 64-bit integer to be divided.
 * @param base The unsigned 32-bit integer to divide the integer by.
 * @return The remainder of the division of the integer by the given base.
 */
#define do_div(n, base) ({ \
    int __res; \
    __res = ((unsigned long long) n) % (unsigned) base; \
    n = ((unsigned long long) n) / (unsigned) base; \
    __res; })

/**
 * @def ZEROPAD
 * @ref number "Used in *number function"
 * @brief Pad the result with zeros instead of spaces.
 */
#define ZEROPAD 1
/**
 * @def SIGN
 * @ref number "Used in *number function"
 * @brief Always include a sign (+ or -) before the number.
 */
#define SIGN 2
/**
 * @def PLUS
 * @ref number "Used in *number function"
 * @brief Include a sign (+) before positive numbers.
 */
#define PLUS 4
/**
 * @def SPACE
 * @ref number "Used in *number function"
 * @brief Include a space before positive numbers (ignored if the PLUS flag is set).
 */
#define SPACE 8
/**
 * @def LEFT
 * @ref number "Used in *number function"
 * @brief Left-justify the result within the given field width.
 */
#define LEFT 16
/**
 * @def SPECIAL
 * @ref number "Used in *number function"
 * @brief Add a 0 or 0x before the number if base is 8 or 16, respectively.
 */
#define SPECIAL 32
/**
 * @def LARGE
 * @ref number "Used in *number function"
 * @brief Use uppercase letters (A-F) instead of lowercase (a-f) for base 16.
 */
#define LARGE 64

/**
 * @brief Converts a long long number to a string representation using a given base and formatting options
 *
 * @param buf Pointer to a character buffer where the resulting string will be stored
 * @param end Pointer to the end of the buffer
 * @param num The number to be converted
 * @param base The base to be used for the conversion (must be between 2 and 36 inclusive)
 * @param size Minimum width of the resulting string; if the resulting string is shorter, it will be padded with spaces (or zeros, if the ZEROPAD flag is set)
 * @param precision Minimum number of digits to be used; if the number is shorter, it will be padded with zeros
 * @param type Flags indicating special formatting options (ZEROPAD, SIGN, PLUS, SPACE, LEFT, SPECIAL or LARGE).
 * @return A pointer to the end of the resulting string
 */
static char *number(char *buf, char *end, long long num, int base, int size, int precision, int type)
{
    char c, sign, tmp[66];
    const char *digits;
    const char small_digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    const char large_digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int i;

    digits = (type & LARGE) ? large_digits : small_digits;
    if (type & LEFT)
        type &= ~ZEROPAD;
    if (base < 2 || base > 36)
        return 0;
    c = (type & ZEROPAD) ? '0' : ' ';
    sign = 0;
    if (type & SIGN)
    {
        if (num < 0)
        {
            sign = '-';
            num = -num;
            size--;
        }
        else if (type & PLUS)
        {
            sign = '+';
            size--;
        }
        else if (type & SPACE)
        {
            sign = ' ';
            size--;
        }
    }
    if (type & SPECIAL)
    {
        if (base == 16)
            size -= 2;
        else if (base == 8)
            size--;
    }
    i = 0;
    if (num == 0)
        tmp[i++] = '0';
    else
        while (num != 0)
            tmp[i++] = digits[do_div(num, base)];
    if (i > precision)
        precision = i;
    size -= precision;
    if (!(type & (ZEROPAD + LEFT)))
    {
        while (size-- > 0)
        {
            if (buf <= end)
                *buf = ' ';
            ++buf;
        }
    }
    if (sign)
    {
        if (buf <= end)
            *buf = sign;
        ++buf;
    }
    if (type & SPECIAL)
    {
        if (base == 8)
        {
            if (buf <= end)
                *buf = '0';
            ++buf;
        }
        else if (base == 16)
        {
            if (buf <= end)
                *buf = '0';
            ++buf;
            if (buf <= end)
                *buf = digits[33];
            ++buf;
        }
    }
    if (!(type & LEFT))
    {
        while (size-- > 0)
        {
            if (buf <= end)
                *buf = c;
            ++buf;
        }
    }
    while (i < precision--)
    {
        if (buf <= end)
            *buf = '0';
        ++buf;
    }
    while (i-- > 0)
    {
        if (buf <= end)
            *buf = tmp[i];
        ++buf;
    }
    while (size-- > 0)
    {
        if (buf <= end)
            *buf = ' ';
        ++buf;
    }
    return buf;
}

/**
 * @brief Formats and prints a string to the given buffer.
 *
 * This function formats a string in the same way as printf() function
and writes the output to the provided buffer. It supports all the standard
format specifiers such as %d, %s, %f, etc. and also some additional
ones such as %n, %p, etc. If the length of the output string is greater
than the size of the buffer, the function will write up to size characters
to the buffer and return the number of characters that would have been
written if the buffer was large enough. If the buffer is NULL, the function
will not write anything and return the number of characters that would have
been written if the buffer was large enough.
 * @note Call this function if you are already dealing with a va_list.
You probably want snprintf instead.
 * @param buf Pointer to the buffer to write the output to.
 * @param size Maximum number of characters to write to the buffer.
 * @param fmt Format string.
 * @param args Variable arguments list containing the values to be printed.
 * @return Number of characters that would have been written if the buffer was
 */
int vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
    int len;
    unsigned long long num;
    int i, base;
    char *str, *end, c;
    const char *s;

    int flags; /* flags to number() */

    int field_width; /* width of output field */
    int precision;   /* min. # of digits for integers; max
                number of chars for from string */
    int qualifier;   /* 'h', 'l', or 'L' for integer fields */
                     /* 'z' support added 23/7/1999 S.H.    */
                     /* 'z' changed to 'Z' --davidm 1/25/99 */

    str = buf;
    end = buf + size - 1;

    if (end < buf - 1)
    {
        end = ((void *)-1);
        size = end - buf + 1;
    }

    for (; *fmt; ++fmt)
    {
        if (*fmt != '%')
        {
            if (str <= end)
                *str = *fmt;
            ++str;
            continue;
        }

        /* process flags */
        flags = 0;
    repeat:
        ++fmt; /* this also skips first '%' */
        switch (*fmt)
        {
        case '-':
            flags |= LEFT;
            goto repeat;
        case '+':
            flags |= PLUS;
            goto repeat;
        case ' ':
            flags |= SPACE;
            goto repeat;
        case '#':
            flags |= SPECIAL;
            goto repeat;
        case '0':
            flags |= ZEROPAD;
            goto repeat;
        }

        /* get field width */
        field_width = -1;
        if (isdigit(*fmt))
            field_width = skip_atoi(&fmt);
        else if (*fmt == '*')
        {
            ++fmt;
            /* it's the next argument */
            field_width = va_arg(args, int);
            if (field_width < 0)
            {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        /* get the precision */
        precision = -1;
        if (*fmt == '.')
        {
            ++fmt;
            if (isdigit(*fmt))
                precision = skip_atoi(&fmt);
            else if (*fmt == '*')
            {
                ++fmt;
                /* it's the next argument */
                precision = va_arg(args, int);
            }
            if (precision < 0)
                precision = 0;
        }

        /* get the conversion qualifier */
        qualifier = -1;
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' || *fmt == 'Z')
        {
            qualifier = *fmt;
            ++fmt;
            if (qualifier == 'l' && *fmt == 'l')
            {
                qualifier = 'L';
                ++fmt;
            }
        }

        /* default base */
        base = 10;

        switch (*fmt)
        {
        case 'c':
            if (!(flags & LEFT))
            {
                while (--field_width > 0)
                {
                    if (str <= end)
                        *str = ' ';
                    ++str;
                }
            }
            c = (unsigned char)va_arg(args, int);
            if (str <= end)
                *str = c;
            ++str;
            while (--field_width > 0)
            {
                if (str <= end)
                    *str = ' ';
                ++str;
            }
            continue;

        case 's':
            s = va_arg(args, char *);
            if (!s)
                s = "<NULL>";

            len = strlen(s); // FIXME: implement strnlen(s, precision);

            if (!(flags & LEFT))
            {
                while (len < field_width--)
                {
                    if (str <= end)
                        *str = ' ';
                    ++str;
                }
            }
            for (i = 0; i < len; ++i)
            {
                if (str <= end)
                    *str = *s;
                ++str;
                ++s;
            }
            while (len < field_width--)
            {
                if (str <= end)
                    *str = ' ';
                ++str;
            }
            continue;

        case 'p':
            if (field_width == -1)
            {
                field_width = 2 * sizeof(void *);
                flags |= ZEROPAD;
            }
            str = number(str, end,
                         (unsigned long)va_arg(args, void *),
                         16, field_width, precision, flags);
            continue;

        case 'n':
            /* FIXME:
             * What does C99 say about the overflow case here? */
            if (qualifier == 'l')
            {
                long *ip = va_arg(args, long *);
                *ip = (str - buf);
            }
            else if (qualifier == 'Z')
            {
                size_t *ip = va_arg(args, size_t *);
                *ip = (str - buf);
            }
            else
            {
                int *ip = va_arg(args, int *);
                *ip = (str - buf);
            }
            continue;

        case '%':
            if (str <= end)
                *str = '%';
            ++str;
            continue;

            /* integer number formats - set up the flags and "break" */
        case 'o':
            base = 8;
            break;

        case 'X':
            flags |= LARGE;
        case 'x':
            base = 16;
            break;

        case 'd':
        case 'i':
            flags |= SIGN;
        case 'u':
            break;

        default:
            if (str <= end)
                *str = '%';
            ++str;
            if (*fmt)
            {
                if (str <= end)
                    *str = *fmt;
                ++str;
            }
            else
            {
                --fmt;
            }
            continue;
        }
        if (qualifier == 'L')
            num = va_arg(args, long long);
        else if (qualifier == 'l')
        {
            num = va_arg(args, unsigned long);
            if (flags & SIGN)
                num = (signed long)num;
        }
        else if (qualifier == 'Z')
        {
            num = va_arg(args, size_t);
        }
        else if (qualifier == 'h')
        {
            num = (unsigned short)va_arg(args, int);
            if (flags & SIGN)
                num = (signed short)num;
        }
        else
        {
            num = va_arg(args, unsigned int);
            if (flags & SIGN)
                num = (signed int)num;
        }
        str = number(str, end, num, base,
                     field_width, precision, flags);
    }
    if (str <= end)
        *str = '\0';
    else if (size > 0)
        /* don't write out a null byte if the buf size is zero */
        *end = '\0';
    /* the trailing null byte doesn't count towards the total
     * ++str;
     */
    return str - buf;
}

/**
 * @brief Writes a formatted string to a buffer.
 *
This function writes a formatted string to the specified buffer, taking into account a maximum
buffer size.
 * @param buf Pointer to the buffer where the output should be written.
 * @param size Maximum number of characters that can be written to the buffer, including the
terminating null character.
 * @param fmt Format string.
 * @param ... Additional arguments to be substituted into the format string.
 * @return The total number of characters that would have been written if the buffer were large
enough, not including the terminating null character. If the returned value is greater
than or equal to the specified buffer size, then the output has been truncated.
 */
int snprintf(char *buf, size_t size, const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsnprintf(buf, size, fmt, args);
    va_end(args);
    return i;
}

/**
 * @brief Formats and writes a string from a va_list of arguments to a buffer.
 *
 * @param buf The buffer to store the resulting string.
 * @param fmt The format string.
 * @param args The va_list of arguments to format.
 * @return The number of characters written (excluding the null byte).
 */
int vsprintf(char *buf, const char *fmt, va_list args)
{
    return vsnprintf(buf, 0xFFFFUL, fmt, args);
}

/**
 * @brief Write formatted data to a string.
 *
 * This function writes the output to the string pointed to by buf, according to the format string fmt
and arguments passed to it. It is a wrapper function that calls vsprintf with a buffer size of 0xFFFFUL.
 * @param buf Pointer to the buffer where the resulting C-string is stored
 * @param fmt Format string containing the specifications for the output
 * @param ... Additional arguments to be formatted and inserted in the resulting C-string
 * @return Number of characters written, not including the terminating null character
 */
int sprintf(char *buf, const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsprintf(buf, fmt, args);
    va_end(args);
    return i;
}

/**
 * @brief Reads formatted input from a string, according to a format specifier.
 *
 * This function is similar to the `sscanf` function, except that the input is
 * read from a string instead of from the standard input. The arguments following
 * the `fmt` argument are pointers to variables where the parsed values should be
 * stored. The format specifier `fmt` is a string that contains conversion
 * specifications, each beginning with a '%' character and optionally ending
 * with a conversion qualifier (one of 'h', 'l', 'L', or 'Z'). The supported
 * conversion characters are 'c', 's', 'd', 'i', 'o', 'u', 'x', and 'X'.
 * @param buf A pointer to the input string.
 * @param fmt A format string that specifies how to interpret the input.
 * @param args A variable-length argument list of pointers to variables where
the parsed values should be stored.
 * @return The number of input items successfully matched and assigned, which
 can be fewer than the number of format specifiers.
 */
int vsscanf(const char *buf, const char *fmt, va_list args)
{
    const char *str = buf;
    char *next;
    int num = 0;
    int qualifier;
    int base;
    int field_width = -1;
    int is_sign = 0;

    while (*fmt && *str)
    {
        /* skip any white space in format */
        /* white space in format matches any amount of
         * white space, including none, in the input.
         */
        if (isspace(*fmt))
        {
            while (isspace(*fmt))
                ++fmt;
            while (isspace(*str))
                ++str;
        }

        /* anything that is not a conversion must match exactly */
        if (*fmt != '%' && *fmt)
        {
            if (*fmt++ != *str++)
                break;
            continue;
        }

        if (!*fmt)
            break;
        ++fmt;

        /* skip this conversion.
         * advance both strings to next white space
         */
        if (*fmt == '*')
        {
            while (!isspace(*fmt) && *fmt)
                fmt++;
            while (!isspace(*str) && *str)
                str++;
            continue;
        }

        /* get field width */
        if (isdigit(*fmt))
            field_width = skip_atoi(&fmt);

        /* get conversion qualifier */
        qualifier = -1;
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' || *fmt == 'Z')
        {
            qualifier = *fmt;
            fmt++;
        }
        base = 10;
        is_sign = 0;

        if (!*fmt || !*str)
            break;

        switch (*fmt++)
        {
        case 'c':
        {
            char *s = (char *)va_arg(args, char *);
            if (field_width == -1)
                field_width = 1;
            do
            {
                *s++ = *str++;
            } while (field_width-- > 0 && *str);
            num++;
        }
            continue;
        case 's':
        {
            char *s = (char *)va_arg(args, char *);
            if (field_width == -1)
                field_width = INT_MAX;
            /* first, skip leading white space in buffer */
            while (isspace(*str))
                str++;

            /* now copy until next white space */
            while (*str && !isspace(*str) && field_width--)
            {
                *s++ = *str++;
            }
            *s = '\0';
            num++;
        }
            continue;
        case 'n':
            /* return number of characters read so far */
            {
                int *i = (int *)va_arg(args, int *);
                *i = str - buf;
            }
            continue;
        case 'o':
            base = 8;
            break;
        case 'x':
        case 'X':
            base = 16;
            break;
        case 'd':
        case 'i':
            is_sign = 1;
        case 'u':
            break;
        case '%':
            /* looking for '%' in str */
            if (*str++ != '%')
                return num;
            continue;
        default:
            /* invalid format; stop here */
            return num;
        }

        /* have some sort of integer conversion.
         * first, skip white space in buffer.
         */
        while (isspace(*str))
            str++;

        if (!*str || !isdigit(*str))
            break;

        switch (qualifier)
        {
        case 'h':
            if (is_sign)
            {
                short *s = (short *)va_arg(args, short *);
                *s = (short)strtol(str, &next, base);
            }
            else
            {
                unsigned short *s = (unsigned short *)va_arg(args, unsigned short *);
                *s = (unsigned short)strtoul(str, &next, base);
            }
            break;
        case 'l':
            if (is_sign)
            {
                long *l = (long *)va_arg(args, long *);
                *l = strtol(str, &next, base);
            }
            else
            {
                unsigned long *l = (unsigned long *)va_arg(args, unsigned long *);
                *l = strtoul(str, &next, base);
            }
            break;
        case 'L':
            if (is_sign)
            {
                long long *l = (long long *)va_arg(args, long long *);
                *l = strtoll(str, &next, base);
            }
            else
            {
                unsigned long long *l = (unsigned long long *)va_arg(args, unsigned long long *);
                *l = strtoull(str, &next, base);
            }
            break;
        case 'Z':
        {
            size_t *s = (size_t *)va_arg(args, size_t *);
            *s = (size_t)strtoul(str, &next, base);
        }
        break;
        default:
            if (is_sign)
            {
                int *i = (int *)va_arg(args, int *);
                *i = (int)strtol(str, &next, base);
            }
            else
            {
                unsigned int *i = (unsigned int *)va_arg(args, unsigned int *);
                *i = (unsigned int)strtoul(str, &next, base);
            }
            break;
        }
        num++;

        if (!next)
            break;
        str = next;
    }
    return num;
}

/**
 * @brief Reads input from a string according to a specified format and stores the results in the corresponding variables.
 *
 * This function is similar to scanf(), but reads input from a string instead of from standard input. It scans the input
from the string buf using the format string fmt, and stores the results in the corresponding variables. The input
is parsed according to the format specifier in the format string, which specifies the types of the variables that the
input should be stored in.
 * @param buf Pointer to the buffer containing the input string to scan.
 * @param fmt Pointer to the format string that specifies the expected format of the input.
 * @param ... A variable number of arguments that correspond to the variables specified in the format string.
 * @return The number of input items that were successfully matched and assigned, which can be fewer than the number of
variables specified in the format string if there are not enough input items available.
 */
int sscanf(const char *buf, const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsscanf(buf, fmt, args);
    va_end(args);
    return i;
}
