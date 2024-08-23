#include "tolang/utils.h"

#if TOLANG_BACKEND == LLVM

#include <sstream>

#include "doctest.h"

#include "llvm/asm/AsmPrinter.h"
#include "llvm/ir/Llvm.h"

/*
Suppose we have such a source file.

=========================================
fn calc ( lhs, rhs ) => +lhs + rhs - lhs * rhs / lhs + 1.0;

var i1;
var i2;

let i1 = 1.0;
get i2;

tag do;
i1 = calc(i1, i2);
if i1 < i2 to do;

put i1;
=========================================

 Input: 12345.6789
Output: 12345.927734

 */

constexpr char EXPECTED[] = R"(; tolang LLVM IR

; Module ID = 'tolang.c'
source_filename = "tolang.c"

declare float @get()
declare void @put(float)


; Function type: float (float, float)
define dso_local float @calc(float %0, float %1) {
    %3 = alloca float
    %4 = alloca float
    store float %0, float* %3
    store float %1, float* %4
    %5 = load float, float* %3
    %6 = load float, float* %4
    %7 = fadd float 0.0, %5
    %8 = fadd float %7, %6
    %9 = fmul float %5, %6
    %10 = fdiv float %9, %5
    %11 = fsub float %8, %10
    %12 = fadd float %11, 1.000000
    ret float %12
}

; Function type: i32 ()
define dso_local i32 @main() {
    %1 = alloca float
    %2 = alloca float
    store float 1.000000, float* %1
    %3 = call float @get()
    store float %3, float* %2
    br label %4
4:                                                ; preds = %0, %10
    %5 = load float, float* %1
    %6 = load float, float* %2
    %7 = call float @calc(float %5, float %6)
    store float %7, float* %1
    %8 = load float, float* %1
    %9 = fcmp olt float %8, %6
    br i1 %9, label %10, label %11
10:                                               ; preds = %4
    br label %4
11:                                               ; preds = %4
    br label %12
12:                                               ; preds = %11
    %13 = load float, float* %1
    call void @put(float %13)
    ret i32 0
}

; End of LLVM IR
)";

static FunctionPtr BuildCalc(ModulePtr module);
static FunctionPtr BuildMain(ModulePtr module, FunctionPtr calc);

TEST_CASE("LLVM IR Test") {
    ModulePtr module = Module::New("tolang.c");

    auto calcFunc = BuildCalc(module);
    auto mainFunc = BuildMain(module, calcFunc);

    module->AddFunction(calcFunc);
    module->AddMainFunction(mainFunc);

    AsmPrinter printer;
    std::ostringstream ss;
    printer.Print(module, ss);
    auto ir = ss.str();

    CHECK_EQ(EXPECTED, ir);
}

static FunctionPtr BuildCalc(ModulePtr module) {
    LlvmContextPtr context = module->Context();

    // initialize arguments
    std::vector<ArgumentPtr> args;
    auto arg_lhs = Argument::New(context->GetFloatTy(), "lhs");
    auto arg_rhs = Argument::New(context->GetFloatTy(), "rhs");
    args.push_back(arg_lhs);
    args.push_back(arg_rhs);
    FunctionPtr calcFunc = Function::New(context->GetFloatTy(), "calc", args);

    // function body
    auto block = calcFunc->NewBasicBlock();

    // function entry
    auto alloca_lhs = AllocaInst::New(context->GetFloatTy());
    auto alloca_rhs = AllocaInst::New(context->GetFloatTy());
    block->InsertInstruction(alloca_lhs)->InsertInstruction(alloca_rhs);

    auto store_lhs = StoreInst::New(arg_lhs, alloca_lhs);
    auto store_rhs = StoreInst::New(arg_rhs, alloca_rhs);
    block->InsertInstruction(store_lhs)->InsertInstruction(store_rhs);

    // load operators
    auto load_lhs = LoadInst::New(alloca_lhs);
    auto load_rhs = LoadInst::New(alloca_rhs);
    block->InsertInstruction(load_lhs)->InsertInstruction(load_rhs);

    // fn calc ( lhs, rhs ) => +lhs + rhs - lhs * rhs / lhs + 1.0;

    // +lhs
    auto t1 = UnaryOperator::New(UnaryOpType::Pos, load_lhs);
    // +lhs + rhs
    auto t2 = BinaryOperator::New(BinaryOpType::Add, t1, load_rhs);
    // lhs * rhs
    auto t3 = BinaryOperator::New(BinaryOpType::Mul, load_lhs, load_rhs);
    // lhs * rhs / lhs
    auto t4 = BinaryOperator::New(BinaryOpType::Div, t3, load_lhs);
    // +lhs + rhs - lhs * rhs / lhs
    auto t5 = BinaryOperator::New(BinaryOpType::Sub, t2, t4);
    // +lhs + rhs - lhs * rhs / lhs + 1.0
    auto one = ConstantData::New(context->GetFloatTy(), 1.0f);
    auto t6 = BinaryOperator::New(BinaryOpType::Add, t5, one);

    block->InsertInstruction(t1)
        ->InsertInstruction(t2)
        ->InsertInstruction(t3)
        ->InsertInstruction(t4)
        ->InsertInstruction(t5)
        ->InsertInstruction(t6);
    block->InsertInstruction(ReturnInst::New(t6));

    return calcFunc;
}

static FunctionPtr BuildMain(ModulePtr module, FunctionPtr calc) {
    LlvmContextPtr context = module->Context();
    FunctionPtr mainFunc =
        Function::New(module->Context()->GetInt32Ty(), "main");
    auto block = mainFunc->NewBasicBlock();

    // var i1; var i2;
    AllocaInstPtr i1 = AllocaInst::New(context->GetFloatTy());
    AllocaInstPtr i2 = AllocaInst::New(context->GetFloatTy());
    block->InsertInstruction(i1)->InsertInstruction(i2);

    // let i1 = 1.0;
    auto one = ConstantData::New(context->GetFloatTy(), 1.0f);
    block->InsertInstruction(StoreInst::New(one, i1));

    // get i2;
    auto get_i2 = InputInst::New(context);
    block->InsertInstruction(get_i2);
    block->InsertInstruction(StoreInst::New(get_i2, i2));

    // tag do;
    auto tag = mainFunc->NewBasicBlock();
    block->InsertInstruction(JumpInst::New(tag));
    block = tag;

    // i1 = calc(i1, i2);
    auto load_i1 = LoadInst::New(i1);
    auto load_i2 = LoadInst::New(i2);
    auto call = CallInst::New(calc, {load_i1, load_i2});
    auto store_i1 = StoreInst::New(call, i1);
    block->InsertInstruction(load_i1)
        ->InsertInstruction(load_i2)
        ->InsertInstruction(call)
        ->InsertInstruction(store_i1);

    // if i1 < i2 to do;
    load_i1 = LoadInst::New(i1);
    auto trueBlock = mainFunc->NewBasicBlock();
    auto falseBlock = mainFunc->NewBasicBlock();
    auto nextBlock = mainFunc->NewBasicBlock();
    auto comp =
        CompareInstruction::New(CompareOpType::LessThan, load_i1, load_i2);
    auto branch = BranchInst::New(comp, trueBlock, falseBlock);
    block->InsertInstruction(load_i1)
        ->InsertInstruction(comp)
        ->InsertInstruction(branch);

    trueBlock->InsertInstruction(JumpInst::New(block));
    falseBlock->InsertInstruction(JumpInst::New(nextBlock));
    block = nextBlock;

    // put i1;
    load_i1 = LoadInst::New(i1);
    block->InsertInstruction(load_i1);
    block->InsertInstruction(OutputInst::New(load_i1));

    // return 0;
    block->InsertInstruction(
        ReturnInst::New(ConstantData::New(context->GetInt32Ty(), 0)));

    return mainFunc;
}

#endif
