#pragma once

#include <string>

struct Token {
    enum TokenType {
        TK_IDENT,
        TK_NUMBER,
        TK_FN,
        TK_LPARENT,
        TK_RPARENT,
        TK_RARROW,
        TK_SEMINCN,
        TK_VAR,
        TK_GET,
        TK_PUT,
        TK_TAG,
        TK_LET,
        TK_ASSIGN,
        TK_IF,
        TK_TO,
        TK_PLUS,
        TK_MINU,
        TK_MULT,
        TK_DIV,
        TK_MOD,
        TK_LT,
        TK_GT,
        TK_LE,
        TK_GE,
        TK_EQ,
        TK_NE,
        TK_COMMA,
        TK_ERR,
        TK_EOF,
    } type;
    std::string content;
    int lineno;

    Token(TokenType type, std::string content, int lineno)
        : type(type), content(content), lineno(lineno) {}
};
