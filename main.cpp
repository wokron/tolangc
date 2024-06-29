#include <ast.h>
#include <front/lexer/lexer.h>
#include <front/parser/parser.h>
#include <fstream>
#include <vector>

#include <iostream>
int main() {
    // std::ifstream inFile("testfile.txt", ios::in);

    // // 词法分析
    // Lexer lexer(inFile);
    // Token cur(Token::TokenType::ASSIGN, "s", 0);
    // vector<Token> tokens;
    // while (lexer.next(cur)) {
    //     tokens.push_back(cur);
    //     // cout  << cur.content << endl;
    // }

    // // 语法分析
    // Parser parser(tokens);
    // struct CompUnit compUnit = parser.parseCompUnit();
    // ofstream outFile("output.txt", ios::out);
    // compUnit.print(outFile);

    return 0;
}
