#pragma once

#include "llvm/ir/value/Value.h"

class Argument final : public Value, public HasParent<Function> {
public:
    ~Argument() override = default;

    static bool classof(const ValueType type) {
        return type == ValueType::ArgumentTy;
    }

    void PrintAsm(AsmWriterPtr out) override;
    void PrintUse(AsmWriterPtr out) override;

    static ArgumentPtr New(TypePtr type, const std::string &name);

private:
    Argument(TypePtr type, const std::string &name);
};