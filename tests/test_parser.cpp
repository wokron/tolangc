#include "doctest.h"
#include "tolang/lexer.h"
#include "tolang/parser.h"
#include <sstream>

constexpr char INPUT[] = R"(
fn add(a, b) => a + b;

var n;
var i;
var a;
var b;

get n;
let i = 0;

tag cond;
if i >= n to done;
tag do;

get a;
get b;
put add(a, b);

let i = i + 1;
to cond;
tag done;
)";

const char EXPECTED[] = R"(FN fn
IDENFR add
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
VARTK var
IDENFR n
<Ident>
SEMICN ;
<VarDecl>
VARTK var
IDENFR i
<Ident>
SEMICN ;
<VarDecl>
VARTK var
IDENFR a
<Ident>
SEMICN ;
<VarDecl>
VARTK var
IDENFR b
<Ident>
SEMICN ;
<VarDecl>
GETTK get
IDENFR n
<Ident>
SEMICN ;
<Stmt>
LETTK let
IDENFR i
<Ident>
ASSIGN =
Number 0
<Number>
SEMICN ;
<Stmt>
TAGTK tag
IDENFR cond
<Ident>
SEMICN ;
<Stmt>
IFTK if
IDENFR i
<Ident>
GEQ >=
IDENFR n
<Ident>
<Cond>
TOTK to
IDENFR done
<Ident>
SEMICN ;
<Stmt>
TAGTK tag
IDENFR do
<Ident>
SEMICN ;
<Stmt>
GETTK get
IDENFR a
<Ident>
SEMICN ;
<Stmt>
GETTK get
IDENFR b
<Ident>
SEMICN ;
<Stmt>
PUTTK put
IDENFR add
<Ident>
LPARENT (
IDENFR a
<Ident>
COMMA ,
IDENFR b
<Ident>
<FuncRParams>
RPARENT )
<CallExp>
SEMICN ;
<Stmt>
LETTK let
IDENFR i
<Ident>
ASSIGN =
IDENFR i
<Ident>
PLUS +
Number 1
<Number>
<AddExp>
SEMICN ;
<Stmt>
TOTK to
IDENFR cond
<Ident>
SEMICN ;
<Stmt>
TAGTK tag
IDENFR done
<Ident>
SEMICN ;
<Stmt>
<CompUnit>
)";

TEST_CASE("testing parser") {
    std::istringstream iss(INPUT);

    Lexer lexer(iss);

    Parser parser(lexer);
    auto root = parser.parse();

    std::ostringstream oss;
    root->print(oss);

    CHECK(oss.str() == EXPECTED);
}