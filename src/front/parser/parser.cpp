#include <error.h>
#include <front/lexer/token.h>
#include <front/parser/parser.h>
#include <memory>
#include <string>

// TODO: error handling

std::shared_ptr<CompUnit> Parser::parse() {
    _lexer.next(_token);
    _lexer.next(_pre_read);
    auto comp_unit = _parse_comp_unit();
    if (_token.type != Token::TK_EOF) {
        ErrorReporter::error(_token.lineno, "unexpected token");
    }
    return comp_unit;
}

std::shared_ptr<CompUnit> Parser::_parse_comp_unit() {
    auto comp_unit = std::make_shared<CompUnit>();
    comp_unit->lineno = _token.lineno;

    while (_token.type != Token::TK_VAR) {
        if (_token.type == Token::TK_FN) {
            comp_unit->funcDefs.push_back(_parse_func_def());
        } else {
            ErrorReporter::error(_token.lineno, "expect function definition");
            _recover();
        }
    }

    while (_token.type != Token::TK_GET && _token.type != Token::TK_PUT &&
           _token.type != Token::TK_TAG && _token.type != Token::TK_LET &&
           _token.type != Token::TK_IF && _token.type != Token::TK_TO) {
        if (_token.type == Token::TK_VAR) {
            comp_unit->varDecls.push_back(_parse_var_decl());
        } else {
            ErrorReporter::error(_token.lineno, "expect variable declaration");
            _recover();
        }
    }

    while (_token.type != Token::TK_EOF) {
        if (_token.type == Token::TK_GET || _token.type == Token::TK_PUT ||
            _token.type == Token::TK_TAG || _token.type == Token::TK_LET ||
            _token.type == Token::TK_IF || _token.type == Token::TK_TO) {
            comp_unit->stmts.push_back(_parse_stmt());
        } else {
            ErrorReporter::error(_token.lineno, "expect statement");
            _recover();
        }
    }

    return comp_unit;
}

std::shared_ptr<FuncDef> Parser::_parse_func_def() {
    auto func_def = std::make_shared<FuncDef>();
    func_def->lineno = _token.lineno;

    if (_token.type != Token::TK_FN) {
        ErrorReporter::error(_token.lineno, "expect function definition");
    }
    _next_token();

    func_def->ident = _parse_ident();

    if (_token.type != Token::TK_LPARENT) {
        ErrorReporter::error(_token.lineno, "expect '('");
    }
    _next_token();

    if (_token.type != Token::TK_RPARENT) {
        func_def->funcFParams = _parse_func_f_params();
    } else {
        func_def->funcFParams = std::make_shared<FuncFParams>();
        func_def->funcFParams->lineno = _token.lineno;
    }

    if (_token.type != Token::TK_RPARENT) {
        ErrorReporter::error(_token.lineno, "expect ')'");
    }
    _next_token();

    if (_token.type != Token::TK_RARROW) {
        ErrorReporter::error(_token.lineno, "expect '->'");
    }
    _next_token();

    func_def->exp = _parse_exp();

    if (_token.type != Token::TK_SEMINCN) {
        ErrorReporter::error(_token.lineno, "expect ';'");
    }
    _next_token();

    return func_def;
}

std::shared_ptr<FuncFParams> Parser::_parse_func_f_params() {
    auto func_f_params = std::make_shared<FuncFParams>();
    func_f_params->lineno = _token.lineno;

    func_f_params->idents.push_back(_parse_ident());
    while (_token.type == Token::TK_COMMA) {
        _next_token();
        func_f_params->idents.push_back(_parse_ident());
    }

    return func_f_params;
}

std::shared_ptr<VarDecl> Parser::_parse_var_decl() {
    auto var_decl = std::make_shared<VarDecl>();
    var_decl->lineno = _token.lineno;

    if (_token.type != Token::TK_VAR) {
        ErrorReporter::error(_token.lineno, "expect variable declaration");
    }
    _next_token();

    var_decl->ident = _parse_ident();

    if (_token.type != Token::TK_SEMINCN) {
        ErrorReporter::error(_token.lineno, "expect ';'");
    }
    _next_token();

    return var_decl;
}

std::shared_ptr<Stmt> Parser::_parse_stmt() {
    switch (_token.type) {
    case Token::TK_GET: {
        GetStmt get_stmt;
        get_stmt.lineno = _token.lineno;

        _next_token();

        get_stmt.ident = _parse_ident();

        if (_token.type != Token::TK_SEMINCN) {
            ErrorReporter::error(_token.lineno, "expect ';'");
        }
        _next_token();

        return std::make_shared<Stmt>(get_stmt);
    } break;
    case Token::TK_PUT: {
        PutStmt put_stmt;
        put_stmt.lineno = _token.lineno;

        _next_token();

        put_stmt.exp = _parse_exp();

        if (_token.type != Token::TK_SEMINCN) {
            ErrorReporter::error(_token.lineno, "expect ';'");
        }
        _next_token();

        return std::make_shared<Stmt>(put_stmt);
    } break;
    case Token::TK_TAG: {
        TagStmt tag_stmt;
        tag_stmt.lineno = _token.lineno;

        _next_token();

        tag_stmt.ident = _parse_ident();

        if (_token.type != Token::TK_SEMINCN) {
            ErrorReporter::error(_token.lineno, "expect ';'");
        }
        _next_token();

        return std::make_shared<Stmt>(tag_stmt);
    } break;
    case Token::TK_LET: {
        LetStmt let_stmt;
        let_stmt.lineno = _token.lineno;

        _next_token();

        let_stmt.ident = _parse_ident();

        if (_token.type != Token::TK_ASSIGN) {
            ErrorReporter::error(_token.lineno, "expect '='");
        }
        _next_token();

        let_stmt.exp = _parse_exp();

        if (_token.type != Token::TK_SEMINCN) {
            ErrorReporter::error(_token.lineno, "expect ';'");
        }
        _next_token();

        return std::make_shared<Stmt>(let_stmt);
    } break;
    case Token::TK_IF: {
        IfStmt if_stmt;
        if_stmt.lineno = _token.lineno;

        _next_token();

        if_stmt.cond = _parse_cond();

        if (_token.type != Token::TK_TO) {
            ErrorReporter::error(_token.lineno, "expect 'to'");
        }
        _next_token();

        if_stmt.ident = _parse_ident();

        if (_token.type != Token::TK_SEMINCN) {
            ErrorReporter::error(_token.lineno, "expect ';'");
        }
        _next_token();

        return std::make_shared<Stmt>(if_stmt);
    } break;
    case Token::TK_TO: {
        ToStmt to_stmt;
        to_stmt.lineno = _token.lineno;

        _next_token();

        to_stmt.ident = _parse_ident();

        if (_token.type != Token::TK_SEMINCN) {
            ErrorReporter::error(_token.lineno, "expect ';'");
        }
        _next_token();

        return std::make_shared<Stmt>(to_stmt);
    } break;
    default:
        ErrorReporter::error(_token.lineno, "expect statement");
        _recover();
        return nullptr;
    }
}

std::shared_ptr<Exp> Parser::_parse_exp() { return _parse_add_exp(); }

std::shared_ptr<Exp> Parser::_parse_add_exp() {
    auto lineno = _token.lineno;

    auto exp = _parse_mul_exp();
    while (_token.type == Token::TK_PLUS || _token.type == Token::TK_MINU) {
        BinaryExp binary_exp;
        binary_exp.lineno = lineno;
        binary_exp.lhs = exp;
        binary_exp.op =
            _token.type == Token::TK_PLUS ? BinaryExp::PLUS : BinaryExp::MINU;

        _next_token();
        auto rhs = _parse_mul_exp();
        binary_exp.rhs = rhs;
        exp = std::make_shared<Exp>(binary_exp);
    }
    return exp;
}

std::shared_ptr<Exp> Parser::_parse_mul_exp() {
    auto lineno = _token.lineno;

    auto exp = _parse_unary_exp();
    while (_token.type == Token::TK_MULT || _token.type == Token::TK_DIV ||
           _token.type == Token::TK_MOD) {
        BinaryExp binary_exp;
        binary_exp.lineno = lineno;
        binary_exp.lhs = exp;
        switch (_token.type) {
        case Token::TK_MULT:
            binary_exp.op = BinaryExp::MULT;
            break;
        case Token::TK_DIV:
            binary_exp.op = BinaryExp::DIV;
            break;
        case Token::TK_MOD:
            binary_exp.op = BinaryExp::MOD;
            break;
        }

        _next_token();
        binary_exp.rhs = _parse_unary_exp();
        exp = std::make_shared<Exp>(binary_exp);
    }
    return exp;
}

std::shared_ptr<Exp> Parser::_parse_unary_exp() {
    if (_token.type == Token::TK_IDENT && _pre_read.type == Token::TK_LPARENT) {
        CallExp call_exp;
        call_exp.lineno = _token.lineno;

        call_exp.ident = _parse_ident();

        if (_token.type != Token::TK_LPARENT) {
            ErrorReporter::error(_token.lineno, "expect '('");
        }
        _next_token();

        if (_token.type != Token::TK_RPARENT) {
            call_exp.funcRParams = _parse_func_r_params();
        } else {
            call_exp.funcRParams = std::make_shared<FuncRParams>();
            call_exp.funcRParams->lineno = _token.lineno;
        }

        if (_token.type != Token::TK_RPARENT) {
            ErrorReporter::error(_token.lineno, "expect ')'");
        }
        _next_token();

        return std::make_shared<Exp>(call_exp);
    } else if (_token.type == Token::TK_IDENT ||
               _token.type == Token::TK_NUMBER ||
               _token.type == Token::TK_LPARENT) {
        return _parse_primary_exp();
    } else if (_token.type == Token::TK_PLUS || _token.type == Token::TK_MINU) {
        UnaryExp unary_exp;
        unary_exp.lineno = _token.lineno;
        unary_exp.op =
            _token.type == Token::TK_PLUS ? UnaryExp::PLUS : UnaryExp::MINU;

        _next_token();
        unary_exp.exp = _parse_unary_exp();
        return std::make_shared<Exp>(unary_exp);
    } else {
        ErrorReporter::error(_token.lineno, "expect unary expression");
        _recover();
        return nullptr;
    }
}

std::shared_ptr<Exp> Parser::_parse_primary_exp() {
    if (_token.type == Token::TK_LPARENT) {
        _next_token();
        auto exp = _parse_exp();
        if (_token.type != Token::TK_RPARENT) {
            ErrorReporter::error(_token.lineno, "expect ')'");
        }
        _next_token();
        return exp;
    } else if (_token.type == Token::TK_IDENT) {
        IdentExp lval_exp;
        lval_exp.lineno = _token.lineno;
        lval_exp.ident = _parse_ident();
        return std::make_shared<Exp>(lval_exp);
    } else if (_token.type == Token::TK_NUMBER) {
        return _parse_number();
    } else {
        ErrorReporter::error(_token.lineno, "expect primary expression");
        _recover();
        return  nullptr;
    }
}

std::shared_ptr<FuncRParams> Parser::_parse_func_r_params() {
    auto func_r_params = std::make_shared<FuncRParams>();
    func_r_params->lineno = _token.lineno;

    func_r_params->exps.push_back(_parse_exp());
    while (_token.type == Token::TK_COMMA) {
        _next_token();
        func_r_params->exps.push_back(_parse_exp());
    }
    return func_r_params;
}

std::shared_ptr<Cond> Parser::_parse_cond() {
    auto cond = std::make_shared<Cond>();
    cond->lineno = _token.lineno;

    cond->lhs = _parse_exp();
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
    _next_token();
    cond->rhs = _parse_exp();
    return cond;
}

std::shared_ptr<Ident> Parser::_parse_ident() {
    if (_token.type == Token::TK_IDENT) {
        auto ident = std::make_shared<Ident>();
        ident->lineno = _token.lineno;
        ident->value = _token.content;
        _next_token();
        return ident;
    } else {
        ErrorReporter::error(_token.lineno, "expect identifier");
        _recover();
        return nullptr;
    }
}

std::shared_ptr<Exp> Parser::_parse_number() {
    if (_token.type == Token::TK_NUMBER) {
        Number number;
        number.lineno = _token.lineno;
        // number is float, so use std::stof
        number.value = std::stof(_token.content);
        _next_token();
        return std::make_shared<Exp>(number);
    } else {
        ErrorReporter::error(_token.lineno, "expect number");
        _recover();
        return nullptr;
    }
}
