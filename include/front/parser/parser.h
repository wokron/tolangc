//
// Created by Zengyuankun on 2024/5/31.
//
#pragma once

#include <front/lexer/token.h>
#include <ast.h>
#include<vector>
using namespace std;
class Parser {
  public:
    struct CompUnit parseCompUnit();
    Parser(vector<Token>& t):tokens(t){pos = 0;};

  private:
    vector<Token> tokens;
    int pos;
    Token getToken();
    bool hasNext();
    struct Ident parseIdent();
    struct FuncFParams parseFuncFParams();
    struct FuncDef parseFuncDef();
    struct VarDecl parseVarDecl();
    Stmt parseStmt();
    struct GetStmt parseGetStmt();
    struct PutStmt parsePutStmt();
    struct TagStmt parseTagStmt();
    struct LetStmt parseLetStmt();
    struct Cond parseCond();
    struct IfStmt parseIfStmt();
    struct ToStmt parseToStmt();
    Exp parseExp();
    struct BinaryExp parseAddExp();
    struct BinaryExp parseMulExp();
    struct CallExp parseCallExp();
    struct UnaryExp parseUnaryExp();
    struct FuncRParams parseFuncRParams();
    struct Number parseNumber();

};
