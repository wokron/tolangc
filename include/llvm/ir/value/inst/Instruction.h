#pragma once

#include "llvm/ir/value/User.h"


class Instruction : public User, public HasParent<BasicBlock>
{
public:
    ~Instruction() override = default;

protected:
    Instruction(ValueType valueType, TypePtr type) : User(valueType, type)
    {
    }
};
