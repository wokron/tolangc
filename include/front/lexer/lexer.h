#pragma once

#include <fstream>
#include <regex>
#include "token.h"

class Lexer {
public:
    std::vector<Token *> tokens;
    int index = 0;

    void append(Token *token) {
        tokens.push_back(token);
    }

    bool has_next() {
        return index < tokens.size();
    }

    Token *look() {
        return tokens[index];
    }

    Token *look_forward(int cnt) {
        return tokens[index + cnt];
    }

    Token *next() {
        return tokens[index++];
    }

    Token *next_til(Token::TokenType type) {
        Token *token = tokens[index];
        if (token->token_type == type) {
            index++;
            return token;
        }
        exit(1);
    }

    void run(std::ifstream *input) {
        std::string line;
        while (getline(*input, line)) {
            _line_number++;
            _deal_with_line(line);
        }
    }

private:
    bool _in_annotation = false;
    int _line_number = 0;

    void _deal_with_line(std::string line) {
        std::string line_without_annotation;
        for (size_t i = 0; i < line.size(); i++) {
            if (_in_annotation) {
                if (line[i] == '*' && i != line.length() - 1 && line[i + 1] == '/') {
                    _in_annotation = false;
                    i++;
                }
            } else {
                if (line[i] == '/' && i != line.size() - 1 && line[i + 1] == '/') {
                    break;
                } else if (line[i] == '/' && i != line.size() - 1 && line[i + 1] == '*') {
                    _in_annotation = true;
                    i++;
                } else {
                    line_without_annotation += line[i];
                }
            }
        }

        _to_tokens(line_without_annotation);
    }

    void _to_tokens(std::string line) {
        size_t word_index = 0;

        std::regex blank("^\\s+");
        std::regex number("^[1-9][0-9]*(\\.[0-9]+)?");
        std::regex ident("^[a-zA-Z_][0-9a-zA-Z_]*");

        std::smatch match;

        while (word_index < line.size()) {
            // match blank
            std::string s = line.substr(word_index);
            if (std::regex_search(s, match, blank)) {
                word_index += match.length();
                continue;
            }

            Token *token;
            // match keyword
            token = _is_keyword(s);
            if (token != nullptr) {
                size_t next_index = word_index + token->content.length();
                if (next_index == line.size() || !isalnum(line[next_index]) && line[next_index] != '_') {
                    word_index = next_index;
                    tokens.push_back(token);
                    continue;
                }
            }

            // match symbol
            token = _is_symbol(s);
            if (token != nullptr) {
                word_index += token->content.length();
                tokens.push_back(token);
                continue;
            }

            // match number
            if (std::regex_search(s, match, number)) {
                tokens.push_back(new Token(Token::NUMBER,
                                           line.substr(word_index, match.length()),
                                           _line_number));
                word_index += match.length();
                continue;
            }

            // match ident
            if (std::regex_search(s, match, ident)) {
                tokens.push_back(new Token(Token::IDENFR,
                                           line.substr(word_index, match.length()),
                                           _line_number));
                word_index += match.length();
                continue;
            }

            tokens.push_back(new Token(Token::ERR, "Err", _line_number));
        }
    }

    Token *_is_keyword(const std::string &line) {
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

    Token *_is_symbol(const std::string &line) {
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
};