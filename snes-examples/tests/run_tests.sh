#!/bin/bash
#---------------------------------------------------------------------------------
# PVSNESLIB Test Runner
#
# Runs all test ROMs using Mesen2 emulator
#
# Usage: ./run_tests.sh [path_to_mesen]
#
# Exit codes:
#   0 = All tests passed
#   1 = Some tests failed
#   2 = Mesen not found
#---------------------------------------------------------------------------------

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MESEN_ARG="${1:-Mesen}"

# Resolve Mesen path to absolute
if [[ "$MESEN_ARG" == /* ]]; then
    MESEN="$MESEN_ARG"
else
    MESEN="$(cd "$(dirname "$MESEN_ARG")" 2>/dev/null && pwd)/$(basename "$MESEN_ARG")"
fi

# Check if Mesen exists
if [ ! -x "$MESEN" ]; then
    echo "ERROR: Mesen not found at: $MESEN"
    echo "Usage: $0 /path/to/Mesen"
    exit 2
fi

TOTAL=0
PASSED=0
FAILED=0

echo "==========================================="
echo "PVSNESLIB Test Suite"
echo "==========================================="
echo ""

# Run build validation (no emulator needed)
echo "Running build validation..."
"$SCRIPT_DIR/build_validation.sh" > /dev/null 2>&1
RESULT=$?
TOTAL=$((TOTAL + 1))

if [ $RESULT -eq 0 ]; then
    echo "  Build validation: PASSED"
    PASSED=$((PASSED + 1))
else
    echo "  Build validation: FAILED"
    FAILED=$((FAILED + 1))
fi

# Run arithmetic tests
echo ""
echo "Running arithmetic tests..."
cd "$SCRIPT_DIR/arithmetic"
"$MESEN" --testrunner test_arithmetic.sfc --lua run_test.lua 2>&1
RESULT=$?
TOTAL=$((TOTAL + 1))

if [ $RESULT -eq 0 ]; then
    echo "  Arithmetic tests: PASSED"
    PASSED=$((PASSED + 1))
else
    echo "  Arithmetic tests: FAILED (exit code: $RESULT)"
    FAILED=$((FAILED + 1))
fi

# Run background init tests
echo ""
echo "Running background init tests..."
cd "$SCRIPT_DIR/background_init"
"$MESEN" --testrunner test_background.sfc --lua run_test.lua 2>&1
RESULT=$?
TOTAL=$((TOTAL + 1))

if [ $RESULT -eq 0 ]; then
    echo "  Background init tests: PASSED"
    PASSED=$((PASSED + 1))
else
    echo "  Background init tests: FAILED (exit code: $RESULT)"
    FAILED=$((FAILED + 1))
fi

# Run malloc tests (Issue #311)
echo ""
echo "Running malloc tests..."
cd "$SCRIPT_DIR/malloc_test"
"$MESEN" --testrunner malloc_test.sfc --lua run_test.lua 2>&1
RESULT=$?
TOTAL=$((TOTAL + 1))

if [ $RESULT -eq 0 ]; then
    echo "  Malloc tests: PASSED"
    PASSED=$((PASSED + 1))
else
    echo "  Malloc tests: FAILED (exit code: $RESULT)"
    FAILED=$((FAILED + 1))
fi

# Run tmx2snes tests (Issue #318 - no emulator needed)
echo ""
echo "Running tmx2snes tests..."
cd "$SCRIPT_DIR/tmx2snes_test"
./run_test.sh > /dev/null 2>&1
RESULT=$?
TOTAL=$((TOTAL + 1))

if [ $RESULT -eq 0 ]; then
    echo "  TMX2SNES tests: PASSED"
    PASSED=$((PASSED + 1))
else
    echo "  TMX2SNES tests: FAILED"
    FAILED=$((FAILED + 1))
fi

echo ""
echo "==========================================="
echo "Results: $PASSED/$TOTAL passed, $FAILED failed"
echo "==========================================="

if [ $FAILED -eq 0 ]; then
    echo "ALL TESTS PASSED"
    exit 0
else
    echo "SOME TESTS FAILED"
    exit 1
fi
