#include "utils.h"
#include <cstdio>
#include <iostream>
#include <cstdarg>


bool _die(const char* message, const char* file, int line, ...)
{
    va_list args;
    va_start(args, line);
    fprintf(stderr, "Error in file %s on line %d: ", file, line);
    vfprintf(stderr, message, args);
    exit(1);
}
