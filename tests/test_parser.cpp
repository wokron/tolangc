#include "doctest.h"
#include "front/lexer/lexer.h"
#include "front/parser/parser.h"
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

// TODO: write the expected output
const char EXPECTED[] = "";

TEST_CASE("testing parser") {
    std::istringstream iss(INPUT);

    Lexer lexer(iss);
    Token cur(Token::TokenType::ASSIGN, "s", 0);
    std::vector<Token> tokens;
    while (lexer.next(cur)) {
        tokens.push_back(cur);
    }

    Parser parser(tokens);
    auto root = parser.parseCompUnit();

    std::ostringstream oss;
    root->print(oss);

    // TODO: check the output
    // CHECK(oss.str() == EXPECTED);
}