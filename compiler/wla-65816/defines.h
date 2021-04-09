
#ifndef _DEFINES_H
#define _DEFINES_H

/* want to use longer strings and labels? change this */

#define MAX_NAME_LENGTH 64

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define FAILED    0
#define SUCCEEDED 1

#define OFF 0
#define ON  1

#define NO  0
#define YES 1

#define HINT_NONE  0
#define HINT_8BIT  1
#define HINT_16BIT 2
#define HINT_24BIT 3

#define STACK_CALCULATE_DELAY 2
#define STACK_RETURN_LABEL 1024

#define STACK_NONE    0
#define STACK_INSIDE  1
#define STACK_OUTSIDE 2

#define SECTION_STATUS_FREE      0
#define SECTION_STATUS_FORCE     1
#define SECTION_STATUS_OVERWRITE 2
#define SECTION_STATUS_HEADER    3
#define SECTION_STATUS_SEMIFREE  4
#define SECTION_STATUS_ABSOLUTE  5
#define SECTION_STATUS_RAM       6
#define SECTION_STATUS_SUPERFREE 7

#define INPUT_NUMBER_EOL           2
#define INPUT_NUMBER_ADDRESS_LABEL 3
#define INPUT_NUMBER_STRING        4
#define INPUT_NUMBER_STACK         5
#define INPUT_NUMBER_FLOAT         6

#define GET_NEXT_TOKEN_STRING         2
#define EVALUATE_TOKEN_NOT_IDENTIFIED 2
#define EVALUATE_TOKEN_EOP            6

#define OUTPUT_OBJECT  0
#define OUTPUT_LIBRARY 1
#define OUTPUT_NONE    2

#define REFERENCE_TYPE_DIRECT_16BIT   0
#define REFERENCE_TYPE_RELATIVE_8BIT  1
#define REFERENCE_TYPE_DIRECT_8BIT    2
#define REFERENCE_TYPE_DIRECT_24BIT   3
#define REFERENCE_TYPE_RELATIVE_16BIT 4

#define ERROR_NONE 0
#define ERROR_LOG  1
#define ERROR_DIR  2
#define ERROR_NUM  3
#define ERROR_INC  4
#define ERROR_INB  5
#define ERROR_UNF  6
#define ERROR_INP  7
#define ERROR_STC  8
#define ERROR_WRN  9
#define ERROR_ERR 10

/**************************************************************/
/* wla internal datatypes                                     */
/**************************************************************/

/* A - absolute section    */
/* c - stack (1 byte)      */
/* C - stack (2 bytes)     */
/* T - stack (3 bytes)     */
/* d - data (1 byte)       */
/* y - data (2 bytes)      */
/* z - data (3 bytes)      */
/* D - data block          */
/* O - origin              */
/* o - absolute origin     */
/* B - ROM bank            */
/* b - BASE (65816)        */
/* L - label               */
/* r - 16bit reference     */
/* R - 8bit pc ref         */
/* M - 16bit pc ref        */
/* Q - 8bit reference      */
/* q - 24bit reference     */
/* S - section             */
/* s - end of section      */
/* x - dsb                 */
/* X - dsw                 */
/* E - end of file         */
/* f - file name id        */
/* k - line number         */
/* P - add_old = add       */
/* p - add = add_old       */
/* g - block (start)       */
/* G - block (end)         */
/* Y - symbol              */
/* Z - breakpoint          */

/**************************************************************/
/* gb-z80                                                     */
/**************************************************************/

#ifdef GB

/* opcode types */

/* 0 - plain text  8b */
/* 1 - x              */
/* 2 - ?              */
/* 3 - plain text 16b */
/* 8 - *           8b */
/* 9 - *          16b */

#define OP_SIZE_MAX 16

#ifdef AMIGA
struct optcode {
  char *op;
  int  hex;
  short int type;
  short int value;
};
#else
struct optcode {
  char *op;
  int  hex;
  int  type;
  int  value;
};
#endif

#endif

/**************************************************************/
/* 6502                                                       */
/**************************************************************/

#ifdef MCS6502

/* opcode types */

/* 0 - plain text  8b */
/* 1 - x              */
/* 2 - ?              */
/* 3 - plain text 16b */
/* 4 - x (absolute)   */

#define OP_SIZE_MAX 12

#ifdef AMIGA
struct optcode {
  char *op;
  int  hex;
  short int type;
  short int skip_8bit;
};
#else
struct optcode {
  char *op;
  int  hex;
  int  type;
  int  skip_8bit;
};
#endif

#endif

/**************************************************************/
/* 65C02                                                      */
/**************************************************************/

#ifdef WDC65C02

/* opcode types */

/* 0 - plain text  8b */
/* 1 - x              */
/* 2 - ?              */
/* 3 - plain text 16b */
/* 4 - x (absolute)   */
/* 5 - x-abs x-rel    */

#define OP_SIZE_MAX 12

#ifdef AMIGA
struct optcode {
  char *op;
  int  hex;
  short int type;
  short int skip_8bit;
};
#else
struct optcode {
  char *op;
  int  hex;
  int  type;
  int  skip_8bit;
};
#endif

#endif

/**************************************************************/
/* huc6280                                                    */
/**************************************************************/

#ifdef HUC6280

/* opcode types */

/* 0 - plain text  8b */
/* 1 - x              */
/* 2 - ?              */
/* 3 - plain text 16b */
/* 4 - ? ? ?          */
/* 5 - x x            */
/* 6 - x ?            */
/* 7 - x (absolute)   */
/* 8 - x-abs x-rel    */

#define OP_SIZE_MAX 12

#ifdef AMIGA
struct optcode {
  char *op;
  int  hex;
  short int type;
  short int skip_8bit;
};
#else
struct optcode {
  char *op;
  int  hex;
  int  type;
  int  skip_8bit;
};
#endif

#endif

/**************************************************************/
/* 6510                                                       */
/**************************************************************/

#ifdef MCS6510

/* opcode types */

/* 0 - plain text  8b */
/* 1 - x              */
/* 2 - ?              */
/* 3 - plain text 16b */
/* 4 - x (absolute)   */

#define OP_SIZE_MAX 12

#ifdef AMIGA
struct optcode {
  char *op;
  int  hex;
  short int type;
  short int skip_8bit;
};
#else
struct optcode {
  char *op;
  int  hex;
  int  type;
  int  skip_8bit;
};
#endif

#endif

/**************************************************************/
/* spc700                                                     */
/**************************************************************/

#ifdef SPC700

/* opcode types */

/* 0 - plain text   */
/* 1 - x            */
/* 2 - ?            */
/* 3 - x ~ (odd)    */
/* 4 - x ~ (even)   */
/* 5 - ~ (direct)   */
/* a - x x          [1, 2] */
/* b - x x          [2, 1] */
/* c - x ~ x (odd)  [1, 2] */
/* d - x ~ x (even) [1, 2] */

#define OP_SIZE_MAX 12

#ifdef AMIGA
struct optcode {
  char *op;
  int  hex;
  short int type;
};
#else
struct optcode {
  char *op;
  int  hex;
  int  type;
};
#endif

#endif

/**************************************************************/
/* z80                                                        */
/**************************************************************/

#ifdef Z80

/* opcode types */

/* 0 - plain text  8b */
/* 1 - x              */
/* 2 - ?              */
/* 3 - plain text 16b */
/* 4 - x              */
/* 5 - x          24b */
/* 6 - ?          16b */
/* 7 - x x        16b */
/* 8 - *           8b */
/* 9 - *          16b */
/* a - * x        24b */

#define OP_SIZE_MAX 20

#ifdef AMIGA
struct optcode {
  char *op;
  int  hex;
  short int type;
  short int hex_x;
  short int value;
};
#else
struct optcode {
  char *op;
  int  hex;
  int  type;
  int  hex_x;
  int  value;
};
#endif

#endif

/**************************************************************/
/* 65816                                                      */
/**************************************************************/

#ifdef W65816

/* opcode types */

/* 0 - plain text  8b */
/* 1 - x              */
/* 2 - ?              */
/* 3 - &              */
/* 4 - x/? (mem/acc)  */
/* 5 - x x            */
/* 6 - REP/SEP        */
/* 7 - x/? (index)    */
/* 8 - plain text 16b */
/* 9 - relative ?     */
/* a - x (absolute)   */

#define OP_SIZE_MAX 16

#ifdef AMIGA
struct optcode {
  char *op;
  int  hex;
  short int type;
  short int skip_xbit;
};
#else
struct optcode {
  char *op;
  int  hex;
  int  type;
  int  skip_xbit;
};
#endif

#endif


#define DEFINITION_TYPE_VALUE  0
#define DEFINITION_TYPE_STRING 1
#define DEFINITION_TYPE_STACK  2

struct definition {
  char   alias[MAX_NAME_LENGTH];
  char   string[MAX_NAME_LENGTH];
  double value;
  int    type;
  int    size;
  struct definition *next;
};

struct macro_argument {
  int type;
  int value;
  int start;
  char string[MAX_NAME_LENGTH];
};

struct macro_static {
  char name[MAX_NAME_LENGTH];
  int  start;
  int  calls;
  int  filename_id;
  int  start_line;
  int  nargument_names;
  char **argument_names;
  struct macro_static *next;
};

struct macro_runtime {
  struct macro_static *macro;
  int macro_end;
  int macro_end_line;
  int macro_end_filename_id;
  int supplied_arguments;
  struct macro_argument **argument_data;
};

struct label_def {
  char label[MAX_NAME_LENGTH];
  unsigned char section_status;
  unsigned char alive;
  unsigned char type;
  unsigned char symbol;
  int  section_id;
  int  address; /* in bank */
  int  bank;
  int  slot;
#ifdef W65816
  int  base;
#endif
  int  filename_id;
  int  linenumber;
  struct label_def *next;
};

struct section_def {
  char name[MAX_NAME_LENGTH];
  int  alignment;
  int  address; /* in bank */
  int  bank;
  int  slot;
  int  size;
  int  status; /* see SECTION_STATUS_* */
  int  alive;
  int  id;
  int  i;
  int  filename_id;
  int  advance_org;
  int  maxsize_status; /* ON/OFF */
  int  maxsize;
  int  listfile_items;
  int  *listfile_ints;
  char *listfile_cmds;
  unsigned char *data;
  struct section_def *next;
};

struct incbin_file_data {
  struct incbin_file_data *next;
  char *data;
  char *name;
  int  size;
};

struct export_def {
  char   name[MAX_NAME_LENGTH];
  struct export_def *next;
};

struct active_file_info {
  int    filename_id;
  int    line_current;
  struct active_file_info *next;
  struct active_file_info *prev;
};

struct file_name_info {
  char   *name;
  int    id;
  struct file_name_info *next;
};

struct slot {
  int address;
  int size;
  struct slot *next;
};

struct block {
  char name[MAX_NAME_LENGTH];
  int  address;
  struct block *next;
};

#define STACK_ITEM_TYPE_VALUE    0
#define STACK_ITEM_TYPE_OPERATOR 1
#define STACK_ITEM_TYPE_STRING   2
#define STACK_ITEM_TYPE_DELETED  3
#define STACK_ITEM_TYPE_STACK    4

#define SI_OP_PLUS         0
#define SI_OP_MINUS        1
#define SI_OP_MULTIPLY     2
#define SI_OP_LEFT         3
#define SI_OP_RIGHT        4
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

#define STACKS_TYPE_8BIT    0
#define STACKS_TYPE_16BIT   1
#define STACKS_TYPE_24BIT   2
#define STACKS_TYPE_UNKNOWN 3

#define STACK_POSITION_DEFINITION 0
#define STACK_POSITION_CODE       1

struct stack_item {
  int type;
  int sign;
  double value;
  char string[MAX_NAME_LENGTH];
};

struct stack {
  struct stack_item *stack;
  struct stack *next;
  int id;
  int position;
  int filename_id;
  int stacksize;
  int linenumber;
  int type;
  int bank;
  int slot;
	int relative_references;
#ifdef W65816
  int base;
#endif
  int section_status;
  int section_id;
  int address;
};

struct structure_item {
  char name[MAX_NAME_LENGTH];
  int size;
  struct structure_item *next;
};

struct structure {
  char name[MAX_NAME_LENGTH];
  struct structure_item *items;
  int size;
  struct structure *next;
};

struct repeat_runtime {
  int start;
  int start_line;
  int counter;
};

struct filepointer {
  char name[MAX_NAME_LENGTH];
  char *filename;
  FILE *f;
  struct filepointer *next;
};

#define TYPE_STRING            0
#define TYPE_VALUE             1
#define TYPE_LABEL             2
#define TYPE_STACK_CALCULATION 3

#define SEC_TO_CHAR(s) s>>24,(s>>16)&0xff,(s>>8)&0xff,s&0xff

#endif

