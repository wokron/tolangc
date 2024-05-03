#include "llvm/ir/LlvmContext.h"
#include "llvm/ir/value/BasicBlock.h"
#include "llvm/ir/value/Function.h"
#include "llvm/ir/value/inst/Instruction.h"


BasicBlockPtr BasicBlock::New(FunctionPtr parent)
{
    return static_cast<BasicBlockPtr>(new BasicBlock(parent));
}


BasicBlock::BasicBlock(FunctionPtr parent)
    : Value(ValueType::BasicBlockTy, parent->Context()->GetLabelTy()),
      HasParent(parent)
{
}


BasicBlockPtr BasicBlock::InsertInstruction(InstructionPtr instruction)
{
    _instructions.push_back(instruction);
    return this;
}


BasicBlockPtr BasicBlock::InsertInstruction(instruction_iterator iter, InstructionPtr inst)
{
    _instructions.insert(iter, inst);
    return this;
}


BasicBlockPtr BasicBlock::RemoveInstruction(InstructionPtr instruction)
{
    instruction->RemoveParent();
    _instructions.remove(instruction);
    return this;
}
