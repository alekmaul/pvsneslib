#include <string.h>
//#include <stdio.h>

void
a(void *x,int y)
{
  if (y != 1234)
    abort ();
}

char allaalla[100];

int
main()
{
  a(strcpy(/* alloca(100) */ allaalla,"abc"),1234);
  return 0;
}
