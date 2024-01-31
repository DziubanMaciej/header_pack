# Prepare command to test, wheter generated file can be compiled
set(COMPILER_PATH "${CMAKE_CXX_COMPILER}")
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(COMPILER_ARGS "-c")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(COMPILER_ARGS "-c")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(COMPILER_ARGS "/c")
else ()
    message(FATAL_ERROR "Unsupported compiler for testing: ${CMAKE_CXX_COMPILER_ID}")
endif ()

# Helper function to define tests
function (define_header_pack_test TEST_NAME)
    set(SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/run_test.cmake")
    set(HEADER_PACK_BINARY "$<TARGET_FILE:header_pack>")
    set(WORK_DIR ${CMAKE_BINARY_DIR}/test_scratch/${TEST_NAME})
    add_test(NAME ${TEST_NAME}_generate
            COMMAND ${CMAKE_COMMAND} -DWORK_DIR=${WORK_DIR} -DHEADER_PACK_BINARY=${HEADER_PACK_BINARY} -DTEST_NAME=${TEST_NAME} -DCOMPILER_PATH=${COMPILER_PATH} -DCOMPILER_ARGS=${COMPILER_ARGS} -P ${SCRIPT}
    )
endfunction()

define_header_pack_test(test-text-cpp)
define_header_pack_test(test-binary)
