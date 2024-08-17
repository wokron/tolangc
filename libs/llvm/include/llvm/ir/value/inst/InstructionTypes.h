#pragma once

#include "llvm/ir/value/Value.h"
#include "llvm/ir/value/inst/Instruction.h"

#pragma region UnaryInstruction

class UnaryInstruction : public Instruction {
public:
    ~UnaryInstruction() override = default;

    static bool classof(const ValueType type) {
        return type == ValueType::LoadInstTy ||
               type == ValueType::UnaryOperatorTy;
    }

    ValuePtr Operand() { return OperandAt(0); }

protected:
    UnaryInstruction(ValueType valueType, TypePtr type, ValuePtr operand);
};

#pragma endregion

#pragma region UnaryOperator

enum class UnaryOpType { Not, Neg, Pos };

class UnaryOperator final : public UnaryInstruction {
public:
    ~UnaryOperator() override = default;

    static bool classof(const ValueType type) {
        return type == ValueType::UnaryOperatorTy;
    }

    void PrintAsm(AsmWriterPtr out) override;

    static UnaryOperatorPtr New(UnaryOpType opType, ValuePtr operand);

    UnaryOpType OpType() const { return _opType; }

private:
    UnaryOperator(TypePtr type, ValuePtr operand, UnaryOpType opType);

    UnaryOpType _opType;
};

#pragma endregion

#pragma region BinaryInstruction

class BinaryInstruction : public Instruction {
public:
    ~BinaryInstruction() override = default;

    static bool classof(const ValueType type) {
        return type == ValueType::BinaryOperatorTy ||
               type == ValueType::StoreInstTy;
    }

    ValuePtr LeftOperand() { return OperandAt(0); }
    ValuePtr RightOperand() { return OperandAt(1); }

protected:
    BinaryInstruction(ValueType valueType, TypePtr type, ValuePtr lhs,
                      ValuePtr rhs);
};

#pragma endregion

#pragma region BinaryOperator

enum class BinaryOpType { Add, Sub, Mul, Div, Mod };

class BinaryOperator final : public BinaryInstruction {
public:
    ~BinaryOperator() override = default;

    static bool classof(const ValueType type) {
        return type == ValueType::BinaryOperatorTy;
    }

    void PrintAsm(AsmWriterPtr out) override;

    static BinaryOperatorPtr New(BinaryOpType opType, ValuePtr lhs,
                                 ValuePtr rhs);

    BinaryOpType OpType() const { return _opType; }

private:
    BinaryOperator(TypePtr type, ValuePtr lhs, ValuePtr rhs,
                   BinaryOpType opType)
        : BinaryInstruction(ValueType::BinaryOperatorTy, type, lhs, rhs),
          _opType(opType) {}

    BinaryOpType _opType;
};

#pragma endregion

#pragma region CompareInstruction

enum class CompareOpType {
    Equal,
    NotEqual,
    GreaterThan,
    GreaterThanOrEqual,
    LessThan,
    LessThanOrEqual
};

class CompareInstruction final : public BinaryInstruction {
public:
    ~CompareInstruction() override = default;

    static bool classof(const ValueType type) {
        return type == ValueType::CompareInstTy;
    }

    void PrintAsm(AsmWriterPtr out) override;

    static CompareInstructionPtr New(CompareOpType opType, ValuePtr lhs,
                                     ValuePtr rhs);

    CompareOpType OpType() const { return _opType; }

protected:
    CompareInstruction(TypePtr type, ValuePtr lhs, ValuePtr rhs,
                       CompareOpType opType)
        : BinaryInstruction(ValueType::CompareInstTy, type, lhs, rhs),
          _opType(opType) {}

private:
    CompareOpType _opType;
};

#pragma endregion