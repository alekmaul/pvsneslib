# PVSnesLib development tools

# Set common variables
set(TOOLS_DIR "${CMAKE_SOURCE_DIR}/devkitsnes/tools")
set(VERSION "1.0.0")
string(TIMESTAMP DATESTRING "%Y%m%d")

# Create tools directory
file(MAKE_DIRECTORY ${TOOLS_DIR})

# Modern CMake approach - use properties instead of raw flags
# Common compiler definitions for tools
set(TOOL_COMPILE_DEFINITIONS 
    __BUILD_DATE="${DATESTRING}" 
    __BUILD_VERSION="${VERSION}"
)

# Common compiler features and standards
set(TOOL_C_STANDARD 99)
set(TOOL_CXX_STANDARD 11)

# Platform-specific settings
if(WIN32)
    set(EXE_EXT ".exe")
    set(TOOL_STATIC_LINKING TRUE)
else()
    set(EXE_EXT "")
    if(NOT APPLE)
        set(TOOL_STATIC_LINKING TRUE)
    else()
        set(TOOL_STATIC_LINKING FALSE)
    endif()
endif()

# Helper function to configure tool targets with modern CMake practices
function(configure_tool_target target_name)
    # Set C/C++ standard based on target type
    get_target_property(target_type ${target_name} TYPE)
    if(target_type STREQUAL "EXECUTABLE")
        # Determine if it's C or C++ based on source files
        get_target_property(sources ${target_name} SOURCES)
        set(is_cpp FALSE)
        foreach(source ${sources})
            get_filename_component(ext ${source} EXT)
            if(ext MATCHES "\\.(cpp|cxx|cc)$")
                set(is_cpp TRUE)
                break()
            endif()
        endforeach()
        
        if(is_cpp)
            set_target_properties(${target_name} PROPERTIES 
                CXX_STANDARD ${TOOL_CXX_STANDARD}
                CXX_STANDARD_REQUIRED ON
            )
        else()
            set_target_properties(${target_name} PROPERTIES 
                C_STANDARD ${TOOL_C_STANDARD}
                C_STANDARD_REQUIRED ON
            )
        endif()
    endif()
    
    # Set compile options using target properties
    target_compile_options(${target_name} PRIVATE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Release>:-O2>
        -Wall
        $<$<COMPILE_LANGUAGE:C>:-Wno-implicit-function-declaration>
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-result>
    )
    
    # Set compile definitions
    target_compile_definitions(${target_name} PRIVATE ${TOOL_COMPILE_DEFINITIONS})
    
    # Set linking options
    if(TOOL_STATIC_LINKING)
        target_link_options(${target_name} PRIVATE -static)
    endif()
    
    # Install target
    install(TARGETS ${target_name} RUNTIME DESTINATION ${TOOLS_DIR})
endfunction()

# 816-opt: Assembly optimizer
add_executable(816-opt
    816-opt/src/main.c
    816-opt/src/optimizer.c
    816-opt/src/helpers.c
)

configure_tool_target(816-opt)
target_link_libraries(816-opt PRIVATE pthread)

if(WIN32)
    target_compile_definitions(816-opt PRIVATE PCRE2_STATIC)
    target_link_libraries(816-opt PRIVATE pcre2-posix pcre2-8 iconv)
endif()

# bin2txt: Binary to text converter  
add_executable(bin2txt
    bin2txt/bin2txt.c
)

configure_tool_target(bin2txt)

# constify: Constant optimizer
add_executable(constify
    constify/constify.cpp
)

configure_tool_target(constify)

# gfx2snes: Graphics converter (legacy)
add_executable(gfx2snes
    gfx2snes/gfx2snes.c
    gfx2snes/imgtools.c
    gfx2snes/loadimg.c
    gfx2snes/lodepng.c
    gfx2snes/lz77.c
)

configure_tool_target(gfx2snes)
target_include_directories(gfx2snes PRIVATE gfx2snes)

# gfx4snes: Modern graphics converter
file(GLOB GFX4SNES_SOURCES gfx4snes/src/*.c)
add_executable(gfx4snes ${GFX4SNES_SOURCES})

configure_tool_target(gfx4snes)
target_include_directories(gfx4snes PRIVATE gfx4snes/src)

# smconv: Music converter
add_executable(smconv
    smconv/brr.cpp
    smconv/conversion.cpp
    smconv/convert.cpp
    smconv/inputdata.cpp
    smconv/io.cpp
    smconv/it2spc.cpp
    smconv/itloader.cpp
)

configure_tool_target(smconv)
target_include_directories(smconv PRIVATE smconv)

# snesbrr: BRR audio converter
add_executable(snesbrr
    snesbrr/base/FileStream.cpp
    snesbrr/base/MemoryStream.cpp
    snesbrr/base/OptionParser.cpp
    snesbrr/base/Stream.cpp
    snesbrr/base/StreamException.cpp
    snesbrr/brr/BrrCodec.cpp
    snesbrr/brr/main.cpp
)

configure_tool_target(snesbrr)
target_include_directories(snesbrr PRIVATE snesbrr snesbrr/base snesbrr/brr)

# snestools: General SNES utilities
add_executable(snestools
    snestools/snestools.c
    snestools/errors.c
)

configure_tool_target(snestools)

# tmx2snes: Tiled map converter
add_executable(tmx2snes
    tmx2snes/tmx2snes.c
)

configure_tool_target(tmx2snes)
target_include_directories(tmx2snes PRIVATE tmx2snes)

# Create combined target for all tools
add_custom_target(pvsneslib_tools
    DEPENDS 816-opt bin2txt constify gfx2snes gfx4snes smconv snesbrr snestools tmx2snes
    COMMENT "Building PVSnesLib development tools"
)

# Create clean alias
add_custom_target(tools
    DEPENDS pvsneslib_tools
    COMMENT "🛠️ SNES development tools"
)

# Ensure compiler tools are built first (if they exist)
if(TARGET pvsneslib_compiler)
    add_dependencies(pvsneslib_tools pvsneslib_compiler)
endif()

# Copy tools to devkitsnes/tools (matching existing behavior)
add_custom_command(TARGET pvsneslib_tools POST_BUILD
    COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR} --prefix ${CMAKE_SOURCE_DIR}
    COMMENT "Installing tools to devkitsnes/tools"
)
