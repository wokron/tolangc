#pragma once

#include "ast.h"
#include "symbol_table.h"

class Visitor {
  public:
    Visitor() : _cur_scope(std::make_shared<SymbolTable>()) {}
    Visitor(std::shared_ptr<SymbolTable> cur_scope) : _cur_scope(cur_scope) {}

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

    std::shared_ptr<Value> visitExp(const Exp &node);
    std::shared_ptr<Value> visitBinaryExp(const BinaryExp &node);
    std::shared_ptr<Value> visitCallExp(const CallExp &node);
    std::shared_ptr<Value> visitUnaryExp(const UnaryExp &node);
    std::shared_ptr<Value> visitIdent(const Ident &node);
    std::shared_ptr<Value> visitNumber(const Number &node);
    std::shared_ptr<Value> visitCond(const Cond &node);

    std::vector<std::shared_ptr<Value>>
    visitFuncRParams(const FuncRParams &node);

  private:
    std::shared_ptr<SymbolTable> _cur_scope;
};
