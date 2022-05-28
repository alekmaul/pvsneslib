long long
f (long long x)
{
  return x / 100000000LL;
}

main ()
{
  if (f (100000000LL) != 1 || f (1000000000LL) != 10)
    abort ();
  exit (0);
}
