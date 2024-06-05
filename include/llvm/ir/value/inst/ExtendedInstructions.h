#pragma once

#include "llvm/ir/value/Value.h"
#include "llvm/ir/value/inst/Instruction.h"
#include "llvm/ir/value/inst/InstructionTypes.h"

#pragma region InputInstruction

class InputInst final : public Instruction {
public:
    ~InputInst() override = default;

    static bool classof(const ValueType type) {
        return type == ValueType::InputInstTy;
    }

    void PrintAsm(AsmWriterPtr out) override;

    static InputInstPtr New(LlvmContextPtr context);

private:
    InputInst(TypePtr type);
};

#pragma endregion

#pragma region OutputInstruction

class OutputInst final : public UnaryInstruction {
public:
    ~OutputInst() override = default;

    static OutputInstPtr New(ValuePtr value);

    void PrintAsm(AsmWriterPtr out) override;

    static bool classof(const ValueType type) {
        return type == ValueType::OutputInstTy;
    }

private:
    OutputInst(ValuePtr value);
};

#pragma endregion