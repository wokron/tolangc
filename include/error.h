#pragma once

#include <string>

// TODO: error message should be ordered by line number when printing, so we
// need to store the line number when calling error function

void error(int lineno, const std::string &msg);

bool has_error();
