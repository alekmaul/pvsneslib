/*---------------------------------------------------------------------------------


    Mode1ContinuosScroll

    example of how to scroll for more than 2 screens of 32x32
    (the tile map pages are loaded during vblank interruption function)
    -- odelot

 Sprite from Calciumtrice, usable under Creative Commons Attribution 3.0 license., http://opengameart.org
 Backgrounds inspired in SoR2
---------------------------------------------------------------------------------*/
#include <snes.h>

extern char BG1_tiles, BG1_tiles_end;
extern char BG1_pal, BG1_pal_end;
extern char BG1_map, BG1_map_end;

extern char BG2_tiles, BG2_tiles_end;
extern char BG2_pal, BG1_pal_end;
extern char BG2_map, BG1_map_end;

extern char BG3_tiles, BG3_tiles_end;
extern char BG3_pal, BG3_pal_end;
extern char BG3_map, BG3_map_end;

extern char character_tiles, character_tiles_end;
extern char character_pal, character_pal_end;

typedef struct
{
    u8 oamAddress;
    int _x;
    int _y;
} character;

// struct to handle scroll (scrolling through pages)
typedef struct
{
    u8 id;
    u16 vram;
    u16 scrX;
    u16 scrY;
    u8 *bgMap;
    u8 mapPage;
    bool vRamFirstPage;
    u16 scrollX;
    u16 maxScrollX;
} scroll;

// struct to control the background pages
typedef struct
{
    u8 *gfxoffset;
    u16 adrgfxvram;
    u16 size;

} bgpage;

// struct to control when update the backgrounds
typedef struct
{
    bgpage bg1;
    bgpage bg2;
    u8 refreshBG1;
    u8 refreshBG2;
} background;

// scroll control for main and sub background
scroll bgMain;
scroll bgSub;

// take care of player position
character player1;

// keep the joypad commands
unsigned short pad0;

// handle a mutex to avoid refresh background while we are changing it
unsigned char bg_mutex;

// control where the background needs to be updated
background bgInfo;

// set background 1 to be updated
void updateBG1(u8 *pgfx, u16 adrspr, int size)
{
    bg_mutex = 1; // to avoid vbl during queue management
    bgInfo.bg1.adrgfxvram = adrspr;
    bgInfo.bg1.gfxoffset = pgfx;
    bgInfo.bg1.size = size;
    bgInfo.refreshBG1 = true;
    bg_mutex = 0; // to avoid vbl during queue management
}

// set background 2 to be updated
void updateBG2(u8 *pgfx, u16 adrspr, int size)
{
    bg_mutex = 1; // to avoid vbl during queue management
    bgInfo.bg2.adrgfxvram = adrspr;
    bgInfo.bg2.gfxoffset = pgfx;
    bgInfo.bg2.size = size;
    bgInfo.refreshBG2 = true;
    bg_mutex = 0; // to avoid vbl during queue management
}

void updatePos(character *p, unsigned short pad)
{

    if (pad & KEY_UP)
    {
        p->_y -= 2;
    }
    if (pad & KEY_DOWN)
    {
        p->_y += 2;
    }
    if (pad & KEY_LEFT)
    {
        p->_x -= 2;
    }
    if (pad & KEY_RIGHT)
    {
        p->_x += 2;
    }
    oamSetXY(p->oamAddress, p->_x, p->_y);
}

// handle scroll of main background (BG1)
void handleScroll(character *p, scroll *s)
{

    if (p->_x > 94)
    {
        if (s->scrX < s->maxScrollX)
        {
            s->scrX += 1;
            p->_x -= 1;
        }

        if ((s->scrX >= (s->mapPage + 1) * 256))
        {
            s->mapPage += 1;
            s->vRamFirstPage = !s->vRamFirstPage;
            u16 vram = s->vram;
            if (s->vRamFirstPage == false)
            {
                vram += 1024;
            }
            updateBG1(s->bgMap + 2048 * s->mapPage, vram, 2048);
        }
        bgSetScroll(s->id, s->scrX - 255, s->scrY - 223);
    }
}

// handle scroll of sub background (BG2)
void handleScrollSub(character *p, scroll *s)
{

    if (p->_x > 94)
    {
        if (s->scrX < s->maxScrollX)
        {

            // scroll faster - parallax effect
            s->scrX += 2;
        }

        if ((s->scrX >= (s->mapPage + 1) * 256))
        {
            s->mapPage += 1;
            s->vRamFirstPage = !s->vRamFirstPage;
            u16 vram = s->vram;
            if (s->vRamFirstPage == false)
            {
                vram += 1024;
            }
            updateBG2(s->bgMap + 2048 * s->mapPage, vram, 2048);
        }
        bgSetScroll(s->id, s->scrX - 255, s->scrY - 223);
    }
}

// interruption of vblank (send information to vram via DMA)
void myconsoleVblank()
{

    dmaCopyOAram((unsigned char *)&oamMemory, 0, 0x220);

    if (bg_mutex == 0)
    {
        if (bgInfo.refreshBG1 == true)
        {
            dmaCopyVram(bgInfo.bg1.gfxoffset, bgInfo.bg1.adrgfxvram, bgInfo.bg1.size);
            bgInfo.refreshBG1 = false;
        }
    }
    if (bg_mutex == 0)
    {
        if (bgInfo.refreshBG2 == true)
        {
            dmaCopyVram(bgInfo.bg2.gfxoffset, bgInfo.bg2.adrgfxvram, bgInfo.bg2.size);
            bgInfo.refreshBG2 = false;
        }
    }
}

//---------------------------------------------------------------------------------
int main(void)
{

    // initializing background map tiles

    // BG0 point to vram address 0x0000 and with two pages of 32 tiles (64x32)
    bgSetMapPtr(0, 0x0000, SC_64x32);

    // BG1 point to vram address 0x0000 + 2048 and with two pages of 32 tiles (64x32)
    bgSetMapPtr(1, 0x0000 + 2048, SC_64x32);

    // BG2 point to vram address 0x0000 and with one pages of 32 tiles (32x32)
    bgSetMapPtr(2, 0x1000, SC_32x32);

    // copy BG0 tiles to vram 0x2000 and palette
    bgInitTileSet(0, &BG1_tiles, &BG1_pal, 2, (&BG1_tiles_end - &BG1_tiles), 16 * 4, BG_16COLORS, 0x2000);
    bgInitTileSet(1, &BG2_tiles, &BG2_pal, 4, (&BG2_tiles_end - &BG2_tiles), 16 * 4, BG_16COLORS, 0x3000);
    bgInitTileSet(2, &BG3_tiles, &BG3_pal, 0, (&BG3_tiles_end - &BG3_tiles), 16 * 4, BG_4COLORS, 0x4000);

    // queue BG1 and BG2 to be updated in the vblank interruption
    bg_mutex = 0;
    updateBG1(&BG1_map, 0x0000, 2048);
    updateBG2(&BG2_map, 0x0000 + 2048, 2048);

    // copy background 3 (not scrollable)
    WaitForVBlank();
    dmaCopyVram(&BG3_map, 0x1000, 2048);

    // IMPORTANT - use mode1 with BG3_MODE1_PRIORITY_HIGH otherwise your HUD will not have the highest priority
    setMode(BG_MODE1, BG3_MODE1_PRIORITY_HIGH);

    // initialize sprite

    player1._x = 20;
    player1._y = 100;
    player1.oamAddress = 0;
    oamSetVisible(player1.oamAddress, OBJ_SHOW);
    // Init Sprites gfx and palette with default size of 32x32
    oamInitGfxSet(&character_tiles, (&character_tiles_end - &character_tiles), &character_pal, (&character_pal_end - &character_pal), 0, 0x6000, OBJ_SIZE16_L32);
    oamSet(player1.oamAddress, player1._x, player1._y, 2, 0, 0, 0, 0); // Put sprite in 100,100, with maximum priority 3 from tile entry 0, palette 0
    oamSetEx(player1.oamAddress, OBJ_SMALL, OBJ_SHOW);

    setScreenOn();

    bgMain.id = 0;
    bgMain.scrX = 255;
    bgMain.scrY = 255;
    bgMain.vram = 0x0000;
    bgMain.vRamFirstPage = true;
    bgMain.mapPage = 0;
    bgMain.bgMap = &BG1_map;
    bgMain.maxScrollX = 768;

    bgSub.id = 1;
    bgSub.scrX = 255;
    bgSub.scrY = 255;
    bgSub.vram = 0x0000 + 2048;
    bgSub.vRamFirstPage = true;
    bgSub.mapPage = 0;
    bgSub.bgMap = &BG2_map;
    bgSub.maxScrollX = 1024 + 128;

    bgSetScroll(0, 0, 32);
    bgSetScroll(1, 0, 32);

    // configure function to be called in the vblank interruption
    nmiSet(myconsoleVblank);

    // Wait for nothing :P
    while (1)
    {
        scanPads();
        pad0 = padsCurrent(0);
        // update character pos
        updatePos(&player1, pad0);

        // handle main background scroll
        handleScroll(&player1, &bgMain);

        // handle sub background scroll
        handleScrollSub(&player1, &bgSub);

        // Wait VBL 'and update sprites too ;-) )
        WaitForVBlank();
    }
    return 0;
}
