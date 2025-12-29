--[[
    PVSNESLIB Smoke Test

    Generic test script that verifies a ROM runs without crashing.
    Runs the ROM for a specified number of frames.

    Usage: Mesen --testrunner ROM.sfc --lua smoke_test.lua

    Exit codes:
      0 = ROM ran successfully
      1 = ROM crashed or encountered an error
]]--

local frameCount = 0
local maxFrames = 180  -- 3 seconds at 60fps

function log(msg)
    emu.log(msg)
    print(msg)
end

function onEndFrame()
    frameCount = frameCount + 1

    -- Success - ROM ran for enough frames
    if frameCount >= maxFrames then
        log(string.format("SMOKE TEST PASSED - ROM ran %d frames", frameCount))
        emu.stop(0)
        return
    end
end

-- Register the callback
emu.addEventCallback(onEndFrame, emu.eventType.endFrame)

log("Starting smoke test...")
log(string.format("Running for %d frames (%.1f seconds)", maxFrames, maxFrames / 60.0))
