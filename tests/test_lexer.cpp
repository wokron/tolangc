#include "doctest.h"

#include "tolang/lexer.h"
#include "tolang/token.h"
#include <sstream>
#include <vector>

constexpr char INPUT[] = R"(fn nonParam() => 1 - 3.3;
fn paramOne(a) => a * 0.2;
fn paramMore(a,b,_c,d) => a / (b + _c);
get a;
put a;# one line comment
let n = a - 2.000003;
# comment 2
# comment 3
# comment 4
let i = 0;
tag entry;
a = - a;
put a * i;
let i = i + 1;
if i <= n to entry;# maybe ok
)";

static std::vector<Token> EXPECTS = {
    Token(Token::TK_FN, "fn", 1),      Token(Token::TK_IDENT, "nonParam", 1),
    Token(Token::TK_LPARENT, "(", 1),  Token(Token::TK_RPARENT, ")", 1),
    Token(Token::TK_RARROW, "=>", 1),  Token(Token::TK_NUMBER, "1", 1),
    Token(Token::TK_MINU, "-", 1),     Token(Token::TK_NUMBER, "3.3", 1),
    Token(Token::TK_SEMINCN, ";", 1),

    Token(Token::TK_FN, "fn", 2),      Token(Token::TK_IDENT, "paramOne", 2),
    Token(Token::TK_LPARENT, "(", 2),  Token(Token::TK_IDENT, "a", 2),
    Token(Token::TK_RPARENT, ")", 2),  Token(Token::TK_RARROW, "=>", 2),
    Token(Token::TK_IDENT, "a", 2),    Token(Token::TK_MULT, "*", 2),
    Token(Token::TK_NUMBER, "0.2", 2), Token(Token::TK_SEMINCN, ";", 2),

    Token(Token::TK_FN, "fn", 3),      Token(Token::TK_IDENT, "paramMore", 3),
    Token(Token::TK_LPARENT, "(", 3),  Token(Token::TK_IDENT, "a", 3),
    Token(Token::TK_COMMA, ",", 3),    Token(Token::TK_IDENT, "b", 3),
    Token(Token::TK_COMMA, ",", 3),    Token(Token::TK_IDENT, "_c", 3),
    Token(Token::TK_COMMA, ",", 3),    Token(Token::TK_IDENT, "d", 3),
    Token(Token::TK_RPARENT, ")", 3),  Token(Token::TK_RARROW, "=>", 3),
    Token(Token::TK_IDENT, "a", 3),    Token(Token::TK_DIV, "/", 3),
    Token(Token::TK_LPARENT, "(", 3),  Token(Token::TK_IDENT, "b", 3),
    Token(Token::TK_PLUS, "+", 3),     Token(Token::TK_IDENT, "_c", 3),
    Token(Token::TK_RPARENT, ")", 3),  Token(Token::TK_SEMINCN, ";", 3),

    Token(Token::TK_GET, "get", 4),    Token(Token::TK_IDENT, "a", 4),
    Token(Token::TK_SEMINCN, ";", 4),

    Token(Token::TK_PUT, "put", 5),    Token(Token::TK_IDENT, "a", 5),
    Token(Token::TK_SEMINCN, ";", 5),

    Token(Token::TK_LET, "let", 6),    Token(Token::TK_IDENT, "n", 6),
    Token(Token::TK_ASSIGN, "=", 6),   Token(Token::TK_IDENT, "a", 6),
    Token(Token::TK_MINU, "-", 6),     Token(Token::TK_NUMBER, "2.000003", 6),
    Token(Token::TK_SEMINCN, ";", 6),

    Token(Token::TK_LET, "let", 10),   Token(Token::TK_IDENT, "i", 10),
    Token(Token::TK_ASSIGN, "=", 10),  Token(Token::TK_NUMBER, "0", 10),
    Token(Token::TK_SEMINCN, ";", 10),

    Token(Token::TK_TAG, "tag", 11),   Token(Token::TK_IDENT, "entry", 11),
    Token(Token::TK_SEMINCN, ";", 11),

    Token(Token::TK_IDENT, "a", 12),   Token(Token::TK_ASSIGN, "=", 12),
    Token(Token::TK_MINU, "-", 12),    Token(Token::TK_IDENT, "a", 12),
    Token(Token::TK_SEMINCN, ";", 12),

    Token(Token::TK_PUT, "put", 13),   Token(Token::TK_IDENT, "a", 13),
    Token(Token::TK_MULT, "*", 13),    Token(Token::TK_IDENT, "i", 13),
    Token(Token::TK_SEMINCN, ";", 13),

    Token(Token::TK_LET, "let", 14),   Token(Token::TK_IDENT, "i", 14),
    Token(Token::TK_ASSIGN, "=", 14),  Token(Token::TK_IDENT, "i", 14),
    Token(Token::TK_PLUS, "+", 14),    Token(Token::TK_NUMBER, "1", 14),
    Token(Token::TK_SEMINCN, ";", 14),

    Token(Token::TK_IF, "if", 15),     Token(Token::TK_IDENT, "i", 15),
    Token(Token::TK_LE, "<=", 15),     Token(Token::TK_IDENT, "n", 15),
    Token(Token::TK_TO, "to", 15),     Token(Token::TK_IDENT, "entry", 15),
    Token(Token::TK_SEMINCN, ";", 15),
};

TEST_CASE("testing lexer") {
    std::vector<Token> tokens;

    std::istringstream input(INPUT);
    Lexer lexer = Lexer(input);
    Token token;

    lexer.next(token);
    while (token.type != Token::TK_EOF) {
        tokens.push_back(token);
        lexer.next(token);
    }
    CHECK_EQ(tokens.size(), EXPECTS.size());
    for (int i = 0; i < tokens.size(); i++) {
        CHECK_EQ(tokens.at(i).type, EXPECTS.at(i).type);
        CHECK_EQ(tokens.at(i).content, EXPECTS.at(i).content);
        CHECK_EQ(tokens.at(i).lineno, EXPECTS.at(i).lineno);
    }
}
