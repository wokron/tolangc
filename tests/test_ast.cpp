#include "ast.h"
#include "doctest.h"
#include "front/lexer/lexer.h"
#include "front/parser/parser.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <cassert>
using namespace std;

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

static void create_tokens(vector<Token>& tokens);
static void create_ans(string& ans);

TEST_CASE("AST TEST") {
    // 词法分析
    vector<Token> tokens;
    create_tokens(tokens);


    // 语法分析
    Parser parser(tokens);
    CompUnit compUnit = *parser.parseCompUnit();
    ofstream outFile("output.txt", ios::out);
    ostringstream out;
    compUnit.print(out);
    string ans;
    create_ans(ans);

    assert(out.str() == ans);


}

void create_tokens(vector<Token>& tokens) {

    tokens.push_back(Token(Token::FN,"fn",0));
    tokens.push_back(Token(Token::IDENFR,"f",0));
    tokens.push_back(Token(Token::LPARENT,"(",0));
    tokens.push_back(Token(Token::IDENFR,"a",0));
    tokens.push_back(Token(Token::COMMA,",",0));
    tokens.push_back(Token(Token::IDENFR,"b",0));
    tokens.push_back(Token(Token::RPARENT,")",0));
    tokens.push_back(Token(Token::RARROW,"=>",0));
    tokens.push_back(Token(Token::IDENFR,"a",0));
    tokens.push_back(Token(Token::PLUS,"+",0));
    tokens.push_back(Token(Token::IDENFR,"b",0));
    tokens.push_back(Token(Token::SEMINCN,";",0));

    tokens.push_back(Token(Token::FN,"fn",0));
    tokens.push_back(Token(Token::IDENFR,"g",0));
    tokens.push_back(Token(Token::LPARENT,"(",0));
    tokens.push_back(Token(Token::RPARENT,")",0));
    tokens.push_back(Token(Token::RARROW,"=>",0));
    tokens.push_back(Token(Token::IDENFR,"3",0));
    tokens.push_back(Token(Token::SEMINCN,";",0));

    tokens.push_back(Token(Token::VARTK,"var",0));
    tokens.push_back(Token(Token::IDENFR,"x",0));
    tokens.push_back(Token(Token::SEMINCN,";",0));

    tokens.push_back(Token(Token::VARTK,"var",0));
    tokens.push_back(Token(Token::IDENFR,"y",0));
    tokens.push_back(Token(Token::SEMINCN,";",0));

    tokens.push_back(Token(Token::VARTK,"var",0));
    tokens.push_back(Token(Token::IDENFR,"z",0));
    tokens.push_back(Token(Token::SEMINCN,";",0));

    tokens.push_back(Token(Token::VARTK,"var",0));
    tokens.push_back(Token(Token::IDENFR,"w",0));
    tokens.push_back(Token(Token::SEMINCN,";",0));

    tokens.push_back(Token(Token::PUTTK,"put",0));
    tokens.push_back(Token(Token::IDENFR,"g",0));
    tokens.push_back(Token(Token::LPARENT,"(",0));
    tokens.push_back(Token(Token::RPARENT,")",0));
    tokens.push_back(Token(Token::SEMINCN,";",0));

    tokens.push_back(Token(Token::GETTK,"get",0));
    tokens.push_back(Token(Token::IDENFR,"x",0));
    tokens.push_back(Token(Token::SEMINCN,";",0));

    tokens.push_back(Token(Token::GETTK,"get",0));
    tokens.push_back(Token(Token::IDENFR,"y",0));
    tokens.push_back(Token(Token::SEMINCN,";",0));

    tokens.push_back(Token(Token::GETTK,"get",0));
    tokens.push_back(Token(Token::IDENFR,"z",0));
    tokens.push_back(Token(Token::SEMINCN,";",0));

    tokens.push_back(Token(Token::PUTTK,"put",0));
    tokens.push_back(Token(Token::IDENFR,"f",0));
    tokens.push_back(Token(Token::LPARENT,"(",0));
    tokens.push_back(Token(Token::IDENFR,"x",0));
    tokens.push_back(Token(Token::COMMA,",",0));
    tokens.push_back(Token(Token::NUMBER,"1",0));
    tokens.push_back(Token(Token::RPARENT,")",0));
    tokens.push_back(Token(Token::SEMINCN,";",0));

    tokens.push_back(Token(Token::LETTK,"let",0));
    tokens.push_back(Token(Token::IDENFR,"x",0));
    tokens.push_back(Token(Token::ASSIGN,"=",0));
    tokens.push_back(Token(Token::IDENFR,"y",0));
    tokens.push_back(Token(Token::SEMINCN,";",0));

    tokens.push_back(Token(Token::TAGTK,"tag",0));
    tokens.push_back(Token(Token::IDENFR,"L1",0));
    tokens.push_back(Token(Token::SEMINCN,";",0));

    tokens.push_back(Token(Token::LETTK,"let",0));
    tokens.push_back(Token(Token::IDENFR,"x",0));
    tokens.push_back(Token(Token::ASSIGN,"=",0));
    tokens.push_back(Token(Token::LPARENT,"(",0));
    tokens.push_back(Token(Token::IDENFR,"x",0));
    tokens.push_back(Token(Token::MULT,"*",0));
    tokens.push_back(Token(Token::NUMBER,"2",0));
    tokens.push_back(Token(Token::PLUS,"+",0));
    tokens.push_back(Token(Token::IDENFR,"y",0));
    tokens.push_back(Token(Token::MULT,"*",0));
    tokens.push_back(Token(Token::LPARENT,"(",0));
    tokens.push_back(Token(Token::IDENFR,"z",0));
    tokens.push_back(Token(Token::MINU,"-",0));
    tokens.push_back(Token(Token::NUMBER,"1",0));
    tokens.push_back(Token(Token::MULT,"*",0));
    tokens.push_back(Token(Token::NUMBER,"2",0));
    tokens.push_back(Token(Token::RPARENT,")",0));
    tokens.push_back(Token(Token::RPARENT,")",0));
    tokens.push_back(Token(Token::PLUS,"+",0));
    tokens.push_back(Token(Token::IDENFR,"y",0));
    tokens.push_back(Token(Token::SEMINCN,";",0));

    tokens.push_back(Token(Token::IFTK,"if",0));
    tokens.push_back(Token(Token::IDENFR,"x",0));
    tokens.push_back(Token(Token::LSS,"<",0));
    tokens.push_back(Token(Token::NUMBER,"2",0));
    tokens.push_back(Token(Token::TOTK,"to",0));
    tokens.push_back(Token(Token::IDENFR,"L1",0));
    tokens.push_back(Token(Token::SEMINCN,";",0));

    tokens.push_back(Token(Token::TOTK,"to",0));
    tokens.push_back(Token(Token::IDENFR,"L2",0));
    tokens.push_back(Token(Token::SEMINCN,";",0));

    tokens.push_back(Token(Token::GETTK,"get",0));
    tokens.push_back(Token(Token::IDENFR,"w",0));
    tokens.push_back(Token(Token::SEMINCN,";",0));

    tokens.push_back(Token(Token::TAGTK,"tag",0));
    tokens.push_back(Token(Token::IDENFR,"L2",0));
    tokens.push_back(Token(Token::SEMINCN,";",0));

    tokens.push_back(Token(Token::PUTTK,"put",0));
    tokens.push_back(Token(Token::IDENFR,"a",0));
    tokens.push_back(Token(Token::SEMINCN,";",0));
}

void create_ans(string& ans) {
    ans =R"(FN fn
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
<UnaryExp>
<MulExp>
PLUS +
IDENFR b
<Ident>
<UnaryExp>
<MulExp>
<AddExp>
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
IDENFR 3
<Ident>
<UnaryExp>
<MulExp>
<AddExp>
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
<UnaryExp>
<MulExp>
<AddExp>
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
<UnaryExp>
<MulExp>
<AddExp>
COMMA ,
Number 1
<Number>
<UnaryExp>
<MulExp>
<AddExp>
<FuncRParams>
RPARENT )
<CallExp>
<UnaryExp>
<MulExp>
<AddExp>
SEMICN ;
<Stmt>
LETTK let
IDENFR x
<Ident>
ASSIGN =
IDENFR y
<Ident>
<UnaryExp>
<MulExp>
<AddExp>
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
LPARENT (
IDENFR x
<Ident>
<UnaryExp>
MULT *
Number 2
<Number>
<UnaryExp>
<MulExp>
<MulExp>
PLUS +
IDENFR y
<Ident>
<UnaryExp>
MULT *
LPARENT (
IDENFR z
<Ident>
<UnaryExp>
<MulExp>
MINU -
Number 1
<Number>
<UnaryExp>
MULT *
Number 2
<Number>
<UnaryExp>
<MulExp>
<MulExp>
<AddExp>
<AddExp>
RPARENT )
<UnaryExp>
<MulExp>
<MulExp>
<AddExp>
<AddExp>
RPARENT )
<UnaryExp>
<MulExp>
PLUS +
IDENFR y
<Ident>
<UnaryExp>
<MulExp>
<AddExp>
<AddExp>
SEMICN ;
<Stmt>
IFTK if
IDENFR x
<Ident>
<UnaryExp>
<MulExp>
<AddExp>
LSS <
Number 2
<Number>
<UnaryExp>
<MulExp>
<AddExp>
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
<UnaryExp>
<MulExp>
<AddExp>
SEMICN ;
<Stmt>
<CompUnit>
)";
}
