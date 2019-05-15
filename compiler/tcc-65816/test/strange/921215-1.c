  void p(void ((*f) (void ())))
    {
      void r()
	{
	  foo ();
	}

      f(r);
    }

  void q(void ((*f)()))
    {
      f();
    }
main()
{
#ifndef NO_TRAMPOLINES

  p(q);
#endif
  exit(0);
}

foo(){}
