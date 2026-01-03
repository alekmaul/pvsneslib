--[[
    PVSNESLIB Background Initialization Test Runner

    This Lua script runs the background init test ROM in Mesen2 and checks results.
    Tests verify that bgSetEnable and bgInitTileSet properly initialize bkgrd_val1.

    Usage: Run Mesen2 with this script:
      Mesen --testrunner test_background.sfc --lua run_test.lua

    Exit codes:
      0 = All tests passed
      1 = Some tests failed
      2 = Timeout (tests didn't complete)
]]--

local frameCount = 0
local maxFrames = 300  -- 5 seconds at 60fps

-- Memory addresses (from .sym file - run 'grep test_ test_background.sym' to verify)
local ADDR_STATUS = 0x7E2008
local ADDR_TOTAL = 0x7E200A
local ADDR_PASSED = 0x7E200C
local ADDR_FAILED = 0x7E200E

local STATUS_RUNNING = 0x0000
local STATUS_PASSED = 0x0001
local STATUS_FAILED = 0x00FF

-- Test names
local testNames = {
    [0] = "bgSetEnable(BG0) clears high byte",
    [1] = "bgSetEnable(BG1) clears high byte",
    [2] = "bgSetEnable(BG2) clears high byte",
    [3] = "bgSetEnable(BG3) clears high byte",
    [4] = "bgInitTileSet(paletteEntry=0) clears high byte",
    [5] = "bgInitTileSet(paletteEntry=1) clears high byte",
}

function log(msg)
    emu.log(msg)
    print(msg)
end

function read16(addr)
    local lo = emu.read(addr, emu.memType.cpu)
    local hi = emu.read(addr + 1, emu.memType.cpu)
    return lo + (hi * 256)
end

function onEndFrame()
    frameCount = frameCount + 1

    -- Wait for ROM to initialize
    if frameCount < 60 then
        return
    end

    local status = read16(ADDR_STATUS)
    local total = read16(ADDR_TOTAL)
    local passed = read16(ADDR_PASSED)
    local failed = read16(ADDR_FAILED)

    -- Check if tests completed
    if status == STATUS_PASSED or status == STATUS_FAILED then
        log("=== PVSNESLIB BACKGROUND INIT TEST SUITE ===")
        log("")
        log(string.format("Total:  %d tests", total))
        log(string.format("Passed: %d", passed))
        log(string.format("Failed: %d", failed))
        log("")

        if status == STATUS_PASSED then
            log("RESULT: ALL TESTS PASSED")
            emu.stop(0)
        else
            log("RESULT: SOME TESTS FAILED")
            emu.stop(1)
        end
        return
    end

    -- Timeout check
    if frameCount >= maxFrames then
        log("=== PVSNESLIB BACKGROUND INIT TEST SUITE ===")
        log("")
        log(string.format("TIMEOUT: Tests did not complete after %d frames", maxFrames))
        log(string.format("Status: 0x%04X, Total: %d, Passed: %d, Failed: %d",
            status, total, passed, failed))
        emu.stop(2)
        return
    end
end

emu.addEventCallback(onEndFrame, emu.eventType.endFrame)
log("Starting PVSNESLIB Background Initialization Test Suite...")
log("Waiting for test ROM to complete...")
