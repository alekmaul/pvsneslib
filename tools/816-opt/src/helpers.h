#ifndef HELPERS_H
#define HELPERS_H

#include <ctype.h>
#include <pcre.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*!
 * @brief Max length of the line.
 */
#define MAXLEN_LINE 10240

/**
 * @struct dynArray
 * @brief Structure to store an array of string
 * and the lenght of the array.
 * @var dynArray::arr
 * Member 'arr' contains the array of strings.
 * @var dynArray::used
 * Member 'used' contains length of arr (number of elements)
 * in the array.
 */
typedef struct dynArray
{
    char **arr;
    size_t used;
} dynArray;

void freedynArray(dynArray s);
int matchStr(const char *str1, const char *str2);
int startWith(const char *source, const char *prefix);
int endWith(const char *source, const char *prefix);
int isInText(const char *source, const char *pattern);
int min(const int a, const int b);
int max(const int a, const int b);
char *trimWhiteSpace(char *str);
char *sliceStr(char *str, int slice_from, int slice_to);
char *replaceStr(char *str, char *orig, char *rep);
char *splitStr(char *str, char *sep, size_t pos);
dynArray regexMatchGroups(char *source, char *regex, const size_t maxGroups);
dynArray pushToArray(dynArray text_opt, char *str);

#endif
