#include "utils.h"
#include "llvm/asm/AsmWriter.h"
#include "llvm/ir/Type.h"
#include "llvm/ir/value/Argument.h"
#include "llvm/ir/value/BasicBlock.h"
#include "llvm/ir/value/ConstantData.h"
#include "llvm/ir/value/Function.h"
#include "llvm/ir/value/GlobalValue.h"
#include "llvm/ir/value/GlobalVariable.h"
#include "llvm/ir/value/Use.h"
#include "llvm/ir/value/Value.h"
#include "llvm/ir/value/inst/ExtendedInstructions.h"
#include "llvm/ir/value/inst/Instruction.h"
#include "llvm/ir/value/inst/InstructionTypes.h"
#include "llvm/ir/value/inst/Instructions.h"

void Value::PrintAsm(AsmWriterPtr out) {
    TOLANG_DIE("Operation not supported.");
}

void Value::PrintName(AsmWriterPtr out) {
    TOLANG_DIE("Operation not supported.");
}

void Value::PrintUse(AsmWriterPtr out) {
    GetType()->PrintAsm(out);
    out->PushSpace();
    PrintName(out);
}

void ConstantData::PrintAsm(AsmWriterPtr out) {
    GetType()->PrintAsm(out);
    out->PushSpace();
    PrintName(out);
}

void ConstantData::PrintName(AsmWriterPtr out) {
    if (GetType()->IsFloatTy()) {
        out->Push(std::to_string(_floatValue));
    } else if (GetType()->IsIntegerTy()) {
        out->Push(std::to_string(_intValue));
    } else {
        TOLANG_DIE("Invalid ConstantData type");
    }
}

void GlobalValue::PrintName(AsmWriterPtr out) {
    out->Push('@').Push(GetName());
}

void GlobalVariable::PrintAsm(AsmWriterPtr out) {
    // Name
    PrintName(out);

    out->PushNext('=');

    // Attribute
    out->PushNext("dso_local").PushNext("global").PushSpace();

    if (_initializer) {
        _initializer->PrintAsm(out);
    } else {
        out->Push("zeroinitializer");
    }

    out->PushNewLine();
}

void Function::PrintAsm(AsmWriterPtr out) {
    // First, we trace all the slot.
    GetSlotTracker()->Trace(this);

    // Blank line.
    out->PushNewLine();

    // For debug purpose, we print some comments.
    out->CommentBegin().Push("Function type: ");
    GetType()->PrintAsm(out);
    out->CommentEnd();

    // Function header.
    out->Push("define dso_local ");
    GetType()->As<FunctionType>()->ReturnType()->PrintAsm(out);

    // Function name.
    out->PushSpace();
    PrintName(out);

    // Function parameters.
    out->Push('(');
    for (auto it = ArgBegin(); it != ArgEnd(); ++it) {
        auto arg = *it;
        if (it != ArgBegin()) {
            out->Push(", ");
        }
        arg->PrintUse(out);
    }
    out->Push(')');

    // Function body.
    out->PushNext('{').PushNewLine();

    // Basic blocks.
    for (auto it = BasicBlockBegin(); it != BasicBlockEnd(); ++it) {
        (*it)->PrintAsm(out);
    }

    // End of function.
    out->Push('}').PushNewLine();
}

void Argument::PrintAsm(AsmWriterPtr out) {
    GetType()->PrintAsm(out);
    out->PushNext('%').Push(
        std::to_string(Parent()->GetSlotTracker()->Slot(this)));
}

void Argument::PrintUse(AsmWriterPtr out) { PrintAsm(out); }

void BasicBlock::PrintAsm(AsmWriterPtr out) {
    for (auto it = InstructionBegin(); it != InstructionEnd(); ++it) {
        out->PushSpaces(4);
        (*it)->PrintAsm(out);
    }
}

void BasicBlock::PrintName(AsmWriterPtr out) {
    out->Push('%').Push(std::to_string(Parent()->GetSlotTracker()->Slot(this)));
}

void BasicBlock::PrintUse(AsmWriterPtr out) {
    GetType()->PrintAsm(out);
    out->PushSpace();
    PrintName(out);
}

void Instruction::PrintName(AsmWriterPtr out) {
    TOLANG_DIE_IF_NOT(!GetType()->IsVoidTy(),
                      "Only instruction with non-void type can call this.");

    out->Push('%').Push(
        std::to_string(Parent()->Parent()->GetSlotTracker()->Slot(this)));
}

void Instruction::PrintUse(AsmWriterPtr out) {
    TOLANG_DIE_IF_NOT(!GetType()->IsVoidTy(),
                      "Only instruction with non-void type can call this.");

    GetType()->PrintAsm(out);
    out->PushNext('%').Push(
        std::to_string(Parent()->Parent()->GetSlotTracker()->Slot(this)));
}

void AllocaInst::PrintAsm(AsmWriterPtr out) {
    PrintName(out);

    out->PushNext('=').PushNext("alloca").PushSpace();
    AllocatedType()->PrintAsm(out);
    out->PushNewLine();
}

void StoreInst::PrintAsm(AsmWriterPtr out) {
    out->Push("store").PushSpace();

    LeftOperand()->PrintUse(out);
    out->Push(", ");
    RightOperand()->PrintUse(out);
    out->PushNewLine();
}

void LoadInst::PrintAsm(AsmWriterPtr out) {
    PrintName(out);

    out->PushNext('=').PushNext("load").PushSpace();
    GetType()->PrintAsm(out);
    out->Push(", ");
    Address()->PrintUse(out);
    out->PushNewLine();
}

void ReturnInst::PrintAsm(AsmWriterPtr out) {
    out->Push("ret");
    ValuePtr ret = OperandAt(0);
    if (!ret || ret->GetType()->IsVoidTy()) {
        out->PushNext("void");
    } else {
        out->PushSpace();
        ret->PrintUse(out);
    }
    out->PushNewLine();
}

void CallInst::PrintAsm(AsmWriterPtr out) {
    if (!GetType()->IsVoidTy()) {
        PrintName(out);
        out->Push(" = ");
    }

    out->Push("call").PushSpace();

    // Return type.
    GetFunction()->ReturnType()->PrintAsm(out);
    out->PushSpace();

    // Function name.
    GetFunction()->PrintName(out);

    // Parameters.
    out->Push('(');
    for (auto it = UseBegin(); it != UseEnd(); ++it) {
        if (it != UseBegin()) {
            out->Push(", ");
        }
        (*it)->GetValue()->PrintUse(out);
    }
    out->Push(')').PushNewLine();
}

/*
 * %2 = add nsw i32 0, %1
 * %2 = sub nsw i32 0, %1
 * %2 = xor i1 %1, 1
 */
void UnaryOperator::PrintAsm(AsmWriterPtr out) {
    if (OpType() == UnaryOpType::Not) {
        PrintName(out);
        out->PushNext('=').Push("xor").PushSpace();
        Operand()->GetType()->PrintAsm(out);
        out->PushSpace();
        Operand()->PrintName(out);
        out->Push(", ").Push('1').PushNewLine();
        return;
    }

    const char *op;

    switch (OpType()) {
    case UnaryOpType::Pos:
        op = "add nsw";
        break;
    case UnaryOpType::Neg:
        op = "sub nsw";
        break;
    default:
        TOLANG_DIE("Shouldn't reach here.");
    }

    PrintName(out);
    out->PushNext("=").PushNext(op).PushSpace();

    GetType()->PrintAsm(out);
    out->PushNext('0').Push(", ");
    out->PushSpace();
    Operand()->PrintName(out);

    out->PushNewLine();
}

/*
 * %11 = add nsw i32 %9, %10
 * %18 = sub nsw i32 0, %7
 * %13 = mul nsw i32 %12, 2
 * %15 = sdiv i32 %14, 2
 * %17 = srem i32 %16, 2
 */
void BinaryOperator::PrintAsm(AsmWriterPtr out) {
    const char *op;
    bool isFloat = GetType()->IsFloatTy();
    switch (OpType()) {
    case BinaryOpType::Add:
        op = isFloat ? "fadd" : "add nsw";
        break;
    case BinaryOpType::Sub:
        op = isFloat ? "fsub" : "sub nsw";
        break;
    case BinaryOpType::Mul:
        op = isFloat ? "fmul" : "mul nsw";
        break;
    case BinaryOpType::Div:
        op = isFloat ? "fdiv" : "sdiv";
        break;
    case BinaryOpType::Mod:
        TOLANG_ASSERT(!isFloat,
                      "Mod operation is not supported for float type.");
        op = "srem";
        break;
    }

    PrintName(out);
    out->PushNext("=").PushNext(op).PushSpace();

    GetType()->PrintAsm(out);
    out->PushSpace();

    LeftOperand()->PrintName(out);
    out->Push(", ");
    RightOperand()->PrintName(out);

    out->PushNewLine();
}

void CompareInstruction::PrintAsm(AsmWriterPtr out) {
    const char *op;

    switch (OpType()) {
    case CompareOpType::Equal:
        op = "eq";
        break;
    case CompareOpType::NotEqual:
        op = "ne";
        break;
    case CompareOpType::GreaterThan:
        op = "sgt";
        break;
    case CompareOpType::GreaterThanOrEqual:
        op = "sge";
        break;
    case CompareOpType::LessThan:
        op = "slt";
        break;
    case CompareOpType::LessThanOrEqual:
        op = "sle";
        break;
    }

    PrintName(out);
    out->PushNext("=").PushNext("icmp").PushNext(op).PushSpace();
    LeftOperand()->GetType()->PrintAsm(out);
    out->Push(", ");
    RightOperand()->PrintName(out);
    out->PushNewLine();
}

void InputInst::PrintAsm(AsmWriterPtr out) {
    PrintName(out);
    out->PushNext('=').PushNext("call").PushSpace();
    GetType()->PrintAsm(out);
    out->PushNext('@').Push(GetName()).Push("()").PushNewLine();
}

void OutputInst::PrintAsm(AsmWriterPtr out) {
    out->Push("call").PushSpace();
    GetType()->PrintAsm(out);
    out->PushSpace();
    out->Push("@").Push(GetName()).Push('(');
    Operand()->PrintUse(out);
    out->Push(')').PushNewLine();
}