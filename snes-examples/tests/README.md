# PVSNESLIB Test Suite

Automated test suite for PVSNESLIB that verifies compiler output, library functions, and tools.

## Prerequisites

- **Mesen2 Emulator**: Required for running ROM-based tests
  - Download from: https://www.mesen.ca/
  - Must support `--testrunner` and `--lua` command line options
- **PVSNESLIB_HOME**: Environment variable must be set

## Running Tests

### Run All Tests

```bash
./run_tests.sh /path/to/Mesen
```

### Run Individual Test Suites

```bash
# Arithmetic tests only
cd arithmetic
/path/to/Mesen --testrunner test_arithmetic.sfc --lua run_test.lua

# Malloc tests only
cd malloc_test
/path/to/Mesen --testrunner malloc_test.sfc --lua run_test.lua

# Background init tests only
cd background_init
/path/to/Mesen --testrunner test_background.sfc --lua run_test.lua

# TMX2SNES tests (no emulator needed)
cd tmx2snes_test
./run_test.sh

# Build validation (no emulator needed)
./build_validation.sh
```

## Test Suites

### arithmetic/ (71 tests)

Tests compiler-generated arithmetic operations for 16-bit and 32-bit integers:

- **Addition/Subtraction**: Basic math, signed numbers, overflow handling
- **Multiplication**: Including optimized paths for constants
- **Division/Modulo**: Signed division, power-of-2 optimizations
- **Bit Operations**: AND, OR, XOR, NOT, shifts
- **Comparisons**: <, >, ==, signed comparisons
- **Compound Assignment**: +=, -=, *=, /=, etc.
- **Array Operations**: Indexing and iteration
- **32-bit (s32)**: Long integer operations via `long long` type

These tests validate the 816-tcc compiler and 816-opt optimizer.

### malloc_test/ (7 tests)

Tests dynamic memory allocation (Issue #311 regression test):

- `malloc()` basic allocation
- `free()` deallocation
- Multiple allocation/free cycles
- Edge cases with zero and large sizes

### background_init/ (6 tests)

Tests SNES background layer initialization:

- Video mode setup
- VRAM address configuration
- Tile map initialization
- Palette loading

### tmx2snes_test/

Tests the TMX to SNES converter tool (Issue #318 regression test):

- Locked layer parsing
- Tileset conversion
- Map data generation

### build_validation.sh

Validates that all example ROMs:
- Compile without errors
- Produce valid SNES ROM files (32KB - 4MB)

### smoke_test.lua

Generic test script that verifies a ROM runs for 3 seconds without crashing.
Useful for quick validation of new examples.

## Exit Codes

| Code | Meaning |
|------|---------|
| 0 | All tests passed |
| 1 | Some tests failed |
| 2 | Mesen not found / Timeout |

## Adding New Tests

### Creating a ROM-based Test

1. Create a new directory under `tests/`:
   ```bash
   mkdir my_test
   cd my_test
   ```

2. Create the test ROM source (`my_test.c`):
   ```c
   #include <snes.h>

   // Test results stored in fixed memory locations
   volatile u16 test_status = 0;  // 0=running, 1=passed, 0xFF=failed
   volatile u16 test_total = 0;
   volatile u16 test_passed = 0;
   volatile u16 test_failed = 0;

   void run_tests() {
       // Your test logic here
       test_total = 1;

       if (/* condition */) {
           test_passed++;
       } else {
           test_failed++;
       }

       test_status = (test_failed == 0) ? 1 : 0xFF;
   }

   int main() {
       run_tests();
       while(1);  // Halt
   }
   ```

3. Create the Lua test runner (`run_test.lua`):
   ```lua
   local ADDR_STATUS = 0x7E2000  -- Match your .sym file
   local maxFrames = 300
   local frameCount = 0

   function onEndFrame()
       frameCount = frameCount + 1
       if frameCount < 30 then return end

       local status = emu.read(ADDR_STATUS, emu.memType.cpu)
       if status == 1 then
           print("PASSED")
           emu.stop(0)
       elseif status == 0xFF then
           print("FAILED")
           emu.stop(1)
       elseif frameCount >= maxFrames then
           print("TIMEOUT")
           emu.stop(2)
       end
   end

   emu.addEventCallback(onEndFrame, emu.eventType.endFrame)
   ```

4. Create standard PVSNESLIB build files:
   - `Makefile` (copy from existing test)
   - `hdr.asm` (ROM header)
   - `linkfile` (linker configuration)

5. Add to `run_tests.sh`:
   ```bash
   # Run my_test
   echo "Running my_test..."
   cd "$SCRIPT_DIR/my_test"
   "$MESEN" --testrunner my_test.sfc --lua run_test.lua 2>&1
   # ... handle result
   ```

### Creating a Tool Test

For testing tools without an emulator:

1. Create a directory with test input files
2. Create `run_test.sh` that:
   - Runs the tool with test inputs
   - Validates output against expected results
   - Returns 0 on success, 1 on failure

## Directory Structure

```
tests/
├── README.md              # This file
├── run_tests.sh           # Main test runner
├── build_validation.sh    # ROM file validation
├── smoke_test.lua         # Generic crash detection
├── Makefile               # Build all test ROMs
├── .gitignore             # Ignore build artifacts
├── arithmetic/            # Compiler arithmetic tests
│   ├── src/
│   │   └── test_arithmetic.c
│   ├── run_test.lua
│   ├── Makefile
│   └── ...
├── malloc_test/           # Memory allocation tests
├── background_init/       # Video initialization tests
└── tmx2snes_test/         # TMX converter tests
```

## Troubleshooting

**"Mesen not found"**
- Provide full path to Mesen executable
- Ensure Mesen has execute permissions

**"Tests timeout"**
- Check that PVSNESLIB_HOME is set correctly
- Rebuild test ROMs: `make clean && make`
- Verify ROM loads manually in Mesen

**"Address mismatch in Lua script"**
- Memory addresses in Lua must match `.sym` file
- Rebuild ROM and check new addresses in `.sym`

## Contributing

When fixing bugs or adding features to PVSNESLIB:

1. Consider adding a regression test
2. Tests should be deterministic (same result every run)
3. Keep test ROMs minimal - test one thing at a time
4. Document what the test validates
