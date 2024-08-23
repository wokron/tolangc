#include "tolang/utils.h"

#if TOLANG_BACKEND == LLVM

#include "doctest.h"
#include "tolang/symtable.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

TEST_CASE("testing low symbol table") {
    std::shared_ptr<SymbolTable> cur_table = std::make_shared<SymbolTable>();
    std::shared_ptr<Symbol> cur_symbol;

    CHECK(cur_table->add_symbol(
        std::make_shared<VariableSymbol>("a", nullptr, 10)));

    CHECK(cur_table->add_symbol(
        std::make_shared<VariableSymbol>("b", nullptr, 20)));

    CHECK_FALSE(cur_table->add_symbol(
        std::make_shared<VariableSymbol>("b", nullptr, 30)));

    CHECK((cur_symbol = cur_table->get_symbol("a")) != nullptr);
    CHECK(cur_symbol->name == "a");
    CHECK(cur_symbol->lineno == 10);

    CHECK((cur_symbol = cur_table->get_symbol("d")) == nullptr);

    // new scope
    cur_table = cur_table->push_scope();

    CHECK((cur_symbol = cur_table->get_symbol("a")) != nullptr);
    CHECK(cur_symbol->name == "a");

    CHECK(cur_table->add_symbol(
        std::make_shared<VariableSymbol>("a", nullptr, 40)));

    CHECK((cur_symbol = cur_table->get_symbol("a")) != nullptr);
    CHECK(cur_symbol->lineno == 40);

    CHECK(cur_table->add_symbol(std::make_shared<TagSymbol>("b", nullptr, 50)));

    CHECK_FALSE(
        cur_table->add_symbol(std::make_shared<TagSymbol>("b", nullptr, 60)));

    // pop scope
    cur_table = cur_table->pop_scope();

    CHECK((cur_symbol = cur_table->get_symbol("a")) != nullptr);
    CHECK(cur_symbol->lineno == 10);
}

#endif
