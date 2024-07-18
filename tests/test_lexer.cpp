// #include "doctest.h"

// #include "front/lexer/lexer.h"
// #include "front/lexer/token.h"

// std::string src;
// std::vector<Token> tokens;

// static void AddSrc();
// static void AddResult();

// TEST_CASE("testing lexer") {
//     AddSrc();
//     AddResult();

//     std::istringstream input(src);
//     Lexer lexer = Lexer(input);
//     Token token = Token(Token::ERR, "", -1);

//     int i = 0;
//     while (lexer.next(token)) {
//         CHECK(i <= tokens.size());
//         Token result = tokens.at(i);
//         CHECK_EQ(token.token_type, result.token_type);
//         CHECK_EQ(token.content, result.content);
//         CHECK_EQ(token.line, result.line);
//         i++;
//     }
//     CHECK_EQ(i, tokens.size());
// }

// static void AddSrc() {
//     src = "fn nonParam() => 1 - 3.3;\n"
//           "fn paramOne(a) => a * 0.2;\n"
//           "fn paramMore(a,b,_c,d) => a / (b + _c) % d;\n"
//           "get a;\n"
//           "put a;// one line comment\n"
//           "let n = a - 2.000003;/* more line comment\n"
//           "another line\n"
//           "final line \n"
//           "*/\n"
//           "let i = 0;\n"
//           "tag entry;\n"
//           "a = - a;\n"
//           "put a * i;\n"
//           "let i = i + 1;\n"
//           "if i <= n to entry;/* maybe ok */\n";
// }

// static void AddResult() {
//     tokens.push_back(Token(Token::FN, "fn", 1));
//     tokens.push_back(Token(Token::IDENFR, "nonParam", 1));
//     tokens.push_back(Token(Token::LPARENT, "(", 1));
//     tokens.push_back(Token(Token::RPARENT, ")", 1));
//     tokens.push_back(Token(Token::RARROW, "=>", 1));
//     tokens.push_back(Token(Token::NUMBER, "1", 1));
//     tokens.push_back(Token(Token::MINU, "-", 1));
//     tokens.push_back(Token(Token::NUMBER, "3.3", 1));
//     tokens.push_back(Token(Token::SEMINCN, ";", 1));

//     tokens.push_back(Token(Token::FN, "fn", 2));
//     tokens.push_back(Token(Token::IDENFR, "paramOne", 2));
//     tokens.push_back(Token(Token::LPARENT, "(", 2));
//     tokens.push_back(Token(Token::IDENFR, "a", 2));
//     tokens.push_back(Token(Token::RPARENT, ")", 2));
//     tokens.push_back(Token(Token::RARROW, "=>", 2));
//     tokens.push_back(Token(Token::IDENFR, "a", 2));
//     tokens.push_back(Token(Token::MULT, "*", 2));
//     tokens.push_back(Token(Token::NUMBER, "0.2", 2));
//     tokens.push_back(Token(Token::SEMINCN, ";", 2));

//     tokens.push_back(Token(Token::FN, "fn", 3));
//     tokens.push_back(Token(Token::IDENFR, "paramMore", 3));
//     tokens.push_back(Token(Token::LPARENT, "(", 3));
//     tokens.push_back(Token(Token::IDENFR, "a", 3));
//     tokens.push_back(Token(Token::COMMA, ",", 3));
//     tokens.push_back(Token(Token::IDENFR, "b", 3));
//     tokens.push_back(Token(Token::COMMA, ",", 3));
//     tokens.push_back(Token(Token::IDENFR, "_c", 3));
//     tokens.push_back(Token(Token::COMMA, ",", 3));
//     tokens.push_back(Token(Token::IDENFR, "d", 3));
//     tokens.push_back(Token(Token::RPARENT, ")", 3));
//     tokens.push_back(Token(Token::RARROW, "=>", 3));
//     tokens.push_back(Token(Token::IDENFR, "a", 3));
//     tokens.push_back(Token(Token::DIV, "/", 3));
//     tokens.push_back(Token(Token::LPARENT, "(", 3));
//     tokens.push_back(Token(Token::IDENFR, "b", 3));
//     tokens.push_back(Token(Token::PLUS, "+", 3));
//     tokens.push_back(Token(Token::IDENFR, "_c", 3));
//     tokens.push_back(Token(Token::RPARENT, ")", 3));
//     tokens.push_back(Token(Token::MOD, "%", 3));
//     tokens.push_back(Token(Token::IDENFR, "d", 3));
//     tokens.push_back(Token(Token::SEMINCN, ";", 3));

//     tokens.push_back(Token(Token::GETTK, "get", 4));
//     tokens.push_back(Token(Token::IDENFR, "a", 4));
//     tokens.push_back(Token(Token::SEMINCN, ";", 4));

//     tokens.push_back(Token(Token::PUTTK, "put", 5));
//     tokens.push_back(Token(Token::IDENFR, "a", 5));
//     tokens.push_back(Token(Token::SEMINCN, ";", 5));

//     tokens.push_back(Token(Token::LETTK, "let", 6));
//     tokens.push_back(Token(Token::IDENFR, "n", 6));
//     tokens.push_back(Token(Token::ASSIGN, "=", 6));
//     tokens.push_back(Token(Token::IDENFR, "a", 6));
//     tokens.push_back(Token(Token::MINU, "-", 6));
//     tokens.push_back(Token(Token::NUMBER, "2.000003", 6));
//     tokens.push_back(Token(Token::SEMINCN, ";", 6));

//     tokens.push_back(Token(Token::LETTK, "let", 10));
//     tokens.push_back(Token(Token::IDENFR, "i", 10));
//     tokens.push_back(Token(Token::ASSIGN, "=", 10));
//     tokens.push_back(Token(Token::NUMBER, "0", 10));
//     tokens.push_back(Token(Token::SEMINCN, ";", 10));

//     tokens.push_back(Token(Token::TAGTK, "tag", 11));
//     tokens.push_back(Token(Token::IDENFR, "entry", 11));
//     tokens.push_back(Token(Token::SEMINCN, ";", 11));

//     tokens.push_back(Token(Token::IDENFR, "a", 12));
//     tokens.push_back(Token(Token::ASSIGN, "=", 12));
//     tokens.push_back(Token(Token::MINU, "-", 12));
//     tokens.push_back(Token(Token::IDENFR, "a", 12));
//     tokens.push_back(Token(Token::SEMINCN, ";", 12));

//     tokens.push_back(Token(Token::PUTTK, "put", 13));
//     tokens.push_back(Token(Token::IDENFR, "a", 13));
//     tokens.push_back(Token(Token::MULT, "*", 13));
//     tokens.push_back(Token(Token::IDENFR, "i", 13));
//     tokens.push_back(Token(Token::SEMINCN, ";", 13));

//     tokens.push_back(Token(Token::LETTK, "let", 14));
//     tokens.push_back(Token(Token::IDENFR, "i", 14));
//     tokens.push_back(Token(Token::ASSIGN, "=", 14));
//     tokens.push_back(Token(Token::IDENFR, "i", 14));
//     tokens.push_back(Token(Token::PLUS, "+", 14));
//     tokens.push_back(Token(Token::NUMBER, "1", 14));
//     tokens.push_back(Token(Token::SEMINCN, ";", 14));

//     tokens.push_back(Token(Token::IFTK, "if", 15));
//     tokens.push_back(Token(Token::IDENFR, "i", 15));
//     tokens.push_back(Token(Token::LEQ, "<=", 15));
//     tokens.push_back(Token(Token::IDENFR, "n", 15));
//     tokens.push_back(Token(Token::TOTK, "to", 15));
//     tokens.push_back(Token(Token::IDENFR, "entry", 15));
//     tokens.push_back(Token(Token::SEMINCN, ";", 15));
// }