long val = 0x5e00;

long
f1 (void)
{
  return 0x13;
}

long
f2 (void)
{
  return 0x5e00;
}

void
f3 (long b)
{
  val = b;
}

void
f4 ()
{
  long v = f1 ();
  long o = f2 ();
  v = (v & 0x00ff) | (o & 0xff00);
  f3 (v);
}

main ()
{
  f4 ();
  if (val != 0x5e13)
    abort ();
  exit (0);
}
