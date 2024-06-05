#pragma once

#include "llvm/ir/Llvm.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

enum SymbolType { Variable, Tag, Function };

struct Symbol {
    SymbolType type;
    std::string name;
    int line_number;
    ValuePtr value;

    virtual ~Symbol() = default;

    Symbol(SymbolType type, std::string name, ValuePtr value, int line_number)
        : type(type), name(name), value(value), line_number(line_number) {}

    std::string printSymbol() const {
        return name + " " + std::to_string(line_number) + "\n";
    }
};

struct VariableSymbol : public Symbol {
    VariableSymbol(std::string name, ValuePtr value, int line_number)
        : Symbol(SymbolType::Variable, name, value, line_number) {}
};

struct TagSymbol : public Symbol {
    BasicBlockPtr target;
    std::vector<InstructionPtr> jump_insts;

    TagSymbol(std::string name, ValuePtr value, int line_number)
        : Symbol(SymbolType::Tag, name, value, line_number) {}
};

struct FunctionSymbol : public Symbol {
    int params_count = 0;

    FunctionSymbol(std::string name, ValuePtr value, int line_number,
                   int params_count)
        : Symbol(SymbolType::Function, name, value, line_number),
          params_count(params_count) {}
};

class SymbolTable : public std::enable_shared_from_this<SymbolTable> {
  private:
    std::unordered_map<std::string, std::shared_ptr<Symbol>> _symbols;
    std::shared_ptr<SymbolTable> _father;

  public:
    SymbolTable() : _father(nullptr) {}

    SymbolTable(std::shared_ptr<SymbolTable> father) : _father(father) {}

    std::shared_ptr<SymbolTable> getFatherSymbolTable() { return _father; }

    std::string printSymbolTable() {
        std::string output;
        for (const auto &kv : _symbols) {
            output += kv.second->printSymbol();
        }
        return output;
    }

    bool existInScope(const std::string &name) {
        return _symbols.find(name) != _symbols.end();
    }

    bool addSymbol(std::shared_ptr<Symbol> symbol) {
        if (existInScope(symbol->name)) {
            return false;
        }

        _symbols[symbol->name] = symbol;
        return true;
    }

    std::shared_ptr<Symbol> getSymbol(const std::string &name) {
        auto it = _symbols.find(name);
        if (it != _symbols.end()) {
            return it->second;
        } else if (_father != nullptr) {
            return _father->getSymbol(name);
        } else {
            return nullptr;
        }
    }

    std::shared_ptr<SymbolTable> pushScope() {
        return std::make_shared<SymbolTable>(shared_from_this());
    }

    std::shared_ptr<SymbolTable> popScope() { return _father; }
};
