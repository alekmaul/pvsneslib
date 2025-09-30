# SNES Examples using PVSnesLib - PURE CMAKE IMPLEMENTATION
# NO MAKEFILES - ONLY CMAKE

# Include our SNES game helper functions
include(${CMAKE_SOURCE_DIR}/cmake/SNESGame.cmake)

# Hello World example
add_snes_game(hello_world
    SOURCES hello_world/src/hello_world.c
    GRAPHICS hello_world/pvsneslibfont.png
    ASM_SOURCES hello_world/data.asm hello_world/hdr.asm
    ROM_NAME hello_world
)

# Breakpoints example
add_snes_game(breakpoints
    SOURCES breakpoints/src/breakpoints.c
    GRAPHICS breakpoints/pvsneslibfont.png
    ASM_SOURCES breakpoints/data.asm breakpoints/hdr.asm
    ROM_NAME breakpoints
)

# Debug example (no graphics)
add_snes_game(debug
    SOURCES debug/debug.c
    ASM_SOURCES debug/data.asm debug/hdr.asm
    ROM_NAME debug
)

# Random example
add_snes_game(random
    SOURCES random/random.c
    GRAPHICS random/pvsneslibfont.bmp
    ASM_SOURCES random/data.asm random/hdr.asm
    ROM_NAME random
)

# Scoring example
add_snes_game(scoring
    SOURCES scoring/scoring.c
    GRAPHICS scoring/pvsneslibfont.bmp
    ASM_SOURCES scoring/data.asm scoring/hdr.asm
    ROM_NAME scoring
)

# Timer example
add_snes_game(timer
    SOURCES timer/timer.c
    GRAPHICS timer/pvsneslibfont.bmp
    ASM_SOURCES timer/data.asm timer/hdr.asm
    ROM_NAME timer
)

# Test region example
add_snes_game(testregion
    SOURCES testregion/testregion.c
    GRAPHICS testregion/pvsneslibfont.bmp
    ASM_SOURCES testregion/data.asm testregion/hdr.asm
    ROM_NAME testregion
)

# Memory mapping example (uses HiROM)
add_snes_game(memory_mapping
    SOURCES memory_mapping/src/memory_mapping.c
    GRAPHICS memory_mapping/pvsneslibfont.png
    ASM_SOURCES memory_mapping/data.asm memory_mapping/hdr.asm
    ROM_NAME memory_mapping
    HIROM ON
    FASTROM ON
)

# Type console example (uses pal_ntsc.c)
add_snes_game(typeconsole
    SOURCES typeconsole/src/pal_ntsc.c
    GRAPHICS typeconsole/pvsneslibfont.bmp
    ASM_SOURCES typeconsole/data.asm typeconsole/hdr.asm
    ROM_NAME typeconsole
)

#=============================================================================
# AUDIO EXAMPLES - Advanced sound and music examples
#=============================================================================

# Audio effects example
add_snes_game(effects
    SOURCES audio/effects/effects.c
    GRAPHICS audio/effects/pvsneslibfont.bmp
    ASM_SOURCES audio/effects/data.asm audio/effects/hdr.asm
    AUDIO_FILES audio/effects/res/effectssfx.it
    SOUNDBANK audio/effects/res/soundbank
    ROM_NAME effects
)

# Effects and music combined example
add_snes_game(effectsandmusic
    SOURCES audio/effectsandmusic/effectsandmusic.c
    GRAPHICS audio/effectsandmusic/pvsneslibfont.bmp
    ASM_SOURCES audio/effectsandmusic/data.asm audio/effectsandmusic/hdr.asm
    AUDIO_FILES audio/effectsandmusic/res/effectssfx.it audio/effectsandmusic/res/pollen8.it
    SOUNDBANK audio/effectsandmusic/res/soundbank
    ROM_NAME effectsandmusic
)

# Basic music example
add_snes_game(music
    SOURCES audio/music/music.c
    GRAPHICS audio/music/pvsneslibfont.bmp
    ASM_SOURCES audio/music/data.asm audio/music/hdr.asm
    AUDIO_FILES audio/music/res/pollen8.it
    SOUNDBANK audio/music/res/soundbank
    ROM_NAME music
)

# Music example 2
add_snes_game(music2
    SOURCES audio/music2/music2.c
    GRAPHICS audio/music2/pvsneslibfont.bmp
    ASM_SOURCES audio/music2/data.asm audio/music2/hdr.asm
    AUDIO_FILES audio/music2/res/pollen8.it
    SOUNDBANK audio/music2/res/soundbank
    ROM_NAME music2
)

# Large music example (>32KB)
add_snes_game(musicGreaterThan32k
    SOURCES audio/musicGreaterThan32k/musicGreaterThan32k.c
    GRAPHICS audio/musicGreaterThan32k/pvsneslibfont.bmp
    ASM_SOURCES audio/musicGreaterThan32k/data.asm audio/musicGreaterThan32k/hdr.asm
    AUDIO_FILES audio/musicGreaterThan32k/res/whatislove.it
    SOUNDBANK audio/musicGreaterThan32k/res/soundbank
    ROM_NAME musicGreaterThan32k
)

# HiROM music example
add_snes_game(musicHiROM
    SOURCES audio/musicHiROM/musicHiROM.c
    GRAPHICS audio/musicHiROM/pvsneslibfont.bmp audio/musicHiROM/dancer.png
    ASM_SOURCES audio/musicHiROM/data.asm audio/musicHiROM/hdr.asm
    AUDIO_FILES audio/musicHiROM/res/whatislove.it
    SOUNDBANK audio/musicHiROM/res/soundbank
    ROM_NAME musicHiROM
    HIROM ON
)

# Simple sound effect example
add_snes_game(tada
    SOURCES audio/tada/tada.c
    GRAPHICS audio/tada/pvsneslibfont.bmp
    ASM_SOURCES audio/tada/data.asm audio/tada/hdr.asm
    ROM_NAME tada
)

#=============================================================================
# GRAPHICS EXAMPLES - Advanced graphics and visual effects
#=============================================================================

# Background Examples
add_snes_game(Mode0
    SOURCES graphics/Backgrounds/Mode0/Mode0.c
    GRAPHICS graphics/Backgrounds/Mode0/bg0.bmp graphics/Backgrounds/Mode0/bg1.bmp graphics/Backgrounds/Mode0/bg2.bmp graphics/Backgrounds/Mode0/bg3.bmp
    ASM_SOURCES graphics/Backgrounds/Mode0/data.asm graphics/Backgrounds/Mode0/hdr.asm
    ROM_NAME Mode0
)

add_snes_game(Mode1
    SOURCES graphics/Backgrounds/Mode1/Mode1.c
    GRAPHICS graphics/Backgrounds/Mode1/pvsneslib.png
    ASM_SOURCES graphics/Backgrounds/Mode1/data.asm graphics/Backgrounds/Mode1/hdr.asm
    ROM_NAME Mode1
)

add_snes_game(Mode1BG3HighPriority
    SOURCES graphics/Backgrounds/Mode1BG3HighPriority/Mode1BG3HighPriority.c
    GRAPHICS graphics/Backgrounds/Mode1BG3HighPriority/BG1.bmp graphics/Backgrounds/Mode1BG3HighPriority/BG2.bmp graphics/Backgrounds/Mode1BG3HighPriority/BG3.bmp
    ASM_SOURCES graphics/Backgrounds/Mode1BG3HighPriority/data.asm graphics/Backgrounds/Mode1BG3HighPriority/hdr.asm
    ROM_NAME Mode1BG3HighPriority
)

add_snes_game(Mode1ContinuousScroll
    SOURCES graphics/Backgrounds/Mode1ContinuosScroll/Mode1ContinuosScroll.c
    GRAPHICS graphics/Backgrounds/Mode1ContinuosScroll/BG1.bmp graphics/Backgrounds/Mode1ContinuosScroll/BG2.bmp graphics/Backgrounds/Mode1ContinuosScroll/BG3.bmp graphics/Backgrounds/Mode1ContinuosScroll/character.bmp
    ASM_SOURCES graphics/Backgrounds/Mode1ContinuosScroll/data.asm graphics/Backgrounds/Mode1ContinuosScroll/hdr.asm
    ROM_NAME Mode1ContinuousScroll
)

add_snes_game(Mode1LZ77
    SOURCES graphics/Backgrounds/Mode1LZ77/Mode1LZ77.c
    GRAPHICS graphics/Backgrounds/Mode1LZ77/pvsneslib.png
    ASM_SOURCES graphics/Backgrounds/Mode1LZ77/data.asm graphics/Backgrounds/Mode1LZ77/hdr.asm
    ROM_NAME Mode1LZ77
)

add_snes_game(Mode1MixedScroll
    SOURCES graphics/Backgrounds/Mode1MixedScroll/Mode1MixedScroll.c
    GRAPHICS graphics/Backgrounds/Mode1MixedScroll/pvsneslib.bmp graphics/Backgrounds/Mode1MixedScroll/shader.bmp
    ASM_SOURCES graphics/Backgrounds/Mode1MixedScroll/data.asm graphics/Backgrounds/Mode1MixedScroll/hdr.asm
    ROM_NAME Mode1MixedScroll
)

add_snes_game(Mode1Png
    SOURCES graphics/Backgrounds/Mode1Png/Mode1.c
    GRAPHICS graphics/Backgrounds/Mode1Png/pvsneslib.png
    ASM_SOURCES graphics/Backgrounds/Mode1Png/data.asm graphics/Backgrounds/Mode1Png/hdr.asm
    ROM_NAME Mode1Png
)

add_snes_game(Mode1Scroll
    SOURCES graphics/Backgrounds/Mode1Scroll/Mode1Scroll.c
    GRAPHICS graphics/Backgrounds/Mode1Scroll/map_512_512.bmp graphics/Backgrounds/Mode1Scroll/pvsneslibfont.bmp
    ASM_SOURCES graphics/Backgrounds/Mode1Scroll/data.asm graphics/Backgrounds/Mode1Scroll/hdr.asm
    ROM_NAME Mode1Scroll
)

add_snes_game(Mode3
    SOURCES graphics/Backgrounds/Mode3/Mode3.c
    GRAPHICS graphics/Backgrounds/Mode3/pvsneslib.bmp
    ASM_SOURCES graphics/Backgrounds/Mode3/data.asm graphics/Backgrounds/Mode3/hdr.asm
    ROM_NAME Mode3
)

add_snes_game(Mode5
    SOURCES graphics/Backgrounds/Mode5/Mode5.c
    GRAPHICS graphics/Backgrounds/Mode5/pvsneslib.png
    ASM_SOURCES graphics/Backgrounds/Mode5/data.asm graphics/Backgrounds/Mode5/hdr.asm
    ROM_NAME Mode5
)

add_snes_game(Mode7
    SOURCES graphics/Backgrounds/Mode7/Mode7.c
    GRAPHICS graphics/Backgrounds/Mode7/mode7bg.bmp
    ASM_SOURCES graphics/Backgrounds/Mode7/data.asm graphics/Backgrounds/Mode7/hdr.asm
    ROM_NAME Mode7
)

add_snes_game(Mode7Perspective
    SOURCES graphics/Backgrounds/Mode7Perspective/Mode7Perspective.c
    GRAPHICS graphics/Backgrounds/Mode7Perspective/ground.png graphics/Backgrounds/Mode7Perspective/sky.png
    ASM_SOURCES graphics/Backgrounds/Mode7Perspective/data.asm graphics/Backgrounds/Mode7Perspective/hdr.asm
    ROM_NAME Mode7Perspective
)

# Effects Examples
add_snes_game(Fading
    SOURCES graphics/Effects/Fading/Fading.c
    GRAPHICS graphics/Effects/Fading/pvsneslib.bmp
    ASM_SOURCES graphics/Effects/Fading/data.asm graphics/Effects/Fading/hdr.asm
    ROM_NAME Fading
)

add_snes_game(GradientColors
    SOURCES graphics/Effects/GradientColors/GradientColors.c
    GRAPHICS graphics/Effects/GradientColors/pvsneslib.png
    ASM_SOURCES graphics/Effects/GradientColors/data.asm graphics/Effects/GradientColors/hdr.asm
    ROM_NAME GradientColors
)

add_snes_game(HDMAGradient
    SOURCES graphics/Effects/HDMAGradient/HDMAGradient.c
    GRAPHICS graphics/Effects/HDMAGradient/pvsneslib.bmp
    ASM_SOURCES graphics/Effects/HDMAGradient/data.asm graphics/Effects/HDMAGradient/hdr.asm
    ROM_NAME HDMAGradient
)

add_snes_game(MosaicShading
    SOURCES graphics/Effects/MosaicShading/MosaicShading.c
    GRAPHICS graphics/Effects/MosaicShading/pvsneslib.bmp
    ASM_SOURCES graphics/Effects/MosaicShading/data.asm graphics/Effects/MosaicShading/hdr.asm
    ROM_NAME MosaicShading
)

add_snes_game(ParallaxScrolling
    SOURCES graphics/Effects/ParallaxScrolling/ParallaxScrolling.c
    ASM_SOURCES graphics/Effects/ParallaxScrolling/data.asm graphics/Effects/ParallaxScrolling/hdr.asm
    ROM_NAME ParallaxScrolling
)

add_snes_game(Transparency
    SOURCES graphics/Effects/Transparency/Transparency.c
    GRAPHICS graphics/Effects/Transparency/backgrounds.bmp graphics/Effects/Transparency/clouds.bmp
    ASM_SOURCES graphics/Effects/Transparency/data.asm graphics/Effects/Transparency/hdr.asm
    ROM_NAME Transparency
)

add_snes_game(TransparentWindow
    SOURCES graphics/Effects/TransparentWindow/src/main.c graphics/Effects/TransparentWindow/src/window.c
    GRAPHICS graphics/Effects/TransparentWindow/res/background.png
    ASM_SOURCES graphics/Effects/TransparentWindow/data.asm graphics/Effects/TransparentWindow/hdr.asm
    ROM_NAME TransparentWindow
)

add_snes_game(Waves
    SOURCES graphics/Effects/Waves/Waves.c
    ASM_SOURCES graphics/Effects/Waves/data.asm graphics/Effects/Waves/hdr.asm
    ROM_NAME Waves
)

add_snes_game(Window
    SOURCES graphics/Effects/Window/Window.c
    GRAPHICS graphics/Effects/Window/pvsneslibbg1.png graphics/Effects/Window/pvsneslibbg2.png
    ASM_SOURCES graphics/Effects/Window/data.asm graphics/Effects/Window/hdr.asm
    ROM_NAME Window
)

# Sprite Examples
add_snes_game(AnimatedSprite
    SOURCES graphics/Sprites/AnimatedSprite/AnimatedSprite.c
    GRAPHICS graphics/Sprites/AnimatedSprite/sprites.bmp
    ASM_SOURCES graphics/Sprites/AnimatedSprite/data.asm graphics/Sprites/AnimatedSprite/hdr.asm
    ROM_NAME AnimatedSprite
)

add_snes_game(DynamicEngineMetaSprite
    SOURCES graphics/Sprites/DynamicEngineMetaSprite/DynamicEngineMetaSprite.c
    GRAPHICS graphics/Sprites/DynamicEngineMetaSprite/pvsneslib.png graphics/Sprites/DynamicEngineMetaSprite/sprite16.png graphics/Sprites/DynamicEngineMetaSprite/sprite32.png
    ASM_SOURCES graphics/Sprites/DynamicEngineMetaSprite/data.asm graphics/Sprites/DynamicEngineMetaSprite/hdr.asm
    ROM_NAME DynamicEngineMetaSprite
)

add_snes_game(DynamicEngineSprite
    SOURCES graphics/Sprites/DynamicEngineSprite/DynamicEngineSprite.c
    GRAPHICS graphics/Sprites/DynamicEngineSprite/pvsneslib.png graphics/Sprites/DynamicEngineSprite/sprite16.png graphics/Sprites/DynamicEngineSprite/sprite32.png graphics/Sprites/DynamicEngineSprite/sprite8.png
    ASM_SOURCES graphics/Sprites/DynamicEngineSprite/data.asm graphics/Sprites/DynamicEngineSprite/hdr.asm
    ROM_NAME DynamicEngineSprite
)

add_snes_game(DynamicSprite
    SOURCES graphics/Sprites/DynamicSprite/DynamicSprite.c
    GRAPHICS graphics/Sprites/DynamicSprite/sprites.bmp
    ASM_SOURCES graphics/Sprites/DynamicSprite/data.asm graphics/Sprites/DynamicSprite/hdr.asm
    ROM_NAME DynamicSprite
)

add_snes_game(ObjectSize
    SOURCES graphics/Sprites/ObjectSize/ObjectSize.c
    GRAPHICS graphics/Sprites/ObjectSize/sprite8.bmp graphics/Sprites/ObjectSize/sprite16.bmp graphics/Sprites/ObjectSize/sprite32.bmp graphics/Sprites/ObjectSize/sprite64.bmp graphics/Sprites/ObjectSize/pvsneslibfont.bmp
    ASM_SOURCES graphics/Sprites/ObjectSize/data.asm graphics/Sprites/ObjectSize/hdr.asm
    ROM_NAME ObjectSize
)

add_snes_game(SimpleSprite
    SOURCES graphics/Sprites/SimpleSprite/SimpleSprite.c
    GRAPHICS graphics/Sprites/SimpleSprite/sprites.bmp
    ASM_SOURCES graphics/Sprites/SimpleSprite/data.asm graphics/Sprites/SimpleSprite/hdr.asm
    ROM_NAME SimpleSprite
)

# Palette Example
add_snes_game(GetColors
    SOURCES graphics/Palette/GetColors/GetColors.c
    GRAPHICS graphics/Palette/GetColors/ortf.png
    ASM_SOURCES graphics/Palette/GetColors/data.asm graphics/Palette/GetColors/hdr.asm
    ROM_NAME GetColors
)

#=============================================================================
# GAMES EXAMPLES - Full game implementations
#=============================================================================

add_snes_game(breakout
    SOURCES games/breakout/breakout.c
    ASM_SOURCES games/breakout/data.asm games/breakout/hdr.asm
    DATA_FILES games/breakout/backpal.dat games/breakout/bg1map.dat games/breakout/bg2map.dat games/breakout/palette.dat games/breakout/tiles1.dat games/breakout/tiles2.dat
    ROM_NAME breakout
)

add_snes_game(likemario
    SOURCES games/likemario/LikeMario.c
    GRAPHICS games/likemario/tiles.png games/likemario/mario_sprite.bmp games/likemario/mariofont.bmp
    ASM_SOURCES games/likemario/data.asm games/likemario/hdr.asm
    AUDIO_FILES games/likemario/overworld.it
    SOUNDBANK games/likemario/soundbank
    ROM_NAME likemario
)

#=============================================================================
# INPUT EXAMPLES - Controller, mouse, and input device examples
#=============================================================================

add_snes_game(controller
    SOURCES input/controller/controller.c
    GRAPHICS input/controller/pvsneslibfont.bmp
    ASM_SOURCES input/controller/data.asm input/controller/hdr.asm
    ROM_NAME controller
)

add_snes_game(mouse
    SOURCES input/mouse/mouse.c
    GRAPHICS input/mouse/pvsneslibfont.bmp input/mouse/cursor.png input/mouse/buttons.png
    ASM_SOURCES input/mouse/data.asm input/mouse/hdr.asm
    ROM_NAME mouse
)

add_snes_game(mouse-data-test
    SOURCES input/mouse-data-test/mouse-data-test.c
    GRAPHICS input/mouse-data-test/pvsneslibfont.bmp
    ASM_SOURCES input/mouse-data-test/data.asm input/mouse-data-test/hdr.asm
    ROM_NAME mouse-data-test
)

add_snes_game(multiplay5
    SOURCES input/multiplay5/multiplay5.c
    GRAPHICS input/multiplay5/pvsneslibfont.bmp
    ASM_SOURCES input/multiplay5/data.asm input/multiplay5/hdr.asm
    ROM_NAME multiplay5
)

add_snes_game(superscope
    SOURCES input/superscope/superscope.c
    GRAPHICS input/superscope/aim_adjust_target.png input/superscope/sprites.png input/superscope/pvsneslibfont.bmp
    ASM_SOURCES input/superscope/data.asm input/superscope/hdr.asm
    ROM_NAME superscope
)

#=============================================================================
# LOGO EXAMPLES - Company logo demonstrations
#=============================================================================

add_snes_game(LogoCapcom
    SOURCES logo/snes-logo-capcom/src/main.c logo/snes-logo-capcom/src/logo.c
    GRAPHICS logo/snes-logo-capcom/res/logo.bmp
    ASM_SOURCES logo/snes-logo-capcom/data.asm logo/snes-logo-capcom/hdr.asm
    AUDIO_FILES logo/snes-logo-capcom/res/logo.it
    SOUNDBANK logo/snes-logo-capcom/soundbank
    ROM_NAME LogoCapcom
)

add_snes_game(LogoKonami
    SOURCES logo/snes-logo-konami/src/main.c logo/snes-logo-konami/src/logo.c
    GRAPHICS logo/snes-logo-konami/res/companyFire.bmp logo/snes-logo-konami/res/companyLogo.bmp
    ASM_SOURCES logo/snes-logo-konami/data.asm logo/snes-logo-konami/hdr.asm
    AUDIO_FILES logo/snes-logo-konami/res/logo.it
    SOUNDBANK logo/snes-logo-konami/soundbank
    ROM_NAME LogoKonami
)

add_snes_game(LogoPVSnesLib
    SOURCES logo/snes-logo-pvsneslib/src/main.c logo/snes-logo-pvsneslib/src/logo.c
    GRAPHICS logo/snes-logo-pvsneslib/res/logo.bmp
    ASM_SOURCES logo/snes-logo-pvsneslib/data.asm logo/snes-logo-pvsneslib/hdr.asm
    AUDIO_FILES logo/snes-logo-pvsneslib/res/logo.it
    SOUNDBANK logo/snes-logo-pvsneslib/soundbank
    ROM_NAME LogoPVSnesLib
)

#=============================================================================
# MAPS EXAMPLES - Advanced mapping and scrolling
#=============================================================================

add_snes_game(DynamicMap
    SOURCES maps/DynamicMap/DynamicMap.c maps/DynamicMap/map32x32.c maps/DynamicMap/map64x64.c maps/DynamicMap/maputil.c
    GRAPHICS maps/DynamicMap/pvsneslibfont.bmp maps/DynamicMap/sprite16.bmp maps/DynamicMap/sprite16_64x64.bmp
    ASM_SOURCES maps/DynamicMap/data.asm maps/DynamicMap/hdr.asm maps/DynamicMap/c64_sprite.asm maps/DynamicMap/ram.asm
    ROM_NAME DynamicMap
)

# Special handling for mapbuffer - preprocess ASM file to fix include path
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/mapbufferextensionA_fixed.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/maps/mapbuffer/src/addons/mapbufferextensionA.asm ${CMAKE_CURRENT_BINARY_DIR}/mapbufferextensionA_fixed.asm
    COMMAND ${CMAKE_COMMAND} -DINPUT_FILE="${CMAKE_CURRENT_BINARY_DIR}/mapbufferextensionA_fixed.asm" -DOUTPUT_FILE="${CMAKE_CURRENT_BINARY_DIR}/mapbufferextensionA_fixed.asm" -DPATTERN="\.include \"hdr\.asm\"" -DREPLACEMENT=".include \"../../hdr.asm\"" -P "${CMAKE_SOURCE_DIR}/cmake/replace_text.cmake"
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/maps/mapbuffer/src/addons/mapbufferextensionA.asm
    COMMENT "Preprocessing mapbuffer ASM file to fix include path"
)

add_snes_game(mapbuffer
    SOURCES maps/mapbuffer/mapbuffer.c maps/mapbuffer/src/gameobjects/mario.c maps/mapbuffer/src/addons/mapbufferextension.c
    GRAPHICS maps/mapbuffer/gfx/mario.png maps/mapbuffer/gfx/tilesMario.png
    ASM_SOURCES maps/mapbuffer/data.asm maps/mapbuffer/hdr.asm ${CMAKE_CURRENT_BINARY_DIR}/mapbufferextensionA_fixed.asm
    INCLUDE_DIRS maps/mapbuffer/src/gameobjects maps/mapbuffer/src/addons maps/mapbuffer/maps
    ROM_NAME mapbuffer
)

add_snes_game(mapscroll
    SOURCES maps/mapscroll/mapscroll.c
    GRAPHICS maps/mapscroll/mario.png maps/mapscroll/tilesMario.png maps/mapscroll/bg_L1.png
    ASM_SOURCES maps/mapscroll/data.asm maps/mapscroll/hdr.asm
    ROM_NAME mapscroll
)

add_snes_game(slopemario
    SOURCES maps/slopemario/slopemario.c
    GRAPHICS maps/slopemario/mario_sprite.bmp maps/slopemario/mariofont.bmp maps/slopemario/tiles.png
    ASM_SOURCES maps/slopemario/data.asm maps/slopemario/hdr.asm
    AUDIO_FILES maps/slopemario/overworld.it
    SOUNDBANK maps/slopemario/soundbank
    ROM_NAME slopemario
)

add_snes_game(tiled
    SOURCES maps/tiled/tiled.c
    GRAPHICS maps/tiled/maplevel1.png maps/tiled/tileslevel1.png
    ASM_SOURCES maps/tiled/data.asm maps/tiled/hdr.asm
    ROM_NAME tiled
)

#=============================================================================
# OBJECTS EXAMPLES - Object and sprite management
#=============================================================================

add_snes_game(mapandobjects
    SOURCES objects/mapandobjects/mapandobjects.c objects/mapandobjects/mario.c objects/mapandobjects/goomba.c objects/mapandobjects/koopatroopa.c
    GRAPHICS objects/mapandobjects/mario.png objects/mapandobjects/goomba.png objects/mapandobjects/koopatroopa.png objects/mapandobjects/tileset.png objects/mapandobjects/tilesMario.png objects/mapandobjects/bg_L1.png
    ASM_SOURCES objects/mapandobjects/data.asm objects/mapandobjects/hdr.asm
    ROM_NAME mapandobjects
)

add_snes_game(moveobjects
    SOURCES objects/moveobjects/moveobjects.c
    GRAPHICS objects/moveobjects/sprites.bmp
    ASM_SOURCES objects/moveobjects/data.asm objects/moveobjects/hdr.asm
    ROM_NAME moveobjects
)

add_snes_game(nogravityobjects
    SOURCES objects/nogravityobject/nogravityobjects.c objects/nogravityobject/link.c
    GRAPHICS objects/nogravityobject/link.png objects/nogravityobject/mapzelda.png objects/nogravityobject/tiles.png
    ASM_SOURCES objects/nogravityobject/data.asm objects/nogravityobject/hdr.asm
    ROM_NAME nogravityobjects
)

#=============================================================================
# SRAM EXAMPLES - Save game and persistent storage
#=============================================================================

add_snes_game(sramoffset
    SOURCES sram/sramoffset/sramoffset.c
    GRAPHICS sram/sramoffset/pvsneslibfont.bmp
    ASM_SOURCES sram/sramoffset/data.asm sram/sramoffset/hdr.asm
    ROM_NAME sramoffset
)

add_snes_game(sramsimple
    SOURCES sram/sramsimple/sram.c
    GRAPHICS sram/sramsimple/pvsneslibfont.bmp
    ASM_SOURCES sram/sramsimple/data.asm sram/sramsimple/hdr.asm
    ROM_NAME sramsimple
)

# Combined target for basic working examples 
add_custom_target(examples
    DEPENDS 
        hello_world 
        breakpoints 
        debug 
        random 
        scoring 
        timer 
        testregion
        typeconsole
    COMMENT "Building basic working SNES examples"
)

# Advanced examples target (includes examples that need special ROM configurations)
add_custom_target(advanced_examples
    DEPENDS
        memory_mapping
    COMMENT "Building advanced SNES examples (HiROM/FastROM)"
)

# Audio examples target
add_custom_target(audio_examples
    DEPENDS
        effects
        effectsandmusic
        music
        music2
        musicGreaterThan32k
        musicHiROM
        tada
    COMMENT "Building audio SNES examples"
)

# Graphics examples target
add_custom_target(graphics_examples
    DEPENDS
        Mode0 Mode1 Mode1BG3HighPriority Mode1ContinuousScroll Mode1LZ77 
        Mode1MixedScroll Mode1Png Mode1Scroll Mode3 Mode5 Mode7 Mode7Perspective
        Fading GradientColors HDMAGradient MosaicShading ParallaxScrolling
        Transparency TransparentWindow Waves Window
        AnimatedSprite DynamicEngineMetaSprite DynamicEngineSprite DynamicSprite
        ObjectSize SimpleSprite GetColors
    COMMENT "Building graphics SNES examples"
)

# Games examples target
add_custom_target(games_examples
    DEPENDS
        breakout
        likemario
    COMMENT "Building game SNES examples"
)

# Input examples target
add_custom_target(input_examples
    DEPENDS
        controller
        mouse
        mouse-data-test
        multiplay5
        superscope
    COMMENT "Building input SNES examples"
)

# Logo examples target
add_custom_target(logo_examples
    DEPENDS
        LogoCapcom
        LogoKonami
        LogoPVSnesLib
    COMMENT "Building logo SNES examples"
)

# Maps examples target
add_custom_target(maps_examples
    DEPENDS
        DynamicMap
        mapbuffer
        mapscroll
        slopemario
        tiled
    COMMENT "Building maps SNES examples"
)

# Objects examples target
add_custom_target(objects_examples
    DEPENDS
        mapandobjects
        moveobjects
        nogravityobjects
    COMMENT "Building objects SNES examples"
)

# SRAM examples target
add_custom_target(sram_examples
    DEPENDS
        sramoffset
        sramsimple
    COMMENT "Building SRAM SNES examples"
)

# Comprehensive target for ALL examples (60+ examples!)
add_custom_target(all_examples
    DEPENDS 
        examples
        advanced_examples
        audio_examples
        graphics_examples
        games_examples
        input_examples
        logo_examples
        maps_examples
        objects_examples
        sram_examples
    COMMENT "Building ALL SNES examples (60+ examples!)"
)

message(STATUS "SNES examples configured. Use 'cmake --build . --target examples' to build all or 'cmake --build . --target hello_world' for individual games.")
