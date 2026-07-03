# Game States Tutorial {#tutorial_game_states}

This tutorial covers game state management on the SNES, including state machines, screen transitions, and common game flow patterns (title screen, gameplay, pause, game over).

## What Are Game States?

A game state represents what the game is currently doing. Most SNES games cycle through a small set of states:

| State | Purpose |
|-------|---------|
| **Title** | Show title screen, wait for START |
| **Playing** | Run game logic, handle input, render |
| **Paused** | Freeze game logic, show overlay message |
| **Game Over** | Display result, wait for restart |

Each state has its own update logic, and transitions between states involve screen fades, VRAM reloads, or both.

## Simple State Machine

The standard SNES pattern uses a state variable and a `switch` in the main loop. Define states as constants and dispatch each frame:

```c
#include <snes.h>

#define STATE_TITLE      0
#define STATE_PLAYING    1
#define STATE_PAUSED     2
#define STATE_GAME_OVER  3

static u8 game_state;

int main(void) {
    consoleInit();
    setMode(BG_MODE1, 0);

    game_state = STATE_TITLE;

    while (1) {
        switch (game_state) {
            case STATE_TITLE:
                stateTitle();
                break;
            case STATE_PLAYING:
                statePlaying();
                break;
            case STATE_PAUSED:
                statePaused();
                break;
            case STATE_GAME_OVER:
                stateGameOver();
                break;
        }

        WaitForVBlank();
    }

    return 0;
}
```

This is the exact pattern used in `examples/games/tetris/main.c`. Each state function runs once per frame and can change `game_state` to trigger a transition.

## State Transitions

Transitioning between states often requires reloading VRAM, resetting variables, or fading the screen. The SNES PPU silently ignores VRAM writes during active display, so transitions must happen during VBlank or forced blank.

### Fade Out / Fade In

The INIDISP register (`$2100`) controls screen brightness (0-15). Stepping through brightness levels over multiple frames creates a smooth fade:

```c
static void fade_out(u8 speed) {
    s8 brightness;
    u8 i;

    for (brightness = 15; brightness >= 0; brightness--) {
        setBrightness((u8)brightness);
        for (i = 0; i < speed; i++) {
            WaitForVBlank();
        }
    }
}

static void fade_in(u8 speed) {
    u8 brightness;
    u8 i;

    for (brightness = 0; brightness <= 15; brightness++) {
        setBrightness(brightness);
        for (i = 0; i < speed; i++) {
            WaitForVBlank();
        }
    }
}
```

A speed of 1 gives a fast 16-frame fade (~267ms). A speed of 3 gives a slower cinematic fade. See `examples/graphics/effects/fading/` for a complete working example.

### VRAM Reload Between States

When switching from title screen to gameplay (or vice versa), you typically need to load different tiles, tilemaps, and palettes. Use forced blank to guarantee VRAM writes succeed:

```c
static void transition_to_gameplay(void) {
    /* Fade to black */
    fade_out(2);

    /* Screen is dark — force blank for safe VRAM access */
    setScreenOff();

    /* Load gameplay assets */
    dmaCopyVram(gameplay_tiles, 0x1000, gameplay_tiles_size);
    dmaCopyVram(gameplay_map, 0x0400, gameplay_map_size);
    dmaCopyCGram(gameplay_palette, 0, 256);

    /* Reset game variables */
    score = 0;
    lives = 3;

    /* Turn screen back on and fade in */
    setScreenOn();
    fade_in(2);

    game_state = STATE_PLAYING;
}
```

The key rule: call `setScreenOff()` before bulk VRAM/CGRAM writes, and `setScreenOn()` after. This is the forced blank pattern used throughout the OpenSNES examples.

## Title Screen Implementation

A title screen waits for the player to press START, then transitions to gameplay. The pattern from `examples/games/tetris/main.c`:

```c
static void stateTitle(void) {
    /* Poll for START press */
    if ((pad_keys[0] & KEY_START) == 0) {
        return;  /* Not pressed — keep waiting */
    }

    /* Wait for START release to prevent immediate pause */
    do {
        WaitForVBlank();
    } while (pad_keys[0] & KEY_START);

    /* Transition to gameplay */
    startGame();
}
```

The release-wait loop (`do { WaitForVBlank(); } while (pad_keys[0] & KEY_START)`) prevents the START press from triggering an immediate pause in the gameplay state. This is a standard debounce pattern on the SNES.

### Complete Title Flow

Breakout (`examples/games/breakout/main.c`) shows the full sequence: display "READY" text, wait for START, clear the message, then enter the game loop:

```c
/* Display ready message in tilemap */
writestring(ST_READY, blockmap, 0x248, 0x3F6);
WaitForVBlank();
dmaCopyVram((u8 *)blockmap, 0x0000, 0x800);

/* Wait for START press */
do { WaitForVBlank(); } while ((pad_keys[0] & KEY_START) == 0);

/* Wait for START release */
do { WaitForVBlank(); } while (pad_keys[0] & KEY_START);

/* Clear message and begin */
writestring(ST_BLANK, blockmap, 0x248, 0x3F6);
WaitForVBlank();
dmaCopyVram((u8 *)blockmap, 0x0000, 0x800);
```

## Pause Screen

Pausing overlays a message on the current gameplay, freezes all game logic, and resumes when START is pressed again. The key is a three-phase wait: release START, wait for START press, wait for release again.

From `examples/games/breakout/main.c`:

```c
static void handle_pause(void) {
    if ((pad0 & KEY_START) == 0) return;

    /* Show "PAUSED" in tilemap and flush to VRAM */
    writestring(ST_PAUSED, blockmap, 0x269, 0x3F6);
    WaitForVBlank();
    dmaCopyVram((u8 *)blockmap, 0x0000, 0x800);

    /* Three-phase wait: release -> press -> release */
    do { WaitForVBlank(); } while (pad_keys[0] & KEY_START);
    do { WaitForVBlank(); } while ((pad_keys[0] & KEY_START) == 0);
    do { WaitForVBlank(); } while (pad_keys[0] & KEY_START);

    /* Clear message and resume */
    writestring(ST_BLANK, blockmap, 0x269, 0x3F6);
    WaitForVBlank();
    dmaCopyVram((u8 *)blockmap, 0x0000, 0x800);
}
```

The three-phase wait prevents the unpause START press from being read as a new pause request on the next frame. Tetris (`examples/games/tetris/main.c`) uses the same pattern with a `prev_pad` edge-detection approach:

```c
u16 pressed = pad & ~prev_pad;  /* newly pressed this frame */

if (pressed & KEY_START) {
    paused = 1;
    hudShowMessage(str_paused);
    /* ... three-phase wait ... */
    paused = 0;
}

if (paused) return;  /* skip all game logic */
```

### Pause Design Rules

- **Do not reload VRAM** for a simple pause overlay. Write to a tilemap buffer and DMA it.
- **Freeze game logic** by returning early from the update function while paused.
- **The NMI handler keeps running** during pause. Input is still read, OAM is still transferred. Only your game logic stops.
- **Redraw sprites** before entering the pause loop if you use delta rendering, so the screen looks correct while frozen.

## Game Over

Game over detection happens in gameplay logic. When the end condition is met, display a message and wait for player input before restarting.

### Simple Game Over (Breakout)

Breakout detects game over when lives reach zero. It shows a message and halts:

```c
static void die(void) {
    if (lives == 0) {
        /* Show GAME OVER message */
        writestring(ST_GAMEOVER, blockmap, 0x267, 0x3F6);
        WaitForVBlank();
        dmaCopyVram((u8 *)blockmap, 0x0000, 0x800);

        /* Halt — player must reset */
        while (1) { WaitForVBlank(); }
    }

    /* Still have lives: reset ball position, continue */
    lives--;
    pos_x = 94;
    pos_y = 109;
    /* ... */
}
```

### Restart Game Over (Tetris)

Tetris allows restarting. When a new piece cannot spawn (board full), the game transitions to `STATE_GAME_OVER`, which shows a message, waits for START, then calls `startGame()` to reinitialize everything:

```c
static void stateGameOver(void) {
    renderBoard();
    hudShowMessage(str_gameover);

    /* Force blank for guaranteed DMA */
    WaitForVBlank();
    setScreenOff();
    renderFlush();
    setScreenOn();

    /* Wait for START to restart */
    while ((pad_keys[0] & KEY_START) == 0) {
        WaitForVBlank();
        renderFlush();
    }
    do { WaitForVBlank(); } while (pad_keys[0] & KEY_START);

    hudClearMessage();
    startGame();  /* reinitialize board, score, spawn first piece */
}
```

### Game Over with Fade

Combining game over with a fade transition creates a polished feel:

```c
static void stateGameOver(void) {
    /* Show game over message */
    writestring("GAME OVER", tilemap_buf, msg_pos, offset);
    WaitForVBlank();
    dmaCopyVram((u8 *)tilemap_buf, TILEMAP_ADDR, TILEMAP_SIZE);

    /* Wait for player acknowledgement */
    while ((pad_keys[0] & KEY_START) == 0) {
        WaitForVBlank();
    }
    do { WaitForVBlank(); } while (pad_keys[0] & KEY_START);

    /* Fade out, reload, fade in */
    fade_out(2);
    setScreenOff();

    /* Reset all game state and reload assets */
    initGame();

    setScreenOn();
    fade_in(2);

    game_state = STATE_TITLE;
}
```

## State Machine Summary

| Concern | Pattern |
|---------|---------|
| State storage | `static u8 game_state;` with `#define` constants |
| Dispatch | `switch (game_state)` in main loop, one call per frame |
| Transition | Set `game_state = NEW_STATE;` then `return;` |
| VRAM reload | `setScreenOff()` before DMA, `setScreenOn()` after |
| Fade | Step `setBrightness(0..15)` across frames |
| Button debounce | Three-phase: release, press, release |
| Pause | Overlay text, freeze logic, same VRAM |
| Game over | Message, wait for input, reinitialize or halt |

## Next Steps

- @ref tutorial_graphics "Graphics & Backgrounds"
- @ref tutorial_sprites "Sprites & Animation"
- @ref tutorial_input "Controller Input"