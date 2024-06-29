#include "error.h"

#include <iostream>

static bool error_flag = false;

void error(int lineno, const std::string &msg) {
    error_flag = true;
    std::cerr << lineno << ": " << msg << std::endl;
}

bool has_error() { return error_flag; }
