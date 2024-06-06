//
// Created by Zengyuankun on 2024/5/31.
//
#include <front/lexer/token.h>
#include <front/parser/parser.h>
#include <memory>
#include <string>

using namespace std;

Token Parser::getToken() { return tokens[pos]; }

bool Parser::hasNext() { return pos < tokens.size(); }
struct CompUnit Parser::parseCompUnit() {
    struct CompUnit compUnit;
    while (hasNext() && getToken().token_type == Token::FN) {
        compUnit.funcDefs.push_back(make_shared<FuncDef>(parseFuncDef()));
    }
    while (hasNext() && getToken().token_type == Token::VARTK) {
        compUnit.varDecls.push_back(make_shared<VarDecl>(parseVarDecl()));
    }
    while (hasNext()) {
        compUnit.stmts.push_back(make_shared<Stmt>(parseStmt()));
    }
    return compUnit;
}

struct Ident Parser::parseIdent() {
    struct Ident ident;
    ident.ident = getToken().content;
    pos++;
    return ident;
}

struct FuncFParams Parser::parseFuncFParams() {
    struct FuncFParams funcFParams;
    funcFParams.idents.push_back(parseIdent());
    return funcFParams;
};

struct FuncDef Parser::parseFuncDef() {
    struct FuncDef funcDef;
    pos++;
    funcDef.ident = parseIdent();
    pos++;
    if (getToken().token_type == Token::RPARENT) {
        funcDef.funcFParams = nullptr;
    } else {
        funcDef.funcFParams = make_shared<FuncFParams>(parseFuncFParams());
    }
    pos++;
    pos++;
    funcDef.exp = make_shared<Exp>(parseExp());
    pos++;
    return funcDef;
};

struct VarDecl Parser::parseVarDecl() {
    struct VarDecl varDecl;
    pos++;
    varDecl.ident = parseIdent();
    pos++;
    return varDecl;
};

Stmt Parser::parseStmt() {
    Stmt stmt;
    switch (getToken().token_type) {
    case Token::GETTK: {
        stmt = parseGetStmt();
        break;
    }
    case Token::PUTTK: {
        stmt = parsePutStmt();
        break;
    }
    case Token::TAGTK: {
        stmt = parseTagStmt();
        break;
    }
    case Token::LETTK: {
        stmt = parseLetStmt();
        break;
    }
    case Token::IFTK: {
        stmt = parseIfStmt();
        break;
    }
    case Token::TOTK: {
        stmt = parseToStmt();
        break;
    }
    }
    return stmt;
}
struct GetStmt Parser::parseGetStmt(){
    struct GetStmt getStmt;
    pos++;
    getStmt.ident = parseIdent();
    pos++;
    return getStmt;
};

struct PutStmt Parser::parsePutStmt(){
    struct PutStmt putStmt;
    pos++;
    putStmt.exp = make_shared<Exp>(parseExp());
    pos++;
    return putStmt;
};

struct TagStmt Parser::parseTagStmt(){
    struct TagStmt tagStmt;
    pos++;
    tagStmt.ident = parseIdent();
    pos++;
    return tagStmt;
};

struct LetStmt Parser::parseLetStmt(){
    struct LetStmt letStmt;
    pos++;
    letStmt.ident = parseIdent();
    pos++;
    letStmt.exp = make_shared<Exp>(parseExp());
    pos++;
    return letStmt;
};


struct IfStmt Parser::parseIfStmt(){
    struct IfStmt ifStmt;
    pos++;
    ifStmt.cond = make_shared<Cond>(parseCond());
    pos++;
    ifStmt.ident = parseIdent();
    pos++;
    return ifStmt;
};

struct ToStmt Parser::parseToStmt(){
    struct ToStmt toStmt;
    pos++;
    toStmt.ident = parseIdent();
    pos++;
    return toStmt;
};

struct Cond Parser::parseCond(){
    struct Cond cond;
    cond.left = make_shared<Exp>(parseExp());
    cond.op = getToken().token_type;
    pos++;
    cond.right = make_shared<Exp>(parseExp());
    return cond;
};

Exp Parser::parseExp() {
    return parseAddExp();
};

struct BinaryExp Parser::parseAddExp() {
    BinaryExp binaryExp;
    binaryExp.op = Token::PLUS;
    binaryExp.lexp = make_shared<Exp>(parseMulExp());
    if (getToken().token_type == Token::PLUS || getToken().token_type == Token::MINU) {
        binaryExp.op = getToken().token_type;
        pos++;
        binaryExp.rexp = make_shared<Exp>(parseAddExp());
    } else {
        binaryExp.rexp = nullptr;
    }
    return binaryExp;
}

struct BinaryExp Parser::parseMulExp() {
    BinaryExp binaryExp;
    binaryExp.op = Token::MULT;
    binaryExp.lexp = make_shared<Exp>(parseUnaryExp());
    if (getToken().token_type == Token::MULT
        || getToken().token_type == Token::DIV
        || getToken().token_type == Token::MOD) {
        binaryExp.op = getToken().token_type;
        pos++;
        binaryExp.rexp = make_shared<Exp>(parseMulExp());
    } else {
        binaryExp.rexp = nullptr;
    }
    return binaryExp;
}



struct UnaryExp Parser::parseUnaryExp(){
    UnaryExp unaryExp;
    if (getToken().token_type == Token::PLUS || getToken().token_type == Token::MINU) {
        unaryExp.op = getToken().token_type;
        unaryExp.hasOp = true;
        pos++;
    } else {
        unaryExp.hasOp = false;
    }
    if (pos + 1 < tokens.size()
        && tokens[pos].token_type == Token::IDENFR
        && tokens[pos + 1].token_type == Token::LPARENT) {
        struct CallExp callExp = parseCallExp();
        Exp exp = callExp;
        unaryExp.exp = make_shared<Exp>(exp);
    } else if (tokens[pos].token_type == Token::LPARENT) {
        pos++;
        Exp exp = parseExp();
        pos++;
        unaryExp.exp = make_shared<Exp>(exp);
    } else if (tokens[pos].token_type == Token::IDENFR){
        struct Ident ident = parseIdent();
        Exp exp = ident;
        unaryExp.exp = make_shared<Exp>(exp);
    } else {
        struct Number number = parseNumber();
        Exp exp = number;
        unaryExp.exp = make_shared<Exp>(exp);
    }
    return unaryExp;
};

struct CallExp Parser::parseCallExp(){
    CallExp callExp;
    callExp.ident = parseIdent();
    pos++;
    if (getToken().token_type == Token::RPARENT) {
        callExp.funcRParams = nullptr;
    } else {
        callExp.funcRParams = make_shared<FuncRParams>(parseFuncRParams());
    }
    pos++;
    return callExp;
};

struct FuncRParams Parser::parseFuncRParams(){
    struct FuncRParams funcRParams;
    funcRParams.exps.push_back(make_shared<Exp>(parseExp()));
    return funcRParams;
};

struct Number Parser::parseNumber(){
    struct Number number;
    number.value = stof(getToken().content);
    pos++;
    return number;
};
