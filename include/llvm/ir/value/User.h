#pragma once

#include "llvm/ir/value/Value.h"

/// <summary>
/// User represent a value that has operands.
/// </summary>
class User : public Value {
public:
    ~User() override = default;

    static bool classof(ValueType type) {
        return type >= ValueType::BinaryOperatorTy;
    }

public:
    void AddOperand(ValuePtr value);
    ValuePtr RemoveOperand(ValuePtr value);
    ValuePtr ReplaceOperand(ValuePtr oldValue, ValuePtr newValue);
    ValuePtr OperandAt(int index);
    ValuePtr OperandAt(int index) const;
    int OperandCount() const;

    use_iterator UseBegin() { return _useList.begin(); }
    use_iterator UseEnd() { return _useList.end(); }
    UseListPtr GetUseList() { return &_useList; }

protected:
    void AddUse(ValuePtr use);
    ValuePtr RemoveUse(ValuePtr use);
    ValuePtr ReplaceUse(ValuePtr oldValue, ValuePtr newValue);

protected:
    User(ValueType valueType, TypePtr type) : Value(valueType, type) {}

    UseList _useList;
};