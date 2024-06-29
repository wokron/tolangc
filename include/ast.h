//
// Created by Zengyuankun on 2024/5/31.
//
#pragma once

#include"front/lexer/token.h"
#include"ast.h"
#include<vector>
using namespace std;
class Parser {
public:
    shared_ptr<CompUnit> parseCompUnit();
    Parser(vector<Token>& t):tokens(t){pos = 0;};

private:
    vector<Token> tokens;
    int pos;
    Token getToken();
    bool hasNext();
    shared_ptr<Ident> parseIdent();
    shared_ptr<FuncFParams> parseFuncFParams();
    shared_ptr<FuncDef> parseFuncDef();
    shared_ptr<VarDecl> parseVarDecl();
    shared_ptr<Stmt> parseStmt();
    shared_ptr<GetStmt> parseGetStmt();
    shared_ptr<PutStmt> parsePutStmt();
    shared_ptr<TagStmt> parseTagStmt();
    shared_ptr<LetStmt> parseLetStmt();
    shared_ptr<Cond> parseCond();
    shared_ptr<IfStmt> parseIfStmt();
    shared_ptr<ToStmt> parseToStmt();
    shared_ptr<Exp> parseExp();
    shared_ptr<BinaryExp> parseAddExp();
    shared_ptr<BinaryExp> parseMulExp();
    shared_ptr<CallExp> parseCallExp();
    shared_ptr<UnaryExp> parseUnaryExp();
    shared_ptr<FuncRParams> parseFuncRParams();
    shared_ptr<Number> parseNumber();

};
