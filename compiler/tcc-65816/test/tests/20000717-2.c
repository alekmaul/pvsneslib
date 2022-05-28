static void
compare (long long foo)
{
  if (foo < 65537LL)
    abort();
}
int main(void)
{
  compare (131071LL);
  exit (0);
}
