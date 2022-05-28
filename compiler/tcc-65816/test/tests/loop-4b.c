int
f()
{
  int j = 1;
  long i;
  i = 0x6000L;
  do
    {
      j <<= 1;
      i += 0x1000L;
    } while (i < -0x6000L);
  return j;
}

int
main ()
{
  if (f () != 2)
    abort ();
  exit (0);
}
