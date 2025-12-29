--[[
    PVSNESLIB Music Validation Test

    Validates that the music example initializes correctly:
    - SPC engine initialized and running
    - SPC RAM has music data loaded
    - Text is rendered to screen
    - Background color is animating

    Usage: Mesen --testrunner music.sfc --lua run_test.lua
]]--

local frameCount = 0
local maxFrames = 180  -- 3 seconds for music to initialize

local testsPassed = 0
local testsFailed = 0
local totalTests = 0

-- Track palette changes over time
local initialPalette = nil
local paletteChanged = false

function log(msg)
    emu.log(msg)
    print(msg)
end

function readVRAM8(byteAddr)
    return emu.read(byteAddr, emu.memType.snesVideoRam)
end

function readSPC(addr)
    return emu.read(addr, emu.memType.spcRam)
end

function readCGRAM(addr)
    return emu.read(addr, emu.memType.snesCgRam)
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
    log("=== MUSIC VALIDATION TEST ===")
    log("")

    -- Test 1: Check SPC RAM has data (driver + music loaded)
    -- The SPC driver and music data should be in SPC RAM
    local spcSum = 0
    for i = 0x200, 0x2FF do  -- Check a range in upper memory
        spcSum = spcSum + readSPC(i)
    end
    check(spcSum > 0, "SPC driver/data loaded to APU RAM",
        string.format("SPC RAM data sum: %d", spcSum))

    -- Test 2: Check text rendered to VRAM
    -- "Let's the music play !" at position (5, 10)
    local tilemapBase = 0x6800 * 2  -- 0xD000
    local textOffset = (10 * 32 + 5) * 2  -- Row 10, col 5
    local firstTile = readVRAM8(tilemapBase + textOffset)
    check(firstTile > 0, "Text rendered to tilemap",
        string.format("Tile at (5,10): $%02X", firstTile))

    -- Test 3: Check font loaded
    local fontBase = 0x3000 * 2  -- 0x6000
    local lTileOffset = 0x2C * 32  -- 'L' tile
    local fontSum = 0
    for i = 0, 15 do
        fontSum = fontSum + readVRAM8(fontBase + lTileOffset + i)
    end
    check(fontSum > 0, "Font tiles loaded to VRAM",
        string.format("Font 'L' data sum: %d", fontSum))

    -- Test 4: Check palette animation (color 0 changes)
    check(paletteChanged, "Background color is animating",
        "Palette color 0 changed over time")

    -- Test 5: Check SPC has varied data (actual music, not just zeros)
    local uniqueBytes = {}
    for i = 0, 255 do
        local val = readSPC(0x300 + i)
        uniqueBytes[val] = true
    end
    local uniqueCount = 0
    for _ in pairs(uniqueBytes) do
        uniqueCount = uniqueCount + 1
    end
    check(uniqueCount >= 5, "SPC RAM has varied music data",
        string.format("Unique bytes in SPC RAM: %d", uniqueCount))

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

    -- Track palette color 0 changes
    local currentPal = readCGRAM(0) + readCGRAM(1) * 256
    if frameCount == 30 then
        initialPalette = currentPal
    elseif frameCount > 60 and initialPalette then
        if currentPal ~= initialPalette then
            paletteChanged = true
        end
    end

    if frameCount < 90 then return end
    if frameCount == 90 then
        runTests()
        return
    end
    if frameCount >= maxFrames then
        log("TIMEOUT")
        emu.stop(2)
    end
end

emu.addEventCallback(onEndFrame, emu.eventType.endFrame)
log("Starting Music validation test...")
log("Waiting for SPC boot and music load (90 frames)...")
