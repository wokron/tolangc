#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>

// X macro magic
#define TOKEN_TYPE                                                             \
    X(TK_IDENT, "identifier")                                                  \
    X(TK_NUMBER, "number")                                                     \
    X(TK_FN, "fn")                                                             \
    X(TK_LPARENT, "(")                                                         \
    X(TK_RPARENT, ")")                                                         \
    X(TK_RARROW, "=>")                                                         \
    X(TK_SEMINCN, ";")                                                         \
    X(TK_VAR, "var")                                                           \
    X(TK_GET, "get")                                                           \
    X(TK_PUT, "put")                                                           \
    X(TK_TAG, "tag")                                                           \
    X(TK_LET, "let")                                                           \
    X(TK_ASSIGN, "=")                                                          \
    X(TK_IF, "if")                                                             \
    X(TK_TO, "to")                                                             \
    X(TK_PLUS, "+")                                                            \
    X(TK_MINU, "-")                                                            \
    X(TK_MULT, "*")                                                            \
    X(TK_DIV, "/")                                                             \
    X(TK_LT, "<")                                                              \
    X(TK_GT, ">")                                                              \
    X(TK_LE, "<=")                                                             \
    X(TK_GE, ">=")                                                             \
    X(TK_EQ, "==")                                                             \
    X(TK_NE, "!=")                                                             \
    X(TK_COMMA, ",")                                                           \
    X(TK_ERR, "err")                                                           \
    X(TK_EOF, "eof")

/**
 * @brief `Token` is a class that represents a token generated by the lexer. And
 * it is consumed by the parser.
 */
struct Token {
    enum TokenType {
#define X(a, b) a,
        TOKEN_TYPE
#undef X
    } type;
    std::string content;
    int lineno;

    Token() = default;

    Token(TokenType type, std::string content, int lineno)
        : type(type), content(content), lineno(lineno) {}
};

inline std::string token_type_to_string(Token::TokenType type) {
    static const std::unordered_map<Token::TokenType, std::string>
        token_type_str = {
#define X(a, b) {Token::a, b},
            TOKEN_TYPE
#undef X
        };
    return token_type_str.at(type);
}