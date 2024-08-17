#pragma once

bool _die(const char *fmt, const char *file, int line, ...);
#define TOLANG_DIE(message, ...)                                               \
    _die(message, __FILE__, __LINE__, ##__VA_ARGS__)
#define TOLANG_DIE_IF_NOT(condition, message, ...)                             \
    ((!!(condition)) || (TOLANG_DIE(message, ##__VA_ARGS__)))

#ifdef _DEBUG
#include <cassert>
#define TOLANG_ASSERT(expression) assert(expression)
#else
#define TOLANG_ASSERT(expression)
#endif
