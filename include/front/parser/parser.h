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

    Token _token;
    AbstractLexer &_lexer;
};
