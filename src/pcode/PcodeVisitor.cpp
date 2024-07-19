#include "pcode/PcodeVisitor.h"

void PcodeVisitor::visit(const CompUnit &node) {
    for (auto &elm : node.func_defs) {
        visitFuncDef(*elm);
    }

    createBlock();
    auto label = PcodeInstruction::create<LabelInst>("_Main");
    _curBlock->insertInst(label);
    _labels.emplace("_Main", _curBlock);

    for (auto &elm : node.var_decls) {
        visitVarDecl(*elm);
    }

    for (auto &elm : node.stmts) {
        visitStmt(*elm);
    }
}

void PcodeVisitor::visitFuncDef(const FuncDef &node) {
    // Get function name and parameter count
    auto funcName = node.ident->value;
    auto paramCounter = node.func_f_params.size();

    // Create a pcode function object
    auto pcodeFunc = PcodeFunction::create(funcName, paramCounter);
    _functions.emplace(funcName, pcodeFunc);

    // Add to symbol table
    auto symbol = PcodeSymbol::create(funcName, PcodeSymbol::P_FUNC);
    _symbolTable.insertSymbol(symbol);

    // Create a new block with a label as the first inst
    createBlock();
    auto label = PcodeInstruction::create<LabelInst>(funcName);
    _labels.emplace(funcName, _curBlock);
    _curBlock->insertInst(label);

    // Create a new symbol table node for function parameters
    _symbolTable.pushScope();
    
    for (auto &param : node.func_f_params) {
        auto symbol = PcodeSymbol::create(param->value, PcodeSymbol::P_VAR);
        _symbolTable.insertSymbol(symbol);
    }

    visitExp(*node.exp);
    _symbolTable.popScope();

    // Add a return instruction manually
    auto ret = PcodeInstruction::create<ReturnInst>();
    _curBlock->insertInst(ret);
}

void PcodeVisitor::visitVarDecl(const VarDecl &node) {
    auto content = node.ident->value;
    // Maintain symbol table
    auto symbol = PcodeSymbol::create(content, PcodeSymbol::P_VAR);
    _symbolTable.insertSymbol(symbol);

    // Maintain variable list of the visitor
    auto var = PcodeVariable::create(content);
    _variables.emplace(content, var);

    // Add pcode instruction
    auto def = PcodeInstruction::create<DefineInst>(_variables.at(content));
    _curBlock->insertInst(def);
}

void PcodeVisitor::visitStmt(const Stmt &node) {
    std::visit(overloaded{
                   [this](const GetStmt &node) { visitGetStmt(node); },
                   [this](const PutStmt &node) { visitPutStmt(node); },
                   [this](const TagStmt &node) { visitTagStmt(node); },
                   [this](const LetStmt &node) { visitLetStmt(node); },
                   [this](const IfStmt &node) { visitIfStmt(node); },
                   [this](const ToStmt &node) { visitToStmt(node); },
               },
               node);
}

void PcodeVisitor::visitGetStmt(const GetStmt &node) {
    auto read = PcodeInstruction::create<ReadInst>();
    _curBlock->insertInst(read);

    auto store = PcodeInstruction::create<StoreInst>(_variables.at(node.ident->value));
    _curBlock->insertInst(store);
}

void PcodeVisitor::visitPutStmt(const PutStmt &node) {
    visitExp(*node.exp);
    
    auto write = PcodeInstruction::create<WriteInst>();
    _curBlock->insertInst(write);
}

void PcodeVisitor::visitTagStmt(const TagStmt &node) {
    auto content = node.ident->value;
    createBlock();
    auto label = PcodeInstruction::create<LabelInst>(content);
    _curBlock->insertInst(label);
    _labels.emplace(content, _curBlock);
}

void PcodeVisitor::visitLetStmt(const LetStmt &node) {
    visitExp(*node.exp);
    auto store = PcodeInstruction::create<StoreInst>(_variables.at(node.ident->value));
    _curBlock->insertInst(store);
}

void PcodeVisitor::visitIfStmt(const IfStmt &node) {
    visitCond(*node.cond);
    auto jit = PcodeInstruction::create<JumpIfTrueInst>(node.ident->value);
    _curBlock->insertInst(jit);
}

void PcodeVisitor::visitToStmt(const ToStmt &node) {
    auto jump = PcodeInstruction::create<JumpInst>(node.ident->value);
    _curBlock->insertInst(jump);
}

void PcodeVisitor::visitExp(const Exp &node) {
    std::visit(
        overloaded{
            [this](const BinaryExp &node) { return visitBinaryExp(node); },
            [this](const CallExp &node) { return visitCallExp(node); },
            [this](const UnaryExp &node) { return visitUnaryExp(node); },
            [this](const IdentExp &node) { return visitIdentExp(node); },
            [this](const Number &node) { return visitNumber(node); },
        },
        node);
}

void PcodeVisitor::visitBinaryExp(const BinaryExp &node) {
    visitExp(*node.lhs);
    visitExp(*node.rhs);
    OperationInst::OperationType op;
    switch (node.op) {
        case BinaryExp::PLUS: op = OperationInst::ADD; break;
        case BinaryExp::MINU: op = OperationInst::SUB; break;
        case BinaryExp::MULT: op = OperationInst::MUL; break;
        case BinaryExp::DIV : op = OperationInst::DIV; break;
        case BinaryExp::MOD : break;
    }
    auto opr = PcodeInstruction::create<OperationInst>(op);
    _curBlock->insertInst(opr);
}

void PcodeVisitor::visitCallExp(const CallExp &node) {
    for (auto &param : node.func_r_params) {
        visitExp(*param);
    }
    auto call = PcodeInstruction::create<CallInst>(_functions.at(node.ident->value));
    _curBlock->insertInst(call);
}

void PcodeVisitor::visitUnaryExp(const UnaryExp &node) {
    visitExp(*node.exp);
    if (node.op == UnaryExp::MINU) {
        auto opr = PcodeInstruction::create<OperationInst>(OperationInst::NEG);
        _curBlock->insertInst(opr);
    }
}

void PcodeVisitor::visitIdentExp(const IdentExp &node) {
    if (_symbolTable.inFunctionScope()) {
        int index = _symbolTable.getIndexOf(node.ident->value);
        if (index > 0) {
            auto arg = PcodeInstruction::create<ArgumentInst>(index);
            _curBlock->insertInst(arg);
        }
    } else {
        auto load = PcodeInstruction::create<LoadInst>(_variables.at(node.ident->value));
        _curBlock->insertInst(load);
    }
}

void PcodeVisitor::visitNumber(const Number &node) {
    auto li = PcodeInstruction::create<LoadImmediateInst>(node.value);
    _curBlock->insertInst(li);
}

void PcodeVisitor::visitCond(const Cond &node) {
    visitExp(*node.lhs);
    visitExp(*node.rhs);
    OperationInst::OperationType op;
    switch (node.op) {
        case Cond::EQ: op = OperationInst::EQL; break;
        case Cond::NE: op = OperationInst::NEQ; break;
        case Cond::LT: op = OperationInst::LES; break;
        case Cond::LE: op = OperationInst::LEQ; break;
        case Cond::GT: op = OperationInst::GRE; break;
        case Cond::GE: op = OperationInst::GEQ; break;
    }
    auto opr = PcodeInstruction::create<OperationInst>(op);
    _curBlock->insertInst(opr);
}
