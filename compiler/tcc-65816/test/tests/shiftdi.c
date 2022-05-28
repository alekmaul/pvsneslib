/* Failed on sparc with -mv8plus because sparc.c:set_extends() thought
   erroneously that SImode ASHIFT chops the upper bits, it does not.  */

typedef unsigned long long uint64;

void g(uint64 x, int y, int z, uint64 *p)
{
	unsigned w = ((x >> y) & 0xffffULL) << (z & 0xf);
	*p |= (w & 0xffffULL) << z;
}

int main(void)
{
	uint64 a = 0;
	g(0xdeadbeefULL, 0, 0, &a);
	return (a == 0xbeef) ? 0 : 1;
}



