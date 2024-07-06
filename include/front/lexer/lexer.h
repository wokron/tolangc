#pragma once

#include "token.h"
#include <fstream>
#include <regex>
#include <unordered_map>

class Lexer {
public:
    bool next(Token &);
    Lexer(std::istream &);

private:
    static const std::unordered_map<std::string, Token::TokenType> _keywords;

    int _line_number = 1;
    std::istream &_input;
};
