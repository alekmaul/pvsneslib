# PVSnesLib Comprehensive Review Notes

**Date:** December 29, 2025
**Branch:** enhancement/compiler-and-build-fixes
**Version:** 4.5.0

---

## Executive Summary

This document contains a thorough analysis of the pvsneslib codebase covering:
- Tools (9 utilities for SNES development)
- Library (pvsneslib core SDK)
- Compiler (816-tcc and 816-opt optimizer)
- Build System (Makefiles and snes_rules)
- Examples and Tests (80+ examples, test infrastructure)
- Security Analysis
- Code Duplication and Refactoring Opportunities

---

## Table of Contents

1. [Tools Review](#1-tools-review)
2. [Library Review](#2-library-review)
3. [Compiler Review](#3-compiler-review)
4. [Build System Review](#4-build-system-review)
5. [Examples and Tests Review](#5-examples-and-tests-review)
6. [Security Analysis](#6-security-analysis)
7. [Code Duplication](#7-code-duplication)
8. [Priority Recommendations](#8-priority-recommendations)
9. [Bug Fixes Applied](#9-bug-fixes-applied)

---

## 1. Tools Review

### Overview

| Tool | Lines | Purpose | Quality | Issues |
|------|-------|---------|---------|--------|
| bin2txt | 278 | Binary to text conversion | Good | 2 low |
| constify | 459 | Move const data to ROM | Fair | 3 moderate |
| 816-opt | 1,126 | Peephole optimizer | Good | 4 high |
| gfx2snes | 795 | Graphics converter (legacy) | Fair | 5 high |
| gfx4snes | ~3,000 | Graphics converter (modern) | Good | 3 moderate |
| smconv | 1,292 | Sound converter | Good | 2 moderate |
| snesbrr | 796 | BRR audio codec | Good | 2 moderate |
| snestools | 1,148 | ROM header analysis | Good | 2 moderate |
| tmx2snes | 637 | Tiled map converter | Fair | 3 moderate |

### Key Issues Found

**Critical:**
- gfx2snes/loadimg.c: BMP RLE8 buffer overflow (lines 177-191)
- tmx2snes: Unbounded array access via tile_index (lines 262-274)

**High:**
- 816-opt: Array bounds checking missing for lookahead window (lines 604, 745)
- gfx2snes: Integer overflow before malloc (line 79)
- lodepng: Multiple TODOs indicating incomplete error handling

**Recommendations:**
1. Add bounds checking to all array accesses in 816-opt
2. Implement integer overflow protection in image loading
3. Validate all indices from parsed input (TMX, BMP headers)

---

## 2. Library Review

### Structure

- **Headers:** 15 files, 3,268 lines in include/snes/
- **Source:** 23 files, 18,637 lines (mostly assembly)
- **Sound:** snesmod SPC700 driver (WLA-DX converted)

### Strengths

1. **Well-documented API** - 686 Doxygen-documented functions
2. **Hardware-optimized** - Critical paths in 65816 assembly
3. **Comprehensive features** - All SNES modes, sprites, audio, objects

### Issues Found

1. **snestypes.h:** Boolean uses 0xff for TRUE (potential subtle bugs)
2. **sprite.h:** 20+ duplicate sprite drawing functions (should be unified)
3. **libc_c.c:** FIXME for strnlen (line 186), incomplete format string support
4. **libm.asm:** FIXME for _RTAR overflow handling (line 208)
5. **No memory safety** - malloc/free without bounds checking

### API Improvement Recommendations

```c
// Current: 20+ separate functions
oamDynamic32Draw(...)
oamFix32Draw(...)
oamDynamic16Draw(...)
// etc.

// Proposed: Single unified function
void oamDraw(u16 id, u16 x, u16 y, u8 priority,
             u8 size, u16 gfx_offset, u8 palette, u8 dynamic);
```

---

## 3. Compiler Review

### 816-tcc Compiler

**Known Limitations:**
- Long double not supported (816-gen.c line 23)
- Index > 65535 handling broken (FIXME line 446)
- Implicit type cast uncertain (XXX line 949)
- Only 6 general-purpose registers (R0-R5)

**Recent Improvements:**
- Power-of-2 division optimization (LSR/AND)
- Multiplication constant optimizations
- Direct page addressing fix in tcc__udiv (fixes #299)

### 816-opt Optimizer

**23+ Optimization Patterns:**
- Store elimination (redundant pseudo-register stores)
- Push-pop pair elimination
- Comparison reduction (13 instructions → 4)
- Mode change elimination (rep/sep sequences)
- Branch optimization (long jump to short branch)

**New Patterns Added (commit 1683419d):**
- rep/sep elimination
- Push-pop pair removal (pha/pla, phx/plx, phy/ply)
- Increment-decrement cancellation

### Optimization Opportunities

| Opportunity | Effort | Impact | ROI |
|-------------|--------|--------|-----|
| Direct page variable allocation | Medium | 5-10% | High |
| Extend multiplication patterns | Low | 5-10% | Very High |
| Inline small functions | Medium | 5-10% | High |
| Fix FIXMEs in 816-gen.c | Medium | 2-3% | High |
| Register allocation improvement | High | 10-20% | Medium |

---

## 4. Build System Review

### Architecture

```
C Source (.c)
    ↓ [816-tcc]
.ps (pseudo-assembly)
    ↓ [816-opt]
.asp (optimized)
    ↓ [constify]
.asm (ROM-ready)
    ↓ [wla-65816]
.obj (object)
    ↓ [wlalink]
.sfc (ROM)
```

### Critical Issues

1. **No dependency tracking** - Header changes don't trigger recompilation
2. **sed -i portability** - Fails on macOS (snes_rules lines 175, 181)
3. **snestools Makefile bug** - Missing `-o` flag (line 40)
4. **smconv LDFLAGS bug** - Not used in link command (line 35)

### Build System Recommendations

**Immediate:**
```makefile
# Fix sed portability (snes_rules)
# Current:
@sed -i 's/://' $(ROMNAME).sym
# Fixed:
@sed 's/://' $(ROMNAME).sym > $(ROMNAME).sym.tmp && mv $(ROMNAME).sym.tmp $(ROMNAME).sym
```

**Short-term:**
- Add .d dependency file generation
- Fix tool Makefile bugs
- Enable parallel builds where safe

---

## 5. Examples and Tests Review

### Coverage

- **17 categories** with 80+ examples
- **12,405 lines** of example C code
- **71 automated tests** (arithmetic only)
- **Screenshot regression tests** for visual validation

### Test Infrastructure

| Test Type | Count | Status |
|-----------|-------|--------|
| Unit tests | 1 (arithmetic) | Comprehensive |
| Screenshot tests | ~30 | Working |
| Smoke tests | Basic | Minimal |
| Input/Audio/Physics tests | 0 | Missing |

### Missing Test Coverage

1. **No input tests** - Controller, mouse, multi-player
2. **No audio tests** - Sound effects, music playback
3. **No physics tests** - Object collision, gravity
4. **No SRAM tests** - Save/load functionality
5. **No map engine tests** - Scrolling, boundaries

### Example Improvements Needed

1. **hello_world** - Add state machines, timing examples
2. **arithmetic** - Document algorithms (power, sqrt)
3. **input/controller** - Add buffering, combo detection
4. **games/likemario** - Extract physics into module

---

## 6. Security Analysis

### Critical Vulnerabilities

| Priority | File | Issue | Impact |
|----------|------|-------|--------|
| CRITICAL | loadimg.c:177-191 | BMP RLE8 buffer overflow | Code execution |
| CRITICAL | loadimg.c:341-343 | Integer overflow in dimensions | Heap overflow |
| CRITICAL | tmx2snes.c:262-274 | Unbounded array write | Heap overflow |
| HIGH | loadimg.c:103-112 | PCX RLE count overflow | Buffer overflow |
| HIGH | loadimg.c:79 | No malloc overflow check | Info disclosure |
| MEDIUM | tmx2snes.c:164-172 | Path traversal in layer names | File write |

### Recommendations

1. **Add overflow checks before all malloc() calculations**
2. **Validate all array indices before use**
3. **Check bounds on RLE decompression counters**
4. **Sanitize file paths from XML/JSON input**
5. **Replace atoi() with strtol() + validation**

---

## 7. Code Duplication

### Major Duplications

| Files | Lines | Consolidation Saving |
|-------|-------|----------------------|
| lodepng.c (2 copies) | 13,478 | ~6,700 lines |
| lz77.c (2 copies) | 947 | ~470 lines |
| errors.c (2 copies) | 187 | ~90 lines |
| Image loaders | 961 | ~480 lines |
| **Total** | ~16,500 | **~8,000 lines** |

### Refactoring Proposal

```
tools/common/
├── lodepng.c          # Consolidated PNG library
├── lz77.c             # Single compression implementation
├── errors.c/h         # Shared error handling
├── images.c/h         # Unified image loader
├── file_utils.c/h     # Safe file operations
└── memory.h           # Safe allocation macros
```

### Build System Changes

```cmake
add_library(pvsneslib_common STATIC
  common/lodepng.c
  common/lz77.c
  common/errors.c
  common/images.c
)

target_link_libraries(gfx2snes pvsneslib_common)
target_link_libraries(gfx4snes pvsneslib_common)
```

---

## 8. Priority Recommendations

### Immediate (This Week)

1. **Security:** Add bounds checking in loadimg.c RLE decompression
2. **Security:** Validate tile_index in tmx2snes before array access
3. **Build:** Fix sed -i portability issue in snes_rules
4. **Build:** Fix snestools Makefile missing -o flag

### Short-term (1-2 Weeks)

1. **Refactor:** Consolidate lodepng.c (eliminate 6,700 duplicate lines)
2. **Refactor:** Consolidate lz77.c (eliminate 470 duplicate lines)
3. **Build:** Add .d dependency file generation
4. **Tests:** Add input handling tests

### Medium-term (1 Month)

1. **Compiler:** Add more multiplication/division patterns to 816-opt
2. **Library:** Unify sprite drawing functions (20+ → 1)
3. **Docs:** Create SETUP.md with step-by-step installation
4. **Tests:** Add audio and physics tests

### Long-term (2-3 Months)

1. **Compiler:** Implement function inlining for small functions
2. **Build:** Fix WLA-DX integration (remove -c/-d flags)
3. **Library:** Add memory safety to malloc/free
4. **Docs:** Create architecture documentation

---

## Documentation Gaps

### Missing Documentation

1. **SETUP.md** - Step-by-step installation guide
2. **ARCHITECTURE.md** - Build pipeline explanation
3. **CONTRIBUTING.md** - Code style, PR process
4. **TROUBLESHOOTING.md** - Common errors and solutions
5. **Tool documentation** - Most tools lack usage guides

### Existing Documentation Quality

| Document | Status | Quality |
|----------|--------|---------|
| README.md | Present | Good |
| API docs (Doxygen) | Generated | Excellent |
| Wiki | External (broken refs) | Poor |
| Example docs | Sparse | Fair |

---

## Summary Metrics

### Code Statistics

| Component | Lines | Files | Quality |
|-----------|-------|-------|---------|
| Tools | ~15,000 | 63 | Fair-Good |
| Library | ~22,000 | 38 | Good |
| Examples | ~12,400 | 80+ | Good |
| Tests | ~2,000 | 15 | Fair |
| **Total** | ~51,400 | 196+ | Good |

### Health Scores (1-10)

| Area | Score | Notes |
|------|-------|-------|
| Code Quality | 7 | Good structure, some issues |
| Security | 5 | Critical vulns in tools |
| Documentation | 6 | API good, guides missing |
| Testing | 5 | Limited coverage |
| Build System | 6 | Works but fragile |
| Maintainability | 6 | Duplication issues |
| **Overall** | **6** | Solid foundation, needs polish |

---

## 9. Bug Fixes Applied

### MetaSprite Tile Ordering Bug (December 30, 2025)

**Problem:** Metasprites rendered incorrectly because tile indices didn't match SNES VRAM layout. Most notably, 16x16 character metasprites rendered with head below feet.

**Root Cause:** Two interconnected issues affecting ALL metasprite sizes (8x8, 16x16, 32x32):

1. **gfx4snes/metasprites.c** - Generated sequential tile indices instead of VRAM-layout-aware indices. The SNES arranges sprites in VRAM with a fixed row width of 128 pixels:
   - 8x8 sprites: 16 per row
   - 16x16 sprites: 8 per row
   - 32x32 sprites: 4 per row

2. **sprites.asm (oamMetaDraw8/16/32)** - All three functions ignored the tile index from metasprite data and used the OAM counter instead, causing incorrect tile selection.

**Fix Applied:**

**File: `tools/gfx4snes/src/metasprites.c`**
```c
// Added VRAM-layout-aware tile index calculation
int sprites_per_vram_row = 128 / blocksize;  // 16 for 8x8, 8 for 16x16, 4 for 32x32
int metasprites_per_vram_row = sprites_per_vram_row / nbsprx;

// For each tile in metasprite:
int meta_base_x = (i % metasprites_per_vram_row) * nbsprx;
int meta_base_y = (i / metasprites_per_vram_row) * nbspry;
int vram_tile_index = (meta_base_y + y) * sprites_per_vram_row + (meta_base_x + x);
```

**File: `pvsneslib/source/sprites.asm`**

Fixed in three functions: `oamMetaDraw8`, `oamMetaDraw16`, `oamMetaDraw32`
```asm
; Before: Used OAM counter (ignored metasprite tile index)
lda sprit_val0      ; A = sprite counter * 4
lsr a               ; A = counter * 2
tax                 ; X = lookup index

; After: Uses actual tile index from metasprite data
lda sprit_val0      ; A = OAM entry offset
pha                 ; save for later
lda oambuffer.1.oamframeid,y  ; A = tile index from metasprite
asl a               ; A = tile index * 2
tax                 ; X = lookup index
```

**Result:**
- Generated metasprite indices now match SNES VRAM layout for all sprite sizes
- Lookup tables (`lkup8idT`, `lkup16idT`, `lkup32idT`) correctly map to character names
- All metasprite sizes (8x8, 16x16, 32x32) now render with correct tile arrangement

**Files Modified:**
- `tools/gfx4snes/src/metasprites.c`
- `pvsneslib/source/sprites.asm` (oamMetaDraw8, oamMetaDraw16, oamMetaDraw32)

**Test Added:**
- `snes-examples/tests/MetaSprite/run_test.lua` - Validates metasprite OAM entries and tile indices

---

## Next Steps

1. Review this document with maintainers
2. Create GitHub issues for critical security fixes
3. Plan refactoring sprints for code consolidation
4. Establish testing requirements for new features
5. Document build process and contribute guidelines

---

*This review was conducted to identify improvement opportunities and does not diminish the significant work done by the pvsneslib community.*
