extern void some_func(char* str, int x);
extern int x;

int b = 0xbb;
int c = 0xcc;
int d = 0xdd;

char* bums = "bums";
char* fallera = "fallera";
char* chachacha = "chachacha";

void other_func(int y)
{
}

int main(int argc, char** argv)
{
  int a;
  a = 0;
  a *= 5;
  a *= x;
  some_func("hello_world", a);
  other_func(a);
}
