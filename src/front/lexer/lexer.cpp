#include "front/lexer/lexer.h"
#include "front/lexer/token.h"
#include <fstream>
#include <regex>

bool Lexer::next(Token &token) {
    while (_cur_line.size() == _word_index) {
        if (!getline(_input, _cur_line)) {
            return false;
        }
        _line_number++;
        _word_index = 0;
        _deal_with_line();
    }

    Token *next_token = _next_token(_cur_line);
    if (next_token == nullptr) {
        return next(token);
    } else {
        token = *next_token;
        return true;
    }
}

Lexer::Lexer(std::istream &in) : _input(in) {}

void Lexer::_deal_with_line() {
    std::string line_without_annotation;
    for (size_t i = 0; i < _cur_line.size(); i++) {
        if (_in_annotation) {
            if (_cur_line[i] == '*' && i != _cur_line.length() - 1 &&
                _cur_line[i + 1] == '/') {
                _in_annotation = false;
                i++;
            }
        } else {
            if (_cur_line[i] == '/' && i != _cur_line.size() - 1 &&
                _cur_line[i + 1] == '/') {
                break;
            } else if (_cur_line[i] == '/' && i != _cur_line.size() - 1 &&
                       _cur_line[i + 1] == '*') {
                _in_annotation = true;
                i++;
            } else {
                line_without_annotation += _cur_line[i];
            }
        }
    }
    _cur_line = line_without_annotation;
}

Token *Lexer::_next_token(std::string line) {

    std::regex blank("^\\s+");
    std::regex number("^[1-9][0-9]*(\\.[0-9]+)?");
    std::regex ident("^[a-zA-Z_][0-9a-zA-Z_]*");

    std::smatch match;

    while (_word_index < line.size()) {
        // match blank
        std::string s = line.substr(_word_index);
        if (std::regex_search(s, match, blank)) {
            _word_index += match.length();
            continue;
        }

        Token *token;
        // match keyword
        token = _is_keyword(s);
        if (token != nullptr) {
            size_t next_index = _word_index + token->content.length();
            if (next_index == line.size() ||
                !isalnum(line[next_index]) && line[next_index] != '_') {
                _word_index = next_index;
                return token;
            }
        }

        // match symbol
        token = _is_symbol(s);
        if (token != nullptr) {
            _word_index += token->content.length();
            return token;
        }

        // match number
        if (std::regex_search(s, match, number)) {
            _word_index += match.length();
            return new Token(
                Token::NUMBER,
                line.substr(_word_index - match.length(), match.length()),
                _line_number);
        }

        // match ident
        if (std::regex_search(s, match, ident)) {
            _word_index += match.length();
            return new Token(
                Token::IDENFR,
                line.substr(_word_index - match.length(), match.length()),
                _line_number);
        }

        return new Token(Token::ERR, "Err", _line_number);
    }
    return nullptr;
}

Token *Lexer::_is_keyword(const std::string &line) {
    if (line.rfind("fn") == 0) {
        return new Token(Token::FN, "fn", _line_number);
    } else if (line.rfind("var") == 0) {
        return new Token(Token::VARTK, "var", _line_number);
    } else if (line.rfind("get") == 0) {
        return new Token(Token::GETTK, "get", _line_number);
    } else if (line.rfind("put") == 0) {
        return new Token(Token::PUTTK, "put", _line_number);
    } else if (line.rfind("tag") == 0) {
        return new Token(Token::TAGTK, "tag", _line_number);
    } else if (line.rfind("let") == 0) {
        return new Token(Token::LETTK, "let", _line_number);
    } else if (line.rfind("if") == 0) {
        return new Token(Token::IFTK, "if", _line_number);
    } else if (line.rfind("to") == 0) {
        return new Token(Token::TOTK, "to", _line_number);
    } else {
        return nullptr;
    }
}

Token *Lexer::_is_symbol(const std::string &line) {
    if (line.rfind('+') == 0) {
        return new Token(Token::PLUS, "+", _line_number);
    } else if (line.rfind('-') == 0) {
        return new Token(Token::MINU, "-", _line_number);
    } else if (line.rfind('*') == 0) {
        return new Token(Token::MULT, "*", _line_number);
    } else if (line.rfind('/') == 0) {
        return new Token(Token::DIV, "/", _line_number);
    } else if (line.rfind('%') == 0) {
        return new Token(Token::MOD, "%", _line_number);
    } else if (line.rfind("<=") == 0) {
        return new Token(Token::LEQ, "<=", _line_number);
    } else if (line.rfind(">=") == 0) {
        return new Token(Token::GEQ, ">=", _line_number);
    } else if (line.rfind('<') == 0) {
        return new Token(Token::LSS, "<", _line_number);
    } else if (line.rfind('>') == 0) {
        return new Token(Token::GRE, ">", _line_number);
    } else if (line.rfind("==") == 0) {
        return new Token(Token::EQL, "==", _line_number);
    } else if (line.rfind("=>") == 0) {
        return new Token(Token::RARROW, "=>", _line_number);
    } else if (line.rfind("!=") == 0) {
        return new Token(Token::NEQ, "!=", _line_number);
    } else if (line.rfind('=') == 0) {
        return new Token(Token::ASSIGN, "=", _line_number);
    } else if (line.rfind('(') == 0) {
        return new Token(Token::LPARENT, "(", _line_number);
    } else if (line.rfind(')') == 0) {
        return new Token(Token::RPARENT, ")", _line_number);
    } else if (line.rfind(';') == 0) {
        return new Token(Token::SEMINCN, ";", _line_number);
    } else {
        return nullptr;
    }
}
