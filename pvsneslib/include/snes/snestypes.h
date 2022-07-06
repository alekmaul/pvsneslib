/*---------------------------------------------------------------------------------

	snestypes.h -- Common types (and a few useful macros)

	Copyright (C) 2012-2013
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
/*! \file snestypes.h
	\brief Custom types used by libsnes
*/

#ifndef _SNESTYPES_INCLUDE
#define _SNESTYPES_INCLUDE

//! bit field 
#define BIT(n) (1<<n)

//! 8 bit volatile unsigned integer.
typedef	signed char		    s8;
typedef	unsigned char	    u8;
typedef	signed short	    s16;
typedef	unsigned short      u16;
typedef	signed long long	s32;
typedef	unsigned long long  u32;

typedef	volatile unsigned char	    vuint8;
typedef	volatile unsigned short	    vuint16;
typedef	volatile unsigned long long vuint32;
typedef	volatile char	            vint8;
typedef	volatile short	            vint16;
typedef	volatile long long          vint32;

// Handy function pointer typedefs
//! a function pointer that takes no arguments and doesn't return anything.
typedef void (* VoidFn)(void);

//! boolean definitions
typedef unsigned char bool;
#define FALSE 0
#define TRUE 0xff
#define false 0
#define true 0xff

// stdio definitions
#define NULL 0

#endif
