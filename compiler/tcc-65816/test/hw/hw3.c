#include "hw3.h"

#if 1
static void gibdir(void);
void testfields(void);
void testlonglong(void);
int testfloat(void);
void testcasts(void);
void testptrconv(void);
void testcontrol(int num);
void testellipsis(void);

char* string = "hello, world!\n";	// pointer goes to RAM, string constant to ROM
char* string2 = "good-bye\n";		// dto.
char string3[5] = "1234";		// RAM

const int a[50] = {1,2,3,4,5};	// RAM
int b[50] = {6,7,8,9,10};	// RAM

#ifndef __APPLE__
const int __attribute__((section(".rotzdata"))) c[50] = {65,66,67,14,15};	// ROM (extra section)
const int __attribute__((section(".rodata"))) e[10] = {99,98,97,96};		// ROM (together with string constants)
#endif


int d[50];	// BSS
int* f = d;
int g = 3;

#ifndef __APPLE__
const char __attribute__((section(".rotzdata"))) x[] = "fritze";	// ROM (extra section)
#endif
const char const* leer1 = "";	// pointer in RAM points to empty string in ROM
const char const leer2[] = "";	// empty string in RAM

#ifndef __65816__
#define OUTC(x) (putchar(x))
#define OUTLIT(x) (putchar(x))
#else
volatile char* out = (char*)0xfffe;
#define OUTC(x) (*out = (x))
#define OUTLIT(x) (*((char*)0xfffe) = (x))
#endif

int zahl(void) {
  return a[2];
}

unsigned short testus(unsigned short n)
{
  return n & 0xf000;
}

short tests(short n)
{
  return n & 0xf000;
}

int test(int n)
{
  return n & 0xf000;
}

void write(char* s) {
//  while(*s) OUTC(*s++);
  while(*s) OUTLIT(*s++);
}

void hexu(unsigned int n) {
  unsigned int nn;
  unsigned int lead = 0;
  unsigned int count = 4;
  write("0x");
  while(count--) {
    nn = (n & 0xf000) >> 12;
    if(nn + lead) {
      if(nn >= 10) OUTC(nn - 10 + 'a');
      else OUTC(nn + '0');
      lead = 1;
    }
    n <<= 4;
  }
  if(!lead) OUTC('0');
}

void hexun(unsigned int n)
{
  hexu(n); OUTC('\n');
}

void simplehex(int n) {
  unsigned int nn;
  nn = n;
  if(nn >= 10) write("ok\n");
  else write("nok\n");
}

void hex(int n) {
  unsigned int nn;
  unsigned int lead = 0;
  unsigned int un;
  int count = 4;
  if(n < 0) {
    //write("n is smaller than zero\n");
    OUTC('-');
    n = -n;
  }
  write("0x");
  un = n;
  //write("un "); hexu(un);
  while(count--) {
    nn = (un & 0xf000) >> 12;
    //write("digit "); hexu(nn);
    if(nn + lead) {
      if(nn >= 10) OUTC(nn - 10 + 'a');
      else OUTC(nn + '0');
      lead = 1;
    }
    un <<= 4;
  }
  if(!lead) OUTC('0');
}

#define hexint hex
#define hex__uint hexu
#define hexchar hex
#define hex__uchar hexu
#define hex__longlong hexl
#define hex__ulonglong hexlu

void hexlu(unsigned long long n) {
  unsigned int nn;
  unsigned int lead = 0;
  unsigned long long un;
  int count = 8;

  write("0x");
  //write("un "); hexu(un);
  while(count--) {
    nn = (n & 0xf0000000ULL) >> 28;
    //write("digit "); hexu(nn);
    if(nn + lead) {
      if(nn >= 10) OUTC(nn - 10 + 'a');
      else OUTC(nn + '0');
      lead = 1;
    }
    n <<= 4;
  }
  if(!lead) OUTC('0');
}

void hexl(long long n) {
  unsigned int nn;
  unsigned int lead = 0;
  unsigned long long un = n;
  int count = 8;

  write("0x");
  //write("un "); hexu(un);
  while(count--) {
    nn = (un & 0xf0000000ULL) >> 28;
    //write("digit "); hexu(nn);
    if(nn + lead) {
      if(nn >= 10) OUTC(nn - 10 + 'a');
      else OUTC(nn + '0');
      lead = 1;
    }
    un <<= 4;
  }
  if(!lead) OUTC('0');
}

void hexus(int n)
{
  hex(n); write(" ("); hexu(n); OUTC(')');
}

void hexn(int n)
{
  hex(n); OUTC('\n');
}

void writeequation(int num, char* op, int num2)
{
  hexu(num); OUTC(' '); write(op); OUTC(' '); hexu(num2); write(" == ");
}

void testasr(int num, int shift)
{
  writeequation(num, ">>", shift);
  //hexu(num); write(" >> "); hexu(shift); write(" == ");
  num >>= shift;
  hexu(num); OUTC('\n');
}

void testlsr(unsigned int num, int shift)
{
  writeequation(num, "u>>", shift);
  //hexu(num); write(" >> "); hexu(shift); write(" == ");
  num >>= shift;
  hexu(num); OUTC('\n');
}

void testsub(int num, int sub)
{
  writeequation(num, "-", sub);
  //hex(num); write(" - "); hex(sub); write(" == ");
  num -= sub;
  hex(num); OUTC('\n');
}

void testadd(int num, int add)
{
  writeequation(num, "+", add);
  num += add;
  hex(num); OUTC('\n');
}

static void testmul(int a, int b)
{
  writeequation(a, "*", b);
  a *= b;
  hex(a); OUTC('\n');
}

void testdiv(int a, int b)
{
  writeequation(a, "/", b);
  a /= b;
  hex(a); OUTC('\n');
}

void testdivu(unsigned int a, unsigned int b)
{
  writeequation(a, "u/", b);
  a /= b;
  hexu(a); OUTC('\n');
}

void testmod(int a, int b)
{
  writeequation(a, "%", b);
  a %= b;
  hex(a); OUTC('\n');
}

#define gen_testop(type) void testop_##type(type a, char* op, type b, type c) \
{ \
  write(#type " "); writeequation(a, op, b); \
  hexus(c); OUTC('\n'); \
}

gen_testop(int)
gen_testop(__uint)
gen_testop(char)
gen_testop(__uchar)
gen_testop(__longlong)
gen_testop(__ulonglong)
gen_testop(float)

#define gen_testunaryop(type) void testunaryop_##type(type a, char* op, type b) \
{ \
  write(#type " "); write(op); hex(a); OUTC(' '); write(op); OUTC(' '); hex(b); OUTC('\n'); \
}

gen_testunaryop(int)
gen_testunaryop(__uint)
gen_testunaryop(char)
gen_testunaryop(__uchar)
gen_testunaryop(__longlong)
gen_testunaryop(__ulonglong)
gen_testunaryop(float)

#define gen_test(op, sop, type) void test##op##type(type a, type b) \
{ \
  testop_##type(a, #sop, b, a sop b); \
}

#define gen_testnotzero(op, sop, type) void test##op##type(type a, type b) \
{ \
  if(b) testop_##type(a, #sop, b, a sop b); \
}

#define gen_testunary(op, sop, type) void test##op##type(type a) \
{ \
  testunaryop_##type(a, #sop, sop a); \
}

#define gen_testconst(op, sop, cst, type) void testconst##op##type(type a) \
{ \
  testunaryop_##type(a, #op #cst, a sop cst); \
}

#if 1
#define gen_testall(op, sop) \
  gen_test(op, sop, int) \
  gen_test(op, sop, __uint) \
  gen_test(op, sop, char) \
  gen_test(op, sop, __uchar) \
  gen_test(op, sop, __longlong) \
  gen_test(op, sop, __ulonglong)
  
#define gen_testallnotzero(op, sop) \
  gen_testnotzero(op, sop, int) \
  gen_testnotzero(op, sop, __uint) \
  gen_testnotzero(op, sop, char) \
  gen_testnotzero(op, sop, __uchar) \
  gen_testnotzero(op, sop, __longlong) \
  gen_testnotzero(op, sop, __ulonglong)
  
#else
#define gen_testall(op, sop)
#endif

#define gen_testreallyall(op, sop) \
  gen_testall(op, sop) \
  gen_test(op, sop, float)

#define gen_testreallyallnotzero(op, sop) \
  gen_testallnotzero(op, sop) \
  gen_testnotzero(op, sop, float)

#define gen_testallunary(op, sop) \
  gen_testunary(op, sop, int) \
  gen_testunary(op, sop, __uint) \
  gen_testunary(op, sop, char) \
  gen_testunary(op, sop, __uchar) \
  gen_testunary(op, sop, __longlong) \
  gen_testunary(op, sop, __ulonglong)

#define gen_testallconst(op, sop, cst) \
  gen_testconst(op, sop, cst, int) \
  gen_testconst(op, sop, cst, __uint) \
  gen_testconst(op, sop, cst, char) \
  gen_testconst(op, sop, cst, __uchar) \
  gen_testconst(op, sop, cst, __longlong) \
  gen_testconst(op, sop, cst, __ulonglong)

#define gen_testreallyallconst(op, sop, cst) \
  gen_testallconst(op, sop, cst) \
  gen_testconst(op, sop, cst, float)

#if 1
#define run_test(op, a, b) \
  test##op ##int(a, b); \
  test##op ##__uint(a, b); \
  test##op ##char(a, b); \
  test##op ##__uchar(a, b); \
  test##op ##__longlong(a, b); \
  test##op ##__ulonglong(a, b);
#else
#define run_test(op, a, b)
#endif

#define run_testreallyall(op, a, b) \
  run_test(op, a, b) \
  test##op ##float(a, b);

#define run_testunary(op, a) \
  test##op ##int(a); \
  test##op ##__uint(a); \
  test##op ##char(a); \
  test##op ##__uchar(a); \
  test##op ##__longlong(a); \
  test##op ##__ulonglong(a);

#define run_testconst(op, a) \
  testconst##op ##int(a); \
  testconst##op ##__uint(a); \
  testconst##op ##char(a); \
  testconst##op ##__uchar(a); \
  testconst##op ##__longlong(a); \
  testconst##op ##__ulonglong(a);

#define run_testreallyallconst(op, a) \
  run_testconst(op, a) \
  testconst##op ##float(a);

gen_testreallyall(add, +)
gen_testreallyall(sub, -)
gen_testreallyall(mul, *)
gen_testreallyallnotzero(div, /)
gen_testreallyall(lt, <)
gen_testreallyall(gt, >)
gen_testreallyall(le, <=)
gen_testreallyall(eq, ==)
gen_testreallyall(ne, !=)
gen_testreallyall(ge, >=)

gen_testall(land, &&)
gen_testall(lor, ||)
gen_testall(xor, ^)
gen_testall(and, &)
gen_testall(or, |)
gen_testall(shl, <<)
gen_testall(shr, >>)
gen_testallnotzero(mod, %)

gen_testreallyallconst(add, +, 42)
gen_testreallyallconst(sub, -, 42)
gen_testreallyallconst(mul, *, 42)
gen_testreallyallconst(div, /, 42)
gen_testreallyallconst(lt, <, 42)
gen_testreallyallconst(gt, >, 42)
gen_testreallyallconst(le, <=, 42)
gen_testreallyallconst(eq, ==, 42)
gen_testreallyallconst(ne, !=, 42)
gen_testreallyallconst(ge, >=, 42)

gen_testallconst(land, &&, 1)
gen_testallconst(lor, ||, 1)
gen_testallconst(xor, ^, 42)
gen_testallconst(and, &, 42)
gen_testallconst(or, |, 42)
gen_testallconst(shl, <<, 5)
gen_testallconst(shr, >>, 5)
gen_testallconst(mod, %, 23)

gen_testallunary(minus, -)
gen_testallunary(not, ~)
gen_testallunary(lnot, !)

#define gen_testcast(from, to, outfn) to testcast##from##to(from v) \
{ \
  write("cast from " #from " "); outfn(v); write(" to " #to " "); \
  outfn((to)v); OUTC('\n'); \
}

void dumplong(__longlong);

#define gen_testallcasts(from) \
  gen_testcast(from, char, hex) \
  gen_testcast(from, __uchar, hexu) \
  gen_testcast(from, __int, hex) \
  gen_testcast(from, __uint, hexu) \
  gen_testcast(from, float, hex) \
  gen_testcast(from, __longlong, dumplong) \
  gen_testcast(from, __ulonglong, dumplong)

gen_testallcasts(char)
gen_testallcasts(__uchar)
gen_testallcasts(__int)
gen_testallcasts(__uint)
gen_testallcasts(__longlong)
gen_testallcasts(__ulonglong)
gen_testallcasts(float)

#define run_cast(from, v) \
  testcast##from ##char(v); \
  testcast##from ##__uchar(v); \
  testcast##from ##__int(v); \
  testcast##from ##__uint(v); \
  testcast##from ##float(v); \
  testcast##from ##__longlong(v); \
  testcast##from ##__ulonglong(v);

typedef void (*testfuncptr)(int, int);
typedef void (*testufuncptr)(int);

const testfuncptr tf[] = {testmul, testdiv, testmod}; //, testxorint};
const testufuncptr tfu[] = {testnotint, testlnotint, testminusint};

#ifdef __65816__
int asmfptest(void);
#endif

int nums1[] = {0, -1, 1, 2, -2, 4, -4, 10, -10, 5, 7, 42, 23, 32767, -32768, -1000, -10000};
// nums2 is local to main()

float float1[] = { 1.0, -1.0, 0.0, 2.5, 1000000.0f };
float float2[] = { 1.0, -1.0, 2.5, 23.0f, -42.0f };

__longlong ll1[] = { -1, 0, 1, 10, 100, 1000000 };
__longlong ll2[] = { -1, 1, 23, 42, 99, -1000000};
__ulonglong* ull1 = ll1;

char char1[] = {-1, 0, 1, 10, 23, 42, 127};
char char2[] = {-1, 1, -10, -23, -42, -128};
__uchar* uchar1 = char1;

extern void printsizes(void);

int main(void)
{
  int i,j,k;
  d[5] = -10;
#ifdef __65816__
  // turn the SNES bg green
  volatile unsigned char* vb = (unsigned char*)0x2100;
  volatile unsigned char* col = (unsigned char*)0x2122;
  *vb = 0x80;
  *col = 0xe0;
  *col = 0;
  *vb = 0xf;
  hexun(asmfptest());
#endif
  hexn(testfloat());
  gibdir();

#if 0
  __longlong blubb = 5;
  __longlong bla = 3;
  hexn(blubb * bla);
  exit(0);
#endif

#if 1
  simplehex(5);
  simplehex(15);
  write(string);
  string += zahl();
  write(string);
  string += -3;
  string -= -1;
  write(string);
  write("signed numbers\n");
  hex(-10);
  hexn(-10);
  hexn(0x5);
  hexn(10);
  write("unsigned numbers\n");
  hexun(-10);
  hexun(5);
  hexun(10);
  write("arith shift right\n");
  testasr(-1, 2);
  testasr(0xff00, 4);
  testasr(0xf0, 2);
  testasr(0x8000, 1);
  testasr(0x8001, 1);
  testasr(0x7fff, 1);
  testlsr(-1, 2);
  testlsr(0xff00, 4);
  testlsr(0xf0, 2);
  testlsr(0x8000, 1);
  testlsr(0x8001, 1);
  testlsr(0x7fff, 1);
  testsub(10, 5);
  testsub(10,10);
  testsub(5, 10);
  testsub(0, 10);
  testsub(-10, 0);
  testsub(10,0);
  testsub(-10,-5);
  testsub(-10,5);
  testsub(10,-10);
  testadd(5,10);
  testadd(-5,10);
  testadd(0,5);
  testadd(0,-5);
  testadd(-5,0);
  testadd(5,0);
  testadd(5,-10);
  testmul(2,4);
  testmul(-2,4);
  testmul(0,2);
  testmul(0,-2);
  testmul(2,-4);
  testmul(2,0);
  testmul(-2,0);
  testmul(256,256);
  testdivu(42,5);
  testdivu(5,42);
  testdivu(-42,5);
  testdivu(42,-5);
  testdivu(-5,42);
  testdivu(40,8);
  testdivu(0,8);
  testdiv(42,5);
  testdiv(5,42);
  testdiv(-42,5);
  testdiv(42,-5);
  testdiv(-5,42);
  testdiv(40,8);
  testdiv(0,8);
  testdiv(-42,-5);
  printsizes();
  write("finished\n");
  write(leer1);
  write(leer2);
  write("leer finished\n");
#endif

  int nums2[] = {-1, 1, 2, -2, 4, -4, 10, -10, 5, 7, 42, 23, 32767, -32768, 1000, 10000};
  // shifts are undefined for negative values and values > type size, so we use separate test values
  int shifts2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 16, 18, 20, 22};
#if 1
  for(j = 0; j < (sizeof(nums1)/sizeof(int)) ; j++) {
    for(k = 0; k < (sizeof(nums2)/sizeof(int)) ; k++) {
      for(i = 0; i < (sizeof(tf)/sizeof(testfuncptr)); i++)
        tf[i](nums1[j],nums2[k]);
      run_test(xor, nums1[j], nums2[k])
      run_test(land, nums1[j], nums2[k])
      run_test(lor, nums1[j], nums2[k])
      run_test(and, nums1[j], nums2[k])
      run_test(or, nums1[j], nums2[k])
      run_test(shl, nums1[j], shifts2[k])
      run_test(shr, nums1[j], shifts2[k])
      run_testreallyall(add, nums1[j], nums2[k])
      run_testreallyall(sub, nums1[j], nums2[k])
      run_testreallyall(mul, nums1[j], nums2[k])
      run_testreallyall(lt, nums1[j], nums2[k])
      run_testreallyall(gt, nums1[j], nums2[k])
      run_testreallyall(le, nums1[j], nums2[k])
      run_testreallyall(eq, nums1[j], nums2[k])
      run_testreallyall(ne, nums1[j], nums2[k])
      run_testreallyall(ge, nums1[j], nums2[k])
      run_testreallyall(div, nums1[j], nums2[k])	/* has one problem when float dividing 32767/-32768
                                                           resulting in -1 instead of zero */
      run_test(mod, nums1[j], nums2[k])
      //testltfloat(nums1[j], nums2[k]);
    }
    run_testconst(xor, nums1[j])
    run_testconst(land, nums1[j])
    run_testconst(lor, nums1[j])
    run_testconst(and, nums1[j])
    run_testconst(or, nums1[j])
    run_testconst(shl, nums1[j])
    run_testconst(shr, nums1[j])
    run_testreallyallconst(add, nums1[j])
    run_testreallyallconst(sub, nums1[j])
    run_testreallyallconst(mul, nums1[j])
    run_testreallyallconst(lt, nums1[j])
    run_testreallyallconst(gt, nums1[j])
    run_testreallyallconst(le, nums1[j])
    run_testreallyallconst(eq, nums1[j])
    run_testreallyallconst(ne, nums1[j])
    run_testreallyallconst(ge, nums1[j])
    run_testreallyallconst(div, nums1[j])
    run_testconst(mod, nums1[j])

  }
#endif

  testcasts();

#if 1
  for(i = 0; i < (sizeof(tfu)/sizeof(testufuncptr)); i++)
  {
    for(j = 0; j < (sizeof(nums1)/sizeof(int)) ; j++) {
      tfu[i](nums1[j]);
      write("xchar "); tfu[i]((char)nums1[j]);
      run_testunary(not, nums1[j])
      run_testunary(lnot, nums1[j])
      run_testunary(minus, nums1[j])
    }
  }

  for(i = 0; i < 5; i++) {
    hex(a[5-i]); OUTC(',');
  }
  OUTC('\n');


  int logic1[] = {0, 1, -1, 42, 23, 32767, -32768};
  for(i = 0; i < (sizeof(logic1)/sizeof(int)); i++) {
    for(j = 0; j < (sizeof(logic1)/sizeof(int)); j++) {
      run_test(lor, logic1[i], logic1[j]);
      run_test(land, logic1[i], logic1[j])
    }
  }
#endif

  gibdir();
  testfields();
  testlonglong();
  testptrconv();
  testellipsis();
  
  for(i = 0; i < 25; i++) testcontrol(i);
  

  // conversion of certain boundary values
  float a = 0xffffU;
  hexn(a);	/* this one results in various values, e.g. 0x7fff, -0x8000, -0x1, depending on
                   GCC options (-m32/64, -msse..., -ffast-math) */
  hexun(a);

  return d[5] + 10;
}

#if 1
struct bla {
  int a;
  int b;
  int c;
  int d;
};

struct bla sx64;

// get struct by value argument
int nimmmich(struct bla x, int off)
{
  return x.c + off;
}

// return struct by value
struct bla holmich(void)
{
  sx64.c = 0xbabe;
  return sx64;
}

// get struct by value, modify and pass it as argument
static void gibdir(void)
{
  struct bla xy = holmich();
  struct bla yz = holmich();
  // overwrite c to check that we actually pass a copy
  xy.c = 0xbeef;
  // write to members before and after to catch wrong offsets
  yz.b = 1;
  yz.d = 2;
  hexun(nimmmich(xy,1));
  hexun(nimmmich(yz,2));
}

#endif

// test bitfields
struct fields {
  int a :1;
  int b :1;
  int c :2;
  int d :4;
  int e :3;
  int f :1;
  int g :12;
  int x;
};

struct fields fd;

// get/modify struct by reference
void modifyfields(struct fields* f, int x)
{
  f->x = 42;
  f->a = 1;
  f->b = 0;
  f->c = 2;
  f->d = x;	// cast to bitfield
  f->e = 0;
  f->f = 1;
  f->g = 23;
}

// return bitfield struct
struct fields getfield(void)
{
  modifyfields(&fd, 2);
  return fd;
}

void testfields(void)
{
  struct fields f = getfield();	// get
  hexn(f.x);
  hexn(f.d);
  hexn(f.g);
  modifyfields(&f, 3);
  hexn(f.x);
  hexn(f.d);
  hexn(f.g);
  hexn(fd.d);
}

__longlong longschlong(void)
{
  __longlong a = 5;
  if(a == 42) return 23;	/* triggers VT_JMPI problems */
  a += 3; //zahl();
  return a;
}

void dumplong(__longlong x)
{
  //hexu(x >> 16); hexun(x);
  hexlu(x); write(" ("); hexl(x); write(")\n");
}

void testlonglong(void)
{
  volatile __longlong x;
  x = longschlong();
  hexn(x);
  x += 32767;
  dumplong(x);
  x -= 5;
  dumplong(x);
  x >>= 3;
  dumplong(x);
  x <<= 3;
  dumplong(x);
  x <<= 3;

  // see if LLONG parameters get passed correctly
  hexu(x >> 16); hexun(x);
  dumplong(x);

  x = -x;
  dumplong(x);
}

float getfloat(void)
{
  return 5;
}

#if 1
int takefloat(float x)
{
  return (int)x;
}
#endif

void testextreme(float x)
{
  write("extreme "); hexun(x);
}

int ex1 = -1;
int ex2 = -32768;
int nex = 42;

int testfloat(void)
{
  float a;
  a = (float)0x112; //5;
  a += getfloat();
  a += g;

  hexun(nex); testextreme(nex);
  hexun(ex1); testextreme(ex1);
  hexun(ex2); testextreme(ex2);
  return takefloat(a);
}
#endif

void xy(int bla)
{
}

void yz(char x)
{
  xy(x);
}

void zz(int y)
{
  yz(y);
}

void testcasts(void)
{
  int i;
#if 1
  for(i = 0; i < (sizeof(nums1)/sizeof(int)); i++) {
    run_cast(char, nums1[i]);
    run_cast(__uchar, nums1[i]);
    run_cast(__int, nums1[i]);
    run_cast(__uint, nums1[i]);
    run_cast(float, nums1[i]);
    run_cast(__longlong, nums1[i]);
    run_cast(__ulonglong, nums1[i]);
  }
#endif

  for(i = 0; i < (sizeof(float1)/sizeof(float)); i++) {
    write("(int)float1["); hex(i); write("] == "); hexn(float1[i]);
  }
#if 1
  for(i = 0; i < (sizeof(float1)/sizeof(float)); i++) {
    run_cast(char, float1[i]);
    run_cast(__uchar, float1[i]);
    run_cast(__int, float1[i]);
    run_cast(__uint, float1[i]);
    run_cast(float, float1[i]);
    run_cast(__longlong, float1[i]);
    run_cast(__ulonglong, float1[i]);
  }
#endif

#if 1
  for(i = 0; i < (sizeof(ll1)/sizeof(__longlong)); i++) {
    write("casting ll1["); hex(i); write("]\n");
#if 1
    run_cast(char, ll1[i]);
    run_cast(__uchar, ll1[i]);
    run_cast(__int, ll1[i]);
    run_cast(__uint, ll1[i]);
    run_cast(float, ll1[i]);
    run_cast(__longlong, ll1[i]);
    run_cast(__ulonglong, ll1[i]);
#endif
    run_cast(char, ull1[i]);
    run_cast(__uchar, ull1[i]);
    run_cast(__int, ull1[i]);
    run_cast(__uint, ull1[i]);
    run_cast(float, ull1[i]);
    run_cast(__longlong, ull1[i]);
    run_cast(__ulonglong, ull1[i]);
  }
#endif
  
#if 1
  for(i = 0; i < (sizeof(char1)/sizeof(char)); i++) {
    run_cast(char, char1[i]);
    run_cast(__uchar, char1[i]);
    run_cast(__int, char1[i]);
    run_cast(__uint, char1[i]);
    run_cast(float, char1[i]);
    run_cast(__longlong, char1[i]);
    run_cast(__ulonglong, char1[i]);
    run_cast(char, uchar1[i]);
    run_cast(__uchar, uchar1[i]);
    run_cast(__int, uchar1[i]);
    run_cast(__uint, uchar1[i]);
    run_cast(float, uchar1[i]);
    run_cast(__longlong, uchar1[i]);
    run_cast(__ulonglong, uchar1[i]);
  }
#endif
}

int* hans = b;
int* bx = (int*)5;

void testptrconv(void)
{
  int* fritz;
  int hannes;
  
  fritz = hans;
  write("fritzhans "); hexn(*fritz);
  
#if 0 // TCC does not allow conversion between pointers and long longs
  __longlong herbert;
  __ulonglong markus;
  herbert = (__longlong)fritz;
  write("herberthans "); hexn(*((int*)herbert));
  
  markus = (__ulonglong)fritz;
  write("markushans "); hexn(*((int*)markus));
#endif
  
  hannes = (int)fritz;

#if 0	// arithmetics are 16-bit only, so this is true on 65816
  if((int*)hannes == hans) write("int saves\n");
  else write("int does not save\n");
#endif

  if(hannes == (int)hans) write("hans2int OK\n");
  else write("hans2int funky\n");
  
  int ax = 3;
  bx += 1;
  if((int)ax == (int)bx) write("huhu\n");
  else write("bubu\n");
}

void testcontrol(int num)
{
#if 1
  switch(num) {
    case 0: write("null"); break;
    case 1: write("eins"); break;
    case 2: write("zwei"); break;
    case 3: write("drei, nicht "); // falltrhu
    case 4: write("vier"); break;
    default: write("wos moanst?"); break;
  }
  OUTC('\n');
#endif
  
#if 1
  if(num <= 10) write("<= 10\n");
  else if(num > 11) write("> 11\n");
  else write("== 11\n");
  
  int i;
  for(i = num; i; --i) hex(i);
  OUTC('\n');
#endif

  i = num;
  do {
    hex(i);
  } while(i--);
  OUTC('\n');
  
  i = num;
label:
  hex(i);
  if(!i--) {
    OUTC('\n');
    return;
  }
  goto label;
}

#include <stdarg.h>

// certain wasteful compilers promote any type to int or double
#ifdef __65816__
#define vachar char
#define vaint int
#define vafloat float
#else
#define vachar int32_t
#define vaint int32_t
#define vafloat double
#endif

void ellprint(char* format, ...)
{
  va_list vl;
  va_start(vl, format);
  while(*format) {
    switch(*format) {
      case 'c': hex(va_arg(vl, vachar)); break;
      case 'i': hex(va_arg(vl, vaint)); break;
      case 'l': hexl(va_arg(vl, __longlong)); break;
      case 'f': write("float "); hex(va_arg(vl, vafloat)); break;
      default: break;
    }
    format++;
  }
  va_end(vl);
}

void testellipsis(void)
{
  ellprint("cilf", char1[3], nums1[3], ll1[3], float1[3]);
}
