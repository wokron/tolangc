#include "doctest.h"
#include "front/parser/parser.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>

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

    // 语法分析
    Parser parser(tokens);
    CompUnit compUnit = *parser.parseCompUnit();
    std::ostringstream out;
    compUnit.print(out);
    std::string ans;
    create_ans(ans);

    assert(out.str() == ans);
}

static void create_tokens(std::vector<Token> &tokens) {

    tokens.push_back(Token(Token::FN, "fn", 0));
    tokens.push_back(Token(Token::IDENFR, "f", 0));
    tokens.push_back(Token(Token::LPARENT, "(", 0));
    tokens.push_back(Token(Token::IDENFR, "a", 0));
    tokens.push_back(Token(Token::COMMA, ",", 0));
    tokens.push_back(Token(Token::IDENFR, "b", 0));
    tokens.push_back(Token(Token::RPARENT, ")", 0));
    tokens.push_back(Token(Token::RARROW, "=>", 0));
    tokens.push_back(Token(Token::IDENFR, "a", 0));
    tokens.push_back(Token(Token::PLUS, "+", 0));
    tokens.push_back(Token(Token::IDENFR, "b", 0));
    tokens.push_back(Token(Token::SEMINCN, ";", 0));

    tokens.push_back(Token(Token::FN, "fn", 0));
    tokens.push_back(Token(Token::IDENFR, "g", 0));
    tokens.push_back(Token(Token::LPARENT, "(", 0));
    tokens.push_back(Token(Token::RPARENT, ")", 0));
    tokens.push_back(Token(Token::RARROW, "=>", 0));
    tokens.push_back(Token(Token::NUMBER, "3", 0));
    tokens.push_back(Token(Token::SEMINCN, ";", 0));

    tokens.push_back(Token(Token::VARTK, "var", 0));
    tokens.push_back(Token(Token::IDENFR, "x", 0));
    tokens.push_back(Token(Token::SEMINCN, ";", 0));

    tokens.push_back(Token(Token::VARTK, "var", 0));
    tokens.push_back(Token(Token::IDENFR, "y", 0));
    tokens.push_back(Token(Token::SEMINCN, ";", 0));

    tokens.push_back(Token(Token::VARTK, "var", 0));
    tokens.push_back(Token(Token::IDENFR, "z", 0));
    tokens.push_back(Token(Token::SEMINCN, ";", 0));

    tokens.push_back(Token(Token::VARTK, "var", 0));
    tokens.push_back(Token(Token::IDENFR, "w", 0));
    tokens.push_back(Token(Token::SEMINCN, ";", 0));

    tokens.push_back(Token(Token::PUTTK, "put", 0));
    tokens.push_back(Token(Token::IDENFR, "g", 0));
    tokens.push_back(Token(Token::LPARENT, "(", 0));
    tokens.push_back(Token(Token::RPARENT, ")", 0));
    tokens.push_back(Token(Token::SEMINCN, ";", 0));

    tokens.push_back(Token(Token::GETTK, "get", 0));
    tokens.push_back(Token(Token::IDENFR, "x", 0));
    tokens.push_back(Token(Token::SEMINCN, ";", 0));

    tokens.push_back(Token(Token::GETTK, "get", 0));
    tokens.push_back(Token(Token::IDENFR, "y", 0));
    tokens.push_back(Token(Token::SEMINCN, ";", 0));

    tokens.push_back(Token(Token::GETTK, "get", 0));
    tokens.push_back(Token(Token::IDENFR, "z", 0));
    tokens.push_back(Token(Token::SEMINCN, ";", 0));

    tokens.push_back(Token(Token::PUTTK, "put", 0));
    tokens.push_back(Token(Token::IDENFR, "f", 0));
    tokens.push_back(Token(Token::LPARENT, "(", 0));
    tokens.push_back(Token(Token::IDENFR, "x", 0));
    tokens.push_back(Token(Token::COMMA, ",", 0));
    tokens.push_back(Token(Token::NUMBER, "1", 0));
    tokens.push_back(Token(Token::RPARENT, ")", 0));
    tokens.push_back(Token(Token::SEMINCN, ";", 0));

    tokens.push_back(Token(Token::LETTK, "let", 0));
    tokens.push_back(Token(Token::IDENFR, "x", 0));
    tokens.push_back(Token(Token::ASSIGN, "=", 0));
    tokens.push_back(Token(Token::IDENFR, "y", 0));
    tokens.push_back(Token(Token::SEMINCN, ";", 0));

    tokens.push_back(Token(Token::TAGTK, "tag", 0));
    tokens.push_back(Token(Token::IDENFR, "L1", 0));
    tokens.push_back(Token(Token::SEMINCN, ";", 0));

    tokens.push_back(Token(Token::LETTK, "let", 0));
    tokens.push_back(Token(Token::IDENFR, "x", 0));
    tokens.push_back(Token(Token::ASSIGN, "=", 0));
    tokens.push_back(Token(Token::LPARENT, "(", 0));
    tokens.push_back(Token(Token::IDENFR, "x", 0));
    tokens.push_back(Token(Token::MULT, "*", 0));
    tokens.push_back(Token(Token::NUMBER, "2", 0));
    tokens.push_back(Token(Token::PLUS, "+", 0));
    tokens.push_back(Token(Token::IDENFR, "y", 0));
    tokens.push_back(Token(Token::MULT, "*", 0));
    tokens.push_back(Token(Token::LPARENT, "(", 0));
    tokens.push_back(Token(Token::IDENFR, "z", 0));
    tokens.push_back(Token(Token::MINU, "-", 0));
    tokens.push_back(Token(Token::NUMBER, "1", 0));
    tokens.push_back(Token(Token::MULT, "*", 0));
    tokens.push_back(Token(Token::NUMBER, "2", 0));
    tokens.push_back(Token(Token::RPARENT, ")", 0));
    tokens.push_back(Token(Token::RPARENT, ")", 0));
    tokens.push_back(Token(Token::PLUS, "+", 0));
    tokens.push_back(Token(Token::IDENFR, "y", 0));
    tokens.push_back(Token(Token::SEMINCN, ";", 0));

    tokens.push_back(Token(Token::IFTK, "if", 0));
    tokens.push_back(Token(Token::IDENFR, "x", 0));
    tokens.push_back(Token(Token::LSS, "<", 0));
    tokens.push_back(Token(Token::NUMBER, "2", 0));
    tokens.push_back(Token(Token::TOTK, "to", 0));
    tokens.push_back(Token(Token::IDENFR, "L1", 0));
    tokens.push_back(Token(Token::SEMINCN, ";", 0));

    tokens.push_back(Token(Token::TOTK, "to", 0));
    tokens.push_back(Token(Token::IDENFR, "L2", 0));
    tokens.push_back(Token(Token::SEMINCN, ";", 0));

    tokens.push_back(Token(Token::GETTK, "get", 0));
    tokens.push_back(Token(Token::IDENFR, "w", 0));
    tokens.push_back(Token(Token::SEMINCN, ";", 0));

    tokens.push_back(Token(Token::TAGTK, "tag", 0));
    tokens.push_back(Token(Token::IDENFR, "L2", 0));
    tokens.push_back(Token(Token::SEMINCN, ";", 0));

    tokens.push_back(Token(Token::PUTTK, "put", 0));
    tokens.push_back(Token(Token::IDENFR, "a", 0));
    tokens.push_back(Token(Token::SEMINCN, ";", 0));
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
