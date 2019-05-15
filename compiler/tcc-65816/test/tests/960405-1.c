#define X  5.948657473873442e37

long double x = X + X;
long double y = 2.0 * X;

main ()
{
#if ! defined (__vax__) && ! defined (_CRAY)
  if (x != y)
    abort ();
#endif
  exit (0);
}
