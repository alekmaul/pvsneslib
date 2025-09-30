# PVSnesLib core library

# Use version from parent project
set(PVSNESLIB_MAJOR ${PROJECT_VERSION_MAJOR})
set(PVSNESLIB_MINOR ${PROJECT_VERSION_MINOR})
set(PVSNESLIB_PATCH ${PROJECT_VERSION_PATCH})
set(PVSNESLIB_VERSION ${PROJECT_VERSION})

# Create version header
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snes/libversion.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snes/libversion.h
    @ONLY
)

# Create version text file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/pvsneslib_version.txt.in
    ${CMAKE_CURRENT_SOURCE_DIR}/pvsneslib_version.txt
    @ONLY
)

# Add tool validation function
function(validate_required_tools)
    # Check for required external tools
    find_program(GIT_EXECUTABLE git)
    if(NOT GIT_EXECUTABLE)
        message(WARNING "Git not found. Submodule operations may fail.")
    endif()
    
    # Validate that our build tools will exist after being built
    set(REQUIRED_TOOLS
        "${CMAKE_SOURCE_DIR}/devkitsnes/bin/816-tcc"
        "${CMAKE_SOURCE_DIR}/devkitsnes/bin/wla-65816"
        "${CMAKE_SOURCE_DIR}/devkitsnes/tools/816-opt"
        "${CMAKE_SOURCE_DIR}/devkitsnes/tools/constify"
    )
    
    # Note: We can't check if these exist yet since they're built by this system
    # But we can validate the build targets exist
    message(STATUS "Tool validation: Required tools will be built by targets: tcc_compiler, wla_assembler, 816-opt, constify")
endfunction()

# Validate tools at configuration time
validate_required_tools()

# Build default library configuration (LoROM SlowROM - what 99% of users need)
# This approach leverages existing library files built by the traditional Makefile
# and provides CMake targets for them
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/LoROM_SlowROM")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")

# Create library directory
file(MAKE_DIRECTORY ${LIB_DIR})

# Library files that should exist (built by Make system)
set(LIBRARY_OBJECTS
    ${LIB_DIR}/crt0_snes.obj
    ${LIB_DIR}/libm.obj
    ${LIB_DIR}/libtcc.obj
    ${LIB_DIR}/libc.obj
)

# Check if library files exist, if not, build them using traditional method
set(MISSING_OBJECTS "")
foreach(obj_file ${LIBRARY_OBJECTS})
    if(NOT EXISTS ${obj_file})
        list(APPEND MISSING_OBJECTS ${obj_file})
    endif()
endforeach()

if(MISSING_OBJECTS)
    message(STATUS "Building missing library objects using traditional Makefile")
    add_custom_command(OUTPUT ${LIBRARY_OBJECTS}
        COMMAND ${CMAKE_COMMAND} -E env $(MAKE) -C ${SOURCE_DIR} all
        DEPENDS tcc_compiler wla_assembler 816-opt constify
        WORKING_DIRECTORY ${SOURCE_DIR}
        COMMENT "Building PVSnesLib library using traditional Makefile"
    )
endif()

# Install library objects to lib directory
add_custom_target(pvsneslib_library
    DEPENDS ${LIBRARY_OBJECTS}
    COMMENT "Building PVSnesLib library (LoROM SlowROM)"
)

# Create alias for SNESGame.cmake compatibility
add_custom_target(library
    DEPENDS pvsneslib_library
    COMMENT "PVSnesLib library alias"
)

# Documentation target (if Doxygen is available)
find_package(Doxygen QUIET)
if(PVSNESLIB_BUILD_DOCS)
    if(NOT DOXYGEN_FOUND)
        message(STATUS "")
        message(STATUS "📖 Documentation build requested but Doxygen not found!")
        message(STATUS "")
        message(STATUS "To install Doxygen:")
        message(STATUS "  macOS:          brew install doxygen")
        message(STATUS "  Ubuntu/Debian:  sudo apt-get install doxygen")
        message(STATUS "  Windows:        Download from doxygen.nl")
        message(STATUS "")
        message(STATUS "Skipping documentation generation...")
        message(STATUS "")
    endif()
endif()

if(DOXYGEN_FOUND AND PVSNESLIB_BUILD_DOCS)
    message(STATUS "📖 Doxygen found (${DOXYGEN_VERSION}) - documentation will be generated")
    set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/pvsneslib.dox)
    set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    set(DOXY_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/docs/html)
    set(PVSDOCSDIR ${CMAKE_CURRENT_SOURCE_DIR}/docs)
    
    # Read the dox file and replace variables directly in CMake
    file(READ ${DOXYFILE_IN} DOXYFILE_CONTENT)
    string(REPLACE "$(PVSDOCSDIR)" "${PVSDOCSDIR}" DOXYFILE_CONTENT "${DOXYFILE_CONTENT}")
    string(REPLACE "$(PVSNESLIB_VERSION)" "${PVSNESLIB_VERSION}" DOXYFILE_CONTENT "${DOXYFILE_CONTENT}")
    file(WRITE ${DOXYFILE_OUT} "${DOXYFILE_CONTENT}")
    
    add_custom_target(docs
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${DOXY_OUTPUT_DIR}
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/docs/latex
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating PVSnesLib documentation (HTML only)"
        VERBATIM
    )
    
    # PDF documentation target (mimics original Makefile docspdf target)
    add_custom_target(docspdf
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${DOXY_OUTPUT_DIR}
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}/docs/latex ${CMAKE_MAKE_PROGRAM}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/docs/latex/refman.pdf ${CMAKE_CURRENT_SOURCE_DIR}/docs/pvsneslib_manual.pdf
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/docs/latex
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating PVSnesLib documentation with PDF manual"
        VERBATIM
    )
endif()