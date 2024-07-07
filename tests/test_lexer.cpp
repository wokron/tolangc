#include "doctest.h"

#include "front/lexer/lexer.h"
#include "front/lexer/token.h"
#include <vector>
#include <sstream>

std::string src;
std::vector<Token> tokens;

static void AddSrc();
static void AddResult();

TEST_CASE("testing lexer") {
    AddSrc();
    AddResult();

    std::istringstream input(src);
    Lexer lexer = Lexer(input);
    Token token = Token(Token::TK_ERR, "", -1);

    int i = 0;
    lexer.next(token);
    while (token.type != Token::TK_EOF) {
        CHECK(i <= tokens.size());
        Token result = tokens.at(i);
        CHECK_EQ(token.type, result.type);
        CHECK_EQ(token.content, result.content);
        CHECK_EQ(token.lineno, result.lineno);
        i++;
        lexer.next(token);
    }
    CHECK_EQ(i, tokens.size());
}

static void AddSrc() {
    src = "fn nonParam() => 1 - 3.3;\n"
          "fn paramOne(a) => a * 0.2;\n"
          "fn paramMore(a,b,_c,d) => a / (b + _c) % d;\n"
          "get a;\n"
          "put a;# one line comment\n"
          "let n = a - 2.000003;\n"
          "# comment 2\n"
          "# comment 3\n"
          "# comment 4\n"
          "let i = 0;\n"
          "tag entry;\n"
          "a = - a;\n"
          "put a * i;\n"
          "let i = i + 1;\n"
          "if i <= n to entry;# maybe ok\n";
}

static void AddResult() {
    tokens.push_back(Token(Token::TK_FN, "fn", 1));
    tokens.push_back(Token(Token::TK_IDENT, "nonParam", 1));
    tokens.push_back(Token(Token::TK_LPARENT, "(", 1));
    tokens.push_back(Token(Token::TK_RPARENT, ")", 1));
    tokens.push_back(Token(Token::TK_RARROW, "=>", 1));
    tokens.push_back(Token(Token::TK_NUMBER, "1", 1));
    tokens.push_back(Token(Token::TK_MINU, "-", 1));
    tokens.push_back(Token(Token::TK_NUMBER, "3.3", 1));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 1));

    tokens.push_back(Token(Token::TK_FN, "fn", 2));
    tokens.push_back(Token(Token::TK_IDENT, "paramOne", 2));
    tokens.push_back(Token(Token::TK_LPARENT, "(", 2));
    tokens.push_back(Token(Token::TK_IDENT, "a", 2));
    tokens.push_back(Token(Token::TK_RPARENT, ")", 2));
    tokens.push_back(Token(Token::TK_RARROW, "=>", 2));
    tokens.push_back(Token(Token::TK_IDENT, "a", 2));
    tokens.push_back(Token(Token::TK_MULT, "*", 2));
    tokens.push_back(Token(Token::TK_NUMBER, "0.2", 2));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 2));

    tokens.push_back(Token(Token::TK_FN, "fn", 3));
    tokens.push_back(Token(Token::TK_IDENT, "paramMore", 3));
    tokens.push_back(Token(Token::TK_LPARENT, "(", 3));
    tokens.push_back(Token(Token::TK_IDENT, "a", 3));
    tokens.push_back(Token(Token::TK_COMMA, ",", 3));
    tokens.push_back(Token(Token::TK_IDENT, "b", 3));
    tokens.push_back(Token(Token::TK_COMMA, ",", 3));
    tokens.push_back(Token(Token::TK_IDENT, "_c", 3));
    tokens.push_back(Token(Token::TK_COMMA, ",", 3));
    tokens.push_back(Token(Token::TK_IDENT, "d", 3));
    tokens.push_back(Token(Token::TK_RPARENT, ")", 3));
    tokens.push_back(Token(Token::TK_RARROW, "=>", 3));
    tokens.push_back(Token(Token::TK_IDENT, "a", 3));
    tokens.push_back(Token(Token::TK_DIV, "/", 3));
    tokens.push_back(Token(Token::TK_LPARENT, "(", 3));
    tokens.push_back(Token(Token::TK_IDENT, "b", 3));
    tokens.push_back(Token(Token::TK_PLUS, "+", 3));
    tokens.push_back(Token(Token::TK_IDENT, "_c", 3));
    tokens.push_back(Token(Token::TK_RPARENT, ")", 3));
    tokens.push_back(Token(Token::TK_MOD, "%", 3));
    tokens.push_back(Token(Token::TK_IDENT, "d", 3));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 3));

    tokens.push_back(Token(Token::TK_GET, "get", 4));
    tokens.push_back(Token(Token::TK_IDENT, "a", 4));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 4));

    tokens.push_back(Token(Token::TK_PUT, "put", 5));
    tokens.push_back(Token(Token::TK_IDENT, "a", 5));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 5));

    tokens.push_back(Token(Token::TK_LET, "let", 6));
    tokens.push_back(Token(Token::TK_IDENT, "n", 6));
    tokens.push_back(Token(Token::TK_ASSIGN, "=", 6));
    tokens.push_back(Token(Token::TK_IDENT, "a", 6));
    tokens.push_back(Token(Token::TK_MINU, "-", 6));
    tokens.push_back(Token(Token::TK_NUMBER, "2.000003", 6));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 6));

    tokens.push_back(Token(Token::TK_LET, "let", 10));
    tokens.push_back(Token(Token::TK_IDENT, "i", 10));
    tokens.push_back(Token(Token::TK_ASSIGN, "=", 10));
    tokens.push_back(Token(Token::TK_NUMBER, "0", 10));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 10));

    tokens.push_back(Token(Token::TK_TAG, "tag", 11));
    tokens.push_back(Token(Token::TK_IDENT, "entry", 11));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 11));

    tokens.push_back(Token(Token::TK_IDENT, "a", 12));
    tokens.push_back(Token(Token::TK_ASSIGN, "=", 12));
    tokens.push_back(Token(Token::TK_MINU, "-", 12));
    tokens.push_back(Token(Token::TK_IDENT, "a", 12));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 12));

    tokens.push_back(Token(Token::TK_PUT, "put", 13));
    tokens.push_back(Token(Token::TK_IDENT, "a", 13));
    tokens.push_back(Token(Token::TK_MULT, "*", 13));
    tokens.push_back(Token(Token::TK_IDENT, "i", 13));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 13));

    tokens.push_back(Token(Token::TK_LET, "let", 14));
    tokens.push_back(Token(Token::TK_IDENT, "i", 14));
    tokens.push_back(Token(Token::TK_ASSIGN, "=", 14));
    tokens.push_back(Token(Token::TK_IDENT, "i", 14));
    tokens.push_back(Token(Token::TK_PLUS, "+", 14));
    tokens.push_back(Token(Token::TK_NUMBER, "1", 14));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 14));

    tokens.push_back(Token(Token::TK_IF, "if", 15));
    tokens.push_back(Token(Token::TK_IDENT, "i", 15));
    tokens.push_back(Token(Token::TK_LE, "<=", 15));
    tokens.push_back(Token(Token::TK_IDENT, "n", 15));
    tokens.push_back(Token(Token::TK_TO, "to", 15));
    tokens.push_back(Token(Token::TK_IDENT, "entry", 15));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 15));
}