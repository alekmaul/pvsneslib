#include <limits.h>

#if 0 //ULONG_LONG_MAX != 18446744073709551615ull && ULONG_MAX != 18446744073709551615ull
int main(void) { exit (0); }
#else
#if 1 //ULONG_MAX != 18446744073709551615ull
typedef unsigned long long ull;
#else
typedef unsigned long ull;
#endif

void checkit(int);

main () {
    const ull a = 0x140000ULL;
    const ull b = 0x8000ULL;
    const ull c = a/b;
    const ull d = 0x140000ULL / 0x8000ULL;

    checkit ((int) c);
    checkit ((int) d);

    exit(0);
}

void checkit (int a)
{
  if (a != 40)
    abort();
}
#endif
