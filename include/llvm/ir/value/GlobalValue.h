#pragma once

#include "llvm/ir/value/Constant.h"

/*
 * GlobalValue is a base class for all top-level components in a module.
 */
class GlobalValue : public Constant {
    friend class Module;

public:
    ~GlobalValue() override = default;

    static bool classof(const ValueType type) {
        return ValueType::FunctionTy <= type &&
               type <= ValueType::GlobalVariableTy;
    }

    void PrintName(AsmWriterPtr out) override;

protected:
    GlobalValue(ValueType valueType, TypePtr type, const std::string &name)
        : Constant(valueType, type) {
        SetName(name);
    }
};