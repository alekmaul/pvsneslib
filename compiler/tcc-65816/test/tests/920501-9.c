#include <stdio.h>

long long proc1(){return 1LL;}
long long proc2(){return 0x1234LL;}
long long proc3(){return 0xabcd1234LL;}
long long proc4(){return -1LL;}
long long proc5(){return 0xabcdLL;}

print_longlong(x,buf)
     long long x;
     char *buf;
{
  unsigned long l;
  l= (x >> 16) & 0xffff;
  if (l != 0)
    sprintf(buf,"%lx%04.lx",l,((unsigned long)x & 0xffff));
  else
    sprintf(buf,"%lx",((unsigned long)x & 0xffff));
}

main(){char buf[100];
print_longlong(proc1(),buf);if(strcmp("1",buf))abort();
print_longlong(proc2(),buf);if(strcmp("1234",buf))abort();
print_longlong(proc3(),buf);if(strcmp("abcd1234",buf))abort();
print_longlong(proc4(),buf);if(strcmp("ffffffff",buf))abort();
print_longlong(proc5(),buf);if(strcmp("abcd",buf))abort();
exit(0);}
