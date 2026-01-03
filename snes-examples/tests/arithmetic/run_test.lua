--[[
    PVSNESLIB Arithmetic Test Runner

    This Lua script runs the arithmetic test ROM in Mesen2 and checks results.

    Usage: Run Mesen2 with this script:
      Mesen --testrunner test_arithmetic.sfc --lua run_test.lua

    Exit codes:
      0 = All tests passed
      1 = Some tests failed
      2 = Timeout (tests didn't complete)
]]--

local frameCount = 0
local maxFrames = 600  -- 10 seconds at 60fps

-- Memory addresses (from .sym file)
local ADDR_STATUS = 0x7E2000
local ADDR_TOTAL = 0x7E2002
local ADDR_PASSED = 0x7E2004
local ADDR_FAILED = 0x7E2006
local ADDR_RESULTS = 0x7E200C    -- u8 array[64]
local ADDR_EXPECTED = 0x7E204C   -- s16 array[64]
local ADDR_ACTUAL = 0x7E20CC     -- s16 array[64]

local STATUS_RUNNING = 0x0000
local STATUS_PASSED = 0x0001
local STATUS_FAILED = 0x00FF

-- Test names matching the order in test_arithmetic.c
local testNames = {
    -- test_addition (4 tests: 0-3)
    [0] = "ADD: 5+3=8",
    [1] = "ADD: 100+0=100",
    [2] = "ADD: -10+3=-7",
    [3] = "ADD: 10000+20000=30000",

    -- test_subtraction (3 tests: 4-6)
    [4] = "SUB: 10-3=7",
    [5] = "SUB: 5-10=-5",
    [6] = "SUB: -10-(-3)=-7",

    -- test_multiplication (7 tests: 7-13)
    [7] = "MUL: 6*7=42",
    [8] = "MUL: 100*0=0",
    [9] = "MUL: 42*1=42",
    [10] = "MUL: -5*7=-35",
    [11] = "MUL: -4*-8=32",
    [12] = "MUL: 100*100=10000",
    [13] = "MUL: 181*181=32761",

    -- test_division (4 tests: 14-17)
    [14] = "DIV: 42/6=7",
    [15] = "DIV: 10/3=3",
    [16] = "DIV: -42/6=-7",
    [17] = "DIV: 123/1=123",

    -- test_modulo (3 tests: 18-20)
    [18] = "MOD: 10%3=1",
    [19] = "MOD: 12%4=0",
    [20] = "MOD: 3%10=3",

    -- test_shifts (4 tests: 21-24)
    [21] = "SHL: 1<<3=8",
    [22] = "SHR: 16>>2=4",
    [23] = "SHL: 5<<10=5120",
    [24] = "SHR: 0x8000>>1=0x4000",

    -- test_comparisons (4 tests: 25-28)
    [25] = "CMP: 5<10 true",
    [26] = "CMP: 5>10 false",
    [27] = "CMP: 42==42 true",
    [28] = "CMP: -10<-5 true",

    -- test_power (2 tests: 29-30)
    [29] = "POW: 2^10=1024",
    [30] = "POW: 3^5=243",

    -- test_bitwise (5 tests: 31-35)
    [31] = "BIT: AND FF00&0FF0",
    [32] = "BIT: OR FF00|00FF",
    [33] = "BIT: XOR AAAA^5555",
    [34] = "BIT: NOT ~00FF",
    [35] = "BIT: Combined mask ops",

    -- test_increment (7 tests: 36-42)
    [36] = "INC: ++a (5->6)",
    [37] = "INC: a++ (10->11)",
    [38] = "DEC: --a (8->7)",
    [39] = "DEC: a-- (3->2)",
    [40] = "INC: a++ returns 5",
    [41] = "INC: a is now 6",
    [42] = "DEC: --a returns 9",

    -- test_unary (6 tests: 43-48)
    [43] = "NEG: -(42)=-42",
    [44] = "NEG: -(-100)=100",
    [45] = "NEG: -(0)=0",
    [46] = "NOT: !0=1",
    [47] = "NOT: !42=0",
    [48] = "NOT: !(-1)=0",

    -- test_compound (9 tests: 49-57)
    [49] = "CMP: a+=5 (10->15)",
    [50] = "CMP: a-=8 (20->12)",
    [51] = "CMP: a*=7 (6->42)",
    [52] = "CMP: a/=4 (100->25)",
    [53] = "CMP: a%=5 (17->2)",
    [54] = "CMP: a&=0F0F",
    [55] = "CMP: a|=0F00",
    [56] = "CMP: a<<=3 (4->32)",
    [57] = "CMP: a>>=2 (64->16)",

    -- test_array (5 tests: 58-62)
    [58] = "ARR: arr[0]=10",
    [59] = "ARR: arr[2]=30",
    [60] = "ARR: arr[4]=50",
    [61] = "ARR: sum(arr)=150",
    [62] = "ARR: arr[1+1]=99",

    -- test_s32_basic (8 tests: 63-70) - now enabled with long long typedef
    [63] = "S32: assign 100k",
    [64] = "S32: 50k+50k=100k",
    [65] = "S32: 500k+500k=1M",
    [66] = "S32: 100k-30k=70k",
    [67] = "S32: 1k*1k=1M",
    [68] = "S32: 500*500=250k",
    [69] = "S32: 1M/1k=1k",
    [70] = "S32: 100k/100=1k",
}

function log(msg)
    emu.log(msg)
    print(msg)
end

function read8(addr)
    return emu.read(addr, emu.memType.cpu)
end

function read16(addr)
    local lo = emu.read(addr, emu.memType.cpu)
    local hi = emu.read(addr + 1, emu.memType.cpu)
    return lo + (hi * 256)
end

function readS16(addr)
    local val = read16(addr)
    if val >= 32768 then
        val = val - 65536
    end
    return val
end

function printDetailedResults(total)
    log("")
    log("=== DETAILED TEST RESULTS ===")
    log("")

    local allPassed = true

    for i = 0, total - 1 do
        local result = read8(ADDR_RESULTS + i)
        local expected = readS16(ADDR_EXPECTED + (i * 2))
        local actual = readS16(ADDR_ACTUAL + (i * 2))
        local name = testNames[i] or string.format("Test #%d", i + 1)

        if result == 1 then
            log(string.format("  [PASS] %s", name))
        else
            log(string.format("  [FAIL] %s", name))
            log(string.format("         Expected: %d, Got: %d", expected, actual))
            allPassed = false
        end
    end

    log("")
    return allPassed
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

    -- Check if tests completed
    if status == STATUS_PASSED or status == STATUS_FAILED then
        log("=== PVSNESLIB ARITHMETIC TEST SUITE ===")
        log("")

        printDetailedResults(total)

        log("=== SUMMARY ===")
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
        log("=== PVSNESLIB ARITHMETIC TEST SUITE ===")
        log("")
        log(string.format("TIMEOUT: Tests did not complete after %d frames", maxFrames))
        log(string.format("Status: 0x%04X, Total: %d, Passed: %d, Failed: %d",
            status, total, passed, failed))
        emu.stop(2)
        return
    end
end

emu.addEventCallback(onEndFrame, emu.eventType.endFrame)
log("Starting PVSNESLIB Arithmetic Test Suite...")
log("Waiting for test ROM to complete...")
