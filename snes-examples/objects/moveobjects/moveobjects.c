/*---------------------------------------------------------------------------------


     snes object engine demo
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char snesfont;

extern char gfxpsrite, gfxpsrite_end;
extern char palsprite, palsprite_end;

u16 numspr, i;
u16 pad0;
s16 xp, yp;

t_objs *snesobj;
s16 *snesox, *snesoy; // basics x/y on screen with fixed point

// Definition of object table with values :
// x & y coordinates
// type of objects
// min x & max x coordinate if we want to update objects on regarding these coordinates
const u16 tabobjects[] =
    {
        15,
        5,
        0,
        0,
        0, // no min & max x speified
        100,
        5,
        0,
        0,
        0, // no min & max x speified
        5,
        15,
        0,
        0,
        0, // no min & max x speified
        150,
        150,
        0,
        0,
        0, // no min & max x speified
        200,
        50,
        0,
        0,
        0, // no min & max x speified
        125,
        25,
        0,
        0,
        0, // no min & max x speified
        200,
        25,
        0,
        0,
        0, // no min & max x speified

        0xFFFF,
};

//---------------------------------------------------------------------------------
// Init function for each object from type #0
void testInit(u16 xp, u16 yp, u16 type, u16 minx, u16 maxx)
{
    // to have a little message regarding init (DO NOT USE FOR REAL SNES GAME, JUST DEBUGGING PURPOSE)
    consoleNocashMessage("testInit %d %d\n", (u16)xp, (u16)yp);

    // prepare new object
    if (objNew(type, xp, yp) == 0)
        // no more space, get out
        return;

    // Init some vars for snes sprite (objgetid is the current object id)
    objGetPointer(objgetid);
    snesobj = &objbuffers[objptr - 1];

    // Put sprite at corrdinates, with maximum priority 3 from tile entry 0, palette 0
    snesobj->sprnum = numspr * 4;
    oamSet(snesobj->sprnum, xp, yp, 3, 0, 0, 0, 0);
    oamSetEx(snesobj->sprnum, OBJ_SMALL, OBJ_SHOW);
    numspr++;
}

// Update function for each object from type #0
void testUpdate(u8 idx)
{
	// go to current object
    snesobj = &objbuffers[idx];

    // grab the coodinate pointers
    snesox = (s16 *)&(snesobj->xpos + 1);
    snesoy = (s16 *)&(snesobj->ypos + 1);
    xp = *snesox;
    yp = *snesoy;

    // Get current #0 pad
    pad0 = padsCurrent(0);

    // Kill current object with pad if A pressed (just need to populate objtokill variable)
    switch (pad0)
    {
    case KEY_A:
        objtokill = 1;
        consoleNocashMessage("killing me %04x\n", (u16)idx);
        break;
    }

    // change sprite coordinates randomly
    i = rand() & 0xF;
    if (i < 7)
    {
        xp++;
        if (xp > 255)
            xp--;
    }
    else
    {
        xp--;
        if (xp < 1)
            xp++;
    }
    i = rand() & 0xF;
    if (i < 7)
    {
        yp++;
        if (yp > 223)
            yp--;
    }
    else
    {
        yp--;
        if (yp < 1)
            yp++;
    }

    // change sprite display
    oamSet(snesobj->sprnum, xp, yp, 3, 0, 0, 0, 0);

    // update variables for the object
    *snesox = xp;
    *snesoy = yp;
}

//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize SNES
    consoleInit();

    // Init Sprites gfx and palette with default size of 32x32
    oamInitGfxSet(&gfxpsrite, (&gfxpsrite_end - &gfxpsrite), &palsprite, (&palsprite_end - &palsprite), 0, 0x0000, OBJ_SIZE32_L64);

    // Now Put in 16 color mode
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Screen activated
    setScreenOn();

    // Object engine activate
    objInitEngine();

    // Init function for state machine
    objInitFunctions(0, &testInit, &testUpdate, NULL);

    // Load all objects into memory
    numspr = 0;
    objLoadObjects((char *)&tabobjects);

	// Need to init map , even if not present to allow update functions to work
	x_pos=0; y_pos=0;
	
    while (1)
    {
        objUpdateAll();
        WaitForVBlank();
    }
    return 0;
}