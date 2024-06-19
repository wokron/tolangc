#pragma once

#include "token.h"
#include <fstream>
#include <regex>

class Lexer {
public:
    bool next(Token &);
    Lexer(std::istream &);

private:
    bool _in_annotation = false;
    int _line_number = 0;
    size_t _word_index = 0;
    std::istream &_input;
    std::string _cur_line;

    void _deal_with_line();

    Token *_next_token(std::string line);

    Token *_is_keyword(const std::string &line);

    Token *_is_symbol(const std::string &line);

    Token *_is_number(const std::string &line);

    Token *_is_ident(const std::string &line);
};