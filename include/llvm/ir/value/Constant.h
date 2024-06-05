#pragma once

#include "llvm/ir/value/Value.h"

// In LLVM, Constant actually inherits from User, but in our simple
// case, it is totally ok to make it a value with no operands.
class Constant : public Value {
public:
    ~Constant() override = default;

    static bool classof(const ValueType type) {
        return type >= ValueType::ConstantTy;
    }

protected:
    Constant(ValueType valueType, TypePtr type) : Value(valueType, type) {}
};