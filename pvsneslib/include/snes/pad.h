/*---------------------------------------------------------------------------------

	Pads registers

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

/*! \file pad.h
    \brief pad support.
*/

#ifndef SNES_PADS_INCLUDE
#define SNES_PADS_INCLUDE

#include <snes/snestypes.h>
#include <snes/interrupts.h>

/*! \file
	\brief common values for pad input.

	common values that can be used to test auto pad.
*/
//! enum values for the pad buttons.
typedef enum KEYPAD_BITS {
  KEY_A      = BIT(7),  //!< pad A button.
  KEY_B      = BIT(15),  //!< pad B button.
  KEY_SELECT = BIT(13),  //!< pad SELECT button.
  KEY_START  = BIT(12),  //!< pad START button.
  KEY_RIGHT  = BIT(8),  //!< pad RIGHT button.
  KEY_LEFT   = BIT(9),  //!< pad LEFT button.
  KEY_DOWN   = BIT(10),  //!< pad DOWN button.
  KEY_UP     = BIT(11),  //!< pad UP button.
  KEY_R      = BIT(4),  //!< Right shoulder button.
  KEY_L      = BIT(5),  //!< Left shoulder button.
  KEY_X      = BIT(6), //!< pad X button.
  KEY_Y      = BIT(14), //!< pad Y button.
} KEYPAD_BITS;

extern u16 pad_keys[2];
extern u16 pad_keysold[2];
extern u16 pad_keysrepeat[2];

/*! \def REG_JOYxLH

    \brief SNES Controllers I/O Ports - Automatic Reading.
	4218h/4219h - JOY1L/JOY1H - Joypad 1 (gameport 1, pin 4) (R)
	421Ah/421Bh - JOY2L/JOY2H - Joypad 2 (gameport 2, pin 4) (R)
	421Ch/421Dh - JOY3L/JOY3H - Joypad 3 (gameport 1, pin 5) (R)
	421Eh/421Fh - JOY4L/JOY4H - Joypad 4 (gameport 2, pin 5) (R)

	Register    Serial     Default
	Bit         Transfer   Purpose
	Number______Order______(Joypads)_____
	15          1st        B Button         (1=Low=Pressed)
	14          2nd        Y Button
	13          3rd        Select Button
	12          4th        Start Button
	11          5th        Up
	10          6th        Down
	9           7th        Left
	8           8th        Right
	7           9th        A Button
	6           10th       X Button
	5           11th       L Button
	4           12th       R Button
	3           13th       0 (High)
	2           14th       0 (High)
	1           15th       0 (High)
	0           16th       0 (High)

	When enabled, the SNES will read 16 bits from each of the 4 controller port
	data lines into registers $4218-f. This begins between H=32.5 and H=95.5 of
	the first V-Blank scanline, and ends 4224 master cycles later. Register $4212
	bit 0 is set during this time. Specifically, it begins at H=74.5 on the first
	frame, and thereafter some multiple of 256 cycles after the start of the
	previous read that falls within the observed range.

	Reading $4218-f during this time will read back incorrect values. The only
	reliable value is that no buttons pressed will return 0 (however, if buttons
	are pressed 0 could still be returned incorrectly). Presumably reading $4016/7
	or writing $4016 during this time will also screw things up.

*/
#define REG_JOYxLH(a)	(((vuint16*)0x4218)[(a)])

/*!	\fn scanPads()
	\brief Wait for pad ready and read pad values in . 
*/
void scanPads(void);

/*!	\fn  padsCurrent(value)
	\brief Return current value of selected pad
	\param value Address of the pad to use (0 or 1)
	\return unsigned short of the current pad value
*/
//unsigned short padsCurrent(u16 value);
#define padsCurrent(value) (pad_keys[value])

/*!	\fn padsDown(u16 value)
	\brief Return value of down keys for selected pad
	\param value Address of the pad to use (0 or 1)
	\return unsigned short of the current pad value
*/
unsigned short padsDown(u16 value);

/*!	\fn padsUp(u16 value)
	\brief Return value of up keys for selected pad
	\param value Address of the pad to use (0 or 1)
	\return unsigned short of the current pad value
*/
unsigned short padsUp(u16 value);


/*!	\fn padsClear(u16 value)
	\brief Clear internal variables for selected pad
	\param value Address of the pad to use (0 or 1)
*/
void padsClear(u16 value);

/*!	\fn scanScope()
	\brief Wait for superscope ready and read values in. 
*/
void scanScope(void);

#endif //SNES_PADS_INCLUDE
