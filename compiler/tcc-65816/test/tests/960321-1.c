char a[10] = "deadbeef";

char
acc_a (long i)
{
  return a[i-30000L];
}

main ()
{
  if (acc_a (30000L) != 'd')
    abort ();
  exit (0);
}
