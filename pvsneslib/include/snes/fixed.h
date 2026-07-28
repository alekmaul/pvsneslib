/*---------------------------------------------------------------------------------

    snestypes.h -- fixed points (and a few useful macros)

    Copyright (C) 2025
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

    fixed point definition inspired by SGDK from Stephane Dallongeville
        https://github.com/Stephane-D/SGDK/blob/master/inc/maths.h
---------------------------------------------------------------------------------*/
#include <snes/snestypes.h>

/**
 *  \typedef fx16
 *      16 bits Fixed point (10.6) type - short version
 */
typedef s16 fx16;

#define FX16_INT_BITS              10
#define FX16_FRAC_BITS             (16 - FX16_INT_BITS)
#define FX16_INT_MASK              (((1 << FX16_INT_BITS) - 1) << FX16_FRAC_BITS)
#define FX16_FRAC_MASK             ((1 << FX16_FRAC_BITS) - 1)

/**
 *  \brief
 *      Convert specified value to fx16
 *
 *  Ex: fx16 v = FX16(-27.12);
 */
#define FX16(value)                ((FX16) ((value) * (1 << FX16_FRAC_BITS)))

/**
 *  \brief
 *      Get integer part of a fx16
 */
#define ifromfx16(value)           ((value) >> FX16_FRAC_BITS)

/**
 *  \brief
 *      Get decimal part of a fx16
 */
#define dfromfx16(value)           ((value) << FX16_FRAC_BITS)

/**
 *  \brief
 *      Multiply 2 fx16 values
 */
#define fx16mul(val1, val2)   ((val1 * val2) >> FX16_FRAC_BITS)

/**
 *  \brief
 *      Devide 2 fx16 values
 */
#define fx16div(val1, val2)   (((val1 << FX16_FRAC_BITS) / val2);


