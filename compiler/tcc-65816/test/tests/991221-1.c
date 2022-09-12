int main( void )
{
   unsigned long totalsize = 80;
   unsigned long msize = 64;

   if (sizeof(long) != 4)
     exit(0);

   if ( totalsize > (32767L   * 2UL + 1)
        || (msize != 0 && ((msize - 1) > (32767L   * 2UL + 1) )))
      abort();
   exit( 0 );
}
