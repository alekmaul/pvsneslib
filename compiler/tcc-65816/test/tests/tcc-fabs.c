#include <math.h>

int main()
{
  if (fabs(20.0) != 20.0) abort();
  if (fabs(-20.0) != 20.0) abort();
  return 0;
}
