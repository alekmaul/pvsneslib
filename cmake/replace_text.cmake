# Cross-platform text replacement utility for CMake
# Usage: cmake -DINPUT_FILE=input.txt -DOUTPUT_FILE=output.txt -DPATTERN="old" -DREPLACEMENT="new" -P replace_text.cmake

if(NOT INPUT_FILE OR NOT OUTPUT_FILE OR NOT DEFINED PATTERN OR NOT DEFINED REPLACEMENT)
    message(FATAL_ERROR "Usage: cmake -DINPUT_FILE=input.txt -DOUTPUT_FILE=output.txt -DPATTERN=\"old\" -DREPLACEMENT=\"new\" -P replace_text.cmake")
endif()

# Read the input file
file(READ "${INPUT_FILE}" FILE_CONTENT)

# Perform the replacement - use string(REPLACE) for literal replacement to avoid regex escaping issues
if(PATTERN MATCHES ".*hdr.*")
    # Special handling for hdr.asm patterns
    string(REPLACE ".include \"hdr.asm\"" "${REPLACEMENT}" FILE_CONTENT "${FILE_CONTENT}")
    string(REPLACE ".INCLUDE \"hdr.asm\"" "${REPLACEMENT}" FILE_CONTENT "${FILE_CONTENT}")
else()
    # Use regex replace for other patterns
    string(REGEX REPLACE "${PATTERN}" "${REPLACEMENT}" FILE_CONTENT "${FILE_CONTENT}")
endif()

# Write to output file
file(WRITE "${OUTPUT_FILE}" "${FILE_CONTENT}")
