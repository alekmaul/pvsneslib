--[[
    PVSNESLIB SimpleSprite Validation Test

    Validates that the SimpleSprite example initializes correctly:
    - Sprite tiles loaded to VRAM at 0x0000
    - Sprite palette loaded to CGRAM (OBJ palette at offset 256)
    - OAM configured with sprite at position (100, 100)

    Usage: Mesen --testrunner SimpleSprite.sfc --lua run_test.lua
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

function runTests()
    log("")
    log("=== SIMPLESPRITE VALIDATION TEST ===")
    log("")

    -- Test 1: Check sprite graphics loaded to VRAM at 0x0000
    -- oamInitGfxSet(..., 0x0000, ...) loads sprite tiles at word address 0x0000
    local spriteSum = 0
    for i = 0, 127 do
        spriteSum = spriteSum + readVRAM8(i)
    end
    check(spriteSum > 0, "Sprite tiles loaded to VRAM $0000",
        string.format("Tile data sum: %d", spriteSum))

    -- Test 2: Check sprite palette loaded to CGRAM
    -- OBJ palette starts at CGRAM offset 256 (128 colors * 2 bytes for BG)
    local palOffset = 256  -- First sprite palette
    local palSum = 0
    for i = 0, 31 do
        palSum = palSum + readCGRAM(palOffset + i)
    end
    check(palSum > 0, "Sprite palette loaded to CGRAM",
        string.format("Palette data sum at offset %d: %d", palOffset, palSum))

    -- Test 3: Check OAM has sprite data
    -- OAM low table: 4 bytes per sprite (X-low, Y, tile, attributes)
    -- Sprite 0 should be at (100, 100)
    local oamX = readOAM(0)  -- X position low byte
    local oamY = readOAM(1)  -- Y position
    local oamTile = readOAM(2)  -- Tile number
    local oamAttr = readOAM(3)  -- Attributes

    check(oamX == 100, "Sprite X position is 100",
        string.format("X = %d (expected 100)", oamX))

    check(oamY == 100, "Sprite Y position is 100",
        string.format("Y = %d (expected 100)", oamY))

    -- Test 4: Check sprite tile index (should be 0)
    check(oamTile == 0, "Sprite tile index is 0",
        string.format("Tile = %d (expected 0)", oamTile))

    -- Test 5: Check OAM high table (sprite visible)
    -- High table at offset 512, 2 bits per sprite
    -- Bit 0 = X-high, Bit 1 = size (0=small, 1=large)
    local oamHigh = readOAM(512)
    -- For sprite 0, bits 0-1 of byte 512
    -- OBJ_SMALL means size bit = 0, OBJ_SHOW means visible
    log(string.format("  OAM high byte: $%02X", oamHigh))

    -- Test 6: Verify sprite has non-zero attributes (priority)
    -- oamSet sets priority 3, which means bits 4-5 of attributes = 11
    local priority = (oamAttr >> 4) & 3
    check(priority == 3, "Sprite priority is 3",
        string.format("Priority = %d (expected 3)", priority))

    -- Summary
    log("")
    log("=== SUMMARY ===")
    log(string.format("Total:  %d tests", totalTests))
    log(string.format("Passed: %d", testsPassed))
    log(string.format("Failed: %d", testsFailed))
    log("")

    if testsFailed == 0 then
        log("RESULT: ALL TESTS PASSED")
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
log("Starting SimpleSprite validation test...")
