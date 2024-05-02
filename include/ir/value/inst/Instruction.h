#pragma once

#include "ir/value/User.h"

class Instruction : public User, public HasParent<BasicBlock>
{
public:
    ~Instruction() override = default;

    bool IsInstruction() const override { return true; }

protected:
    Instruction(ValueType valueType, TypePtr type);
};
