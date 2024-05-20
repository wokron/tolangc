#include "error.h"

#include <iostream>

void error(int lineno, const std::string &msg) {
    std::cerr << lineno << ": " << msg << std::endl;
}
