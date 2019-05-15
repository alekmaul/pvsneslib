main()
{
#if 0
  if ((double) 18446744073709551615ULL < 1.84467440737095e+19 ||
      (double) 18446744073709551615ULL > 1.84467440737096e+19)
    abort();
#endif

  if (16777217LL != (float)16777217e0)
    abort();

  exit(0);
}
