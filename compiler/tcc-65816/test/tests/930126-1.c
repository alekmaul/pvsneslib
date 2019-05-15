struct s {
  unsigned long a:4, b:16;
};

struct s
f(struct s x)
{
  x.b = 0xcdef;
  return x;
}

main()
{
  static struct s i;
  i.a = 12;
  i = f(i);
  if (i.a != 12 || i.b != 0xcdef)
    abort();
  exit(0);
}
