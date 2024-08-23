#include "tolang/visitor.h"

#if TOLANG_BACKEND == LLVM

#include "tolang/error.h"
#include "tolang/utils.h"
#include "llvm/ir/Llvm.h"
#include "llvm/ir/value/ConstantData.h"

void Visitor::visit(const CompUnit &node) {
    for (auto &elm : node.func_defs) {
        _visit_func_def(*elm);
    }

    // create main function
    auto context = _ir_module->Context();
    _cur_func = Function::New(context->GetInt32Ty(), "main");
    _ir_module->AddMainFunction(_cur_func);
    _cur_block = _cur_func->NewBasicBlock();

    _cur_scope = _cur_scope->push_scope();
    for (auto &elm : node.var_decls) {
        _visit_var_decl(*elm);
    }
    for (auto &elm : node.stmts) {
        if (elm == nullptr) { // invalid ast
            continue;
        }
        _visit_stmt(*elm);
    }
    _cur_scope = _cur_scope->pop_scope();

    _cur_block->InsertInstruction(
        ReturnInst::New(ConstantData::New(context->GetInt32Ty(), 0)));

    _cur_block = nullptr;
    _cur_func = nullptr;
}

void Visitor::_visit_func_def(const FuncDef &node) {
    if (node.ident == nullptr) { // invalid ast
        return;
    }

    std::vector<std::shared_ptr<VariableSymbol>> param_symbols;
    for (auto &ident : node.func_f_params) {
        if (ident == nullptr) { // invalid ast
            continue;
        }
        // here we don't create ir argument, just create symbol
        param_symbols.push_back(std::make_shared<VariableSymbol>(
            ident->value, nullptr, ident->lineno));
    }

    // create ir function
    auto context = _ir_module->Context();
    std::vector<ArgumentPtr> args;
    for (auto &param_symbol : param_symbols) {
        auto arg = Argument::New(context->GetFloatTy(), param_symbol->name);
        args.push_back(arg);
        param_symbol->value = arg;
    }
    _cur_func = Function::New(context->GetFloatTy(), node.ident->value, args);

    // create function symbol
    auto symbol = std::make_shared<FunctionSymbol>(
        node.ident->value, _cur_func, node.lineno, param_symbols.size());

    // try to add function symbol to current scope
    if (!_cur_scope->add_symbol(symbol)) {
        auto pre_defined = _cur_scope->get_symbol(node.ident->value);
        ErrorReporter::error(node.lineno,
                             "redefine function " + node.ident->value +
                                 ", previous defined at line " +
                                 std::to_string(pre_defined->lineno));
        return;
    }

    // if success, add function to module
    _ir_module->AddFunction(_cur_func);

    // then create entry block
    _cur_block = _cur_func->NewBasicBlock();

    // new scope for function
    _cur_scope = _cur_scope->push_scope();

    for (auto &param_symbol : param_symbols) {
        if (!_cur_scope->add_symbol(param_symbol)) {
            auto pre_defined = _cur_scope->get_symbol(param_symbol->name);
            ErrorReporter::error(param_symbol->lineno,
                                 "redefine parameter " + param_symbol->name +
                                     ", previous defined at line " +
                                     std::to_string(pre_defined->lineno));
        }

        // alloca & store inst should be inserted at the first block
        auto alloca = AllocaInst::New(context->GetFloatTy());
        _cur_block->InsertInstruction(alloca);
        auto store = StoreInst::New(param_symbol->value, alloca);
        _cur_block->InsertInstruction(store);
        param_symbol->value = alloca;
    }

    // our tiny function body only contains a return statement
    if (node.exp == nullptr) { // invalid ast
        return;
    }
    auto val = _visit_exp(*node.exp);
    if (val == nullptr) {
        return;
    }
    _cur_block->InsertInstruction(ReturnInst::New(val));

    _cur_scope = _cur_scope->pop_scope();

    _cur_block = nullptr;
    _cur_func = nullptr;
}

void Visitor::_visit_var_decl(const VarDecl &node) {
    if (node.ident == nullptr) { // invalid ast
        return;
    }

    auto context = _ir_module->Context();
    auto alloca = AllocaInst::New(context->GetFloatTy());
    auto symbol = std::make_shared<VariableSymbol>(node.ident->value, alloca,
                                                   node.ident->lineno);

    if (!_cur_scope->add_symbol(symbol)) {
        auto pre_defined = _cur_scope->get_symbol(node.ident->value);
        ErrorReporter::error(node.ident->lineno,
                             "redefine variable " + node.ident->value +
                                 ", previous defined at line " +
                                 std::to_string(pre_defined->lineno));
        return;
    }

    // alloca inst should be inserted at the first block
    (*_cur_func->BasicBlockBegin())->InsertInstruction(alloca);
}

void Visitor::_visit_stmt(const Stmt &node) {
    std::visit(overloaded{
                   [this](const GetStmt &node) { _visit_get_stmt(node); },
                   [this](const PutStmt &node) { _visit_put_stmt(node); },
                   [this](const TagStmt &node) { _visit_tag_stmt(node); },
                   [this](const LetStmt &node) { _visit_let_stmt(node); },
                   [this](const IfStmt &node) { _visit_if_stmt(node); },
                   [this](const ToStmt &node) { _visit_to_stmt(node); },
               },
               node);
}

void Visitor::_visit_get_stmt(const GetStmt &node) {
    if (node.ident == nullptr) { // invalid ast
        return;
    }

    auto symbol = _cur_scope->get_symbol(node.ident->value);
    if (symbol == nullptr) {
        ErrorReporter::error(node.ident->lineno,
                             "undefined symbol " + node.ident->value);
        return;
    }
    auto input = InputInst::New(_ir_module->Context());
    _cur_block->InsertInstruction(input);
    auto store = StoreInst::New(input, symbol->value);
    _cur_block->InsertInstruction(store);
}

void Visitor::_visit_put_stmt(const PutStmt &node) {
    if (node.exp == nullptr) { // invalid ast
        return;
    }
    auto val = _visit_exp(*node.exp);
    if (val == nullptr) {
        return;
    }
    _cur_block->InsertInstruction(OutputInst::New(val));
}

void Visitor::_visit_tag_stmt(const TagStmt &node) {
    if (node.ident == nullptr) { // invalid ast
        return;
    }

    if (_cur_scope->exist_in_scope(node.ident->value)) {
        auto symbol = _cur_scope->get_symbol(node.ident->value);
        if (symbol->type != SymbolType::TAG) {
            ErrorReporter::error(node.ident->lineno,
                                 node.ident->value + " is not a tag, but a " +
                                     symbol_type_to_string(symbol->type) +
                                     ", defined at line " +
                                     std::to_string(symbol->lineno));
            return;
        }

        auto tag_symbol = std::dynamic_pointer_cast<TagSymbol>(symbol);

        // create new label
        auto jump = JumpInst::New(_ir_module->Context());
        _cur_block->InsertInstruction(jump);
        _cur_block = _cur_func->NewBasicBlock();
        jump->SetTarget(_cur_block);
        auto label = _cur_block;
        tag_symbol->target = label;

        // backpatching all jump inst before tag
        for (auto &inst : tag_symbol->jump_insts) {
            if (auto branch = dynamic_cast<BranchInst *>(inst)) {
                branch->SetTrueBlock(label);
            } else if (auto jump = dynamic_cast<JumpInst *>(inst)) {
                jump->SetTarget(label);
            }
        }

    } else {
        auto jump = JumpInst::New(_ir_module->Context());
        _cur_block->InsertInstruction(jump);
        _cur_block = _cur_func->NewBasicBlock();
        jump->SetTarget(_cur_block);
        auto label = _cur_block;

        auto symbol = std::make_shared<TagSymbol>(node.ident->value, nullptr,
                                                  node.ident->lineno);
        symbol->target = label;
        _cur_scope->add_symbol(symbol);
    }
}

void Visitor::_visit_let_stmt(const LetStmt &node) {
    if (node.ident == nullptr) { // invalid ast
        return;
    }

    auto symbol = _cur_scope->get_symbol(node.ident->value);
    if (symbol == nullptr) {
        ErrorReporter::error(node.ident->lineno,
                             "undefined symbol " + node.ident->value);
        return;
    }
    if (symbol->type != SymbolType::VAR) {
        ErrorReporter::error(node.ident->lineno,
                             node.ident->value + " is not a variable, but a " +
                                 symbol_type_to_string(symbol->type) +
                                 ", defined at line " +
                                 std::to_string(symbol->lineno));
        return;
    }

    auto var_symbol = std::dynamic_pointer_cast<VariableSymbol>(symbol);

    if (node.exp == nullptr) { // invalid ast
        return;
    }
    auto exp_val = _visit_exp(*node.exp);
    if (exp_val == nullptr) {
        return;
    }

    // get var symbol's addr (alloca inst)
    auto addr = var_symbol->value;

    // store value to addr
    _cur_block->InsertInstruction(StoreInst::New(exp_val, addr));
}

void Visitor::_visit_if_stmt(const IfStmt &node) {
    if (node.cond == nullptr) { // invalid ast
        return;
    }

    auto val = _visit_cond(*node.cond);
    if (val == nullptr) {
        return;
    }

    if (_cur_scope->exist_in_scope(node.ident->value)) {
        auto symbol = _cur_scope->get_symbol(node.ident->value);
        if (symbol->type != SymbolType::TAG) {
            ErrorReporter::error(node.ident->lineno,
                                 node.ident->value + " is not a tag, but a " +
                                     symbol_type_to_string(symbol->type) +
                                     ", defined at line " +
                                     std::to_string(symbol->lineno));
            return;
        }

        auto jump = BranchInst::New(val, nullptr, nullptr);
        _cur_block->InsertInstruction(jump);
        _cur_block = _cur_func->NewBasicBlock();
        jump->SetFalseBlock(_cur_block);

        // if the tag is not declared, add jump inst to symbol.
        // otherwise set the target of jump to tag
        auto tag_symbol = std::static_pointer_cast<TagSymbol>(symbol);
        if (tag_symbol->target) {
            jump->SetTrueBlock(tag_symbol->target);
        } else {
            tag_symbol->jump_insts.push_back(jump);
        }

    } else {
        auto jump = BranchInst::New(val, nullptr, nullptr);
        _cur_block->InsertInstruction(jump);
        _cur_block = _cur_func->NewBasicBlock();
        jump->SetFalseBlock(_cur_block);

        auto symbol =
            std::make_shared<TagSymbol>(node.ident->value, nullptr, -1);
        _cur_scope->add_symbol(symbol);
        // add jump inst to symbol
        symbol->jump_insts.push_back(jump);
    }
}

void Visitor::_visit_to_stmt(const ToStmt &node) {
    if (node.ident == nullptr) { // invalid ast
        return;
    }

    if (_cur_scope->exist_in_scope(node.ident->value)) {
        auto symbol = _cur_scope->get_symbol(node.ident->value);
        if (symbol->type != SymbolType::TAG) {
            ErrorReporter::error(node.ident->lineno,
                                 node.ident->value + " is not a tag, but a " +
                                     symbol_type_to_string(symbol->type) +
                                     ", defined at line " +
                                     std::to_string(symbol->lineno));
            return;
        }

        auto jump = JumpInst::New(_ir_module->Context());
        _cur_block->InsertInstruction(jump);
        _cur_block = _cur_func->NewBasicBlock();

        // if the tag is not declared, add jump inst to symbol.
        // otherwise set the target of jump to tag
        auto tag_symbol = std::static_pointer_cast<TagSymbol>(symbol);
        if (tag_symbol->target) {
            jump->SetTarget(tag_symbol->target);
        } else {
            tag_symbol->jump_insts.push_back(jump);
        }
    } else {
        auto jump = JumpInst::New(_ir_module->Context());
        _cur_block->InsertInstruction(jump);
        _cur_block = _cur_func->NewBasicBlock();

        auto symbol =
            std::make_shared<TagSymbol>(node.ident->value, nullptr, -1);
        _cur_scope->add_symbol(symbol);

        // add jump inst to symbol
        symbol->jump_insts.push_back(jump);
    }
}

ValuePtr Visitor::_visit_exp(const Exp &node) {
    return std::visit(
        overloaded{
            [this](const BinaryExp &node) { return _visit_binary_exp(node); },
            [this](const CallExp &node) { return _visit_call_exp(node); },
            [this](const UnaryExp &node) { return _visit_unary_exp(node); },
            [this](const IdentExp &node) { return _visit_ident_exp(node); },
            [this](const Number &node) { return _visit_number(node); },
        },
        node);
}

ValuePtr Visitor::_visit_binary_exp(const BinaryExp &node) {
    if (node.lhs == nullptr || node.rhs == nullptr) { // invalid ast
        return nullptr;
    }
    auto left_val = _visit_exp(*node.lhs);
    auto right_val = _visit_exp(*node.rhs);
    if (!left_val || !right_val) {
        return nullptr;
    }

    // get ir binary op type, just map from ast to ir
    BinaryOpType ir_op;
    switch (node.op) {
    case BinaryExp::PLUS:
        ir_op = BinaryOpType::Add;
        break;
    case BinaryExp::MINU:
        ir_op = BinaryOpType::Sub;
        break;
    case BinaryExp::MULT:
        ir_op = BinaryOpType::Mul;
        break;
    case BinaryExp::DIV:
        ir_op = BinaryOpType::Div;
        break;
    case BinaryExp::MOD:
        ir_op = BinaryOpType::Mod;
        break;
    default:
        break; // unreachable
    }

    auto val = BinaryOperator::New(ir_op, left_val, right_val);
    _cur_block->InsertInstruction(val);

    return val;
}

ValuePtr Visitor::_visit_call_exp(const CallExp &node) {
    if (node.ident == nullptr) { // invalid ast
        return nullptr;
    }

    auto symbol = _cur_scope->get_symbol(node.ident->value);
    if (symbol == nullptr) {
        ErrorReporter::error(node.ident->lineno,
                             "undefined symbol " + node.ident->value);
        return nullptr;
    }
    if (symbol->type != SymbolType::FUNC) {
        ErrorReporter::error(node.ident->lineno,
                             node.ident->value + " is not a function, but a " +
                                 symbol_type_to_string(symbol->type) +
                                 ", defined at line " +
                                 std::to_string(symbol->lineno));
        return nullptr;
    }

    auto func_symbol = std::static_pointer_cast<FunctionSymbol>(symbol);

    if (func_symbol->params_count != node.func_r_params.size()) {
        ErrorReporter::error(
            node.ident->lineno,
            "params number not matched in function call " + node.ident->value +
                ", expect " + std::to_string(func_symbol->params_count) +
                " but got " + std::to_string(node.func_r_params.size()));
        return nullptr;
    }

    std::vector<ValuePtr> values;
    for (auto &exp : node.func_r_params) {
        if (exp == nullptr) { // invalid ast
            continue;
        }
        auto exp_val = _visit_exp(*exp);
        if (exp_val == nullptr) {
            continue;
        }
        values.push_back(exp_val);
    }

    if (values.size() != func_symbol->params_count) { // invalid ast
        return nullptr;
    }

    // create call inst
    std::vector<ValuePtr> params;
    for (auto &val : values) {
        params.push_back(val);
    }
    auto call =
        CallInst::New(static_cast<FunctionPtr>(func_symbol->value), params);
    _cur_block->InsertInstruction(call);

    return call;
}

ValuePtr Visitor::_visit_unary_exp(const UnaryExp &node) {
    if (node.exp == nullptr) { // invalid ast
        return nullptr;
    }
    auto exp_val = _visit_exp(*node.exp);
    if (exp_val == nullptr) {
        return nullptr;
    }

    // get ir unary op type, just map from ast to ir
    UnaryOpType ir_op;
    switch (node.op) {
    case UnaryExp::MINU:
        ir_op = UnaryOpType::Neg;
        break;
    case UnaryExp::PLUS:
        ir_op = UnaryOpType::Pos;
        break;
    default:
        break; // unreachable
    }

    auto val = UnaryOperator::New(ir_op, exp_val);
    _cur_block->InsertInstruction(val);

    return val;
}

ValuePtr Visitor::_visit_ident_exp(const IdentExp &node) {
    if (node.ident == nullptr) { // invalid ast
        return nullptr;
    }

    auto symbol = _cur_scope->get_symbol(node.ident->value);
    if (symbol == nullptr) {
        ErrorReporter::error(node.ident->lineno,
                             "undefined symbol " + node.ident->value);
        return nullptr;
    }
    if (symbol->type != SymbolType::VAR) {
        ErrorReporter::error(node.ident->lineno,
                             node.ident->value + " is not a variable, but a " +
                                 symbol_type_to_string(symbol->type) +
                                 ", defined at line " +
                                 std::to_string(symbol->lineno));
        return nullptr;
    }

    // load value from addr
    auto var_symbol = std::static_pointer_cast<VariableSymbol>(symbol);
    auto addr = var_symbol->value;
    auto val = LoadInst::New(addr);
    _cur_block->InsertInstruction(val);

    return val;
}

ValuePtr Visitor::_visit_number(const Number &node) {
    // return const value
    auto val =
        ConstantData::New(_ir_module->Context()->GetFloatTy(), node.value);
    return val;
}

ValuePtr Visitor::_visit_cond(const Cond &node) {
    if (node.lhs == nullptr || node.rhs == nullptr) { // invalid ast
        return nullptr;
    }
    auto left_val = _visit_exp(*node.lhs);
    auto right_val = _visit_exp(*node.rhs);
    if (left_val == nullptr || right_val == nullptr) {
        return nullptr;
    }

    CompareOpType ir_op;
    switch (node.op) {
    case Cond::EQ:
        ir_op = CompareOpType::Equal;
        break;
    case Cond::NE:
        ir_op = CompareOpType::NotEqual;
        break;
    case Cond::LT:
        ir_op = CompareOpType::LessThan;
        break;
    case Cond::LE:
        ir_op = CompareOpType::LessThanOrEqual;
        break;
    case Cond::GT:
        ir_op = CompareOpType::GreaterThan;
        break;
    case Cond::GE:
        ir_op = CompareOpType::GreaterThanOrEqual;
        break;
    default:
        throw std::runtime_error("invalid compare op");
    }

    auto cmp = CompareInstruction::New(ir_op, left_val, right_val);
    _cur_block->InsertInstruction(cmp);

    return cmp;
}

#elif TOLANG_BACKEND == PCODE

void PcodeVisitor::visit(const CompUnit &node) {
    for (auto &elm : node.func_defs) {
        visitFuncDef(*elm);
    }

    createBlock();
    auto label = PcodeInstruction::create<PcodeLabelInst>("_Main");
    _curBlock->insertInst(label);
    _module.addLabel("_Main", _curBlock);

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
    _module.addFunction(funcName, pcodeFunc);

    // Add to symbol table
    auto symbol = PcodeSymbol::create(funcName, PcodeSymbol::P_FUNC);
    _symbolTable.insertSymbol(symbol);

    // Create a new block with a label as the first inst
    createBlock();
    auto label = PcodeInstruction::create<PcodeLabelInst>(funcName);
    _module.addLabel(funcName, _curBlock);
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
    auto ret = PcodeInstruction::create<PcodeReturnInst>();
    _curBlock->insertInst(ret);
}

void PcodeVisitor::visitVarDecl(const VarDecl &node) {
    auto content = node.ident->value;
    // Maintain symbol table
    auto symbol = PcodeSymbol::create(content, PcodeSymbol::P_VAR);
    _symbolTable.insertSymbol(symbol);

    // Maintain variable list of the visitor
    auto var = PcodeVariable::create(content);
    _module.addVariable(content, var);

    // Add pcode instruction
    auto def = PcodeInstruction::create<PcodeDefineInst>(_module.getVariable(content));
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
    auto read = PcodeInstruction::create<PcodeReadInst>();
    _curBlock->insertInst(read);

    auto store = PcodeInstruction::create<PcodeStoreInst>(_module.getVariable(node.ident->value));
    _curBlock->insertInst(store);
}

void PcodeVisitor::visitPutStmt(const PutStmt &node) {
    visitExp(*node.exp);
    
    auto write = PcodeInstruction::create<PcodeWriteInst>();
    _curBlock->insertInst(write);
}

void PcodeVisitor::visitTagStmt(const TagStmt &node) {
    auto content = node.ident->value;
    createBlock();
    auto label = PcodeInstruction::create<PcodeLabelInst>(content);
    _curBlock->insertInst(label);
    _module.addLabel(content, _curBlock);
}

void PcodeVisitor::visitLetStmt(const LetStmt &node) {
    visitExp(*node.exp);
    auto store = PcodeInstruction::create<PcodeStoreInst>(_module.getVariable(node.ident->value));
    _curBlock->insertInst(store);
}

void PcodeVisitor::visitIfStmt(const IfStmt &node) {
    visitCond(*node.cond);
    auto jit = PcodeInstruction::create<PcodeJumpIfTrueInst>(node.ident->value);
    _curBlock->insertInst(jit);
}

void PcodeVisitor::visitToStmt(const ToStmt &node) {
    auto jump = PcodeInstruction::create<PcodeJumpInst>(node.ident->value);
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
    PcodeOperationInst::OperationType op;
    switch (node.op) {
        case BinaryExp::PLUS: op = PcodeOperationInst::ADD; break;
        case BinaryExp::MINU: op = PcodeOperationInst::SUB; break;
        case BinaryExp::MULT: op = PcodeOperationInst::MUL; break;
        case BinaryExp::DIV : op = PcodeOperationInst::DIV; break;
        case BinaryExp::MOD : break;
    }
    auto opr = PcodeInstruction::create<PcodeOperationInst>(op);
    _curBlock->insertInst(opr);
}

void PcodeVisitor::visitCallExp(const CallExp &node) {
    for (auto &param : node.func_r_params) {
        visitExp(*param);
    }
    auto call = PcodeInstruction::create<PcodeCallInst>(_module.getFunction(node.ident->value));
    _curBlock->insertInst(call);
}

void PcodeVisitor::visitUnaryExp(const UnaryExp &node) {
    visitExp(*node.exp);
    if (node.op == UnaryExp::MINU) {
        auto opr = PcodeInstruction::create<PcodeOperationInst>(PcodeOperationInst::NEG);
        _curBlock->insertInst(opr);
    }
}

void PcodeVisitor::visitIdentExp(const IdentExp &node) {
    if (_symbolTable.inFunctionScope()) {
        int index = _symbolTable.getIndexOf(node.ident->value);
        if (index > 0) {
            auto arg = PcodeInstruction::create<PcodeArgumentInst>(index);
            _curBlock->insertInst(arg);
        }
    } else {
        auto load = PcodeInstruction::create<PcodeLoadInst>(_module.getVariable(node.ident->value));
        _curBlock->insertInst(load);
    }
}

void PcodeVisitor::visitNumber(const Number &node) {
    auto li = PcodeInstruction::create<PcodeLoadImmediateInst>(node.value);
    _curBlock->insertInst(li);
}

void PcodeVisitor::visitCond(const Cond &node) {
    visitExp(*node.lhs);
    visitExp(*node.rhs);
    PcodeOperationInst::OperationType op;
    switch (node.op) {
        case Cond::EQ: op = PcodeOperationInst::EQL; break;
        case Cond::NE: op = PcodeOperationInst::NEQ; break;
        case Cond::LT: op = PcodeOperationInst::LES; break;
        case Cond::LE: op = PcodeOperationInst::LEQ; break;
        case Cond::GT: op = PcodeOperationInst::GRE; break;
        case Cond::GE: op = PcodeOperationInst::GEQ; break;
    }
    auto opr = PcodeInstruction::create<PcodeOperationInst>(op);
    _curBlock->insertInst(opr);
}

#else

#error "unknown backend"

#endif
