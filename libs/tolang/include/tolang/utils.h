#pragma once

/**
 * @brief A helper class to create a visitor pattern for a variant.
 * @tparam Ts The types of the variant.
 */
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;


// define the backend to use
#define LLVM 1
#define PCODE 2

#ifndef TOLANG_BACKEND
#define TOLANG_BACKEND LLVM
#endif
