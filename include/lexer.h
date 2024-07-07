#pragma once

#include "token.h"
#include <istream>
#include <unordered_map>

/**
 * @brief `Lexer` is a class that reads input from a stream and generates
 * tokens.
 */
class Lexer {
public:
    /**
     * @brief Get the next token from the input stream.
     * @param token The token to be filled.
     * @note Get EOF token if the end of the input stream is reached. This
     * function can still be called after EOF is reached.
     */
    void next(Token &token);

    /**
     * @brief Construct a new Lexer object.
     * @param in The input stream.
     */
    Lexer(std::istream &in) : _input(in) {}

private:
    // A table that maps keywords to their token types. Since keywords are
    // similar to identifiers, we can first identify a identifier and then check
    // if it is a keyword.
    static const std::unordered_map<std::string, Token::TokenType>
        _keywords_table;

    int _lineno = 1;

    // The input stream. we can use `get` and `unget` to read and unread
    // characters, which is useful for implementing the lexer.
    std::istream &_input;
};
