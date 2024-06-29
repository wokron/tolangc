#include <ast.h>
#include <doctest.h>
#include <front/lexer/lexer.h>
#include <front/parser/parser.h>
#include <vector>
#include <iostream>
using namespace std;

/*
fn f(a,b) => a + b;
var x;
var y;
var z;
var w;
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

TEST_CASE("testing ast") {
    // 词法分析
    vector<Token> tokens;
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
    
    
    // 语法分析
    Parser parser(tokens);
    struct CompUnit compUnit = parser.parseCompUnit();
    ofstream outFile("output.txt", ios::out);
    compUnit.print(std::cout);

}
