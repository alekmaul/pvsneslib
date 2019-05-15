struct foo
{
  unsigned half:8;
  unsigned long __attribute__((packed)) whole:16;
};

f (struct foo *q)
{
  if (q->half != 0x12)
    abort ();
  if (q->whole != 0x5678L)
    abort ();
}

main ()
{
  struct foo bar;

  bar.half = 0x12;
  bar.whole = 0x5678L;
  f (&bar);
  exit (0);
}
