#pragma once

#include "llvm/ir/value/Value.h"
#include <list>

class BasicBlock final : public Value, public HasParent<Function> {
public:
    ~BasicBlock() override = default;

    static bool classof(const ValueType type) {
        return type == ValueType::BasicBlockTy;
    }

    void PrintAsm(AsmWriterPtr out) override;
    void PrintName(AsmWriterPtr out) override;
    void PrintUse(AsmWriterPtr out) override;

    static BasicBlockPtr New(FunctionPtr parent = nullptr);

public:
    using instruction_iterator = std::list<InstructionPtr>::iterator;

    int InstructionCount() const {
        return static_cast<int>(_instructions.size());
    }

    // Insert an instruction at the end of the basic block.
    BasicBlockPtr InsertInstruction(InstructionPtr instruction);
    // Insert an instruction before the specified iterator.
    BasicBlockPtr InsertInstruction(instruction_iterator iter,
                                    InstructionPtr inst);
    // Remove an instruction from the basic block.
    BasicBlockPtr RemoveInstruction(InstructionPtr instruction);

    instruction_iterator InstructionBegin() { return _instructions.begin(); }
    instruction_iterator InstructionEnd() { return _instructions.end(); }

private:
    BasicBlock(FunctionPtr parent);

    std::list<InstructionPtr> _instructions;
};