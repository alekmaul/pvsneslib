/*---------------------------------------------------------------------------------

    object.h -- definitions for SNES objects

    Copyright (C) 2012-2021
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
/*! \file object.h
    \brief snes object engine functionality.
    \brief 64 maximum of objects managed by engine
    \brief 32 type of objects maximum by engine
*/

#ifndef SNES_OBJECTS_INCLUDE
#define SNES_OBJECTS_INCLUDE

#include <snes/snestypes.h>

#define OB_MAX 64 // total number of objects in the game

typedef struct
{
    u16 prev; //  0 previous object linked list
    u16 next; //  2 next object in linked list
    u8 type;  //  4 type of object, depend of the game
    u8 nID;   //  5 obj ID in linked list

    u16 sprnum;     //  6 sprite oam number of the object (multiple of 4)
    u16 sprid3216;  //  8 sprite oam id in sprite oam memory
    u16 sprblk3216; // 10 sprite oam address in sprites tiles memory
    u8 sprflip;     // 12 sprite flip attribute
    u8 sprpal;      // 13 sprite palette
    u16 sprframe;   // 14 current frame

    u8 xpos[3]; // 16 x position on the screen (3 fixed point)
    u8 ypos[3]; // 19 y position on the screen (3 fixed point)

    u16 xofs;   // 22 x offset of the object (from the square 16x16 or 32x32)
    u16 yofs;   // 24 y offset of the object (from the square 16x16 or 32x32)
    u16 width;  // 26 width of the object (from the square 16x16 or 32x32)
    u16 height; // 28 height of the object (from the square 16x16 or 32x32)
    u16 xmin;   // 30 min x coordinate for action of object, depends of game (ex: revert direction)
    u16 xmax;   // 32 max x coordinate for action of object, depends of game (ex: revert direction)

    u16 xvel; // 34 x velocity
    u16 yvel; // 36 y velocity

    u16 tilestand; // 38 tile number object is standing on
    u16 tileabove; // 40 tile number above object
    u16 tilesprop; // 42 tile property stand on
    u16 tilebprop; // 44 tile property on left/right side

    u16 action; // 46 current action of the object, depends of game
    u8 status;  // 48 status of object regarding collision
    u8 tempo;   // 49 if object needs tempo
    u8 count;   // 50 if object needs a counter
    u8 dir;     // 51 if object needs to manage direction

    u16 parentID;  // 52 obj ID of parent (useful for projectiles)
    u8 hitpoints;  // 54 number of hit points of object
    u8 sprrefresh; // 55 if object needs sprite to be refresh
    u8 onscreen;         // 56 to know if object is on screen or not

    u8 objnotused[7];                 // OB_SIZE-55-1 for future use
} t_objs __attribute__((__packed__)); // seems to do nothing :/

extern u16 objptr;   /*!< \brief pointer to current object */
extern u8 objtokill; /*!< \brief put 1 in variable to kill current object */
extern u16 objgetid; /*!< \brief id of current object (useful when creating it) */

extern t_objs objbuffers[OB_MAX]; /*!< \brief current object buffer with all objects */

#define T_EMPTY 0x0000 /*!< \brief Type Empty (action will be fall) for tile  */
#define T_SOLID 0xFF00 /*!< \brief Type Solid (action is walk only) for tile  */
#define T_LADDE 0x0001 /*!< \brief Type Ladder (action will be climb) for tile  */
#define T_FIRES 0x0002 /*!< \brief Type Fire (action will be burn) for tile  */
#define T_SPIKE 0x0004 /*!< \brief Type Spyke (action will be die) for tile  */
#define T_PLATE 0x0008 /*!< \brief Type Ground (action will be jump through and land) for tile  */

#define ACT_STAND 0x0000 /*!< \brief Action type STAND for object */
#define ACT_WALK 0x0001  /*!< \brief Action type WALK for object */
#define ACT_JUMP 0x0002  /*!< \brief Action type JUMP for object */
#define ACT_FALL 0x0004  /*!< \brief Action type FALL for object */
#define ACT_CLIMB 0x0008 /*!< \brief Action type CLIMB for object */
#define ACT_DIE 0x0010   /*!< \brief Action type DIE for object */
#define ACT_BURN 0x0020  /*!< \brief Action type BURN for object */

/*! \fn objInitEngine(void)
    \brief Initialize object engine, need to be called once
*/
void objInitEngine(void);

/*! \fn objInitGravity(u8 objgravity, u8 objfriction, u16 objvelocity)
    \brief Initialize the object type functions (initialize, update)
    \param objgravity  The gravity of object when falling (default is 41)
    \param objfriction 	Friction for object when walking (default is 16)
*/
void objInitGravity(u16 objgravity, u16 objfriction);

/*! \fn objInitFunctions(u8 objtype, void *initfct,void *updfct)
    \brief Initialize the object type functions (initialize, update)
    \param objtype  The type of object depending of the game
	\param initfct  The address of the function when we init the type of object
	\param updfct   The address of the function when we update the type of object (need to be called once per frame)
	\param reffct   The address of the function when we refresh sprites of object (need to be called once per frame)
*/
void objInitFunctions(u8 objtype, void *initfct, void *updfct, void *reffct);

/*! \fn objLoadObjects(u8 *sourceO)
    \brief Load all objects for a specific table in memory
    \brief Call, after loading, each init function of the type of objects for each object
    \brief The table has an entry with x,y,type,minx,maxx for each object
    \brief where:
    \brief x,y are coordinates of object,
    \brief type if the type of the object (maximum 32 types)
    \brief minx,maxx are the coordinates of minimum & maxinmum possible on x
    \brief the last four parameters are useful to do some actions where minimum or maximum is reached
    \brief The table needs to finish with FFFF to indicate that no more objects are availables
    \param sourceO table of objects
*/
void objLoadObjects(u8 *sourceO);

/*! \fn objNew(u8 objtype, u16 x, u16 y)
    \brief Initialize a new object in game, objgetid will has the id of the object
    \param objtype  The type of object depending of the game
    \param x 	    The X coordinate of object on map or screen
    \param y 	    The Y coordinate of object on map or screen
    \return id of the object in object id table
*/
u16 objNew(u8 objtype, u16 x, u16 y);

/*! \fn objGetPointer(u16 objhandle)
    \brief get the pointer to an object from its handle (need to do -1 to have offset after), objptr will has the pointer of the object
    \param objhandle handle of the object
*/
void objGetPointer(u16 objhandle);

/*! \fn objKill(u16 objhandle)
    \brief kill an object from the game
    \param objhandle handle of the object
*/
void objKill(u16 objhandle);

/*! \fn objKillAll(void)
    \brief kill all objects currently active
*/
void objKillAll(void);

/*! \fn objUpdateAll(void)
    \brief call update function for all objects currently active (if they are in "virtual screen" coordinates).
    \brief  "virtual screen" coordinates are -64<x<320 and -64<y<288.
*/
void objUpdateAll(void);

/*! \fn objRefreshAll(void)
    \brief call refresh function for all objects currently on screen.
    \brief  useful to avoid flickering when using a platform scrolling game.
    \brief  screen coordinates are -32<x<256 and -32<y<y<224.
*/
void objRefreshAll(void);

/*! \fn objCollidMap(u16 objhandle)
    \brief check if an object collides the map
    \brief update tilestand, tileabove with tiles on map regarding object attributes
    \brief update xvel, yvel with velocity regarding tiles on map
    \param objhandle handle of the object
*/
void objCollidMap(u16 objhandle);

/*! \fn objCollidMap1D(u16 objhandle)
    \brief check if an object collides the map
    \brief update tilestand, tileabove with tiles on map regarding object attributes
    \brief update xvel, yvel with no gravity regarding tiles on map
    \param objhandle handle of the object
*/
void objCollidMap1D(u16 objhandle);

/*! \fn objCollidObj(u16 objhandle1,u16 objhandle2)
    \brief check if two objects are colliding
    \param objhandle1 handle of the 1st object
    \param objhandle2 handle of the 2nd object
    \return 1 if collision detected, 0 elsewhere
*/
u16 objCollidObj(u16 objhandle1, u16 objhandle2);

/*! \fn objUpdateXY(u16 objhandle)
    \brief Update X & Y coordinates of object regarding its own velocitys
    \brief It uses xvel and yvel to do such computation
    \param objhandle handle of the object
*/
void objUpdateXY(u16 objhandle);

#endif // SNES_OBJECTS_INCLUDE
