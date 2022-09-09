extern void abort (void);

long long
f (long long a, long long b)
{
  return ((a + b) << 16) >> 16;
}

long long a = 0x12345678LL;
long long b = 0x23456787LL;
long long c = ((0x12345678LL + 0x23456787LL) << 16) >> 16;

int
main ()
{
  if (f (a, b) != c)
    abort ();
  return 0;
}
