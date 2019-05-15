/* Copyright (C) 2001 Free Software Foundation, Inc.  */

/* Source: Neil Booth, 4 Nov 2001, derived from PR 2820 - field lookup in
   nested anonymous entities was broken.  */

struct
{
  int x;
  struct
  {
    int a;
    union
    {
      int b;
    } zz;
  } yy;
} foo;

int
main(int argc, char *argv[])
{
  foo.yy.zz.b = 6;
  foo.yy.a = 5;

  if (foo.yy.zz.b != 6)
    abort ();

  return 0;
}
