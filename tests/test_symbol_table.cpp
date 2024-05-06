#include "doctest.h"
#include "symbol_table.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

TEST_CASE("testing low symbol table") {
    // 创建父符号表
    std::shared_ptr<SymbolTableManager> symbol_table_manager =
        std::make_shared<SymbolTableManager>();
    std::shared_ptr<SymbolTable> cur_symbol_table;
    std::shared_ptr<Symbol> cur_symbol;
    cur_symbol_table = symbol_table_manager->createSymbolTable();
    //    CHECK(cur_symbol_table);
    //    CHECK(symbol_table_manager);
    if (!cur_symbol_table->addSymbol(
            std::make_shared<VariableSymbol>("a", 10))) {
        CHECK(1 != 1);
    }

    if (!cur_symbol_table->addSymbol(std::make_shared<TagSymbol>("b", 20))) {
        CHECK(1 != 1);
    }

    if (!cur_symbol_table->addSymbol(
            std::make_shared<FunctionSymbol>("a", 30))) {
        // 重复声明，错误处理
    } else {
        CHECK(1 != 1);
    }

    if ((cur_symbol = cur_symbol_table->getSymbol("a")) != nullptr) {
        CHECK(cur_symbol->getName() == "a");
        CHECK(cur_symbol->getLineNumber() == 10);
    } else {
        // 未声明变量，错误处理
        CHECK(1 != 1);
    }

    if ((cur_symbol = cur_symbol_table->getSymbol("d")) != nullptr) {
        CHECK(1 != 1);
    } else {
        // 未声明变量，错误处理
    }

    CHECK(cur_symbol_table->printSymbolTable() == "a 10\nb 20\n");

    ///////////////////////////////////////////////////////////////////////////////////////
    // 创建新符号表
    cur_symbol_table =
        symbol_table_manager->createSymbolTable(cur_symbol_table);

    if ((cur_symbol = cur_symbol_table->getSymbol("a")) != nullptr) {
        CHECK(cur_symbol->getLineNumber() == 10);
    } else {
        CHECK(1 != 1);
    }

    if (!cur_symbol_table->addSymbol(
            std::make_shared<VariableSymbol>("a", 40))) {
        CHECK(1 != 1);
    }

    if ((cur_symbol = cur_symbol_table->getSymbol("a")) != nullptr) {
        CHECK(cur_symbol->getLineNumber() == 40);
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
    CHECK(cur_symbol_table->printSymbolTable() == "a 40\nb 50\n");

    // 还原父符号表
    cur_symbol_table = cur_symbol_table->getFatherSymbolTable();

    if ((cur_symbol = cur_symbol_table->getSymbol("a")) != nullptr) {
        CHECK(cur_symbol->getLineNumber() == 10);
    } else {
        CHECK(1 != 1);
    }

    CHECK(cur_symbol_table->printSymbolTable() == "a 10\nb 20\n");

    CHECK(symbol_table_manager->printAllSymbolTable() ==
          "a 10\nb 20\n======\na 40\nb 50\n======\n");
}
