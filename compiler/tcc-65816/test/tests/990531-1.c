   unsigned long bad(int reg, unsigned long inWord)
   {
       union {
           unsigned long word;
           unsigned char byte[4];
       } data;

       data.word = inWord;
       data.byte[reg] = 0;

       return data.word;
   }

main()
{
  /* XXX This test could be generalized.  */
  if (sizeof (long) != 2)
    exit (0);

  if (bad (0, 0xdead) == 0xdead)
    abort ();
  exit (0);
}
