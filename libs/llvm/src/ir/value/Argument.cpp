#include "llvm/ir/value/Argument.h"
#include "llvm/ir/LlvmContext.h"
#include "llvm/ir/Type.h"

ArgumentPtr Argument::New(TypePtr type, const std::string &name) {
    return type->Context()->SaveValue(new Argument(type, name));
}

Argument::Argument(TypePtr type, const std::string &name)
    : Value(ValueType::ArgumentTy, type, name) {}