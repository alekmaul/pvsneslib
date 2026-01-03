#!/bin/bash
# TMX2SNES Locked Layer Test (Issue #318)
# Tests that tmx2snes can parse TMX files with locked layers

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

# Check for PVSNESLIB_HOME
if [ -z "$PVSNESLIB_HOME" ]; then
    echo "ERROR: PVSNESLIB_HOME not set"
    exit 1
fi

TMX2SNES="$PVSNESLIB_HOME/devkitsnes/tools/tmx2snes"
GFXCONV="$PVSNESLIB_HOME/devkitsnes/tools/gfx4snes"

if [ ! -x "$TMX2SNES" ]; then
    echo "ERROR: tmx2snes not found at $TMX2SNES"
    exit 1
fi

if [ ! -x "$GFXCONV" ]; then
    echo "ERROR: gfx4snes not found at $GFXCONV"
    exit 1
fi

echo "=== TMX2SNES Locked Layer Test ==="

# Clean previous output
rm -f test_tileset.pic test_tileset.pal test_tileset.map
rm -f Background.m16 Background.t16 test_locked_layer.b16

# Generate tileset.map from tileset PNG
echo "Generating tileset.map from test_tileset.png..."
"$GFXCONV" -s 8 -o 48 -u 16 -p -m -i test_tileset.png

if [ ! -f "test_tileset.map" ]; then
    echo "FAIL: test_tileset.map not created by gfx4snes"
    exit 1
fi
echo "PASS: test_tileset.map created"

# Run tmx2snes on the test file with locked layer (JSON format)
echo "Running tmx2snes on test_locked_layer.tmj (has locked layer)..."

# Run with timeout (works on both macOS and Linux)
if command -v timeout &> /dev/null; then
    # Linux
    if timeout 10 "$TMX2SNES" -q test_locked_layer.tmj test_tileset.map; then
        echo "PASS: tmx2snes completed without hanging"
    else
        EXIT_CODE=$?
        if [ $EXIT_CODE -eq 124 ]; then
            echo "FAIL: tmx2snes timed out (hung on locked layer)"
            exit 1
        else
            echo "FAIL: tmx2snes returned error code $EXIT_CODE"
            exit 1
        fi
    fi
else
    # macOS - run in background and check
    "$TMX2SNES" -q test_locked_layer.tmj test_tileset.map &
    TMX_PID=$!
    sleep 5
    if kill -0 $TMX_PID 2>/dev/null; then
        # Still running after 5 seconds - likely hung
        kill $TMX_PID 2>/dev/null
        echo "FAIL: tmx2snes timed out (hung on locked layer)"
        exit 1
    else
        # Process finished
        wait $TMX_PID
        EXIT_CODE=$?
        if [ $EXIT_CODE -eq 0 ]; then
            echo "PASS: tmx2snes completed without hanging"
        else
            echo "FAIL: tmx2snes returned error code $EXIT_CODE"
            exit 1
        fi
    fi
fi

# Check that output files were created
if [ -f "Background.m16" ]; then
    echo "PASS: Background.m16 created"
else
    echo "FAIL: Background.m16 not created"
    exit 1
fi

if [ -f "test_locked_layer.b16" ]; then
    echo "PASS: test_locked_layer.b16 created"
else
    echo "FAIL: test_locked_layer.b16 not created"
    exit 1
fi

# Check file sizes are reasonable (not empty)
M16_SIZE=$(stat -f%z "Background.m16" 2>/dev/null || stat -c%s "Background.m16" 2>/dev/null)
if [ "$M16_SIZE" -gt 10 ]; then
    echo "PASS: Background.m16 has content ($M16_SIZE bytes)"
else
    echo "FAIL: Background.m16 is too small ($M16_SIZE bytes)"
    exit 1
fi

echo ""
echo "=== All TMX2SNES tests PASSED ==="

# Cleanup
rm -f test_tileset.pic test_tileset.pal test_tileset.map
rm -f Background.m16 Background.t16 test_locked_layer.b16

exit 0
