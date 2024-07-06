#include "front/lexer/lexer.h"
#include "error.h"
#include "front/lexer/token.h"
#include <ctype.h>
#include <fstream>
#include <regex>
#include <string>

const std::unordered_map<std::string, Token::TokenType> Lexer::_keywords = {
    {"fn", Token::FN},     {"var", Token::VARTK}, {"get", Token::GETTK},
    {"put", Token::PUTTK}, {"tag", Token::TAGTK}, {"let", Token::LETTK},
    {"if", Token::IFTK},   {"to", Token::TOTK},
};

bool Lexer::next(Token &token) {
    std::string content;
    char ch = _input.get();
    content.append(1, ch);
    if (isalpha(ch) || ch == '_') {
        ch = _input.get();
        while (isalnum(ch) || ch == '_') {
            content.append(1, ch);
            ch = _input.get();
        }
        _input.unget();
        auto it = _keywords.find(content);
        auto type = it == _keywords.end() ? Token::IDENFR : it->second;
        token = Token(type, content, _line_number);
    } else if (isdigit(ch)) {
        if (ch != '0') {
            ch = _input.get();
            while (isdigit(ch)) {
                content.append(1, ch);
                ch = _input.get();
            }
        } else {
            ch = _input.get();
        }
        if (ch == '.') {
            content.append(1, ch);
            ch = _input.get();
            while (isdigit(ch)) {
                content.append(1, ch);
                ch = _input.get();
            }
        }
        _input.unget();
        token = Token(Token::NUMBER, content, _line_number);
    } else if (ch == '+') {
        token = Token(Token::PLUS, content, _line_number);
    } else if (ch == '-') {
        token = Token(Token::MINU, content, _line_number);
    } else if (ch == '*') {
        token = Token(Token::MULT, content, _line_number);
    } else if (ch == '/') {
        token = Token(Token::DIV, content, _line_number);
    } else if (ch == '%') {
        token = Token(Token::MOD, content, _line_number);
    } else if (ch == '<') {
        ch = _input.get();
        if (ch == '=') {
            content.append(1, ch);
            token = Token(Token::LEQ, content, _line_number);
        } else {
            _input.unget();
            token = Token(Token::LSS, content, _line_number);
        }
    } else if (ch == '>') {
        ch = _input.get();
        if (ch == '=') {
            content.append(1, ch);
            token = Token(Token::GEQ, content, _line_number);
        } else {
            _input.unget();
            token = Token(Token::GRE, content, _line_number);
        }
    } else if (ch == '=') {
        ch = _input.get();
        if (ch == '=') {
            content.append(1, ch);
            token = Token(Token::EQL, content, _line_number);
        } else if (ch == '>') {
            content.append(1, ch);
            token = Token(Token::RARROW, content, _line_number);
        } else {
            _input.unget();
            token = Token(Token::ASSIGN, content, _line_number);
        }
    } else if (ch == '!') {
        ch = _input.get();
        if (ch == '=') {
            content.append(1, ch);
            token = Token(Token::NEQ, content, _line_number);
        } else {
            _input.unget();
            token = Token(Token::ERR, content, _line_number);
            error(_line_number, "invalid character '!'");
        }
    } else if (ch == ';') {
        token = Token(Token::SEMINCN, content, _line_number);
    } else if (ch == ',') {
        token = Token(Token::COMMA, content, _line_number);
    } else if (ch == '(') {
        token = Token(Token::LPARENT, content, _line_number);
    } else if (ch == ')') {
        token = Token(Token::RPARENT, content, _line_number);
    } else if (ch == '#') { // comment
        while (ch != '\n' && ch != EOF) {
            ch = _input.get();
        }
        _input.unget();
        return next(token);
    } else if (ch == '\n') {
        _line_number++;
        return next(token);
    } else if (isblank(ch)) {
        return next(token);
    } else if (ch == EOF) {
        token = Token(Token::EOFTK, content, _line_number);
        return false;
    } else {
        token = Token(Token::ERR, content, _line_number);
        error(_line_number, "invalid character '" + content + "'");
    }
    return true;
}

Lexer::Lexer(std::istream &in) : _input(in) {}
