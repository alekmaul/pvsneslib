cmake_minimum_required(VERSION 3.16)
project(PVSnesLib VERSION 4.4.0 LANGUAGES C CXX)

# Set C standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set C++ standard for tools that need it
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Pure CMake build system - no Make dependencies
option(PVSNESLIB_BUILD_EXAMPLES "Build SNES examples" ON)
option(PVSNESLIB_AUTO_SUBMODULES "Automatically initialize submodules" ON)
option(PVSNESLIB_BUILD_DOCS "Build documentation (requires Doxygen)" ON)

# Set PVSNESLIB_HOME - support both environment variable and local builds
if(DEFINED ENV{PVSNESLIB_HOME})
    set(PVSNESLIB_HOME $ENV{PVSNESLIB_HOME} CACHE PATH "PVSnesLib home directory")
    message(STATUS "Using PVSNESLIB_HOME from environment: ${PVSNESLIB_HOME}")
else()
    set(PVSNESLIB_HOME ${CMAKE_SOURCE_DIR} CACHE PATH "PVSnesLib home directory") 
    message(STATUS "Using local PVSnesLib build: ${PVSNESLIB_HOME}")
endif()

# Platform detection (mirroring existing Makefile logic)
if(WIN32)
    set(PVSNESLIB_OS "windows")
elseif(CMAKE_SYSTEM_NAME MATCHES "MINGW")
    set(PVSNESLIB_OS "mingw") 
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(PVSNESLIB_OS "darwin")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(PVSNESLIB_OS "linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    set(PVSNESLIB_OS "freebsd")
else()
    message(FATAL_ERROR "Unsupported operating system: ${CMAKE_SYSTEM_NAME}")
endif()

# Architecture detection (improved over hardcoded 64b)
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
    set(PVSNESLIB_ARCH "arm64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    set(PVSNESLIB_ARCH "x64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i386|i686")
    set(PVSNESLIB_ARCH "x86")
else()
    # Fallback to original naming for unknown architectures
    set(PVSNESLIB_ARCH "64b")
endif()

message(STATUS "Building PVSnesLib for: ${PVSNESLIB_ARCH}_${PVSNESLIB_OS}")
message(STATUS "PVSnesLib home: ${PVSNESLIB_HOME}")
message(STATUS "Detected architecture: ${CMAKE_SYSTEM_PROCESSOR} → ${PVSNESLIB_ARCH}")

message(STATUS "Using pure CMake build system")

# Include ExternalProject for handling submodules
include(ExternalProject)

# Add our CMake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Automatically initialize and update submodules if requested
if(PVSNESLIB_AUTO_SUBMODULES)
    message(STATUS "Synchronizing submodules to match superproject...")
    
    find_package(Git REQUIRED)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE GIT_RESULT
        OUTPUT_VARIABLE GIT_OUTPUT
        ERROR_VARIABLE GIT_ERROR
    )
    
    if(NOT GIT_RESULT EQUAL 0)
        message(WARNING "Failed to synchronize submodules.")
        message(WARNING "Please run manually: git submodule update --init --recursive")
        message(WARNING "Git output: ${GIT_OUTPUT}")
        message(WARNING "Git error: ${GIT_ERROR}")
    else()
        message(STATUS "Submodules synchronized successfully!")
    endif()
endif()

# Create directories that match existing structure
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/devkitsnes/bin)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/devkitsnes/tools)

# Add subdirectories in dependency order (matching existing Makefile)
add_subdirectory(compiler)
add_subdirectory(tools)
add_subdirectory(pvsneslib)

if(PVSNESLIB_BUILD_EXAMPLES)
    add_subdirectory(snes-examples)
endif()


# Documentation is handled in pvsneslib/CMakeLists.txt

#=============================================================================
# ESSENTIAL TARGETS - Only 3 Things You Need
#=============================================================================

# 1. Absolute minimum to write SNES programs and make ROMs
add_custom_target(minimal
    DEPENDS compiler tools library
    COMMENT "🔧 Minimal SNES development environment (compiler + tools + library only)"
)

# 2. Build examples only (assumes minimal is already built)
add_custom_target(examples_only
    DEPENDS examples audio_examples games_examples logo_examples sram_examples objects_examples input_examples advanced_examples
    COMMENT "🎮 Build working SNES examples (40+ ROMs)"
)

# 3. Build absolutely everything
add_custom_target(everything
    DEPENDS compiler tools library examples audio_examples games_examples logo_examples sram_examples objects_examples input_examples advanced_examples
    COMMENT "🚀 Build everything (minimal + working examples)"
)

# Note: Clean aliases (compiler, tools, library) are defined in their respective subdirectories

#=============================================================================
# RELEASE SYSTEM - Complete Package Creation
#=============================================================================

# Release target that matches and improves upon original Makefile release
add_custom_target(release
    # Prerequisites: Build everything and generate documentation
    DEPENDS everything docs
    
    # Create release directory structure
    COMMAND ${CMAKE_COMMAND} -E remove_directory release
    COMMAND ${CMAKE_COMMAND} -E make_directory release/pvsneslib
    
    # Copy core development tools (compiler and utilities)
    COMMAND ${CMAKE_COMMAND} -E copy_directory devkitsnes release/
    
    # Copy library components
    COMMAND ${CMAKE_COMMAND} -E copy_directory pvsneslib/include release/pvsneslib
    COMMAND ${CMAKE_COMMAND} -E copy_directory pvsneslib/lib release/pvsneslib
    COMMAND ${CMAKE_COMMAND} -E copy_directory pvsneslib/docs/html release/pvsneslib
    
    # Copy library metadata and documentation
    COMMAND ${CMAKE_COMMAND} -E copy pvsneslib/PVSnesLib_Logo.png release/pvsneslib/
    COMMAND ${CMAKE_COMMAND} -E copy pvsneslib/pvsneslib_license.txt release/pvsneslib/
    COMMAND ${CMAKE_COMMAND} -E copy pvsneslib/pvsneslib_snesmod.txt release/pvsneslib/
    COMMAND ${CMAKE_COMMAND} -E copy pvsneslib/pvsneslib_version.txt release/pvsneslib/
    
    # Copy examples and CMake system for external projects
    COMMAND ${CMAKE_COMMAND} -E copy_directory snes-examples release/snes-examples
    COMMAND ${CMAKE_COMMAND} -E copy_directory cmake release/cmake
    
    # Create release archive with version and platform info
    COMMAND ${CMAKE_COMMAND} -E chdir release tar czf pvsneslib_${PROJECT_VERSION}_${PVSNESLIB_ARCH}_${PVSNESLIB_OS}.tar.gz pvsneslib cmake
    
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "🚀 Creating PVSnesLib ${PROJECT_VERSION} release package for ${PVSNESLIB_ARCH}_${PVSNESLIB_OS}"
    VERBATIM
)

# Version information target
add_custom_target(version
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "🎮 PVSnesLib Version Information"
    COMMAND ${CMAKE_COMMAND} -E echo "================================="
    COMMAND ${CMAKE_COMMAND} -E echo "Library Version: ${PROJECT_VERSION}"
    COMMAND ${CMAKE_COMMAND} -E echo "Build Platform: ${PVSNESLIB_OS}"
    COMMAND ${CMAKE_COMMAND} -E echo "Architecture: ${PVSNESLIB_ARCH}"
    COMMAND ${CMAKE_COMMAND} -E echo "Release Name: pvsneslib_${PROJECT_VERSION}_${PVSNESLIB_ARCH}_${PVSNESLIB_OS}.tar.gz"
    COMMAND ${CMAKE_COMMAND} -E echo "CMake Version: ${CMAKE_VERSION}"
    COMMAND ${CMAKE_COMMAND} -E echo "Build Type: ${CMAKE_BUILD_TYPE}"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMENT "Display PVSnesLib version information"
)

#=============================================================================
# CUSTOM HELP TARGET - User-Friendly Target Overview
#=============================================================================

add_custom_target(guide
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "🎮 PVSnesLib - 3 Essential Targets"
    COMMAND ${CMAKE_COMMAND} -E echo "=================================="
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "🔧 1. MINIMAL - Absolute minimum to write SNES programs:"
    COMMAND ${CMAKE_COMMAND} -E echo "   cmake --build build --target minimal"
    COMMAND ${CMAKE_COMMAND} -E echo "   Result: C compiler + assembler + tools + library"
    COMMAND ${CMAKE_COMMAND} -E echo "   Use this: To write your own SNES programs"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "🎮 2. EXAMPLES_ONLY - Build all example ROMs:"
    COMMAND ${CMAKE_COMMAND} -E echo "   cmake --build build --target examples_only"
    COMMAND ${CMAKE_COMMAND} -E echo "   Result: 60+ example SNES ROMs"
    COMMAND ${CMAKE_COMMAND} -E echo "   Use this: To learn from existing examples"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "🚀 3. EVERYTHING - Build absolutely everything:"
    COMMAND ${CMAKE_COMMAND} -E echo "   cmake --build build --target everything"
    COMMAND ${CMAKE_COMMAND} -E echo "   Result: Complete development environment + all examples"
    COMMAND ${CMAKE_COMMAND} -E echo "   Use this: For complete setup"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "📋 QUICK START:"
    COMMAND ${CMAKE_COMMAND} -E echo "   cmake -B build -DPVSNESLIB_BUILD_EXAMPLES=ON"
    COMMAND ${CMAKE_COMMAND} -E echo "   cmake --build build --target everything"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "📖 4. DOCUMENTATION - Generate API docs [requires Doxygen]:"
    COMMAND ${CMAKE_COMMAND} -E echo "   cmake --build build --target docs"
    COMMAND ${CMAKE_COMMAND} -E echo "   cmake --build build --target docspdf     # HTML + PDF manual"
    COMMAND ${CMAKE_COMMAND} -E echo "   Result: HTML documentation in pvsneslib/docs/html/"
    COMMAND ${CMAKE_COMMAND} -E echo "   Use this: To browse the complete API reference"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "🚀 5. RELEASE - Create distribution package:"
    COMMAND ${CMAKE_COMMAND} -E echo "   cmake --build build --target release"
    COMMAND ${CMAKE_COMMAND} -E echo "   Result: pvsneslib_${PROJECT_VERSION}_${PVSNESLIB_ARCH}_${PVSNESLIB_OS}.tar.gz"
    COMMAND ${CMAKE_COMMAND} -E echo "   Use this: To create release packages for distribution"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "ℹ️  6. VERSION INFO - Display version information:"
    COMMAND ${CMAKE_COMMAND} -E echo "   cmake --build build --target version"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "🎯 INDIVIDUAL COMPONENTS - if needed:"
    COMMAND ${CMAKE_COMMAND} -E echo "   compiler, tools, library, examples, hello_world, effects, etc."
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMENT "Displaying PVSnesLib essential targets"
)

# Print build summary
message(STATUS "")
message(STATUS "PVSnesLib ${PROJECT_VERSION} build configuration:")
message(STATUS "  OS: ${PVSNESLIB_OS}")
message(STATUS "  Build examples: ${PVSNESLIB_BUILD_EXAMPLES}")
message(STATUS "  Build docs: ${PVSNESLIB_BUILD_DOCS}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
