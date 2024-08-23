cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

project(tolangc)

option(PCODE_BACKEND "if use pcode as backend" OFF)

if(${PCODE_BACKEND})
    add_definitions(-DTOLANG_BACKEND=2)
endif()

# enable unit tests
enable_testing()

# binary executable
add_subdirectory(bin)

# libraries
add_subdirectory(libs)

# unit tests
add_subdirectory(tests)
