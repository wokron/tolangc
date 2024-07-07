#include "visitor.h"
#include "error.h"
#include "utils.h"
#include "llvm/ir/Llvm.h"
#include "llvm/ir/value/ConstantData.h"

void Visitor::visit(const CompUnit &node) {
    for (auto &elm : node.funcDefs) {
        visitFuncDef(*elm);
    }

    // create main function
    auto context = _ir_module->Context();
    _cur_func = Function::New(context->GetInt32Ty(), "main");
    _ir_module->AddMainFunction(_cur_func);
    _cur_block = _cur_func->NewBasicBlock();

    _cur_scope = _cur_scope->pushScope();
    for (auto &elm : node.varDecls) {
        visitVarDecl(*elm);
    }
    for (auto &elm : node.stmts) {
        if (elm == nullptr) { // invalid ast
            continue;
        }
        visitStmt(*elm);
    }
    _cur_scope = _cur_scope->popScope();

    _cur_block->InsertInstruction(
        ReturnInst::New(ConstantData::New(context->GetInt32Ty(), 0)));

    _cur_block = nullptr;
    _cur_func = nullptr;
}

void Visitor::visitFuncDef(const FuncDef &node) {
    if (node.ident == nullptr) { // invalid ast
        return;
    }

    auto param_symbols = visitFuncFParams(*node.funcFParams);

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
    if (!_cur_scope->addSymbol(symbol)) {
        ErrorReporter::error(node.lineno, "redefine function " + node.ident->value);
        return;
    }

    // if success, add function to module
    _ir_module->AddFunction(_cur_func);

    // then create entry block
    _cur_block = _cur_func->NewBasicBlock();

    // new scope for function
    _cur_scope = _cur_scope->pushScope();

    for (auto &param_symbol : param_symbols) {
        if (!_cur_scope->addSymbol(param_symbol)) {
            ErrorReporter::error(param_symbol->line_number,
                  "redefine parameter " + param_symbol->name);
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
    auto val = visitExp(*node.exp);
    if (val == nullptr) {
        return;
    }
    _cur_block->InsertInstruction(ReturnInst::New(val));

    _cur_scope = _cur_scope->popScope();

    _cur_block = nullptr;
    _cur_func = nullptr;
}

std::vector<std::shared_ptr<VariableSymbol>>
Visitor::visitFuncFParams(const FuncFParams &node) {
    std::vector<std::shared_ptr<VariableSymbol>> rt;
    for (auto &ident : node.idents) {
        if (ident == nullptr) { // invalid ast
            continue;
        }
        // here we don't create ir argument, just create symbol
        rt.push_back(std::make_shared<VariableSymbol>(ident->value, nullptr,
                                                      ident->lineno));
    }
    return rt;
}

void Visitor::visitVarDecl(const VarDecl &node) {
    if (node.ident == nullptr) { // invalid ast
        return;
    }

    auto context = _ir_module->Context();
    auto alloca = AllocaInst::New(context->GetFloatTy());
    auto symbol = std::make_shared<VariableSymbol>(node.ident->value, alloca,
                                                   node.ident->lineno);

    if (!_cur_scope->addSymbol(symbol)) {
        ErrorReporter::error(node.ident->lineno, "redefine variable " + node.ident->value);
        return;
    }

    // alloca inst should be inserted at the first block
    (*_cur_func->BasicBlockBegin())->InsertInstruction(alloca);
}

void Visitor::visitStmt(const Stmt &node) {
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

void Visitor::visitGetStmt(const GetStmt &node) {
    if (node.ident == nullptr) { // invalid ast
        return;
    }

    auto symbol = _cur_scope->getSymbol(node.ident->value);
    if (symbol == nullptr) {
        ErrorReporter::error(node.ident->lineno, "undefined symbol " + node.ident->value);
        return;
    }
    auto input = InputInst::New(_ir_module->Context());
    _cur_block->InsertInstruction(input);
    auto store = StoreInst::New(input, symbol->value);
    _cur_block->InsertInstruction(store);
}

void Visitor::visitPutStmt(const PutStmt &node) {
    if (node.exp == nullptr) { // invalid ast
        return;
    }
    auto val = visitExp(*node.exp);
    if (val == nullptr) {
        return;
    }
    _cur_block->InsertInstruction(OutputInst::New(val));
}

void Visitor::visitTagStmt(const TagStmt &node) {
    if (node.ident == nullptr) { // invalid ast
        return;
    }

    if (_cur_scope->existInScope(node.ident->value)) {
        auto symbol = _cur_scope->getSymbol(node.ident->value);
        if (symbol->type != SymbolType::Tag) {
            ErrorReporter::error(node.ident->lineno, node.ident->value + " is not a tag");
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
        _cur_scope->addSymbol(symbol);
    }
}

void Visitor::visitLetStmt(const LetStmt &node) {
    if (node.ident == nullptr) { // invalid ast
        return;
    }

    auto symbol = _cur_scope->getSymbol(node.ident->value);
    if (symbol == nullptr) {
        ErrorReporter::error(node.ident->lineno, "undefined symbol " + node.ident->value);
        return;
    }
    if (symbol->type != SymbolType::Variable) {
        ErrorReporter::error(node.ident->lineno, node.ident->value + " is not a variable");
        return;
    }

    auto var_symbol = std::dynamic_pointer_cast<VariableSymbol>(symbol);

    if (node.exp == nullptr) { // invalid ast
        return;
    }
    auto exp_val = visitExp(*node.exp);
    if (exp_val == nullptr) {
        return;
    }

    // get var symbol's addr (alloca inst)
    auto addr = var_symbol->value;

    // store value to addr
    _cur_block->InsertInstruction(StoreInst::New(exp_val, addr));
}

void Visitor::visitIfStmt(const IfStmt &node) {
    if (node.cond == nullptr) { // invalid ast
        return;
    }

    auto val = visitCond(*node.cond);
    if (val == nullptr) {
        return;
    }

    if (_cur_scope->existInScope(node.ident->value)) {
        auto symbol = _cur_scope->getSymbol(node.ident->value);
        if (symbol->type != SymbolType::Tag) {
            ErrorReporter::error(node.ident->lineno, node.ident->value + " is not a tag");
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
        _cur_scope->addSymbol(symbol);
        // add jump inst to symbol
        symbol->jump_insts.push_back(jump);
    }
}

void Visitor::visitToStmt(const ToStmt &node) {
    if (node.ident == nullptr) { // invalid ast
        return;
    }

    if (_cur_scope->existInScope(node.ident->value)) {
        auto symbol = _cur_scope->getSymbol(node.ident->value);
        if (symbol->type != SymbolType::Tag) {
            ErrorReporter::error(node.ident->lineno, node.ident->value + " is not a tag");
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
        _cur_scope->addSymbol(symbol);

        // add jump inst to symbol
        symbol->jump_insts.push_back(jump);
    }
}

ValuePtr Visitor::visitExp(const Exp &node) {
    return std::visit(
        overloaded{
            [this](const BinaryExp &node) { return visitBinaryExp(node); },
            [this](const CallExp &node) { return visitCallExp(node); },
            [this](const UnaryExp &node) { return visitUnaryExp(node); },
            [this](const IdentExp &node) { return visitIdentExp(node); },
            [this](const Number &node) { return visitNumber(node); },
        },
        node);
}

ValuePtr Visitor::visitBinaryExp(const BinaryExp &node) {
    if (node.lhs == nullptr || node.rhs == nullptr) { // invalid ast
        return nullptr;
    }
    auto left_val = visitExp(*node.lhs);
    auto right_val = visitExp(*node.rhs);
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

ValuePtr Visitor::visitCallExp(const CallExp &node) {
    if (node.ident == nullptr) { // invalid ast
        return nullptr;
    }

    auto symbol = _cur_scope->getSymbol(node.ident->value);
    if (symbol == nullptr) {
        ErrorReporter::error(node.ident->lineno, "undefined symbol " + node.ident->value);
        return nullptr;
    }
    if (symbol->type != SymbolType::Function) {
        ErrorReporter::error(node.ident->lineno, node.ident->value + " is not a variable");
        return nullptr;
    }

    auto func_symbol = std::static_pointer_cast<FunctionSymbol>(symbol);

    if (func_symbol->params_count != node.funcRParams->exps.size()) {
        ErrorReporter::error(node.ident->lineno,
              "params number not matched in function call " +
                  node.ident->value);
        return nullptr;
    }
    auto values = visitFuncRParams(*node.funcRParams);
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

ValuePtr Visitor::visitUnaryExp(const UnaryExp &node) {
    if (node.exp == nullptr) { // invalid ast
        return nullptr;
    }
    auto exp_val = visitExp(*node.exp);
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

ValuePtr Visitor::visitIdentExp(const IdentExp &node) {
    if (node.ident == nullptr) { // invalid ast
        return nullptr;
    }

    auto symbol = _cur_scope->getSymbol(node.ident->value);
    if (symbol == nullptr) {
        ErrorReporter::error(node.ident->lineno, "undefined symbol " + node.ident->value);
        return nullptr;
    }
    if (symbol->type != SymbolType::Variable) {
        ErrorReporter::error(node.ident->lineno, node.ident->value + " is not a variable");
        return nullptr;
    }

    // load value from addr
    auto var_symbol = std::static_pointer_cast<VariableSymbol>(symbol);
    auto addr = var_symbol->value;
    auto val = LoadInst::New(addr);
    _cur_block->InsertInstruction(val);

    return val;
}

ValuePtr Visitor::visitNumber(const Number &node) {
    // return const value
    auto val =
        ConstantData::New(_ir_module->Context()->GetFloatTy(), node.value);
    return val;
}

ValuePtr Visitor::visitCond(const Cond &node) {
    if (node.lhs == nullptr || node.rhs == nullptr) { // invalid ast
        return nullptr;
    }
    auto left_val = visitExp(*node.lhs);
    auto right_val = visitExp(*node.rhs);
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
        break; // unreachable
    }

    auto cmp = CompareInstruction::New(ir_op, left_val, right_val);
    _cur_block->InsertInstruction(cmp);

    return cmp;
}

std::vector<ValuePtr> Visitor::visitFuncRParams(const FuncRParams &node) {
    std::vector<ValuePtr> rt;

    for (auto &exp : node.exps) {
        if (exp == nullptr) { // invalid ast
            continue;
        }
        auto exp_val = visitExp(*exp);
        if (exp_val == nullptr) {
            continue;
        }
        rt.push_back(exp_val);
    }

    return rt;
}
