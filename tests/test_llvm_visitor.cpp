#include "tolang/utils.h"

#if TOLANG_BACKEND == LLVM

#include "tolang/ast.h"
#include "tolang/lexer.h"
#include "tolang/parser.h"
#include "tolang/visitor.h"
#include "llvm/asm/AsmPrinter.h"
#include <doctest.h>
#include <sstream>

static constexpr char INPUT[] = R"(fn add(a, b) => a + b;

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
)";

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

TEST_CASE("testing visitor") {
    std::istringstream input(INPUT);
    Lexer lexer = Lexer(input);
    Parser parser = Parser(lexer);
    auto root = parser.parse();

    ModulePtr module = Module::New("tolang.c");
    auto visitor = Visitor(module);
    visitor.visit(*root);

    AsmPrinter printer;

    std::ostringstream ss;
    printer.Print(module, ss);

    auto ir = ss.str();

    CHECK_EQ(ir, EXPECTED);
}

#endif
