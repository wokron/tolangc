#pragma once

#include "llvm/ir/IrForward.h"
#include "llvm/ir/value/inst/InstructionTypes.h"

#pragma region AllocaInst

// %1 = alloca i32
class AllocaInst final : public Instruction {
public:
    ~AllocaInst() override = default;

    static bool classof(const ValueType type) {
        return type == ValueType::AllocaInstTy;
    }

    void PrintAsm(AsmWriterPtr out) override;

    static AllocaInstPtr New(TypePtr type);

    TypePtr AllocatedType() const;

private:
    AllocaInst(TypePtr type);
};

#pragma endregion

#pragma region LoadInst

// %4 = load i32, i32* %2, align 4
class LoadInst final : public UnaryInstruction {
public:
    ~LoadInst() override = default;

    static bool classof(const ValueType type) {
        return type == ValueType::LoadInstTy;
    }

    void PrintAsm(AsmWriterPtr out) override;

    static LoadInstPtr New(ValuePtr address);

    ValuePtr Address() const;

private:
    LoadInst(TypePtr type, ValuePtr address)
        : UnaryInstruction(ValueType::LoadInstTy, type, address) {}
};

#pragma endregion

#pragma region StoreInst

// store i32 0, i32* %1, align 4
// store i32 %4, i32* %3, align 4
class StoreInst final : public BinaryInstruction {
public:
    ~StoreInst() override = default;

    static bool classof(const ValueType type) {
        return type == ValueType::StoreInstTy;
    }

    void PrintAsm(AsmWriterPtr out) override;

    static StoreInstPtr New(ValuePtr value, ValuePtr address);

private:
    StoreInst(ValuePtr value, ValuePtr address);
};

#pragma endregion

#pragma region BranchInst

class BranchInst final : public Instruction {
public:
    ~BranchInst() override = default;

    static bool classof(const ValueType type) {
        return type == ValueType::BranchInstTy;
    }

    void PrintAsm(AsmWriterPtr out) override;

    static BranchInstPtr New(ValuePtr condition, BasicBlockPtr trueBlock,
                             BasicBlockPtr falseBlock);

    ValuePtr Condition() const { return _condition; }
    BasicBlockPtr TrueBlock() const { return _trueBlock; }
    BasicBlockPtr SetTrueBlock(BasicBlockPtr block);
    BasicBlockPtr FalseBlock() const { return _falseBlock; }
    BasicBlockPtr SetFalseBlock(BasicBlockPtr block);

private:
    BranchInst(ValuePtr condition, BasicBlockPtr trueBlock,
               BasicBlockPtr falseBlock);

    ValuePtr _condition;
    BasicBlockPtr _trueBlock;
    BasicBlockPtr _falseBlock;
};

#pragma endregion

#pragma region JumpInst

class JumpInst final : public Instruction {
public:
    ~JumpInst() override = default;

    static bool classof(const ValueType type) {
        return type == ValueType::JumpInstTy;
    }

    void PrintAsm(AsmWriterPtr out) override;

    static JumpInstPtr New(BasicBlockPtr target);
    static JumpInstPtr New(LlvmContextPtr context);

    BasicBlockPtr Target() const { return _target; }
    BasicBlockPtr SetTarget(BasicBlockPtr block);

private:
    JumpInst(BasicBlockPtr target);
    JumpInst(LlvmContextPtr context);

    BasicBlockPtr _target;
};

#pragma endregion

#pragma region ReturnInst

// ret i32 0
// ret
class ReturnInst final : public Instruction {
public:
    ~ReturnInst() override = default;

    static bool classof(const ValueType type) {
        return type == ValueType::ReturnInstTy;
    }

    void PrintAsm(AsmWriterPtr out) override;

    static ReturnInstPtr New(ValuePtr value);
    static ReturnInstPtr New(LlvmContextPtr context);

    ValuePtr ReturnValue() const;

private:
    ReturnInst(TypePtr type, ValuePtr value);
    ReturnInst(TypePtr type);
};

#pragma endregion

#pragma region CallInst

// %6 = call i32 @add(i32 %4, i32 %5)
// The parameters are the operands of the call instruction.
class CallInst final : public Instruction {
public:
    static bool classof(const ValueType type) {
        return type == ValueType::CallInstTy;
    }

    void PrintAsm(AsmWriterPtr out) override;

    static CallInstPtr New(FunctionPtr function,
                           const std::vector<ValuePtr> &params);
    static CallInstPtr New(FunctionPtr function);

    FunctionPtr GetFunction() const { return _function; }

private:
    CallInst(FunctionPtr function, const std::vector<ValuePtr> &parameters);
    CallInst(FunctionPtr function);

    FunctionPtr _function;
};

#pragma endregion