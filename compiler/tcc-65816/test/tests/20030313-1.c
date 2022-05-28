struct A
{
  unsigned long p, q, r, s;
} x = { 13, 14, 15, 16 };

extern void abort (void);
extern void exit (int);

static inline struct A *
bar (void)
{
#if 1
  struct A *r;

  switch (8)
    {
    case 2:
      exit (1);
      break;
    case 8:
      r = &x;
      break;
    default:
      exit (2);
      break;
    }
    
  if(r != &x) exit(42);
  return r;
#else
  return &x;
#endif
}

void
foo (unsigned long *x, int y)
{
#if 1
  if (y != 12)
    exit (3);
  if (x[0] != 1 || x[1] != 11)
    exit (4);
  if (x[2] != 2 || x[3] != 12)
    exit (5);
#endif
  if (x[4] != 3) exit(23); if(x[5] != 13)
    exit (6);
#if 1
  if (x[6] != 4 || x[7] != 14)
    exit (7);
  if (x[8] != 5 || x[9] != 15)
    exit (8);
  if (x[10] != 6 || x[11] != 16)
    exit (9);
#endif
}

int
main (void)
{
  unsigned long a[40];
  int b = 0;

#if 1
  a[b++] = 1;
  a[b++] = 11;
  a[b++] = 2;
  a[b++] = 12;
#endif
  a[b++] = 3;
  a[b++] = bar()->p;
#if 1
  a[b++] = 4;
  a[b++] = bar()->q;
  a[b++] = 5;
  a[b++] = bar()->r;
  a[b++] = 6;
  a[b++] = bar()->s;
#endif
  foo (a, b);
  exit (0);
}
