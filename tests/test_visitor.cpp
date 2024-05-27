#include "ast.h"
#include "visitor.h"
#include "llvm/asm/AsmPrinter.h"
#include <doctest.h>
#include <sstream>

/*
The example source file is as follows:

fn add(a, b) => a + b;
var a;
var b;
get a;
get b;
put add(a, b);
*/

static constexpr char EXPECTED[] = R"(; tolang LLVM IR

; Module ID = 'tolang.c'
source_filename = "tolang.c"

declare i32 @get()
declare void @put(i32)


; Function type: i32 (i32, i32)
define dso_local i32 @add(i32 %0, i32 %1) {
    %3 = alloca i32
    store i32 %0, i32* %3
    %4 = alloca i32
    store i32 %1, i32* %4
    %5 = load i32, i32* %3
    %6 = load i32, i32* %4
    %7 = add nsw i32 %5, %6
    ret i32 %7
}

; Function type: void ()
define dso_local void @main() {
    %1 = alloca i32
    %2 = alloca i32
    %3 = call i32 @get()
    %4 = call i32 @get()
}

; End of LLVM IR
)";

TEST_CASE("testing visitor") {
    // first, we create the AST
    auto root = std::make_shared<CompUnit>();

    auto add = std::make_shared<FuncDef>();
    add->ident = "add";
    add->funcFParams = std::make_shared<FuncFParams>();
    add->funcFParams->idents.push_back("a");
    add->funcFParams->idents.push_back("b");

    auto add_exp = BinaryExp();
    add_exp.lexp = std::make_shared<Exp>(Ident("a"));
    add_exp.rexp = std::make_shared<Exp>(Ident("b"));
    add->exp = std::make_shared<Exp>(add_exp);

    root->funcDefs.push_back(add);

    auto var_a = std::make_shared<VarDecl>();
    var_a->ident = "a";
    root->varDecls.push_back(var_a);

    auto var_b = std::make_shared<VarDecl>();
    var_b->ident = "b";
    root->varDecls.push_back(var_b);

    auto get_a = GetStmt();
    get_a.ident = "a";
    root->stmts.push_back(std::make_shared<Stmt>(get_a));

    auto get_b = GetStmt();
    get_b.ident = "b";
    root->stmts.push_back(std::make_shared<Stmt>(get_b));

    // then, we use visitor to generate the LLVM IR
    ModulePtr module = Module::New("tolang.c");
    auto visitor = Visitor(module);
    visitor.visit(*root);

    AsmPrinter printer;

    std::ostringstream ss;
    printer.Print(module, ss);

    auto ir = ss.str();
    CHECK_EQ(ir, EXPECTED);
}