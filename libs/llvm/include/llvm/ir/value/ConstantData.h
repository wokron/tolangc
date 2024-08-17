#pragma once

#include "llvm/utils.h"

#include "llvm/ir/value/Constant.h"

class ConstantData : public Constant {
public:
    ~ConstantData() override = default;

    static bool classof(const ValueType type) {
        return type == ValueType::ConstantDataTy;
    }

    void PrintAsm(AsmWriterPtr out) override;
    void PrintName(AsmWriterPtr out) override;

    static ConstantDataPtr New(TypePtr type, int value);
    static ConstantDataPtr New(TypePtr type, float value);

    int GetIntValue() const { return _intValue; }
    float GetFloatValue() const { return _floatValue; }

private:
    ConstantData(TypePtr type, int value)
        : Constant(ValueType::ConstantDataTy, type), _intValue(value) {}

    ConstantData(TypePtr type, float value)
        : Constant(ValueType::ConstantDataTy, type), _floatValue(value) {}

    // Anonymous union.
    union {
        int _intValue;
        float _floatValue;
    };
};