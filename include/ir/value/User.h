#pragma once

#include "ir/value/Value.h"


/// <summary>
/// User represent a value that has operands.
/// </summary>
class User : public Value
{
public:
    ~User() override = default;

    static bool classof(ValueType type) { return type >= ValueType::BinaryOperatorTy; }

public:
    void AddOperand(ValuePtr value);
    ValuePtr OperandAt(int index);
    ValuePtr OperandAt(int index) const;
    int OperandCount() const;

protected:
    User(ValueType valueType, TypePtr type) : Value(valueType, type)
    {
    }
};
