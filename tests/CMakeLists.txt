aux_source_directory(. TEST_FILES)
add_executable(tests
    ${TEST_FILES}
        test_symbol_table.cpp)
target_include_directories(tests PRIVATE .)
target_link_libraries(tests ${PROJECT_LIBRARY})

add_test(NAME TestAll COMMAND tests)
