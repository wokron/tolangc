#include "doctest.h"
#include "symbol_table.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

TEST_CASE("testing low symbol table") {
    std::shared_ptr<SymbolTable> cur_symbol_table = std::make_shared<SymbolTable>();
    std::shared_ptr<Symbol> cur_symbol;

    if (!cur_symbol_table->addSymbol(
            std::make_shared<VariableSymbol>("a", 10))) {
        CHECK(1 != 1);
    }

    if (!cur_symbol_table->addSymbol(std::make_shared<TagSymbol>("b", 20))) {
        CHECK(1 != 1);
    }

    if (!cur_symbol_table->addSymbol(
            std::make_shared<FunctionSymbol>("a", 30, 0))) {
        // 重复声明，错误处理
    } else {
        CHECK(1 != 1);
    }

    if ((cur_symbol = cur_symbol_table->getSymbol("a")) != nullptr) {
        CHECK(cur_symbol->name == "a");
        CHECK(cur_symbol->line_number == 10);
    } else {
        // 未声明变量，错误处理
        CHECK(1 != 1);
    }

    if ((cur_symbol = cur_symbol_table->getSymbol("d")) != nullptr) {
        CHECK(1 != 1);
    } else {
        // 未声明变量，错误处理
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    // 创建新符号表
    cur_symbol_table = cur_symbol_table->pushScope();

    if ((cur_symbol = cur_symbol_table->getSymbol("a")) != nullptr) {
        CHECK(cur_symbol->line_number == 10);
    } else {
        CHECK(1 != 1);
    }

    if (!cur_symbol_table->addSymbol(
            std::make_shared<VariableSymbol>("a", 40))) {
        CHECK(1 != 1);
    }

    if ((cur_symbol = cur_symbol_table->getSymbol("a")) != nullptr) {
        CHECK(cur_symbol->line_number == 40);
    } else {
        CHECK(1 != 1);
    }

    if (!cur_symbol_table->addSymbol(std::make_shared<TagSymbol>("b", 50))) {
        CHECK(1 != 1);
    }

    if (!cur_symbol_table->addSymbol(std::make_shared<TagSymbol>("b", 60))) {
        // 重复声明，错误处理
    } else {
        CHECK(1 != 1);
    }

    // 还原父符号表
    cur_symbol_table = cur_symbol_table->getFatherSymbolTable();

    if ((cur_symbol = cur_symbol_table->getSymbol("a")) != nullptr) {
        CHECK(cur_symbol->line_number == 10);
    } else {
        CHECK(1 != 1);
    }
}
