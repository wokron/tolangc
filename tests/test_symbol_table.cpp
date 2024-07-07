#include "doctest.h"
#include "symbol_table.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

TEST_CASE("testing low symbol table") {
    std::shared_ptr<SymbolTable> cur_symbol_table =
        std::make_shared<SymbolTable>();
    std::shared_ptr<Symbol> cur_symbol;

    if (!cur_symbol_table->add_symbol(
            std::make_shared<VariableSymbol>("a", nullptr, 10))) {
        CHECK(1 != 1);
    }

    if (!cur_symbol_table->add_symbol(
            std::make_shared<TagSymbol>("b", nullptr, 20))) {
        CHECK(1 != 1);
    }

    if (!cur_symbol_table->add_symbol(
            std::make_shared<FunctionSymbol>("a", nullptr, 30, 0))) {
        // 重复声明，错误处理
    } else {
        CHECK(1 != 1);
    }

    if ((cur_symbol = cur_symbol_table->get_symbol("a")) != nullptr) {
        CHECK(cur_symbol->name == "a");
        CHECK(cur_symbol->lineno == 10);
    } else {
        // 未声明变量，错误处理
        CHECK(1 != 1);
    }

    if ((cur_symbol = cur_symbol_table->get_symbol("d")) != nullptr) {
        CHECK(1 != 1);
    } else {
        // 未声明变量，错误处理
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    // 创建新符号表
    cur_symbol_table = cur_symbol_table->push_scope();

    if ((cur_symbol = cur_symbol_table->get_symbol("a")) != nullptr) {
        CHECK(cur_symbol->lineno == 10);
    } else {
        CHECK(1 != 1);
    }

    if (!cur_symbol_table->add_symbol(
            std::make_shared<VariableSymbol>("a", nullptr, 40))) {
        CHECK(1 != 1);
    }

    if ((cur_symbol = cur_symbol_table->get_symbol("a")) != nullptr) {
        CHECK(cur_symbol->lineno == 40);
    } else {
        CHECK(1 != 1);
    }

    if (!cur_symbol_table->add_symbol(
            std::make_shared<TagSymbol>("b", nullptr, 50))) {
        CHECK(1 != 1);
    }

    if (!cur_symbol_table->add_symbol(
            std::make_shared<TagSymbol>("b", nullptr, 60))) {
        // 重复声明，错误处理
    } else {
        CHECK(1 != 1);
    }

    // 还原父符号表
    cur_symbol_table = cur_symbol_table->get_father_symbol_table();

    if ((cur_symbol = cur_symbol_table->get_symbol("a")) != nullptr) {
        CHECK(cur_symbol->lineno == 10);
    } else {
        CHECK(1 != 1);
    }
}
