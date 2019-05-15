/* { dg-do run { target fpic } } */
/* { dg-options "-O2 -fpic" } */

extern void abort (void);
extern void exit (int);

double
foo (void)
{
  return (__extension__ ((union { unsigned long long __l __attribute__((__mode__(__SI__))); float __d; }) { __l: 0x4080UL }).__d);
}

main ()
{
  if (foo() != 1.0)
    abort ();
  exit (0);
}
