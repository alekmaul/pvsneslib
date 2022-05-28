long int a = 100;
long int b = 328;

long
f ()
{
  return ((long long) a * (long long) b) >> 8;
}

main ()
{
  if (f () < 0)
    abort ();
  exit (0);
}
