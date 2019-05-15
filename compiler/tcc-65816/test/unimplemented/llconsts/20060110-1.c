extern void abort (void);

long long 
f (long long a) 
{ 
  return (a << 16) >> 16; 
} 
long long a = 0x12345678LL;
long long b = (0x12345678LL << 16) >> 16;
int
main ()
{
  if (f (a) != b)
    abort ();
  return 0;
}
