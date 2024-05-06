#pragma once

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

    int GetValue() const { return _value; }

  private:
    ConstantData(TypePtr type, int value)
        : Constant(ValueType::ConstantDataTy, type), _value(value) {}

    int _value;
};