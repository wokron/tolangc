#include "llvm/ir/value/GlobalVariable.h"
#include "llvm/ir/LlvmContext.h"
#include "llvm/ir/Type.h"
#include "llvm/ir/value/ConstantData.h"

GlobalVariablePtr GlobalVariable::New(TypePtr type, const std::string &name) {
    ConstantDataPtr initializer = nullptr;
    if (type->IsIntegerTy()) {
        initializer = ConstantData::New(type, 0);
    } else if (type->IsFloatTy()) {
        initializer = ConstantData::New(type, 0.0f);
    } else {
        TOLANG_DIE("Invalid type for GlobalVariable");
    }
    return type->Context()->SaveValue(
        new GlobalVariable(PointerType::Get(type), name, initializer));
}