#pragma once

#include "ast.h"
#include "front/lexer/lexer.h"
#include <vector>

class Parser {
public:
    Parser(AbstractLexer &lexer) : _lexer(lexer){};

    std::shared_ptr<CompUnit> parse();

private:
    std::shared_ptr<CompUnit> _parse_comp_unit();
    std::shared_ptr<FuncDef> _parse_func_def();
    std::shared_ptr<FuncFParams> _parse_func_f_params();
    std::shared_ptr<VarDecl> _parse_var_decl();
    std::shared_ptr<Stmt> _parse_stmt();
    std::shared_ptr<Exp> _parse_exp();
    std::shared_ptr<Exp> _parse_add_exp();
    std::shared_ptr<Exp> _parse_mul_exp();
    std::shared_ptr<Exp> _parse_unary_exp();
    std::shared_ptr<Exp> _parse_primary_exp();
    std::shared_ptr<FuncRParams> _parse_func_r_params();
    std::shared_ptr<Cond> _parse_cond();
    std::shared_ptr<Ident> _parse_ident();
    std::shared_ptr<Exp> _parse_number();

    void _next_token() {
        _token = _pre_read;
        _lexer.next(_pre_read);
    }

    void _recover() {
        do {
            _next_token();
        } while (_token.type != Token::TK_SEMINCN &&
                 _token.type != Token::TK_EOF);
        _next_token();
    }

    Token _token, _pre_read;
    AbstractLexer &_lexer;
};
