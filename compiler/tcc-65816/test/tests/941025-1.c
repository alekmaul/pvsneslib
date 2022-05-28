long f (x, y)
     long x,y;
{
  return (x > 1) ? y : (y & 1);
}

main ()
{
  if (f (2L, 0xdcdeL) != 0xdcdeL)
    abort ();
  exit (0);
}
