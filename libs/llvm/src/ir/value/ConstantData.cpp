#include "llvm/ir/value/ConstantData.h"
#include "llvm/utils.h"
#include "llvm/ir/LlvmContext.h"
#include "llvm/ir/Type.h"

ConstantDataPtr ConstantData::New(TypePtr type, int value) {
    TOLANG_DIE_IF_NOT(type->IsIntegerTy(),
                      "ConstantData must be of integer type");
    return type->Context()->SaveValue(new ConstantData(type, value));
}

ConstantDataPtr ConstantData::New(TypePtr type, float value) {
    TOLANG_DIE_IF_NOT(type->IsFloatTy(), "ConstantData must be of float type");
    return type->Context()->SaveValue(new ConstantData(type, value));
}