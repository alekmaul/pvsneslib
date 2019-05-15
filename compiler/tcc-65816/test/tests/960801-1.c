#if 0
unsigned
f ()
{
  long long l2;
  unsigned short us;
  unsigned long long ul;
  short s2;

  ul = us = l2 = s2 = -1;
  return ul;
}
#endif

unsigned long long
g ()
{
  long long l2;
  unsigned short us;
  unsigned long long ul;
  short s2;

  ul = us = l2 = s2 = -1;
  return ul;
}

main ()
{
#if 0
  if (f () != (unsigned short) -1)
    abort ();
#endif
  if (g () != (unsigned short) -1)
    abort ();
  exit (0);
}
