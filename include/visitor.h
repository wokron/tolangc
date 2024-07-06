#pragma once

#include "ast.h"
#include "symbol_table.h"
#include "llvm/ir/Module.h"

class Visitor {
public:
    Visitor(ModulePtr module)
        : _ir_module(module), _cur_scope(std::make_shared<SymbolTable>()) {}
    Visitor(ModulePtr module, std::shared_ptr<SymbolTable> cur_scope)
        : _ir_module(module), _cur_scope(cur_scope) {}

    void visit(const CompUnit &node);
    void visitFuncDef(const FuncDef &node);
    std::vector<std::shared_ptr<VariableSymbol>>
    visitFuncFParams(const FuncFParams &node);
    void visitVarDecl(const VarDecl &node);

    void visitStmt(const Stmt &node);
    void visitGetStmt(const GetStmt &node);
    void visitPutStmt(const PutStmt &node);
    void visitTagStmt(const TagStmt &node);
    void visitLetStmt(const LetStmt &node);
    void visitIfStmt(const IfStmt &node);
    void visitToStmt(const ToStmt &node);

    ValuePtr visitExp(const Exp &node);
    ValuePtr visitBinaryExp(const BinaryExp &node);
    ValuePtr visitCallExp(const CallExp &node);
    ValuePtr visitUnaryExp(const UnaryExp &node);
    ValuePtr visitIdentExp(const IdentExp &node);
    ValuePtr visitNumber(const Number &node);
    ValuePtr visitCond(const Cond &node);

    std::vector<ValuePtr> visitFuncRParams(const FuncRParams &node);

private:
    std::shared_ptr<SymbolTable> _cur_scope;
    ModulePtr _ir_module;
    FunctionPtr _cur_func = nullptr;
    BasicBlockPtr _cur_block = nullptr;
};
