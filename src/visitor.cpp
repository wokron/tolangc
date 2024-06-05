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
        visitStmt(*elm);
    }
    _cur_scope = _cur_scope->popScope();

    _cur_block->InsertInstruction(
        ReturnInst::New(ConstantData::New(context->GetInt32Ty(), 0)));

    _cur_block = nullptr;
    _cur_func = nullptr;
}

void Visitor::visitFuncDef(const FuncDef &node) {
    auto param_symbols = visitFuncFParams(*node.funcFParams);

    // create ir function
    auto context = _ir_module->Context();
    std::vector<ArgumentPtr> args;
    for (auto &param_symbol : param_symbols) {
        auto arg = Argument::New(context->GetFloatTy(), param_symbol->name);
        args.push_back(arg);
        param_symbol->value = arg;
    }
    _cur_func = Function::New(context->GetFloatTy(), node.ident, args);

    // create function symbol
    auto symbol = std::make_shared<FunctionSymbol>(
        node.ident, _cur_func, node.line, param_symbols.size());

    // try to add function symbol to current scope
    if (!_cur_scope->addSymbol(symbol)) {
        error(node.line, "redefine function " + node.ident);
    }

    // if success, add function to module
    _ir_module->AddFunction(_cur_func);

    // then create entry block
    _cur_block = _cur_func->NewBasicBlock();

    // new scope for function
    _cur_scope = _cur_scope->pushScope();

    for (auto &param_symbol : param_symbols) {
        if (!_cur_scope->addSymbol(param_symbol)) {
            error(param_symbol->line_number,
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
    auto val = visitExp(*node.exp);
    _cur_block->InsertInstruction(ReturnInst::New(val));

    _cur_scope = _cur_scope->popScope();

    _cur_block = nullptr;
    _cur_func = nullptr;
}

std::vector<std::shared_ptr<VariableSymbol>>
Visitor::visitFuncFParams(const FuncFParams &node) {
    std::vector<std::shared_ptr<VariableSymbol>> rt;
    for (auto &elm : node.idents) {
        // here we don't create ir argument, just create symbol
        rt.push_back(std::make_shared<VariableSymbol>(elm, nullptr, node.line));
    }
    return rt;
}

void Visitor::visitVarDecl(const VarDecl &node) {
    auto context = _ir_module->Context();
    auto alloca = AllocaInst::New(context->GetFloatTy());
    auto symbol =
        std::make_shared<VariableSymbol>(node.ident, alloca, node.line);

    if (!_cur_scope->addSymbol(symbol)) {
        error(symbol->line_number, "redefine variable " + node.ident);
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
    auto symbol = _cur_scope->getSymbol(node.ident);
    if (!symbol) {
        error(node.line, "undefined symbol " + node.ident);
    }
    auto input = InputInst::New(_ir_module->Context());
    _cur_block->InsertInstruction(input);
    auto store = StoreInst::New(input, symbol->value);
    _cur_block->InsertInstruction(store);
}

void Visitor::visitPutStmt(const PutStmt &node) {
    auto val = visitExp(*node.exp);
    _cur_block->InsertInstruction(OutputInst::New(val));
}

void Visitor::visitTagStmt(const TagStmt &node) {
    if (_cur_scope->existInScope(node.ident)) {
        auto symbol = _cur_scope->getSymbol(node.ident);
        if (symbol->type != SymbolType::Tag) {
            error(node.line, node.ident + " is not a tag");
            return;
        }

        auto tag_symbol = std::dynamic_pointer_cast<TagSymbol>(symbol);

        // TODO: IR: create new label

        // TODO: backpatching all jump inst before tag

    } else {
        // TODO: IR: create new label

        auto symbol =
            std::make_shared<TagSymbol>(node.ident, nullptr, node.line);
        _cur_scope->addSymbol(symbol);
    }
}

void Visitor::visitLetStmt(const LetStmt &node) {
    auto symbol = _cur_scope->getSymbol(node.ident);
    if (!symbol) {
        error(node.line, "undefined symbol " + node.ident);
    }
    if (symbol->type != SymbolType::Variable) {
        error(node.line, node.ident + " is not a variable");
        return;
    }

    auto var_symbol = std::dynamic_pointer_cast<VariableSymbol>(symbol);

    auto exp_val = visitExp(*node.exp);

    // get var symbol's addr (alloca inst)
    auto addr = var_symbol->value;

    // store value to addr
    _cur_block->InsertInstruction(StoreInst::New(exp_val, addr));
}

void Visitor::visitIfStmt(const IfStmt &node) {
    auto val = visitCond(*node.cond);

    if (_cur_scope->existInScope(node.ident)) {
        auto symbol = _cur_scope->getSymbol(node.ident);
        if (symbol->type != SymbolType::Tag) {
            error(node.line, node.ident + " is not a tag");
            return;
        }

        // TODO: IR: jump inst

        // TODO: if the tag is not declared, add jump inst to symbol.
        // otherwise set the target of jump to tag

    } else {
        // TODO: IR: jump inst

        // TODO: add jump inst to symbol
        auto symbol = std::make_shared<TagSymbol>(node.ident, nullptr, -1);
        _cur_scope->addSymbol(symbol);
    }
}

void Visitor::visitToStmt(const ToStmt &node) {
    if (_cur_scope->existInScope(node.ident)) {
        auto symbol = _cur_scope->getSymbol(node.ident);
        if (symbol->type != SymbolType::Tag) {
            error(node.line, node.ident + " is not a tag");
            return;
        }

        // TODO: IR: jump inst

        // TODO: if the tag is not declared, add jump inst to symbol.
        // otherwise set the target of jump to tag

    } else {
        // TODO: IR: jump inst

        // TODO: add jump inst to symbol
        auto symbol = std::make_shared<TagSymbol>(node.ident, nullptr, -1);
        _cur_scope->addSymbol(symbol);
    }
}

ValuePtr Visitor::visitExp(const Exp &node) {
    return std::visit(
        overloaded{
            [this](const BinaryExp &node) { return visitBinaryExp(node); },
            [this](const CallExp &node) { return visitCallExp(node); },
            [this](const UnaryExp &node) { return visitUnaryExp(node); },
            [this](const Ident &node) { return visitIdent(node); },
            [this](const Number &node) { return visitNumber(node); },
        },
        node);
}

ValuePtr Visitor::visitBinaryExp(const BinaryExp &node) {
    auto left_val = visitExp(*node.lexp);
    auto right_val = visitExp(*node.rexp);

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
    auto symbol = _cur_scope->getSymbol(node.ident);
    if (!symbol) {
        error(node.line, "undefined symbol " + node.ident);
    }
    if (symbol->type != SymbolType::Function) {
        error(node.line, node.ident + " is not a variable");
        return nullptr;
    }

    auto func_symbol = std::static_pointer_cast<FunctionSymbol>(symbol);

    auto values = visitFuncRParams(*node.funcRParams);
    if (func_symbol->params_count != values.size()) {
        error(node.line,
              "params number not matched in function call " + node.ident);
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
    auto exp_val = visitExp(*node.exp);

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

ValuePtr Visitor::visitIdent(const Ident &node) {
    auto symbol = _cur_scope->getSymbol(node);
    if (!symbol) {
        error(-1, "undefined symbol " +
                      node); // TODO: Ident should record lineno too
        return nullptr;
    }
    if (symbol->type != SymbolType::Variable) {
        error(-1, node + " is not a variable");
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
    auto left_val = visitExp(*node.left);
    auto right_val = visitExp(*node.right);

    CompareOpType ir_op;
    switch (node.op) {
    case Cond::EQL:
        ir_op = CompareOpType::Equal;
        break;
    case Cond::NEQ:
        ir_op = CompareOpType::NotEqual;
        break;
    case Cond::LSS:
        ir_op = CompareOpType::LessThan;
        break;
    case Cond::LEQ:
        ir_op = CompareOpType::LessThanOrEqual;
        break;
    case Cond::GRE:
        ir_op = CompareOpType::GreaterThan;
        break;
    case Cond::GEQ:
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

    for (auto &elm : node.exps) {
        rt.push_back(visitExp(*elm));
    }

    return rt;
}
