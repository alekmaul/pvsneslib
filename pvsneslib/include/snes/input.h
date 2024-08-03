/*---------------------------------------------------------------------------------

    Input registers

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

/*!
 * \file input.h
 * \brief input support.
 *
 * The inputs are automatically read by the \ref VBlank-ISR on non-lag frames.
 */

#ifndef SNES_INPUT_INCLUDE
#define SNES_INPUT_INCLUDE

#include <snes/snestypes.h>
#include <snes/interrupt.h>

/*!
    \brief common values for pad input.

    common values that can be used to test auto pad.
*/
//! enum values for the pad buttons.
typedef enum KEYPAD_BITS
{
    KEY_A = BIT(7),       //!< pad A button.
    KEY_B = BIT(15),      //!< pad B button.
    KEY_SELECT = BIT(13), //!< pad SELECT button.
    KEY_START = BIT(12),  //!< pad START button.
    KEY_RIGHT = BIT(8),   //!< pad RIGHT button.
    KEY_LEFT = BIT(9),    //!< pad LEFT button.
    KEY_DOWN = BIT(10),   //!< pad DOWN button.
    KEY_UP = BIT(11),     //!< pad UP button.
    KEY_R = BIT(4),       //!< Right shoulder button.
    KEY_L = BIT(5),       //!< Left shoulder button.
    KEY_X = BIT(6),       //!< pad X button.
    KEY_Y = BIT(14),      //!< pad Y button.
} KEYPAD_BITS;

/*!
    \brief common values for SuperScope input.
*/
//! enum values for the SuperScope buttons and flags.
typedef enum SUPERSCOPE_BITS
{
    SSC_FIRE = BIT(15),     //!< superscope FIRE button.
    SSC_CURSOR = BIT(14),   //!< superscope CURSOR button.
    SSC_PAUSE = BIT(12),    //!< superscope PAUSE button.
    SSC_TURBO = BIT(13),    //!< superscope TURBO flag.
    SSC_OFFSCREEN = BIT(9), //!< superscope OFFSCREEN flag.
    SSC_NOISE = BIT(8),     //!< superscope NOISE flag.
} SUPERSCOPE_BITS;

extern u16 pad_keys[5];     //!< current pad value
extern u16 pad_keysold[5];  //!< previous pad value
extern u16 pad_keysdown[5]; //!< newly pressed down pad keys

extern u8 snes_mplay5;  /*!< \brief 1 if MultiPlay5 is connected */
extern u8 snes_mouse;   /*!< \brief 1 if Mouse is going to be used */
extern u8 snes_sscope;  /*!< \brief 1 if SuperScope is connected */

extern u8 mouseConnect[2];        /*!< \brief 1 if Mouse present */
extern u8 mouseButton[2];         /*!< \brief 1 if button is pressed, stays for a bit and then it gets released (Click mode). */
extern u8 mousePressed[2];        /*!< \brief 1 if button is pressed, stays until is unpressed (Turbo mode). */
extern u8 mouse_x[2];             /*!< \brief Mouse X acceleration. daaaaaaa, d = direction (0: up/left, 1: down/right), a = acceleration. */
extern u8 mouse_y[2];             /*!< \brief Mouse Y acceleration. daaaaaaa, d = direction (0: up/left, 1: down/right), a = acceleration. */

/*!
 * \brief Mouse sensitivity
 *
 *  * When a mouse is connected to the port: sensitivity bits read from the mouse.
 *  * When no mouse is connected: The sensitivity to set the mouse to when the mouse is connected to the console.
 *
 * CAUTION: The Hyperkin clone mouse ignores sensitivity changes and always reports a sensitivity of 0.
 */
extern u8 mouseSensitivity[2];

/*!
 * \brief Request a change mouse sensitivity.
 *
 * To prevent auto-joypad read corruption the change sensitivity commands are delayed until the
 * next non-lag VBlank ISR (after the mouse data has been read).
 *
 * Values:
 *  * `0x00`: No changes to mouse sensitivity.
 *  * `0x01`: Cycle the mouse sensitivity once.
 *  * `0x02-0x7f`: Cycle the mouse sensitivity twice.
 *  * `0x80-0xff`: Set the sensitivity to `value & 3`.
 *
 * CAUTION: The Hyperkin clone mouse ignores sensitivity changes and always reports a sensitivity of 0.
 */
extern u8 mouseRequestChangeSensitivity[2];

#define mouse_L 0x01 /*!< \brief SNES Mouse Left button mask.*/
#define mouse_R 0x02 /*!< \brief SNES Mouse Right button mask.*/

extern u16 scope_holddelay; /*!< \brief Hold delay. */
extern u16 scope_repdelay;  /*!< \brief Repeat rate. */
extern u16 scope_shothraw;  /*!< \brief Horizontal shot position, not adjusted. */
extern u16 scope_shotvraw;  /*!< \brief Vertical shot position, not adjusted. */
extern u16 scope_shoth;     /*!< \brief Horizontal shot position, adjusted for aim. */
extern u16 scope_shotv;     /*!< \brief Vertical shot position, adjusted for aim. */
extern u16 scope_centerh;   /*!< \brief 0x0000 is the center of the screen, positive values go to bottom right. */
extern u16 scope_centerv;   /*!< \brief 0x0000 is the center of the screen, positive values go to bottom right. */
extern u16 scope_down;      /*!< \brief flags that are currently true.*/
extern u16 scope_now;       /*!< \brief flags that have become true this frame.*/
extern u16 scope_held;      /*!< \brief flagsthat have been true for a certain length of time.*/
extern u16 scope_last;      /*!< \brief flags that were true on the previous frame.*/
extern u16 scope_sinceshot; /*!< \brief Number of frames elapsed since last shot was fired.*/

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
#define REG_JOYxLH(a) (((vuint16 *)0x4218)[(a)])

/*!	\fn  padsCurrent(value)
    \brief Return current value of selected pad
    \param value pad index to use (0-1 or 0-4 if multiplayer 5 connected)
    \return unsigned short of the current pad value
*/
// unsigned short padsCurrent(u16 value);
#define padsCurrent(value) (pad_keys[value])

/*!	\fn padsDown(value)
    \brief Return value of down keys for selected pad
    \param value pad index to use (0-1 or 0-4 if multiplayer 5 connected)
    \return unsigned short of the newly pressed down keys (0 -> 1 transition)
*/
// unsigned short padsDown(u16 value);
#define padsDown(value) (pad_keysdown[value])

/*!	\fn padsUp(u16 value)
    \brief Return value of up keys for selected pad
    \param value pad index to use (0-1 or 0-4 if multiplayer 5 connected)
    \return unsigned short of the released keys (1 -> 0 transition)
*/
unsigned short padsUp(u16 value);

/*!	\fn padsClear(u16 value)
    \brief Clear internal variables for selected pad
    \param value pad index to use (0-1 or 0-4 if multiplayer 5 connected)
*/
void padsClear(u16 value);

/*!	\fn detectMPlay5(void)
    \brief Check if MultiPlayer5 is connected and populate snes_mplay5 (0 or 1 for connected)

    \b CAUTION: REG_WRIO ($4201) must not be written to while MultiPlayer5 is active.
    (Bit 7 of REG_WRIO must be set when Auto Joy reads the controllers, shortly after the VBlank Period starts.)
*/
void detectMPlay5(void);

/*!	\fn detectMouse(void)
    \brief Check if Mouse is connected and populate snes_mouse (0 or 1 for connected)
*/
void detectMouse(void);

/*!	\fn detectSuperScope(void)
    \brief Detects if SuperScope is connected on Port 1 (second controller port on console) and populate snes_sscope (0 or 1 for connected)
*/
void detectSuperScope(void);


/*!
 * \brief Enable mouse reading and set the initial mouse sensitivity
 *
 * Initialises mouse variables and enable mouse reading in the VBlank ISR.
 *
 * \param sensitivity sensitivity to use when  the mouse is connected to the console
 * (has no effect on a Hyperkin clone mouse)
 */
void initMouse(u8 sensitivity);


/*!
 * \brief Queue a cycle mouse sensitivity command for the next VBlank.
 *
 * \param port the port the mouse is connected to (0 or 1).
 *
 * CAUTION:
 *  * The changes to @ref mouseSensitivity are delayed one frame.
 *  * This function will override any pending @ref mouseRequestChangeSensitivity commands.
 *  * This function has no effect on the Hyperkin clone mouse.
 */
void mouseCycleSensitivity(u16 port);

/*!
 * \brief Queue a cycle mouse sensitivity twice (decrementing the sensitivity) command for the next VBlank.
 *
 * \param port the port the mouse is connected to (0 or 1).
 *
 * CAUTION:
 *  * The changes to @ref mouseSensitivity are delayed one frame.
 *  * This function will override any pending @ref mouseRequestChangeSensitivity commands.
 *  * This function has no effect on the Hyperkin clone mouse.
 */
void mouseCycleSensitivityTwice(u16 port);

/*!
 * \brief Queue a set mouse sensitivity command (to be executed on the next VBlank).
 *
 * \param port the port the mouse is connected to (0 or 1).
 * \param sensitivity the sensitivity to set the mouse to (0 - 2).
 *
 * CAUTION:
 *  * The changes to @ref mouseSensitivity are delayed one frame.
 *  * This function will override any pending @ref mouseRequestChangeSensitivity commands.
 *  * This function has no effect on the Hyperkin clone mouse.
 */
void mouseSetSensitivity(u16 port, u16 sensitivity);

#endif // SNES_PADS_INCLUDE
