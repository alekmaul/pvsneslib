main ()
{
  int i = 0;


  if (sizeof (unsigned long int) == 2)
    {
      unsigned long int z = 0;

      do {
	z -= 0x0040;
	i++;
	if (i > 0x0400)
	  abort ();
      } while (z > 0);
      exit (0);
    }
  else if (sizeof (unsigned int) == 2)
    {
      unsigned int z = 0;

      do {
	z -= 0x0040;
	i++;
	if (i > 0x0400)
	  abort ();
      } while (z > 0);
      exit (0);
    }
  else
    exit (0);
}
