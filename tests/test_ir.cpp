#include <sstream>

#include "doctest.h"

#include "llvm/asm/AsmPrinter.h"
#include "llvm/ir/Llvm.h"
#include "llvm/ir/value/ConstantData.h"

/*
Suppose we have such source file.

int i1;
int i2;

int add(int a, int b)
{
    return a + b;
}

int main()
{
    int i3;

    i1 = get();
    i2 = get();
    i3 = add(i1, i2);
    put(i3);

    return 0;
}
 */

constexpr char EXPECTED[] = R"(; tolang LLVM IR

; Module ID = 'tolang.c'
source_filename = "tolang.c"

declare i32 @get()
declare void @put(i32)

@i1 = dso_local global
@i2 = dso_local global

; Function type: i32 (i32, i32)
define dso_local i32 @add(i32 %0, i32 %1) {
    %3 = alloca i32
    %4 = alloca i32
    store i32 %0, i32* %3
    store i32 %1, i32* %4
    %5 = load i32, i32* %3
    %6 = load i32, i32* %4
    %7 = add nsw i32 %5, %6
    ret i32 %7
}

; Function type: i32 ()
define dso_local i32 @main() {
    %1 = alloca i32
    %2 = call i32 @get()
    %3 = call i32 @get()
    %4 = call i32 @add(i32 @i1, i32 @i2)
    store i32 %4, i32* %1
    %5 = load i32, i32* %1
    call void @put(i32 %5)
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

    auto i1 = GlobalVariable::New(context->GetInt32Ty(), "i1");
    auto i2 = GlobalVariable::New(context->GetInt32Ty(), "i2");
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
    int line = 1;
    int ch = 1;
    for (int i = 0; i < ir.size(); ++i) {
        if (ir[i] == '\n') {
            line++;
            ch = 1;
        } else {
            ch++;
        }
        if (ir[i] != EXPECTED[i]) {
            std::cout << "line: " << line << ", ch: " << ch
                      << ", expected: " << EXPECTED[i] << ", actual: " << ir[i]
                      << std::endl;
        }
    }
    CHECK_EQ(EXPECTED, ir);
    std::cout << ir;
}

FunctionPtr BuildAdd(ModulePtr module) {
    auto context = module->Context();

    std::vector<ArgumentPtr> args;
    auto arg_a = Argument::New(context->GetInt32Ty(), "a");
    auto arg_b = Argument::New(context->GetInt32Ty(), "b");
    args.push_back(arg_a);
    args.push_back(arg_b);
    auto function = Function::New(context->GetInt32Ty(), "add", args);

    auto block = function->NewBasicBlock();

    auto alloca_a = AllocaInst::New(context->GetInt32Ty());
    auto alloca_b = AllocaInst::New(context->GetInt32Ty());
    block->InsertInstruction(alloca_a)->InsertInstruction(alloca_b);

    auto store_a = StoreInst::New(arg_a, alloca_a);
    auto store_b = StoreInst::New(arg_b, alloca_b);
    block->InsertInstruction(store_a)->InsertInstruction(store_b);

    auto load_a = LoadInst::New(alloca_a);
    auto load_b = LoadInst::New(alloca_b);
    block->InsertInstruction(load_a)->InsertInstruction(load_b);

    auto add = BinaryOperator::New(BinaryOpType::Add, load_a, load_b);
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

    auto alloca_i3 = AllocaInst::New(context->GetInt32Ty());
    block->InsertInstruction(alloca_i3);

    auto input_i1 = InputInst::New(context);
    auto input_i2 = InputInst::New(context);
    block->InsertInstruction(input_i1)->InsertInstruction(input_i2);

    std::vector<ValuePtr> params;
    params.push_back(i1);
    params.push_back(i2);
    auto call = CallInst::New(add, params);
    auto store_i3 = StoreInst::New(call, alloca_i3);
    block->InsertInstruction(call)->InsertInstruction(store_i3);

    auto load_i3 = LoadInst::New(alloca_i3);
    auto output_i3 = OutputInst::New(load_i3);
    block->InsertInstruction(load_i3)->InsertInstruction(output_i3);

    auto ret = ReturnInst::New(ConstantData::New(context->GetInt32Ty(), 0));
    block->InsertInstruction(ret);

    return function;
}
