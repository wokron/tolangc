add_subdirectory(tolang)

if(${PCODE_BACKEND})
    add_subdirectory(pcode)
else()
    add_subdirectory(llvm)
    add_subdirectory(mips)
endif()
