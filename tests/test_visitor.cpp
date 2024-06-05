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

declare float @get()
declare void @put(float)


; Function type: float (float, float)
define dso_local float @add(float %0, float %1) {
    %3 = alloca float
    store float %0, float* %3
    %4 = alloca float
    store float %1, float* %4
    %5 = load float, float* %3
    %6 = load float, float* %4
    %7 = fadd float %5, %6
    ret float %7
}

; Function type: i32 ()
define dso_local i32 @main() {
    %1 = alloca float
    %2 = alloca float
    %3 = call float @get()
    store float %3, float* %1
    %4 = call float @get()
    store float %4, float* %2
    %5 = load float, float* %1
    %6 = load float, float* %2
    %7 = call float @add(float %5, float %6)
    call void @put(float %7)
    ret i32 0
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

    auto put_add = PutStmt();
    auto add_call = CallExp();
    add_call.ident = "add";
    add_call.funcRParams = std::make_shared<FuncRParams>();
    add_call.funcRParams->exps.push_back(std::make_shared<Exp>(Ident("a")));
    add_call.funcRParams->exps.push_back(std::make_shared<Exp>(Ident("b")));
    put_add.exp = std::make_shared<Exp>(add_call);
    root->stmts.push_back(std::make_shared<Stmt>(put_add));

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