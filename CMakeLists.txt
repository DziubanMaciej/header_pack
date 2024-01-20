project(header_pack)
cmake_minimum_required(VERSION 3.12)
add_executable(header_pack header_pack.cpp)
target_compile_features(header_pack PRIVATE cxx_std_17)
target_compile_options(header_pack PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

function (header_pack_generate TARGET INPUT_FILE_NAME OUTPUT_FILE_NAME MODE)
    # Parse optional arguments
    set(OPTION_ARGS)
    set(VALUE_ARGS VARIABLE VALUES_PER_LINE)
    set(MULTI_VALUE_ARGS)
    cmake_parse_arguments(ARG "${OPTION_ARGS}" "${VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

    # Preprocess arguments
    if (${MODE} STREQUAL TEXT)
        set(MODE "-t")
    elseif(${MODE} STREQUAL BINARY)
        set(MODE "-b")
    else()
        message(FATAL_ERROR "Invalid mode specified. Must be TEXT or BINARY")
    endif()
    if ("${ARG_VARIABLE}d" STREQUAL "d")
        set(VARIABLE "variable")
    else()
        set(VARIABLE "${ARG_VARIABLE}")
    endif()
    if ("${ARG_VALUES_PER_LINE}d" STREQUAL "d")
        set(VALUES_PER_LINE "16")
    else()
        set(VALUES_PER_LINE "${ARG_VALUES_PER_LINE}")
    endif()

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
        COMMAND $<TARGET_FILE:header_pack> ARGS "${INPUT_PATH}" "${OUTPUT_PATH}" ${MODE} -n ${VARIABLE} -c ${VALUES_PER_LINE}
        MAIN_DEPENDENCY "${INPUT_PATH}"
        DEPENDS "${INPUT_PATH}" header_pack
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "Generating ${OUTPUT_PATH}"
    )
    target_sources(${TARGET} PRIVATE "${OUTPUT_PATH}")

    # Since this is a header file, we setup an include directory.
    target_include_directories(${TARGET} PRIVATE "${GENERATE_DIR_ROOT}")
endfunction()
