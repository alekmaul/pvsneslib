static inline long
g1 (double x)
{
  return (double) (long) x;
}

long
g2 (double f)
{
  return f;
}

double
f (long i)
{
  if (g1 (i) != g2 (i))
    abort ();
  return g2 (i);
}

main ()
{
  if (f (12345L) != 12345L)
    abort ();
  if (f (12345L) != g2 (12345L))
    abort ();
  exit (0);
}
