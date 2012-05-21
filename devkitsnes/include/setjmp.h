typedef int jmp_buf[3];

int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int val);
