#pragma once

#include "ir/value/inst/Instruction.h"
#include "ir/value/inst/InstructionTypes.h"

#pragma region InputInstruction

class InputInst final : public Instruction
{
public:
    ~InputInst() override = default;

    static bool classof(const ValueType type) { return type == ValueType::InputInstTy; }

    static InputInstPtr New(LlvmContextPtr context);

private:
    InputInst(TypePtr type) : Instruction(ValueType::InputInstTy, type) {}
};

#pragma endregion

#pragma region OutputInstruction

class OutputInst final : public UnaryInstruction
{
public:
    ~OutputInst() override = default;

    static OutputInstPtr New(ValuePtr value);

    static bool classof(const ValueType type) { return type == ValueType::OutputInstTy; }

private:
    OutputInst(ValuePtr value);
};

#pragma endregion