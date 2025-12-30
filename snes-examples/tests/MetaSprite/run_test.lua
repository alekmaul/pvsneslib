--[[
    PVSNESLIB MetaSprite Validation Test

    Validates that the MetaSprite example correctly renders 16x16 metasprites
    with proper VRAM-layout-aware tile indices.

    This test verifies the fix for the bug where 16x16 character metasprites
    rendered with head below feet due to incorrect tile ordering.

    Tests:
    - 32x32 metasprite at (16, 140): 4 sub-sprites (2x2)
    - 16x16 metasprite at (144, 140): 6 sub-sprites (2x3)
    - Tile indices follow VRAM layout (8 sprites per row for 16x16)

    Usage: Mesen --testrunner MetaSprite.sfc --lua run_test.lua
]]--

local frameCount = 0
local maxFrames = 120

local testsPassed = 0
local testsFailed = 0
local totalTests = 0

function log(msg)
    emu.log(msg)
    print(msg)
end

function readVRAM8(byteAddr)
    return emu.read(byteAddr, emu.memType.snesVideoRam)
end

function readCGRAM(addr)
    return emu.read(addr, emu.memType.snesCgRam)
end

function readOAM(addr)
    return emu.read(addr, emu.memType.snesSpriteRam)
end

function check(condition, name, details)
    totalTests = totalTests + 1
    if condition then
        testsPassed = testsPassed + 1
        log(string.format("  [PASS] %s", name))
        return true
    else
        testsFailed = testsFailed + 1
        log(string.format("  [FAIL] %s", name))
        if details then
            log(string.format("         %s", details))
        end
        return false
    end
end

function getOAMEntry(spriteId)
    -- OAM low table: 4 bytes per sprite (X-low, Y, tile, attributes)
    local baseAddr = spriteId * 4
    return {
        x = readOAM(baseAddr),
        y = readOAM(baseAddr + 1),
        tile = readOAM(baseAddr + 2),
        attr = readOAM(baseAddr + 3)
    }
end

function runTests()
    log("")
    log("=== METASPRITE VALIDATION TEST ===")
    log("")

    -- Test 1: Check 32x32 sprite tiles loaded to VRAM at 0x0000
    log("-- 32x32 Sprite VRAM Check --")
    local sprite32Sum = 0
    for i = 0, 255 do
        sprite32Sum = sprite32Sum + readVRAM8(i)
    end
    check(sprite32Sum > 0, "32x32 sprite tiles loaded to VRAM $0000",
        string.format("Tile data sum: %d", sprite32Sum))

    -- Test 2: Check 16x16 sprite tiles loaded to VRAM at 0x1000
    log("-- 16x16 Sprite VRAM Check --")
    local sprite16Sum = 0
    for i = 0x2000, 0x2100 do  -- VRAM word address $1000 = byte address $2000
        sprite16Sum = sprite16Sum + readVRAM8(i)
    end
    check(sprite16Sum > 0, "16x16 sprite tiles loaded to VRAM $1000",
        string.format("Tile data sum at $1000: %d", sprite16Sum))

    -- Test 3: Check sprite palette loaded to CGRAM
    log("-- Palette Check --")
    local palOffset = 256  -- First sprite palette
    local palSum = 0
    for i = 0, 31 do
        palSum = palSum + readCGRAM(palOffset + i)
    end
    check(palSum > 0, "Sprite palette loaded to CGRAM",
        string.format("Palette data sum at offset %d: %d", palOffset, palSum))

    -- Test 4: Check 32x32 metasprite OAM entries (ID 0, 4 sub-sprites)
    -- oamMetaDraw32(0, 16, 140, ...) creates sprites at OAM entries 0-3
    log("-- 32x32 Metasprite OAM Check --")
    local baseX32 = 16
    local baseY32 = 140
    local sprite32Offsets = {
        {x = 0, y = 0},
        {x = 32, y = 0},
        {x = 0, y = 32},
        {x = 32, y = 32}
    }

    for i = 1, 4 do
        local oam = getOAMEntry(i - 1)
        local expectedX = baseX32 + sprite32Offsets[i].x
        local expectedY = baseY32 + sprite32Offsets[i].y

        check(oam.x == expectedX,
            string.format("32x32 sub-sprite %d X position", i),
            string.format("X = %d (expected %d)", oam.x, expectedX))
        check(oam.y == expectedY,
            string.format("32x32 sub-sprite %d Y position", i),
            string.format("Y = %d (expected %d)", oam.y, expectedY))
    end

    -- Test 5: Check 16x16 metasprite OAM entries (ID 8, 6 sub-sprites at 2x3 layout)
    -- oamMetaDraw16(8, 144, 140, ...) creates sprites at OAM entries 8-13
    -- The 16x16 metasprite is 2 wide x 3 tall (32x48 pixels)
    log("-- 16x16 Metasprite OAM Check --")
    local baseX16 = 144
    local baseY16 = 140
    local sprite16Offsets = {
        {x = 0, y = 0},      -- Top-left
        {x = 16, y = 0},     -- Top-right
        {x = 0, y = 16},     -- Middle-left
        {x = 16, y = 16},    -- Middle-right
        {x = 0, y = 32},     -- Bottom-left
        {x = 16, y = 32}     -- Bottom-right
    }

    -- Expected tile indices after the fix (VRAM-layout-aware)
    -- For lkup16idT (16x16 small sprites), index maps to character name
    -- Index 0 -> $0100, Index 1 -> $0102, Index 8 -> $0120, etc.
    -- The low byte of the character name is stored in OAM tile field
    local expectedTiles = {0x00, 0x02, 0x20, 0x22, 0x40, 0x42}

    for i = 1, 6 do
        local oam = getOAMEntry(8 + i - 1)  -- Starting at OAM entry 8
        local expectedX = baseX16 + sprite16Offsets[i].x
        local expectedY = baseY16 + sprite16Offsets[i].y

        check(oam.x == expectedX,
            string.format("16x16 sub-sprite %d X position", i),
            string.format("X = %d (expected %d)", oam.x, expectedX))
        check(oam.y == expectedY,
            string.format("16x16 sub-sprite %d Y position", i),
            string.format("Y = %d (expected %d)", oam.y, expectedY))

        -- Check tile index (validates the VRAM-layout-aware fix)
        check(oam.tile == expectedTiles[i],
            string.format("16x16 sub-sprite %d tile index (VRAM layout)", i),
            string.format("Tile = $%02X (expected $%02X)", oam.tile, expectedTiles[i]))
    end

    -- Test 6: Verify 16x16 sprite priorities (should be 2 from metasprite data)
    log("-- 16x16 Metasprite Priority Check --")
    for i = 1, 6 do
        local oam = getOAMEntry(8 + i - 1)
        local priority = (oam.attr >> 4) & 3
        check(priority == 2,
            string.format("16x16 sub-sprite %d priority", i),
            string.format("Priority = %d (expected 2)", priority))
    end

    -- Summary
    log("")
    log("=== SUMMARY ===")
    log(string.format("Total:  %d tests", totalTests))
    log(string.format("Passed: %d", testsPassed))
    log(string.format("Failed: %d", testsFailed))
    log("")

    if testsFailed == 0 then
        log("RESULT: ALL TESTS PASSED")
        log("")
        log("The 16x16 metasprite tile ordering fix is verified!")
        log("Tiles use VRAM-layout-aware indices (0,1,8,9,16,17 -> $00,$02,$20,$22,$40,$42)")
        emu.stop(0)
    else
        log("RESULT: SOME TESTS FAILED")
        emu.stop(1)
    end
end

function onEndFrame()
    frameCount = frameCount + 1
    if frameCount < 60 then return end
    if frameCount == 60 then
        runTests()
        return
    end
    if frameCount >= maxFrames then
        log("TIMEOUT")
        emu.stop(2)
    end
end

emu.addEventCallback(onEndFrame, emu.eventType.endFrame)
log("Starting MetaSprite validation test...")
log("This test validates the 16x16 metasprite tile ordering fix.")
