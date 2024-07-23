#pragma once

#include "pcode/PcodeModule.h"
#include "pcode/PcodeInstruction.h"
#include "pcode/PcodeInstructions.h"
#include "pcode/PcodeSymbol.h"
#include "pcode/PcodeBlock.h"
#include "utils.h"
#include "ast.h"

#include <string>
#include <map>
#include <vector>

class PcodeVisitor {
private:
    PcodeModule &_module;

    PcodeBlockPtr _curBlock = nullptr;

    PcodeSymbolTable _symbolTable;

    void createBlock() {
        if (_curBlock == nullptr) {
            _curBlock = PcodeBlock::create();
        } else {
            _curBlock->next = PcodeBlock::create();
            _curBlock = _curBlock->next;
        }
        _module.addBlock(_curBlock);
    }

    void visitFuncDef(const FuncDef &node);

    void visitVarDecl(const VarDecl &node);

    void visitStmt(const Stmt &node);
    void visitGetStmt(const GetStmt &node);
    void visitPutStmt(const PutStmt &node);
    void visitTagStmt(const TagStmt &node);
    void visitLetStmt(const LetStmt &node);
    void visitIfStmt(const IfStmt &node);
    void visitToStmt(const ToStmt &node);

    void visitExp(const Exp &node);
    void visitBinaryExp(const BinaryExp &node);
    void visitCallExp(const CallExp &node);
    void visitUnaryExp(const UnaryExp &node);
    void visitIdentExp(const IdentExp &node);
    void visitNumber(const Number &node);
    void visitCond(const Cond &node);

public:
    void visit(const CompUnit &node);

    PcodeVisitor(PcodeModule &pm) : _module(pm) {}
};