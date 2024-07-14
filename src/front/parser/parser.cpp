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
        funcDef.funcFParams =
            std::make_shared<FuncFParams>(*parseFuncFParams());
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

std::shared_ptr<Exp> Parser::parseExp() {
    return std::make_shared<Exp>(*parseAddExp());
};

std::shared_ptr<Exp> Parser::parseAddExp() {
    std::shared_ptr<Exp> exp = parseMulExp();
    if (getToken().token_type == Token::PLUS ||
        getToken().token_type == Token::MINU) {
        BinaryExp binaryExp;
        binaryExp.lexp = exp;
        binaryExp.op = getToken().token_type == Token::PLUS ? BinaryExp::PLUS
                                                            : BinaryExp::MINU;
        pos++;
        binaryExp.rexp = parseAddExp();
        return std::make_shared<Exp>(binaryExp);
    }
    return exp;
}

std::shared_ptr<Exp> Parser::parseMulExp() {
    std::shared_ptr<Exp> exp = std::make_shared<Exp>(*parseUnaryExp());
    if (getToken().token_type == Token::MULT ||
        getToken().token_type == Token::DIV ||
        getToken().token_type == Token::MOD) {
        BinaryExp binaryExp;
        binaryExp.op = getToken().token_type == Token::MULT  ? BinaryExp::MULT
                       : getToken().token_type == Token::DIV ? BinaryExp::DIV
                                                             : BinaryExp::MOD;
        pos++;
        binaryExp.lexp = exp;
        binaryExp.rexp = parseMulExp();
        return std::make_shared<Exp>(binaryExp);
    }
    return exp;
}

std::shared_ptr<Exp> Parser::parseUnaryExp() {
    bool hasOp = false;
    bool isPlus;
    if (getToken().token_type == Token::PLUS ||
        getToken().token_type == Token::MINU) {
        hasOp = true;
        isPlus = getToken().token_type == Token::PLUS;
        pos++;
    }
    std::shared_ptr<Exp> exp;
    if (pos + 1 < tokens.size() && tokens[pos].token_type == Token::IDENFR &&
        tokens[pos + 1].token_type == Token::LPARENT) {
        struct CallExp callExp = *parseCallExp();
        exp = std::make_shared<Exp>(callExp);
    } else if (tokens[pos].token_type == Token::LPARENT) {
        pos++;
        exp = parseExp();
        pos++;
    } else if (tokens[pos].token_type == Token::IDENFR) {
        struct Ident ident = *parseIdent();
        exp = std::make_shared<Exp>(ident);
    } else {
        struct Number number = *parseNumber();
        exp = std::make_shared<Exp>(number);
    }
    if (hasOp) {
        UnaryExp unaryExp;
        unaryExp.op = isPlus ? UnaryExp::PLUS : UnaryExp::MINU;
        unaryExp.exp = exp;
        return std::make_shared<Exp>(unaryExp);
    }
    return exp;
};

std::shared_ptr<CallExp> Parser::parseCallExp() {
    CallExp callExp;
    callExp.ident = *parseIdent();
    pos++;
    if (getToken().token_type == Token::RPARENT) {
        callExp.funcRParams = std::make_shared<FuncRParams>(FuncRParams());
    } else {
        callExp.funcRParams =
            std::make_shared<FuncRParams>(*parseFuncRParams());
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
