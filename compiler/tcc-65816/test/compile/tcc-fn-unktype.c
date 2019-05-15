/* using unknown pointer type as the first argument in function implementations caused segfault
   because it was mistaken for a K&R-style function declaration */
int main(unknown *x, void *x)
{
  return 0;
}
