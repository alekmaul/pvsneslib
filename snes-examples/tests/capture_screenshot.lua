--[[
    PVSNESLIB Screenshot Capture

    Captures a screenshot after the ROM has initialized and saves it to disk.

    Usage: Mesen --testrunner ROM.sfc --lua capture_screenshot.lua

    The screenshot will be saved to .mesen_screenshots/screenshot.png
]]--

local frameCount = 0
local captureFrame = 120  -- Capture after 2 seconds (enough for initialization)
local screenshotTaken = false

-- Get the script directory to determine output path
local scriptPath = emu.getScriptDataFolder()
local outputDir = "/Users/k0b3/workspaces/github/pvsneslib/snes-examples/tests/.mesen_screenshots"
local outputFile = outputDir .. "/screenshot.png"

function log(msg)
    emu.log(msg)
    print(msg)
end

function onEndFrame()
    frameCount = frameCount + 1

    if frameCount == captureFrame and not screenshotTaken then
        screenshotTaken = true
        log(string.format("Capturing screenshot at frame %d...", frameCount))

        -- takeScreenshot() returns PNG data as binary string
        local pngData = emu.takeScreenshot()

        -- Write to file
        local file = io.open(outputFile, "wb")
        if file then
            file:write(pngData)
            file:close()
            log(string.format("Screenshot saved to: %s", outputFile))
            emu.stop(0)
        else
            log("ERROR: Could not open output file")
            emu.stop(1)
        end
    end

    -- Timeout safety
    if frameCount > captureFrame + 60 then
        log("Timeout waiting for screenshot")
        emu.stop(1)
    end
end

emu.addEventCallback(onEndFrame, emu.eventType.endFrame)
log("Screenshot capture script loaded")
log(string.format("Will capture at frame %d and save to %s", captureFrame, outputFile))
