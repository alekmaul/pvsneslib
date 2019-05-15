#include <math.h>

int main()
{
  float a = 2.0;
  float b = 6.0;
  if (fabs(pow(a,b) - 64.0) > .0002) abort();
  return 0;
}
