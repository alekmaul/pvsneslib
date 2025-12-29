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

# Run arithmetic tests
cd "$SCRIPT_DIR/arithmetic"
"$MESEN" --testrunner test_arithmetic.sfc --lua run_test.lua 2>&1
RESULT=$?
TOTAL=$((TOTAL + 1))

if [ $RESULT -eq 0 ]; then
    echo "  PASSED"
    PASSED=$((PASSED + 1))
else
    echo "  FAILED (exit code: $RESULT)"
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
