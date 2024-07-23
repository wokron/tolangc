#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

/**
 * An easier implementation of symbol table, ONLY for pcode
 */

struct PcodeSymbol;
using PcodeSymbolPtr = std::shared_ptr<PcodeSymbol>;

struct PcodeSymbol {
    typedef enum {
        P_VAR, 
        P_FUNC
    } SymbolType;

    std::string content;

    SymbolType type;

    PcodeSymbol(const std::string &c, SymbolType t) : 
        content(c), type(t) {}
    
    static PcodeSymbolPtr create(const std::string &c, SymbolType t) {
        return std::make_shared<PcodeSymbol>(c, t);
    }
};

class PcodeSymbolTableNode;
using PcodeSymbolNodePtr = std::shared_ptr<PcodeSymbolTableNode>;

class PcodeSymbolTableNode {
private:
    std::map<std::string, PcodeSymbolPtr> _symbols;
    std::vector<std::string> _symbolNames;
    int _parent;
    std::vector<int> _children;

public:
    PcodeSymbolTableNode(int parent) : _parent(parent) {}

    static PcodeSymbolNodePtr create(int parent) {
        return std::make_shared<PcodeSymbolTableNode>(parent);
    }

    int getParent() const { return _parent; }

    void insertSymbol(const PcodeSymbolPtr &symbol) {
        _symbols.emplace(symbol->content, symbol);
        _symbolNames.push_back(symbol->content);
    }

    void addChild(int child) { _children.push_back(child); }

    PcodeSymbolPtr getSymbol(const std::string &content) const {
        return _symbols.at(content);
    }

    PcodeSymbolPtr searchSymbol(const std::string &content) const {
        auto it = _symbols.find(content);
        if (it != _symbols.end()) {
            return it->second;
        }
        return nullptr;
    }

    int getIndexOf(const std::string &content) const {
        for (int i = 0; i < _symbolNames.size(); i++) {
            if (_symbolNames[i] == content) { 
                return i; 
            }
        }
        return -1;
    }
};

class PcodeSymbolTable;
using PcodeSymbolTablePtr = std::shared_ptr<PcodeSymbolTable>;

class PcodeSymbolTable {
private:
    std::vector<PcodeSymbolNodePtr> _table;
    int _current;
    int _last;

public:
    PcodeSymbolTable();

    void pushScope();

    void popScope();

    void insertSymbol(const PcodeSymbolPtr &symbol) {
        _table[_current]->insertSymbol(symbol);
    }

    PcodeSymbolPtr searchSymbol(const std::string &content) const {
        return _table[_current]->searchSymbol(content);
    }

    bool inFunctionScope() const {
        return _table[_current]->getParent() != -1;
    }

    int getIndexOf(const std::string &content) const {
        return _table[_current]->getIndexOf(content) + 1;
    }
};
