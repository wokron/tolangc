#pragma once

#include "utils.h"

#if TOLANG_BACKEND == LLVM

#include "llvm/ir/Llvm.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

enum SymbolType { VAR, TAG, FUNC };

/**
 * @brief Convert a symbol type to a string.
 * @param type The symbol type.
 * @return The string representation of the symbol type.
 */
inline std::string symbol_type_to_string(SymbolType type) {
    switch (type) {
    case SymbolType::VAR:
        return "variable";
    case SymbolType::TAG:
        return "tag";
    case SymbolType::FUNC:
        return "function";
    default:
        throw std::runtime_error("unknown symbol type");
    }
}

/**
 * @brief `Symbol` is a class for different types of symbols, which are
 * used in the symbol table.
 * @note This is a base class for different types of symbols.
 */
struct Symbol {
    SymbolType type;
    std::string name;
    int lineno;
    ValuePtr value;

    virtual ~Symbol() = default;

    Symbol(SymbolType type, std::string name, ValuePtr value, int lineno)
        : type(type), name(name), value(value), lineno(lineno) {}
};

/**
 * @brief A symbol representing a variable.
 */
struct VariableSymbol : public Symbol {
    VariableSymbol(std::string name, ValuePtr value, int lineno)
        : Symbol(SymbolType::VAR, name, value, lineno) {}
};

/**
 * @brief A symbol representing a tag.
 */
struct TagSymbol : public Symbol {
    BasicBlockPtr target;
    std::vector<InstructionPtr> jump_insts;

    TagSymbol(std::string name, ValuePtr value, int lineno)
        : Symbol(SymbolType::TAG, name, value, lineno) {}
};

/**
 * @brief A symbol representing a function.
 */
struct FunctionSymbol : public Symbol {
    int params_count = 0;

    FunctionSymbol(std::string name, ValuePtr value, int lineno,
                   int params_count)
        : Symbol(SymbolType::FUNC, name, value, lineno),
          params_count(params_count) {}
};

/**
 * @brief A symbol table, which is a map from symbol names to symbols.
 * @note This is a recursive data structure, where each symbol table has a
 * pointer to its parent symbol table.
 */
class SymbolTable : public std::enable_shared_from_this<SymbolTable> {
public:
    SymbolTable() : _father(nullptr) {}

    /**
     * @brief Check if a symbol with the given name exists in the current scope.
     * @param name The name of the symbol.
     * @return `true` if the symbol exists, `false` otherwise.
     */
    bool exist_in_scope(const std::string &name) {
        return _symbols.find(name) != _symbols.end();
    }

    /**
     * @brief Add a symbol to the current scope.
     * @param symbol The symbol to be added.
     * @return `true` if the symbol is added successfully, `false` if a symbol
     * with the same name already exists in the current scope.
     */
    bool add_symbol(std::shared_ptr<Symbol> symbol) {
        if (exist_in_scope(symbol->name)) {
            return false;
        }

        _symbols[symbol->name] = symbol;
        return true;
    }

    /**
     * @brief Get a symbol with the given name.
     * @param name The name of the symbol.
     * @return The symbol if it exists, `nullptr` otherwise.
     */
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

    /**
     * @brief Create a new scope, which means a new symbol table will be
     * created. And the current symbol table will be the father of the new
     * symbol table.
     * @return The new symbol table.
     */
    std::shared_ptr<SymbolTable> push_scope() {
        auto new_table = std::make_shared<SymbolTable>();
        new_table->_father = shared_from_this();
        return new_table;
    }

    /**
     * @brief Pop the current scope, which means the father of the current
     * symbol table will be returned.
     * @return The father symbol table.
     * @note Since we use `shared_ptr`, the current symbol table will be
     * automatically deleted if there is no other reference to it.
     */
    std::shared_ptr<SymbolTable> pop_scope() { return _father; }

private:
    std::unordered_map<std::string, std::shared_ptr<Symbol>> _symbols;

    // The father symbol table, `nullptr` if this is the global symbol table.
    std::shared_ptr<SymbolTable> _father;
};

#endif