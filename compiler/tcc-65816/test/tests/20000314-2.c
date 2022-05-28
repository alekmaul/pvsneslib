typedef unsigned long long uint32;
const uint32 bigconst = 1 << 18;

int a = 1;

static
uint32 getmask(void)
{
    if (a)
      return bigconst;
    else
      return 0;
}

main()
{
    uint32 f = getmask();
    if (sizeof (long long) == 4
	&& f != bigconst) abort ();
    exit (0);
}


