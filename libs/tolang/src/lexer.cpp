#include "tolang/lexer.h"
#include "tolang/error.h"
#include "tolang/token.h"
#include <ctype.h>
#include <fstream>
#include <string>

const std::unordered_map<std::string, Token::TokenType> Lexer::_keywords_table =
    {
        {"fn", Token::TK_FN},   {"var", Token::TK_VAR}, {"get", Token::TK_GET},
        {"put", Token::TK_PUT}, {"tag", Token::TK_TAG}, {"let", Token::TK_LET},
        {"if", Token::TK_IF},   {"to", Token::TK_TO},
};

void Lexer::next(Token &token) {
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
        auto it = _keywords_table.find(content);
        auto type = it == _keywords_table.end() ? Token::TK_IDENT : it->second;
        token = Token(type, content, _lineno);
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
        token = Token(Token::TK_NUMBER, content, _lineno);
    } else if (ch == '+') {
        token = Token(Token::TK_PLUS, content, _lineno);
    } else if (ch == '-') {
        token = Token(Token::TK_MINU, content, _lineno);
    } else if (ch == '*') {
        token = Token(Token::TK_MULT, content, _lineno);
    } else if (ch == '/') {
        token = Token(Token::TK_DIV, content, _lineno);
    } else if (ch == '<') {
        ch = _input.get();
        if (ch == '=') {
            content.append(1, ch);
            token = Token(Token::TK_LE, content, _lineno);
        } else {
            _input.unget();
            token = Token(Token::TK_LT, content, _lineno);
        }
    } else if (ch == '>') {
        ch = _input.get();
        if (ch == '=') {
            content.append(1, ch);
            token = Token(Token::TK_GE, content, _lineno);
        } else {
            _input.unget();
            token = Token(Token::TK_GT, content, _lineno);
        }
    } else if (ch == '=') {
        ch = _input.get();
        if (ch == '=') {
            content.append(1, ch);
            token = Token(Token::TK_EQ, content, _lineno);
        } else if (ch == '>') {
            content.append(1, ch);
            token = Token(Token::TK_RARROW, content, _lineno);
        } else {
            _input.unget();
            token = Token(Token::TK_ASSIGN, content, _lineno);
        }
    } else if (ch == '!') {
        ch = _input.get();
        if (ch == '=') {
            content.append(1, ch);
            token = Token(Token::TK_NE, content, _lineno);
        } else {
            _input.unget();
            token = Token(Token::TK_ERR, content, _lineno);
            ErrorReporter::error(_lineno, "invalid character '!'");
        }
    } else if (ch == ';') {
        token = Token(Token::TK_SEMINCN, content, _lineno);
    } else if (ch == ',') {
        token = Token(Token::TK_COMMA, content, _lineno);
    } else if (ch == '(') {
        token = Token(Token::TK_LPARENT, content, _lineno);
    } else if (ch == ')') {
        token = Token(Token::TK_RPARENT, content, _lineno);
    } else if (ch == '#') { // comment
        while (ch != '\n' && ch != EOF) {
            ch = _input.get();
        }
        _input.unget();
        return next(token);
    } else if (ch == '\n') {
        _lineno++;
        return next(token);
    } else if (isblank(ch)) {
        return next(token);
    } else if (ch == EOF) {
        token = Token(Token::TK_EOF, content, _lineno);
    } else {
        token = Token(Token::TK_ERR, content, _lineno);
        ErrorReporter::error(_lineno, "invalid character '" + content + "'");
    }
}
