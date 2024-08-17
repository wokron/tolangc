#include "llvm/utils.h"
#include "llvm/asm/AsmWriter.h"
#include "llvm/ir/Type.h"
#include "llvm/ir/value/Argument.h"
#include "llvm/ir/value/BasicBlock.h"
#include "llvm/ir/value/ConstantData.h"
#include "llvm/ir/value/Function.h"
#include "llvm/ir/value/GlobalValue.h"
#include "llvm/ir/value/Use.h"
#include "llvm/ir/value/Value.h"
#include "llvm/ir/value/inst/ExtendedInstructions.h"
#include "llvm/ir/value/inst/Instruction.h"
#include "llvm/ir/value/inst/InstructionTypes.h"
#include "llvm/ir/value/inst/Instructions.h"

#include <algorithm>

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
    // We don't print the first basic block.
    if (this != *Parent()->BasicBlockBegin()) {
        auto tracker = Parent()->GetSlotTracker();
        std::string slot(std::to_string(tracker->Slot(this)));
        out->Push(slot).Push(':');
        if (!this->GetUserList()->empty()) {
            int padding = 50 - static_cast<int>(slot.length()) - 1;
            out->PushSpaces(padding).Push("; preds = ");
            std::vector<int> preds;
            for (auto it = this->UserBegin(); it != UserEnd(); ++it) {
                // The user MUST be a BranchInst or JumpInst.
                auto user =
                    dynamic_cast<HasParent<BasicBlock> *>((*it)->GetUser());
                TOLANG_ASSERT(user);
                preds.push_back(tracker->Slot(user->Parent()));
            }
            std::sort(preds.begin(), preds.end(), std::less<int>());
            for (auto it = preds.begin(); it != preds.end(); ++it) {
                out->Push('%').Push(std::to_string(*it));
                if (it + 1 != preds.end()) {
                    out->Push(", ");
                }
            }
        }
        out->PushNewLine();
    }
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

void BranchInst::PrintAsm(AsmWriterPtr out) {
    out->Push("br").PushSpace();

    Condition()->PrintUse(out);
    out->Push(", ");
    TrueBlock()->PrintUse(out);
    out->Push(", ");
    FalseBlock()->PrintUse(out);
    out->PushNewLine();
}

void JumpInst::PrintAsm(AsmWriterPtr out) {
    out->Push("br").PushSpace();
    Target()->PrintUse(out);
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
    bool isFloat = GetType()->IsFloatTy();

    switch (OpType()) {
    case UnaryOpType::Pos:
        op = isFloat ? "fadd" : "add nsw";
        break;
    case UnaryOpType::Neg:
        op = isFloat ? "fsub" : "sub nsw";
        break;
    default:
        TOLANG_DIE("Shouldn't reach here.");
    }

    PrintName(out);
    out->PushNext("=").PushNext(op).PushSpace();

    GetType()->PrintAsm(out);
    out->PushNext(isFloat ? "0.0" : "0").Push(',').PushSpace();
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
        TOLANG_ASSERT(!isFloat);
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
    bool isFloat = LeftOperand()->GetType()->IsFloatTy();

    switch (OpType()) {
    case CompareOpType::Equal:
        op = isFloat ? "oeq" : "eq";
        break;
    case CompareOpType::NotEqual:
        op = isFloat ? "one" : "ne";
        break;
    case CompareOpType::GreaterThan:
        op = isFloat ? "ogt" : "sgt";
        break;
    case CompareOpType::GreaterThanOrEqual:
        op = isFloat ? "oge" : "sge";
        break;
    case CompareOpType::LessThan:
        op = isFloat ? "olt" : "slt";
        break;
    case CompareOpType::LessThanOrEqual:
        op = isFloat ? "ole" : "sle";
        break;
    }

    PrintName(out);
    out->PushNext("=");
    if (isFloat) {
        out->PushNext("fcmp");
    } else {
        out->PushNext("icmp");
    }
    out->PushNext(op).PushSpace();
    LeftOperand()->GetType()->PrintAsm(out);
    out->PushSpace();
    LeftOperand()->PrintName(out);
    out->Push(',').PushSpace();
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