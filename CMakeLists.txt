project(header_pack)
cmake_minimum_required(VERSION 3.10)
add_executable(header_pack header_pack.cpp)
target_compile_features(header_pack PRIVATE cxx_std_17)

function (header_pack_generate TARGET INPUT_FILE_NAME OUTPUT_FILE_NAME MODE_ARG VARIABLE_NAME HEX_VALUES_PER_LINE)
    set(INPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${INPUT_FILE_NAME}")

    # Double appearance of "generated" in path is intentional. It lets C++ code do includes like
    # #include "generated/something.h", making it obvious that used code is generated.
    set(GENERATE_DIR_ROOT "${CMAKE_BINARY_DIR}/generated")
    set(OUTPUT_DIR "${GENERATE_DIR_ROOT}/generated")
    set(OUTPUT_PATH "${OUTPUT_DIR}/${OUTPUT_FILE_NAME}")

    # The custom command specifies output file it generates. If we add this file as one of the
    # target's source files, then this target will start depending on the custom command.
    add_custom_command(
        OUTPUT "${OUTPUT_PATH}"
        COMMAND ${CMAKE_COMMAND} ARGS -E make_directory "${OUTPUT_DIR}"
        COMMAND $<TARGET_FILE:header_pack> ARGS "${INPUT_PATH}" "${OUTPUT_PATH}" ${MODE_ARG} -n ${VARIABLE_NAME} -c ${HEX_VALUES_PER_LINE}
        MAIN_DEPENDENCY "${INPUT_PATH}"
        DEPENDS "${INPUT_PATH}" header_pack
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "Generating ${OUTPUT_PATH}"
    )
    target_sources(${TARGET} PRIVATE "${OUTPUT_PATH}")

    # Since this is a header file, we setup an include directory.
    target_include_directories(${TARGET} PRIVATE "${GENERATE_DIR_ROOT}")
endfunction()

function (header_pack_generate_from_text TARGET INPUT_FILE_NAME OUTPUT_FILE_NAME VARIABLE_NAME)
    set(MODE_ARG "-t")
    set(HEX_VALUES_PER_LINE 0)
    header_pack_generate("${TARGET}" "${INPUT_FILE_NAME}" "${OUTPUT_FILE_NAME}" ${MODE_ARG} ${VARIABLE_NAME} ${HEX_VALUES_PER_LINE})
endfunction()

function (header_pack_generate_from_binary TARGET INPUT_FILE_NAME OUTPUT_FILE_NAME VARIABLE_NAME HEX_VALUES_PER_LINE)
    set(MODE_ARG "-b")
    if ("${HEX_VALUES_PER_LINE}d" STREQUAL "d")
        set(HEX_VALUES_PER_LINE 16)
    endif()
    header_pack_generate("${TARGET}" "${INPUT_FILE_NAME}" "${OUTPUT_FILE_NAME}" ${MODE_ARG} ${VARIABLE_NAME} ${HEX_VALUES_PER_LINE})
endfunction()
