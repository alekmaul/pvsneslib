/*---------------------------------------------------------------------------------


    super scope demo
    -- alekmaul

    demo game made by DigiDwrf


---------------------------------------------------------------------------------*/

#include <snes.h>
#include "superscope.h"
#include "sin_cos.h"

extern char tilfont, palfont;

extern unsigned char sprites_map, sprites_map_end, sprites_palette;                                      // Sprites
extern unsigned char aim_target_tileset, aim_target_tileset_end, aim_target_map, aim_target_palette; // BG

void hideSprites()
{
    u8 i;
    for (i = 0; i < 128; i++)
        oamSetVisible((i << 2), OBJ_HIDE); // Hide sprites
}

void resetGame()
{
    // Let's reset everything
    consoleDrawText(12, 8, "        ");
    consoleDrawText(11, 10, "           ");
    consoleDrawText(3, 14, "                          ");
    consoleDrawText(3, 15, "                         ");

    hideSprites();

    targets_shot = 0;
    enable_fire = false;
    bullet_id = 0;
    shot_bullets = 0;

    u8 i;
    for (i = 0; i < max_bullets; i++)
    {
        difuse_xn[i] = 0;
        difuse_yn[i] = 0;
        difuse_x[i] = 0;
        difuse_y[i] = 0;
        bullet_frame[i] = 0;
        bullet_draw[i] = 0;
        bullet_frame_n[i] = 0;
        bullet_gravity[i] = 0;
        bullet_diff[i] = 0;
    }

    for (target_id = 0; target_id < 8; target_id++)
    {
        target_kill[target_id] = false;
        target_kill_count[target_id] = 0;
        target_gravity[target_id] = 0;
    }
}

//---------------------------------------------------------------------------------
// Main program
int main(void)
{
    // Initialize text console with our font
    consoleSetTextMapPtr(0x2000);
    consoleSetTextGfxPtr(0x3800);
    consoleSetTextOffset(0x0080);
    consoleInitText(1, 16 * 2, &tilfont, &palfont);

    // Init background
    bgInitTileSet(0, &aim_target_tileset, &aim_target_palette, 0, (&aim_target_tileset_end - &aim_target_tileset), 16 * 2, BG_16COLORS, 0x3000);
    bgSetGfxPtr(1, 0x3800);
    bgSetMapPtr(1, 0x2000, SC_32x32);

    // Init sprites
    oamInitGfxSet(&sprites_map, (&sprites_map_end - &sprites_map), &sprites_palette, 48 * 2, 0, 0x0000, OBJ_SIZE16_L32);

    u8 i;
    for (i = 0; i < max_targets; i++)
    {
        oamSet(16 + (i << 2), 0, 0, 2, 0, 0, 0x84, 1); // Target sprites
        oamSetEx(16 + (i << 2), OBJ_SMALL, OBJ_HIDE);
    }

    // Set mode 1 and disable BG3
    setMode(BG_MODE1, 0);
    bgSetDisable(2);

    // Set vertical scrolling to -1, default BG scrolling is one horizontal line above screen. We're doing this to place our center of screen perfectly.
    bgSetScroll(0, 0, -1);
    bgSetScroll(1, 0, -1);

START_OVER:
    bgInitMapSet(0, &aim_target_map, 0x700, SC_32x32, 0x2800);

    // Draw a wonderful text :P
    consoleDrawText(8, 1, "SUPERSCOPE Test");

    if (pause_adjust)
        consoleDrawText(1, 3, "Press PAUSE to return to game");
    else
    {
        detectSuperScope(); // Let's check if SuperScope is connected on boot
        if (snes_sscope == false)
            consoleDrawText(1, 26, "Connect SuperScope into Port 2");
        else
            consoleDrawText(6, 26, "SuperScope connected!");
    }

    // Sprites setup
    oamSet(0, 0, 0, 2, 0, 0, 0x80, 2); // Spot sprite, used for aim adjustment
    oamSetEx(0, OBJ_SMALL, OBJ_HIDE);

    setScreenOn();

    while (snes_sscope == false) // no SuperScope? Let's plug one
    {
        WaitForVBlank();
        detectSuperScope();
        if (snes_sscope)
            consoleDrawText(1, 26, "     SuperScope connected!    ");
    }

    if (pause_adjust == false)
        WaitNVBlank(60); // Hold on for one second

ADJUST_AIM:
    consoleDrawText(9, 24, "              ");
    consoleDrawText(3, 25, "  Shoot center of screen  ");
    consoleDrawText(3, 26, "       to adjust aim      ");

    while (1)
    {
        if (snes_sscope == false)
        {
            detectSuperScope();
            consoleDrawText(5, 25, "                      ");
            consoleDrawText(1, 26, "Connect SuperScope into Port 2");
            sscope_disconnected = true;
        }
        else
        {
            if (sscope_disconnected)
            {
                consoleDrawText(1, 26, "     SuperScope connected!    ");
                WaitNVBlank(60); // Hold on for one second
                sscope_disconnected = false;
                goto ADJUST_AIM;
            }

            if ((scope_down & SSC_FIRE) == false) // Don't go forward until fire is off at start
                enable_fire = true;

            if (pause_adjust)
                if (scope_down & SSC_PAUSE) // we read pause input from scope to continue game (pause) or adjust aim again (boot)
                {
                    setFadeEffect(FADE_OUT);
                    oamSet(0, 0, 0, 2, 0, 0, 0, 0); // Restore bullet sprite
                    oamSetVisible(0, OBJ_HIDE);     // Hide sprite
                    pause_adjust = false;
                    goto CONTINUE_GAME;
                }

            if (enable_fire)
            {
                if (scope_down & SSC_FIRE) // we read fire input from scope to begin aim adjust, just after VBlank
                {
                    enable_fire = false;
                    break;
                }
            }
        }
        WaitForVBlank();
    }

    // Start aim adjust process: it stores scope's raw values relative to the center of screen, so shot matches superscope's physical aim
    // We also move our scope's center to the center of screen
    scope_centerh = 0x80 - scope_shothraw;
    scope_centerv = 0x70 - scope_shotvraw;

    if (pause_adjust)
        consoleDrawText(1, 3, "                             ");
    consoleDrawText(5, 25, "         Good.        ");
    consoleDrawText(8, 26, "                ");

    oamSetXY(0, 0x78, 0x68); // Draw spot at center, just for reference, most games do this to tell the player that SuperScope was shot

    WaitNVBlank(60);            // Hold on for one second
    oamSetVisible(0, OBJ_HIDE); // Hide spot
    consoleDrawText(9, 25, "Test your aim");

    while (1)
    {
        if (snes_sscope == false)
        {
            detectSuperScope();
            consoleDrawText(5, 25, "                      ");
            consoleDrawText(1, 26, "Connect SuperScope into Port 2");
            sscope_disconnected = true;
        }
        else
        {
            if (sscope_disconnected)
            {
                consoleDrawText(1, 26, "     SuperScope connected!    ");
                WaitNVBlank(60); // Hold on for one second
                sscope_disconnected = false;
                goto ADJUST_AIM;
            }

            if ((scope_down & SSC_FIRE) == false) // Don't go forward until fire is off at start
                enable_fire = true;

            if (enable_fire)
            {
                if (scope_down & SSC_FIRE) // we read fire input from scope to see if our aim is adjusted
                {
                    enable_fire = false;
                    break;
                }
            }
        }
        WaitForVBlank();
    }

    // To finish aim adjust we store coords relative to the center of screen
    oamSetXY(0, scope_shoth - 0x08, scope_shotv - 0x08); // Draw spot aligned with our aim if the aim was on the center on first shot, this means our aim is adjusted.

    WaitNVBlank(60); // Hold on for one second

    consoleDrawText(9, 24, "Are you ready?");
    if (pause_adjust)
    {
        consoleDrawText(5, 25, "Press PAUSE to return");
        consoleDrawText(3, 26, "Press CURSOR to adjust aim");
    }
    else
    {
        consoleDrawText(4, 25, "Press PAUSE to adjust aim");
        consoleDrawText(6, 26, "Press CURSOR to play");
    }

    while (1)
    {
        if (snes_sscope == false)
        {
            detectSuperScope();
            consoleDrawText(2, 24, "                             ");
            consoleDrawText(4, 25, "                         ");
            consoleDrawText(1, 26, "Connect SuperScope into Port 2");
            oamSetVisible(0, OBJ_HIDE); // Hide spot
            sscope_disconnected = true;
        }
        else
        {
            if ((scope_down & SSC_CURSOR) == false) // Don't go ahead until cursor is off at start
                enable_cursor = true;

            if (sscope_disconnected)
            {
                consoleDrawText(1, 26, "     SuperScope connected!    ");
                WaitNVBlank(60); // Hold on for one second
                sscope_disconnected = false;
                goto ADJUST_AIM;
            }

            if (scope_down & SSC_PAUSE) // we read pause input from scope to continue game (pause) or adjust aim again (boot)
            {
                oamSetVisible(0, OBJ_HIDE); // Hide spot
                if (pause_adjust)
                {
                    setFadeEffect(FADE_OUT);
                    oamSet(0, 0, 0, 2, 0, 0, 0, 0); // Restore bullet sprite
                    oamSetVisible(0, OBJ_HIDE);     // Hide sprite
                    pause_adjust = false;
                    goto CONTINUE_GAME;
                }
                else
                    goto ADJUST_AIM;
            }

            if (enable_cursor)
                if (scope_down & SSC_CURSOR) // we read cursor input from scope to adjust aim again (pause) or begin game (boot)
                {
                    enable_cursor = false;
                    if (pause_adjust)
                    {
                        oamSetVisible(0, OBJ_HIDE); // Hide spot
                        consoleDrawText(1, 3, "Press PAUSE to return to game");
                        goto ADJUST_AIM;
                    }
                    else
                        break;
                }
        }

        WaitForVBlank();
    }

    oamSetVisible(0, OBJ_HIDE); // Hide spot

    /* Before we enter main game, let's setup bullet sprites, we need them to be bigger at first, make them dissapear into the horizon and draw new bullets over old ones,
    so we're going to set multiple sprites and make them appear one after another, like a "set of cards". No coordinates are needed at this time. */

    while (bullet_frame[0] < 8)
    {
        oamSet(bullet_draw[0] << 2, 0, 0, 2, 0, 0, bullet_frames[bullet_frame[0]], 0);
        oamSetVisible(bullet_draw[0] << 2, OBJ_HIDE); // Hide sprite
        bullet_draw[0]++;
        if (bullet_draw[0] == 4) // We skip some sprite IDs, so targets are in between bullets, for a more realistic behavior
            bullet_draw[0] = 68;

        bullet_frame_n[0]++;
        if (bullet_frame_n[0] > ((bullet_frame[0] - 4) << 3)) // This will just repeat multiple bullet sprites into OAM, so they seem to go far away
        {
            bullet_frame[0]++;
            bullet_frame_n[0] = 0;
        }
    }

    // We also have to setup Fire rate
    scope_holddelay = 60; // We wait a secont until we want to consider that the button has been held
    scope_repdelay = slow_fire_rate;  // Fire rate

    setFadeEffect(FADE_OUT);

PLAY_GAME:

    // Initial positions of targets
    for (target_id = 0; target_id < 8; target_id++)
    {
        target_x_inc[target_id] = target_id << 6;
        target_y_inc[target_id] = target_id << 6;
        target_oam_id[target_id] = 16 + target_id << 2;
    }

    bool print_once = true; // We'll use this later to print text just once inside our loop
    u8 text_timer = 0;

CONTINUE_GAME:

    if (text_timer < 120)
    {
        consoleDrawText(13, 13, "SHOOT");
        consoleDrawText(10, 14, "THE TARGETS!");
    }

    consoleDrawText(8, 1, "               ");
    consoleDrawText(1, 3, "                             ");
    consoleDrawText(8, 4, "                ");
    consoleDrawText(9, 24, "              ");
    consoleDrawText(4, 25, "                         ");
    consoleDrawText(3, 26, "                          ");

    setPaletteColor(0, 0x7FFF);  // Let's use a white BACK
    setPaletteColor(17, 0x0000); // and black for texts

    WaitForVBlank();
    dmaFillVram(&aim_target_map, 0x2800, 0x700); // Wipe screen
    oamInitGfxAttr(0x0000, OBJ_SIZE32_L64);       // Set to bigger sprites

    setScreenOn();

    // MAIN GAME
    while (1)
    {
        switch (text_timer)
        {
        case 120: // Wait two seconds before we wipe text from screen
            consoleDrawText(13, 13, "     ");
            consoleDrawText(10, 14, "            ");
        default:
            text_timer++;
            break;
        case 121:
            // Do nothing
            break;
        }

        if (snes_sscope == false)
        {
            detectSuperScope();
            consoleDrawText(1, 13, "Connect SuperScope into Port 2");
            consoleDrawText(3, 14, "                          ");
            consoleDrawText(3, 15, "                         ");
            sscope_disconnected = true;
        }
        else
        {
            if (sscope_disconnected)
            {
                consoleDrawText(1, 13, "     SuperScope connected!    ");
                WaitNVBlank(60); // Hold on for one second
                consoleDrawText(6, 13, "                     ");
                sscope_disconnected = false;
            }

            if ((scope_down & SSC_FIRE) == false) // Don't shoot until fire is off at start
                enable_fire = true;
            if ((scope_down & SSC_PAUSE) == false) // Don't pause until pause is off at start
                enable_pause = true;
            if ((scope_down & SSC_CURSOR) == false) // Don't play again until cursor is off at start
                enable_cursor = true;

            if (enable_fire)
            {
                if (scope_held & SSC_FIRE) // We're using scope_held to swap from a normal fire rate to a slower firerate
                    if (cool_down < 2)
                        cool_down++;

                if ((scope_down & SSC_FIRE) == 0)
                    cool_down = 0;

                if (((scope_held & SSC_FIRE) && (cool_down == 2)) || ((scope_down & SSC_FIRE) && (ready_to_fire == normal_fire_rate)))
                {
                    bullet_id++;
                    if (bullet_id == 32)
                        bullet_id = 0;

                    if (scope_shoth < 0x110)
                    {
                        bullet_x[bullet_id] = scope_shoth - 0x10;
                        if (scope_shotv < 0xE0)
                            bullet_y[bullet_id] = scope_shotv - 0x10;
                        else
                            bullet_y[bullet_id] = 0xE0;
                    }
                    else
                    {
                        bullet_x[bullet_id] = 0x110;
                        bullet_y[bullet_id] = 0xE0;
                    }

                    bullet_frame[bullet_id] = 0;
                    bullet_draw[bullet_id] = 0;

                    // To make more of a natural shot, we add some random directions and gravity
                    bullet_gravity[bullet_id] = 0;
                    difuse_xn[bullet_id] = 0;
                    difuse_yn[bullet_id] = 0;
                    difuse_x[bullet_id] = (rand() & 0x0F) - 8;
                    difuse_y[bullet_id] = ((rand() & 0xF0) >> 4) - 8;

                    shot_bullets++; // incease shot bullets by one
                    ready_to_fire = 0;
                }

                if (cool_down < 2)
                {
                    if (ready_to_fire < normal_fire_rate)
                        ready_to_fire++; // This will prevent lag if we shoot too fast
                }
                else
                    ready_to_fire = 0;

                for (bullet_num = 0; bullet_num < shot_bullets; bullet_num++)
                {
                    s8 id = bullet_id - bullet_num;
                    if (id < 0)
                        id += 32; // To avoid negative overflow on reset

                    if (bullet_frame[id] < 8)
                    {
                        if (bullet_draw[id])
                            oamSetVisible((bullet_draw[id] - bullet_diff[id]) << 2, OBJ_HIDE); // Remove used sprites
                        difuse_xn[id] += difuse_x[id];
                        bullet_gravity[id]++;
                        difuse_yn[id] += difuse_y[id] + (bullet_gravity[id] >> 2);

                        oamSetXY(bullet_draw[id] << 2, bullet_x[id] + (difuse_xn[id] >> 6), bullet_y[id] + (difuse_yn[id] >> 6)); // Draw new bullets sprites on screen

                        bullet_draw[id]++;
                        if (bullet_draw[id] == 4)
                        {
                            bullet_draw[id] = 68; // We skip some sprite IDs, so targets are in between bullets, for a more realistic behavior
                            bullet_diff[id] = 65;
                        }
                        else
                            bullet_diff[id] = 1;

                        bullet_frame_n[id]++;
                        if (bullet_frame_n[id] > ((bullet_frame[id] - 4) << 3))
                        {
                            bullet_frame[id]++;
                            bullet_frame_n[id] = 0;
                        }
                    }
                    else if (bullet_frame[id] == 8)
                    {
                        oamSetVisible((bullet_draw[id] - 1) << 2, OBJ_HIDE); // Remove last used sprite
                        shot_bullets--;                                      // decrease shot bullets by one
                        bullet_frame[id]++;                                  // just to be sure this part of code executes just once
                    }
                }
            }

            // DRAW TARGETS ON SCREEN

            if (targets_shot < 8)
            {
                // Pause routine
                if (enable_pause)
                    if (scope_down & SSC_PAUSE)
                    {
                        setFadeEffect(FADE_OUT);
                        consoleDrawText(13, 13, "     ");
                        consoleDrawText(10, 14, "            ");

                        setPalette(&aim_target_palette, 0, 2); // Restore blue BACK
                        setPaletteColor(17, 0x7FFF);            // Restore white text

                        hideSprites();
                        pause_adjust = true;

                        enable_fire = false;
                        enable_cursor = false;
                        goto START_OVER;
                    }

                // We are going to create a rotating carrousel made of 8 targets
                for (target_id = 0; target_id < 8; target_id++)
                {
                    if (target_kill[target_id] == false)
                    {
                        target_x_inc[target_id]++;
                        target_y_inc[target_id]++;
                        if (target_x_inc[target_id] > 511)
                            target_x_inc[target_id] = 0;
                        if (target_y_inc[target_id] > 511)
                            target_y_inc[target_id] = 0;

                        target_x[target_id] = ((cos_table(target_x_inc[target_id]) << 1) / 3) + 27;
                        target_y[target_id] = ((sin_table(target_y_inc[target_id]) << 1) / 3) + 11;

                        oamSetXY(target_oam_id[target_id], target_x[target_id], target_y[target_id]); // Target sprites

                        target_collision_x[target_id] = oamGetX(target_oam_id[target_id]) - oamGetX(12); // id 3 (* 4) is the fourth frame of bullet animation. We use this one to collide with target
                        target_collision_y[target_id] = oamGetY(target_oam_id[target_id]) - oamGetY(12); // id 3 (* 4) is the fourth frame of bullet animation. We use this one to collide with target

                        if ((target_collision_x[target_id] > -12) && (target_collision_x[target_id] < 12) && (target_collision_y[target_id] > -12) && (target_collision_y[target_id] < 12))
                            target_kill[target_id] = true;
                    }

                    // Kill target if it was "touched" by a bullet
                    if (target_kill[target_id])
                    {
                        switch (target_kill_count[target_id])
                        {
                        case 0:
                            oamSetGfxOffset(target_oam_id[target_id], 0x88);
                            target_x[target_id] -= 16; // Fix larger object offset
                            target_y[target_id] -= 16; // Fix larger object offset
                            oamSetXY(target_oam_id[target_id], target_x[target_id], target_y[target_id]);
                            oamSetEx(target_oam_id[target_id], OBJ_LARGE, OBJ_SHOW);
                            break;
                        case 3:
                            oamSetGfxOffset(target_oam_id[target_id], 0x100);
                            break;
                        case 6:
                            oamSetGfxOffset(target_oam_id[target_id], 0x108);
                            break;
                        case 12:
                            oamSetGfxOffset(target_oam_id[target_id], 0x84);         // Restore target sprite GFX
                            oamSetEx(target_oam_id[target_id], OBJ_SMALL, OBJ_HIDE); // Restore small size and hide sprite
                            targets_shot++;
                            enable_pause = false;
                            enable_cursor = false;
                            target_kill_count[target_id]++; // Get out of case
                            break;
                        }
                        if (target_kill_count[target_id] < 12)
                        {
                            target_gravity[target_id]++;
                            oamSetXY(target_oam_id[target_id], target_x[target_id], target_y[target_id] + (target_gravity[target_id] >> 2));
                            target_kill_count[target_id]++;

                            if (target_kill_count[target_id] & 0x02)
                                oamSetVisible(target_oam_id[target_id], OBJ_HIDE); // Blink
                        }
                    }
                }
            }
            else // We have won!
            {
                if (print_once == true)
                {
                    consoleDrawText(12, 8, "YOU WIN!");
                    consoleDrawText(11, 10, "Play again?");
                    print_once = false;
                }

                consoleDrawText(3, 14, "Press CURSOR to play again");
                consoleDrawText(3, 15, "Press PAUSE to adjust aim");

                if (enable_cursor)
                    if (scope_down & SSC_CURSOR)
                    {
                        setFadeEffect(FADE_OUT);

                        resetGame();
                        goto PLAY_GAME;
                    }

                if (enable_pause)
                    if (scope_down & SSC_PAUSE)
                    {
                        setFadeEffect(FADE_OUT);

                        resetGame();
                        setPalette(&aim_target_palette, 0, 2); // Restore blue BACK
                        setPaletteColor(17, 0x7FFF);            // Restore white text
                        goto START_OVER;
                    }
            }
        }

        WaitForVBlank();
    }

    return 0;
}