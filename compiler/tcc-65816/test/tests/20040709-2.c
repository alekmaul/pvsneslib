// preprocessor deficiency
// inserting the macro by hand makes the test work

/* Test arithmetics on bitfields.  */

extern void abort (void);
extern void exit (int);

unsigned int
myrnd (void)
{
  static unsigned int s = 1388815473;
  s *= 1103515245;
  s += 12345;
  return (s / 65536) % 2048;
}

#define T(S)					\
struct S s##S;					\
struct S retme##S (struct S x)			\
{						\
  return x;					\
}						\
						\
unsigned int fn1##S (unsigned int x)		\
{						\
  struct S y = s##S;				\
  y.k += x;					\
  y = retme##S (y);				\
  return y.k;					\
}						\
						\
unsigned int fn2##S (unsigned int x)		\
{						\
  struct S y = s##S;				\
  y.k += x;					\
  y.k %= 15;					\
  return y.k;					\
}						\
						\
unsigned int retit##S (void)			\
{						\
  return s##S.k;				\
}						\
						\
unsigned int fn3##S (unsigned int x)		\
{						\
  s##S.k += x;					\
  return retit##S ();				\
}						\
						\
void test##S (void)				\
{						\
  int i;					\
  unsigned int mask, v, a, r;			\
  struct S x;					\
  char *p = (char *) &s##S;			\
  for (i = 0; i < sizeof (s##S); ++i)		\
    *p++ = myrnd ();				\
  if (0) /* __builtin_classify_type (s##S.l) == 8) */	\
    s##S.l = (int)5.25;				\
  s##S.k = -1;					\
  mask = s##S.k;				\
  v = myrnd ();					\
  a = myrnd ();					\
  s##S.k = v;					\
  x = s##S;					\
  r = fn1##S (a);				\
  if (x.i != s##S.i || x.j != s##S.j		\
      || x.k != s##S.k || x.l != s##S.l		\
      || ((v + a) & mask) != r)			\
    abort ();					\
  v = myrnd ();					\
  a = myrnd ();					\
  s##S.k = v;					\
  x = s##S;					\
  r = fn2##S (a);				\
  if (x.i != s##S.i || x.j != s##S.j		\
      || x.k != s##S.k || x.l != s##S.l		\
      || ((((v + a) & mask) % 15) & mask) != r)	\
    abort ();					\
  v = myrnd ();					\
  a = myrnd ();					\
  s##S.k = v;					\
  x = s##S;					\
  r = fn3##S (a);				\
  if (x.i != s##S.i || x.j != s##S.j		\
      || s##S.k != r || x.l != s##S.l		\
      || ((v + a) & mask) != r)			\
    abort ();					\
}

#define pck __attribute__((packed))

// TCC is more picky when it comes to where attributes can be placed.
// struct A pck {...}; breaks
// struct A {...} pck; works

struct A { unsigned short i : 1, l : 1, j : 3, k : 11; } pck; T(A)
struct B { unsigned short i : 4, j : 1, k : 11; unsigned int l; } pck; T(B)
struct C { unsigned int l; unsigned short i : 4, j : 1, k : 11; } pck; T(C)
//struct D { unsigned long long l : 6, i : 6, j : 23, k : 29; } pck; T(D)
//struct E { unsigned long long l, i : 12, j : 23, k : 29; } pck; T(E)
//struct F { unsigned long long i : 12, j : 23, k : 29, l; } pck; T(F)
struct G { unsigned short i : 1, j : 1, k : 6; unsigned long long l; } pck; T(G)
struct H { unsigned short i : 6, j : 2, k : 8; unsigned long long l; } pck; T(H)
struct I { unsigned short i : 1, j : 6, k : 1; unsigned long long l; } pck; T(I)
struct J { unsigned short i : 1, j : 8, k : 7; unsigned short l; } pck; T(J)
struct K { unsigned int k : 6, l : 1, j : 10, i : 15; } pck; T(K)
struct L { unsigned int k : 6, j : 11, i : 15; unsigned int l; } pck; T(L)
struct M { unsigned int l; unsigned short k : 6, j : 11, i : 15; } pck; T(M)
//struct N { unsigned long long l : 6, k : 6, j : 23, i : 29; } pck; T(N)
//struct O { unsigned long long l, k : 12, j : 23, i : 29; } pck; T(O)
//struct P { unsigned long long k : 12, j : 23, i : 29, l; } pck; T(P)
struct Q { unsigned short k : 12, j : 1, i : 3; unsigned long long l; } pck; T(Q)
struct R { unsigned short k : 2, j : 11, i : 3; unsigned long long l; } pck; T(R)
struct S { unsigned short k : 1, j : 6, i : 9; unsigned long long l; } pck; T(S)
struct T { unsigned short k : 1, j : 8, i : 7; unsigned short l; } pck; T(T)
struct U { unsigned short j : 6, k : 1, i : 9; unsigned long long l; } pck; T(U)
struct V { unsigned short j : 8, k : 1, i : 7; unsigned short l; } pck; T(V)
struct W { long double l; unsigned int k : 12, j : 13, i : 7; } pck; T(W)
struct X { unsigned int k : 12, j : 13, i : 7; long double l; } pck; T(X)
struct Y { unsigned int k : 12, j : 11, i : 9; long double l; } pck; T(Y)
struct Z { long double l; unsigned int j : 13, i : 7, k : 12; } pck; T(Z)

int
main (void)
{
  testA ();
  testB ();
  testC ();
//  testD ();
//  testE ();
//  testF ();
  testG ();
  testH ();
  testI ();
  testJ ();
  testK ();
  testL ();
  testM ();
//  testN ();
//  testO ();
//  testP ();
  testQ ();
  testR ();
  testS ();
  testT ();
  testU ();
  testV ();
  testW ();
  testX ();
  testY ();
  testZ ();
  exit (0);
}
