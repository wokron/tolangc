#include "doctest.h"
#include "front/parser/parser.h"
#include <iostream>
#include <sstream>
#include <vector>

class MockLexer : public AbstractLexer {
public:
    MockLexer(std::vector<Token> &tokens) : _tokens(tokens), _pos(0) {}

    bool next(Token &token) override {
        if (_pos >= _tokens.size()) {
            token = Token(Token::TK_EOF, "", 0);
            return false;
        }
        token = _tokens[_pos++];
        return true;
    }

private:
    std::vector<Token> &_tokens;
    int _pos;
};

/*
fn f(a,b) => a + b;
fn g() => 3;
var x;
var y;
var z;
var w;
put g();
get x;
get y;
get z;
put f(x,1);
let x = y;
tag L1;
let x = (x * 2 + y * ( z - 1 * 2 ))+y;
if x < 2 to L1;
to L2;
get w;
tag L2;
put a;
*/

static void create_tokens(std::vector<Token> &tokens);
static void create_ans(std::string &ans);

TEST_CASE("testing ast") {
    // 词法分析
    std::vector<Token> tokens;
    create_tokens(tokens);

    MockLexer lexer(tokens);

    // 语法分析
    Parser parser(lexer);
    CompUnit compUnit = *parser.parse();
    std::ostringstream out;
    compUnit.print(out);
    std::string ans;
    create_ans(ans);

    CHECK_EQ(out.str(), ans);
}

static void create_tokens(std::vector<Token> &tokens) {

    tokens.push_back(Token(Token::TK_FN, "fn", 0));
    tokens.push_back(Token(Token::TK_IDENT, "f", 0));
    tokens.push_back(Token(Token::TK_LPARENT, "(", 0));
    tokens.push_back(Token(Token::TK_IDENT, "a", 0));
    tokens.push_back(Token(Token::TK_COMMA, ",", 0));
    tokens.push_back(Token(Token::TK_IDENT, "b", 0));
    tokens.push_back(Token(Token::TK_RPARENT, ")", 0));
    tokens.push_back(Token(Token::TK_RARROW, "=>", 0));
    tokens.push_back(Token(Token::TK_IDENT, "a", 0));
    tokens.push_back(Token(Token::TK_PLUS, "+", 0));
    tokens.push_back(Token(Token::TK_IDENT, "b", 0));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 0));

    tokens.push_back(Token(Token::TK_FN, "fn", 0));
    tokens.push_back(Token(Token::TK_IDENT, "g", 0));
    tokens.push_back(Token(Token::TK_LPARENT, "(", 0));
    tokens.push_back(Token(Token::TK_RPARENT, ")", 0));
    tokens.push_back(Token(Token::TK_RARROW, "=>", 0));
    tokens.push_back(Token(Token::TK_NUMBER, "3", 0));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 0));

    tokens.push_back(Token(Token::TK_VAR, "var", 0));
    tokens.push_back(Token(Token::TK_IDENT, "x", 0));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 0));

    tokens.push_back(Token(Token::TK_VAR, "var", 0));
    tokens.push_back(Token(Token::TK_IDENT, "y", 0));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 0));

    tokens.push_back(Token(Token::TK_VAR, "var", 0));
    tokens.push_back(Token(Token::TK_IDENT, "z", 0));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 0));

    tokens.push_back(Token(Token::TK_VAR, "var", 0));
    tokens.push_back(Token(Token::TK_IDENT, "w", 0));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 0));

    tokens.push_back(Token(Token::TK_PUT, "put", 0));
    tokens.push_back(Token(Token::TK_IDENT, "g", 0));
    tokens.push_back(Token(Token::TK_LPARENT, "(", 0));
    tokens.push_back(Token(Token::TK_RPARENT, ")", 0));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 0));

    tokens.push_back(Token(Token::TK_GET, "get", 0));
    tokens.push_back(Token(Token::TK_IDENT, "x", 0));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 0));

    tokens.push_back(Token(Token::TK_GET, "get", 0));
    tokens.push_back(Token(Token::TK_IDENT, "y", 0));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 0));

    tokens.push_back(Token(Token::TK_GET, "get", 0));
    tokens.push_back(Token(Token::TK_IDENT, "z", 0));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 0));

    tokens.push_back(Token(Token::TK_PUT, "put", 0));
    tokens.push_back(Token(Token::TK_IDENT, "f", 0));
    tokens.push_back(Token(Token::TK_LPARENT, "(", 0));
    tokens.push_back(Token(Token::TK_IDENT, "x", 0));
    tokens.push_back(Token(Token::TK_COMMA, ",", 0));
    tokens.push_back(Token(Token::TK_NUMBER, "1", 0));
    tokens.push_back(Token(Token::TK_RPARENT, ")", 0));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 0));

    tokens.push_back(Token(Token::TK_LET, "let", 0));
    tokens.push_back(Token(Token::TK_IDENT, "x", 0));
    tokens.push_back(Token(Token::TK_ASSIGN, "=", 0));
    tokens.push_back(Token(Token::TK_IDENT, "y", 0));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 0));

    tokens.push_back(Token(Token::TK_TAG, "tag", 0));
    tokens.push_back(Token(Token::TK_IDENT, "L1", 0));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 0));

    tokens.push_back(Token(Token::TK_LET, "let", 0));
    tokens.push_back(Token(Token::TK_IDENT, "x", 0));
    tokens.push_back(Token(Token::TK_ASSIGN, "=", 0));
    tokens.push_back(Token(Token::TK_LPARENT, "(", 0));
    tokens.push_back(Token(Token::TK_IDENT, "x", 0));
    tokens.push_back(Token(Token::TK_MULT, "*", 0));
    tokens.push_back(Token(Token::TK_NUMBER, "2", 0));
    tokens.push_back(Token(Token::TK_PLUS, "+", 0));
    tokens.push_back(Token(Token::TK_IDENT, "y", 0));
    tokens.push_back(Token(Token::TK_MULT, "*", 0));
    tokens.push_back(Token(Token::TK_LPARENT, "(", 0));
    tokens.push_back(Token(Token::TK_IDENT, "z", 0));
    tokens.push_back(Token(Token::TK_MINU, "-", 0));
    tokens.push_back(Token(Token::TK_NUMBER, "1", 0));
    tokens.push_back(Token(Token::TK_MULT, "*", 0));
    tokens.push_back(Token(Token::TK_NUMBER, "2", 0));
    tokens.push_back(Token(Token::TK_RPARENT, ")", 0));
    tokens.push_back(Token(Token::TK_RPARENT, ")", 0));
    tokens.push_back(Token(Token::TK_PLUS, "+", 0));
    tokens.push_back(Token(Token::TK_IDENT, "y", 0));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 0));

    tokens.push_back(Token(Token::TK_IF, "if", 0));
    tokens.push_back(Token(Token::TK_IDENT, "x", 0));
    tokens.push_back(Token(Token::TK_LT, "<", 0));
    tokens.push_back(Token(Token::TK_NUMBER, "2", 0));
    tokens.push_back(Token(Token::TK_TO, "to", 0));
    tokens.push_back(Token(Token::TK_IDENT, "L1", 0));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 0));

    tokens.push_back(Token(Token::TK_TO, "to", 0));
    tokens.push_back(Token(Token::TK_IDENT, "L2", 0));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 0));

    tokens.push_back(Token(Token::TK_GET, "get", 0));
    tokens.push_back(Token(Token::TK_IDENT, "w", 0));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 0));

    tokens.push_back(Token(Token::TK_TAG, "tag", 0));
    tokens.push_back(Token(Token::TK_IDENT, "L2", 0));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 0));

    tokens.push_back(Token(Token::TK_PUT, "put", 0));
    tokens.push_back(Token(Token::TK_IDENT, "a", 0));
    tokens.push_back(Token(Token::TK_SEMINCN, ";", 0));
}

static void create_ans(std::string &ans) {
    ans = R"(FN fn
IDENFR f
<Ident>
LPARENT (
IDENFR a
<Ident>
COMMA ,
IDENFR b
<Ident>
<FuncFParams>
RPARENT )
RARROW =>
IDENFR a
<Ident>
PLUS +
IDENFR b
<Ident>
<AddExp>
SEMICN ;
<FuncDef>
FN fn
IDENFR g
<Ident>
LPARENT (
<FuncFParams>
RPARENT )
RARROW =>
Number 3
<Number>
SEMICN ;
<FuncDef>
VARTK var
IDENFR x
<Ident>
SEMICN ;
<VarDecl>
VARTK var
IDENFR y
<Ident>
SEMICN ;
<VarDecl>
VARTK var
IDENFR z
<Ident>
SEMICN ;
<VarDecl>
VARTK var
IDENFR w
<Ident>
SEMICN ;
<VarDecl>
PUTTK put
IDENFR g
<Ident>
LPARENT (
<FuncRParams>
RPARENT )
<CallExp>
SEMICN ;
<Stmt>
GETTK get
IDENFR x
<Ident>
SEMICN ;
<Stmt>
GETTK get
IDENFR y
<Ident>
SEMICN ;
<Stmt>
GETTK get
IDENFR z
<Ident>
SEMICN ;
<Stmt>
PUTTK put
IDENFR f
<Ident>
LPARENT (
IDENFR x
<Ident>
COMMA ,
Number 1
<Number>
<FuncRParams>
RPARENT )
<CallExp>
SEMICN ;
<Stmt>
LETTK let
IDENFR x
<Ident>
ASSIGN =
IDENFR y
<Ident>
SEMICN ;
<Stmt>
TAGTK tag
IDENFR L1
<Ident>
SEMICN ;
<Stmt>
LETTK let
IDENFR x
<Ident>
ASSIGN =
IDENFR x
<Ident>
MULT *
Number 2
<Number>
<MulExp>
PLUS +
IDENFR y
<Ident>
MULT *
IDENFR z
<Ident>
MINU -
Number 1
<Number>
MULT *
Number 2
<Number>
<MulExp>
<AddExp>
<MulExp>
<AddExp>
PLUS +
IDENFR y
<Ident>
<AddExp>
SEMICN ;
<Stmt>
IFTK if
IDENFR x
<Ident>
LSS <
Number 2
<Number>
<Cond>
TOTK to
IDENFR L1
<Ident>
SEMICN ;
<Stmt>
TOTK to
IDENFR L2
<Ident>
SEMICN ;
<Stmt>
GETTK get
IDENFR w
<Ident>
SEMICN ;
<Stmt>
TAGTK tag
IDENFR L2
<Ident>
SEMICN ;
<Stmt>
PUTTK put
IDENFR a
<Ident>
SEMICN ;
<Stmt>
<CompUnit>
)";
}
