#pragma once

#include "llvm/ir/value/GlobalValue.h"


class GlobalVariable : public GlobalValue
{
public:
    ~GlobalVariable() override = default;

    static bool classof(const ValueType type) { return type == ValueType::GlobalVariableTy; }

    virtual void PrintAsm(AsmWriterPtr out);

    static GlobalVariablePtr New(TypePtr type, const std::string& name);

private:
    GlobalVariable(TypePtr type, const std::string& name)
        : GlobalValue(ValueType::GlobalVariableTy, type, name)
    {
    }
};
