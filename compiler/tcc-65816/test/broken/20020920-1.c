// serious problem: a.b is not initialized properly; TCC
// does a.b.x, then stops. does not look like it's easy to fix

extern void abort (void);
extern void exit (int);

struct B
{
  int x;
  int y;
};

struct A
{
  int z;
  struct B b;
};

struct A
f ()
{
  struct B b = { 0, 1 };
  struct A a = { 2, b };
  return a;
}

int
main (void)
{
  struct A a = f ();
#if 0
  if (a.z != 2 || a.b.x != 0 || a.b.y != 1)
    abort ();
#endif
  //if (a.z != 2) exit(1);
  //if(a.b.x != 0) exit(2);
  if(a.b.y != 1) exit(3);
  exit (0);
}
