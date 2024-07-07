#pragma once

#include "token.h"
#include <istream>
#include <unordered_map>

class Lexer {
public:
    void next(Token &token);
    Lexer(std::istream &in) : _input(in) {}

private:
    static const std::unordered_map<std::string, Token::TokenType>
        _keywords_table;

    int _lineno = 1;
    std::istream &_input;
};
