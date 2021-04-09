
#define FAILED 0
#define SUCCEEDED 1

#define OFF 0
#define ON 1

#define ARG_MODE_NONE 0
#define ARG_MODE_BRUTAL 1
#define ARG_MODE_ANALYZER 2

#define OP_SIZE_MAX 16

#ifdef GB
struct optcode {
  char *op;
  int  type;
  int  hex;
  int  branch;
};
#endif

struct label {
  char label[32];
  int address;
  struct label *next;
};

