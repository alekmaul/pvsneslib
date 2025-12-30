# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

PVSnesLib is a free and open source SDK for developing Super Nintendo Entertainment System (SNES) software. It includes:
- **816-tcc**: C compiler targeting the 65816 CPU
- **816-opt**: Peephole optimizer for generated assembly
- **wla-dx**: Multi-architecture assembler/linker (submodule)
- **pvsneslib**: SNES hardware abstraction library
- **Tools**: Graphics converters, sound tools, ROM utilities

## Build Commands

```bash
# Set environment (required before building)
export PVSNESLIB_HOME=/path/to/pvsneslib

# Build entire SDK (compiler, tools, library)
make

# Build only tools
cd tools && make

# Build only library
cd pvsneslib && make

# Build a specific example
cd snes-examples/hello_world && make

# Build all examples
cd snes-examples && make

# Clean everything
make clean
```

## Compilation Pipeline

```
C Source (.c)
    ↓ [816-tcc]
.ps (pseudo-assembly)
    ↓ [816-opt]
.asp (optimized assembly)
    ↓ [constify]
.asm (ROM-ready, const data moved to .rodata)
    ↓ [wla-65816]
.obj (object file)
    ↓ [wlalink]
.sfc (SNES ROM)
```

## Directory Structure

```
pvsneslib/
├── compiler/
│   ├── tcc/           # 816-tcc C compiler (modified TinyCC)
│   └── wla-dx/        # Assembler/linker (git submodule)
├── devkitsnes/        # Build output (binaries, includes, libs)
├── pvsneslib/
│   ├── include/snes/  # Public headers (sprite.h, video.h, etc.)
│   └── source/        # Library source (mostly 65816 assembly)
├── snes-examples/     # 80+ example projects organized by category
│   └── tests/         # Automated test suite with Mesen2 integration
└── tools/             # Development utilities
    ├── 816-opt/       # Peephole optimizer
    ├── constify/      # Moves const data to ROM
    ├── gfx2snes/      # Legacy graphics converter
    ├── gfx4snes/      # Modern graphics converter (metasprites, tiles)
    ├── smconv/        # Sound/music converter (IT format)
    ├── snesbrr/       # BRR audio encoder
    ├── snestools/     # ROM header analysis
    └── tmx2snes/      # Tiled map converter
```

## Testing

```bash
# Run automated tests (requires Mesen2 emulator)
cd snes-examples/tests
./run_tests.sh /path/to/Mesen

# Test suite (5 tests, all should pass):
# 1. Build validation: Verifies all 135 ROMs compile correctly
# 2. Arithmetic tests: 71 tests (addition, multiplication, division, etc.)
# 3. Background init: 6 tests for bgSetEnable/bgInitTileSet initialization
# 4. Malloc tests: 7 tests validating heap allocation (Issue #311 fix)
# 5. TMX2SNES tests: Validates locked layer parsing (Issue #318 fix)

# Run individual tests:
./build_validation.sh                    # No emulator needed
cd malloc_test && make                   # Build malloc test ROM
cd tmx2snes_test && ./run_test.sh        # No emulator needed
```

### Test Lua Scripts

Test ROMs communicate results via BSS variables. Lua scripts read from symbol file addresses:
```lua
-- Example: Find addresses with 'grep test_ *.sym'
-- 007e2008 test_status
-- 007e200a test_total
local ADDR_STATUS = 0x7E2008  -- NOT 0x7E2000!
```

## Key Technical Details

### SNES Hardware Concepts
- **OAM**: Object Attribute Memory (sprite data) - 128 sprites max
- **VRAM**: Video RAM layout varies by sprite size:
  - 8x8 sprites: 16 per row (128 pixels / 8)
  - 16x16 sprites: 8 per row
  - 32x32 sprites: 4 per row
- **CGRAM**: Color palette RAM (256 colors for BG, 256 for sprites)
- **DMA**: Direct Memory Access for fast VRAM transfers

### Sprite Configuration (OBJ_SIZE)
```c
// Common configurations
OBJ_SIZE8_L16   // Small=8x8 at VRAM $0000, Large=16x16 at $0000
OBJ_SIZE16_L32  // Small=16x16 at VRAM $1000, Large=32x32 at $0000
OBJ_SIZE8_L32   // Small=8x8 at VRAM $0000, Large=32x32 at $0000
```

### Metasprite System
- Metasprites combine multiple hardware sprites into one logical unit
- `gfx4snes -ms` generates metasprite data with VRAM-layout-aware indices
- `oamMetaDraw8/16/32` functions render metasprites to OAM

### 65816 Assembly Notes
- `rep #$20`: 16-bit accumulator mode
- `sep #$20`: 8-bit accumulator mode
- Direct page ($00-$FF): Fast access variables
- Bank 0 ($7E): Work RAM

## Code Style

### C Code
- Use `u8`, `u16`, `s16`, `s32` from `snestypes.h` for fixed-width types
- `TRUE`/`FALSE` macros (note: TRUE is 0xff, not 1)
- Document functions with Doxygen-style comments

### Assembly
- Use WLA-DX syntax for 65816 assembly
- Label format: `_functionName`, `.localLabel`
- Comment register states after operations

## Common Issues and Solutions

### "Sprites not visible"
1. Check VRAM address matches sprite size configuration
2. Verify palette is loaded to correct CGRAM offset
3. Ensure sprites aren't at Y=240 (hidden scanline)

### "Build fails on macOS"
- Some tools may need `sed` portability fixes
- Ensure `PVSNESLIB_HOME` is exported before building

### "Header file changes not detected"
- The build system doesn't track header dependencies
- Use `make clean && make` after header changes

### "malloc() returns NULL in small projects" (Issue #311 - FIXED)
- **Cause:** Compiler bug where NULL comparison only checks low 16 bits
- **Fix Applied:** Added `__heap_guard` in crt0_snes.asm to ensure heap starts at non-zero address
- **Verification:** Run `snes-examples/tests/malloc_test/` - all 7 tests should pass

### "tmx2snes hangs on locked layers" (Issue #318 - FIXED)
- **Cause:** cute_tiled parser didn't handle `locked` property from newer Tiled versions
- **Fix Applied:** Added locked field handler in cute_tiled.h
- **Verification:** Run `snes-examples/tests/tmx2snes_test/run_test.sh`

## Important Submodules

```bash
# Initialize submodules (required for wla-dx)
git submodule update --init --recursive
```

- `compiler/wla-dx/`: Don't modify directly; it's an upstream project

## Documentation

- **API Reference**: Generated via Doxygen from header comments
- **Wiki**: External wiki (may have outdated links)
- **Examples**: Best source of usage patterns
- **REVIEW_NOTES.md**: Comprehensive codebase analysis and known issues
