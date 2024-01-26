function (define_header_pack_test TEST_NAME)
    set(SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/run_test.cmake")
    set(HEADER_PACK_BINARY "$<TARGET_FILE:header_pack>")
    set(WORK_DIR ${CMAKE_BINARY_DIR}/test_scratch/${TEST_NAME})
    add_test(NAME ${TEST_NAME}_generate
            COMMAND ${CMAKE_COMMAND} -DWORK_DIR=${WORK_DIR} -DHEADER_PACK_BINARY=${HEADER_PACK_BINARY} -DTEST_NAME=${TEST_NAME} -P ${SCRIPT}
    )
endfunction()

define_header_pack_test(test-text-cpp)
define_header_pack_test(test-binary)
