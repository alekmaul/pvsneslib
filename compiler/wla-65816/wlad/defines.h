
#define FAILED 0
#define SUCCEEDED 1

#define OFF 0
#define ON 1

#define OP_SIZE_MAX 16



struct optcode {
  char op[OP_SIZE_MAX];
  int  hex;
  int  type;
};

