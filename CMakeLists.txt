cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

project(tolangc)

# enable unit tests
enable_testing()

# binary executable
add_subdirectory(bin)

# libraries
add_subdirectory(libs)

# unit tests
add_subdirectory(tests)
