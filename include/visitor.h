#pragma once

#include "ast.h"
#include "symtable.h"
#include "llvm/ir/Module.h"

class Visitor {
public:
    Visitor(ModulePtr module)
        : _ir_module(module), _cur_scope(std::make_shared<SymbolTable>()) {}

    void visit(const CompUnit &node);

private:
    void _visit_func_def(const FuncDef &node);
    void _visit_var_decl(const VarDecl &node);

    void _visit_stmt(const Stmt &node);
    void _visit_get_stmt(const GetStmt &node);
    void _visit_put_stmt(const PutStmt &node);
    void _visit_tag_stmt(const TagStmt &node);
    void _visit_let_stmt(const LetStmt &node);
    void _visit_if_stmt(const IfStmt &node);
    void _visit_to_stmt(const ToStmt &node);

    ValuePtr _visit_exp(const Exp &node);
    ValuePtr _visit_binary_exp(const BinaryExp &node);
    ValuePtr _visit_call_exp(const CallExp &node);
    ValuePtr _visit_unary_exp(const UnaryExp &node);
    ValuePtr _visit_ident_exp(const IdentExp &node);
    ValuePtr _visit_number(const Number &node);
    ValuePtr _visit_cond(const Cond &node);

    std::vector<ValuePtr> _visit_func_r_params(const FuncRParams &node);

    std::shared_ptr<SymbolTable> _cur_scope;
    ModulePtr _ir_module;
    FunctionPtr _cur_func = nullptr;
    BasicBlockPtr _cur_block = nullptr;
};
