#pragma once

#include <utility>

#include "iostream"
#include "string"
#include "vector"

class Token {
  public:
    enum TokenType {
        IDENFR,
        NUMBER,
        FN,
        LPARENT,
        RPARENT,
        RARROW,
        SEMINCN,
        VARTK,
        GETTK,
        PUTTK,
        TAGTK,
        LETTK,
        ASSIGN,
        IFTK,
        TOTK,
        PLUS,
        MINU,
        MULT,
        DIV,
        MOD,
        LSS,
        GRE,
        LEQ,
        GEQ,
        EQL,
        NEQ,
        ERR
    };

    TokenType token_type;
    std::string content;
    int line;
    Token(TokenType type, std::string cont, int line)
        : token_type(type), content(cont), line(line) {}
};
