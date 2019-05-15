/* Verify that flexible arrays can be initialized from STRING_CST
   constructors. */

#if 1
/* Baselines.  */
struct {
  char a1c;
  char *a1p;
} a1 = {
  '4',
  "62"
};
#endif

#if 1
struct {
  char a2c;
  char a2p[2];
} a2 = {
  'v',
  "cq"
};
#endif

#if 1
/* The tests.  */
struct {
  char y4c;
  char y4p[];
} y4 = {
  '9',
  { 'e', 'b' }
};

struct {
  char x3c;
  char x3p[];
} x3 = {
  'o',
  "wx"
};
#endif


main()
{
#if 1
  if (a1.a1c != '4')
    abort();
  if (a1.a1p[0] != '6')
    abort();
  if (a1.a1p[1] != '2')
    abort();
  if (a1.a1p[2] != '\0')
    abort();
#endif

#if 1
  if (a2.a2c != 'v')
    abort();
  if (a2.a2p[0] != 'c')
    abort();
  if (a2.a2p[1] != 'q')
    abort();
#endif

#if 1
  if (x3.x3c != 'o')
    abort();
  if (x3.x3p[0] != 'w')
    abort();
  if (x3.x3p[1] != 'x')
    abort();

  if (y4.y4c != '9')
    abort();
  if (y4.y4p[0] != 'e')
    abort();
  if (y4.y4p[1] != 'b')
    abort();
#endif

  return 0;
}
