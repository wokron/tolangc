aux_source_directory(. TEST_FILES)
add_executable(tests
    ${TEST_FILES}
)
target_include_directories(tests PRIVATE .)

if(${PCODE_BACKEND})
    target_link_libraries(tests pcode tolang)
else()
    target_link_libraries(tests llvm mips tolang)
endif()

add_test(NAME TestAll COMMAND tests)