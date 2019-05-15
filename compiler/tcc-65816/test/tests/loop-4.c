int
f()
{
  int j = 1;
  long i;
  for (i = -0x7000L; i < 0x6000L; i += 0x1000L) j <<= 1;
  return j;
}

int
main ()
{
  if (f () != 8192)
    abort ();
  exit (0);
}
