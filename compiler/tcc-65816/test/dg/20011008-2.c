/* { dg-do run } */
/* { dg-options "-O0" } */

extern void abort (void);
extern void exit (int);

struct { union {int x; int y;} z; int q; } b;
union { struct {int x;} z; int q; } e;

main()
{
  b.z.y = 10;
  b.z.x = 15;
  if (b.z.y != 15)
    abort();

  e.z.x = 10;
  e.q = 15;
  if (e.z.x != 15)
    abort();

  exit(0);
}
