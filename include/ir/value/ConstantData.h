#pragma once

#include "ir/value/Constant.h"

class ConstantData : public Constant
{
public:
    ~ConstantData() override = default;

    static bool classof(const ValueType type) { return type == ValueType::ConstantDataTy; }

    static ConstantDataPtr New(TypePtr type, int value);

    int GetValue() const { return _value; }

private:
    ConstantData(TypePtr type, int value);

    int _value;
};
