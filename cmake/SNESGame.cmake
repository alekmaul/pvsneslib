# Helper functions for building SNES games with PVSnesLib

# Function to add a SNES game target
# Usage: add_snes_game(target_name 
#           SOURCES source1.c source2.c ...
#           GRAPHICS sprite.png background.bmp ...
#           AUDIO music.it sound.wav ...
#           ROM_NAME output_name
#           HIROM ON/OFF
#           FASTROM ON/OFF
#        )
function(add_snes_game target_name)
    cmake_parse_arguments(SNES 
        "HIROM;FASTROM" 
        "ROM_NAME;SOUNDBANK" 
        "SOURCES;GRAPHICS;AUDIO;ASM_SOURCES;DATA_FILES;AUDIO_FILES;INCLUDE_DIRS"
        ${ARGN}
    )
    
    if(NOT SNES_ROM_NAME)
        set(SNES_ROM_NAME ${target_name})
    endif()
    
    if(NOT SNES_SOURCES)
        message(FATAL_ERROR "add_snes_game: No SOURCES specified for ${target_name}")
    endif()
    
    # Determine library configuration
    if(SNES_HIROM)
        set(ROM_TYPE "HiROM")
        set(TCC_HIROM_FLAG "-H")
    else()
        set(ROM_TYPE "LoROM")
        set(TCC_HIROM_FLAG "")
    endif()
    
    if(SNES_FASTROM)
        set(ROM_SPEED "FastROM") 
        set(TCC_FASTROM_FLAG "-F")
    else()
        set(ROM_SPEED "SlowROM")
        set(TCC_FASTROM_FLAG "")
    endif()
    
    set(LIB_CONFIG "${ROM_TYPE}_${ROM_SPEED}")
    
    # Get PVSNESLIB_HOME - either from environment or parent project
    if(DEFINED ENV{PVSNESLIB_HOME})
        set(PVSNESLIB_HOME_PATH $ENV{PVSNESLIB_HOME})
    elseif(DEFINED PVSNESLIB_HOME)
        set(PVSNESLIB_HOME_PATH ${PVSNESLIB_HOME})
    else()
        message(FATAL_ERROR "PVSNESLIB_HOME not found. Please set PVSNESLIB_HOME environment variable or ensure you're building within PVSnesLib source tree.")
    endif()
    
    # Set up paths using PVSNESLIB_HOME
    set(TCC_COMPILER "${PVSNESLIB_HOME_PATH}/devkitsnes/bin/816-tcc")
    set(WLA_ASSEMBLER "${PVSNESLIB_HOME_PATH}/devkitsnes/bin/wla-65816")
    set(WLA_LINKER "${PVSNESLIB_HOME_PATH}/devkitsnes/bin/wlalink")
    set(OPTIMIZER "${PVSNESLIB_HOME_PATH}/devkitsnes/tools/816-opt")
    set(CONSTIFY "${PVSNESLIB_HOME_PATH}/devkitsnes/tools/constify")
    
    set(LIB_DIR "${PVSNESLIB_HOME_PATH}/pvsneslib/lib/${LIB_CONFIG}")
    
    # Include directories
    set(INCLUDE_DIRS
        -I${PVSNESLIB_HOME_PATH}/pvsneslib/include
        -I${PVSNESLIB_HOME_PATH}/devkitsnes/include
        -I${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    # Add additional include directories if specified
    foreach(inc_dir ${SNES_INCLUDE_DIRS})
        list(APPEND INCLUDE_DIRS -I${CMAKE_CURRENT_SOURCE_DIR}/${inc_dir})
    endforeach()
    
    # Simplified path management - determine source directory from first source file
    # Default to target name directory if no sources or source is in current dir
    set(target_source_dir "${CMAKE_CURRENT_SOURCE_DIR}")
    if(SNES_SOURCES)
        list(GET SNES_SOURCES 0 first_source)
        get_filename_component(source_dir ${first_source} DIRECTORY)
        if(source_dir)
            get_filename_component(target_source_dir "${CMAKE_CURRENT_SOURCE_DIR}/${source_dir}" ABSOLUTE)
        endif()
    endif()
    
    # Validate that the source directory exists
    if(NOT EXISTS "${target_source_dir}")
        message(WARNING "Source directory ${target_source_dir} does not exist for target ${target_name}")
        set(target_source_dir "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()
    
    # Convert graphics files
    set(CONVERTED_GRAPHICS "")
    foreach(gfx ${SNES_GRAPHICS})
        get_filename_component(gfx_name ${gfx} NAME_WE)
        get_filename_component(gfx_ext ${gfx} EXT)
        get_filename_component(gfx_full_name ${gfx} NAME)
        get_filename_component(gfx_dir ${gfx} DIRECTORY)
        
        # Check if pre-converted files exist (including Mode7 formats)
        set(existing_pic "${CMAKE_CURRENT_SOURCE_DIR}/${gfx_dir}/${gfx_name}.pic")
        set(existing_pal "${CMAKE_CURRENT_SOURCE_DIR}/${gfx_dir}/${gfx_name}.pal")
        set(existing_pc7 "${CMAKE_CURRENT_SOURCE_DIR}/${gfx_dir}/${gfx_name}.pc7")
        set(existing_mp7 "${CMAKE_CURRENT_SOURCE_DIR}/${gfx_dir}/${gfx_name}.mp7")
        
        set(gfx_output "${target_name}_${gfx_full_name}.pic")
        set(pal_output "${target_name}_${gfx_full_name}.pal")
        
        if(EXISTS ${existing_pc7} AND EXISTS ${existing_mp7})
            # Use existing Mode7 converted files
            set(gfx_output "${target_name}_${gfx_full_name}.pc7")
            set(pal_output "${target_name}_${gfx_full_name}.mp7")
            add_custom_command(OUTPUT ${gfx_output} ${pal_output}
                COMMAND ${CMAKE_COMMAND} -E copy ${existing_pc7} ${CMAKE_CURRENT_BINARY_DIR}/${gfx_output}
                COMMAND ${CMAKE_COMMAND} -E copy ${existing_mp7} ${CMAKE_CURRENT_BINARY_DIR}/${pal_output}
                DEPENDS ${existing_pc7} ${existing_mp7}
                COMMENT "Using existing Mode7 graphics ${gfx_name}"
            )
        elseif(EXISTS ${existing_pic} AND EXISTS ${existing_pal})
            # Use existing converted files
            add_custom_command(OUTPUT ${gfx_output} ${pal_output}
                COMMAND ${CMAKE_COMMAND} -E copy ${existing_pic} ${CMAKE_CURRENT_BINARY_DIR}/${gfx_output}
                COMMAND ${CMAKE_COMMAND} -E copy ${existing_pal} ${CMAKE_CURRENT_BINARY_DIR}/${pal_output}
                DEPENDS ${existing_pic} ${existing_pal}
                COMMENT "Using existing converted graphics ${gfx_name}"
            )
        elseif(gfx_ext STREQUAL ".png")
            # PNG files - use gfx4snes - include full filename to avoid conflicts
            get_filename_component(gfx_absolute ${CMAKE_CURRENT_SOURCE_DIR}/${gfx} ABSOLUTE)
            get_filename_component(gfx_dir_abs ${gfx_absolute} DIRECTORY)
            add_custom_command(OUTPUT ${gfx_output} ${pal_output}
                COMMAND ${PVSNESLIB_HOME_PATH}/devkitsnes/tools/gfx4snes -s 8 -o 16 -u 16 -p -e 0 -i ${gfx_absolute}
                COMMAND ${CMAKE_COMMAND} -E copy ${gfx_dir_abs}/${gfx_name}.pic ${CMAKE_CURRENT_BINARY_DIR}/${gfx_output}
                COMMAND ${CMAKE_COMMAND} -E copy ${gfx_dir_abs}/${gfx_name}.pal ${CMAKE_CURRENT_BINARY_DIR}/${pal_output}
                DEPENDS ${gfx_absolute}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Converting ${gfx} (PNG) to SNES format"
            )
        elseif(gfx_ext STREQUAL ".bmp")
            # BMP files - use gfx2snes (which properly supports BMP) - include full filename to avoid conflicts
            get_filename_component(gfx_absolute ${CMAKE_CURRENT_SOURCE_DIR}/${gfx} ABSOLUTE)
            get_filename_component(gfx_dir_abs ${gfx_absolute} DIRECTORY)
            add_custom_command(OUTPUT ${gfx_output} ${pal_output}
                COMMAND ${PVSNESLIB_HOME_PATH}/devkitsnes/tools/gfx2snes -pc16 -gs8 -pe0 -p -m -fbmp ${gfx_absolute}
                COMMAND ${CMAKE_COMMAND} -E copy ${gfx_dir_abs}/${gfx_name}.pic ${CMAKE_CURRENT_BINARY_DIR}/${gfx_output}
                COMMAND ${CMAKE_COMMAND} -E copy ${gfx_dir_abs}/${gfx_name}.pal ${CMAKE_CURRENT_BINARY_DIR}/${pal_output}
                DEPENDS ${gfx_absolute}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Converting ${gfx} (BMP) to SNES format"
            )
        endif()
        list(APPEND CONVERTED_GRAPHICS ${gfx_output} ${pal_output})
    endforeach()
    
    # Convert audio files and generate soundbank
    set(CONVERTED_AUDIO "")
    set(SOUNDBANK_DEPS "")
    if(SNES_AUDIO_FILES AND SNES_SOUNDBANK)
        # Generate soundbank from IT files using smconv - make unique per target
        get_filename_component(soundbank_name ${SNES_SOUNDBANK} NAME)
        string(REPLACE "/" "_" soundbank_unique "${SNES_SOUNDBANK}")
        set(soundbank_asm "${target_name}_${soundbank_unique}.asm")
        set(soundbank_obj "${target_name}_${soundbank_unique}.obj") 
        set(soundbank_bnk "${target_name}_${soundbank_unique}.bnk")
        set(soundbank_h "${target_name}_${soundbank_unique}.h")
        
        # Create smconv command with all audio files
        set(SMCONV_CMD "${PVSNESLIB_HOME_PATH}/devkitsnes/tools/smconv")
        set(SMCONV_ARGS "-s" "-o" "${SNES_SOUNDBANK}" "-V" "-b" "5")
        
        # Generate soundbank and copy object file to build directory in one step
        set(unique_soundbank_obj "${CMAKE_CURRENT_BINARY_DIR}/${soundbank_obj}")
        add_custom_command(OUTPUT ${unique_soundbank_obj} ${soundbank_asm} ${soundbank_bnk} ${soundbank_h}
            COMMAND ${SMCONV_CMD} ${SMCONV_ARGS} ${SNES_AUDIO_FILES}
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${SNES_SOUNDBANK}.obj ${unique_soundbank_obj}
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${SNES_SOUNDBANK}.h ${target_source_dir}/soundbank.h
            DEPENDS ${SNES_AUDIO_FILES}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating soundbank from ${SNES_AUDIO_FILES}"
        )
        
        list(APPEND SOUNDBANK_DEPS ${unique_soundbank_obj})
        list(APPEND CONVERTED_AUDIO ${soundbank_asm})
        list(APPEND OBJ_FILES ${unique_soundbank_obj})
    endif()
    
    # Legacy audio conversion for individual files
    foreach(audio ${SNES_AUDIO})
        get_filename_component(audio_name ${audio} NAME_WE)
        get_filename_component(audio_ext ${audio} EXT)
        
        if(audio_ext STREQUAL ".it" OR audio_ext STREQUAL ".mod")
            set(audio_output "${audio_name}.bnk")
            add_custom_command(OUTPUT ${audio_output}
                COMMAND ${PVSNESLIB_HOME_PATH}/devkitsnes/tools/smconv ${audio}
                DEPENDS ${audio}
                COMMENT "Converting ${audio} to SNES format"
            )
            list(APPEND CONVERTED_AUDIO ${audio_output})
        endif()
    endforeach()
    
    # Handle data files (.dat files) - copy them to build directory
    set(CONVERTED_DATA "")
    foreach(data_file ${SNES_DATA_FILES})
        get_filename_component(data_name ${data_file} NAME)
        set(data_output "${target_name}_${data_name}")
        add_custom_command(OUTPUT ${data_output}
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${data_file} ${data_output}
            DEPENDS ${data_file}
            COMMENT "Copying data file ${data_file}"
        )
        list(APPEND CONVERTED_DATA ${data_output})
    endforeach()
    
    # Compile C sources to assembly
    set(ASM_FILES "")
    foreach(src ${SNES_SOURCES})
        get_filename_component(src_name ${src} NAME_WE)
        # Make output files unique per target to avoid conflicts
        set(ps_file "${target_name}_${src_name}.ps")
        set(asm_file "${target_name}_${src_name}.asm")
        set(obj_file "${target_name}_${src_name}.obj")
        
        # C -> .ps (intermediate) - use absolute paths
        get_filename_component(src_absolute ${src} ABSOLUTE)
        add_custom_command(OUTPUT ${ps_file}
            COMMAND ${TCC_COMPILER} ${INCLUDE_DIRS} -Wall -c ${src_absolute} ${TCC_HIROM_FLAG} ${TCC_FASTROM_FLAG} -o ${ps_file}
            DEPENDS ${src_absolute}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Compiling ${src} to .ps"
        )
        
        # .ps -> .asm (optimized)
        add_custom_command(OUTPUT ${asm_file}
            COMMAND ${OPTIMIZER} ${ps_file} > ${src_name}.asp
            COMMAND ${CONSTIFY} ${src_absolute} ${src_name}.asp ${asm_file}
            COMMAND ${CMAKE_COMMAND} -E remove ${src_name}.asp
            DEPENDS ${ps_file}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Optimizing ${ps_file} to ${asm_file}"
        )
        
        # .asm -> .obj (copy necessary includes to build dir)
        # Look for hdr.asm and data.asm using a helper function
        function(find_asm_file filename result_var)
            # First check in ASM_SOURCES list
            foreach(asm_src ${SNES_ASM_SOURCES})
                get_filename_component(asm_name ${asm_src} NAME)
                if(asm_name STREQUAL ${filename})
                    get_filename_component(full_path "${CMAKE_CURRENT_SOURCE_DIR}/${asm_src}" ABSOLUTE)
                    if(EXISTS "${full_path}")
                        set(${result_var} "${full_path}" PARENT_SCOPE)
                        return()
                    endif()
                endif()
            endforeach()
            
            # Then check in target source directory
            set(fallback_path "${target_source_dir}/${filename}")
            if(EXISTS "${fallback_path}")
                set(${result_var} "${fallback_path}" PARENT_SCOPE)
            else()
                message(WARNING "Required ASM file ${filename} not found for target ${target_name}")
                set(${result_var} "${fallback_path}" PARENT_SCOPE)  # Use anyway, might be generated
            endif()
        endfunction()
        
        find_asm_file("hdr.asm" hdr_asm_path)
        find_asm_file("data.asm" data_asm_path)
        
        add_custom_command(OUTPUT ${obj_file}
            COMMAND ${CMAKE_COMMAND} -E copy ${hdr_asm_path} ${CMAKE_CURRENT_BINARY_DIR}/hdr.asm
            COMMAND ${CMAKE_COMMAND} -E copy ${data_asm_path} ${CMAKE_CURRENT_BINARY_DIR}/data.asm
            COMMAND ${WLA_ASSEMBLER} -d -s -x -o ${obj_file} ${asm_file}
            DEPENDS ${asm_file}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Assembling ${asm_file} to ${obj_file}"
        )
        
        list(APPEND ASM_FILES ${asm_file})
        list(APPEND OBJ_FILES ${obj_file})
    endforeach()
    
    # Handle additional ASM sources - make output files unique per target
    foreach(asm ${SNES_ASM_SOURCES})
        get_filename_component(asm_name ${asm} NAME_WE)
        get_filename_component(asm_absolute ${asm} ABSOLUTE)
        get_filename_component(asm_dir ${asm_absolute} DIRECTORY)
        set(obj_file "${target_name}_${asm_name}.obj")
        
        add_custom_command(OUTPUT ${obj_file}
            COMMAND ${WLA_ASSEMBLER} -d -s -x -o ${CMAKE_CURRENT_BINARY_DIR}/${obj_file} ${asm_absolute}
            DEPENDS ${asm_absolute}
            WORKING_DIRECTORY ${asm_dir}
            COMMENT "Assembling ${asm} to ${obj_file}"
        )
        
        list(APPEND OBJ_FILES ${obj_file})
    endforeach()
    
    # Create linkfile and link ROM
    set(ROM_FILE "${target_source_dir}/${SNES_ROM_NAME}.sfc")
    set(LINKFILE "${target_name}_linkfile")
    set(SYM_FILE "${target_name}_${SNES_ROM_NAME}.sym")
    
    # Create linkfile generation script
    set(LINKFILE_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/create_${target_name}_linkfile.cmake")
    file(WRITE ${LINKFILE_SCRIPT}
        "file(WRITE \"${LINKFILE}\" \"[objects]\\n\")\n"
    )
    foreach(obj_file ${OBJ_FILES})
        file(APPEND ${LINKFILE_SCRIPT}
            "file(APPEND \"${LINKFILE}\" \"${obj_file}\\n\")\n"
        )
    endforeach()
    # Add library objects
    file(APPEND ${LINKFILE_SCRIPT}
        "file(APPEND \"${LINKFILE}\" \"${LIB_DIR}/crt0_snes.obj\\n\")\n"
        "file(APPEND \"${LINKFILE}\" \"${LIB_DIR}/libm.obj\\n\")\n"
        "file(APPEND \"${LINKFILE}\" \"${LIB_DIR}/libtcc.obj\\n\")\n"
        "file(APPEND \"${LINKFILE}\" \"${LIB_DIR}/libc.obj\\n\")\n"
    )

    add_custom_command(OUTPUT ${ROM_FILE}
        # Create linkfile
        COMMAND ${CMAKE_COMMAND} -P ${LINKFILE_SCRIPT}
        
        # Link ROM
        COMMAND ${WLA_LINKER} -d -s -v -A -c -L ${LIB_DIR} ${LINKFILE} ${ROM_FILE}
        
        # Clean up
        COMMAND ${CMAKE_COMMAND} -E remove ${LINKFILE}
        
        DEPENDS ${OBJ_FILES} ${CONVERTED_GRAPHICS} ${CONVERTED_AUDIO} ${CONVERTED_DATA} ${SOUNDBANK_DEPS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Linking ${ROM_FILE}"
    )
    
    # Create the target
    add_custom_target(${target_name}
        DEPENDS ${ROM_FILE}
        COMMENT "Building SNES game: ${SNES_ROM_NAME}"
    )
    
    # Note: Individual clean targets removed for simplicity
    # Use 'cmake --build build --target clean' to clean everything
    
endfunction()

# Helper function for simple SNES games (most common case)
function(add_simple_snes_game target_name)
    cmake_parse_arguments(SIMPLE "" "ROM_NAME" "SOURCES" ${ARGN})
    
    if(NOT SIMPLE_ROM_NAME)
        set(SIMPLE_ROM_NAME ${target_name})
    endif()
    
    add_snes_game(${target_name}
        SOURCES ${SIMPLE_SOURCES}
        ROM_NAME ${SIMPLE_ROM_NAME}
        HIROM OFF
        FASTROM OFF
    )
endfunction()
