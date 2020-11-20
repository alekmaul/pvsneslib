/*---------------------------------------------------------------------------------

	Copyright (C) 2012-2020
		Alekmaul


	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.

	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.

	3.	This notice may not be removed or altered from any source
		distribution.

---------------------------------------------------------------------------------*/
/*! \file scores.h
\brief Wrapper functions for score management

*/
#ifndef SNES_SCORE_INCLUDE
#define SNES_SCORE_INCLUDE

#include <snes/snestypes.h>

typedef struct { 
	u16 scolo; 
	u16 scohi; 
} scoMemory;

/*! \brief clear a score memory
	\param source the source to clear from
*/
void scoreClear(scoMemory *source);

/*! \brief add a value to a score memory
	\param source the source to clear from
	\param value the value to add
*/
void scoreAdd(scoMemory *source, u16 value);

/*! \brief copy a score memory to another score memory
	\param source the source to copy from
	\param dest the destination to copy to
*/
void scoreCpy(scoMemory *source, scoMemory *dest);

/*! \brief comapre a score memory to another score memory
	\param source the source to compare from
	\param dest the destination to compare to
	\return	0 if equals, 0xFF if source greater than destination, 1 otherwise
*/
u8 scoreCmp(scoMemory *source, scoMemory *dest);

#endif //SNES_SCORE_INCLUDE
