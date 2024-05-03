#pragma once

#include "llvm/ir/value/Value.h"


class Argument final : public Value, public HasParent<Function>
{
public:
    ~Argument() override = default;

    static bool classof(const ValueType type) { return type == ValueType::ArgumentTy; }

    static ArgumentPtr New(TypePtr type, const std::string& name, int argNo);
    int ArgNo() const { return _argNo; }

private:
    Argument(TypePtr type, const std::string& name, int argNo);

    FunctionPtr _parent;
    int _argNo;
};
