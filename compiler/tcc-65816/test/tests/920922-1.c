unsigned long long*
f(p)unsigned long long*p;
{
  unsigned long long a = (*p++) >> 24;
  return p + a;
}

main ()
{
  unsigned long long x = 0x80000000UL;
  if (f(&x) != &x + 0x81)
    abort();
  exit(0);
}
