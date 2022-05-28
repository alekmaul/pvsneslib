int main()
{
  short x = -32768;
  float a = x;
  printf("0x%x\n", *((int*)&a));
  printf("%lf\n", a);
  short y = 32767;
  float b = y;
  printf("%lf\n", b/a);
  short z = b/a;
  printf("%d\n", z);
}