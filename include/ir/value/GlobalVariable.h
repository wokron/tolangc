#pragma once

#include "ir/value/GlobalValue.h"

class GlobalVariable : public GlobalValue
{
public:
    ~GlobalVariable() override = default;

    static bool classof(const ValueType type) { return type == ValueType::GlobalVariableTy; }

    static GlobalVariablePtr New(TypePtr valueType, const std::string& name);

private:
    GlobalVariable(TypePtr type, const std::string& name);
};
