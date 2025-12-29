#!/bin/bash
#---------------------------------------------------------------------------------
# PVSNESLIB Golden Screenshot Generator
#
# Generates reference screenshots for all examples.
# Run this on a KNOWN-GOOD state (e.g., master branch) to create golden references.
#
# Usage: ./generate_golden_screenshots.sh /path/to/Mesen
#
# Screenshots are saved to: tests/screenshots/<example_name>.png
#---------------------------------------------------------------------------------

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
EXAMPLES_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
SCREENSHOTS_DIR="$SCRIPT_DIR/screenshots"
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
    echo "Usage: $0 /path/to/Mesen"
    exit 2
fi

# Create directories
mkdir -p "$SCREENSHOTS_DIR"
mkdir -p "$SCRIPT_DIR/.mesen_screenshots"

echo "==========================================="
echo "PVSNESLIB Golden Screenshot Generator"
echo "==========================================="
echo "Mesen: $MESEN"
echo "Output: $SCREENSHOTS_DIR"
echo ""
echo "WARNING: Only run this on a KNOWN-GOOD state!"
echo "         (e.g., master branch, tagged release)"
echo ""

TOTAL=0
SUCCESS=0
FAILED=0

# Find all .sfc files (excluding bin/ and tests/)
while IFS= read -r rom_path; do
    # Skip bin directory
    if [[ "$rom_path" == */bin/* ]]; then
        continue
    fi
    # Skip tests directory
    if [[ "$rom_path" == */tests/* ]]; then
        continue
    fi

    # Generate screenshot name from path
    rel_path="${rom_path#$EXAMPLES_DIR/}"
    dir_name="$(dirname "$rel_path")"
    rom_name="$(basename "$rom_path" .sfc)"

    # Create a safe filename (replace / with _)
    safe_name="${dir_name//\//_}_${rom_name}"
    screenshot_path="$SCREENSHOTS_DIR/${safe_name}.png"

    echo -n "[$dir_name/$rom_name] "
    TOTAL=$((TOTAL + 1))

    # Clear any previous temp screenshot
    rm -f "$TEMP_SCREENSHOT" 2>/dev/null

    # Capture screenshot
    "$MESEN" --testrunner "$rom_path" --lua "$SCRIPT_DIR/capture_screenshot.lua" >/dev/null 2>&1
    RESULT=$?

    if [ $RESULT -eq 0 ] && [ -f "$TEMP_SCREENSHOT" ]; then
        # Move screenshot to final location
        mv "$TEMP_SCREENSHOT" "$screenshot_path"

        # Generate hash
        if command -v md5sum &> /dev/null; then
            md5sum "$screenshot_path" | cut -d' ' -f1 > "${screenshot_path}.md5"
        elif command -v md5 &> /dev/null; then
            md5 -q "$screenshot_path" > "${screenshot_path}.md5"
        fi

        echo "OK"
        SUCCESS=$((SUCCESS + 1))
    else
        echo "FAILED (exit code: $RESULT)"
        FAILED=$((FAILED + 1))
    fi

done < <(find "$EXAMPLES_DIR" -name "*.sfc" -type f | sort)

echo ""
echo "==========================================="
echo "Results: $SUCCESS/$TOTAL screenshots captured"
echo "==========================================="

if [ $FAILED -eq 0 ]; then
    echo "All golden screenshots generated!"
    echo ""
    echo "Now commit these to your repo:"
    echo "  git add $SCREENSHOTS_DIR"
    echo "  git commit -m 'Add golden screenshots for visual regression tests'"
    exit 0
else
    echo "Some screenshots failed to capture."
    exit 1
fi
