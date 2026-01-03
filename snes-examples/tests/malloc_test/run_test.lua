--[[
    PVSNESLIB Malloc Test Runner

    Validates malloc() works in small projects (Issue #311)

    This Lua script runs the malloc test ROM in Mesen2 and checks results.

    Usage: Run Mesen2 with this script:
      Mesen --testrunner malloc_test.sfc --lua run_test.lua

    Exit codes:
      0 = All tests passed
      1 = Some tests failed
      2 = Timeout (tests didn't complete)
]]--

local frameCount = 0
local maxFrames = 300  -- 5 seconds at 60fps

-- Memory addresses (from .sym file - run 'grep test_ malloc_test.sym' to verify)
local ADDR_STATUS = 0x7E2008
local ADDR_TOTAL = 0x7E200A
local ADDR_PASSED = 0x7E200C
local ADDR_FAILED = 0x7E200E

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
    if frameCount < 30 then
        return
    end

    local status = read16(ADDR_STATUS)
    local total = read16(ADDR_TOTAL)
    local passed = read16(ADDR_PASSED)
    local failed = read16(ADDR_FAILED)

    -- Check if tests completed (status == 1)
    if status == 1 then
        log("=== Malloc Test Results ===")
        log(string.format("Total tests: %d", total))
        log(string.format("Passed: %d", passed))
        log(string.format("Failed: %d", failed))
        log("")

        if failed == 0 and total > 0 then
            log("MALLOC TEST: PASSED")
            emu.stop(0)
        else
            log("MALLOC TEST: FAILED")
            emu.stop(1)
        end
        return
    end

    -- Timeout check
    if frameCount >= maxFrames then
        log("=== Malloc Test Results ===")
        log(string.format("TIMEOUT: Tests did not complete after %d frames", maxFrames))
        log(string.format("Status: %d, Total: %d, Passed: %d, Failed: %d",
            status, total, passed, failed))
        log("Test did not complete - malloc may have failed")
        emu.stop(2)
        return
    end
end

emu.addEventCallback(onEndFrame, emu.eventType.endFrame)
log("Starting Malloc test - monitoring for up to " .. maxFrames .. " frames...")
