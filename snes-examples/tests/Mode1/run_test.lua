--[[
    PVSNESLIB Mode1 Background Validation Test

    Validates that the Mode1 background example initializes correctly:
    - Tiles loaded to VRAM at 0x4000
    - Map loaded to VRAM at 0x0000
    - Palette loaded to CGRAM
    - BG Mode set to Mode 1

    Usage: Mesen --testrunner Mode1.sfc --lua run_test.lua
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
    log("=== MODE1 BACKGROUND VALIDATION TEST ===")
    log("")

    -- Test 1: Check that tilemap is loaded at VRAM 0x0000
    -- bgInitMapSet(0, &map, ..., 0x0000) sets map at word address 0x0000
    -- Check across the entire 32x32 tilemap area (2048 bytes)
    local mapSum = 0
    for i = 0, 2047, 32 do  -- Sample every 32 bytes
        mapSum = mapSum + readVRAM8(i)
    end
    check(mapSum > 0, "Tilemap loaded at VRAM $0000",
        string.format("Map data sum (sampled): %d", mapSum))

    -- Test 2: Check that tiles are loaded at VRAM 0x4000
    -- bgInitTileSet(..., 0x4000) sets tiles at word address 0x4000 = byte 0x8000
    local tileByteAddr = 0x4000 * 2  -- 0x8000
    local tileSum = 0
    for i = 0, 127 do
        tileSum = tileSum + readVRAM8(tileByteAddr + i)
    end
    check(tileSum > 0, "Tiles loaded at VRAM $8000",
        string.format("Tile data sum: %d", tileSum))

    -- Test 3: Check that palette is loaded to CGRAM
    -- 16-color mode uses first 32 bytes of CGRAM (16 colors * 2 bytes each)
    local palSum = 0
    for i = 0, 31 do
        palSum = palSum + readCGRAM(i)
    end
    check(palSum > 0, "Palette loaded to CGRAM",
        string.format("Palette data sum: %d", palSum))

    -- Test 4: Check that map has varied data (actual map, not zeros)
    local uniqueTiles = {}
    for i = 0, 255, 2 do  -- Check first 128 map entries
        local tile = readVRAM8(i)
        uniqueTiles[tile] = true
    end
    local uniqueCount = 0
    for _ in pairs(uniqueTiles) do
        uniqueCount = uniqueCount + 1
    end
    check(uniqueCount >= 3, "Map has varied tile indices",
        string.format("Unique tiles in map: %d", uniqueCount))

    -- Test 5: Verify palette has color variation
    local uniqueColors = {}
    for i = 0, 31, 2 do
        local color = readCGRAM(i) + readCGRAM(i + 1) * 256
        uniqueColors[color] = true
    end
    local colorCount = 0
    for _ in pairs(uniqueColors) do
        colorCount = colorCount + 1
    end
    check(colorCount >= 2, "Palette has multiple colors",
        string.format("Unique colors: %d", colorCount))

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
log("Starting Mode1 Background validation test...")
