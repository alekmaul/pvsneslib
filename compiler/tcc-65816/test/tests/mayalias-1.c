/* Tests that the may_alias attribute works as expected.
   Author: Osku Salerma <osku@iki.fi> Apr 2002.  */

extern void abort(void);
extern void exit(int);

typedef char __attribute__((__may_alias__)) short_a;

int
main (void)
{
  int a = 0x1234;
  short_a *b = (short_a*) &a;

  b[1] = 0;

  if (a == 0x1234)
    abort();

  exit(0);
}
