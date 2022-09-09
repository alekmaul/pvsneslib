// does not work with 32-bit long longs

#ifdef __65816__
typedef unsigned long long __ulonglong;
typedef long long __longlong;
#else
typedef unsigned int __ulonglong;
typedef int __longlong;
#endif

__ulonglong
gcd_ll (__ulonglong x, __ulonglong y)
{
  for (;;)
    {
      if (y == 0)
	return (__ulonglong) x;
      x = x % y;
      if (x == 0)
	return (__ulonglong) y;
      y = y % x;
    }
}

__ulonglong
powmod_ll (__ulonglong b, __ulonglong e, __ulonglong m)
{
  __ulonglong t;
  __ulonglong pow;
  __longlong i;

  if (e == 0)
    return 1;

  /* Find the most significant bit in E.  */
  t = e;
  for (i = 0; t != 0; i++)
    t >>= 1;

  /* The most sign bit in E is handled outside of the loop, by beginning
     with B in POW, and decrementing I.  */
  pow = b;
  i -= 2;

  for (; i >= 0; i--)
    {
      pow = pow * pow % m;
      if ((1 << i) & e)
	pow = pow * b % m;
    }

  return pow;
}

__ulonglong factab[10];

void
facts (t, a_int, x0, p)
     __ulonglong t;
     __longlong a_int;
     __longlong x0;
     __ulonglong p;
{
  __ulonglong *xp = factab;
  __ulonglong x, y;
  __ulonglong q = 1;
  __ulonglong a = a_int;
  __longlong i;
  __ulonglong d;
  __longlong j = 1;
  __ulonglong tmp;
  __longlong jj = 0;

  if(t != 134217727 || a_int != -1 || x0 != 3 || p != 27) exit(5);

  x = x0;
  y = x0;

  if(x != 3 || y != 3) exit(6);

  for (i = 1; i < 5; i++)
    {
      x = powmod_ll (x, p, t) + a;
      y = powmod_ll (y, p, t) + a;
      y = powmod_ll (y, p, t) + a;

      if (x > y)
	tmp = x - y;
      else
	tmp = y - x;
      q = (__ulonglong) q * tmp % t;

      if (i == j)
	{
	  jj += 1;
	  j += jj;
	  d = gcd_ll (q, t);
	  if (d != 1)
	    {
	      *xp++ = d;
	      t /= d;
	      if (t == 1)
		{
		  return;
		  *xp = 0;
		}
	    }
	}
    }
}

main ()
{
  __ulonglong t;
  __ulonglong x0, a;
  __ulonglong p;

  p = 27;
  t = (1ULL << p) - 1;
  if(t != 134217727) exit(3);

  a = -1;
  x0 = 3;

  if(a != -1 || x0 != 3) exit(4);
  facts (t, a, x0, p);
  //exitl4(factab[0], factab[1], factab[2], factab[3]);
  //printf("%lld %lld %lld %lld\n",factab[0], factab[1], factab[2], factab[3]);
  if (factab[0] != 7 || factab[1] != 73 /* || factab[2] != 262657 */)
    abort();
  exit (0);
}
