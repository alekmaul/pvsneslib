int main ()
{
  long double x, y;

  x = 0x1.fffffffffffff8p10L;
  x *= 2;
  y = 0x1.fffffffffffff8p11L;
  if (memcmp (&x, &y, sizeof (x)) != 0)
    abort ();
  exit (0);
}
