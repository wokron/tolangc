#include <sstream>

#include "doctest.h"

#include "llvm/asm/AsmPrinter.h"
#include "llvm/ir/Llvm.h"

/*
Suppose we have such a source file.

fn add ( lhs, rhs ) => lhs + rhs;

var i1;
var i2;

get i1;
get i2;
let i3 = add(i1, i2);
put i3;
 */

constexpr char EXPECTED[] = R"(; tolang LLVM IR

; Module ID = 'tolang.c'
source_filename = "tolang.c"

declare float @get()
declare void @put(float)

@i1 = dso_local global float 0.000000
@i2 = dso_local global float 0.000000

; Function type: float (float, float)
define dso_local float @add(float %0, float %1) {
    %3 = alloca float
    %4 = alloca float
    store float %0, float* %3
    store float %1, float* %4
    %5 = load float, float* %3
    %6 = load float, float* %4
    %7 = fadd float %5, %6
    ret float %7
}

; Function type: i32 ()
define dso_local i32 @main() {
    %1 = alloca float
    %2 = call float @get()
    %3 = call float @get()
    store float %2, float* @i1
    store float %3, float* @i2
    %4 = load float, float* @i1
    %5 = load float, float* @i2
    %6 = call float @add(float %4, float %5)
    store float %6, float* %1
    %7 = load float, float* %1
    call void @put(float %7)
    ret i32 0
}

; End of LLVM IR
)";

FunctionPtr BuildAdd(ModulePtr module);
FunctionPtr BuildMain(ModulePtr module, FunctionPtr add, GlobalVariablePtr i1,
                      GlobalVariablePtr i2);

TEST_CASE("LLVM IR Test") {
    ModulePtr module = Module::New("tolang.c");
    LlvmContextPtr context = module->Context();

    auto i1 = GlobalVariable::New(context->GetFloatTy(), "i1");
    auto i2 = GlobalVariable::New(context->GetFloatTy(), "i2");
    module->AddGlobalVariable(i1);
    module->AddGlobalVariable(i2);

    auto add = BuildAdd(module);
    auto main = BuildMain(module, add, i1, i2);

    module->AddFunction(add);
    module->AddMainFunction(main);

    AsmPrinter printer;

    std::ostringstream ss;
    printer.Print(module, ss);
    auto ir = ss.str();

    CHECK_EQ(EXPECTED, ir);
    std::cout << ir;
}

FunctionPtr BuildAdd(ModulePtr module) {
    auto context = module->Context();

    std::vector<ArgumentPtr> args;
    auto arg_lhs = Argument::New(context->GetFloatTy(), "lhs");
    auto arg_rhs = Argument::New(context->GetFloatTy(), "rhs");
    args.push_back(arg_lhs);
    args.push_back(arg_rhs);
    auto function = Function::New(context->GetFloatTy(), "add", args);

    auto block = function->NewBasicBlock();

    auto alloca_lhs = AllocaInst::New(context->GetFloatTy());
    auto alloca_rhs = AllocaInst::New(context->GetFloatTy());
    block->InsertInstruction(alloca_lhs)->InsertInstruction(alloca_rhs);

    auto store_lhs = StoreInst::New(arg_lhs, alloca_lhs);
    auto store_rhs = StoreInst::New(arg_rhs, alloca_rhs);
    block->InsertInstruction(store_lhs)->InsertInstruction(store_rhs);

    auto load_lhs = LoadInst::New(alloca_lhs);
    auto load_rhs = LoadInst::New(alloca_rhs);
    block->InsertInstruction(load_lhs)->InsertInstruction(load_rhs);

    auto add = BinaryOperator::New(BinaryOpType::Add, load_lhs, load_rhs);
    block->InsertInstruction(add);

    auto ret = ReturnInst::New(add);
    block->InsertInstruction(ret);

    return function;
}

FunctionPtr BuildMain(ModulePtr module, FunctionPtr add, GlobalVariablePtr i1,
                      GlobalVariablePtr i2) {
    auto context = module->Context();

    auto function = Function::New(module->Context()->GetInt32Ty(), "main");
    auto block = function->NewBasicBlock();

    auto alloca_i3 = AllocaInst::New(context->GetFloatTy());
    block->InsertInstruction(alloca_i3);

    auto input_i1 = InputInst::New(context);
    auto input_i2 = InputInst::New(context);
    block->InsertInstruction(input_i1)->InsertInstruction(input_i2);

    block->InsertInstruction(StoreInst::New(input_i1, i1));
    block->InsertInstruction(StoreInst::New(input_i2, i2));

    std::vector<ValuePtr> params;
    auto load_i1 = LoadInst::New(i1);
    auto load_i2 = LoadInst::New(i2);
    block->InsertInstruction(load_i1)->InsertInstruction(load_i2);
    params.push_back(load_i1);
    params.push_back(load_i2);
    auto call = CallInst::New(add, params);
    block->InsertInstruction(call);
    block->InsertInstruction(StoreInst::New(call, alloca_i3));

    auto load_i3 = LoadInst::New(alloca_i3);
    auto output_i3 = OutputInst::New(load_i3);
    block->InsertInstruction(load_i3)->InsertInstruction(output_i3);

    auto ret = ReturnInst::New(ConstantData::New(context->GetInt32Ty(), 0));
    block->InsertInstruction(ret);

    return function;
}
