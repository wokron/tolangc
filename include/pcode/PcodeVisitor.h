#pragma once

#include "./PcodeInstruction.h"
#include "./PcodeInstructions.h"
#include "./PcodeSymbol.h"
#include "utils.h"
#include "ast.h"

#include <string>
#include <map>
#include <vector>

struct PcodeBlock;
using PcodeBlockPtr = std::shared_ptr<PcodeBlock>; 

struct PcodeBlock {
    std::vector<PcodeInstPtr> instructions;
    PcodeBlockPtr next = nullptr;

    void insertInst(PcodeInstPtr &inst) {
        instructions.push_back(inst);
    }

    static PcodeBlockPtr create() {
        return std::make_shared<PcodeBlock>();
    }
};

class PcodeVisitor {
private:
    std::vector<PcodeBlockPtr> _blocks;
    PcodeBlockPtr _curBlock = nullptr;

    std::map<std::string, PcodeVarPtr> _variables;
    std::map<std::string, PcodeFuncPtr> _functions;
    std::map<std::string, PcodeBlockPtr> _labels;

    PcodeSymbolTable _symbolTable;

    void createBlock() {
        if (_curBlock == nullptr) {
            _curBlock = PcodeBlock::create();
        } else {
            _curBlock->next = PcodeBlock::create();
            _curBlock = _curBlock->next;
        }
        _blocks.push_back(_curBlock);
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

    void print(std::ostream &out) const {
        for (auto &b : _blocks) {
            for (auto &i : b->instructions) {
                i->print(out);
            }
            out << "\n";
        }
    }

    friend class PcodeRuntime;
};