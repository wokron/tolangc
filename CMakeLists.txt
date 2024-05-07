cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

project(tolangc)

# tolangc library
set(PROJECT_LIBRARY lib${PROJECT_NAME})

# sysyc library
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_library(${PROJECT_LIBRARY} ${SOURCES})
target_include_directories(${PROJECT_LIBRARY} PUBLIC include)
# lib and exe with same name
set_property(TARGET ${PROJECT_LIBRARY} PROPERTY OUTPUT_NAME ${PROJECT_NAME})

# unit tests
enable_testing()
add_subdirectory(tests)

# tolangc compiler
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_LIBRARY})