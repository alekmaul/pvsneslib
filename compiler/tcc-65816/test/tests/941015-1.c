int
foo1 (value)
     long long value;
{
  register const long long constant = 0xc0008000LL;

  if (value < constant)
    return 1;
  else
    return 2;
}

int
foo2 (value)
     unsigned long long value;
{
  register const unsigned long long constant = 0xc0008000LL;

  if (value < constant)
    return 1;
  else
    return 2;
}

main ()
{
  unsigned long long value = 0xc0000001LL;
  int x, y;

  x = foo1 (value);
  y = foo2 (value);
  if (x != y || x != 1)
    abort ();
  exit (0);
}
