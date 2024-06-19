#include "llvm/ir/value/BasicBlock.h"
#include "llvm/ir/LlvmContext.h"
#include "llvm/ir/value/Function.h"
#include "llvm/ir/value/inst/Instruction.h"

BasicBlockPtr BasicBlock::New(FunctionPtr parent) {
    return static_cast<BasicBlockPtr>(new BasicBlock(parent));
}

BasicBlock::BasicBlock(FunctionPtr parent)
    : Value(ValueType::BasicBlockTy, parent->Context()->GetLabelTy()),
      HasParent(parent) {}

BasicBlockPtr BasicBlock::InsertInstruction(InstructionPtr instruction) {
    instruction->SetParent(this);
    _instructions.push_back(instruction);
    return this;
}

BasicBlockPtr BasicBlock::InsertInstruction(instruction_iterator iter,
                                            InstructionPtr instruction) {
    instruction->SetParent(this);
    _instructions.insert(iter, instruction);
    return this;
}

BasicBlockPtr BasicBlock::RemoveInstruction(InstructionPtr instruction) {
    instruction->RemoveParent();
    _instructions.remove(instruction);
    return this;
}