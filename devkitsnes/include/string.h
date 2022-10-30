#include "stddef.h"

void *memcpy(void *s1, const void *s2, size_t n);
void *mempcpy(void *s1, const void *s2, size_t n);
void *memmove(void *s1, const void *s2, size_t n);
void *memset(void *b, int c, size_t n);

int memcmp(const void *s1, const void *s2, size_t n);

int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);

char *strcpy(char *s1, const char *s2);
char *strncpy(char *s1, const char *s2, size_t n);

size_t strlen(const char *);
char *strchr(const char *, int);
char *strrchr(const char *, int);

char *strdup(const char *);
