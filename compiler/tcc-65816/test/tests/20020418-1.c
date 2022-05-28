/* ifcvt accidently deletes a referenced label while generating
   conditional traps on machines having such patterns */

#define __builtin_expect(a, b) (a)
#define __builtin_trap() abort()

struct foo { int a; };

void gcc_crash(struct foo *p)
{
	if (__builtin_expect(p->a < 52, 0))
		__builtin_trap();
 top:
	p->a++;
	if (p->a >= 62)
		goto top;
}

int main(void)
{
	struct foo x;

	x.a = 53;
	gcc_crash(&x);

	exit (0);
}
