#include "visitor.h"
#include "error.h"
#include "utils.h"

void Visitor::visit(const CompUnit &node) {
    for (auto &elm : node.funcDefs) {
        visitFuncDef(*elm);
    }

    // TODO: IR: create main function

    _cur_scope = _cur_scope->pushScope();
    for (auto &elm : node.varDecls) {
        visitVarDecl(*elm);
    }
    for (auto &elm : node.stmts) {
        visitStmt(*elm);
    }
    _cur_scope = _cur_scope->popScope();
}

void Visitor::visitFuncDef(const FuncDef &node) {
    auto param_symbols = visitFuncFParams(*node.funcFParams);

    auto symbol = std::make_shared<FunctionSymbol>(node.ident, node.line,
                                                   param_symbols.size());

    if (!_cur_scope->addSymbol(symbol)) {
        error(node.line, "redefine function " + node.ident);
    }

    // TODO: IR: create function

    _cur_scope = _cur_scope->pushScope();

    for (auto &param_symbol : param_symbols) {
        if (!_cur_scope->addSymbol(param_symbol)) {
            error(param_symbol->line_number,
                  "redefine parameter " + param_symbol->name);
        }
    }

    _cur_scope = _cur_scope->popScope();
}

std::vector<std::shared_ptr<VariableSymbol>>
Visitor::visitFuncFParams(const FuncFParams &node) {
    std::vector<std::shared_ptr<VariableSymbol>> rt;
    for (auto &elm : node.idents) {
        rt.push_back(std::make_shared<VariableSymbol>(elm, node.line));
    }
    return rt;
}

void Visitor::visitVarDecl(const VarDecl &node) {
    auto symbol = std::make_shared<VariableSymbol>(node.ident, node.line);

    if (!_cur_scope->addSymbol(symbol)) {
        error(symbol->line_number, "redefine variable " + node.ident);
    }
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
    // TODO: IR: input builtin
}

void Visitor::visitPutStmt(const PutStmt &node) {
    // TODO: IR: output builtin
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

        auto symbol = std::make_shared<TagSymbol>(node.ident, node.line);
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

    // TODO: IR: store value to addr
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
        auto symbol = std::make_shared<TagSymbol>(node.ident, -1);
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
        auto symbol = std::make_shared<TagSymbol>(node.ident, -1);
        _cur_scope->addSymbol(symbol);
    }
}

std::shared_ptr<Value> Visitor::visitExp(const Exp &node) {
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

std::shared_ptr<Value> Visitor::visitBinaryExp(const BinaryExp &node) {
    auto left_val = visitExp(*node.lexp);
    auto right_val = visitExp(*node.rexp);

    // TODO: IR: binary inst

    return nullptr;
}

std::shared_ptr<Value> Visitor::visitCallExp(const CallExp &node) {
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

    // TODO: IR: call inst

    return nullptr;
}

std::shared_ptr<Value> Visitor::visitUnaryExp(const UnaryExp &node) {
    auto val = visitExp(*node.exp);

    // TODO: IR: binary inst

    return val;
}

std::shared_ptr<Value> Visitor::visitIdent(const Ident &node) {
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

    // TODO: IR: load value from addr

    return nullptr;
}

std::shared_ptr<Value> Visitor::visitNumber(const Number &node) {
    // TODO: IR: return const value
    return nullptr;
}

std::shared_ptr<Value> Visitor::visitCond(const Cond &node) {
    auto left_val = visitExp(*node.left);
    auto right_val = visitExp(*node.right);

    // TODO: IR: compare left and right

    return nullptr;
}

std::vector<std::shared_ptr<Value>>
Visitor::visitFuncRParams(const FuncRParams &node) {
    std::vector<std::shared_ptr<Value>> rt;

    for (auto &elm : node.exps) {
        rt.push_back(visitExp(*elm));
    }

    return rt;
}
