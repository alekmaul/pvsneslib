f (signed long long int x)
{
  return x > 0xFFFFLL || x < -0x8000LL;
}

main ()
{
  if (f (0) != 0)
    abort ();
  exit (0);
}
