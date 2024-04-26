add_executable(tests
    main.cpp
)
target_include_directories(tests PRIVATE .)
target_link_libraries(tests ${PROJECT_LIBRARY})

add_test(NAME TestAll COMMAND tests)
