void main(void)
{
  volatile unsigned char* vb = (unsigned char*)0x2100;
  volatile unsigned char* col = (unsigned char*)0x2122;
  *vb = 0x80;
  *col = 0xe0;
  *col = 0;
  *vb = 0xf;
}