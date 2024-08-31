#include "tolang/utils.h"

#if TOLANG_BACKEND == LLVM

#include "mips/translator.h"
#include "llvm/ir/Llvm.h"
#include <doctest.h>
#include <sstream>

/*
source file
 =========================================================
fn average ( lhs, rhs ) => (lhs + rhs) / 2.0;

var i1;
var i2;

let i1 = 1.0;
get i2;
let i1 = average(i1 * 3, i2);

if i1 > 10 to do;
let i1 = i2 / 4;
to end;

tag do;
put i1;

tag end;
 =========================================================

llvm source file
 =========================================================
define dso_local float @average(float %1, float %2) {
    %3 = alloca float
    %4 = alloca float
    store float %1, float* %3
    store float %2, float* %4
    %5 = load float, float* %3
    %6 = load float, float* %4
    %7 = fadd float %5, %6
    %8 = fdiv float %7, 2.000000
    ret float %8
}
define dso_local float @main() {
    %1 = alloca float
    %2 = alloca float
    store float 1, float * %1
    %3 = call float @getint()
    store float %3, float * %2
    %4 = load float, float * %1
    %5 = fmul float %4, 3
    %6 = load float, float * %2
    %7 = call float @average(float %5, float %6)
    store float %7, float * %1
    %8 = load float, float * %1
    %9 = fcmp ogt float %8, 10
    br i1 %9, label %10, label %11
10:
    %11 = load float, float * %1
    call void @putint(float %11)
    br label %14

11:
    %12 = load float, float * %2
    %13 = fsub float %12, 4
    store float %13, float * %1
    br label %14

14:
    ret float 0
}
 =========================================================
*/

static constexpr char EXPECTED[] = R"(.data
flt0: .float 2
flt1: .float 0
flt2: .float 1
flt3: .float 3
flt4: .float 10
flt5: .float 4
.text
j main
nop

average:

average_0:
l.s $f1, 0($sp)
l.s $f2, -4($sp)
add.s $f3, $f1, $f2
l.s $f4, flt0
div.s $f5, $f3, $f4
l.s $f6, flt1
add.s $f0, $f5, $f6
jr $ra
nop

main:

main_0:
l.s $f1, flt2
s.s $f1, 0($sp)
addiu $v0, $zero, 6
syscall
l.s $f2, flt1
add.s $f3, $f0, $f2
s.s $f3, -4($sp)
l.s $f4, 0($sp)
l.s $f5, flt3
mul.s $f6, $f4, $f5
l.s $f7, -4($sp)
s.s $f6, -12($sp)
s.s $f7, -16($sp)
sw $ra, -8($sp)
addiu $sp, $sp, -12
jal average
nop
subiu $sp, $sp, -12
lw $ra, -8($sp)
l.s $f9, flt1
add.s $f8, $f0, $f9
s.s $f8, 0($sp)
l.s $f10, 0($sp)
l.s $f11, flt4
c.le.s $f10, $f11
bc1f main_1
nop
addiu $t0, $zero, 0
j main_2
nop

main_1:
addiu $t0, $zero, 1

main_2:
bnez $t0, main_3
nop
j main_4
nop

main_3:
l.s $f13, 0($sp)
l.s $f14, flt1
add.s $f12, $f13, $f14
addiu $v0, $zero, 2
syscall
addiu $a0, $zero, 10
addiu $v0, $zero, 11
syscall
j main_5
nop

main_4:
l.s $f15, -4($sp)
l.s $f16, flt5
sub.s $f17, $f15, $f16
s.s $f17, 0($sp)
j main_5
nop

main_5:
addiu $v0, $zero, 10
syscall
)";

ModulePtr build_module();

TEST_CASE("testing mips") {
    // create llvm module
    auto modulePtr = build_module();
    std::cout << "build module success" << std::endl;

    // translate
    Translator translator;
    translator.translate(modulePtr);
    std::cout << "translate success" << std::endl;
    std::ostringstream ss;
    translator.print(ss);

    auto ir = ss.str();
//    std::cout << ir;
    CHECK_EQ(ir, EXPECTED);
}

static FunctionPtr BuildAverage(ModulePtr module) {
    LlvmContextPtr context = module->Context();

    // initialize arguments
    std::vector<ArgumentPtr> args;
    auto arg_lhs = Argument::New(context->GetFloatTy(), "lhs");
    auto arg_rhs = Argument::New(context->GetFloatTy(), "rhs");
    args.push_back(arg_lhs);
    args.push_back(arg_rhs);
    FunctionPtr avrFunc = Function::New(context->GetFloatTy(), "average", args);

    // function body
    auto block = avrFunc->NewBasicBlock();

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

    // fn calc ( lhs, rhs ) => (lhs + rhs)/2;
    // lhs + rhs
    auto t1 = BinaryOperator::New(BinaryOpType::Add, load_lhs, load_rhs);
    auto two = ConstantData::New(context->GetFloatTy(), 2.0f);
    // lhs * rhs
    auto t2 = BinaryOperator::New(BinaryOpType::Div, t1, two);
    // lhs * rhs / lhs

    block->InsertInstruction(t1)->InsertInstruction(t2);
    block->InsertInstruction(ReturnInst::New(t2));
    return avrFunc;
}

static FunctionPtr BuildMain(ModulePtr module, FunctionPtr average) {
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

    // i1 = average(i1 * 3, i2);
    auto load_i1 = LoadInst::New(i1);
    auto three = ConstantData::New(context->GetFloatTy(), 3.0f);
    auto load_i1_3 = BinaryOperator::New(BinaryOpType::Mul, load_i1, three);
    auto load_i2 = LoadInst::New(i2);
    auto call = CallInst::New(average, {load_i1_3, load_i2});
    auto store_i1 = StoreInst::New(call, i1);
    block->InsertInstruction(load_i1)
        ->InsertInstruction(load_i1_3)
        ->InsertInstruction(load_i2)
        ->InsertInstruction(call)
        ->InsertInstruction(store_i1);

    // if i1 > 10 to do;
    load_i1 = LoadInst::New(i1);
    auto ten = ConstantData::New(context->GetFloatTy(), 10.0f);
    auto comp =
        CompareInstruction::New(CompareOpType::GreaterThan, load_i1, ten);
    auto trueBlock = mainFunc->NewBasicBlock();
    auto falseBlock = mainFunc->NewBasicBlock();
    auto nextBlock = mainFunc->NewBasicBlock();
    auto branch = BranchInst::New(comp, trueBlock, falseBlock);
    block->InsertInstruction(load_i1)
        ->InsertInstruction(comp)
        ->InsertInstruction(branch);

    // i1 = i2 % 4;
    load_i2 = LoadInst::New(i2);
    auto four = ConstantData::New(context->GetFloatTy(), 4.0f);
    auto sub = BinaryOperator::New(BinaryOpType::Sub, load_i2, four);
    store_i1 = StoreInst::New(sub, i1);
    falseBlock->InsertInstruction(load_i2)
        ->InsertInstruction(sub)
        ->InsertInstruction(store_i1);

    // to end;
    falseBlock->InsertInstruction(JumpInst::New(nextBlock));

    // put i1;
    load_i1 = LoadInst::New(i1);
    trueBlock->InsertInstruction(load_i1)
        ->InsertInstruction(OutputInst::New(load_i1))
        ->InsertInstruction(JumpInst::New(nextBlock));

    // return 0;
    block = nextBlock;
    block->InsertInstruction(
        ReturnInst::New(ConstantData::New(context->GetInt32Ty(), 0)));

    return mainFunc;
}

ModulePtr build_module() {
    ModulePtr modulePtr = Module::New("tolang.c");
    auto calcFunc = BuildAverage(modulePtr);
    auto mainFunc = BuildMain(modulePtr, calcFunc);

    modulePtr->AddFunction(calcFunc);
    modulePtr->AddMainFunction(mainFunc);

    return modulePtr;
}

#endif