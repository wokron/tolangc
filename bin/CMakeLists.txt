add_executable(${PROJECT_NAME} main.cpp)

if(${PCODE_BACKEND})
    target_link_libraries(${PROJECT_NAME} PRIVATE pcode tolang)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE llvm mips tolang)
endif()
