#include <error.h>
#include <front/lexer/token.h>
#include <front/parser/parser.h>
#include <memory>
#include <string>

Token Parser::getToken() { return tokens[pos]; }

bool Parser::hasNext() { return pos < tokens.size(); }
std::shared_ptr<CompUnit> Parser::parseCompUnit() {
    struct CompUnit compUnit;
    while (hasNext() && getToken().token_type == Token::FN) {
        compUnit.funcDefs.push_back(std::make_shared<FuncDef>(*parseFuncDef()));
    }
    while (hasNext() && getToken().token_type == Token::VARTK) {
        compUnit.varDecls.push_back(std::make_shared<VarDecl>(*parseVarDecl()));
    }
    while (hasNext()) {
        compUnit.stmts.push_back(std::make_shared<Stmt>(*parseStmt()));
    }
    return std::make_shared<CompUnit>(compUnit);
}

std::shared_ptr<Ident> Parser::parseIdent() {
    struct Ident ident;
    ident.value = getToken().content;
    pos++;
    return std::make_shared<Ident>(ident);
}

std::shared_ptr<FuncFParams> Parser::parseFuncFParams() {
    struct FuncFParams funcFParams;
    funcFParams.idents.push_back(*parseIdent());
    while (getToken().token_type == Token::COMMA) {
        pos++;
        funcFParams.idents.push_back(*parseIdent());
    }
    return std::make_shared<FuncFParams>(funcFParams);
};

std::shared_ptr<FuncDef> Parser::parseFuncDef() {
    struct FuncDef funcDef;
    pos++;
    funcDef.ident = *parseIdent();
    pos++;
    if (getToken().token_type == Token::RPARENT) {
        funcDef.funcFParams = std::make_shared<FuncFParams>(FuncFParams());
    } else {
        funcDef.funcFParams = std::make_shared<FuncFParams>(*parseFuncFParams());
    }
    pos++;
    pos++;
    funcDef.exp = std::make_shared<Exp>(*parseExp());
    pos++;
    return std::make_shared<FuncDef>(funcDef);
};

std::shared_ptr<VarDecl> Parser::parseVarDecl() {
    struct VarDecl varDecl;
    pos++;
    varDecl.ident = *parseIdent();
    pos++;
    return std::make_shared<VarDecl>(varDecl);
};

std::shared_ptr<Stmt> Parser::parseStmt() {
    Stmt stmt;
    auto token = getToken();
    switch (token.token_type) {
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
    default:
        error(token.line, "unexpected token");
    }
    return std::make_shared<Stmt>(stmt);
}
std::shared_ptr<GetStmt> Parser::parseGetStmt() {
    struct GetStmt getStmt;
    pos++;
    getStmt.ident = *parseIdent();
    pos++;
    return std::make_shared<GetStmt>(getStmt);
};

std::shared_ptr<PutStmt> Parser::parsePutStmt() {
    struct PutStmt putStmt;
    pos++;
    putStmt.exp = std::make_shared<Exp>(*parseExp());
    pos++;
    return std::make_shared<PutStmt>(putStmt);
};

std::shared_ptr<TagStmt> Parser::parseTagStmt() {
    struct TagStmt tagStmt;
    pos++;
    tagStmt.ident = *parseIdent();
    pos++;
    return std::make_shared<TagStmt>(tagStmt);
};

std::shared_ptr<LetStmt> Parser::parseLetStmt() {
    struct LetStmt letStmt;
    pos++;
    letStmt.ident = *parseIdent();
    pos++;
    letStmt.exp = std::make_shared<Exp>(*parseExp());
    pos++;
    return std::make_shared<LetStmt>(letStmt);
};

std::shared_ptr<IfStmt> Parser::parseIfStmt() {
    struct IfStmt ifStmt;
    pos++;
    ifStmt.cond = std::make_shared<Cond>(*parseCond());
    pos++;
    ifStmt.ident = *parseIdent();
    pos++;
    return std::make_shared<IfStmt>(ifStmt);
};

std::shared_ptr<ToStmt> Parser::parseToStmt() {
    struct ToStmt toStmt;
    pos++;
    toStmt.ident = *parseIdent();
    pos++;
    return std::make_shared<ToStmt>(toStmt);
};

std::shared_ptr<Cond> Parser::parseCond() {
    struct Cond cond;
    cond.left = std::make_shared<Exp>(*parseExp());
    Token::TokenType t = getToken().token_type;
    cond.op = t == Token::GRE   ? Cond::GRE
              : t == Token::LSS ? Cond::LSS
              : t == Token::GEQ ? Cond::GEQ
              : t == Token::LEQ ? Cond::LEQ
              : t == Token::EQL ? Cond::EQL
                                : Cond::NEQ;
    pos++;
    cond.right = std::make_shared<Exp>(*parseExp());
    return std::make_shared<Cond>(cond);
};

std::shared_ptr<Exp> Parser::parseExp() { return std::make_shared<Exp>(*parseAddExp()); };

std::shared_ptr<BinaryExp> Parser::parseAddExp() {
    BinaryExp binaryExp;
    binaryExp.op = BinaryExp::PLUS;
    binaryExp.lexp = std::make_shared<Exp>(*parseMulExp());
    if (getToken().token_type == Token::PLUS ||
        getToken().token_type == Token::MINU) {
        binaryExp.op = getToken().token_type == Token::PLUS ? BinaryExp::PLUS
                                                            : BinaryExp::MINU;
        pos++;
        binaryExp.rexp = std::make_shared<Exp>(*parseAddExp());
    } else {
        binaryExp.rexp = nullptr;
    }
    return std::make_shared<BinaryExp>(binaryExp);
}

std::shared_ptr<BinaryExp> Parser::parseMulExp() {
    BinaryExp binaryExp;
    binaryExp.op = BinaryExp::MULT;
    binaryExp.lexp = std::make_shared<Exp>(*parseUnaryExp());
    if (getToken().token_type == Token::MULT ||
        getToken().token_type == Token::DIV ||
        getToken().token_type == Token::MOD) {
        binaryExp.op = getToken().token_type == Token::MULT  ? BinaryExp::MULT
                       : getToken().token_type == Token::DIV ? BinaryExp::DIV
                                                             : BinaryExp::MOD;
        pos++;
        binaryExp.rexp = std::make_shared<Exp>(*parseMulExp());
    } else {
        binaryExp.rexp = nullptr;
    }
    return std::make_shared<BinaryExp>(binaryExp);
}

std::shared_ptr<UnaryExp> Parser::parseUnaryExp() {
    UnaryExp unaryExp;
    if (getToken().token_type == Token::PLUS ||
        getToken().token_type == Token::MINU) {
        unaryExp.op = getToken().token_type == Token::PLUS ? UnaryExp::PLUS
                                                           : UnaryExp::MINU;
        unaryExp.hasOp = true;
        pos++;
    } else {
        unaryExp.hasOp = false;
    }
    if (pos + 1 < tokens.size() && tokens[pos].token_type == Token::IDENFR &&
        tokens[pos + 1].token_type == Token::LPARENT) {
        struct CallExp callExp = *parseCallExp();
        Exp exp = callExp;
        unaryExp.exp = std::make_shared<Exp>(exp);
    } else if (tokens[pos].token_type == Token::LPARENT) {
        pos++;
        Exp exp = *parseExp();
        pos++;
        unaryExp.exp = std::make_shared<Exp>(exp);
    } else if (tokens[pos].token_type == Token::IDENFR) {
        struct Ident ident = *parseIdent();
        Exp exp = ident;
        unaryExp.exp = std::make_shared<Exp>(exp);
    } else {
        struct Number number = *parseNumber();
        Exp exp = number;
        unaryExp.exp = std::make_shared<Exp>(exp);
    }
    return std::make_shared<UnaryExp>(unaryExp);
};

std::shared_ptr<CallExp> Parser::parseCallExp() {
    CallExp callExp;
    callExp.ident = *parseIdent();
    pos++;
    if (getToken().token_type == Token::RPARENT) {
        callExp.funcRParams = std::make_shared<FuncRParams>(FuncRParams());
    } else {
        callExp.funcRParams = std::make_shared<FuncRParams>(*parseFuncRParams());
    }
    pos++;
    return std::make_shared<CallExp>(callExp);
};

std::shared_ptr<FuncRParams> Parser::parseFuncRParams() {
    struct FuncRParams funcRParams;
    funcRParams.exps.push_back(std::make_shared<Exp>(*parseExp()));
    while (getToken().token_type == Token::COMMA) {
        pos++;
        funcRParams.exps.push_back(std::make_shared<Exp>(*parseExp()));
    }
    return std::make_shared<FuncRParams>(funcRParams);
};

std::shared_ptr<Number> Parser::parseNumber() {
    struct Number number;
    number.value = stof(getToken().content);
    pos++;
    return std::make_shared<Number>(number);
};
