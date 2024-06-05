#pragma once

#include "llvm/ir/value/User.h"

class Instruction : public User, public HasParent<BasicBlock> {
public:
    ~Instruction() override = default;

    void PrintName(AsmWriterPtr out) override;
    void PrintUse(AsmWriterPtr out) override;

protected:
    Instruction(ValueType valueType, TypePtr type) : User(valueType, type) {}
};