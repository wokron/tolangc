#pragma once

#include "ir/IrForward.h"
#include "ir/value/inst/InstructionTypes.h"

#pragma region AllocaInst

// %1 = alloca i32
class AllocaInst final : public Instruction
{
public:
    ~AllocaInst() override = default;

    static bool classof(const ValueType type) { return type == ValueType::AllocaInstTy; }

    static AllocaInstPtr New(TypePtr type);

    TypePtr AllocatedType() const;

private:
    AllocaInst(TypePtr type);
};

#pragma endregion

#pragma region LoadInst

// %4 = load i32, i32* %2, align 4
class LoadInst final : public UnaryInstruction
{
public:
    ~LoadInst() override = default;

    static bool classof(const ValueType type) { return type == ValueType::LoadInstTy; }

    static LoadInstPtr New(ValuePtr address);

    ValuePtr Address() const;

private:
    LoadInst(TypePtr type, ValuePtr address);
};

#pragma endregion

#pragma region StoreInst

// store i32 0, i32* %1, align 4
// store i32 %4, i32* %3, align 4
class StoreInst final : public BinaryInstruction
{
public:
    ~StoreInst() override = default;

    static bool classof(const ValueType type) { return type == ValueType::StoreInstTy; }

    static StoreInstPtr New(ValuePtr value, ValuePtr address);

private:
    StoreInst(ValuePtr value, ValuePtr address);
};

#pragma endregion

#pragma region ReturnInst

// ret i32 0
// ret
class ReturnInst final : public Instruction
{
public:
    ~ReturnInst() override = default;

    static ReturnInstPtr New(ValuePtr value);
    static ReturnInstPtr New(LlvmContextPtr context);

    static bool classof(const ValueType type) { return type == ValueType::ReturnInstTy; }

    ValuePtr ReturnValue() const;

private:
    ReturnInst(TypePtr type, ValuePtr value);
    ReturnInst(TypePtr type);
};

#pragma endregion

#pragma region CallInst

// %6 = call i32 @add(i32 %4, i32 %5)
// The parameters are the operands of the call instruction.
class CallInst final : public Instruction
{
public:
    static bool classof(const ValueType type) { return type == ValueType::CallInstTy; }

    static CallInstPtr New(FunctionPtr function, const std::vector<ValuePtr>& params);
    static CallInstPtr New(FunctionPtr function);

    FunctionPtr GetFunction() const { return _function; }

private:
    CallInst(FunctionPtr function, const std::vector<ValuePtr>& parameters);
    CallInst(FunctionPtr function);

    FunctionPtr _function;
};

#pragma endregion
