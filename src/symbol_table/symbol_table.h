//
// Created by 86166 on 2024/5/5.
//

#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

enum SymbolType {
    Variable,
    Tag,
    Function
};

// 符号类
class Symbol {
private:
    SymbolType type;     // 符号类型
    std::string name;   // 符号名
    int line_number;     // 符号的行号

public:
    virtual ~Symbol() = default;

    Symbol(SymbolType type, std::string name, int line_number)
            : type(type), name(name), line_number(line_number) {}

    int getLineNumber() const {
        return line_number;
    }

    std::string getName() {
        return name;
    }

    SymbolType getType() {
        return type;
    }

    std::string printSymbol() {
        return name + " " + std::to_string(line_number) + "\n";
    }
};

// 派生类 VariableSymbol
class VariableSymbol : public Symbol {
private:

public:
    VariableSymbol(std::string name, int line_number)
            : Symbol(SymbolType::Variable, name, line_number) {}
};

// 派生类 TagSymbol
class TagSymbol : public Symbol {

public:
    TagSymbol(std::string name, int line_number)
            : Symbol(SymbolType::Tag, name, line_number) {}
};

// 派生类 FunctionSymbol
class FunctionSymbol : public Symbol {
private:
    std::vector<SymbolType> params; // 函数形参类型列表

public:
    FunctionSymbol(std::string name, int line_number)
            : Symbol(SymbolType::Function, name, line_number) {}

    void addParams(SymbolType symbolType) {
        params.push_back(symbolType);
    }

    int getParamsLen() {
        return params.size();
    }
};

// 符号表类
class SymbolTable {
private:
    std::vector<std::shared_ptr<Symbol>> symbols; // 使用vector存储符号
    std::shared_ptr<SymbolTable> father_symbol_table;      // 父符号表

public:
    SymbolTable() : father_symbol_table(nullptr) {}

    SymbolTable(std::shared_ptr<SymbolTable> father_symbol_table)
            : father_symbol_table(father_symbol_table) {}

    // 获取父符号表
    std::shared_ptr<SymbolTable> getFatherSymbolTable() {
        return father_symbol_table;
    }

    std::string printSymbolTable() {
        std::string output;
        for (const auto& symbol : symbols) {
            output += symbol->printSymbol();
        }
        return output;
    }

    // 添加符号到符号表
    bool addSymbol(std::shared_ptr<Symbol> symbol) {
        // 检查符号是否已存在
        for (const auto& existing_symbol : symbols) {
            if (existing_symbol->getName() == symbol->getName()) {
                return false;
            }
        }
        symbols.push_back(symbol);
        return true;
    }

    // 查找符号，递归向父符号表查找
    std::shared_ptr<Symbol> getSymbol(const std::string& name) {
        for (const auto& symbol : symbols) {
            if (symbol->getName() == name) {
                return symbol;
            }
        }
        // 如果当前符号表中未找到，递归在父符号表中查找
        if (father_symbol_table) {
            return father_symbol_table->getSymbol(name);
        }
        // 如果没有找到，返回空指针
        return nullptr;
    }
};

// 符号管理器类
class SymbolTableManager {
private:
    std::vector<std::shared_ptr<SymbolTable>> symbol_tables; // 全局符号表

public:
    SymbolTableManager() = default;

    // 创建新的局部符号表，并关联到父符号表
    std::shared_ptr<SymbolTable> createSymbolTable(std::shared_ptr<SymbolTable> father_symbol_table) {
        std::shared_ptr<SymbolTable> st = std::make_shared<SymbolTable>(father_symbol_table);
        symbol_tables.push_back(st);
        return st;
    }

    std::shared_ptr<SymbolTable> createSymbolTable() {
        std::shared_ptr<SymbolTable> st = std::make_shared<SymbolTable>();
        symbol_tables.push_back(st);
        return st;
    }

    std::string printAllSymbolTable() {
        std::string output;
        for (const auto& symbol_table : symbol_tables) {
            output += symbol_table->printSymbolTable();
            output += "======\n";
        }
        return output;
    }
};