#include "ast.h"
#include "visitor.h"
#include "llvm/asm/AsmPrinter.h"
#include <doctest.h>
#include <sstream>

/*
The example source file is as follows:

fn add(a, b) => a + b;

var n;
var i;
var a;
var b;

get n;
let i = 0;

tag cond;
if i >= n to done;
tag do;

get a;
get b;
put add(a, b);

let i = i + 1;
to cond;
tag done;
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
    %3 = alloca float
    %4 = alloca float
    %5 = call float @get()
    store float %5, float* %1
    store float 0.000000, float* %2
    br label %6
6:                                                ; preds = %0, %11
    %7 = load float, float* %2
    %8 = load float, float* %1
    %9 = fcmp oge float %7, %8
    br i1 %9, label %20, label %10
10:                                               ; preds = %6
    br label %11
11:                                               ; preds = %10
    %12 = call float @get()
    store float %12, float* %3
    %13 = call float @get()
    store float %13, float* %4
    %14 = load float, float* %3
    %15 = load float, float* %4
    %16 = call float @add(float %14, float %15)
    call void @put(float %16)
    %17 = load float, float* %2
    %18 = fadd float %17, 1.000000
    store float %18, float* %2
    br label %6
19:
    br label %20
20:                                               ; preds = %6, %19
    ret i32 0
}

; End of LLVM IR
)";

std::shared_ptr<CompUnit> build_ast();

TEST_CASE("testing visitor") {
    // first, we create the AST
    auto root = build_ast();

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

std::shared_ptr<CompUnit> build_ast() {
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

    // var n;
    auto var_n = std::make_shared<VarDecl>();
    var_n->ident = "n";
    root->varDecls.push_back(var_n);

    // var i;
    auto var_i = std::make_shared<VarDecl>();
    var_i->ident = "i";
    root->varDecls.push_back(var_i);

    // var a;
    auto var_a = std::make_shared<VarDecl>();
    var_a->ident = "a";
    root->varDecls.push_back(var_a);

    // var b;
    auto var_b = std::make_shared<VarDecl>();
    var_b->ident = "b";
    root->varDecls.push_back(var_b);

    // get n;
    auto get_n = GetStmt();
    get_n.ident = "n";
    root->stmts.push_back(std::make_shared<Stmt>(get_n));

    // let i = 1;
    auto let_i = LetStmt();
    let_i.ident = "i";
    auto zero = Number();
    zero.value = 0.0;
    let_i.exp = std::make_shared<Exp>(zero);
    root->stmts.push_back(std::make_shared<Stmt>(let_i));

    // tag cond;
    auto tag_cond = TagStmt();
    tag_cond.ident = "cond";
    root->stmts.push_back(std::make_shared<Stmt>(tag_cond));

    // if i >= n to done;
    auto if_stmt = IfStmt();
    auto cond = Cond();
    cond.left = std::make_shared<Exp>(Ident("i"));
    cond.op = Cond::GEQ;
    cond.right = std::make_shared<Exp>(Ident("n"));
    if_stmt.cond = std::make_shared<Cond>(cond);
    if_stmt.ident = "done";
    root->stmts.push_back(std::make_shared<Stmt>(if_stmt));

    // tag do;
    auto tag_do = TagStmt();
    tag_do.ident = "do";
    root->stmts.push_back(std::make_shared<Stmt>(tag_do));

    // get a;
    auto get_a = GetStmt();
    get_a.ident = "a";
    root->stmts.push_back(std::make_shared<Stmt>(get_a));

    // get b;
    auto get_b = GetStmt();
    get_b.ident = "b";
    root->stmts.push_back(std::make_shared<Stmt>(get_b));

    // put add(a, b);
    auto put_add = PutStmt();
    auto add_call = CallExp();
    add_call.ident = "add";
    add_call.funcRParams = std::make_shared<FuncRParams>();
    add_call.funcRParams->exps.push_back(std::make_shared<Exp>(Ident("a")));
    add_call.funcRParams->exps.push_back(std::make_shared<Exp>(Ident("b")));
    put_add.exp = std::make_shared<Exp>(add_call);
    root->stmts.push_back(std::make_shared<Stmt>(put_add));

    // let i = i + 1;
    auto let_i2 = LetStmt();
    let_i2.ident = "i";
    auto i_plus_1 = BinaryExp();
    i_plus_1.lexp = std::make_shared<Exp>(Ident("i"));
    i_plus_1.op = BinaryExp::PLUS;
    auto one = Number();
    one.value = 1.0;
    i_plus_1.rexp = std::make_shared<Exp>(one);
    let_i2.exp = std::make_shared<Exp>(i_plus_1);
    root->stmts.push_back(std::make_shared<Stmt>(let_i2));

    // to cond;
    auto to_cond = ToStmt();
    to_cond.ident = "cond";
    root->stmts.push_back(std::make_shared<Stmt>(to_cond));

    // tag done;
    auto tag_done = TagStmt();
    tag_done.ident = "done";
    root->stmts.push_back(std::make_shared<Stmt>(tag_done));

    return root;
}