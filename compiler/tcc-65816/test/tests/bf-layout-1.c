struct { long f8:4; long f24:12; } a;
struct { long f32:16; } b;

main ()
{
  if (sizeof (a) != sizeof (b))
    abort ();
  exit (0);
}
