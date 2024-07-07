#pragma once

#include "llvm/ir/Llvm.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

enum SymbolType { VAR, TAG, FUNC };

struct Symbol {
    SymbolType type;
    std::string name;
    int lineno;
    ValuePtr value;

    virtual ~Symbol() = default;

    Symbol(SymbolType type, std::string name, ValuePtr value, int line_number)
        : type(type), name(name), value(value), lineno(line_number) {}

    std::string print_symbol() const {
        return name + " " + std::to_string(lineno) + "\n";
    }
};

struct VariableSymbol : public Symbol {
    VariableSymbol(std::string name, ValuePtr value, int line_number)
        : Symbol(SymbolType::VAR, name, value, line_number) {}
};

struct TagSymbol : public Symbol {
    BasicBlockPtr target;
    std::vector<InstructionPtr> jump_insts;

    TagSymbol(std::string name, ValuePtr value, int line_number)
        : Symbol(SymbolType::TAG, name, value, line_number) {}
};

struct FunctionSymbol : public Symbol {
    int params_count = 0;

    FunctionSymbol(std::string name, ValuePtr value, int line_number,
                   int params_count)
        : Symbol(SymbolType::FUNC, name, value, line_number),
          params_count(params_count) {}
};

class SymbolTable : public std::enable_shared_from_this<SymbolTable> {
public:
    SymbolTable() : _father(nullptr) {}

    SymbolTable(std::shared_ptr<SymbolTable> father) : _father(father) {}

    std::shared_ptr<SymbolTable> get_father_symbol_table() { return _father; }

    std::string print_symbol_table() {
        std::string output;
        for (const auto &kv : _symbols) {
            output += kv.second->print_symbol();
        }
        return output;
    }

    bool exist_in_scope(const std::string &name) {
        return _symbols.find(name) != _symbols.end();
    }

    bool add_symbol(std::shared_ptr<Symbol> symbol) {
        if (exist_in_scope(symbol->name)) {
            return false;
        }

        _symbols[symbol->name] = symbol;
        return true;
    }

    std::shared_ptr<Symbol> get_symbol(const std::string &name) {
        auto it = _symbols.find(name);
        if (it != _symbols.end()) {
            return it->second;
        } else if (_father != nullptr) {
            return _father->get_symbol(name);
        } else {
            return nullptr;
        }
    }

    std::shared_ptr<SymbolTable> push_scope() {
        return std::make_shared<SymbolTable>(shared_from_this());
    }

    std::shared_ptr<SymbolTable> pop_scope() { return _father; }

private:
    std::unordered_map<std::string, std::shared_ptr<Symbol>> _symbols;
    std::shared_ptr<SymbolTable> _father;
};
