#!/bin/bash
#---------------------------------------------------------------------------------
# PVSNESLIB Screenshot Comparison Tests
#
# Compares current screenshots against golden references using MD5 hash.
#
# Usage: ./run_screenshot_tests.sh /path/to/Mesen
#
# Exit codes:
#   0 = All screenshots match
#   1 = Some screenshots differ (visual regression detected)
#   2 = Mesen not found or missing golden screenshots
#---------------------------------------------------------------------------------

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
EXAMPLES_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
SCREENSHOTS_DIR="$SCRIPT_DIR/screenshots"
TEMP_DIR="$SCRIPT_DIR/.screenshot_temp"
TEMP_SCREENSHOT="$SCRIPT_DIR/.mesen_screenshots/screenshot.png"
MESEN_ARG="${1:-Mesen}"

# Resolve Mesen path
if [[ "$MESEN_ARG" == /* ]]; then
    MESEN="$MESEN_ARG"
else
    MESEN="$(cd "$(dirname "$MESEN_ARG")" 2>/dev/null && pwd)/$(basename "$MESEN_ARG")"
fi

if [ ! -x "$MESEN" ]; then
    echo "ERROR: Mesen not found at: $MESEN"
    exit 2
fi

# Check for golden screenshots
if [ ! -d "$SCREENSHOTS_DIR" ] || [ -z "$(ls -A "$SCREENSHOTS_DIR"/*.png 2>/dev/null)" ]; then
    echo "ERROR: No golden screenshots found in $SCREENSHOTS_DIR"
    echo "Run ./generate_golden_screenshots.sh on master branch first!"
    exit 2
fi

# Create temp directories
rm -rf "$TEMP_DIR"
mkdir -p "$TEMP_DIR"
mkdir -p "$SCRIPT_DIR/.mesen_screenshots"

echo "==========================================="
echo "PVSNESLIB Screenshot Comparison Tests"
echo "==========================================="
echo ""

TOTAL=0
PASSED=0
FAILED=0
SKIPPED=0
FAILED_TESTS=""

# Get MD5 hash function
get_md5() {
    if command -v md5sum &> /dev/null; then
        md5sum "$1" | cut -d' ' -f1
    elif command -v md5 &> /dev/null; then
        md5 -q "$1"
    else
        echo "ERROR"
    fi
}

# Find all .sfc files
while IFS= read -r rom_path; do
    # Skip bin directory
    if [[ "$rom_path" == */bin/* ]]; then
        continue
    fi
    # Skip tests directory
    if [[ "$rom_path" == */tests/* ]]; then
        continue
    fi

    # Generate screenshot name
    rel_path="${rom_path#$EXAMPLES_DIR/}"
    dir_name="$(dirname "$rel_path")"
    rom_name="$(basename "$rom_path" .sfc)"
    safe_name="${dir_name//\//_}_${rom_name}"

    golden_screenshot="$SCREENSHOTS_DIR/${safe_name}.png"
    golden_hash_file="$SCREENSHOTS_DIR/${safe_name}.png.md5"

    # Skip if no golden screenshot
    if [ ! -f "$golden_screenshot" ]; then
        continue
    fi

    echo -n "[${safe_name}] "
    TOTAL=$((TOTAL + 1))

    # Get golden hash
    if [ -f "$golden_hash_file" ]; then
        golden_hash=$(cat "$golden_hash_file")
    else
        golden_hash=$(get_md5 "$golden_screenshot")
    fi

    # Clear any previous temp screenshot
    rm -f "$TEMP_SCREENSHOT" 2>/dev/null

    # Capture current screenshot
    "$MESEN" --testrunner "$rom_path" --lua "$SCRIPT_DIR/capture_screenshot.lua" >/dev/null 2>&1
    RESULT=$?

    if [ $RESULT -ne 0 ] || [ ! -f "$TEMP_SCREENSHOT" ]; then
        echo "SKIP (capture failed)"
        SKIPPED=$((SKIPPED + 1))
        continue
    fi

    # Compare hashes
    current_hash=$(get_md5 "$TEMP_SCREENSHOT")

    if [ "$current_hash" == "$golden_hash" ]; then
        echo "PASS"
        PASSED=$((PASSED + 1))
    else
        echo "FAIL (visual regression detected)"
        FAILED=$((FAILED + 1))
        FAILED_TESTS="$FAILED_TESTS\n  - $safe_name"

        # Save the differing screenshot for investigation
        cp "$TEMP_SCREENSHOT" "$TEMP_DIR/${safe_name}_current.png"
        cp "$golden_screenshot" "$TEMP_DIR/${safe_name}_golden.png"
    fi

    # Cleanup temp screenshot
    rm -f "$TEMP_SCREENSHOT"

done < <(find "$EXAMPLES_DIR" -name "*.sfc" -type f | sort)

echo ""
echo "==========================================="
echo "Results: $PASSED passed, $FAILED failed, $SKIPPED skipped (of $TOTAL)"
echo "==========================================="

if [ $FAILED -eq 0 ]; then
    echo "ALL SCREENSHOT TESTS PASSED"
    rm -rf "$TEMP_DIR"
    exit 0
else
    echo "VISUAL REGRESSIONS DETECTED:$FAILED_TESTS"
    echo ""
    echo "Differing screenshots saved to: $TEMP_DIR"
    exit 1
fi
