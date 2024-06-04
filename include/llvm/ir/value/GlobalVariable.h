#pragma once

#include "llvm/ir/value/GlobalValue.h"

class GlobalVariable : public GlobalValue {
  public:
    ~GlobalVariable() override = default;

    static bool classof(const ValueType type) {
        return type == ValueType::GlobalVariableTy;
    }

    void PrintAsm(AsmWriterPtr out) override;

    static GlobalVariablePtr New(TypePtr type, const std::string &name);

  private:
    GlobalVariable(TypePtr type, const std::string &name,
                   ConstantDataPtr initializer)
        : GlobalValue(ValueType::GlobalVariableTy, type, name),
          _initializer(initializer) {}

    ConstantDataPtr _initializer;
};