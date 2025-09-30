# Compiler tools: TCC (816-tcc) and WLA-DX (wla-65816, wla-spc700, wlalink)

# Set variables matching existing Makefile
set(TCC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tcc")
set(WLA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/wla-dx")
set(BIN_DIR "${CMAKE_SOURCE_DIR}/devkitsnes/bin")

# Platform-specific settings (matching existing Makefile)
if(WIN32)
    set(CMAKE_TYPE "MSYS Makefiles")
else()
    set(CMAKE_TYPE "Unix Makefiles")
endif()

# TCC Compiler (816-tcc)
# Check if TCC submodule is initialized
if(EXISTS ${TCC_DIR}/configure)
    message(STATUS "TCC submodule found, building 816-tcc compiler")
    ExternalProject_Add(tcc_compiler
        SOURCE_DIR ${TCC_DIR}
        CONFIGURE_COMMAND ${CMAKE_COMMAND} -E chdir ${TCC_DIR} ./configure
        BUILD_COMMAND ${CMAKE_COMMAND} -E chdir ${TCC_DIR} $(MAKE)
        INSTALL_COMMAND ${CMAKE_COMMAND} -E copy ${TCC_DIR}/816-tcc ${BIN_DIR}/816-tcc
        BUILD_IN_SOURCE TRUE
        BUILD_ALWAYS FALSE
    )
else()
    message(WARNING "TCC submodule not initialized. Run: git submodule update --init --recursive")
    # Create a dummy target that fails with helpful message
    add_custom_target(tcc_compiler
        COMMAND ${CMAKE_COMMAND} -E echo "Error: TCC submodule not initialized"
        COMMAND ${CMAKE_COMMAND} -E echo "Please run: git submodule update --init --recursive"
        COMMAND ${CMAKE_COMMAND} -E false
        COMMENT "TCC submodule missing"
    )
endif()

# WLA-DX Assembler (already uses CMake!)
# Check if WLA-DX submodule is initialized
if(EXISTS ${WLA_DIR}/CMakeLists.txt)
    message(STATUS "WLA-DX submodule found, building assembler tools")
    ExternalProject_Add(wla_assembler
        SOURCE_DIR ${WLA_DIR}
        CMAKE_ARGS 
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -G "${CMAKE_TYPE}"
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target wla-65816 wla-spc700 wlalink
    INSTALL_COMMAND 
        ${CMAKE_COMMAND} -E copy <BINARY_DIR>/binaries/wla-65816 ${BIN_DIR}/wla-65816
    COMMAND
        ${CMAKE_COMMAND} -E copy <BINARY_DIR>/binaries/wla-spc700 ${BIN_DIR}/wla-spc700
    COMMAND
        ${CMAKE_COMMAND} -E copy <BINARY_DIR>/binaries/wlalink ${BIN_DIR}/wlalink
        BUILD_ALWAYS FALSE
    )
else()
    message(WARNING "WLA-DX submodule not initialized. Run: git submodule update --init --recursive")
    # Create a dummy target that fails with helpful message
    add_custom_target(wla_assembler
        COMMAND ${CMAKE_COMMAND} -E echo "Error: WLA-DX submodule not initialized"
        COMMAND ${CMAKE_COMMAND} -E echo "Please run: git submodule update --init --recursive"
        COMMAND ${CMAKE_COMMAND} -E false
        COMMENT "WLA-DX submodule missing"
    )
endif()

# Create a combined target for all compiler tools
add_custom_target(pvsneslib_compiler
    DEPENDS tcc_compiler wla_assembler
    COMMENT "Building PVSnesLib compiler tools"
)

# Clean target for compiler
add_custom_target(clean_compiler
    COMMAND ${CMAKE_COMMAND} -E chdir ${TCC_DIR} $(MAKE) clean || true
    COMMAND ${CMAKE_COMMAND} -E chdir ${WLA_DIR} $(MAKE) clean || true
    COMMAND ${CMAKE_COMMAND} -E remove -f ${WLA_DIR}/CMakeCache.txt
    COMMENT "Cleaning compiler tools"
)

# Create clean alias for compiler
add_custom_target(compiler
    DEPENDS pvsneslib_compiler
    COMMENT "🔧 SNES C compiler and assembler"
)

# Note: Submodule updates should be done manually with:
# git submodule update --remote --merge tcc
# git submodule update --remote --merge wla-dx
