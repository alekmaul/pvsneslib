float modf(float x, float *fp)
{
    *fp = (float)(long long)x;
    return x - *fp;
}

float ldexp(float x, int exp)
{
  float factor;
  int bit;

  /* Check for zero, nan and infinity. */
  if (x != x || x + x == x )
    return x;

  if (exp < 0)
    {
      exp = -exp;
      factor = 0.5L;
    }
  else
    factor = 2.0L;

  for (bit = 1; bit <= exp; bit <<= 1, factor *= factor)
    if (exp & bit)
      x *= factor;

  return x;
}

float floor(float d)
{
	float fract;

	if (d<0.0) {
		d = -d;
		fract = modf(d, &d);
		if (fract != 0.0)
			d += 1;
		d = -d;
	} else
		modf(d, &d);
	return(d);
}

float ceil(float d)
{
	return(-floor(-d));
}


float exp(float arg)
{
	static float	p0	= .2080384346694663001443843411e7;
	static float	p1	= .3028697169744036299076048876e5;
	static float	p2	= .6061485330061080841615584556e2;
	static float	q0	= .6002720360238832528230907598e7;
	static float	q1	= .3277251518082914423057964422e6;
	static float	q2	= .1749287689093076403844945335e4;
	static float	log2e	= 1.4426950408889634073599247;
	static float	sqrt2	= 1.4142135623730950488016887;
	static float	maxf	= 10000;
	float fract;
	float temp1, temp2, xsq;
	int ent;

	if(arg == 0.)
		return(1.);
	if(arg < -maxf)
		return(0.);
	if(arg > maxf) {
		/* errno = ERANGE; */
		return 1e42; //(HUGE);
	}
#if 1
	arg *= log2e;
	ent = floor(arg);
	fract = (arg-ent) - 0.5;
	xsq = fract*fract;
	temp1 = ((p2*xsq+p1)*xsq+p0)*fract;
	temp2 = ((1.0*xsq+q2)*xsq+q1)*xsq + q0;
	return(ldexp(sqrt2*(temp2+temp1)/(temp2-temp1), ent));
#else
	float x = arg;
	/* Express e**x = e**g 2**n
	 *   = e**g e**( n loge(2) )
	 *   = e**( g + n loge(2) )
	 */
	float z = floor( log2e * x + 0.5f ); /* floor() truncates toward -infinity. */
	x -= z * 0.693359375;
	x -= z * -2.12194440e-4;
	int n = z;

	z = x * x;
	/* Theoretical peak relative error in [-0.5, +0.5] is 4.2e-9. */
	z =
	((((( 1.9875691500E-4f  * x
	   + 1.3981999507E-3f) * x
	   + 8.3334519073E-3f) * x
	   + 4.1665795894E-2f) * x
	   + 1.6666665459E-1f) * x
	   + 5.0000001201E-1f) * z
	   + x
	   + 1.0f;

	/* multiply by power of 2 */
	x = ldexp( z, n );

	return( x );
#endif
}

float frexp(float x, int *exp)
{
  float exponents[20], *next;
  int exponent, bit;

  /* Check for zero, nan and infinity. */
  if (x != x || x + x == x )
    {
      *exp = 0;
      return x;
    }

  if (x < 0)
    return -frexp(-x, exp);

  exponent = 0;
  if (x > 1.0)
    {
      for (next = exponents, exponents[0] = 2.0L, bit = 1;
           *next <= x + x;
           bit <<= 1, next[1] = next[0] * next[0], next++);

      for (; next >= exponents; bit >>= 1, next--)
        if (x + x >= *next)
          {
            x /= *next;
            exponent |= bit;
          }

    }

  else if (x < 0.5)
    {
      for (next = exponents, exponents[0] = 0.5L, bit = 1;
           *next > x;
           bit <<= 1, next[1] = next[0] * next[0], next++);

      for (; next >= exponents; bit >>= 1, next--)
        if (x < *next)
          {
            x /= *next;
            exponent |= bit;
          }

      exponent = -exponent;
    }

  *exp = exponent;
  return x;
}


float log(float arg)
{
	static float	log2	= 0.693147180559945309e0;
	static float	sqrto2	= 0.707106781186547524e0;
	static float	p0	= -.240139179559210510e2;
	static float	p1	= 0.309572928215376501e2;
	static float	p2	= -.963769093368686593e1;
	static float	p3	= 0.421087371217979714e0;
	static float	q0	= -.120069589779605255e2;
	static float	q1	= 0.194809660700889731e2;
	static float	q2	= -.891110902798312337e1;
	float x,z, zsq, temp;
	int exp;

	if(arg <= 0.) {
		//errno = EDOM;
		return -1e42;//(-HUGE);
	}
	x = frexp(arg,&exp);
	while(x<0.5) {
		x = x*2;
		exp = exp-1;
	}
	if(x<sqrto2) {
		x = 2*x;
		exp = exp-1;
	}

	z = (x-1)/(x+1);
	zsq = z*z;

	temp = ((p3*zsq + p2)*zsq + p1)*zsq + p0;
	temp = temp/(((1.0*zsq + q2)*zsq + q1)*zsq + q0);
	temp = temp*z + exp*log2;
	return(temp);
}

float log10(float arg)
{
	static float	ln10	= 2.302585092994045684;
	return(log(arg)/ln10);
}

float pow(float arg1, float arg2)
{
	float temp;
	long l;

	if(arg1 <= 0.) {
		if(arg1 == 0.) {
			if(arg2 <= 0.)
				goto domain;
			return(0.);
		}
		l = arg2;
		if(l != arg2)
			goto domain;
		temp = exp(arg2 * log(-arg1));
		if(l & 1)
			temp = -temp;
		return(temp);
	}
	return(exp(arg2 * log(arg1)));

domain:
	//errno = EDOM;
	return(0.);
}

float fabs(float f)
{
	if (f < 0) return -f;
	else return f;
}
