/* { dg-do run } */
/* { dg-options "-O3" } */
/* { dg-require-effective-target ilp32 } */

typedef unsigned int u32;
typedef unsigned long long u64;

u64 __attribute__((noinline))
foo (u32 high, u32 low)
{
  return ((u64)high << 16) | low;
}

u32 __attribute__((noinline))
right (u64 t)
{
  return (u32)(t >> 16);
}

u64 __attribute__((noinline))
left (u32 t)
{
  return (u64)t << 16;
}

u32 __attribute__((noinline))
right2 (u64 t)
{
  return (u32)(t >> 24);
}

u64 __attribute__((noinline))
left2 (u32 t)
{
  return (u64)t << 8;
}

u64 __attribute__((noinline))
zeroextend (u32 t)
{
  return (u64)t;
}

extern void abort ();

int
main ()
{
  if (foo (13000, 12000) != 851980000ULL)
    abort ();

  if (right (851980000ULL) != 13000)
    abort ();

  if (left (13000) != 851968000ULL)
    abort ();

  if (right2 (851968000ULL) != 50)
    abort ();

  if (left2 (13000) != 3328000ULL)
    abort ();

  if (zeroextend (13000) != 13000ULL)
    abort ();

  return 0;
}
