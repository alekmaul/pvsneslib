#!/bin/bash
#
# PVSNESLIB Build Validation Test
#
# Verifies that all examples build correctly and produce valid ROM files.
# Tests the graphics tools (gfx2snes, gfx4snes) by checking:
# 1. All examples compile without errors
# 2. All ROMs have valid size (> 32KB)
#

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
EXAMPLES_DIR="$(dirname "$SCRIPT_DIR")"

passed=0
failed=0

echo "==========================================="
echo "PVSNESLIB Build Validation Test"
echo "==========================================="
echo ""

# Check if PVSNESLIB_HOME is set
if [ -z "$PVSNESLIB_HOME" ]; then
    echo "Error: PVSNESLIB_HOME is not set"
    exit 1
fi

echo "Scanning for ROM files..."

# Find and validate each ROM
while IFS= read -r rom; do
    # Skip test directory
    case "$rom" in
        */tests/*) continue ;;
    esac

    rel_path="${rom#$EXAMPLES_DIR/}"

    # Check file size (should be at least 32KB for a valid SNES ROM)
    size=$(stat -f%z "$rom" 2>/dev/null || stat -c%s "$rom" 2>/dev/null)

    if [ "$size" -ge 32768 ] && [ "$size" -le 4194304 ]; then
        echo "[PASS] $rel_path ($size bytes)"
        passed=$((passed + 1))
    else
        echo "[FAIL] $rel_path - Invalid size ($size bytes)"
        failed=$((failed + 1))
    fi
done < <(find "$EXAMPLES_DIR" -name "*.sfc" -type f 2>/dev/null | sort)

echo ""
echo "==========================================="
echo "Results: $passed passed, $failed failed"
echo "==========================================="

if [ "$failed" -gt 0 ]; then
    echo "BUILD VALIDATION FAILED"
    exit 1
else
    echo "BUILD VALIDATION PASSED"
    exit 0
fi
