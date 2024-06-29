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
shared_ptr<CompUnit> Parser::parseCompUnit() {
    struct CompUnit compUnit;
    while (hasNext() && getToken().token_type == Token::FN) {
        compUnit.funcDefs.push_back(make_shared<FuncDef>(*parseFuncDef()));
    }
    while (hasNext() && getToken().token_type == Token::VARTK) {
        compUnit.varDecls.push_back(make_shared<VarDecl>(*parseVarDecl()));
    }
    while (hasNext()) {
        compUnit.stmts.push_back(make_shared<Stmt>(*parseStmt()));
    }
    return make_shared<CompUnit>(compUnit);
}

shared_ptr<Ident> Parser::parseIdent() {
    struct Ident ident;
    ident.ident = getToken().content;
    pos++;
    return make_shared<Ident>(ident);
}

shared_ptr<FuncFParams> Parser::parseFuncFParams() {
    struct FuncFParams funcFParams;
    funcFParams.idents.push_back(*parseIdent());
    while (getToken().token_type == Token::COMMA) {
        pos++;
        funcFParams.idents.push_back(*parseIdent());
    }
    return make_shared<FuncFParams>(funcFParams);
};

shared_ptr<FuncDef> Parser::parseFuncDef() {
    struct FuncDef funcDef;
    pos++;
    funcDef.ident = *parseIdent();
    pos++;
    if (getToken().token_type == Token::RPARENT) {
        funcDef.funcFParams = make_shared<FuncFParams>(FuncFParams());
    } else {
        funcDef.funcFParams = make_shared<FuncFParams>(*parseFuncFParams());
    }
    pos++;
    pos++;
    funcDef.exp = make_shared<Exp>(*parseExp());
    pos++;
    return make_shared<FuncDef>(funcDef);
};

shared_ptr<VarDecl> Parser::parseVarDecl() {
    struct VarDecl varDecl;
    pos++;
    varDecl.ident = *parseIdent();
    pos++;
    return make_shared<VarDecl>(varDecl);
};

shared_ptr<Stmt> Parser::parseStmt() {
    Stmt stmt;
    switch (getToken().token_type) {
    case Token::GETTK: {
        stmt = *parseGetStmt();
        break;
    }
    case Token::PUTTK: {
        stmt = *parsePutStmt();
        break;
    }
    case Token::TAGTK: {
        stmt = *parseTagStmt();
        break;
    }
    case Token::LETTK: {
        stmt = *parseLetStmt();
        break;
    }
    case Token::IFTK: {
        stmt = *parseIfStmt();
        break;
    }
    case Token::TOTK: {
        stmt = *parseToStmt();
        break;
    }
    }
    return make_shared<Stmt>(stmt);
}
shared_ptr<GetStmt> Parser::parseGetStmt(){
    struct GetStmt getStmt;
    pos++;
    getStmt.ident = *parseIdent();
    pos++;
    return make_shared<GetStmt>(getStmt);
};

shared_ptr<PutStmt> Parser::parsePutStmt(){
    struct PutStmt putStmt;
    pos++;
    putStmt.exp = make_shared<Exp>(*parseExp());
    pos++;
    return make_shared<PutStmt>(putStmt);
};

shared_ptr<TagStmt> Parser::parseTagStmt(){
    struct TagStmt tagStmt;
    pos++;
    tagStmt.ident = *parseIdent();
    pos++;
    return make_shared<TagStmt>(tagStmt);
};

shared_ptr<LetStmt> Parser::parseLetStmt(){
    struct LetStmt letStmt;
    pos++;
    letStmt.ident = *parseIdent();
    pos++;
    letStmt.exp = make_shared<Exp>(*parseExp());
    pos++;
    return make_shared<LetStmt>(letStmt);
};


shared_ptr<IfStmt> Parser::parseIfStmt(){
    struct IfStmt ifStmt;
    pos++;
    ifStmt.cond = make_shared<Cond>(*parseCond());
    pos++;
    ifStmt.ident = *parseIdent();
    pos++;
    return make_shared<IfStmt>(ifStmt);
};

shared_ptr<ToStmt> Parser::parseToStmt(){
    struct ToStmt toStmt;
    pos++;
    toStmt.ident = *parseIdent();
    pos++;
    return make_shared<ToStmt>(toStmt);
};

shared_ptr<Cond> Parser::parseCond(){
    struct Cond cond;
    cond.left = make_shared<Exp>(*parseExp());
    Token::TokenType t = getToken().token_type;
    cond.op = t == Token::GRE ? Cond::GRE :
              t == Token::LSS ? Cond::LSS:
              t == Token::GEQ ? Cond::GEQ:
              t == Token::LEQ ? Cond::LEQ:
              t == Token::EQL ? Cond::EQL:
                              Cond::NEQ;
    pos++;
    cond.right = make_shared<Exp>(*parseExp());
    return make_shared<Cond>(cond);
};

shared_ptr<Exp> Parser::parseExp() {
    return make_shared<Exp>(*parseAddExp());
};

shared_ptr<BinaryExp> Parser::parseAddExp() {
    BinaryExp binaryExp;
    binaryExp.op = BinaryExp::PLUS;
    binaryExp.lexp = make_shared<Exp>(*parseMulExp());
    if (getToken().token_type == Token::PLUS || getToken().token_type == Token::MINU) {
        binaryExp.op = getToken().token_type == Token::PLUS ? BinaryExp::PLUS : BinaryExp::MINU;
        pos++;
        binaryExp.rexp = make_shared<Exp>(*parseAddExp());
    } else {
        binaryExp.rexp = nullptr;
    }
    return make_shared<BinaryExp>(binaryExp);
}

shared_ptr<BinaryExp> Parser::parseMulExp() {
    BinaryExp binaryExp;
    binaryExp.op = BinaryExp::MULT;
    binaryExp.lexp = make_shared<Exp>(*parseUnaryExp());
    if (getToken().token_type == Token::MULT
        || getToken().token_type == Token::DIV
        || getToken().token_type == Token::MOD) {
        binaryExp.op = getToken().token_type == Token::MULT ? BinaryExp::MULT :
                       getToken().token_type == Token::DIV ? BinaryExp::DIV:
                                                           BinaryExp::MOD;
        pos++;
        binaryExp.rexp = make_shared<Exp>(*parseMulExp());
    } else {
        binaryExp.rexp = nullptr;
    }
    return make_shared<BinaryExp>(binaryExp);
}



shared_ptr<UnaryExp> Parser::parseUnaryExp(){
    UnaryExp unaryExp;
    if (getToken().token_type == Token::PLUS || getToken().token_type == Token::MINU) {
        unaryExp.op = getToken().token_type == Token::PLUS ? UnaryExp::PLUS : UnaryExp::MINU;
        unaryExp.hasOp = true;
        pos++;
    } else {
        unaryExp.hasOp = false;
    }
    if (pos + 1 < tokens.size()
        && tokens[pos].token_type == Token::IDENFR
        && tokens[pos + 1].token_type == Token::LPARENT) {
        struct CallExp callExp = *parseCallExp();
        Exp exp = callExp;
        unaryExp.exp = make_shared<Exp>(exp);
    } else if (tokens[pos].token_type == Token::LPARENT) {
        pos++;
        Exp exp = *parseExp();
        pos++;
        unaryExp.exp = make_shared<Exp>(exp);
    } else if (tokens[pos].token_type == Token::IDENFR){
        struct Ident ident = *parseIdent();
        Exp exp = ident;
        unaryExp.exp = make_shared<Exp>(exp);
    } else {
        struct Number number = *parseNumber();
        Exp exp = number;
        unaryExp.exp = make_shared<Exp>(exp);
    }
    return make_shared<UnaryExp>(unaryExp);
};

shared_ptr<CallExp> Parser::parseCallExp(){
    CallExp callExp;
    callExp.ident = *parseIdent();
    pos++;
    if (getToken().token_type == Token::RPARENT) {
        callExp.funcRParams = make_shared<FuncRParams>(FuncRParams());
    } else {
        callExp.funcRParams = make_shared<FuncRParams>(*parseFuncRParams());
    }
    pos++;
    return make_shared<CallExp>(callExp);
};

shared_ptr<FuncRParams> Parser::parseFuncRParams(){
    struct FuncRParams funcRParams;
    funcRParams.exps.push_back(make_shared<Exp>(*parseExp()));
    while (getToken().token_type == Token::COMMA) {
        pos++;
        funcRParams.exps.push_back(make_shared<Exp>(*parseExp()));
    }
    return make_shared<FuncRParams>(funcRParams);
};

shared_ptr<Number> Parser::parseNumber(){
    struct Number number;
    number.value = stof(getToken().content);
    pos++;
    return make_shared<Number>(number);
};
