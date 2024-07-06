#include <error.h>
#include <front/lexer/token.h>
#include <front/parser/parser.h>
#include <memory>
#include <string>

std::shared_ptr<CompUnit> Parser::parse() {
    _lexer.next(_token);
    auto comp_unit = _parse_comp_unit();
    if (_token.type != Token::TK_EOF) {
        error(_token.lineno, "expect EOF");
    }
    return comp_unit;
}

std::shared_ptr<CompUnit> Parser::_parse_comp_unit() {
    auto comp_unit = std::make_shared<CompUnit>();

    while (_token.type == Token::TK_FN) {
        comp_unit->funcDefs.push_back(_parse_func_def());
    }

    while (_token.type == Token::TK_VAR) {
        comp_unit->varDecls.push_back(_parse_var_decl());
    }

    while (_token.type == Token::TK_GET || _token.type == Token::TK_PUT ||
           _token.type == Token::TK_TAG || _token.type == Token::TK_LET ||
           _token.type == Token::TK_IF || _token.type == Token::TK_TO) {
        comp_unit->stmts.push_back(_parse_stmt());
    }

    return comp_unit;
}

std::shared_ptr<FuncDef> Parser::_parse_func_def() {
    auto func_def = std::make_shared<FuncDef>();

    if (_token.type != Token::TK_FN) {
        error(_token.lineno, "expect function definition");
    }
    _lexer.next(_token);

    func_def->ident = *_parse_ident();

    if (_token.type != Token::TK_LPARENT) {
        error(_token.lineno, "expect '('");
    }
    _lexer.next(_token);

    if (_token.type != Token::TK_RPARENT) {
        func_def->funcFParams = _parse_func_f_params();
    } else {
        func_def->funcFParams = std::make_shared<FuncFParams>();
    }

    if (_token.type != Token::TK_RPARENT) {
        error(_token.lineno, "expect ')'");
    }
    _lexer.next(_token);

    if (_token.type != Token::TK_RARROW) {
        error(_token.lineno, "expect '->'");
    }
    _lexer.next(_token);

    func_def->exp = _parse_exp();

    if (_token.type != Token::TK_SEMINCN) {
        error(_token.lineno, "expect ';'");
    }
    _lexer.next(_token);

    return func_def;
}

std::shared_ptr<FuncFParams> Parser::_parse_func_f_params() {
    auto func_f_params = std::make_shared<FuncFParams>();

    func_f_params->idents.push_back(
        *_parse_ident()); // TODO: use ident ptr instead of ident
    while (_token.type == Token::TK_COMMA) {
        _lexer.next(_token);
        func_f_params->idents.push_back(*_parse_ident());
    }

    return func_f_params;
}

std::shared_ptr<VarDecl> Parser::_parse_var_decl() {
    auto var_decl = std::make_shared<VarDecl>();

    if (_token.type != Token::TK_VAR) {
        error(_token.lineno, "expect variable declaration");
    }
    _lexer.next(_token);

    var_decl->ident = *_parse_ident();

    if (_token.type != Token::TK_SEMINCN) {
        error(_token.lineno, "expect ';'");
    }
    _lexer.next(_token);

    return var_decl;
}

std::shared_ptr<Stmt> Parser::_parse_stmt() {
    switch (_token.type) {
    case Token::TK_GET: {
        _lexer.next(_token);

        GetStmt get_stmt;

        get_stmt.ident = *_parse_ident();

        if (_token.type != Token::TK_SEMINCN) {
            error(_token.lineno, "expect ';'");
        }
        _lexer.next(_token);

        return std::make_shared<Stmt>(get_stmt);
    } break;
    case Token::TK_PUT: {
        _lexer.next(_token);

        PutStmt put_stmt;

        put_stmt.exp = _parse_exp();

        if (_token.type != Token::TK_SEMINCN) {
            error(_token.lineno, "expect ';'");
        }
        _lexer.next(_token);

        return std::make_shared<Stmt>(put_stmt);
    } break;
    case Token::TK_TAG: {
        _lexer.next(_token);

        TagStmt tag_stmt;
        tag_stmt.ident = *_parse_ident();

        if (_token.type != Token::TK_SEMINCN) {
            error(_token.lineno, "expect ';'");
        }
        _lexer.next(_token);

        return std::make_shared<Stmt>(tag_stmt);
    } break;
    case Token::TK_LET: {
        _lexer.next(_token);

        LetStmt let_stmt;
        let_stmt.ident = *_parse_ident();

        if (_token.type != Token::TK_ASSIGN) {
            error(_token.lineno, "expect '='");
        }
        _lexer.next(_token);

        let_stmt.exp = _parse_exp();

        if (_token.type != Token::TK_SEMINCN) {
            error(_token.lineno, "expect ';'");
        }
        _lexer.next(_token);

        return std::make_shared<Stmt>(let_stmt);
    } break;
    case Token::TK_IF: {
        _lexer.next(_token);

        IfStmt if_stmt;

        if_stmt.cond = _parse_cond();

        if (_token.type != Token::TK_TO) {
            error(_token.lineno, "expect 'to'");
        }
        _lexer.next(_token);

        if_stmt.ident = *_parse_ident();

        if (_token.type != Token::TK_SEMINCN) {
            error(_token.lineno, "expect ';'");
        }
        _lexer.next(_token);

        return std::make_shared<Stmt>(if_stmt);
    } break;
    case Token::TK_TO: {
        _lexer.next(_token);

        ToStmt to_stmt;

        to_stmt.ident = *_parse_ident();

        if (_token.type != Token::TK_SEMINCN) {
            error(_token.lineno, "expect ';'");
        }
        _lexer.next(_token);

        return std::make_shared<Stmt>(to_stmt);
    } break;
    default:
        error(_token.lineno, "expect statement");
        break;
    }
}

std::shared_ptr<Exp> Parser::_parse_exp() { return _parse_add_exp(); }

std::shared_ptr<Exp> Parser::_parse_add_exp() {
    auto exp = _parse_mul_exp();
    while (_token.type == Token::TK_PLUS || _token.type == Token::TK_MINU) {
        auto op =
            _token.type == Token::TK_PLUS ? BinaryExp::PLUS : BinaryExp::MINU;
        _lexer.next(_token);
        auto rhs = _parse_mul_exp();
        exp = std::make_shared<Exp>(BinaryExp(op, exp, rhs));
    }
    return exp;
}

std::shared_ptr<Exp> Parser::_parse_mul_exp() {
    auto exp = _parse_unary_exp();
    while (_token.type == Token::TK_MULT || _token.type == Token::TK_DIV ||
           _token.type == Token::TK_MOD) {
        BinaryExp::BinaryOp op;
        switch (_token.type) {
        case Token::TK_MULT:
            op = BinaryExp::MULT;
            break;
        case Token::TK_DIV:
            op = BinaryExp::DIV;
            break;
        case Token::TK_MOD:
            op = BinaryExp::MOD;
            break;
        }
        _lexer.next(_token);
        auto rhs = _parse_unary_exp();
        exp = std::make_shared<Exp>(BinaryExp(op, exp, rhs));
    }
    return exp;
}

std::shared_ptr<Exp> Parser::_parse_unary_exp() {
    if (_token.type == Token::TK_IDENT || _token.type == Token::TK_NUMBER ||
        _token.type == Token::TK_LPARENT) {
        return _parse_primary_exp();
    } else if (_token.type == Token::TK_PLUS || _token.type == Token::TK_MINU) {
        UnaryExp unary_exp;
        unary_exp.op =
            _token.type == Token::TK_PLUS ? UnaryExp::PLUS : UnaryExp::MINU;
        _lexer.next(_token);
        unary_exp.exp = _parse_unary_exp();
        return std::make_shared<Exp>(unary_exp);
    } else { // TODO: function call, need pre read
        error(_token.lineno, "expect unary expression");
    }
}

std::shared_ptr<Exp> Parser::_parse_primary_exp() {
    if (_token.type == Token::TK_LPARENT) {
        _lexer.next(_token);
        auto exp = _parse_exp();
        if (_token.type != Token::TK_RPARENT) {
            error(_token.lineno, "expect ')'");
        }
        _lexer.next(_token);
        return exp;
    } else if (_token.type == Token::TK_IDENT) {
        return std::make_shared<Exp>(*_parse_ident()); // TODO: need ident exp
    } else if (_token.type == Token::TK_NUMBER) {
        return _parse_number();
    } else {
        error(_token.lineno, "expect primary expression");
    }
}

std::shared_ptr<FuncRParams> Parser::_parse_func_r_params() {
    auto func_r_params = std::make_shared<FuncRParams>();
    func_r_params->exps.push_back(_parse_exp());
    while (_token.type == Token::TK_COMMA) {
        _lexer.next(_token);
        func_r_params->exps.push_back(_parse_exp());
    }
    return func_r_params;
}

std::shared_ptr<Cond> Parser::_parse_cond() {
    auto cond = std::make_shared<Cond>();
    cond->left = _parse_exp();
    switch (_token.type) {
    case Token::TK_LT:
        cond->op = Cond::LT;
        break;
    case Token::TK_GT:
        cond->op = Cond::GT;
        break;
    case Token::TK_LE:
        cond->op = Cond::LE;
        break;
    case Token::TK_GE:
        cond->op = Cond::GE;
        break;
    case Token::TK_EQ:
        cond->op = Cond::EQ;
        break;
    case Token::TK_NE:
        cond->op = Cond::NE;
        break;
    }
    _lexer.next(_token);
    cond->right = _parse_exp();
    return cond;
}

std::shared_ptr<Ident> Parser::_parse_ident() {
    if (_token.type == Token::TK_IDENT) {
        auto ident = std::make_shared<Ident>();
        ident->value = _token.content;
        _lexer.next(_token);
        return ident;
    } else {
        error(_token.lineno, "expect identifier");
    }
}

std::shared_ptr<Exp> Parser::_parse_number() {
    if (_token.type == Token::TK_NUMBER) {
        Number number;
        // number is float, so use std::stof
        number.value = std::stof(_token.content);
        _lexer.next(_token);
        return std::make_shared<Exp>(number);
    } else {
        error(_token.lineno, "expect number");
    }
}
