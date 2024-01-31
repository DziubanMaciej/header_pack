set(TEST_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/${TEST_NAME}")

file(READ "${TEST_SOURCE_DIR}/arguments" ARGS)
separate_arguments(ARGS UNIX_COMMAND "${ARGS}")

set(EXPECTED_OUTPUT_PATH "${TEST_SOURCE_DIR}/expected_output.h")
set(ACTUAL_OUTPUT_PATH "${WORK_DIR}/output.h")

# Prepare output directory
file(REMOVE_RECURSE "${WORK_DIR}")
file(MAKE_DIRECTORY "${WORK_DIR}")
if(NOT EXISTS "${WORK_DIR}")
    message(FATAL_ERROR "Could not create working directory ${WORK_DIR}")
endif()
file(COPY "${TEST_SOURCE_DIR}/input" DESTINATION "${WORK_DIR}")

# Run header_pack
execute_process(
    COMMAND ${HEADER_PACK_BINARY} ${ARGS}
    WORKING_DIRECTORY "${WORK_DIR}"
    RESULT_VARIABLE HEADER_PACK_RESULT
)
if (NOT ${HEADER_PACK_RESULT} STREQUAL 0)
    message(FATAL_ERROR "header_pack failed with a result value of ${HEADER_PACK_RESULT}")
endif()

# Try to compile
file(WRITE "${WORK_DIR}/main.cpp" "#include \"output.h\"\n")
execute_process(
    COMMAND ${COMPILER_PATH} ${COMPILER_ARGS} main.cpp
    WORKING_DIRECTORY "${WORK_DIR}"
    RESULT_VARIABLE COMPILATION_RESULT)
if (NOT ${COMPILATION_RESULT} STREQUAL 0)
    message(FATAL_ERROR "compilation of generated header file failed with a result value of ${COMPILATION_RESULT}")
endif()

# Verify output file
if(NOT EXISTS "${ACTUAL_OUTPUT_PATH}")
    message(FATAL_ERROR "Output does not exist")
endif()
file(READ "${EXPECTED_OUTPUT_PATH}" EXPECTED_OUTPUT)
file(READ "${ACTUAL_OUTPUT_PATH}" ACTUAL_OUTPUT)
if (NOT EXPECTED_OUTPUT STREQUAL ACTUAL_OUTPUT)
    message(FATAL_ERROR "Incorrect output")
endif()
