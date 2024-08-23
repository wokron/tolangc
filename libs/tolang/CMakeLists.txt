set(LIBRARY_NAME tolang)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_library(${LIBRARY_NAME} ${SOURCES})
target_include_directories(${LIBRARY_NAME} PUBLIC include)

if(${PCODE_BACKEND})
    target_link_libraries(${LIBRARY_NAME} pcode)
else()
    target_link_libraries(${LIBRARY_NAME} llvm)
endif()
