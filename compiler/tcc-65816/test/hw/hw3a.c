#include "hw3.h"

extern void write(char* s);
extern void hexun(int);

void printsizes(void)
{
  write("sizeof(char) == "); hexun(sizeof(char));
  write("sizeof(short) == "); hexun(sizeof(short));
  write("sizeof(int) == "); hexun(sizeof(int));
  write("sizeof(long) == "); hexun(sizeof(long));
  write("sizeof(long long) == "); hexun(sizeof(__longlong));
}
