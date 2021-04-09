
#define OBJ_FORMAT_INFO  0
#define OBJ_EMPTY_FILL   4
#define OBJ_MISC_BITS    5
#define OBJ_MORE_BITS    6
#define OBJ_ROMBANKS     7
#define OBJ_ROMBANKMAP  11

#define LIB_FORMAT_INFO       0
#define LIB_SOURCE_FILE_NAMES 4

#define OUTPUT_ROM 0
#define OUTPUT_PRG 1

#define SNES_ROM_MODE_LOROM 0
#define SNES_ROM_MODE_HIROM 2

#define SNES_ROM_SPEED_SLOWROM 0
#define SNES_ROM_SPEED_FASTROM 8

#define WLA_VERSION_OBJ 0
#define WLA_VERSION_LIB 1

#define DATA_TYPE_BLOCK   0
#define DATA_TYPE_SECTION 1

#define FAILED    1
#define SUCCEEDED 0

#define TRUE  1
#define FALSE 0

#define STATE_NONE       0
#define STATE_OBJECT     1
#define STATE_LIBRARY    2
#define STATE_HEADER     3
#define STATE_FOOTER     4
#define STATE_DEFINITION 5

#define SYMBOL_MODE_NONE   0
#define SYMBOL_MODE_NOCA5H 1
#define SYMBOL_MODE_WLA    2

#define OFF 0
#define ON  1

#define NO  0
#define YES 1

#define MAX_NAME_LENGTH 63

struct source_file_name {
  char *name;
  int  id;
  struct source_file_name *next;
};

struct object_file {
  unsigned char *data;
  unsigned char *memorymap;
  unsigned char *source_file_names;
  unsigned char *exported_defines;
  unsigned char *data_blocks;
  char *name;
  int rom_banks;
  int format;
  int bank;
  int slot;
  int size;
  int base;
  int base_defined;
  int id;
  struct source_file_name *source_file_names_list;
  struct object_file *next;
};

#define LABEL_STATUS_LABEL      0
#define LABEL_STATUS_DEFINE     1
#define LABEL_STATUS_STACK      2
#define LABEL_STATUS_SYMBOL     3
#define LABEL_STATUS_BREAKPOINT 4

struct label {
  char name[MAX_NAME_LENGTH + 1];
  int file_id;
  int file_id_source;
  int linenumber;
  int rom_address;
  int section;
  int section_status;
  int bank;
  int base;
  int slot;
  int status;
  double address;
  struct label *next;
  struct label *prev;
};

#define REFERENCE_TYPE_DIRECT_16BIT   0
#define REFERENCE_TYPE_RELATIVE_8BIT  1
#define REFERENCE_TYPE_DIRECT_8BIT    2
#define REFERENCE_TYPE_DIRECT_24BIT   3
#define REFERENCE_TYPE_RELATIVE_16BIT 4

struct reference {
  char name[MAX_NAME_LENGTH + 1];
  int  address;
  int  type;
  int  section;
  int  section_status;
  int  bank;
  int  slot;
  int  file_id;
  int  file_id_source;
  int  linenumber;
  struct reference *next;
  struct reference *prev;
};

#define SECTION_STATUS_FREE      0
#define SECTION_STATUS_FORCE     1
#define SECTION_STATUS_OVERWRITE 2
#define SECTION_STATUS_HEADER    3
#define SECTION_STATUS_SEMIFREE  4
#define SECTION_STATUS_ABSOLUTE  5
#define SECTION_STATUS_RAM       6
#define SECTION_STATUS_SUPERFREE 7

struct section {
  char name[MAX_NAME_LENGTH + 1];
  int  file_id;
  int  file_id_source;
  int  address;
  int  output_address;
  int  status;
  int  bank;
  int  slot;
  int  size;
  int  base;
  int  base_defined;
  int  library_status;
  int  id;
  int  i;
  int  referenced;
  int  alive;
  int  alignment;
  int  listfile_items;
  int  *listfile_ints;
  char *listfile_cmds;
  unsigned char *data;
  struct section *next;
  struct section *prev;
};

struct slot {
  int address;
  int size;
  int usage;
};

#define STACK_ITEM_TYPE_VALUE    0
#define STACK_ITEM_TYPE_OPERATOR 1
#define STACK_ITEM_TYPE_STRING   2
#define STACK_ITEM_TYPE_STACK    4

#define SI_OP_PLUS         0
#define SI_OP_MINUS        1
#define SI_OP_MULTIPLY     2
#define SI_OP_OR           5
#define SI_OP_AND          6
#define SI_OP_DIVIDE       7
#define SI_OP_POWER        8
#define SI_OP_SHIFT_LEFT   9
#define SI_OP_SHIFT_RIGHT 10
#define SI_OP_MODULO      11
#define SI_OP_XOR         12
#define SI_OP_LOW_BYTE    13
#define SI_OP_HIGH_BYTE   14

#define SI_SIGN_POSITIVE 0
#define SI_SIGN_NEGATIVE 1

#define STACKS_TYPE_8BIT  0
#define STACKS_TYPE_16BIT 1
#define STACKS_TYPE_24BIT 2

#define STACK_POSITION_DEFINITION 0
#define STACK_POSITION_CODE       1

struct stackitem {
  char string[MAX_NAME_LENGTH + 1];
  int type;
  int sign;
  double value;
};

struct stack {
  struct stackitem *stack;
  struct stack *next;
  struct stack *prev;
  int id;
	int relative_references;
  int under_work;
  int computed;
  int result;
  int position;
  int file_id;
  int file_id_source;
  int stacksize;
  int linenumber;
  int type;
  int bank;
  int slot;
  int section_status;
  int section;
  int address;
  int memory_address;
};
