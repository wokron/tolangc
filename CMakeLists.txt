cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

project(tolangc)

set(PROJECT_LIBRARY lib${PROJECT_NAME})

# sysyc library
aux_source_directory(src SOURCES)
add_library(${PROJECT_LIBRARY} ${SOURCES})
target_include_directories(${PROJECT_LIBRARY} PUBLIC include)

# unit tests
enable_testing()
add_subdirectory(tests)

# sysyc compiler
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_LIBRARY})