f (void (*func) ())
{
  func ();
}

  void t0 ()
    {
    }

  void t1 ()
    {
      f (t0);
    }

  void t2 ()
    {
      t1 ();
    }
main ()
{
#ifndef NO_TRAMPOLINES

  t1 ();
  t1 ();
  t2 ();
#endif
  exit (0);
}
