#pragma once

#include "token.h"
#include <fstream>
#include <regex>

class Lexer {
  public:
    bool has_next();

    Token *look();

    Token *look_forward(int);

    Token *next();

    Token *next_assert(Token::TokenType);

    void run(std::ifstream *input);

  private:
    std::vector<Token *> _tokens;
    int _index = 0;
    bool _in_annotation = false;
    int _line_number = 0;

    void _deal_with_line(std::string line);

    void _to_tokens(std::string line);

    Token *_is_keyword(const std::string &line);

    Token *_is_symbol(const std::string &line);
};