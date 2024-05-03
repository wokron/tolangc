#include "llvm/ir/value/Argument.h"
#include "llvm/ir/LlvmContext.h"
#include "llvm/ir/Type.h"


ArgumentPtr Argument::New(TypePtr type, const std::string& name, int argNo)
{
    return type->Context()->SaveValue(new Argument(type, name, argNo));
}


Argument::Argument(TypePtr type, const std::string& name, int argNo)
    : Value(ValueType::ArgumentTy, type, name), _argNo(argNo)
{
}
