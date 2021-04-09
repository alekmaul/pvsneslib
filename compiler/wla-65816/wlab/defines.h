
#define FAILED 0
#define SUCCEEDED 1

#define OFF 0
#define ON 1

#define MOD_NONE 0
#define MOD_DEC 1
#define MOD_HEX 2
#define MOD_BIN 3

#define OP_SIZE_MAX 16



struct optcode {
  char op[OP_SIZE_MAX];
  int  hex;
  int  type;
};

