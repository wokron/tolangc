#include "llvm/ir/value/GlobalVariable.h"
#include "llvm/ir/LlvmContext.h"
#include "llvm/ir/Type.h"

GlobalVariablePtr GlobalVariable::New(TypePtr type, const std::string &name) {
    return type->Context()->SaveValue(new GlobalVariable(type, name));
}