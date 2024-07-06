#pragma once

#include "token.h"
#include <istream>
#include <unordered_map>

class AbstractLexer {
public:
    virtual bool next(Token &) = 0;
};

class Lexer : public AbstractLexer {
public:
    bool next(Token &) override;
    Lexer(std::istream &);

private:
    static const std::unordered_map<std::string, Token::TokenType>
        _keywords_table;

    int _lineno = 1;
    std::istream &_input;
};
