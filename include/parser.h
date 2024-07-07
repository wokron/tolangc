#pragma once

#include "ast.h"
#include "lexer.h"
#include <vector>

class Parser {
public:
    Parser(Lexer &lexer) : _lexer(lexer){};

    std::unique_ptr<CompUnit> parse();

private:
    std::unique_ptr<CompUnit> _parse_comp_unit();
    std::unique_ptr<FuncDef> _parse_func_def();
    void
    _parse_func_f_params(std::vector<std::unique_ptr<Ident>> &func_f_params);
    std::unique_ptr<VarDecl> _parse_var_decl();
    std::unique_ptr<Stmt> _parse_stmt();
    std::unique_ptr<Exp> _parse_exp();
    std::unique_ptr<Exp> _parse_add_exp();
    std::unique_ptr<Exp> _parse_mul_exp();
    std::unique_ptr<Exp> _parse_unary_exp();
    std::unique_ptr<Exp> _parse_primary_exp();
    void _parse_func_r_params(std::vector<std::unique_ptr<Exp>> &func_r_params);
    std::unique_ptr<Cond> _parse_cond();
    std::unique_ptr<Ident> _parse_ident();
    std::unique_ptr<Exp> _parse_number();

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
    Lexer &_lexer;
};
