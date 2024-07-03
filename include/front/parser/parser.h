#pragma once

#include "ast.h"
#include "front/lexer/token.h"
#include <vector>

class Parser {
public:
    std::shared_ptr<CompUnit> parseCompUnit();
    Parser(std::vector<Token> &t) : tokens(t) { pos = 0; };

private:
    std::vector<Token> tokens;
    int pos;
    Token getToken();
    bool hasNext();
    std::shared_ptr<Ident> parseIdent();
    std::shared_ptr<FuncFParams> parseFuncFParams();
    std::shared_ptr<FuncDef> parseFuncDef();
    std::shared_ptr<VarDecl> parseVarDecl();
    std::shared_ptr<Stmt> parseStmt();
    std::shared_ptr<GetStmt> parseGetStmt();
    std::shared_ptr<PutStmt> parsePutStmt();
    std::shared_ptr<TagStmt> parseTagStmt();
    std::shared_ptr<LetStmt> parseLetStmt();
    std::shared_ptr<Cond> parseCond();
    std::shared_ptr<IfStmt> parseIfStmt();
    std::shared_ptr<ToStmt> parseToStmt();
    std::shared_ptr<Exp> parseExp();
    std::shared_ptr<Exp> parseAddExp();
    std::shared_ptr<Exp> parseMulExp();
    std::shared_ptr<CallExp> parseCallExp();
    std::shared_ptr<UnaryExp> parseUnaryExp();
    std::shared_ptr<FuncRParams> parseFuncRParams();
    std::shared_ptr<Number> parseNumber();
};
