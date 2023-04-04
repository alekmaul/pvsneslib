#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "helpers.h"

/*!
 * @brief Define comment for ASM files
 */
#define ASM_COMMENT ";"

/*!
 * @brief BSS start of a section block
 */
#define BSS_SECTION_START ".RAMSECTION \".bss\" BANK $7e SLOT 2"

/*!
 * @brief End of a section block
 */
#define SECTION_END ".ENDS"

/*!
 * @brief Stores (accu/x/y/zero) to pseudo-registers
 */
#define STORE_AXYZ_TO_PSEUDO "st([axyz]).b tcc__([rf][0-9]{0,}h{0,1})$"
/*!
 * @brief Stores (x/y) to pseudo-registers
 */
#define STORE_XY_TO_PSEUDO "st([xy]).b tcc__([rf][0-9]{0,}h{0,1})$"
/*!
 * @brief Stores (accu only) to pseudo-registers
 */
#define STORE_A_TO_PSEUDO "sta.b tcc__([rf][0-9]{0,}h{0,1})$"

int verbosity();
dynArray tidyFile(const int argc, char **argv);
dynArray storeBss(dynArray file);
dynArray optimizeAsm(dynArray file, dynArray bss, size_t verbose);

#endif