#include "tolang/parser.h"
#include "tolang/error.h"
#include "tolang/token.h"
#include <memory>
#include <string>

std::unique_ptr<CompUnit> Parser::parse() {
    _lexer.next(_token);
    _lexer.next(_pre_read);
    auto comp_unit = _parse_comp_unit();
    if (_token.type != Token::TK_EOF) {
        ErrorReporter::error(_token.lineno, "expect end of file");
    }
    return comp_unit;
}

std::unique_ptr<CompUnit> Parser::_parse_comp_unit() {
    auto comp_unit = std::make_unique<CompUnit>();
    comp_unit->lineno = _token.lineno;

    while (_token.type != Token::TK_VAR && _token.type != Token::TK_GET &&
           _token.type != Token::TK_PUT && _token.type != Token::TK_TAG &&
           _token.type != Token::TK_LET && _token.type != Token::TK_IF &&
           _token.type != Token::TK_TO && _token.type != Token::TK_EOF) {
        if (_token.type == Token::TK_FN) {
            comp_unit->func_defs.push_back(_parse_func_def());
        } else {
            ErrorReporter::error(_token.lineno, "expect function definition");
            _recover();
        }
    }

    while (_token.type != Token::TK_GET && _token.type != Token::TK_PUT &&
           _token.type != Token::TK_TAG && _token.type != Token::TK_LET &&
           _token.type != Token::TK_IF && _token.type != Token::TK_TO &&
           _token.type != Token::TK_EOF) {
        if (_token.type == Token::TK_VAR) {
            comp_unit->var_decls.push_back(_parse_var_decl());
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

std::unique_ptr<FuncDef> Parser::_parse_func_def() {
    auto func_def = std::make_unique<FuncDef>();
    func_def->lineno = _token.lineno;

    _match(_token, Token::TK_FN);

    func_def->ident = _parse_ident();

    _match(_token, Token::TK_LPARENT);

    if (_token.type != Token::TK_RPARENT) {
        _parse_func_f_params(func_def->func_f_params);
    }

    _match(_token, Token::TK_RPARENT);

    _match(_token, Token::TK_RARROW);

    func_def->exp = _parse_exp();

    _match(_token, Token::TK_SEMINCN);

    return func_def;
}

void Parser::_parse_func_f_params(
    std::vector<std::unique_ptr<Ident>> &func_f_params) {
    func_f_params.push_back(_parse_ident());
    while (_token.type == Token::TK_COMMA) {
        _next_token();
        func_f_params.push_back(_parse_ident());
    }
}

std::unique_ptr<VarDecl> Parser::_parse_var_decl() {
    auto var_decl = std::make_unique<VarDecl>();
    var_decl->lineno = _token.lineno;

    _match(_token, Token::TK_VAR);

    var_decl->ident = _parse_ident();

    _match(_token, Token::TK_SEMINCN);

    return var_decl;
}

std::unique_ptr<Stmt> Parser::_parse_stmt() {
    switch (_token.type) {
    case Token::TK_GET: {
        GetStmt get_stmt;
        get_stmt.lineno = _token.lineno;

        _next_token();

        get_stmt.ident = _parse_ident();

        _match(_token, Token::TK_SEMINCN);

        return std::make_unique<Stmt>(std::move(get_stmt));
    }
    case Token::TK_PUT: {
        PutStmt put_stmt;
        put_stmt.lineno = _token.lineno;

        _next_token();

        put_stmt.exp = _parse_exp();

        _match(_token, Token::TK_SEMINCN);

        return std::make_unique<Stmt>(std::move(put_stmt));
    }
    case Token::TK_TAG: {
        TagStmt tag_stmt;
        tag_stmt.lineno = _token.lineno;

        _next_token();

        tag_stmt.ident = _parse_ident();

        _match(_token, Token::TK_SEMINCN);

        return std::make_unique<Stmt>(std::move(tag_stmt));
    }
    case Token::TK_LET: {
        LetStmt let_stmt;
        let_stmt.lineno = _token.lineno;

        _next_token();

        let_stmt.ident = _parse_ident();

        _match(_token, Token::TK_ASSIGN);

        let_stmt.exp = _parse_exp();

        _match(_token, Token::TK_SEMINCN);

        return std::make_unique<Stmt>(std::move(let_stmt));
    }
    case Token::TK_IF: {
        IfStmt if_stmt;
        if_stmt.lineno = _token.lineno;

        _next_token();

        if_stmt.cond = _parse_cond();

        _match(_token, Token::TK_TO);

        if_stmt.ident = _parse_ident();

        _match(_token, Token::TK_SEMINCN);

        return std::make_unique<Stmt>(std::move(if_stmt));
    }
    case Token::TK_TO: {
        ToStmt to_stmt;
        to_stmt.lineno = _token.lineno;

        _next_token();

        to_stmt.ident = _parse_ident();

        _match(_token, Token::TK_SEMINCN);

        return std::make_unique<Stmt>(std::move(to_stmt));
    }
    default:
        ErrorReporter::error(_token.lineno, "expect statement");
        _recover();
        return nullptr;
    }
}

std::unique_ptr<Exp> Parser::_parse_exp() { return _parse_add_exp(); }

std::unique_ptr<Exp> Parser::_parse_add_exp() {
    auto lineno = _token.lineno;

    auto exp = _parse_mul_exp();
    while (_token.type == Token::TK_PLUS || _token.type == Token::TK_MINU) {
        BinaryExp binary_exp;
        binary_exp.lineno = lineno;
        binary_exp.lhs = std::move(exp);
        binary_exp.op =
            _token.type == Token::TK_PLUS ? BinaryExp::PLUS : BinaryExp::MINU;

        _next_token();
        auto rhs = _parse_mul_exp();
        binary_exp.rhs = std::move(rhs);
        exp = std::make_unique<Exp>(std::move(binary_exp));
    }
    return exp;
}

std::unique_ptr<Exp> Parser::_parse_mul_exp() {
    auto lineno = _token.lineno;

    auto exp = _parse_unary_exp();
    while (_token.type == Token::TK_MULT || _token.type == Token::TK_DIV) {
        BinaryExp binary_exp;
        binary_exp.lineno = lineno;
        binary_exp.lhs = std::move(exp);
        switch (_token.type) {
        case Token::TK_MULT:
            binary_exp.op = BinaryExp::MULT;
            break;
        case Token::TK_DIV:
            binary_exp.op = BinaryExp::DIV;
            break;
        default:
            throw std::runtime_error("unreachable code");
        }

        _next_token();
        binary_exp.rhs = _parse_unary_exp();
        exp = std::make_unique<Exp>(std::move(binary_exp));
    }
    return exp;
}

std::unique_ptr<Exp> Parser::_parse_unary_exp() {
    if (_token.type == Token::TK_IDENT && _pre_read.type == Token::TK_LPARENT) {
        CallExp call_exp;
        call_exp.lineno = _token.lineno;

        call_exp.ident = _parse_ident();

        _match(_token, Token::TK_LPARENT);

        if (_token.type != Token::TK_RPARENT) {
            _parse_func_r_params(call_exp.func_r_params);
        }

        _match(_token, Token::TK_RPARENT);

        return std::make_unique<Exp>(std::move(call_exp));
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
        return std::make_unique<Exp>(std::move(unary_exp));
    } else {
        ErrorReporter::error(_token.lineno, "expect unary expression");
        _recover();
        return nullptr;
    }
}

std::unique_ptr<Exp> Parser::_parse_primary_exp() {
    switch (_token.type) {
    case Token::TK_LPARENT: {
        _next_token();
        auto exp = _parse_exp();

        _match(_token, Token::TK_RPARENT);

        return exp;
    }
    case Token::TK_IDENT: {
        IdentExp lval_exp;
        lval_exp.lineno = _token.lineno;
        lval_exp.ident = _parse_ident();
        return std::make_unique<Exp>(std::move(lval_exp));
    }
    case Token::TK_NUMBER: {
        return _parse_number();
    }
    default:
        ErrorReporter::error(_token.lineno, "expect primary expression");
        _recover();
        return nullptr;
    }
}

void Parser::_parse_func_r_params(
    std::vector<std::unique_ptr<Exp>> &func_r_params) {
    func_r_params.push_back(_parse_exp());
    while (_token.type == Token::TK_COMMA) {
        _next_token();
        func_r_params.push_back(_parse_exp());
    }
}

std::unique_ptr<Cond> Parser::_parse_cond() {
    auto cond = std::make_unique<Cond>();
    cond->lineno = _token.lineno;

    cond->lhs = _parse_exp();
    switch (_token.type) {
    case Token::TK_LT:
        cond->op = Cond::LT;
        _next_token();
        break;
    case Token::TK_GT:
        cond->op = Cond::GT;
        _next_token();
        break;
    case Token::TK_LE:
        cond->op = Cond::LE;
        _next_token();
        break;
    case Token::TK_GE:
        cond->op = Cond::GE;
        _next_token();
        break;
    case Token::TK_EQ:
        cond->op = Cond::EQ;
        _next_token();
        break;
    case Token::TK_NE:
        cond->op = Cond::NE;
        _next_token();
        break;
    default:
        ErrorReporter::error(_token.lineno, "expect comparison operator");
        break;
    }
    cond->rhs = _parse_exp();
    return cond;
}

std::unique_ptr<Ident> Parser::_parse_ident() {
    if (_token.type == Token::TK_IDENT) {
        auto ident = std::make_unique<Ident>();
        ident->lineno = _token.lineno;
        ident->value = _token.content;
        _next_token();
        return ident;
    } else {
        ErrorReporter::error(_token.lineno, "expect identifier");
        return nullptr;
    }
}

std::unique_ptr<Exp> Parser::_parse_number() {
    if (_token.type == Token::TK_NUMBER) {
        Number number;
        number.lineno = _token.lineno;
        // number is float, so use std::stof
        number.value = std::stof(_token.content);
        _next_token();
        return std::make_unique<Exp>(number);
    } else {
        ErrorReporter::error(_token.lineno, "expect number");
        return nullptr;
    }
}

void Parser::_match(const Token &token, Token::TokenType expected) {
    if (token.type != expected) {
        ErrorReporter::error(_token.lineno,
                             "expect '" + token_type_to_string(expected) + "'");
    } else {
        _next_token();
    }
}
