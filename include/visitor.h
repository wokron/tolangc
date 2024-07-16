#pragma once

#include "ast.h"
#include "symtable.h"
#include "llvm/ir/Module.h"

/**
 * @brief `Visitor` is a class that visits the abstract syntax tree and
 * generates the intermediate representation.
 */
class Visitor {
public:
    /**
     * @brief Construct a new Visitor object.
     * @param module The intermediate representation module.
     */
    Visitor(ModulePtr module)
        : _ir_module(module), _cur_scope(std::make_shared<SymbolTable>()) {}

    /**
     * @brief Visit the given abstract syntax tree.
     * @param node The root of the abstract syntax tree.
     * @note This function will generate the intermediate representation, and
     * store it in the `module`, which has been passed to the constructor.
     */
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

    std::shared_ptr<SymbolTable> _cur_scope;
    ModulePtr _ir_module;
    FunctionPtr _cur_func = nullptr;
    BasicBlockPtr _cur_block = nullptr;
};
