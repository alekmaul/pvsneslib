#define mask  0xff00L
#define value 0xab00L

long
foo (long x)
{
  if ((x & mask) == value)
    return x & 0xffL;
  return 1;
}

int
main (void)
{
  if (foo (value) != 0 || foo (0) != 1)
    abort ();

  exit (0);
}
