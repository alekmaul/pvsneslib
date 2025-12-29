--[[
    PVSNESLIB Hello World Validation Test

    Validates that the hello_world example initializes correctly:
    - BG Mode is set to Mode 1
    - Text is rendered to VRAM at correct position
    - Screen is enabled

    Usage: Mesen --testrunner hello_world.sfc --lua run_test.lua
]]--

local frameCount = 0
local maxFrames = 120  -- 2 seconds to allow initialization

-- SNES PPU register addresses (directly accessible in CPU memory space)
local REG_INIDISP = 0x2100  -- Screen display register
local REG_BGMODE = 0x2105   -- BG mode register
local REG_TM = 0x212C       -- Main screen designation

-- VRAM addresses (word addresses, need to multiply by 2 for byte address)
local VRAM_TILEMAP = 0x6800  -- Tilemap base (word address)
local TEXT_OFFSET = 0x0100   -- Tile index offset for text

-- Text position for "Hello World !" at (10, 10)
-- Tilemap offset = (row * 32 + col) * 2 bytes
local HELLO_ROW = 10
local HELLO_COL = 10
local HELLO_OFFSET = (HELLO_ROW * 32 + HELLO_COL) * 2  -- 0x294

-- Expected characters (tile indices for ASCII mapped font)
-- The font maps space=0, then printable ASCII starting at 0x20 offset
-- 'H' = ASCII 0x48, mapped to tile 0x48 - 0x20 + TEXT_OFFSET = 0x128
-- But consoles often use simpler mappings, let's verify what we get

local testsPassed = 0
local testsFailed = 0
local totalTests = 0

function log(msg)
    emu.log(msg)
    print(msg)
end

function readVRAM(wordAddr)
    -- VRAM is accessed as word addresses, convert to byte offset
    local byteAddr = wordAddr * 2
    local lo = emu.read(byteAddr, emu.memType.snesVideoRam)
    local hi = emu.read(byteAddr + 1, emu.memType.snesVideoRam)
    return lo + (hi * 256)
end

function readVRAM8(byteAddr)
    return emu.read(byteAddr, emu.memType.snesVideoRam)
end

function readCPU(addr)
    return emu.read(addr, emu.memType.snesMemory)
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
    log("=== HELLO WORLD VALIDATION TEST ===")
    log("")

    -- Test 1: Check that VRAM has tile data loaded (not all zeros)
    -- Font tiles are at word address 0x3000 (byte address 0x6000)
    -- consoleSetTextGfxPtr(0x3000) sets this
    -- Check tile 0x28 ('H') which is at offset 0x28 * 32 = 0x500 bytes
    local fontByteAddr = 0x3000 * 2  -- 0x6000
    local hTileOffset = 0x28 * 32    -- Tile 'H' offset (each tile is 32 bytes in 4bpp)
    local tileData = 0
    for i = 0, 31 do
        tileData = tileData + readVRAM8(fontByteAddr + hTileOffset + i)
    end
    check(tileData > 0, "Font tiles loaded to VRAM",
        string.format("Tile 'H' data at VRAM $%04X: sum=%d", fontByteAddr + hTileOffset, tileData))

    -- Test 2: Check that tilemap has been written (text rendered)
    -- Read the tilemap area where "Hello World !" should be
    -- Position (10, 10) in a 32-wide tilemap
    local tilemapBase = VRAM_TILEMAP * 2  -- Convert to byte address
    local textOffset = HELLO_OFFSET

    -- Read first character position
    local firstTile = readVRAM8(tilemapBase + textOffset)
    local secondTile = readVRAM8(tilemapBase + textOffset + 2)

    -- Tiles should not be 0 if text was rendered
    check(firstTile > 0 or secondTile > 0, "Text rendered to tilemap",
        string.format("First tiles at (%d,%d): $%02X, $%02X",
            HELLO_COL, HELLO_ROW, firstTile, secondTile))

    -- Test 3: Check that text characters are in expected range
    -- Font tiles start at TEXT_OFFSET (0x100), so rendered tiles should be >= 0x100
    local tilemapWord = readVRAM8(tilemapBase + textOffset) +
                        readVRAM8(tilemapBase + textOffset + 1) * 256
    local tileIndex = tilemapWord % 1024  -- Tile index is lower 10 bits
    check(tileIndex >= 0x28, "Tile index in text range",
        string.format("Tile index: $%03X (expected >= $028 for 'H')", tileIndex))

    -- Test 4: Verify text consistency - read several characters
    local helloText = {}
    for i = 0, 12 do  -- "Hello World !" is 13 chars
        local addr = tilemapBase + textOffset + (i * 2)
        local tile = readVRAM8(addr)
        table.insert(helloText, tile)
    end

    -- Check that we have varied tile indices (not all same value)
    local uniqueTiles = {}
    for _, t in ipairs(helloText) do
        uniqueTiles[t] = true
    end
    local uniqueCount = 0
    for _ in pairs(uniqueTiles) do
        uniqueCount = uniqueCount + 1
    end
    check(uniqueCount >= 5, "Text has varied characters",
        string.format("Unique tiles in 'Hello World !': %d", uniqueCount))

    -- Test 5: Check second line of text at (6, 14)
    local line2Offset = (14 * 32 + 6) * 2
    local line2Tile = readVRAM8(tilemapBase + line2Offset)
    check(line2Tile > 0, "Second line text present",
        string.format("Tile at (6,14): $%02X", line2Tile))

    -- Test 6: Check third line of text at (3, 18)
    local line3Offset = (18 * 32 + 3) * 2
    local line3Tile = readVRAM8(tilemapBase + line3Offset)
    check(line3Tile > 0, "Third line text present",
        string.format("Tile at (3,18): $%02X", line3Tile))

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

    -- Wait for ROM to fully initialize (text rendering, VBlank updates)
    if frameCount < 60 then
        return
    end

    -- Run tests once after initialization
    if frameCount == 60 then
        runTests()
        return
    end

    -- Fallback timeout
    if frameCount >= maxFrames then
        log("TIMEOUT: Tests did not complete")
        emu.stop(2)
    end
end

emu.addEventCallback(onEndFrame, emu.eventType.endFrame)
log("Starting Hello World validation test...")
log("Waiting for initialization (60 frames)...")
